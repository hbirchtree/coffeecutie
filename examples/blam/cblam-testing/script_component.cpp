#include "script_component.h"

#include "blam_files.h"
#include "caching.h"
#include "cinematic.h"
#include "components.h"
#include "data.h"
#include "model_markers.h"
#include "network/networking.h"
#include "physics.h"
#include "script_vm.h"
#include "selected_version.h"
#include "sound_cache.h"
#include "sounds.h"
#include "ticks.h"

#include <blam/volta/blam_actor.h>
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
        const PlayerCamera,
        const PlayerInfo,
        const PlayerInput,
        const NetworkInfo,
        const CameraLerp,
        AnimationPlayback,
        Model,
        ObjectPhysics>,
    type_list_t<
        GameEventBus,
        CinematicState,
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
        m_walk_speeds.clear();
        m_tests = {};
        m_loops.clear();
        m_impulses.clear();
        m_speakers.clear();
        m_riders.clear();
        m_attached.clear();
        m_hovering.clear();
        m_spawning.clear();
        m_despawning.clear();
        m_recreate.clear();
        m_deferred.clear();
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
        m_proxy = &p;
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
            place_riders(p);
        }
        /* Every frame, so props follow the animated hands holding them */
        place_attached(p);
        m_proxy = nullptr;
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
            auto palette   = group.palette.data(m_magic);
            auto instances = group.instances.data(m_magic);
            if(!palette.has_value() || !instances.has_value())
                return;
            for(u32 i = 0; i < instances.value().size(); i++)
            {
                auto const& spawn = instances.value()[i];
                i16 const   name  = spawn.name;
                if(name < 0 || static_cast<size_t>(name) >= names.value().size() ||
                   spawn.ref < 0 || static_cast<size_t>(spawn.ref) >= palette.value().size())
                    continue;
                m_named[lowered(names.value()[name].name.str())] = {
                    .group    = group_id,
                    .index    = i,
                    .object   = &palette.value()[spawn.ref][0],
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

        /* Played on normal, alone */
        case op::game_difficulty_get:
        case op::game_difficulty_get_real:
            return {.type = type_t::game_difficulty, .integer = 1};

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
            PlayerTeleportEvent move{
                .entity_id = id, .position = flag->position, .rotation = facing};
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
    };

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
            auto marker = bind_marker(*model->tag, candidate.marker_name.str(), m_magic);
            unseat(p, unit);
            m_riders.push_back({
                .unit    = unit,
                .vehicle = vehicle,
                .seat    = label,
                .marker  = marker.value_or(Matf4(1)),
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
            Matf4 const at = vehicle->transform * rider.marker;
            unit->position = Vecf3(at[3]);
            unit->rotation = glm::normalize(Quatf(Matf3(at)));
            unit->update_matrix();
        }
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
            GameEvent               ev{.type = GameEvent::PlayModelAnimation};
            PlayModelAnimationEvent play{
                .entity    = id,
                .animation = i,
                .graph     = graph,
                .fade      = blend ? .2f : 0.f,
            };
            p.template subsystem<GameEventBus>().inject(ev, &play);
            bool const moves = clip.frame_info_type_ != blam::antr::frame_info_type::none;
            m_custom[id]     = {.clip = &clip, .moves = moves};
            if(moves && !walks(p, id))
                carry(p, id, true);
            return;
        }
        log(fmt::format("custom_animation: no animation {}", name));
    }

    void stop_custom(Proxy& p, u64 id)
    {
        auto it = m_custom.find(id);
        if(it == m_custom.end())
            return;
        bool const moves = it->second.moves;
        m_custom.erase(it);
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
                if(model)
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
        using blam::antr::frame_info_type;
        u32 const stride = clip.frame_info_type_ == frame_info_type::dx_dy           ? 8
                           : clip.frame_info_type_ == frame_info_type::dx_dy_dyaw    ? 12
                           : clip.frame_info_type_ == frame_info_type::dx_dy_dz_dyaw ? 16
                                                                                     : 0;
        auto bytes = clip.frame_info.data(m_magic);
        if(!stride || !bytes.has_value() || (frame + 1) * stride > bytes.value().size())
            return {};
        f32 delta[4]{};
        std::memcpy(delta, bytes.value().data() + frame * stride, stride);
        Vecf3 const move{delta[0], delta[1], stride == 16 ? delta[2] : 0.f};
        f32 const   yaw = stride == 16 ? delta[3] : stride == 12 ? delta[2] : 0.f;
        return {move, yaw};
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
    };

    void start_recording(Proxy& p, u64 id, std::string_view name, end_t end)
    {
        auto const* recording = named(m_scenario->recorded_animations, name);
        if(!recording)
            return log(fmt::format("recording_play: no recording {}", name));
        auto stream = recording->stream(m_magic);
        if(stream.has_error())
            return log(fmt::format("recording_play: {} unreadable", name));
        stop_recording(p, id, end_t::stop);
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
            /* Dropships climb on jump and sink on crouch; pitch is only
             * where they look */
            bool const  plane = drive && drive->type == VehicleDrive::human_plane;
            auto const  feet  = drive ? Vecf2{} : walk_speeds(p, it->entity);
            f32 const   top   = drive ? drive->forward_speed * (plane ? plane_cruise : 1.f)
                                      : feet.x;
            f32 const   slide = drive ? drive->slide_speed : feet.y;
            f32 const   accel = drive && drive->acceleration > 0.f ? drive->acceleration : default_acceleration;
            f32 const   climb = plane ? (input.held(control_jump) ? plane_climb : 0.f) -
                                            (input.held(control_crouch) ? plane_sink : 0.f)
                                      : 0.f;
            Vecf3 const want  = ahead * (input.movement.x * top) +
                               right * (input.movement.y * slide) +
                               Vecf3{0.f, 0.f, climb * slide};
            Vecf3 const delta = want - it->velocity;
            f32 const   step  = accel * dt;
            it->velocity += glm::length(delta) > step ? glm::normalize(delta) * step : delta;

            /* Bipeds stay upright and on the ground */
            bool const upright = physics && physics->upright;
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

    /* A unit on foot covers the ground its move clips do */
    Vecf2 walk_speeds(Proxy& p, u64 id)
    {
        using Slot        = blam::antr::unit_weapon::animation_idx_t;
        auto const* anim  = p.template get<AnimationPlayback>(id);
        auto const* graph = anim ? anim->graph : nullptr;
        if(!graph)
            return Vecf2(default_speed);
        auto [it, fresh] = m_walk_speeds.try_emplace(std::pair{graph, anim->stance});
        if(fresh)
        {
            f32 front = move_speed(*graph, Slot::move_front, m_magic, anim->stance, "unarmed");
            f32 side  = move_speed(*graph, Slot::move_left, m_magic, anim->stance, "unarmed");
            if(front <= 0.f)
                front = move_speed(*graph, Slot::move_front, m_magic, "stand", "unarmed");
            if(side <= 0.f)
                side = move_speed(*graph, Slot::move_left, m_magic, "stand", "unarmed");
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

    /* To a cutscene camera point over `ticks`, in an object's space if
     * given */
    void camera_set(Proxy& p, std::string_view point_name, f32 ticks, u64 relative_to)
    {
        auto const* point = named(m_scenario->cutscene.camera_points, point_name);
        if(!point)
            return log(fmt::format("camera_set: no camera point {}", point_name));
        auto [position, rotation] = point->to_camera(
            Matf3(Vecf3{0, -1, 0}, Vecf3{-1, 0, 0}, Vecf3{0, 0, -1}));
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
    /* Of a plane's top speed at full throttle: b30's dropships land where
     * their marines stand at this, not at the tag's maximum */
    static constexpr f32 plane_cruise = .75f;
    /* Of its slide speed: c10's and b30's dropships sink onto their landing
     * zones at this, and b40's climbs onto its ledge at that */
    static constexpr f32 plane_sink  = .5f;
    static constexpr f32 plane_climb = .2f;
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
    std::vector<attached_t>                 m_attached;
    std::set<u32>                           m_spawning;
    std::vector<std::pair<u32, std::function<void(Proxy&, u64)>>> m_deferred;
    std::set<u64>                           m_hovering;
    std::map<u64, custom_t>                 m_custom;
    std::map<u64, Vecf3>                    m_walking;
    struct speaker_t
    {
        std::vector<u8> mouth;
        libc_types::i64 from{0};
    };
    std::map<u64, speaker_t> m_speakers;
    std::set<u32>                           m_despawning;
    std::set<u32>                           m_recreate;
    std::map<std::pair<blam::antr::header const*, std::string>, Vecf2> m_walk_speeds;
    action_tests_t                          m_tests;
    std::map<u32, libc_types::i64>          m_impulses; /* tag, tick started */
    std::set<u32>                           m_loops;
    std::set<u32>                           m_unhandled;
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
        0, [&script](auto&, auto*) { script.m_scenario = nullptr; });
    gbus.addEventFunction<MapChangedEvent<halo_version>>(
        0, [&script](auto&, MapChangedEvent<halo_version>* changed) {
            script.load_from(changed->container, changed->scenario);
        });
}
