#include "gameplay.h"

#include "blam_files.h"
#include "components.h"
#include "data.h"
#include "physics.h"
#include "selected_version.h"
#include "sounds.h"

#include <blam/volta/blam_effects.h>
#include <blam/volta/blam_shaders.h>

#include <coffee/components/proxy.h>
#include <coffee/components/restricted_subsystem.h>
#include <coffee/core/debug/formatting.h>

#include <map>
#include <set>
#include <vector>

using GameplayManifest = compo::SubsystemManifest<
    type_list_t<
        PlayerInfo,
        const PlayerInput,
        PlayerCamera,
        NetworkInfo,
        const Model,
        const ObjectPhysics,
        AnimationPlayback>,
    type_list_t<
        GameEventBus,
        PhysicsBus,
        const BlamFiles<halo_version>,
        const LoadingStatus>,
    type_list_t<comp_app::EventBus<SoundEvent>>>;

struct Gameplay : compo::RestrictedSubsystem<Gameplay, GameplayManifest>
{
    using type  = Gameplay;
    using Proxy = compo::proxy_of<GameplayManifest>;

    Gameplay()
    {
        /* After physics has moved vehicles, before the graphics loop
         * builds cameras from them */
        this->priority = 898;
    }

    void start_restricted(Proxy& p, time_point const& t)
    {
        auto& physics = p.template subsystem<PhysicsBus>();
        auto& game    = p.template subsystem<GameEventBus>();

        f32 const dt =
            m_last_frame == time_point{}
                ? 0.f
                : std::chrono::duration<f32>(t - m_last_frame).count();
        m_last_frame = t;
        animate_bipeds(p, physics, dt);

        for(auto player : p.template select<
                          PlayerInfo,
                          PlayerInput,
                          PlayerCamera,
                          NetworkInfo>())
        {
            auto [info, input, cam, net] = player.components();
            if(info.is_remote())
                continue;
            bool const playing =
                input.input_mode == PlayerInput::input_mode_t::game;

            /* Forge: carry what the cursor (screen centre) points at */
            Physics::Event grab_ev{Physics::Event::Grab};
            Physics::Grab  grab{
                 .entity_id = player.id(),
                 .origin    = cam.camera.position,
                 .to_world  = Quatf(glm::transpose(Matf3(cam.rotation))),
                 .held      = playing && input.intent.grab &&
                         info.permissions.camera && !info.mode.physics &&
                         info.riding.vehicle == 0,
            };
            physics.process(grab_ev, &grab);

            bool& was_using = m_using[player.id()];
            if(playing && input.intent.use && !was_using)
                use(p, game, player.id(), info, cam);
            was_using = input.intent.use;

            if(info.riding.vehicle != 0)
                ride(p, physics, info, input, cam, net);
        }

        for(auto const& enter : std::exchange(m_enters, {}))
            enter_vehicle(p, enter);
        for(auto const& exit : std::exchange(m_exits, {}))
            exit_vehicle(p, exit);
    }

    /* A rider gets out; anyone else gets in the closest vehicle in reach */
    void use(
        Proxy&              p,
        GameEventBus&       game,
        u64                 player,
        PlayerInfo const&   info,
        PlayerCamera const& cam)
    {
        if(info.riding.vehicle != 0)
        {
            GameEvent            ev{.type = GameEvent::UnitExitVehicle};
            UnitExitVehicleEvent exit{.unit = player};
            game.process(ev, &exit);
            return;
        }
        Vecf3 const feet =
            cam.camera.position - Vecf3{0, 0, info.biped.eye_height};
        u64 nearest  = 0;
        f32 distance = vehicle_reach;
        for(auto object : p.template select<Model, ObjectPhysics>())
        {
            auto [model, physics] = object.components();
            if(!physics.drive)
                continue;
            if(f32 d = glm::distance(model.position, feet); d < distance)
            {
                nearest  = object.id();
                distance = d;
            }
        }
        if(nearest == 0)
            return;
        GameEvent             ev{.type = GameEvent::UnitEnterVehicle};
        UnitEnterVehicleEvent enter{.unit = player, .vehicle = nearest};
        game.process(ev, &enter);
    }

