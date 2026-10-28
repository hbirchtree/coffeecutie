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
        Actor,
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
        m_commands.clear();
        m_scaling.clear();
        m_suspended.clear();
        m_walk_speeds.clear();
        m_conversations.clear();
        m_tests = {};
        m_loops.clear();
        m_impulses.clear();
        m_speakers.clear();
        m_riders.clear();
        m_seat_markers.clear();
        m_attached.clear();
        m_hovering.clear();
        m_spawning.clear();
        m_actor_slots.clear();
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
                u64 const id = live_entity(p, net_id);
                if(id)
                    enlist(p, id, net_id);
                return id != 0;
            });
            m_vm.tick();
            play_recordings(p);
            play_custom(p);
            run_commands(p);
            board_vehicles(p);
            run_conversations(p);
            scale_objects(p);
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
        m_commands.clear();
        for(auto& [conv, c] : m_conversations)
            finish_conversation(p, c);
        m_scaling.clear();
        m_suspended.clear();
        m_speakers   = std::move(saved.speakers);
        m_tests      = saved.tests;
        m_impulses   = std::move(saved.impulses);
        m_loops      = std::move(saved.loops);
        std::erase_if(m_recordings, [&](recording_t const& r) { return !there(r.entity); });
        std::erase_if(m_riders, [&](rider_t const& r) {
            bool const gone = !there(r.unit) || !there(r.vehicle);
            if(gone)
                seated(p, r.unit, 0, -1);
            return gone;
        });
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
            /* Spawned since the value was made, e.g. while a deferred call
             * waited on something else */
            else if(u64 const id = live_entity(p, net_id);
                    id && std::find(objects.begin(), objects.end(), id) == objects.end())
                later(p, id);
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
                enlist(p, id, net_id);
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
            if(std::getenv("BLAM_DEBUG_AI"))
                log(fmt::format(
                    "vehicle_load_magic: {} {} units {} pending {} vehicle pending {}",
                    text_of(arg(0)),
                    text_of(arg(1)),
                    arg(2).objects.size(),
                    arg(2).pending.size(),
                    arg(0).pending.size()));
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
        case op::unit_custom_animation_at_frame:
            with_objects(
                p,
                arg(0),
                [this,
                 graph = graph_of(arg(1)),
                 name  = std::string(text_of(arg(2))),
                 blend = arg(3).truthy(),
                 frame = static_cast<i32>(arg(4).as_real())](Proxy& p, u64 id) {
                    custom_animation(p, id, graph, name, blend, frame);
                });
            return value_t::boolean(true);
        /* Suspended, a unit hangs where it is: no physics moves it */
        case op::unit_suspended:
            for(u64 id : objects_of(p, arg(0)))
            {
                if(arg(1).truthy())
                    m_suspended.insert(id);
                else
                    m_suspended.erase(id);
                carry(p, id, arg(1).truthy() || playing_recording(id) || m_hovering.contains(id));
            }
            return {};
        case op::object_set_scale:
            with_objects(
                p,
                arg(0),
                [this, to = static_cast<f32>(arg(1).as_real()), ticks = static_cast<i32>(arg(2).as_real())](
                    Proxy& p, u64 id) {
                    auto const* model = p.template get<Model>(id);
                    m_scaling[id]     = {.from = model ? model->scale : 1.f, .to = to, .ticks = std::max(ticks, 0)};
                });
            return {};
        case op::effect_new:
            if(auto const* flag = named(m_scenario->cutscene.flags, text_of(arg(1))))
            {
                /* A flag faces by yaw and pitch: x10's projector points up */
                f32 const cp = std::cos(flag->facing.y);
                spawn_effect(
                    p,
                    arg(0),
                    flag->position,
                    Vecf3{cp * std::cos(flag->facing.x), cp * std::sin(flag->facing.x), std::sin(flag->facing.y)});
            }
            return {};
        case op::effect_new_on_object_marker:
            for(u64 id : objects_of(p, arg(1)))
                if(auto const* model = p.template get<Model>(id))
                {
                    Matf4 at = model->transform;
                    if(auto const marker = model->tag ? bind_marker_node(*model->tag, text_of(arg(2)), m_magic) : std::nullopt)
                    {
                        auto const* anim = p.template get<AnimationPlayback>(id);
                        auto const  node = static_cast<size_t>(marker->node);
                        if(anim && marker->node >= 0 && node < anim->pose.size())
                            at = at * anim->pose[node];
                        at = at * marker->transform;
                    }
                    spawn_effect(p, arg(0), Vecf3(at[3]), glm::normalize(Vecf3(at[0])));
                }
            return {};
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
        case op::ai_living_fraction:
        case op::ai_strength: {
            auto const it    = m_actors.find(lowered(text_of(arg(0))));
            f32 const  total = it == m_actors.end() ? 0.f : static_cast<f32>(it->second.size());
            f32 const  alive = static_cast<f32>(ai_actors(p, text_of(arg(0))).size());
            return value_t::number(type_t::real_, total > 0.f ? alive / total : 0.f);
        }
        /* Orders to the actors: what the actor system follows */
        case op::ai_attack:
        case op::ai_defend:
        case op::ai_retreat:
            for(u64 id : ai_actors(p, text_of(arg(0))))
                if(auto* actor = p.template get<Actor>(id))
                    actor->order = opcode == op::ai_attack   ? Actor::order_t::attack
                                   : opcode == op::ai_defend ? Actor::order_t::defend
                                                             : Actor::order_t::retreat;
            return {};
        case op::ai_braindead:
        case op::ai_set_blind:
        case op::ai_set_deaf:
        case op::ai_follow_target_players:
            for(u64 id : ai_actors(p, text_of(arg(0))))
                if(auto* actor = p.template get<Actor>(id))
                {
                    bool const on = opcode == op::ai_follow_target_players || arg(1).truthy();
                    if(opcode == op::ai_braindead)
                        actor->braindead = on;
                    else if(opcode == op::ai_set_blind)
                        actor->blind = on;
                    else if(opcode == op::ai_set_deaf)
                        actor->deaf = on;
                    else
                        actor->follow_players = true;
                }
            return {};
        case op::ai_disregard:
            /* The named objects are left alone; today only the target drops */
            for(u64 id : objects_of(p, arg(0)))
                for(auto ent : p.template select<Actor>())
                    if(auto& actor = ent.template get<Actor>(); actor.target == id)
                        actor.target = 0;
            return {};
        case op::ai_magically_see_players:
        case op::ai_magically_see_encounter: {
            std::vector<u64> seen;
            if(opcode == op::ai_magically_see_players)
            {
                for(auto player : p.template select<PlayerInfo, PlayerCamera, NetworkInfo>())
                {
                    auto [info, cam, net] = player.components();
                    if(biped_shown(info, cam, net))
                        seen.push_back(player.id());
                }
            } else
                seen = ai_actors(p, text_of(arg(1)));
            for(u64 id : ai_actors(p, text_of(arg(0))))
            {
                auto* actor = p.template get<Actor>(id);
                auto const* model = p.template get<Model>(id);
                if(!actor || !model || seen.empty())
                    continue;
                u64   nearest = 0;
                f32   best    = std::numeric_limits<f32>::max();
                for(u64 other : seen)
                    if(auto const* target = p.template get<Model>(other))
                        if(f32 const d = glm::distance(target->position, model->position); d < best)
                        {
                            best    = d;
                            nearest = other;
                        }
                if(nearest && nearest != id)
                {
                    actor->target    = nearest;
                    actor->last_seen = p.template get<Model>(nearest)->position;
                    actor->seen_ago  = 0.f;
                    if(actor->state == Actor::state_t::idle)
                        actor->state = Actor::state_t::combat;
                }
            }
            return {};
        }
        case op::ai_migrate: {
            /* The actors join another encounter's squad, named like ai_place */
            auto const path  = std::string(text_of(arg(1)));
            auto const slash = path.find('/');
            auto const ename = lowered(path.substr(0, slash));
            auto const sname = slash == std::string::npos ? std::string{} : lowered(path.substr(slash + 1));
            i16         enc = -1, sq = -1;
            std::string toon;
            if(auto list = m_scenario->ai.encounters.data(m_magic); list.has_value())
                for(size_t i = 0; i < list.value().size() && enc < 0; i++)
                    if(lowered(list.value()[i].name.str()) == ename)
                    {
                        enc = static_cast<i16>(i);
                        if(auto squads = list.value()[i].squads.data(m_magic); squads.has_value())
                        {
                            for(size_t j = 0; j < squads.value().size() && sq < 0; j++)
                                if(sname.empty() || lowered(squads.value()[j].name.str()) == sname)
                                    sq = static_cast<i16>(j);
                            /* A platoon: its first squad */
                            for(size_t j = 0; j < squads.value().size() && sq < 0; j++)
                                if(platoon_name(list.value()[i], squads.value()[j]) == sname)
                                    sq = static_cast<i16>(j);
                            if(sq >= 0)
                                toon = platoon_name(list.value()[i], squads.value()[sq]);
                        }
                    }
            if(enc < 0 || sq < 0)
                return log(fmt::format("ai_migrate: no squad {}", path)), value_t{};
            auto const from = lowered(text_of(arg(0)));
            if(std::getenv("BLAM_DEBUG_AI"))
                log(fmt::format("ai_migrate: {} from {} to {}", ai_actors(p, from).size(), from, path));
            for(u64 id : ai_actors(p, from))
                if(auto* actor = p.template get<Actor>(id))
                {
                    actor->encounter = enc;
                    actor->squad     = sq;
                    actor->position  = -1;
                    actor->order     = Actor::order_t::none;
                }
            /* The names follow them: out of their old encounter and squad,
             * into the new */
            if(auto it = m_actors.find(from); it != m_actors.end())
            {
                std::set<u32> const moved(it->second.begin(), it->second.end());
                for(auto& [name, ids] : m_actors)
                    std::erase_if(ids, [&moved](u32 id) { return moved.contains(id); });
                m_actors[ename].insert(m_actors[ename].end(), moved.begin(), moved.end());
                if(!sname.empty())
                    m_actors[ename + "/" + sname].insert(
                        m_actors[ename + "/" + sname].end(), moved.begin(), moved.end());
                if(!toon.empty() && toon != sname)
                    m_actors[ename + "/" + toon].insert(
                        m_actors[ename + "/" + toon].end(), moved.begin(), moved.end());
            }
            return {};
        }
        case op::ai_maneuver_enable:
        case op::ai_free_units:
        case op::ai_playfight:
            return {};
        /* Command lists: units walking, flying and looking as a list says */
        case op::ai_attach_free:
            return {};
        case op::ai_detach:
            for(u64 id : objects_of(p, arg(0)))
                stop_commands(p, id);
            return {};
        case op::ai_command_list:
        case op::ai_command_list_by_unit: {
            auto const* list = command_list(arg(1));
            if(!list)
                return log(fmt::format("ai_command_list: no list {}", text_of(arg(1)))), value_t{};
            auto const units = opcode == op::ai_command_list ? ai_actors(p, text_of(arg(0))) : objects_of(p, arg(0));
            for(u64 id : units)
                start_commands(p, id, *list);
            return {};
        }
        case op::ai_command_list_advance:
            for(u64 id : ai_actors(p, text_of(arg(0))))
                if(auto it = m_commands.find(id); it != m_commands.end() && it->second.waiting)
                {
                    it->second.waiting = false;
                    it->second.at++;
                }
            return {};
        case op::ai_command_list_status: {
            /* 0 none, 1 finished, 2 at its loop or waiting, 3 running */
            i32 status = 0;
            for(u64 id : objects_of(p, arg(0)))
                if(auto it = m_commands.find(id); it != m_commands.end())
                    status = std::max(status, it->second.done ? 1 : it->second.looped || it->second.waiting ? 2 : 3);
            return value_t::number(type_t::short_, static_cast<f32>(status));
        }

        case op::ai_kill:
        case op::ai_erase:
            for(u64 id : ai_actors(p, text_of(arg(0))))
                despawn(p, id);
            return {};
        /* Vehicles: they run to it and take the seat */
        case op::ai_go_to_vehicle: {
            auto const vehicles = objects_of(p, arg(1));
            if(!vehicles.empty())
                board(p, ai_actors(p, text_of(arg(0))), vehicles.front(), std::string(text_of(arg(2))));
            return {};
        }
        case op::ai_going_to_vehicle: {
            /* How many are still on their way to it */
            auto const vehicles = objects_of(p, arg(0));
            i32        going    = 0;
            for(auto ent : p.template select<Actor>())
                if(auto const& actor = ent.template get<Actor>();
                   actor.board && !vehicles.empty() && actor.board == vehicles.front())
                    going++;
            return value_t::number(type_t::short_, static_cast<f32>(going));
        }
        /* Conversations: lines played in turn by actors of their encounters */
        case op::ai_conversation:
            start_conversation(p, arg(0));
            return {};
        case op::ai_conversation_stop:
            if(auto* c = running_conversation(arg(0)))
                finish_conversation(p, *c);
            return {};
        case op::ai_conversation_advance:
            if(auto* c = running_conversation(arg(0)))
            {
                c->advanced = true;
                c->waiting  = false;
            }
            return {};
        case op::ai_conversation_status: {
            /* 0 none, 2 waiting to be advanced, 3 playing, 5 over */
            auto const* c = running_conversation(arg(0));
            i32 const   status = !c ? 0 : c->done ? 5 : c->waiting ? 2 : 3;
            return value_t::number(type_t::short_, static_cast<f32>(status));
        }
        case op::ai_conversation_line: {
            auto const* c = running_conversation(arg(0));
            return value_t::number(type_t::short_, static_cast<f32>(c ? c->line : -1));
        }

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
            /* Held for it until the game has seated it */
            m_player_claims[{vehicle, lowered(seat)}] = m_vm.now();
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
        for(size_t seat_idx = 0; seat_idx < seats.value().size(); seat_idx++)
        {
            auto const& candidate = seats.value()[seat_idx];
            std::string label(candidate.label.str());
            std::transform(label.begin(), label.end(), label.begin(), [](unsigned char c) {
                return static_cast<char>(std::tolower(c));
            });
            bool const fits = exact ? label == seat : label.find(seat) != std::string::npos;
            bool taken = std::any_of(m_riders.begin(), m_riders.end(), [&](rider_t const& r) {
                return r.vehicle == vehicle && r.seat == label;
            });
            /* A player's seat too, or one a player was just sent to */
            if(auto claim = m_player_claims.find({vehicle, label});
               claim != m_player_claims.end() && m_vm.now() - claim->second < player_claim_ticks)
                taken = true;
            for(auto player : p.template select<PlayerInfo>())
                if(auto const& riding = player.template get<PlayerInfo>().riding;
                   riding.vehicle == vehicle && riding.seat == static_cast<i16>(seat_idx))
                    taken = true;
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
            seated(p, unit, vehicle, static_cast<i16>(seat_idx));
            if(std::getenv("BLAM_DEBUG_AI"))
                log(fmt::format("seat: {} in {} seat {}", unit, vehicle, label));
            return;
        }
        log(fmt::format("seat: no free {} seat for {} in {}", seat, unit, vehicle));
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
        {
            carry(p, unit, false);
            seated(p, unit, 0, -1);
        }
    }

    /* In a vehicle's seat, or out of it with no vehicle */
    static void seated(Proxy& p, u64 unit, u64 vehicle, i16 seat)
    {
        if(auto* physics = p.template get<ObjectPhysics>(unit))
        {
            physics->seated    = vehicle != 0;
            physics->seated_in = vehicle;
            physics->seat      = seat;
        }
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
        Proxy& p, u64 id, blam::antr::header const* graph, std::string const& name, bool blend, i32 frame = 0)
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
                .time      = static_cast<f32>(frame) / tick_rate,
            };
            if(held)
                m_held.insert(id);
            else
                m_held.erase(id);
            p.template subsystem<GameEventBus>().inject(ev, &play);
            bool const moves = clip.frame_info_type_ != blam::antr::frame_info_type::none;
            m_custom[id]     = {.clip = &clip, .frame = frame, .moves = moves};
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
            /* Scaled with its parent, turned as the marker is */
            Matf3 const turn(glm::normalize(Vecf3(at[0])), glm::normalize(Vecf3(at[1])), glm::normalize(Vecf3(at[2])));
            child->position = Vecf3(at[3]);
            child->rotation = glm::normalize(Quatf(turn));
            child->scale    = parent->scale;
            child->update_matrix();
        }
    }

    /* An effe (or snd!) tag the script names, played at a point */
    void spawn_effect(Proxy& p, value_t const& effect, Vecf3 const& at, Vecf3 const& forward)
    {
        auto tag = m_tags.find(static_cast<u32>(effect.integer));
        if(tag == m_tags.end() || !((*tag).matches(blam::tag_class_t::effe) || (*tag).matches(blam::tag_class_t::snd)))
            return log("effect_new: not an effect");
        GameEvent        ev{.type = GameEvent::SpawnEffect};
        SpawnEffectEvent spawn{.effect = (*tag).as_ref(), .position = at, .direction = forward};
        p.template subsystem<GameEventBus>().inject(ev, &spawn);
    }

    /* Scales eased toward what the script set, a tick at a time */
    void scale_objects(Proxy& p)
    {
        for(auto it = m_scaling.begin(); it != m_scaling.end();)
        {
            auto* model = p.template get<Model>(it->first);
            auto& s     = it->second;
            if(!model)
            {
                it = m_scaling.erase(it);
                continue;
            }
            s.at++;
            model->scale = s.at >= s.ticks ? s.to : glm::mix(s.from, s.to, static_cast<f32>(s.at) / static_cast<f32>(s.ticks));
            model->update_matrix();
            it = s.at >= s.ticks ? m_scaling.erase(it) : std::next(it);
        }
    }

    /* ---- Command lists ---- */

    struct commands_t
    {
        blam::scn::ai::command_list const* list{nullptr};
        i32  at{0};       /* the atom it is on */
        i32  ticks{0};    /* spent on it */
        f32  radius{.5f}; /* near enough a point */
        bool looped{false};
        bool waiting{false};
        bool done{false};
    };

    blam::scn::ai::command_list const* command_list(value_t const& v) const
    {
        if(auto const* list = named(m_scenario->ai.command_lists, text_of(v)))
            return list;
        auto lists = m_scenario->ai.command_lists.data(m_magic);
        if(!lists.has_value() || v.integer < 0 || static_cast<size_t>(v.integer) >= lists.value().size())
            return nullptr;
        return &lists.value()[static_cast<size_t>(v.integer)];
    }

    void start_commands(Proxy& p, u64 id, blam::scn::ai::command_list const& list)
    {
        m_commands[id] = {.list = &list};
        if(auto* actor = p.template get<Actor>(id))
        {
            using flags_t   = blam::scn::ai::command_list::flags_t;
            actor->scripted = true;
            actor->release  = false;
            actor->may_fight =
                (list.flags & (flags_t::allow_initiative | flags_t::allow_targeting)) != 0;
        }
        carry(p, id, true);
    }

    void stop_commands(Proxy& p, u64 id)
    {
        if(auto* actor = p.template get<Actor>(id))
            actor->scripted = false;
        if(m_commands.erase(id) && !playing_recording(id) && !m_hovering.contains(id))
            stand(p, id);
    }

    /* ---- Vehicles ---- */

    /* Each free seat whose label holds `seat` is claimed by the nearest
     * of them not already on its way to a vehicle, who runs there and,
     * within its boarding radius, sits */
    void board(Proxy& p, std::vector<u64> const& units, u64 vehicle, std::string const& seat)
    {
        auto const* model = p.template get<Model>(vehicle);
        if(!model || !model->origin_object)
            return;
        auto vehi = model->origin_object->template data<blam::scn::unit>(m_magic);
        if(!vehi.has_value())
            return;
        auto seats = vehi.value()[0].seats.data(m_magic);
        if(!seats.has_value())
            return;
        /* Its hull keeps them off its centre: near enough is its bound */
        f32 const radius = std::max(vehi.value()[0].ai_vehicle_radius, vehi.value()[0].bound_radius) + .5f;
        std::vector<std::pair<f32, Actor*>> idle;
        for(u64 id : units)
            if(auto* actor = p.template get<Actor>(id); actor && !actor->board)
                if(auto const* at = p.template get<Model>(id);
                   at && std::none_of(m_riders.begin(), m_riders.end(), [id](rider_t const& r) { return r.unit == id; }))
                    idle.emplace_back(glm::distance(at->position, model->position), actor);
        std::sort(idle.begin(), idle.end(), [](auto const& a, auto const& b) { return a.first < b.first; });
        std::set<std::string> claimed; /* seats others are on their way to */
        for(auto ent : p.template select<Actor>())
            if(auto const& other = ent.template get<Actor>(); other.board == vehicle)
                claimed.insert(std::string(other.board_seat.str()));
        auto next = idle.begin();
        for(auto const& candidate : seats.value())
        {
            if(next == idle.end())
                break;
            std::string const label = lowered(candidate.label.str());
            bool const        taken =
                label.find(lowered(seat)) == std::string::npos ||
                claimed.contains(label) ||
                std::any_of(m_riders.begin(), m_riders.end(), [&](rider_t const& r) {
                    return r.vehicle == vehicle && r.seat == label;
                });
            if(taken)
                continue;
            next->second->board        = vehicle;
            next->second->board_radius = radius;
            next->second->board_seat   = blam::bl_string::from(label).value_or(blam::bl_string{});
            ++next;
        }
        if(std::getenv("BLAM_DEBUG_AI"))
            log(fmt::format(
                "ai_go_to_vehicle: {} of {} sent to {} seats '{}'", next - idle.begin(), units.size(), vehicle, seat));
    }

    /* Those sent to a vehicle sit once they are at it; a seat already
     * taken is given up on */
    void board_vehicles(Proxy& p)
    {
        for(auto ent : p.template select<Actor, Model>())
        {
            auto [actor, model] = ent.components();
            if(!actor.board)
                continue;
            u64 const   id      = ent.id();
            auto const* vehicle = p.template get<Model>(actor.board);
            bool const  riding  = std::any_of(
                m_riders.begin(), m_riders.end(), [id](rider_t const& r) { return r.unit == id; });
            if(vehicle && !riding &&
               glm::distance(Vecf2(model.position), Vecf2(vehicle->position)) > actor.board_radius)
                continue;
            if(vehicle && !riding)
                seat_unit(p, id, actor.board, std::string(actor.board_seat.str()), true);
            if(std::getenv("BLAM_DEBUG_AI"))
                log(fmt::format(
                    "ai_go_to_vehicle: {} at {} seat {} seated {}",
                    id,
                    actor.board,
                    actor.board_seat.str(),
                    std::any_of(m_riders.begin(), m_riders.end(), [id](rider_t const& r) { return r.unit == id; })));
            actor.board = 0;
        }
    }

    /* ---- Conversations ---- */

    struct conversation_t
    {
        blam::scn::ai::conversation const* conv{nullptr};
        std::vector<u64> participants; /* by index, 0 = nobody: radio, disembodied */
        std::vector<u64> held;         /* actors standing still for it */
        i32              line{-1};
        libc_types::i64  until{0};     /* the tick its line is over */
        bool             waiting{false};
        bool             advanced{false};
        bool             done{false};
    };

    blam::scn::ai::conversation const* conversation_of(value_t const& v) const
    {
        if(auto const* conv = named(m_scenario->ai.conversations, text_of(v)))
            return conv;
        auto list = m_scenario->ai.conversations.data(m_magic);
        if(!list.has_value() || v.integer < 0 || static_cast<size_t>(v.integer) >= list.value().size())
            return nullptr;
        return &list.value()[static_cast<size_t>(v.integer)];
    }

    conversation_t* running_conversation(value_t const& v)
    {
        auto const* conv = conversation_of(v);
        auto        it   = conv ? m_conversations.find(conv) : m_conversations.end();
        return it == m_conversations.end() ? nullptr : &it->second;
    }

    /* The actr type of an actor, by its squad's palette entry */
    std::optional<blam::scn::actor_type_t> actor_type_of(Proxy& p, u64 id) const
    {
        auto const* actor      = p.template get<Actor>(id);
        auto        encounters = m_scenario->ai.encounters.data(m_magic);
        auto        palette    = m_scenario->ai.actor_palette.data(m_magic);
        if(!actor || !actor->active() || !encounters.has_value() || !palette.has_value() ||
           static_cast<size_t>(actor->encounter) >= encounters.value().size())
            return std::nullopt;
        auto squads = encounters.value()[actor->encounter].squads.data(m_magic);
        if(!squads.has_value() || actor->squad < 0 ||
           static_cast<size_t>(actor->squad) >= squads.value().size())
            return std::nullopt;
        i16 const type = actor->variant >= 0 ? actor->variant : squads.value()[actor->squad].actor_type;
        if(type < 0 || static_cast<size_t>(type) >= palette.value().size())
            return std::nullopt;
        auto variant = m_tags.find(palette.value()[type].tag_id);
        if(variant == m_tags.end())
            return std::nullopt;
        auto actv = (*variant).template data<blam::scn::actor_variant>(m_magic);
        if(!actv.has_value())
            return std::nullopt;
        auto definition = m_tags.find(actv.value()->actor_definition);
        if(definition == m_tags.end())
            return std::nullopt;
        auto actr = (*definition).template data<blam::scn::actor>(m_magic);
        if(!actr.has_value())
            return std::nullopt;
        return actr.value()->type;
    }

    /* Who may speak a part: its encounter's actors, or without one the
     * player's allies nearest the player, in or out of their vehicle as
     * the part asks */
    std::vector<u64> speakers_for(Proxy& p, blam::scn::ai::conversation_participant const& part)
    {
        using part_t = blam::scn::ai::conversation_participant;
        using team_t = blam::scn::unit::team_t;
        if(!part.encounter_name.str().empty())
            return ai_actors(p, lowered(part.encounter_name.str()));
        auto const player  = player_near(p, 0);
        u64        vehicle = 0;
        for(auto ent : p.template select<PlayerInfo>())
            if(!vehicle)
                vehicle = ent.template get<PlayerInfo>().riding.vehicle;
        auto riding = [this](u64 id) -> u64 {
            for(auto const& r : m_riders)
                if(r.unit == id)
                    return r.vehicle;
            return 0;
        };
        std::vector<std::pair<f32, u64>> found;
        for(auto ent : p.template select<Actor, Model>())
        {
            auto const& actor = ent.template get<Actor>();
            auto const  team  = static_cast<team_t>(actor.team);
            if(!actor.active())
                continue;
            if(part.selection_type != part_t::any_actor && team != team_t::player && team != team_t::human)
                continue;
            if(part.selection_type == part_t::in_players_vehicle && (!vehicle || riding(ent.id()) != vehicle))
                continue;
            if(part.selection_type == part_t::not_in_a_vehicle && riding(ent.id()))
                continue;
            f32 const d = player ? glm::distance(*player, ent.template get<Model>().position) : 0.f;
            found.emplace_back(d, ent.id());
        }
        std::sort(found.begin(), found.end());
        std::vector<u64> out;
        for(auto const& [d, id] : found)
            out.push_back(id);
        return out;
    }

    /* Its participants are gathered from their encounters, and its lines
     * then play one after another, each for its sound and delay */
    void start_conversation(Proxy& p, value_t const& v)
    {
        using part_t = blam::scn::ai::conversation_participant;
        using conv_t = blam::scn::ai::conversation;
        auto const* conv = conversation_of(v);
        if(!conv)
            return log(fmt::format("ai_conversation: no conversation {}", text_of(v)));
        if(auto it = m_conversations.find(conv); it != m_conversations.end())
            finish_conversation(p, it->second);
        conversation_t c{.conv = conv};
        auto           parts = conv->participants.data(m_magic);
        auto           names = m_scenario->objects.object_names.data(m_magic);
        std::set<u64>  taken;
        if(std::getenv("BLAM_DEBUG_AI") && parts.has_value())
        {
            for(auto const& part : parts.value())
                log(fmt::format(
                    "  part flags {} selection {} type {} object {} encounter '{}'",
                    static_cast<int>(part.flags),
                    static_cast<int>(part.selection_type),
                    static_cast<int>(part.actor_type),
                    part.use_this_object,
                    part.encounter_name.str()));
            if(auto lines = conv->lines.data(m_magic); lines.has_value())
                for(auto const& line : lines.value())
                    log(fmt::format(
                        "  line flags {} participant {} addressee {}/{} delay {}",
                        static_cast<int>(line.flags),
                        line.participant,
                        line.addressee,
                        line.addressee_participant,
                        line.line_delay_time));
        }
        if(parts.has_value())
            for(auto const& part : parts.value())
            {
                u64        unit     = 0;
                bool const bodiless = part.selection_type == part_t::disembodied ||
                                      part.selection_type == part_t::radio_unit ||
                                      part.selection_type == part_t::radio_sergeant;
                if(part.use_this_object >= 0 && names.has_value() &&
                   static_cast<size_t>(part.use_this_object) < names.value().size())
                    unit = named_entity(
                        p, lowered(names.value()[static_cast<size_t>(part.use_this_object)].name.str()));
                else if(!bodiless)
                    for(u64 id : speakers_for(p, part))
                    {
                        auto const* status = p.template get<BipedStatus>(id);
                        if(taken.contains(id) || !status || status->dead)
                            continue;
                        auto const type = static_cast<blam::scn::actor_type_t>(part.actor_type);
                        if(type != blam::scn::actor_type_t::none && actor_type_of(p, id) != type)
                            continue;
                        unit = id;
                        break;
                    }
                bool const optional = (part.flags & (part_t::is_optional | part_t::is_alternative)) != 0;
                if(!unit && !bodiless && !optional)
                {
                    log(fmt::format(
                        "ai_conversation: {} has nobody for {} (selection {})",
                        conv->name.str(),
                        part.encounter_name.str(),
                        static_cast<int>(part.selection_type)));
                    c.done = true;
                }
                taken.insert(unit);
                c.participants.push_back(unit);
            }
        log(fmt::format("ai_conversation: {}{}", conv->name.str(), c.done ? " (nobody)" : ""));
        /* They hold still for it when it says so, unless a list has them */
        if(!c.done && (conv->flags & conv_t::stop_other_actions))
            for(u64 id : c.participants)
                if(auto* actor = id ? p.template get<Actor>(id) : nullptr;
                   actor && !actor->scripted && !m_commands.contains(id))
                {
                    actor->scripted = true;
                    actor->moving   = false;
                    walk(p, id, {});
                    c.held.push_back(id);
                }
        m_conversations[conv] = std::move(c);
    }

    void finish_conversation(Proxy& p, conversation_t& c)
    {
        c.done = true;
        for(u64 id : c.held)
            if(auto* actor = p.template get<Actor>(id); actor && !m_commands.contains(id))
                actor->scripted = false;
        c.held.clear();
    }

    /* A tick of each conversation: the next line once the last is over,
     * cut short by a death or a fight when its flags say so */
    void run_conversations(Proxy& p)
    {
        using conv_t = blam::scn::ai::conversation;
        using line_t = blam::scn::ai::conversation_line;
        for(auto& [conv, c] : m_conversations)
        {
            if(c.done)
                continue;
            auto lines = conv->lines.data(m_magic);
            if(!lines.has_value())
            {
                finish_conversation(p, c);
                continue;
            }
            for(u64 id : c.participants)
            {
                auto const* status = id ? p.template get<BipedStatus>(id) : nullptr;
                auto const* actor  = id ? p.template get<Actor>(id) : nullptr;
                if((conv->flags & conv_t::stop_if_death) && status && status->dead)
                    c.done = true;
                if((conv->flags & (conv_t::stop_if_visible_enemy | conv_t::stop_if_alerted_to_enemy)) &&
                   actor && actor->state == Actor::state_t::combat && actor->seen_ago < 1.f)
                    c.done = true;
            }
            if(c.done)
            {
                finish_conversation(p, c);
                continue;
            }
            if(c.waiting || m_vm.now() < c.until)
                continue;
            if(c.line >= 0 && (lines.value()[c.line].flags & line_t::wait_after_until_told_to_advance) &&
               !c.advanced)
            {
                c.waiting = true;
                continue;
            }
            c.advanced = false;
            if(++c.line >= static_cast<i32>(lines.value().size()))
            {
                finish_conversation(p, c);
                continue;
            }
            auto const& line = lines.value()[c.line];
            auto        who  = [&c](i16 i) -> u64 {
                return i >= 0 && static_cast<size_t>(i) < c.participants.size() ? c.participants[i] : 0;
            };
            u64 const speaker = who(line.participant);
            /* Addressed to the player (1) or a participant (2) */
            std::optional<Vecf3> at;
            if(line.addressee == 1)
                at = player_near(p, speaker);
            else if(line.addressee == 2)
                if(auto const* model = p.template get<Model>(who(line.addressee_participant)))
                    at = model->position;
            auto const* mouth = speaker ? p.template get<Model>(speaker) : nullptr;
            if(speaker && at)
                turn(p, speaker, *at);
            if((line.flags & line_t::addressee_looking_at_speaker) && mouth && line.addressee == 2)
                turn(p, who(line.addressee_participant), mouth->position);
            for(u64 id : c.participants)
                if(id && id != speaker)
                {
                    if((line.flags & line_t::everyone_look_at_speaker) && mouth)
                        turn(p, id, mouth->position);
                    else if((line.flags & line_t::everyone_look_at_addressee) && at)
                        turn(p, id, *at);
                }
            f32 seconds = line.line_delay_time;
            for(auto const& variant : line.variants)
            {
                if(!variant.valid())
                    continue;
                value_t sound;
                sound.integer = static_cast<i32>(variant.tag_id.value());
                if(!sound_tag(sound))
                    continue;
                play(p, sound, speaker ? std::vector<u64>{speaker} : std::vector<u64>{}, false);
                seconds += length_of(p, sound);
                if(speaker && p.template get<AnimationPlayback>(speaker))
                    m_speakers[speaker] = {.mouth = mouth_of(p, sound), .from = m_vm.now()};
                break;
            }
            c.until = m_vm.now() + static_cast<libc_types::i64>(seconds * tick_rate);
            if(std::getenv("BLAM_DEBUG_AI"))
                log(fmt::format("ai_conversation: {} line {} by {} for {}s", conv->name.str(), c.line, speaker, seconds));
        }
    }

    /* The player nearest a unit */
    std::optional<Vecf3> player_near(Proxy& p, u64 from)
    {
        auto const*          origin  = from ? p.template get<Model>(from) : nullptr;
        std::optional<Vecf3> best;
        f32                  nearest = 1e30f;
        for(auto ent : p.template select<PlayerInfo, Model>())
        {
            auto const& model = ent.template get<Model>();
            f32 const   d     = origin ? glm::distance(origin->position, model.position) : 0.f;
            if(d < nearest)
            {
                nearest = d;
                best    = model.position;
            }
        }
        return best;
    }

    /* A unit faces a point; players look where they please */
    void turn(Proxy& p, u64 who, Vecf3 const& at)
    {
        auto* model = p.template get<Model>(who);
        if(!model || p.template get<PlayerInfo>(who))
            return;
        Vecf3 const d = at - model->position;
        if(d.x * d.x + d.y * d.y > 1e-4f)
            model->rotation = glm::angleAxis(std::atan2(d.y, d.x), Vecf3{0.f, 0.f, 1.f});
    }

    /* A tick of each list: instant atoms run through, moving and looking
     * ones take their time */
    void run_commands(Proxy& p)
    {
        using atom   = blam::scn::ai::command_list_command::atom_type_t;
        constexpr f32 dt = 1.f / tick_rate;
        for(auto it = m_commands.begin(); it != m_commands.end();)
        {
            /* An enemy in sight and a list that allows it: the actors fight */
            if(auto* actor = p.template get<Actor>(it->first); actor && actor->release)
            {
                actor->scripted = false;
                actor->release  = false;
                it              = m_commands.erase(it);
                continue;
            }
            /* On its way to a vehicle: the list waits */
            if(auto const* actor = p.template get<Actor>(it->first); actor && actor->board)
            {
                ++it;
                continue;
            }
            auto* model = p.template get<Model>(it->first);
            auto  cmds  = it->second.list->commands.data(m_magic);
            auto  pts   = it->second.list->points.data(m_magic);
            if(!model || !cmds.has_value())
            {
                if(auto* actor = p.template get<Actor>(it->first))
                    actor->scripted = false;
                it = m_commands.erase(it);
                continue;
            }
            auto& run   = it->second;
            auto point  = [&](i16 i) -> std::optional<Vecf3> {
                if(!pts.has_value() || i < 0 || static_cast<size_t>(i) >= pts.value().size())
                    return std::nullopt;
                return pts.value()[static_cast<size_t>(i)].position;
            };
            auto object = [&](i16 i) -> std::optional<Vecf3> {
                auto names = m_scenario->objects.object_names.data(m_magic);
                if(!names.has_value() || i < 0 || static_cast<size_t>(i) >= names.value().size())
                    return std::nullopt;
                auto const* target = p.template get<Model>(named_entity(p, lowered(names.value()[static_cast<size_t>(i)].name.str())));
                return target ? std::optional(target->position) : std::nullopt;
            };
            auto face = [&](Vecf3 const& at) {
                Vecf3 const d = at - model->position;
                if(d.x * d.x + d.y * d.y > 1e-4f)
                    model->rotation = glm::angleAxis(std::atan2(d.y, d.x), Vecf3{0.f, 0.f, 1.f});
            };
            auto next = [&run] {
                run.at++;
                run.ticks = 0;
            };
            /* Instant atoms run on; a bound keeps a loop of them from spinning */
            for(u32 step = 0; step < 16 && !run.done; step++)
            {
                if(run.at < 0 || static_cast<size_t>(run.at) >= cmds.value().size())
                {
                    run.done = true;
                    break;
                }
                auto const& c = cmds.value()[static_cast<size_t>(run.at)];
                bool waits = false;
                switch(c.atom_type)
                {
                case atom::set_radius:
                    run.radius = c.param1 > 0.f ? c.param1 : run.radius;
                    next();
                    break;
                case atom::pause:
                    if(run.ticks++ >= static_cast<i32>(c.param1 * tick_rate))
                        next();
                    else
                        waits = true;
                    break;
                case atom::go_to:
                case atom::goto_and_face: {
                    auto const to = point(c.point_1);
                    Vecf3 const d = to ? *to - model->position : Vecf3{};
                    f32 const   left = glm::length(d);
                    if(!to || left <= run.radius)
                    {
                        next();
                        break;
                    }
                    f32 const step = std::min(left, walk_speeds(p, it->first).x * dt);
                    face(*to);
                    model->position += d / left * step;
                    waits = true;
                    break;
                }
                case atom::move_immediate:
                case atom::teleport:
                    if(auto const to = point(c.point_1))
                        model->position = *to;
                    next();
                    break;
                case atom::look:
                case atom::look_object:
                case atom::look_player: {
                    auto const at = c.atom_type == atom::look_object ? object(c.object_name) : point(c.point_1);
                    if(at)
                        face(*at);
                    if(run.ticks++ >= static_cast<i32>(c.param1 * tick_rate))
                        next();
                    else
                        waits = true;
                    break;
                }
                case atom::wait:
                    run.waiting = true;
                    waits       = true;
                    break;
                case atom::loop:
                    run.looped = true;
                    run.at     = c.command >= 0 ? c.command : 0;
                    run.ticks  = 0;
                    waits      = true;
                    break;
                default:
                    next();
                    break;
                }
                if(waits)
                    break;
            }
            model->update_matrix();
            ++it;
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
        if(!walks(p, id) && !rolls(p, id))
            carry(p, id, true);
        /* Choreographed: where it meets someone, it does not run them down */
        if(auto* physics = p.template get<ObjectPhysics>(id); physics && rolls(p, id))
            physics->recorded = true;
    }

    /* Wheels and treads, which physics drives along the ground */
    static bool rolls(Proxy& p, u64 id)
    {
        auto const* physics = p.template get<ObjectPhysics>(id);
        return physics && physics->drive && physics->mass_points &&
               (physics->drive->type == VehicleDrive::human_jeep ||
                physics->drive->type == VehicleDrive::human_tank);
    }

    void stop_recording(Proxy& p, u64 id, end_t end)
    {
        std::erase_if(m_recordings, [id](recording_t const& r) { return r.entity == id; });
        if(auto* physics = p.template get<ObjectPhysics>(id))
            physics->recorded = false;
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
        carry(p, id, on || (playing && !rolls(p, id)));
    }

    void carry(Proxy& p, u64 id, bool scripted)
    {
        scripted      = scripted || m_suspended.contains(id);
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
            if(drive && rolls(p, it->entity))
            {
                /* Driven as a player drives it: throttle against the motion
                 * brakes, the wheels steer toward the facing */
                it->from = std::nullopt;
                Physics::Event ev{Physics::Event::Drive};
                Physics::Drive steer{
                    .vehicle  = it->entity,
                    .throttle = input.movement.x,
                    .steer    = input.movement.y,
                    .aim      = facing.direction(),
                    .brake    = input.held(blam::scn::recorded::control_jump),
                    .recorded = true,
                };
                p.template subsystem<PhysicsBus>().inject(ev, &steer);
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
        i16                             encounter_idx = -1;
        if(auto list = m_scenario->ai.encounters.data(m_magic); list.has_value())
            for(size_t i = 0; i < list.value().size(); i++)
                if(lowered(list.value()[i].name.str()) == lowered(name))
                {
                    encounter     = &list.value()[i];
                    encounter_idx = static_cast<i16>(i);
                }
        if(!encounter)
            return log(fmt::format("ai_place: no encounter {}", path));
        log(fmt::format("ai_place: {}", path));
        auto squads  = encounter->squads.data(m_magic);
        auto palette = m_scenario->ai.actor_palette.data(m_magic);
        if(!squads.has_value() || !palette.has_value())
            return;
        for(size_t si = 0; si < squads.value().size(); si++)
        {
            auto const& squad = squads.value()[si];
            if(!which.empty() && lowered(squad.name.str()) != lowered(which))
                continue;
            auto starts = squad.start_locations.data(m_magic);
            if(!starts.has_value() || starts.value().empty())
                continue;
            auto      lists = m_scenario->ai.command_lists.data(m_magic);
            u32 const count = std::max<u32>(squad.normal_diff_count, 1);
            for(u32 i = 0; i < count && i < starts.value().size(); i++)
            {
                auto const& start = starts.value()[i];
                /* A location's own actor type beats the squad's */
                i16 const type = start.actor_type >= 0 ? start.actor_type : squad.actor_type;
                if(type < 0 || static_cast<size_t>(type) >= palette.value().size())
                    continue;
                auto const* unit = unit_of(palette.value()[type]);
                if(!unit)
                    continue;
                u32 const        net_id = script_net_id_base | m_next_actor++;
                GameEvent        ev{.type = GameEvent::SpawnObject};
                SpawnObjectEvent spawn{
                    .object       = *unit,
                    .position     = start.position,
                    .rotation     = glm::angleAxis(start.facing, Vecf3{0.f, 0.f, 1.f}),
                    .net_id       = net_id,
                    .server_owned = true,
                    .held         = weapon_of(palette.value()[type]),
                };
                p.template subsystem<GameEventBus>().inject(ev, &spawn);
                m_spawning.insert(net_id);
                m_actor_slots[net_id] = {
                    .encounter = encounter_idx,
                    .squad     = static_cast<i16>(si),
                    .variant   = type,
                    .team      = static_cast<u16>(encounter->team_index),
                    .post      = start.position};
                /* The encounter, its squad and its platoon name it */
                m_actors[lowered(name)].push_back(net_id);
                m_actors[fmt::format("{}/{}", lowered(name), lowered(squad.name.str()))].push_back(net_id);
                if(auto toon = platoon_name(*encounter, squad); !toon.empty())
                    m_actors[fmt::format("{}/{}", lowered(name), toon)].push_back(net_id);
                /* Once it is there: its state's stance, and the location's
                 * command list */
                using state_t = blam::scn::ai::state_t;
                auto const  state = start.initial_state != state_t::none ? start.initial_state : squad.initial_state;
                auto const* list  = lists.has_value() && start.command_list >= 0 &&
                                           static_cast<size_t>(start.command_list) < lists.value().size()
                                        ? &lists.value()[static_cast<size_t>(start.command_list)]
                                        : nullptr;
                m_deferred.push_back({net_id, [this, state, list](Proxy& p, u64 id) {
                                          if(auto* anim = p.template get<AnimationPlayback>(id))
                                              anim->stance = stance_of(state);
                                          if(list)
                                              start_commands(p, id, *list);
                                      }});
            }
        }
    }

    /* The unit (stance) an actor animates from in a state */
    static char const* stance_of(blam::scn::ai::state_t state)
    {
        using state_t = blam::scn::ai::state_t;
        switch(state)
        {
        case state_t::sleeping:
            return "asleep";
        case state_t::alert:
            return "alert";
        case state_t::fleeing:
            return "flee";
        default:
            return "stand";
        }
    }

    /* A placed unit is told its squad once it exists, and fights by it */
    void enlist(Proxy& p, u64 id, u32 net_id)
    {
        auto slot = m_actor_slots.find(net_id);
        if(slot == m_actor_slots.end())
            return;
        if(auto* actor = p.template get<Actor>(id))
        {
            using team_t = blam::scn::unit::team_t;
            actor->encounter = slot->second.encounter;
            actor->squad     = slot->second.squad;
            actor->variant   = slot->second.variant;
            actor->post      = slot->second.post;
            actor->team      = slot->second.team;
            /* The encounter's default is the unit tag's own team */
            if(actor->team == 0)
                if(auto const* model = p.template get<Model>(id); model && model->origin_object)
                    if(auto unit = model->origin_object->template data<blam::scn::unit>(m_magic);
                       unit.has_value())
                        actor->team = static_cast<u16>(unit.value()[0].default_team);
            if(std::getenv("BLAM_DEBUG_AI"))
                cDebug(
                    "ai: enlist {} encounter {} squad {} team {} scripted {}",
                    id,
                    actor->encounter,
                    actor->squad,
                    actor->team,
                    actor->scripted);
        } else if(std::getenv("BLAM_DEBUG_AI"))
            cDebug("ai: enlist {}: no Actor component", id);
        m_actor_slots.erase(slot);
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

    /* The lowered name of a squad's platoon, empty without one */
    std::string platoon_name(blam::scn::ai::encounter const& encounter, blam::scn::ai::squad const& squad) const
    {
        auto platoons = encounter.platoons.data(m_magic);
        if(!platoons.has_value() || squad.platoon >= platoons.value().size())
            return {};
        return lowered(platoons.value()[squad.platoon].name.str());
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
        {
            if(std::getenv("BLAM_DEBUG_AI") && m_unnamed.insert(lowered(path)).second)
                cDebug("script: no actors named {}", path);
            return out;
        }
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
    std::map<u64, commands_t>               m_commands;
    std::map<blam::scn::ai::conversation const*, conversation_t> m_conversations;
    static constexpr libc_types::i64 player_claim_ticks = 60;
    std::map<std::pair<u64, std::string>, libc_types::i64> m_player_claims; /* vehicle, seat: tick asked */
    mutable std::set<std::string> m_unnamed; /* ai paths that named nobody, logged once */
    struct scaling_t
    {
        f32 from{1.f};
        f32 to{1.f};
        i32 ticks{0};
        i32 at{0};
    };
    std::map<u64, scaling_t> m_scaling;
    std::set<u64>            m_suspended;
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
    /* Squads of the units ai_place is spawning, by net id */
    struct actor_slot_t
    {
        i16   encounter{-1};
        i16   squad{-1};
        i16   variant{-1};
        u16   team{0};
        Vecf3 post{};
    };
    std::map<u32, actor_slot_t> m_actor_slots;
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
