#include "game_mode.h"
#include "devices.h"
#include "script_component.h"

#include "blam_files.h"
#include "caching.h"
#include "cinematic.h"
#include "components.h"
#include "data.h"
#include "model_markers.h"
#include "network/networking.h"
#include "physics.h"
#include "player_controller.h"
#include "script_vm.h"
#include "selected_version.h"
#include "sound_cache.h"
#include "sounds.h"
#include "ticks.h"

#include <blam/volta/blam_actor.h>
#include <blam/volta/blam_antr_sample.h>
#include <blam/volta/blam_recorded_animation.h>
#include <blam/volta/blam_scenario.h>
#include <blam/volta/blam_stl.h>
#include <blam/volta/hsc/blam_bytecode.h>

#include <coffee/core/debug/formatting.h>
#include <coffee/components/proxy.h>
#include <coffee/components/restricted_subsystem.h>
#include <magic_enum/magic_enum.hpp>
#include <peripherals/stl/string/split.h>

#include <map>
#include <set>

#if defined(FEATURE_ENABLE_ImGui)
#include <coffee/imgui/imgui_binding.h>
#endif

using libc_types::f32;
using libc_types::u32;
using libc_types::u8;
using type_safety::empty_list_t;
using type_safety::type_list_t;

using Coffee::cDebug;
using Coffee::cWarning;

using BlamScriptManifest = compo::SubsystemManifest<
    type_list_t<
        PlayerCamera,
        const Attachment,
        const PlayerInfo,
        const PlayerInput,
        const NetworkInfo,
        const CameraLerp,
        AnimationPlayback,
        Model,
        ObjectPhysics,
        BipedStatus,
        DeviceState,
        const BipedEquipment>,
    type_list_t<
        GameEventBus,
        CinematicState,
        const GameState,
        const TickClock,
        DeviceGroups,
        const NetworkState,
        const BlamFiles<halo_version>,
        const SoundCache<halo_version>,
        BSPCache<halo_version>,
        PhysicsBus>,
    type_list_t<comp_app::EventBus<SoundEvent>>>;

/* Script names are lowercased by the compiler; tag names keep their case */
std::string lowered(std::string_view name)
{
    std::string out(name);
    std::transform(out.begin(), out.end(), out.begin(), [](unsigned char c) {
        return static_cast<char>(std::tolower(c));
    });
    return out;
}

/* Runs the scenario's scripts against the world, a step each tick, where the
 * world is decided: offline, or on the server */