    /* Behind the vehicle along the view, which is also where a driver
     * steers toward */
    void ride(
        Proxy&             p,
        PhysicsBus&        physics,
        PlayerInfo&        info,
        PlayerInput const& input,
        PlayerCamera&      cam,
        NetworkInfo&       net)
    {
        auto const* vehicle = p.template get<Model>(info.riding.vehicle);
        if(!vehicle)
        {
            info.riding = {};
            return;
        }
        Vecf3 const view =
            glm::transpose(Matf3(cam.rotation)) * Vecf3{0.f, 0.f, -1.f};
        cam.camera.position =
            vehicle->position + Vecf3{0.f, 0.f, chase_height} -
            view * chase_distance;
        net.changes.transform = net.changes.viewport = true;
        if(!info.riding.driver)
            return;
        bool const playing =
            input.input_mode == PlayerInput::input_mode_t::game;
        Physics::Event ev{Physics::Event::Drive};
        Physics::Drive drive{
            .vehicle  = info.riding.vehicle,
            .throttle = playing ? input.intent.throttle : 0.f,
            .aim      = view,
        };
        physics.process(ev, &drive);
    }

    /* Seats come from the vehicle's unit tag; a seat is taken while another
     * player rides in it */
    void enter_vehicle(Proxy& p, UnitEnterVehicleEvent const& enter)
    {
        auto* info    = p.template get<PlayerInfo>(enter.unit);
        auto* vehicle = p.template get<Model>(enter.vehicle);
        if(!info || !vehicle || !vehicle->origin_object ||
           info->riding.vehicle != 0)
            return;
        auto const& magic =
            p.template subsystem<BlamFiles<halo_version>>().container.magic;
        auto unit = vehicle->origin_object->data<blam::scn::unit>(magic);
        if(!unit.has_value())
            return;
        auto seats = unit.value()[0].seats.data(magic);
        if(!seats.has_value())
            return;
        std::set<i16> taken;
        for(auto rider : p.template select<PlayerInfo>())
            if(auto const& riding = rider.template get<PlayerInfo>().riding;
               riding.vehicle == enter.vehicle)
                taken.insert(riding.seat);
        using flags_t    = blam::scn::unit::seat_t::seat_flags_t;
        auto const label = enter.seat.str();
        for(i16 i = 0; i < static_cast<i16>(seats.value().size()); i++)
        {
            auto const& seat = seats.value()[i];
            bool const  driver =
                (seat.flags & flags_t::driver) != flags_t::none;
            if(taken.contains(i) ||
               (label.empty() ? !driver : seat.label.str() != label))
                continue;
            info->riding = {
                .vehicle = enter.vehicle,
                .seat    = i,
                .driver  = driver,
            };
            cDebug(
                "Player {} takes seat {} of {}",
                info->player_idx,
                seat.label.str(),
                enter.vehicle);
            return;
        }
    }

    void exit_vehicle(Proxy& p, UnitExitVehicleEvent const& exit)
    {
        auto* info = p.template get<PlayerInfo>(exit.unit);
        auto* cam  = p.template get<PlayerCamera>(exit.unit);
        if(!info || !cam || info->riding.vehicle == 0)
            return;
        /* Out on its left side, standing */
        if(auto const* vehicle = p.template get<Model>(info->riding.vehicle))
        {
            Vecf3 left = vehicle->rotation * Vecf3{0.f, 1.f, 0.f};
            left.z     = 0.f;
            if(glm::dot(left, left) > 1e-6f)
                left = glm::normalize(left);
            cam->camera.position =
                vehicle->position + left * exit_distance +
                Vecf3{0.f, 0.f, info->biped.eye_height + .3f};
        }
        cDebug("Player {} leaves {}", info->player_idx, info->riding.vehicle);
        info->riding = {};
        if(auto* net = p.template get<NetworkInfo>(exit.unit))
            net->changes.viewport = net->changes.transform = true;
    }

