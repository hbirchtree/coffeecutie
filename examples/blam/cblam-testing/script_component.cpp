#include "script_component.h"

#include "blam_files.h"
#include "cinematic.h"
#include "components.h"
#include "data.h"
#include "model_markers.h"
#include "network/networking.h"
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
using type_safety::empty_list_t;
using type_safety::type_list_t;

using Coffee::cDebug;
using Coffee::cWarning;

using BlamScriptManifest = compo::SubsystemManifest<
    type_list_t<
        const PlayerCamera,
        const PlayerInfo,
        const NetworkInfo,
        const CameraLerp,
        Model,
        ObjectPhysics>,
    type_list_t<
        GameEventBus,
        CinematicState,
        const NetworkState,
        const BlamFiles<halo_version>,
        const SoundCache<halo_version>>,
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
        m_riders.clear();
        m_hovering.clear();
        m_spawning.clear();
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

        if(!m_running || p.template subsystem<NetworkState>().replica)
            return;
        m_proxy = &p;
        for(size_t i = 0; i < ticks.size(); i++)
        {
            run_deferred(p);
            std::erase_if(m_spawning, [this, &p](u32 net_id) {
                return entity_by_net_id(p, net_id) != 0;
            });
            m_vm.tick();
            play_recordings(p);
            place_riders(p);
        }
        m_proxy = nullptr;
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
        return entity_by_net_id(
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
            if(u64 const id = entity_by_net_id(p, net_id))
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
        case op::object_destroy_containing:
            for(auto const& [name, placement] : m_named)
            {
                if(name.find(text_of(arg(0))) == std::string::npos)
                    continue;
                if(opcode == op::object_create_containing)
                    object_create(p, name, false);
                else if(u64 const id = named_entity(p, name))
                    despawn(p, id);
            }
            return {};
        case op::object_teleport:
            with_objects(p, arg(0), [this, flag = std::string(text_of(arg(1)))](Proxy& p, u64 id) {
                teleport(p, id, flag);
            });
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
        case op::sound_impulse_start:
            play(p, arg(0), objects_of(p, arg(1)), false);
            return {};
        case op::sound_impulse_time:
            return value_t::number(type_t::short_, length_of(p, arg(0)) * tick_rate);
        case op::sound_looping_start:
            play(p, arg(0), objects_of(p, arg(1)), true);
            return {};
        case op::sound_looping_stop:
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
        if(u64 const existing = entity_by_net_id(p, net_id))
        {
            if(!anew)
                return;
            despawn(p, existing);
        }
        log(fmt::format("object_create: {}", name));
        GameEvent        ev{.type = GameEvent::SpawnObject};
        SpawnObjectEvent spawn{
            .object       = *placement.object,
            .position     = placement.position,
            .net_id       = net_id,
            .server_owned = true,
        };
        p.template subsystem<GameEventBus>().inject(ev, &spawn);
        m_spawning.insert(net_id);
    }

    void despawn(Proxy& p, u64 id)
    {
        auto const* net = p.template get<NetworkInfo>(id);
        if(!net || net->instance_id == 0)
            return;
        GameEvent          ev{.type = GameEvent::DespawnObject};
        DespawnObjectEvent gone{.net_id = net->instance_id};
        p.template subsystem<GameEventBus>().inject(ev, &gone);
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
        auto const* model = on.empty() ? nullptr : p.template get<Model>(on.front());
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
                carry(p, id, false);
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
            f32 const   top   = drive ? drive->forward_speed * (plane ? plane_cruise : 1.f)
                                      : default_speed;
            f32 const   slide = drive ? drive->slide_speed : default_speed;
            f32 const   accel = drive && drive->acceleration > 0.f ? drive->acceleration : default_acceleration;
            f32 const   climb = plane ? (input.held(control_jump) ? 1.f : 0.f) -
                                            (input.held(control_crouch) ? 1.f : 0.f)
                                      : 0.f;
            Vecf3 const want  = ahead * (input.movement.x * top) +
                               right * (input.movement.y * slide) +
                               Vecf3{0.f, 0.f, climb * slide};
            Vecf3 const delta = want - it->velocity;
            f32 const   step  = accel * dt;
            it->velocity += glm::length(delta) > step ? glm::normalize(delta) * step : delta;

            model->position += it->velocity * dt;
            model->rotation = glm::angleAxis(yaw, Vecf3{0.f, 0.f, 1.f}) *
                              glm::angleAxis(-facing.pitch_radians(), Vecf3{0.f, 1.f, 0.f});
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
            if(u64 const id = entity_by_net_id(p, net_id))
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
            ImGui::BeginChild("conlog");
            for(auto const& line : m_log)
                ImGui::TextDisabled("%s", line.c_str());
            ImGui::EndChild();
            ImGui::EndTabItem();
        }
        if(ImGui::BeginTabItem("Threads"))
        {
            for(auto const& thread : m_vm.threads())
                ImGui::Text(
                    "%-32s %-10s %-9s",
                    thread.script->name.str().data(),
                    blam::hsc::to_string(thread.script->schedule).data(),
                    magic_enum::enum_name(thread.status).data());
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
    /* Where a body is tested against volumes, above its feet */
    static constexpr f32 body_middle = .3f;

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
    std::set<u32>                           m_spawning;
    std::vector<std::pair<u32, std::function<void(Proxy&, u64)>>> m_deferred;
    std::set<u64>                           m_hovering;
    std::set<u32>                           m_unhandled;
    Proxy*                                  m_proxy{nullptr};

    std::vector<std::string>   m_log;
    bool                       m_running{false};
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