template<typename Ver>
struct BlamScript
    : compo::RestrictedSubsystem<BlamScript<Ver>, BlamScriptManifest>
{
    using type       = BlamScript<Ver>;
    using Proxy      = compo::proxy_of<BlamScriptManifest>;
    using time_point = compo::time_point;
    using BC         = typename Ver::bytecode_type;
    using vm_t       = script_vm::vm<BC>;
    using value_t    = script_vm::value_t;
    using node_t     = typename vm_t::node_t;
    using thread_t   = typename vm_t::thread_t;
    using type_t     = blam::hsc::type_t;

    static constexpr u32 max_log_lines = 1024;
    /* Net ids for what scripts place, apart from the server's own */
    static constexpr u32 script_net_id_base = 0xE0000000;
    /* Sound entity ids: loops by tag, impulses counted */
    static constexpr u64 script_sound_base   = 0x5343525000000000ULL;
    static constexpr u64 script_impulse_base = 0x5343524900000000ULL;

    BlamScript()
    {
        this->priority = 2048;
    }

    bool main_thread_only() const override
    {
        return true;
    }

    void load_from(
        blam::map_container<Ver> const& map, blam::scn::scenario<Ver> const* scenario)
    {
        m_map      = &map;
        m_magic    = map.magic;
        m_tags     = blam::tag_index_view(map);
        m_scenario = scenario;
        m_named.clear();
        m_actors.clear();
        m_recordings.clear();
        m_custom.clear();
        m_held.clear();
        m_walk_speeds.clear();
        m_tests = {};
        m_loops.clear();
        m_impulses.clear();
        m_speakers.clear();
        m_riders.clear();
        m_seat_markers.clear();
        m_attached.clear();
        m_hovering.clear();
        m_spawning.clear();
        m_despawning.clear();
        m_recreate.clear();
        m_deferred.clear();
        m_queued_recordings.clear();
        m_log.clear();

        auto strings = scenario->string_segment(map.magic);
        auto globals = scenario->script.globals.data(map.magic);
        if(strings.has_error() || globals.has_error())
        {
            cWarning("Scripts unreadable: no string segment or globals");
            m_scenario = nullptr;
            return;
        }
        m_strings = strings.value();
        index_object_names();
        m_vm.load(
            scenario->bytecode(map.magic),
            m_strings,
            scenario->function_table(map.magic),
            globals.value(),
            [this](BC opcode, node_t const& call, std::vector<value_t>& args, thread_t& self) {
                return native(*m_proxy, opcode, call, args, self);
            });
        m_running = true;
    }

    void start_restricted(Proxy& p, time_point const&)
    {
        auto const ticks = m_ticks->take();
        if(!m_scenario)
            return;
        if constexpr(compile_info::platform::is_32bit)
            return;

#if defined(FEATURE_ENABLE_ImGui)
        if(ImGui::Begin("Scripts"))
            render_tabs();
        ImGui::End();
#endif

        if(p.template subsystem<NetworkState>().replica)
            return;
        if(std::exchange(m_stop_all, false))
            stop_all(p);
        if(!m_running)
            return;
        /* A networked host holds its players back until everyone has loaded,
         * and scripts seat them in their opening cutscenes */
        for(auto player : p.template select<PlayerInfo>())
            if(auto const& info = player.template get<PlayerInfo>();
               !info.spawned && !info.is_remote())
                return;
        m_proxy = &p;
        m_save_queue->poll();
        m_restore_queue->poll();
        if(std::exchange(m_save_now, false))
            save();
        if(std::exchange(m_restore_now, false))
            restore(p);
        watch_player(p);
        for(size_t i = 0; i < ticks.size(); i++)
        {
            settle_despawns(p);
            run_deferred(p);
            std::erase_if(m_spawning, [this, &p](u32 net_id) {
                return live_entity(p, net_id) != 0;
            });
            m_vm.tick();
            play_recordings(p);
            play_custom(p);
            send_walks(p);
            move_mouths(p);
        }
        /* Every frame, so riders and props follow what carries them as drawn */
        draw_recordings(p);
        place_riders(p);
        place_attached(p);
        m_proxy = nullptr;
    }

    void save()
    {
        m_saved = saved_t{
            .vm         = m_vm,
            .actors     = m_actors,
            .next_actor = m_next_actor,
            .recordings = m_recordings,
            .riders     = m_riders,
            .attached   = m_attached,
            .hovering   = m_hovering,
            .custom     = m_custom,
            .speakers   = m_speakers,
            .tests      = m_tests,
            .impulses   = m_impulses,
            .loops      = m_loops,
        };
    }

    /* Back to the checkpoint's threads and globals. What they had going on
     * objects that have since gone is dropped; sounds looping since stop,
     * and the checkpoint's start again */
    void restore(Proxy& p)
    {
        if(!m_saved)
            return;
        auto const there = [&p](u64 id) { return p.template get<Model>(id) != nullptr; };
        for(u32 loop : m_loops)
            if(!m_saved->loops.contains(loop))
                if(auto* sounds = p.template service<comp_app::EventBus<SoundEvent>>())
                {
                    SoundEvent stop{.type = SoundEvent::stop_sound, .entity_id = loop_id({.integer = static_cast<i32>(loop)})};
                    sounds->inject(stop, nullptr);
                }
        for(u32 loop : m_saved->loops)
            if(!m_loops.contains(loop))
                play(p, {.integer = static_cast<i32>(loop)}, {}, true);

        auto saved   = *m_saved;
        m_vm         = std::move(saved.vm);
        m_actors     = std::move(saved.actors);
        m_next_actor = saved.next_actor;
        m_recordings = std::move(saved.recordings);
        m_riders     = std::move(saved.riders);
        m_attached   = std::move(saved.attached);
        m_hovering   = std::move(saved.hovering);
        m_custom     = std::move(saved.custom);
        m_speakers   = std::move(saved.speakers);
        m_tests      = saved.tests;
        m_impulses   = std::move(saved.impulses);
        m_loops      = std::move(saved.loops);
        std::erase_if(m_recordings, [&](recording_t const& r) { return !there(r.entity); });
        std::erase_if(m_riders, [&](rider_t const& r) { return !there(r.unit) || !there(r.vehicle); });
        std::erase_if(m_attached, [&](attached_t const& a) { return !there(a.parent) || !there(a.child); });
        std::erase_if(m_hovering, [&](u64 id) { return !there(id); });
        std::erase_if(m_custom, [&](auto const& c) { return !there(c.first); });
        std::erase_if(m_speakers, [&](auto const& s) { return !there(s.first); });
        m_walking.clear();
        m_deferred.clear();
        m_spawning.clear();
        m_despawning.clear();
        m_recreate.clear();
        m_queued_recordings.clear();
        log("back to the checkpoint");
    }

    /* Every thread ends and the players get their view and controls back */
    void stop_all(Proxy& p)
    {
        for(size_t i = 0; i < m_vm.threads().size(); i++)
            m_vm.stop(i);
        for(auto const& r : std::vector(m_recordings))
            stop_recording(p, r.entity, end_t::stop);
        for(auto const& [id, custom] : std::map(m_custom))
            stop_custom(p, id);
        for_local_seats(p, [&](u32 seat) {
            GameEvent            ev{.type = GameEvent::CinematicCamera};
            CinematicCameraEvent control{.seat_idx = seat, .control = false};
            p.template subsystem<GameEventBus>().inject(ev, &control);
        });
        auto& state     = p.template subsystem<CinematicState>();
        state.fade      = {};
        state.title.reset();
        state.letterbox = false;
        state.playing   = false;
        state.show_hud  = true;
        state.input     = true;
        log("stopped every script");
    }

    /* ---- Names ---- */

    struct placement_t
    {
        ScenarioGroup         group{ScenarioGroup::None};
        u32                   index{0};
        blam::tagref_t const* object{nullptr};
        Vecf3                 position{};
        Vecf3                 rotation{};
    };

    /* Scenario objects by the name scripts use for them */
    void index_object_names()
    {
        auto names = m_scenario->objects.object_names.data(m_magic);
        if(!names.has_value())
            return;
        auto add = [&](auto const& group, ScenarioGroup group_id) {
            for(auto const& placed : blam::scn::placements(group, m_magic))
            {
                auto const& spawn = placed.placement;
                i16 const   name  = spawn.name;
                if(name < 0 || static_cast<size_t>(name) >= names.value().size() || !placed.object)
                    continue;
                m_named[lowered(names.value()[name].name.str())] = {
                    .group    = group_id,
                    .index    = static_cast<u32>(placed.index),
                    .object   = placed.object,
                    .position = spawn.pos,
                    .rotation = spawn.rot,
                };
            }
        };
        auto const& o = m_scenario->objects;
        add(o.scenery, ScenarioGroup::Scenery);
        add(o.light_fixtures, ScenarioGroup::LightFixture);
        add(o.machines, ScenarioGroup::Machine);
        add(o.controls, ScenarioGroup::Control);
        add(o.vehicles, ScenarioGroup::Vehicle);
        add(o.bipeds, ScenarioGroup::Biped);
        add(o.equips, ScenarioGroup::Equipment);
        add(o.weapon_spawns, ScenarioGroup::Weapon);
    }

    std::string_view text_of(value_t const& v) const
    {
        return m_vm.text(v.name);
    }

    u64 entity_by_net_id(Proxy& p, u32 net_id) const
    {
        if(net_id == 0)
            return 0;
        for(auto ent : p.template select<NetworkInfo>())
            if(ent.template get<NetworkInfo>().instance_id == net_id)
                return ent.id();
        return 0;
    }

    u64 named_entity(Proxy& p, std::string const& name) const
    {
        auto it = m_named.find(lowered(name));
        if(it == m_named.end())
            return 0;
        return live_entity(
            p, SpawnObjectEvent::scenario_net_id(it->second.group, it->second.index));
    }

    /* Calls `fn` on each object a value names; a named object still being
     * spawned gets it once it is there */
    template<typename Fn>
    void with_objects(Proxy& p, value_t const& v, Fn&& fn)
    {
        auto const objects = objects_of(p, v);
        for(u64 id : objects)
            fn(p, id);
        if(!objects.empty() && v.pending.empty())
            return;
        std::vector<u32> waiting = v.pending;
        if(auto it = m_named.find(lowered(text_of(v))); it != m_named.end())
            waiting.push_back(
                SpawnObjectEvent::scenario_net_id(it->second.group, it->second.index));
        std::function<void(Proxy&, u64)> later(std::forward<Fn>(fn));
        for(u32 net_id : waiting)
            if(m_spawning.contains(net_id))
                m_deferred.push_back({net_id, later});
    }

    /* What waited on objects now spawned */
    void run_deferred(Proxy& p)
    {
        auto waiting = std::exchange(m_deferred, {});
        for(auto& [net_id, fn] : waiting)
        {
            if(u64 const id = live_entity(p, net_id))
            {
                m_spawning.erase(net_id);
                fn(p, id);
            } else
                m_deferred.emplace_back(net_id, std::move(fn));
        }
    }

    /* Net ids of the objects a value names that are still being spawned */
    std::vector<u32> spawning_of(value_t const& v) const
    {
        std::vector<u32> out;
        for(u32 net_id : v.pending)
            if(m_spawning.contains(net_id))
                out.push_back(net_id);
        if(auto it = m_named.find(lowered(text_of(v))); it != m_named.end())
            if(u32 const net_id = SpawnObjectEvent::scenario_net_id(it->second.group, it->second.index);
               m_spawning.contains(net_id))
                out.push_back(net_id);
        return out;
    }

    /* An object value: a named scenario object, or what a list holds */
    std::vector<u64> objects_of(Proxy& p, value_t const& v) const
    {
        if(!v.objects.empty() || v.name == 0)
            return v.objects;
        u64 const id = named_entity(p, std::string(text_of(v)));
        return id ? std::vector<u64>{id} : std::vector<u64>{};
    }

    template<typename T>
    T const* named(blam::reference<T> const& block, std::string_view name) const
    {
        auto items = block.data(m_magic);
        if(!items.has_value())
            return nullptr;
        for(auto const& item : items.value())
            if(lowered(item.name.str()) == lowered(name))
                return &item;
        return nullptr;
    }

    /* ---- Natives ---- */

    value_t native(
        Proxy& p, BC opcode, node_t const& call, std::vector<value_t>& args, thread_t&)
    {
        using op = BC;
        auto arg = [&args](size_t i) -> value_t const& {
            static value_t const none{};
            return i < args.size() ? args[i] : none;
        };
        auto list = [](std::vector<u64> objects) {
            return value_t{.type = type_t::obj_list, .objects = std::move(objects)};
        };

        switch(opcode)
        {
        case op::print_:
            log(fmt::format("print: {}", text_of(arg(0))));
            return {};

        /* Objects and lists */
        case op::players: {
            std::vector<u64> out;
            for(auto player : p.template select<PlayerInfo, PlayerCamera, NetworkInfo>())
            {
                auto [info, cam, net] = player.components();
                if(biped_shown(info, cam, net))
                    out.push_back(player.id());
            }
            return list(std::move(out));
        }
        case op::list_get: {
            auto const objects = objects_of(p, arg(0));
            auto const at      = static_cast<size_t>(arg(1).as_real());
            value_t    out{.type = call.ret_type};
            if(at < objects.size())
                out.objects.push_back(objects[at]);
            return out;
        }
        case op::list_count:
            return value_t::number(
                type_t::short_, static_cast<f32>(objects_of(p, arg(0)).size()));
        case op::unit:
            return {.type = type_t::unit, .objects = objects_of(p, arg(0))};

        /* Devices: positions and powers, alone or by group */
        case op::device_set_position:
        case op::device_set_position_immediate:
        case op::device_set_power: {
            bool moved = false;
            for(u64 id : objects_of(p, arg(0)))
                if(auto* device = p.template get<DeviceState>(id))
                {
                    f32 const to = std::clamp(arg(1).as_real(), 0.f, 1.f);
                    if(opcode == op::device_set_power)
                        device->power_target = to;
                    else
                        device->position_target = to;
                    if(opcode == op::device_set_position_immediate)
                    {
                        device->position = to;
                        device->snaps++;
                    }
                    moved = true;
                }
            return value_t::boolean(moved);
        }
        case op::device_get_position:
        case op::device_get_power: {
            auto const  ids    = objects_of(p, arg(0));
            auto const* device = ids.empty() ? nullptr : p.template get<DeviceState>(ids.front());
            if(!device)
                return value_t::number(type_t::real_, 0.f);
            return value_t::number(
                type_t::real_, opcode == op::device_get_power ? device->power : device->position);
        }
        case op::device_group_get:
            return value_t::number(
                type_t::real_,
                p.template subsystem<DeviceGroups>().get(static_cast<i16>(arg(0).integer)));
        case op::device_group_set:
        case op::device_group_set_immediate:
            p.template subsystem<DeviceGroups>().set(
                static_cast<i16>(arg(0).integer), std::clamp(arg(1).as_real(), 0.f, 1.f));
            return {};
        case op::device_one_sided_set:
            for(u64 id : objects_of(p, arg(0)))
                if(auto* device = p.template get<DeviceState>(id))
                    device->kind_flags = arg(1).truthy()
                                             ? device->kind_flags | DeviceState::one_sided
                                             : device->kind_flags & ~DeviceState::one_sided;
            return {};
        case op::device_operates_automatically_set:
            for(u64 id : objects_of(p, arg(0)))
                if(auto* device = p.template get<DeviceState>(id))
                    device->automatic = arg(1).truthy();
            return {};
        case op::device_set_never_appears_locked:
            return {};

        case op::object_create:
        case op::object_create_anew:
            object_create(p, std::string(text_of(arg(0))), opcode == op::object_create_anew);
            return {};
        case op::object_destroy:
            for(u64 id : objects_of(p, arg(0)))
                despawn(p, id);
            return {};
        case op::object_create_containing:
        case op::object_create_anew_containing:
        case op::object_destroy_containing:
            for(auto const& [name, placement] : m_named)
            {
                if(name.find(text_of(arg(0))) == std::string::npos)
                    continue;
                if(opcode != op::object_destroy_containing)
                    object_create(p, name, opcode == op::object_create_anew_containing);
                else if(u64 const id = named_entity(p, name))
                    despawn(p, id);
            }
            return {};
        case op::object_teleport:
            with_objects(p, arg(0), [this, flag = std::string(text_of(arg(1)))](Proxy& p, u64 id) {
                teleport(p, id, flag);
            });
            return {};

        case op::object_set_facing:
            with_objects(p, arg(0), [this, flag = std::string(text_of(arg(1)))](Proxy& p, u64 id) {
                face(p, id, flag);
            });
            return {};
        case op::objects_attach:
            with_objects(
                p,
                arg(0),
                [this,
                 children = arg(2),
                 marker   = std::string(text_of(arg(1))),
                 at       = std::string(text_of(arg(3)))](Proxy& p, u64 parent) {
                    with_objects(p, children, [this, parent, marker, at](Proxy& p, u64 child) {
                        attach(p, parent, marker, child, at);
                    });
                });
            return {};
        case op::objects_detach:
            for(u64 parent : objects_of(p, arg(0)))
                for(u64 child : objects_of(p, arg(1)))
                    detach(p, parent, child);
            return {};

        /* Trigger volumes */
        case op::volume_test_object:
        case op::volume_test_objects:
        case op::volume_test_objects_all: {
            auto const* volume  = named(m_scenario->trigger_volumes, text_of(arg(0)));
            auto const  objects = objects_of(p, arg(1));
            if(!volume || objects.empty())
                return value_t::boolean(false);
            bool const all = opcode == op::volume_test_objects_all;
            for(u64 id : objects)
            {
                auto const* model = p.template get<Model>(id);
                bool const  inside =
                    model && volume->contains(model->position + Vecf3{0.f, 0.f, body_middle});
                if(inside != all)
                    return value_t::boolean(inside);
            }
            return value_t::boolean(all);
        }

        /* Cutscenes */
        case op::camera_control:
            for_local_seats(p, [&](u32 seat) {
                GameEvent            ev{.type = GameEvent::CinematicCamera};
                CinematicCameraEvent control{.seat_idx = seat, .control = arg(0).truthy()};
                p.template subsystem<GameEventBus>().inject(ev, &control);
            });
            return {};
        case op::camera_set:
            camera_set(p, text_of(arg(0)), arg(1).as_real(), 0);
            return {};
        case op::camera_set_relative: {
            auto const objects = objects_of(p, arg(2));
            camera_set(p, text_of(arg(0)), arg(1).as_real(), objects.empty() ? 0 : objects.front());
            return {};
        }
        case op::camera_time:
            return value_t::number(type_t::short_, camera_time(p));
        case op::fade_in:
        case op::fade_out: {
            auto& fade = p.template subsystem<CinematicState>().fade;
            bool const out = opcode == op::fade_out;
            fade = {
                .color   = Vecf3(arg(0).as_real(), arg(1).as_real(), arg(2).as_real()),
                .from    = out ? fade.opacity() : 1.f,
                .to      = out ? 1.f : 0.f,
                .seconds = arg(3).as_real() / tick_rate,
            };
            return {};
        }
        case op::cinematic_start:
        case op::cinematic_stop: {
            auto& state     = p.template subsystem<CinematicState>();
            bool const on   = opcode == op::cinematic_start;
            state.letterbox = on;
            state.playing   = on;
            state.input     = !on;
            return {};
        }
        case op::cinematic_show_letterbox:
            p.template subsystem<CinematicState>().letterbox = arg(0).truthy();
            return {};
        case op::cinematic_set_title:
            set_title(p, text_of(arg(0)));
            return {};
        case op::show_hud:
            p.template subsystem<CinematicState>().show_hud = arg(0).truthy();
            return {};
        case op::player_enable_input:
            p.template subsystem<CinematicState>().input = arg(0).truthy();
            return {};

        /* Sounds: an object's, or at the listener without one */
        case op::sound_impulse_start: {
            auto const on = objects_of(p, arg(1));
            play(p, arg(0), on, false);
            m_impulses[static_cast<u32>(arg(0).integer)] = m_vm.now();
            /* A unit says it: its mouth follows the line */
            if(!on.empty() && p.template get<AnimationPlayback>(on.front()))
                m_speakers[on.front()] = {.mouth = mouth_of(p, arg(0)), .from = m_vm.now()};
            return {};
        }
        case op::unit_set_seat:
            with_objects(p, arg(0), [stance = std::string(text_of(arg(1)))](Proxy& p, u64 id) {
                if(auto* anim = p.template get<AnimationPlayback>(id))
                    anim->stance = stance;
            });
            return {};
        /* Health, as fractions of what a unit can have */
        case op::unit_get_health:
        case op::unit_get_shield: {
            auto const  units  = objects_of(p, arg(0));
            auto const* status = units.empty() ? nullptr : p.template get<BipedStatus>(units[0]);
            if(!status)
                return value_t::number(type_t::real_, -1.f);
            return value_t::number(
                type_t::real_, opcode == op::unit_get_health ? status->body : status->shield);
        }
        case op::unit_kill:
        case op::unit_kill_silent:
            with_objects(p, arg(0), [this](Proxy& p, u64 id) {
                if(auto* status = p.template get<BipedStatus>(id))
                    set_vitality(p, id, *status, 0.f, 0.f);
            });
            return {};
        case op::unit_set_current_vitality:
            with_objects(
                p, arg(0), [this, body = arg(1).as_real(), shield = arg(2).as_real()](Proxy& p, u64 id) {
                    if(auto* status = p.template get<BipedStatus>(id))
                        set_vitality(
                            p,
                            id,
                            *status,
                            fraction(body, status->maximum_body()),
                            fraction(shield, status->maximum_shield()));
                });
            return {};
        case op::unit_set_maximum_vitality:
            with_objects(
                p, arg(0), [body = arg(1).as_real(), shield = arg(2).as_real()](Proxy& p, u64 id) {
                    if(auto* status = p.template get<BipedStatus>(id))
                    {
                        status->max_body   = body;
                        status->max_shield = shield;
                    }
                });
            return {};
        case op::object_cannot_take_damage:
        case op::object_can_take_damage:
            with_objects(p, arg(0), [harmless = opcode == op::object_cannot_take_damage](Proxy& p, u64 id) {
                if(auto* status = p.template get<BipedStatus>(id))
                    status->invulnerable = harmless;
            });
            return {};
        case op::unit_set_emotion:
            with_objects(p, arg(0), [emotion = static_cast<i16>(arg(1).as_real())](Proxy& p, u64 id) {
                if(auto* anim = p.template get<AnimationPlayback>(id))
                    anim->emotion = emotion;
            });
            return {};
        case op::sound_impulse_time: {
            /* What is left of it, 0 once played out or never started */
            auto it = m_impulses.find(static_cast<u32>(arg(0).integer));
            if(it == m_impulses.end())
                return value_t::number(type_t::short_, 0.f);
            f32 const left = length_of(p, arg(0)) * tick_rate - static_cast<f32>(m_vm.now() - it->second);
            return value_t::number(type_t::short_, std::max(left, 0.f));
        }
        case op::sound_looping_start:
            /* Already playing: carries on */
            if(!loops(arg(0)) || m_loops.insert(static_cast<u32>(arg(0).integer)).second)
                play(p, arg(0), objects_of(p, arg(1)), true);
            return {};
        case op::sound_looping_stop:
            m_loops.erase(static_cast<u32>(arg(0).integer));
            if(auto* sounds = p.template service<comp_app::EventBus<SoundEvent>>())
            {
                SoundEvent stop{.type = SoundEvent::stop_sound, .entity_id = loop_id(arg(0))};
                sounds->inject(stop, nullptr);
            }
            return {};

        /* Seats */
        case op::unit_enter_vehicle:
            with_objects(
                p,
                arg(1),
                [this, units = arg(0), seat = std::string(text_of(arg(2)))](Proxy& p, u64 vehicle) {
                    with_objects(p, units, [this, vehicle, seat](Proxy& p, u64 unit) {
                        seat_unit(p, unit, vehicle, seat, true);
                    });
                });
            return {};
        case op::vehicle_load_magic:
            with_objects(
                p,
                arg(0),
                [this, units = arg(2), seat = std::string(text_of(arg(1)))](Proxy& p, u64 vehicle) {
                    with_objects(p, units, [this, vehicle, seat](Proxy& p, u64 unit) {
                        seat_unit(p, unit, vehicle, seat, false);
                    });
                });
            return {};
        case op::unit_exit_vehicle:
            with_objects(p, arg(0), [this](Proxy& p, u64 unit) { unseat(p, unit); });
            return {};
        case op::vehicle_unload:
            with_objects(p, arg(0), [this, seat = std::string(text_of(arg(1)))](Proxy& p, u64 vehicle) {
                for(auto const& rider : std::vector(m_riders))
                    if(rider.vehicle == vehicle && rider.seat.find(seat) != std::string::npos)
                        unseat(p, rider.unit);
                for(auto player : p.template select<PlayerInfo>())
                    if(player.template get<PlayerInfo>().riding.vehicle == vehicle)
                        unseat(p, player.id());
            });
            return {};

        /* Recorded control, flown */
        case op::recording_play:
        case op::recording_play_and_hover:
        case op::recording_play_and_delete:
            /* Waiting on its object, a recording still has all of its time */
            if(auto const* recording = named(m_scenario->recorded_animations, text_of(arg(1))))
                for(u32 net_id : spawning_of(arg(0)))
                    m_queued_recordings[net_id] = recording->length_of_animation;
            with_objects(
                p,
                arg(0),
                [this,
                 name = std::string(text_of(arg(1))),
                 end  = opcode == op::recording_play_and_hover    ? end_t::hover
                        : opcode == op::recording_play_and_delete ? end_t::destroy
                                                                  : end_t::stop](Proxy& p, u64 id) {
                    start_recording(p, id, name, end);
                });
            return value_t::boolean(true);
        case op::recording_kill:
            for(u64 id : objects_of(p, arg(0)))
                stop_recording(p, id, end_t::stop);
            return {};
        case op::recording_time: {
            f32 left = 0.f;
            for(u64 id : objects_of(p, arg(0)))
                for(auto const& r : m_recordings)
                    if(r.entity == id)
                        left = std::max(left, static_cast<f32>(r.length - static_cast<i32>(r.play.tick())));
            for(u32 net_id : spawning_of(arg(0)))
                if(auto it = m_queued_recordings.find(net_id); it != m_queued_recordings.end())
                    left = std::max(left, static_cast<f32>(it->second));
            return value_t::number(type_t::short_, left);
        }
        case op::vehicle_hover:
            with_objects(p, arg(0), [this, on = arg(1).truthy()](Proxy& p, u64 id) {
                hover(p, id, on);
            });
            return {};

        /* The tutorial's questions about the first player */
        case op::player_action_test_reset:
            m_tests = {};
            return {};
        case op::player_action_test_look_relative_left:
            return value_t::boolean(m_tests.left > look_test);
        case op::player_action_test_look_relative_right:
            return value_t::boolean(m_tests.right > look_test);
        case op::player_action_test_look_relative_up:
            return value_t::boolean(m_tests.up > look_test);
        case op::player_action_test_look_relative_down:
            return value_t::boolean(m_tests.down > look_test);
        case op::player_action_test_look_relative_all_directions:
            return value_t::boolean(
                std::min({m_tests.left, m_tests.right, m_tests.up, m_tests.down}) > look_test);
        case op::player_action_test_action:
            return value_t::boolean(m_tests.action);
        case op::player_action_test_accept:
            return value_t::boolean(m_tests.accept);
        case op::player_action_test_back:
            return value_t::boolean(m_tests.back);
        case op::player_action_test_primary_trigger:
            return value_t::boolean(m_tests.primary);
        case op::player_action_test_grenade_trigger:
            return value_t::boolean(m_tests.grenade);
        case op::player0_joystick_set_is_normal:
            return value_t::boolean(true);
        case op::player0_look_pitch_is_inverted:
            return value_t::boolean(false);
        case op::objects_can_see_object: {
            auto const  targets = objects_of(p, arg(1));
            auto const* target  = targets.empty() ? nullptr : p.template get<Model>(targets.front());
            return value_t::boolean(
                target && can_see(p, objects_of(p, arg(0)), target->position + Vecf3{0.f, 0.f, body_middle}, arg(2).as_real()));
        }
        case op::objects_can_see_flag: {
            auto const* flag = named(m_scenario->cutscene.flags, text_of(arg(1)));
            return value_t::boolean(flag && can_see(p, objects_of(p, arg(0)), flag->position, arg(2).as_real()));
        }

        /* Animations of the unit's own, or the cutscene's */
        case op::custom_animation:
            with_objects(
                p,
                arg(0),
                [this,
                 graph = graph_of(arg(1)),
                 name  = std::string(text_of(arg(2))),
                 blend = arg(3).truthy()](Proxy& p, u64 id) {
                    custom_animation(p, id, graph, name, blend);
                });
            return value_t::boolean(true);
        case op::unit_open:
        case op::unit_close:
            for(u64 id : objects_of(p, arg(0)))
                unit_door(p, id, opcode == op::unit_open);
            return {};
        case op::unit_stop_custom_animation:
            for(u64 id : objects_of(p, arg(0)))
                stop_custom(p, id);
            return {};
        case op::unit_get_custom_animation_time:
        case op::unit_is_playing_custom_animation: {
            f32 left = 0.f;
            for(u64 id : objects_of(p, arg(0)))
                if(auto it = m_custom.find(id); it != m_custom.end())
                    left = std::max(left, static_cast<f32>(it->second.clip->frame_count - it->second.frame));
            if(opcode == op::unit_is_playing_custom_animation)
                return value_t::boolean(left > 0.f);
            return value_t::number(type_t::short_, left);
        }

        /* The game being played: the shell's difficulty, checkpoints, the
         * level's end; the game mode keeps them */
        case op::game_difficulty_get:
        case op::game_difficulty_get_real:
            return {.type    = type_t::game_difficulty,
                    .integer = p.template subsystem<GameState>().difficulty};
        case op::game_is_cooperative: {
            u32 players = 0;
            for(auto const& player : p.template subsystem<GameState>().players)
                players += player.player_idx != ObjectHitEvent::no_player;
            return value_t::boolean(players > 1);
        }
        case op::game_save:
        case op::game_save_no_timeout:
        case op::game_save_totally_unsafe: {
            GameEvent       ev{.type = GameEvent::Checkpoint};
            CheckpointEvent save{};
            if(opcode == op::game_save_no_timeout)
                save.timeout = std::numeric_limits<f32>::infinity();
            else if(opcode == op::game_save_totally_unsafe)
                save.timeout = 0.f;
            p.template subsystem<GameEventBus>().inject(ev, &save);
            return {};
        }
        case op::game_saving:
            return value_t::boolean(p.template subsystem<GameState>().saving);
        case op::game_safe_to_save:
        case op::game_all_quiet:
            return value_t::boolean(p.template subsystem<GameState>().quiet);
        case op::game_reverted:
            return value_t::boolean(p.template subsystem<GameState>().reverts > 0);
        case op::game_won: {
            GameEvent    ev{.type = GameEvent::GameWon};
            GameWonEvent won{};
            p.template subsystem<GameEventBus>().inject(ev, &won);
            return {};
        }
        case op::game_time:
            return value_t::number(type_t::long_, static_cast<f32>(m_vm.now()));

        /* Structure BSPs */
        case op::switch_bsp: {
            auto&     bsps  = p.template subsystem<BSPCache<halo_version>>();
            i32 const index = static_cast<i32>(arg(0).as_real());
            if(index >= 0 && static_cast<size_t>(index) < bsps.sections.size())
                bsps.active_section = static_cast<i16>(index);
            return {};
        }
        case op::structure_bsp_index:
            return value_t::number(
                type_t::short_, p.template subsystem<BSPCache<halo_version>>().active_section);

        /* Encounters, without minds of their own */
        case op::ai_place:
            ai_place(p, std::string(text_of(arg(0))));
            return {};
        case op::ai_actors: {
            auto out    = list(ai_actors(p, text_of(arg(0))));
            out.pending = ai_spawning(text_of(arg(0)));
            return out;
        }
        case op::ai_living_count:
            return value_t::number(
                type_t::short_, static_cast<f32>(ai_actors(p, text_of(arg(0))).size()));
        case op::ai_kill:
        case op::ai_erase:
            for(u64 id : ai_actors(p, text_of(arg(0))))
                despawn(p, id);
            return {};

        default:
            if(m_unhandled.insert(static_cast<u32>(opcode)).second)
                log(fmt::format("unhandled: {}", blam::hsc::bc::to_string(opcode)));
            return value_t::of(call.ret_type);
        }
    }

    /* ---- World ---- */

    void object_create(Proxy& p, std::string const& name, bool anew)
    {
        auto it = m_named.find(lowered(name));
        if(it == m_named.end())
            return log(fmt::format("object_create: no object named {}", name));
        auto const& placement = it->second;
        u32 const   net_id =
            SpawnObjectEvent::scenario_net_id(placement.group, placement.index);
        if(u64 const existing = live_entity(p, net_id))
        {
            if(!anew)
                return;
            despawn(p, existing);
        }
        log(fmt::format("object_create: {}", name));
        m_spawning.insert(net_id);
        /* Same net id: only once the old one is gone */
        if(m_despawning.contains(net_id))
            m_recreate.insert(net_id);
        else
            spawn(p, placement, net_id);
    }

    void spawn(Proxy& p, placement_t const& placement, u32 net_id)
    {
        GameEvent        ev{.type = GameEvent::SpawnObject};
        SpawnObjectEvent spawn{
            .object       = *placement.object,
            .position     = placement.position,
            .net_id       = net_id,
            .server_owned = true,
        };
        p.template subsystem<GameEventBus>().inject(ev, &spawn);
    }

    void despawn(Proxy& p, u64 id)
    {
        auto const* net = p.template get<NetworkInfo>(id);
        if(!net || net->instance_id == 0)
            return;
        GameEvent          ev{.type = GameEvent::DespawnObject};
        DespawnObjectEvent gone{.net_id = net->instance_id};
        p.template subsystem<GameEventBus>().inject(ev, &gone);
        m_despawning.insert(net->instance_id);
    }

    /* Recreations wait for the objects they replace to be gone */
    void settle_despawns(Proxy& p)
    {
        std::erase_if(m_despawning, [this, &p](u32 net_id) {
            return entity_by_net_id(p, net_id) == 0;
        });
        for(auto it = m_recreate.begin(); it != m_recreate.end();)
        {
            if(m_despawning.contains(*it))
            {
                ++it;
                continue;
            }
            for(auto const& [name, placement] : m_named)
                if(SpawnObjectEvent::scenario_net_id(placement.group, placement.index) == *it)
                    spawn(p, placement, *it);
            it = m_recreate.erase(it);
        }
    }

    /* Its entity, unless a script has destroyed it */
    u64 live_entity(Proxy& p, u32 net_id) const
    {
        return m_despawning.contains(net_id) ? 0 : entity_by_net_id(p, net_id);
    }

    /* Facing a cutscene flag's way, where it stands */
    void face(Proxy& p, u64 id, std::string_view flag_name)
    {
        auto const* flag  = named(m_scenario->cutscene.flags, flag_name);
        auto*       model = p.template get<Model>(id);
        if(!flag || !model)
            return;
        model->rotation = glm::angleAxis(flag->facing.x, Vecf3{0.f, 0.f, 1.f});
        model->update_matrix();
    }

    /* Onto a cutscene flag, facing its way */
    void teleport(Proxy& p, u64 id, std::string_view flag_name)
    {
        auto const* flag = named(m_scenario->cutscene.flags, flag_name);
        if(!flag)
            return log(fmt::format("object_teleport: no flag {}", flag_name));
        Quatf const facing = glm::angleAxis(flag->facing.x, Vecf3{0.f, 0.f, 1.f});
        if(p.template get<PlayerInfo>(id))
        {
            GameEvent           ev{.type = GameEvent::PlayerTeleport};
            /* A player's turn is its camera's */
            PlayerTeleportEvent move{
                .entity_id = id,
                .position  = flag->position,
                .rotation  = camera_look(flag->facing.x, 0.f, 0.f)};
            p.template subsystem<GameEventBus>().inject(ev, &move);
            return;
        }
        auto* model = p.template get<Model>(id);
        if(!model)
            return;
        model->position = flag->position;
        model->rotation = facing;
        model->update_matrix();
        if(auto* physics = p.template get<ObjectPhysics>(id))
            physics->teleported = true;
    }

    blam::tag_t const* sound_tag(value_t const& v) const
    {
        auto tag = m_tags.find(static_cast<u32>(v.integer));
        if(tag == m_tags.end() ||
           !((*tag).matches(blam::tag_class_t::snd) || (*tag).matches(blam::tag_class_t::lsnd)))
            return nullptr;
        return &(*tag);
    }

    bool unspatialized(value_t const& v) const
    {
        using sound_t   = blam::sound::sound;
        auto const* tag = sound_tag(v);
        if(!tag || !tag->matches(blam::tag_class_t::snd))
            return false;
        auto sound = tag->template data<sound_t>(m_magic);
        return sound.has_value() &&
               sound.value()->type == sound_t::scripted_dialog_force_unspatialized;
    }

    /* A looping sound that repeats until stopped, not one played through */
    bool loops(value_t const& v) const
    {
        auto const* tag = sound_tag(v);
        if(!tag || !tag->matches(blam::tag_class_t::lsnd))
            return false;
        auto lsnd = tag->template data<blam::sound::looping_sound>(m_magic);
        return lsnd.has_value() &&
               !(lsnd.value()[0].flags & blam::sound::looping_sound::not_a_loop);
    }

    /* A looping sound is known by its tag, impulses each by their own */
    static u64 loop_id(value_t const& v)
    {
        return script_sound_base | static_cast<u32>(v.integer);
    }

    void play(Proxy& p, value_t const& sound, std::vector<u64> const& on, bool looping)
    {
        auto*       sounds = p.template service<comp_app::EventBus<SoundEvent>>();
        auto const* tag    = sound_tag(sound);
        if(!sounds || !tag)
            return;
        auto const* model = on.empty() || unspatialized(sound) ? nullptr
                                                               : p.template get<Model>(on.front());
        SoundEvent  ev{
             .type      = SoundEvent::play_sound,
             .entity_id = looping ? loop_id(sound) : script_impulse_base | m_next_impulse++,
        };
        PlaySoundEvent play{
            .sound_tag = tag,
            .position  = model ? model->position : Vecf3{},
            .relative  = !model,
            .looping   = looping,
        };
        sounds->inject(ev, &play);
    }

    /* Seconds a sound plays: its first piece, chunk by chunk */
    f32 length_of(Proxy& p, value_t const& v) const
    {
        using sound_t     = blam::sound::sound;
        auto const* tag   = sound_tag(v);
        auto const& index = p.template subsystem<SoundCache<halo_version>>().index;
        if(!tag || !tag->matches(blam::tag_class_t::snd))
            return 0.f;
        auto sound = tag->template data<sound_t>(m_magic);
        if(!sound.has_value())
            return 0.f;
        auto ranges = index.deref(*tag, sound.value()->pitch_ranges_);
        if(!ranges.has_value() || ranges.value().empty())
            return 0.f;
        auto perms = index.deref(*tag, ranges.value()[0].permutations_);
        if(!perms.has_value() || perms.value().empty())
            return 0.f;
        f32 const channels = sound.value()->channels == sound_t::stereo ? 2.f : 1.f;
        f32 const rate     = sound.value()->sample_rate == sound_t::_22kHz ? 22050.f : 44100.f;
        f32       seconds  = 0.f;
        for(i32 i = 0, guard = 0; i >= 0 && static_cast<size_t>(i) < perms.value().size() &&
                                  guard < 256;
            guard++)
        {
            auto const& chunk = perms.value()[i];
            /* Ogg knows its decoded size, ADPCM packs 64 samples in 36 bytes */
            f32 const samples = sound.value()->codec == sound_t::ogg
                                    ? chunk.decoded_size / (2.f * channels)
                                    : chunk.sample_size / (36.f * channels) * 64.f;
            seconds += samples / rate;
            i = chunk.next_permutation_idx;
        }
        return seconds;
    }

    /* Mouth openness of a line, a byte per tick across its chunks */
    std::vector<u8> mouth_of(Proxy& p, value_t const& v) const
    {
        using sound_t     = blam::sound::sound;
        auto const* tag   = sound_tag(v);
        auto const& index = p.template subsystem<SoundCache<halo_version>>().index;
        std::vector<u8> out;
        if(!tag || !tag->matches(blam::tag_class_t::snd))
            return out;
        auto sound = tag->template data<sound_t>(m_magic);
        if(!sound.has_value())
            return out;
        auto ranges = index.deref(*tag, sound.value()->pitch_ranges_);
        if(!ranges.has_value() || ranges.value().empty())
            return out;
        auto perms = index.deref(*tag, ranges.value()[0].permutations_);
        if(!perms.has_value())
            return out;
        for(i32 i = 0, guard = 0; i >= 0 && static_cast<size_t>(i) < perms.value().size() &&
                                  guard < 256;
            guard++)
        {
            auto const& chunk = perms.value()[i];
            if(auto bytes = chunk.mouth_data().data(m_magic); bytes.has_value())
                out.insert(out.end(), bytes.value().begin(), bytes.value().end());
            i = chunk.next_permutation_idx;
        }
        return out;
    }

    /* A tick of each speaker's mouth */
    void move_mouths(Proxy& p)
    {
        for(auto it = m_speakers.begin(); it != m_speakers.end();)
        {
            auto*      anim = p.template get<AnimationPlayback>(it->first);
            auto const at   = static_cast<size_t>(m_vm.now() - it->second.from);
            if(!anim || at >= it->second.mouth.size())
            {
                if(anim)
                    anim->mouth = 0.f;
                it = m_speakers.erase(it);
                continue;
            }
            anim->mouth = it->second.mouth[at] / 255.f;
            ++it;
        }
    }

    /* ---- Seats ---- */

    /* An AI body carried on a seat's marker; players ride for real */
    struct rider_t
    {
        u64         unit;
        u64         vehicle;
        std::string seat;
        Matf4       marker{1};
        i32         node{-1}; /*!< The vehicle node the marker hangs off */
        std::string stance;   /*!< Its own, before it sat */
    };

    /* ---- Health ---- */

    static f32 fraction(f32 points, f32 maximum)
    {
        return maximum > 0.f ? std::clamp(points / maximum, 0.f, 1.f) : 0.f;
    }

    /* Told to everyone, as a hit would be */
    void set_vitality(Proxy& p, u64 id, BipedStatus& status, f32 body, f32 shield)
    {
        status.body   = body;
        status.shield = shield;
        status.dead   = body <= 0.f;
        auto const*       info = p.template get<PlayerInfo>(id);
        GameEvent         ev{.type = GameEvent::UnitVitality};
        UnitVitalityEvent vitality{
            .unit   = id,
            .player = info ? info->player_idx : ObjectHitEvent::no_player,
            .body   = status.body,
            .shield = status.shield,
            .dead   = status.dead,
        };
        p.template subsystem<GameEventBus>().inject(ev, &vitality);
    }

    /* Into the seat labelled `seat`, or the first free one whose label holds
     * it when not `exact` */
    void seat_unit(Proxy& p, u64 unit, u64 vehicle, std::string const& seat, bool exact)
    {
        if(p.template get<PlayerInfo>(unit))
        {
            GameEvent             ev{.type = GameEvent::UnitEnterVehicle};
            UnitEnterVehicleEvent enter{.unit = unit, .vehicle = vehicle};
            if(auto label = blam::bl_string::from(seat))
                enter.seat = *label;
            p.template subsystem<GameEventBus>().inject(ev, &enter);
            return;
        }
        auto const* model = p.template get<Model>(vehicle);
        if(!model || !model->origin_object || !model->tag)
            return;
        auto vehi = model->origin_object->template data<blam::scn::unit>(m_magic);
        if(!vehi.has_value())
            return;
        auto seats = vehi.value()[0].seats.data(m_magic);
        if(!seats.has_value())
            return;
        for(auto const& candidate : seats.value())
        {
            std::string label(candidate.label.str());
            std::transform(label.begin(), label.end(), label.begin(), [](unsigned char c) {
                return static_cast<char>(std::tolower(c));
            });
            bool const fits = exact ? label == seat : label.find(seat) != std::string::npos;
            bool const taken = std::any_of(m_riders.begin(), m_riders.end(), [&](rider_t const& r) {
                return r.vehicle == vehicle && r.seat == label;
            });
            if(!fits || taken)
                continue;
            auto marker = bind_marker_node(*model->tag, candidate.marker_name.str(), m_magic);
            unseat(p, unit);
            /* It animates as the seat's stance, its idle when a custom
             * animation ends */
            auto*       anim = p.template get<AnimationPlayback>(unit);
            std::string own  = anim ? anim->stance : std::string("stand");
            if(anim)
                anim->stance = std::string(candidate.label.str());
            m_riders.push_back({
                .unit    = unit,
                .vehicle = vehicle,
                .seat    = label,
                .marker  = marker ? marker->transform : Matf4(1),
                .node    = marker ? marker->node : -1,
                .stance  = std::move(own),
            });
            carry(p, unit, true);
            return;
        }
    }

    void unseat(Proxy& p, u64 unit)
    {
        if(p.template get<PlayerInfo>(unit))
        {
            GameEvent            ev{.type = GameEvent::UnitExitVehicle};
            UnitExitVehicleEvent exit{.unit = unit};
            p.template subsystem<GameEventBus>().inject(ev, &exit);
            return;
        }
        auto const before = m_riders.size();
        for(auto const& r : m_riders)
            if(r.unit == unit)
                if(auto* anim = p.template get<AnimationPlayback>(unit))
                    anim->stance = r.stance;
        std::erase_if(m_riders, [unit](rider_t const& r) { return r.unit == unit; });
        if(m_riders.size() != before)
            carry(p, unit, false);
    }

    /* Riders sit where their seats are as the vehicle moves */
    void place_riders(Proxy& p)
    {
        for(auto const& rider : m_riders)
        {
            auto const* vehicle = p.template get<Model>(rider.vehicle);
            auto*       unit    = p.template get<Model>(rider.unit);
            if(!vehicle || !unit)
                continue;
            /* Off the marker's node as the vehicle is posed */
            Matf4       at   = vehicle->transform;
            auto const* anim = p.template get<AnimationPlayback>(rider.vehicle);
            if(anim && rider.node >= 0 && static_cast<size_t>(rider.node) < anim->pose.size())
                at = at * anim->pose[rider.node];
            at = at * rider.marker;
            unit->position = Vecf3(at[3]);
            unit->rotation = glm::normalize(Quatf(Matf3(at)));
            unit->update_matrix();
        }
        /* Players the game seated ride a recording as it was drawn too, not
         * as it stood a frame ago */
        for(auto ent : p.template select<PlayerInfo, Model>())
        {
            auto const& info = ent.template get<PlayerInfo>();
            auto const* flight  = flown(info.riding.vehicle);
            auto const* vehicle = flight ? p.template get<Model>(info.riding.vehicle) : nullptr;
            /* Its view too, which the game set off the same stale pose */
            if(auto* cam = vehicle ? p.template get<PlayerCamera>(ent.id()) : nullptr;
               cam && !cam->cinematic)
            {
                Vecf3 const eye = Vecf3(flight->moved * Vecf4(cam->drawn_eye, 1.f));
                cam->camera.position += eye - cam->drawn_eye;
                cam->drawn_eye = eye;
                build_camera_matrix(*cam, eye + cam->view_offset);
                /* And the arms and weapon hanging off it */
                for(auto held : p.template select<Attachment, Model>())
                {
                    auto [attachment, model] = held.components();
                    if(attachment.parent == ent.id())
                        place_first_person(*cam, eye, attachment, model);
                }
            }
            if(auto const* marker = vehicle ? seat_marker(*vehicle, info.riding.seat) : nullptr)
            {
                auto&       body = ent.template get<Model>();
                Matf4 const at   = vehicle->transform * *marker;
                body.position    = Vecf3(at[3]);
                body.rotation    = glm::normalize(Quatf(Matf3(at)));
                body.update_matrix();
            }
        }
    }

    /* A seat's marker on its vehicle, in the bind pose */
    Matf4 const* seat_marker(Model const& vehicle, i16 seat)
    {
        if(!vehicle.tag || !vehicle.origin_object || seat < 0)
            return nullptr;
        auto [it, fresh] = m_seat_markers.try_emplace({vehicle.tag, seat});
        if(fresh)
            if(auto unit = vehicle.origin_object->template data<blam::scn::unit>(m_magic); unit.has_value())
                if(auto seats = unit.value()[0].seats.data(m_magic);
                   seats.has_value() && static_cast<size_t>(seat) < seats.value().size())
                    it->second = bind_marker(*vehicle.tag, seats.value()[seat].marker_name.str(), m_magic);
        return it->second ? &*it->second : nullptr;
    }

    /* ---- The player, as the tutorial watches them ---- */

    struct action_tests_t
    {
        std::optional<Vecf2> from; /* yaw, pitch at the reset */
        f32  left{0.f}, right{0.f}, up{0.f}, down{0.f}; /* furthest turned */
        bool action{false}, accept{false}, back{false}, primary{false}, grenade{false};
    };

    /* Where a unit looks from, and which way */
    std::optional<std::pair<Vecf3, Vecf3>> eye_of(Proxy& p, u64 id) const
    {
        if(auto const* cam = p.template get<PlayerCamera>(id))
            return std::pair{
                cam->camera.position, glm::transpose(Matf3(cam->rotation)) * Vecf3{0.f, 0.f, -1.f}};
        if(auto const* model = p.template get<Model>(id))
            return std::pair{
                model->position + Vecf3{0.f, 0.f, eye_height}, model->rotation * Vecf3{1.f, 0.f, 0.f}};
        return std::nullopt;
    }

    bool can_see(Proxy& p, std::vector<u64> const& viewers, Vecf3 const& target, f32 degrees) const
    {
        for(u64 id : viewers)
            if(auto eye = eye_of(p, id))
            {
                Vecf3 const to = target - eye->first;
                if(glm::length(to) < 1e-4f ||
                   glm::dot(glm::normalize(to), glm::normalize(eye->second)) >=
                       std::cos(glm::radians(degrees)))
                    return true;
            }
        return false;
    }

    /* Every frame, so no press slips between ticks */
    void watch_player(Proxy& p)
    {
        for(auto player : p.template select<PlayerInfo, PlayerInput, PlayerCamera>())
        {
            auto [info, input, cam] = player.components();
            if(info.seat_idx != 0 || info.is_remote())
                continue;
            Vecf3 const f     = glm::transpose(Matf3(cam.rotation)) * Vecf3{0.f, 0.f, -1.f};
            Vecf2 const angle = {std::atan2(f.y, f.x), std::asin(glm::clamp(f.z, -1.f, 1.f))};
            if(!m_tests.from)
                m_tests.from = angle;
            f32 const yaw   = std::remainder(angle.x - m_tests.from->x, glm::two_pi<f32>());
            f32 const pitch = angle.y - m_tests.from->y;
            m_tests.left    = std::max(m_tests.left, yaw);
            m_tests.right   = std::max(m_tests.right, -yaw);
            m_tests.up      = std::max(m_tests.up, pitch);
            m_tests.down    = std::max(m_tests.down, -pitch);
            m_tests.action |= input.intent.use;
            m_tests.accept |= input.accept.current;
            m_tests.back |= input.back.current;
            m_tests.primary |= input.intent.fire;
            m_tests.grenade |= input.intent.grenade || input.grenade.current;
        }
    }

    /* ---- Custom animations ---- */

    struct custom_t
    {
        blam::antr::animation const* clip;
        i32                          frame{0};
        bool                         moves{false}; /* by its root motion */
    };

    blam::antr::header const* graph_of(value_t const& v) const
    {
        auto tag = m_tags.find(static_cast<u32>(v.integer));
        if(tag == m_tags.end() || !(*tag).matches(blam::tag_class_t::antr))
            return nullptr;
        auto header = (*tag).template data<blam::antr::header>(m_magic);
        return header.has_value() ? &header.value()[0] : nullptr;
    }

    void custom_animation(
        Proxy& p, u64 id, blam::antr::header const* graph, std::string const& name, bool blend)
    {
        if(!graph)
            return log(fmt::format("custom_animation: no graph for {}", name));
        auto anims = graph->animations.data(m_magic);
        if(!anims.has_value())
            return;
        for(i32 i = 0; i < static_cast<i32>(anims.value().size()); i++)
        {
            auto const& clip = anims.value()[i];
            if(lowered(clip.name.str()) != lowered(name))
                continue;
            /* Riding a marker, it keeps the clip until told otherwise */
            bool const held = std::ranges::any_of(m_attached, [id](attached_t const& a) { return a.child == id; });
            GameEvent               ev{.type = GameEvent::PlayModelAnimation};
            PlayModelAnimationEvent play{
                .entity    = id,
                .animation = i,
                .graph     = graph,
                .loop      = held,
                .fade      = blend ? .2f : 0.f,
            };
            if(held)
                m_held.insert(id);
            else
                m_held.erase(id);
            p.template subsystem<GameEventBus>().inject(ev, &play);
            bool const moves = clip.frame_info_type_ != blam::antr::frame_info_type::none;
            m_custom[id]     = {.clip = &clip, .moves = moves};
            if(moves && !walks(p, id))
                carry(p, id, true);
            return;
        }
        log(fmt::format("custom_animation: no animation {}", name));
    }

    /* A unit's opening plays out and holds; closing too, or without one,
     * the opening's first frame */
    void unit_door(Proxy& p, u64 id, bool open)
    {
        auto const* anim = p.template get<AnimationPlayback>(id);
        if(!anim || !anim->graph)
            return;
        auto units = anim->graph->units.data(m_magic);
        if(!units.has_value() || units.value().empty())
            return;
        auto slots = units.value()[0].animations.data(m_magic);
        auto clip  = [&](u32 slot) -> i32 {
            return slots.has_value() && slot < slots.value().size() ? slots.value()[slot].animation
                                                                     : -1;
        };
        using Slot     = blam::antr::unit::animation_idx_t;
        i32 const shut = clip(Slot::closing);
        i32 const play = open ? clip(Slot::opening) : shut >= 0 ? shut : clip(Slot::opening);
        if(play < 0)
            return;
        GameEvent               ev{.type = GameEvent::PlayModelAnimation};
        PlayModelAnimationEvent door{
            .entity    = id,
            .animation = play,
            .fade      = 0.f,
            .rate      = open || shut >= 0 ? 1.f : 0.f,
        };
        p.template subsystem<GameEventBus>().inject(ev, &door);
    }

    void stop_custom(Proxy& p, u64 id)
    {
        auto it = m_custom.find(id);
        if(it == m_custom.end())
            return;
        bool const moves = it->second.moves;
        m_custom.erase(it);
        m_held.erase(id);
        GameEvent               ev{.type = GameEvent::PlayModelAnimation};
        PlayModelAnimationEvent stop{.entity = id};
        p.template subsystem<GameEventBus>().inject(ev, &stop);
        if(moves && !playing_recording(id) && !m_hovering.contains(id))
            stand(p, id);
    }

    bool playing_recording(u64 id) const
    {
        return std::any_of(m_recordings.begin(), m_recordings.end(), [id](recording_t const& r) {
            return r.entity == id;
        });
    }

    /* A tick of each: the unit moves as the clip's root does */
    void play_custom(Proxy& p)
    {
        for(auto it = m_custom.begin(); it != m_custom.end();)
        {
            auto* model = p.template get<Model>(it->first);
            auto& state = it->second;
            if(!model || state.frame >= state.clip->frame_count)
            {
                u64 const id = (it++)->first;
                if(model && m_held.contains(id))
                    m_custom.erase(id);
                else if(model)
                    stop_custom(p, id);
                else
                    m_custom.erase(id);
                continue;
            }
            if(state.moves && !riding(it->first))
            {
                auto const [move, yaw] = root_motion(*state.clip, state.frame);
                Vecf3 const step       = model->rotation * move;
                if(walks(p, it->first))
                    walk(p, it->first, step * tick_rate);
                else
                    model->position += step;
                model->rotation = glm::normalize(glm::angleAxis(yaw, Vecf3{0.f, 0.f, 1.f}) * model->rotation);
                model->update_matrix();
            }
            state.frame++;
            ++it;
        }
    }

    /* A frame of the clip's root: a move in the unit's space, and a turn */
    std::pair<Vecf3, f32> root_motion(blam::antr::animation const& clip, i32 frame) const
    {
        auto bytes = clip.frame_info.data(m_magic);
        if(!bytes.has_value())
            return {};
        return blam::antr::root_motion(clip, bytes.value(), frame);
    }

    /* Units on foot keep to the ground: physics walks them */
    static bool walks(Proxy& p, u64 id)
    {
        auto const* physics = p.template get<ObjectPhysics>(id);
        return physics && physics->upright;
    }

    bool riding(u64 id) const
    {
        return std::any_of(m_riders.begin(), m_riders.end(), [id](rider_t const& r) {
            return r.unit == id;
        });
    }

    /* A recording and a clip's root can both move a unit: summed, sent once */
    void walk(Proxy&, u64 id, Vecf3 const& velocity)
    {
        m_walking[id] += velocity;
    }

    void send_walks(Proxy& p)
    {
        for(auto const& [id, velocity] : std::exchange(m_walking, {}))
        {
            Physics::Event    ev{Physics::Event::Velocity};
            Physics::Velocity walking{.entity_id = id, .velocity = velocity, .preserve_z = true};
            p.template subsystem<PhysicsBus>().process(ev, &walking);
        }
    }

    /* Done being moved: a unit stops, anything else is let go */
    void stand(Proxy& p, u64 id)
    {
        if(walks(p, id))
            walk(p, id, {});
        else
            carry(p, id, false);
    }

    /* ---- Attachments ---- */

    struct attached_t
    {
        u64            parent;
        u64            child;
        bound_marker_t marker;   /* on the parent */
        Matf4          at{1.f};  /* the child's own marker, undone */
    };

    void attach(Proxy& p, u64 parent, std::string const& marker, u64 child, std::string const& at)
    {
        auto const* model = p.template get<Model>(parent);
        auto const* held  = p.template get<Model>(child);
        if(!model || !held || !model->tag || !held->tag)
            return;
        auto bound = marker.empty() ? std::optional<bound_marker_t>(bound_marker_t{Matf4(1.f)})
                                    : bind_marker_node(*model->tag, marker, m_magic);
        if(!bound)
            return log(fmt::format("objects_attach: no marker {}", marker));
        auto own = at.empty() ? std::optional<Matf4>(Matf4(1.f)) : bind_marker(*held->tag, at, m_magic);
        detach(p, 0, child);
        m_attached.push_back({
            .parent = parent,
            .child  = child,
            .marker = *bound,
            .at     = glm::inverse(own.value_or(Matf4(1.f))),
        });
        carry(p, child, true);
    }

    /* Every attachment of `child`, or only the one on `parent` */
    void detach(Proxy& p, u64 parent, u64 child)
    {
        auto const gone = std::erase_if(m_attached, [parent, child](attached_t const& a) {
            return a.child == child && (!parent || a.parent == parent);
        });
        if(gone && !playing_recording(child) && !m_hovering.contains(child))
            carry(p, child, false);
        if(gone && m_held.erase(child))
        {
            GameEvent               ev{.type = GameEvent::PlayModelAnimation};
            PlayModelAnimationEvent stop{.entity = child};
            p.template subsystem<GameEventBus>().inject(ev, &stop);
        }
    }

    void place_attached(Proxy& p)
    {
        for(auto const& a : m_attached)
        {
            auto const* parent = p.template get<Model>(a.parent);
            auto*       child  = p.template get<Model>(a.child);
            if(!parent || !child)
                continue;
            Matf4       at   = parent->transform;
            auto const* anim = p.template get<AnimationPlayback>(a.parent);
            auto const  node = static_cast<size_t>(a.marker.node);
            if(anim && a.marker.node >= 0 && node < anim->pose.size())
                at = at * anim->pose[node];
            at              = at * a.marker.transform * a.at;
            child->position = Vecf3(at[3]);
            child->rotation = glm::normalize(Quatf(Matf3(at)));
            child->update_matrix();
        }
    }

    /* ---- Recordings ---- */

    enum class end_t
    {
        stop,
        hover,
        destroy,
    };

    struct recording_t
    {
        u64                             entity;
        blam::scn::recorded::playback   play;
        i32                             length;
        end_t                           end;
        Vecf3                           velocity{};
        /*! A dropship's own flight, in Halo's per-tick units */
        struct flight_t
        {
            Vecf3 velocity{}; /*!< wu/tick */
            f32   speed{0.f}; /*!< What the throttle asks for, wu/tick */
            f32   hover{0.f};
            Vecf2 facing{};   /*!< Yaw, pitch the body points */
            Vecf2 turning{};  /*!< rad/tick */
            f32   roll{0.f};
            f32   rolling{0.f};
            f32   power{0.f}; /*!< The pilot's seat, powering up */
        };
        std::optional<flight_t> flight;
        /*! Flying: the pose before the last tick, to draw between */
        std::optional<std::pair<Vecf3, Quatf>> from;
        /*! As drawn last frame, which the game seated its riders off */
        std::optional<Matf4> drawn;
        /*! From last frame's drawing to this one's */
        Matf4 moved{1.f};
    };

    recording_t const* flown(u64 vehicle) const
    {
        auto it = std::find_if(m_recordings.begin(), m_recordings.end(), [vehicle](auto const& r) {
            return vehicle && r.entity == vehicle && r.drawn;
        });
        return it != m_recordings.end() ? &*it : nullptr;
    }

    void start_recording(Proxy& p, u64 id, std::string_view name, end_t end)
    {
        auto const* recording = named(m_scenario->recorded_animations, name);
        if(!recording)
            return log(fmt::format("recording_play: no recording {}", name));
        auto stream = recording->stream(m_magic);
        if(stream.has_error())
            return log(fmt::format("recording_play: {} unreadable", name));
        stop_recording(p, id, end_t::stop);
        if(auto const* net = p.template get<NetworkInfo>(id))
            m_queued_recordings.erase(net->instance_id);
        m_recordings.push_back({
            .entity = id,
            .play   = blam::scn::recorded::playback(stream.value()),
            .length = recording->length_of_animation,
            .end    = end,
        });
        if(!walks(p, id))
            carry(p, id, true);
    }

    void stop_recording(Proxy& p, u64 id, end_t end)
    {
        std::erase_if(m_recordings, [id](recording_t const& r) { return r.entity == id; });
        switch(end)
        {
        case end_t::destroy:
            despawn(p, id);
            break;
        case end_t::hover:
            break;
        case end_t::stop:
            if(!m_hovering.contains(id))
                stand(p, id);
            break;
        }
    }

    /* Held where it is, or let go to fall */
    void hover(Proxy& p, u64 id, bool on)
    {
        if(on)
            m_hovering.insert(id);
        else
            m_hovering.erase(id);
        bool const playing = std::any_of(
            m_recordings.begin(), m_recordings.end(), [id](recording_t const& r) {
                return r.entity == id;
            });
        carry(p, id, on || playing);
    }

    void carry(Proxy& p, u64 id, bool scripted)
    {
        auto* physics = p.template get<ObjectPhysics>(id);
        if(!physics || physics->scripted == scripted)
            return;
        physics->scripted   = scripted;
        physics->teleported = true;
    }

    /* A tick of each recording: the unit faces where it says and moves at
     * its throttle, within what its tag lets it */
    void play_recordings(Proxy& p)
    {
        constexpr f32 dt = 1.f / tick_rate;
        for(auto it = m_recordings.begin(); it != m_recordings.end();)
        {
            auto* model   = p.template get<Model>(it->entity);
            auto* physics = p.template get<ObjectPhysics>(it->entity);
            if(!model)
            {
                it = m_recordings.erase(it);
                continue;
            }
            using namespace blam::scn::recorded;
            auto const& input  = it->play.state();
            auto const& facing = input.facing();
            f32 const   yaw    = facing.yaw_radians();
            Vecf3 const ahead{std::cos(yaw), std::sin(yaw), 0.f};
            Vecf3 const right{std::sin(yaw), -std::cos(yaw), 0.f};
            auto const* drive = physics && physics->drive ? &*physics->drive : nullptr;
            bool const plane = drive && drive->type == VehicleDrive::human_plane &&
                               physics->mass_points;
            if(plane)
            {
                it->from = std::pair{model->position, model->rotation};
                fly(p, it->entity, *it, *model, *drive, *physics->mass_points, input);
                if(it->play.advance())
                {
                    ++it;
                    continue;
                }
                u64 const   id  = it->entity;
                end_t const end = it->end;
                it              = m_recordings.erase(it);
                if(end == end_t::hover)
                    m_hovering.insert(id);
                stop_recording(p, id, end);
                continue;
            }
            auto const  feet  = drive ? Vecf2{} : walk_speeds(p, it->entity);
            f32 const   top   = drive ? drive->forward_speed : feet.x;
            f32 const   slide = drive ? drive->slide_speed : feet.y;
            f32 const   accel = drive && drive->acceleration > 0.f ? drive->acceleration : default_acceleration;
            Vecf3 const want  = ahead * (input.movement.x * top) +
                               right * (input.movement.y * slide);
            Vecf3 const delta = want - it->velocity;
            f32 const   step  = accel * dt;
            it->velocity += glm::length(delta) > step ? glm::normalize(delta) * step : delta;

            /* Bipeds stay upright and on the ground */
            bool const upright = physics && physics->upright;
            it->from = upright ? std::nullopt : std::optional(std::pair{model->position, model->rotation});
            if(upright)
                walk(p, it->entity, it->velocity);
            else
                model->position += it->velocity * dt;
            model->rotation    = glm::angleAxis(yaw, Vecf3{0.f, 0.f, 1.f}) *
                              glm::angleAxis(upright ? 0.f : -facing.pitch_radians(), Vecf3{0.f, 1.f, 0.f});
            model->update_matrix();

            if(it->play.advance())
            {
                ++it;
                continue;
            }
            u64 const  id  = it->entity;
            end_t const end = it->end;
            it              = m_recordings.erase(it);
            if(end == end_t::hover)
                m_hovering.insert(id);
            stop_recording(p, id, end);
        }
    }

    /* A human plane as Halo flies one: the throttle sets a speed the nose
     * thrusts toward, lift comes from airspeed and a hover that fades as the
     * thrust rises (crouch lowers it, jump raises it and brakes), drag is
     * the mass points' air friction along each body axis, and the body is
     * torqued toward the pilot's facing, so it lags it. Per tick throughout */
    void fly(
        Proxy&                                    p,
        u64                                       id,
        recording_t&                              r,
        Model&                                    model,
        VehicleDrive const&                       drive,
        MassPoints const&                         body,
        blam::scn::recorded::input_state const&   input)
    {
        using namespace blam::scn::recorded;
        auto const& facing = input.facing();
        Vecf2 const want{facing.yaw_radians(), facing.pitch_radians()};
        if(!r.flight)
            r.flight = typename recording_t::flight_t{.facing = want};
        auto& f = *r.flight;
        /* Lift and thrust come up with the pilot's seat; gravity is there
         * from the start */
        f32 const powerup = seat_powerup(model);
        f.power           = powerup > 0.f ? std::min(f.power + 1.f / (powerup * tick_rate), 1.f) : 1.f;

        f32 const top   = drive.forward_speed / tick_rate;
        f32 const back  = drive.reverse_speed / tick_rate;
        f32 const accel = drive.acceleration / (tick_rate * tick_rate);
        f32 const brake = drive.deceleration_or_accel() / (tick_rate * tick_rate);
        bool const jump   = input.held(control_jump);
        bool const crouch = input.held(control_crouch);
        f.speed = seek(f.speed, jump ? 0.f : input.movement.x, top, back, accel, brake);

        f32 const thrust =
            (top > 0.f ? glm::pow(std::clamp(f.speed, 0.f, top) / top, 2.f) : 0.f) * f.power;
        f32 const hold = (crouch ? hover_low : jump ? hover_high : hover_cruise) *
                         (1.f - thrust / std::max(f.power, 1e-3f)) * f.power;
        f.hover += std::clamp(hold - f.hover, -hover_rate, hover_rate);

        /* Turned toward the facing: aiming to close the gap over
         * turn_ticks, as fast as the torque lets the inertia follow */
        f32 const reach = body.radius * body.radius * body.mass * torque_scale;
        Vecf2 const gain{
            body.moments.z > 0.f ? std::min(reach / body.moments.z, 1.f) : 1.f,
            body.moments.y > 0.f ? std::min(reach / body.moments.y, 1.f) : 1.f};
        Vecf2 const gap{
            glm::mod(want.x - f.facing.x + glm::pi<f32>(), glm::two_pi<f32>()) - glm::pi<f32>(),
            want.y - f.facing.y};
        f.turning += (gap / turn_ticks - f.turning) * gain;
        f.facing += f.turning;

        Vecf3 const nose{
            std::cos(f.facing.y) * std::cos(f.facing.x),
            std::cos(f.facing.y) * std::sin(f.facing.x),
            std::sin(f.facing.y)};
        Vecf3 up = Vecf3{-nose.z * nose.x, -nose.z * nose.y, 1.f - nose.z * nose.z};
        up       = glm::length(up) > 1e-4f ? glm::normalize(up) : Vecf3{1.f, 0.f, 0.f};
        /* Banked into a sideways slip, which turns its lift into the turn */
        f32 const slip = top > 0.f ? (f.velocity.y * std::cos(want.x) - f.velocity.x * std::sin(want.x)) *
                                         glm::half_pi<f32>() / top
                                   : 0.f;
        f32 const roll_gain = body.moments.x > 0.f ? std::min(reach / body.moments.x, 1.f) : 1.f;
        f.rolling += ((slip - f.roll) / turn_ticks - f.rolling) * roll_gain;
        f.roll += f.rolling;
        up = up * std::cos(f.roll) + glm::cross(nose, up) * std::sin(f.roll);
        Vecf3 const left = glm::cross(up, nose);

        f32 const airspeed = glm::dot(f.velocity, nose);
        f32 const gravity  = halo_gravity * body.gravity_scale;
        f32 const lift = ((top > 0.f ? std::abs(airspeed / top) : 0.f) * speed_lift +
                          f.hover * hover_lift) *
                         gravity * f.power;
        Vecf3 const drag = air_drag(body);
        f.velocity += nose * ((f.speed - airspeed) * thrust * drive_rate) + up * lift -
                      Vecf3{0.f, 0.f, gravity} -
                      body.air_friction *
                          (nose * (drag.x * airspeed) + left * (drag.y * glm::dot(f.velocity, left)) +
                           up * (drag.z * glm::dot(f.velocity, up)));

        model.position += f.velocity;
        /* Its mass points rest on what is below, not on its own riders */
        Physics::Event   ev{Physics::Event::Raycast};
        Physics::Raycast ray{
            .from   = model.position + Vecf3{0.f, 0.f, ground_probe},
            .to     = model.position - Vecf3{0.f, 0.f, ground_probe},
            .ignore = id,
        };
        for(u32 pass = 0; pass < max_rider_passes; pass++)
        {
            p.template subsystem<PhysicsBus>().process(ev, &ray);
            if(!ray.hit || !rides(p, ray.entity_id, id))
                break;
            ray = {.from = ray.point - Vecf3{0.f, 0.f, rider_skip}, .to = ray.to, .ignore = id};
        }
        if(f32 const floor = ray.point.z + resting_height(body); ray.hit && model.position.z < floor)
        {
            model.position.z = floor;
            f.velocity.z     = std::max(f.velocity.z, 0.f);
        }
        model.rotation = glm::angleAxis(f.facing.x, Vecf3{0.f, 0.f, 1.f}) *
                         glm::angleAxis(-f.facing.y, Vecf3{0.f, 1.f, 0.f}) *
                         glm::angleAxis(f.roll, Vecf3{1.f, 0.f, 0.f});
        model.update_matrix();
        r.velocity = f.velocity * tick_rate;
    }

    /* Seated in `vehicle`, by the script or the game */
    bool rides(Proxy& p, u64 unit, u64 vehicle) const
    {
        if(unit == 0)
            return false;
        for(auto const& r : m_riders)
            if(r.unit == unit && r.vehicle == vehicle)
                return true;
        auto const* info = p.template get<PlayerInfo>(unit);
        return info && info->riding.vehicle == vehicle;
    }

    /* Seconds the driver's seat takes to power up, 0 if it has none */
    f32 seat_powerup(Model const& model) const
    {
        if(!model.origin_object)
            return 0.f;
        auto unit = model.origin_object->template data<blam::scn::unit>(m_magic);
        if(!unit.has_value())
            return 0.f;
        auto seats = unit.value()[0].powered_seats.data(m_magic);
        return seats.has_value() && !seats.value().empty() ? seats.value()[0].driver_powerup_time : 0.f;
    }

    /* How far its origin sits above the ground its lowest mass point
     * touches */
    static f32 resting_height(MassPoints const& body)
    {
        f32 lowest = 0.f;
        for(auto const& point : body.points)
            lowest = std::min(lowest, point.position.z - point.radius);
        return -lowest;
    }

    /* Toward the target speed, as fast as the tag accelerates or brakes,
     * never past the top speed either way */
    static f32 seek(f32 speed, f32 target, f32 top, f32 back, f32 accel, f32 brake)
    {
        if(speed < target)
        {
            speed = speed < -brake ? speed + brake : speed >= 0.f ? speed + accel : (speed / brake + 1.f) * accel;
            return std::min({speed, target, top});
        }
        if(speed > target)
        {
            speed = speed > brake ? speed - brake : speed <= 0.f ? speed - accel : (speed / brake - 1.f) * accel;
            return std::max({speed, target, -back});
        }
        return speed;
    }

    /* Each body axis's share of air friction: point masses drag every way
     * alike, the others scale along and across their own axis */
    static Vecf3 air_drag(MassPoints const& body)
    {
        Matf3 sum(0.f);
        f32   mass = 0.f;
        for(auto const& point : body.points)
        {
            using friction_t = MassPoints::point_t::friction_t;
            Matf3 scale(1.f);
            if(point.friction != friction_t::point)
            {
                Vecf3 axis = point.friction == friction_t::forward ? point.forward
                             : point.friction == friction_t::up   ? point.up
                                                                  : glm::cross(point.up, point.forward);
                axis              = glm::normalize(axis);
                Matf3 const along = glm::outerProduct(axis, axis);
                scale = along * point.parallel + (Matf3(1.f) - along) * point.perpendicular;
            }
            sum += scale * point.mass;
            mass += point.mass;
        }
        return mass > 0.f ? Vecf3{sum[0][0], sum[1][1], sum[2][2]} / mass : Vecf3{1.f};
    }

    /* Recordings step a tick at a time; flying ones are drawn between their
     * last two poses, so they glide at any frame rate */
    void draw_recordings(Proxy& p)
    {
        f32 const alpha = p.template subsystem<TickClock>().alpha();
        for(auto& r : m_recordings)
            if(auto* model = r.from ? p.template get<Model>(r.entity) : nullptr)
            {
                model->transform =
                    glm::translate(Matf4(1), glm::mix(r.from->first, model->position, alpha)) *
                    glm::mat4_cast(glm::slerp(r.from->second, model->rotation, alpha));
                r.moved = r.drawn ? model->transform * glm::inverse(*r.drawn) : Matf4(1.f);
                r.drawn = model->transform;
            }
    }

    /* A unit on foot covers the ground its move clips do */
    Vecf2 walk_speeds(Proxy& p, u64 id)
    {
        using Slot        = blam::antr::unit_weapon::animation_idx_t;
        auto const* anim  = p.template get<AnimationPlayback>(id);
        auto const* graph = anim ? anim->graph : nullptr;
        if(!graph)
            return Vecf2(default_speed);
        /* It strides as it holds its weapon */
        auto const* equip  = p.template get<BipedEquipment>(id);
        auto const* gun    = equip ? equip->held(p.template subsystem<BlamFiles<halo_version>>().load_generation) : nullptr;
        std::string weapon = gun ? std::string(gun->weapon->label.str()) : std::string("unarmed");
        auto [it, fresh]   = m_walk_speeds.try_emplace(std::tuple{graph, anim->stance, weapon});
        if(fresh)
        {
            f32 front = move_speed(*graph, Slot::move_front, m_magic, anim->stance, weapon);
            f32 side  = move_speed(*graph, Slot::move_left, m_magic, anim->stance, weapon);
            if(front <= 0.f)
                front = move_speed(*graph, Slot::move_front, m_magic, "stand", weapon);
            if(side <= 0.f)
                side = move_speed(*graph, Slot::move_left, m_magic, "stand", weapon);
            it->second      = {front > 0.f ? front : default_speed, side > 0.f ? side : default_speed};
        }
        return it->second;
    }

    /* The title's text from the scenario's cutscene title strings */
    void set_title(Proxy& p, std::string_view name)
    {
        auto const* title = named(m_scenario->cutscene.titles, name);
        auto        list  = m_tags.find(m_scenario->ui_text.cutscene_titles);
        if(!title || !m_scenario->ui_text.cutscene_titles.valid() || list == m_tags.end())
            return log(fmt::format("cinematic_set_title: no title {}", name));
        auto strings = (*list).template data<blam::ui::unicode_string_list>(m_magic);
        if(!strings.has_value())
            return;
        auto entries     = strings.value()->data.data(m_magic);
        i32 const string = static_cast<i32>(title->string_index);
        if(!entries.has_value() || string < 0 || static_cast<size_t>(string) >= entries.value().size())
            return;
        auto text = entries.value()[string].str(m_magic);
        if(text.has_error())
            return;
        auto const& c = title->text_color;
        p.template subsystem<CinematicState>().title = CinematicState::title_t{
            .text     = std::u16string(text.value()),
            .bounds   = Vecf4(title->text_bounds),
            .align    = title->justification == blam::scn::cutscene_title::center ? .5f
                        : title->justification == blam::scn::cutscene_title::right ? 1.f
                                                                                   : 0.f,
            .color    = (c.r | c.g | c.b) ? Vecf4(c.r, c.g, c.b, 255.f) / 255.f : Vecf4(1.f),
            .fade_in  = title->fade_time / tick_rate,
            .hold     = title->up_time / tick_rate,
            .fade_out = title->fade_out_time / tick_rate,
        };
    }

    template<typename Fn>
    void for_local_seats(Proxy& p, Fn&& fn)
    {
        for(auto player : p.template select<PlayerInfo>())
            if(!player.template get<PlayerInfo>().is_remote())
                fn(player.template get<PlayerInfo>().seat_idx);
    }

    /* A view along yaw and pitch, rolled about its line of sight, built as
     * the player camera's rotation is */
    static Quatf camera_look(f32 yaw, f32 pitch, f32 roll)
    {
        return glm::normalize(
            glm::angleAxis(roll, Vecf3{0.f, 0.f, -1.f}) *
            glm::angleAxis(pitch, Vecf3{-1.f, 0.f, 0.f}) *
            glm::angleAxis(glm::pi<f32>() - yaw, Vecf3{0.f, 1.f, 0.f}));
    }

    /* To a cutscene camera point over `ticks`, in an object's space if
     * given */
    void camera_set(Proxy& p, std::string_view point_name, f32 ticks, u64 relative_to)
    {
        auto const* point = named(m_scenario->cutscene.camera_points, point_name);
        if(!point)
            return log(fmt::format("camera_set: no camera point {}", point_name));
        Vecf3 const position = point->position;
        /* After the yaw, a tilt about the world's horizontal axes: along
         * the view it is pitch, across it roll */
        f32 const   yaw   = point->rotation.x;
        Vecf2 const tilt{point->rotation.y, point->rotation.z};
        Quatf const rotation = camera_look(
            yaw,
            tilt.x * std::cos(yaw) + tilt.y * std::sin(yaw),
            tilt.y * std::cos(yaw) - tilt.x * std::sin(yaw));
        for_local_seats(p, [&](u32 seat) {
            GameEvent             ev{.type = GameEvent::PlayerCameraLerp};
            PlayerCameraLerpEvent lerp{
                .seat_idx    = seat,
                .position    = position,
                .rotation    = rotation,
                .duration    = std::chrono::milliseconds(static_cast<libc_types::i64>(ticks * 1000.f / tick_rate)),
                .cinematic   = true,
                .fov         = glm::degrees(point->fov),
                .relative_to = relative_to,
            };
            p.template subsystem<GameEventBus>().inject(ev, &lerp);
        });
    }

    /* Ticks left of the cutscene camera's move */
    f32 camera_time(Proxy& p) const
    {
        f32 out = 0.f;
        for(auto camera : p.template select<CameraLerp>())
            for(auto const& lerp : camera.template get<CameraLerp>().lerps)
                if(lerp.cinematic)
                    out = std::max(out, lerp.remaining_time.count() * tick_rate / 1000.f);
        return out;
    }

    /* `encounter` or `encounter/squad`: each squad's actors at its own
     * starting locations */
    void ai_place(Proxy& p, std::string const& path)
    {
        auto const slash = path.find('/');
        auto const name  = std::string_view(path).substr(0, slash);
        auto const which = slash == std::string::npos
                               ? std::string_view{}
                               : std::string_view(path).substr(slash + 1);
        blam::scn::ai::encounter const* encounter = nullptr;
        if(auto list = m_scenario->ai.encounters.data(m_magic); list.has_value())
            for(auto const& e : list.value())
                if(lowered(e.name.str()) == lowered(name))
                    encounter = &e;
        if(!encounter)
            return log(fmt::format("ai_place: no encounter {}", path));
        log(fmt::format("ai_place: {}", path));
        auto squads  = encounter->squads.data(m_magic);
        auto palette = m_scenario->ai.actor_palette.data(m_magic);
        if(!squads.has_value() || !palette.has_value())
            return;
        for(auto const& squad : squads.value())
        {
            if(!which.empty() && lowered(squad.name.str()) != lowered(which))
                continue;
            i16 const type = squad.actor_type;
            if(type < 0 || static_cast<size_t>(type) >= palette.value().size())
                continue;
            auto const* unit = unit_of(palette.value()[type]);
            if(!unit)
                continue;
            blam::tagref_t const held = weapon_of(palette.value()[type]);
            /* Squad starting locations, 28 bytes like the encounter's */
            auto const& block = reinterpret_cast<
                blam::reference<blam::scn::ai::squad_spawn> const&>(
                squad.start_locations);
            auto starts = block.data(m_magic);
            if(!starts.has_value() || starts.value().empty())
                continue;
            u32 const count = std::max<u32>(squad.normal_diff_count, 1);
            for(u32 i = 0; i < count && i < starts.value().size(); i++)
            {
                auto const&      start  = starts.value()[i];
                u32 const        net_id = script_net_id_base | m_next_actor++;
                GameEvent        ev{.type = GameEvent::SpawnObject};
                SpawnObjectEvent spawn{
                    .object       = *unit,
                    .position     = start.position,
                    .rotation     = glm::angleAxis(start.yaw, Vecf3{0.f, 0.f, 1.f}),
                    .net_id       = net_id,
                    .server_owned = true,
                    .held         = held,
                };
                p.template subsystem<GameEventBus>().inject(ev, &spawn);
                m_spawning.insert(net_id);
                /* Both the encounter and its squad name it */
                m_actors[lowered(name)].push_back(net_id);
                m_actors[fmt::format("{}/{}", lowered(name), lowered(squad.name.str()))].push_back(net_id);
            }
        }
    }

    blam::tagref_t const* unit_of(blam::tagref_t const& variant_ref) const
    {
        auto tag = m_tags.find(variant_ref.tag_id);
        if(tag == m_tags.end())
            return nullptr;
        auto variant = (*tag).template data<blam::scn::actor_variant>(m_magic);
        if(!variant.has_value() || !variant.value()->unit.valid())
            return nullptr;
        return &static_cast<blam::tagref_t const&>(variant.value()->unit);
    }

    /* What the variant fights with, if anything */
    blam::tagref_t weapon_of(blam::tagref_t const& variant_ref) const
    {
        auto tag = m_tags.find(variant_ref.tag_id);
        if(tag == m_tags.end())
            return {};
        auto variant = (*tag).template data<blam::scn::actor_variant>(m_magic);
        if(!variant.has_value() || !variant.value()->weapon.valid())
            return {};
        return variant.value()->weapon;
    }

    std::vector<u32> ai_spawning(std::string_view path) const
    {
        std::vector<u32> out;
        if(auto it = m_actors.find(lowered(path)); it != m_actors.end())
            for(u32 net_id : it->second)
                if(m_spawning.contains(net_id))
                    out.push_back(net_id);
        return out;
    }

    std::vector<u64> ai_actors(Proxy& p, std::string_view path) const
    {
        std::vector<u64> out;
        auto             it = m_actors.find(lowered(path));
        if(it == m_actors.end())
            return out;
        for(u32 net_id : it->second)
            if(u64 const id = live_entity(p, net_id))
                out.push_back(id);
        return out;
    }

    void log(std::string const& line)
    {
        cDebug("script: {}", line);
        if(m_log.size() >= max_log_lines)
            m_log.erase(m_log.begin());
        m_log.push_back(line);
    }

#if defined(FEATURE_ENABLE_ImGui)
    void render_tabs()
    {
        if(!ImGui::BeginTabBar("scripting"))
            return;
        if(ImGui::BeginTabItem("Console"))
        {
            if(ImGui::Button(m_running ? "Pause" : "Run"))
                m_running = !m_running;
            ImGui::SameLine();
            if(ImGui::Button("Stop all"))
                m_stop_all = true;
            ImGui::BeginChild("conlog");
            for(auto const& line : m_log)
                ImGui::TextDisabled("%s", line.c_str());
            ImGui::EndChild();
            ImGui::EndTabItem();
        }
        if(ImGui::BeginTabItem("Threads"))
        {
            auto const& threads = m_vm.threads();
            for(size_t i = 0; i < threads.size(); i++)
            {
                auto const& thread = threads[i];
                ImGui::PushID(static_cast<int>(i));
                if(thread.status == script_vm::status_t::dormant)
                {
                    if(ImGui::SmallButton("Wake"))
                        m_vm.wake(i);
                } else if(thread.status != script_vm::status_t::finished)
                {
                    if(ImGui::SmallButton("Stop"))
                        m_vm.stop(i);
                } else
                    ImGui::TextDisabled("    ");
                ImGui::SameLine();
                ImGui::Text(
                    "%-32s %-10s %-9s",
                    thread.script->name.str().data(),
                    blam::hsc::to_string(thread.script->schedule).data(),
                    magic_enum::enum_name(thread.status).data());
                ImGui::PopID();
            }
            ImGui::EndTabItem();
        }
        if(ImGui::BeginTabItem("HaloScript"))
        {
            auto lines = blam::hsc::to_halo_script(*m_scenario, m_magic);
            for(auto const& line :
                stl_types::str::split::spliterator<char>(lines, '\n'))
                ImGui::Text("%.*s", static_cast<int>(line.size()), line.data());
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
#endif

    static constexpr f32 tick_rate = 30.f;
    /* Recordings on something without a drive of its own */
    static constexpr f32 default_speed        = 2.f;  /* wu/s */
    static constexpr f32 default_acceleration = 4.f; /* wu/s^2 */
    /* Human planes: hover lift with crouch, at rest and with jump, its
     * rate, the lift scales, how hard the drive closes on the speed, and
     * the torque toward the facing */
    static constexpr f32 hover_low    = .25f;
    static constexpr f32 hover_cruise = .75f;
    static constexpr f32 hover_high   = 1.f;
    static constexpr f32 hover_rate   = .05f; /* per tick */
    static constexpr f32 hover_lift   = 1.3f;
    static constexpr f32 speed_lift   = 1.05f;
    static constexpr f32 drive_rate   = .05f;
    static constexpr f32 turn_ticks   = 30.f;
    static constexpr f32 torque_scale = .05f;
    static constexpr f32 ground_probe = 2.f; /* wu, either way of the origin */
    static constexpr u32 max_rider_passes = 8;
    static constexpr f32 rider_skip       = .01f; /* wu past a rider to look again */
    static constexpr f32 halo_gravity = .0035651792f; /* wu/tick^2 */
    /* Where a body is tested against volumes, above its feet */
    static constexpr f32 body_middle = .3f;
    static constexpr f32 eye_height  = .6f;
    /* How far a look test wants the view turned, radians */
    static constexpr f32 look_test = .25f;

    blam::map_container<Ver> const* m_map{nullptr};
    blam::tag_index_view<Ver>       m_tags{};
    blam::map_ptr                   m_magic{};
    blam::scn::scenario<Ver> const* m_scenario{nullptr};
    blam::string_segment_ref        m_strings{};

    vm_t                                    m_vm;
    std::map<std::string, placement_t>      m_named;
    std::map<std::string, std::vector<u32>> m_actors;
    u32                                     m_next_actor{1};
    u64                                     m_next_impulse{1};
    std::vector<recording_t>                m_recordings;
    std::vector<rider_t>                    m_riders;
    std::map<std::pair<blam::tag_t const*, i16>, std::optional<Matf4>> m_seat_markers;
    std::vector<attached_t>                 m_attached;
    std::set<u32>                           m_spawning;
    std::vector<std::pair<u32, std::function<void(Proxy&, u64)>>> m_deferred;
    std::set<u64>                           m_hovering;
    std::map<u64, custom_t>                 m_custom;
    std::set<u64>                           m_held; /* attached, looping their custom clip */
    std::map<u64, Vecf3>                    m_walking;
    struct speaker_t
    {
        std::vector<u8> mouth;
        libc_types::i64 from{0};
    };
    std::map<u64, speaker_t> m_speakers;
    std::set<u32>                           m_despawning;
    std::set<u32>                           m_recreate;
    std::map<u32, i32>                      m_queued_recordings; /* spawning net id: its recording's length */
    std::map<std::tuple<blam::antr::header const*, std::string, std::string>, Vecf2> m_walk_speeds;
    action_tests_t                          m_tests;
    std::map<u32, libc_types::i64>          m_impulses; /* tag, tick started */
    std::set<u32>                           m_loops;
    std::set<u32>                           m_unhandled;

    /* What a checkpoint keeps of the scripts: their threads and globals,
     * and what they set going */
    struct saved_t
    {
        vm_t                                    vm;
        std::map<std::string, std::vector<u32>> actors;
        u32                                     next_actor;
        std::vector<recording_t>                recordings;
        std::vector<rider_t>                    riders;
        std::vector<attached_t>                 attached;
        std::set<u64>                           hovering;
        std::map<u64, custom_t>                 custom;
        std::map<u64, speaker_t>                speakers;
        action_tests_t                          tests;
        std::map<u32, libc_types::i64>          impulses;
        std::set<u32>                           loops;
    };
    std::optional<saved_t> m_saved;
    bool                   m_save_now{false};
    bool                   m_restore_now{false};
    std::shared_ptr<GameEventBus::queue_type<WorldSaveEvent>>    m_save_queue;
    std::shared_ptr<GameEventBus::queue_type<WorldRestoreEvent>> m_restore_queue;
    Proxy*                                  m_proxy{nullptr};

    std::vector<std::string>   m_log;
    bool                       m_running{false};
    bool                       m_stop_all{false};
    std::unique_ptr<TickQueue> m_ticks;
};

void alloc_scripting(compo::EntityContainer& e)
{
    e.register_subsystem_inplace<CinematicState>();
    auto& script   = e.register_subsystem_inplace<BlamScript<halo_version>>();
    auto& gbus     = e.subsystem_cast<GameEventBus>();
    script.m_ticks = std::make_unique<TickQueue>(gbus);
    gbus.addEventFunction<MapLoadEvent>(
        0, [&script](auto&, auto*) {
            script.m_scenario = nullptr;
            script.m_saved.reset();
        });
    script.m_save_queue = gbus.addQueuedEventFunction<WorldSaveEvent>(
        0, [&script](GameEvent&, WorldSaveEvent*) { script.m_save_now = true; });
    script.m_restore_queue = gbus.addQueuedEventFunction<WorldRestoreEvent>(
        0, [&script](GameEvent&, WorldRestoreEvent*) { script.m_restore_now = true; });
    gbus.addEventFunction<MapChangedEvent<halo_version>>(
        0, [&script](auto&, MapChangedEvent<halo_version>* changed) {
            script.load_from(changed->container, changed->scenario);
        });
}