    /* A biped walks the way its camera moves, in the animation its graph
     * names for that; feet coming down are heard on what they land on */
    void animate_bipeds(Proxy& p, PhysicsBus& physics, f32 dt)
    {
        auto const& files   = p.template subsystem<BlamFiles<halo_version>>();
        auto const& loading = p.template subsystem<LoadingStatus>();
        bool const  loaded  = loading.loaded_map == LoadingStatus::loaded;
        if(files.load_generation != m_names_generation)
        {
            m_names.clear();
            m_names_generation = files.load_generation;
        }

        for(auto player : p.template select<
                          PlayerInfo,
                          PlayerCamera,
                          NetworkInfo,
                          AnimationPlayback>())
        {
            auto [info, cam, net, anim] = player.components();
            auto const& biped           = info.biped;
            /* Graphs from an unloaded map must not be touched */
            if(!loaded || !biped.anim_graph ||
               biped.load_generation != files.load_generation ||
               !biped_in_play(info, cam, net))
            {
                anim.stop(0);
                m_walkers.erase(player.id());
                continue;
            }

            Vecf3 const feet =
                cam.camera.position - Vecf3{0.f, 0.f, biped.eye_height};
            auto& walker = m_walkers[player.id()];
            Vecf3 velocity{};
            if(walker.seen && dt > 0.f)
                velocity = (feet - walker.feet) / dt;
            walker.feet = feet;
            walker.seen = true;
            velocity.z  = 0.f;
            /* Smoothed: remote cameras arrive in steps */
            walker.velocity = glm::mix(walker.velocity, velocity, .3f);
            f32 const speed = glm::length(walker.velocity);

            Vecf3 forward =
                glm::transpose(Matf3(cam.rotation)) * Vecf3{0.f, 0.f, -1.f};
            forward.z = 0.f;
            forward   = glm::dot(forward, forward) > 1e-6f
                            ? glm::normalize(forward)
                            : Vecf3{1.f, 0.f, 0.f};
            Vecf3 const left{-forward.y, forward.x, 0.f};

            char const* move = nullptr;
            if(speed > walk_threshold)
            {
                f32 const ahead = glm::dot(walker.velocity, forward);
                f32 const side  = glm::dot(walker.velocity, left);
                move            = std::abs(ahead) >= std::abs(side)
                                      ? (ahead > 0.f ? "move-front" : "move-back")
                                      : (side > 0.f ? "move-left" : "move-right");
            }
            i32 const animation =
                move ? find(p, biped.anim_graph, {std::string("stand unarmed ") + move})
                     : find(
                           p,
                           biped.anim_graph,
                           {"stand unarmed idle",
                            "stand rifle idle",
                            "stand pistol idle"});
            if(animation < 0)
            {
                anim.stop(0);
                continue;
            }
            auto& base = anim.layers[0];
            if(base.graph != biped.anim_graph ||
               base.animation != static_cast<u32>(animation))
                anim.play(0, biped.anim_graph, static_cast<u32>(animation));
            base.rate =
                move ? std::clamp(speed / stride_speed, .5f, 2.f) : 1.f;

            /* Markers come from the last frame's advance */
            if(anim.footsteps && biped.footsteps)
                for(auto foot :
                    {AnimationPlayback::left_foot, AnimationPlayback::right_foot})
                {
                    if(!(anim.footsteps & foot))
                        continue;
                    Physics::Event       ev{Physics::Event::GroundProbe};
                    Physics::GroundProbe probe{
                        .entity_id = player.id(),
                        .from      = feet + Vecf3{0.f, 0.f, .3f},
                        .to        = feet - Vecf3{0.f, 0.f, .4f},
                        .user      = speed > run_threshold ? running : walking,
                    };
                    physics.process(ev, &probe);
                }
            anim.footsteps = AnimationPlayback::no_feet;
        }

        for(auto const& hit : std::exchange(m_steps, {}))
            footstep(p, hit);
    }

    /* The first of `names` the graph has */
    i32 find(
        Proxy&                             p,
        blam::antr::header const*          graph,
        std::initializer_list<std::string> names)
    {
        for(auto const& name : names)
        {
            auto key = std::pair{graph, name};
            if(auto it = m_names.find(key); it != m_names.end())
            {
                if(it->second >= 0)
                    return it->second;
                continue;
            }
            auto& index = m_names[key];
            index       = -1;
            auto const& magic =
                p.template subsystem<BlamFiles<halo_version>>().container.magic;
            if(auto anims = graph->animations.data(magic); anims.has_value())
                for(i32 i = 0; i < static_cast<i32>(anims.value().size()); i++)
                    if(anims.value()[i].name.str() == name)
                    {
                        index = i;
                        break;
                    }
            if(index >= 0)
                return index;
        }
        return -1;
    }

    /* foot tag: effects[walk|run].materials[the shader's physics material] */
    void footstep(Proxy& p, Physics::GroundHit const& hit)
    {
        auto const* info = p.template get<PlayerInfo>(hit.entity_id);
        if(!info || !info->biped.footsteps || !hit.shader ||
           !hit.shader->valid())
            return;
        auto const& files = p.template subsystem<BlamFiles<halo_version>>();
        auto const& magic = files.container.magic;
        blam::tag_index_view<halo_version> index(files.container);
        auto shader = index.find(*hit.shader);
        if(shader == index.end())
            return;
        auto surface =
            (*shader).template data<blam::shader::radiosity_properties>(magic);
        auto feet = info->biped.footsteps->template data<
            blam::scn::material_effects>(magic);
        if(!surface.has_value() || !feet.has_value())
            return;
        auto effects = feet.value()[0].effects.data(magic);
        if(!effects.has_value() || hit.user >= effects.value().size())
            return;
        auto materials = effects.value()[hit.user].materials.data(magic);
        auto material  = static_cast<size_t>(surface.value()[0].physics);
        if(!materials.has_value() || material >= materials.value().size())
            return;
        blam::tagref_t const& sound = materials.value()[material].sound;
        if(!sound.valid())
            return;
        auto* sounds = p.template service<comp_app::EventBus<SoundEvent>>();
        if(!sounds)
            return;
        SoundEvent     ev{.type = SoundEvent::play_sound};
        PlaySoundEvent play{.sound = &sound, .position = hit.point};
        sounds->inject(ev, &play);
    }

    static constexpr f32 vehicle_reach  = 2.5f;
    static constexpr f32 chase_height   = 1.2f;
    static constexpr f32 chase_distance = 3.5f;
    static constexpr f32 exit_distance  = 1.6f;

    /* Events from anyone (scripts, players), acted on with the proxy */
    std::vector<UnitEnterVehicleEvent> m_enters;
    std::vector<UnitExitVehicleEvent>  m_exits;
    std::map<u64, bool>                m_using;

    /* Footsteps: foot tag effect slots, and how fast a stride goes */
    static constexpr u32 walking        = 0;
    static constexpr u32 running        = 1;
    static constexpr f32 walk_threshold = .25f; /* wu/s */
    static constexpr f32 run_threshold  = 2.5f;
    static constexpr f32 stride_speed   = 1.5f;

    struct walker_t
    {
        Vecf3 feet{};
        Vecf3 velocity{};
        bool  seen{false};
    };
    time_point                                                 m_last_frame{};
    std::map<u64, walker_t>                                    m_walkers;
    std::map<std::pair<blam::antr::header const*, std::string>, i32> m_names;
    u32                                     m_names_generation{0};
    std::vector<Physics::GroundHit>         m_steps;
};

void alloc_gameplay(compo::EntityContainer& e)
{
    auto& gameplay = e.register_subsystem_inplace<Gameplay>();
    e.subsystem_cast<PhysicsBus>().addEventFunction<Physics::GroundHit>(
        0, [&gameplay](Physics::Event&, Physics::GroundHit* hit) {
            gameplay.m_steps.push_back(*hit);
        });
    auto& game     = e.subsystem_cast<GameEventBus>();
    game.addEventFunction<UnitEnterVehicleEvent>(
        1024, [&gameplay](GameEvent&, UnitEnterVehicleEvent* enter) {
            gameplay.m_enters.push_back(*enter);
        });
    game.addEventFunction<UnitExitVehicleEvent>(
        1024, [&gameplay](GameEvent&, UnitExitVehicleEvent* exit) {
            gameplay.m_exits.push_back(*exit);
        });
}
