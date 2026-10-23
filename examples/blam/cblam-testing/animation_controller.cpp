#include "animation_controller.h"

#include "blam_files.h"
#include "caching.h"
#include "components.h"
#include "data.h"
#include "physics.h"
#include "selected_version.h"
#include "sounds.h"
#include "ticks.h"

#include <blam/volta/blam_effects.h>
#include <blam/volta/blam_shaders.h>

#include <coffee/components/proxy.h>
#include <coffee/components/restricted_subsystem.h>

#include <map>
#include <set>
#include <tuple>
#include <vector>

using AnimationControllerManifest = compo::SubsystemManifest<
    type_list_t<
        const PlayerInfo,
        const PlayerCamera,
        const NetworkInfo,
        const Model,
        const ObjectPhysics,
        const PhysicsData,
        const Attachment,
        const BipedEquipment,
        const BipedStatus,
        HeldObject,
        AnimationPlayback>,
    type_list_t<
        PhysicsBus,
        ModelCache<halo_version>,
        const BlamFiles<halo_version>,
        const LoadingStatus,
        const TickClock>,
    type_list_t<comp_app::EventBus<SoundEvent>>>;

struct AnimationController
    : compo::RestrictedSubsystem<AnimationController, AnimationControllerManifest>
{
    using type  = AnimationController;
    using Proxy = compo::proxy_of<AnimationControllerManifest>;

    AnimationController()
    {
        this->priority = 900;
    }

    void start_restricted(Proxy& p, time_point const&)
    {
        /* Playback steps with the simulation, and is drawn between ticks */
        auto const ticks = m_ticks->take();
        f32        dt    = 0.f;
        for(auto const& tick : ticks)
            dt += tick.seconds;
        f32 const lookahead =
            p.template subsystem<TickClock>().alpha() * TickClock::tick_seconds;

        auto const& files = p.template subsystem<BlamFiles<halo_version>>();
        if(files.load_generation != m_generation)
        {
            m_stances.clear();
            m_names.clear();
            m_walkers.clear();
            m_custom.clear();
            m_generation = files.load_generation;
            /* Players outlive the load, their layers point into the old map.
             * A fast load can finish before we ever see it unloaded. */
            for(auto player :
                p.template select<PlayerInfo, AnimationPlayback>())
            {
                auto& anim = player.template get<AnimationPlayback>();
                anim.stop_all();
                anim.graph = nullptr;
            }
        }

        for(auto const& play : std::exchange(m_requests, {}))
            apply(p, play);
        m_fire_queue->poll();
        for(auto const& shot : std::exchange(m_shots, {}))
            recoil(p, shot, files);
        if(dt > 0.f)
        {
            animate_bipeds(p, dt);
            animate_vehicles(p, dt);
        }
        animate_first_person(p, files);
        animate_faces(p);
        animate_turrets(p);
        end_actions(p);

        auto& cache = p.template subsystem<ModelCache<halo_version>>();
        for(auto ent : p.template select<AnimationPlayback>())
        {
            auto& anim = ent.template get<AnimationPlayback>();
            for(auto const& tick : ticks)
            {
                cache.advance_playback(anim, tick.seconds);
                hear(p, ent.id(), anim);
            }
            anim.lookahead = lookahead;
            /* Biped and device hulls follow the pose */
            auto const* info    = p.template get<PlayerInfo>(ent.id());
            auto const* physics = p.template get<ObjectPhysics>(ent.id());
            auto const* model   = p.template get<Model>(ent.id());
            /* First-person models too, for markers such as the muzzle */
            bool const posed = (info && info->biped.collision) ||
                               (physics && (physics->upright || physics->animated)) ||
                               p.template get<Attachment>(ent.id()) != nullptr ||
                               m_aimed.contains(ent.id());
            if(posed && model && anim.graph)
            {
                anim.pose.assign(cache.bone_count(model->model), Matf4(1));
                cache.evaluate_pose(
                    model->model,
                    anim,
                    Span<Matf4>(anim.pose.data(), anim.pose.size()));
            } else
                anim.pose.clear();
        }

        for(auto const& hit : std::exchange(m_steps, {}))
            footstep(p, hit);
    }

    using Slot       = blam::antr::unit_weapon::animation_idx_t;
    using UnitSlot   = blam::antr::unit::animation_idx_t;
    using WeaponSlot = blam::antr::weapon_animation_idx_t;

    /* A unit's animations in one stance holding one weapon: the antr
     * unit/weapon slot table, so new behaviour only names a Slot */
    struct stance_t
    {
        std::array<i32, Slot::zapping + 1>      slots{};
        std::array<i32, UnitSlot::hovering + 1> unit_slots{};
        std::array<i32, 10>                     weapon_slots{};
        blam::antr::screen_bounds               aim{};

        stance_t()
        {
            slots.fill(-1);
            unit_slots.fill(-1);
            weapon_slots.fill(-1);
        }

        i32 operator[](Slot slot) const
        {
            return slots[slot];
        }

        /* Whatever the weapon held: enter, exit, look, accelerations */
        i32 operator[](UnitSlot slot) const
        {
            return unit_slots[slot];
        }

        /* The held weapon's own: fire, reload, melee */
        i32 operator[](WeaponSlot slot) const
        {
            return weapon_slots[static_cast<size_t>(slot)];
        }
    };

    /* `unit` is the stance ("stand", "crouch", a seat), `weapon` the held
     * weapon's label: a weapon type ("ar") within a class ("rifle"), or the
     * class itself; the first class stands in when neither is there */
    stance_t const& stance(
        Proxy&                    p,
        blam::antr::header const* graph,
        std::string const&        unit  = "stand",
        std::string const&        weapon = unarmed)
    {
        auto key = std::tuple{graph, unit, weapon};
        if(auto it = m_stances.find(key); it != m_stances.end())
            return it->second;
        auto&       out   = m_stances[key];
        auto const& magic =
            p.template subsystem<BlamFiles<halo_version>>().container.magic;
        auto units = graph->units.data(magic);
        if(!units.has_value())
            return out;
        blam::antr::unit const* found = nullptr;
        for(auto const& candidate : units.value())
            if(candidate.label.str() == unit)
                found = &candidate;
        /* A stance the graph lacks falls back to standing */
        if(!found)
            return unit == "stand" ? out : (out = stance(p, graph, "stand", weapon));
        if(auto refs = found->animations.data(magic); refs.has_value())
            for(size_t i = 0;
                i < out.unit_slots.size() && i < refs.value().size();
                i++)
                out.unit_slots[i] = refs.value()[i].animation;
        auto weapons = found->weapons.data(magic);
        if(!weapons.has_value() || weapons.value().empty())
            return out;
        blam::antr::unit_weapon const* held = &weapons.value()[0];
        blam::antr::weapon_type const* type = nullptr;
        for(auto const& candidate : weapons.value())
        {
            if(!type && candidate.name.str() == weapon)
                held = &candidate;
            if(auto types = candidate.weapon_types.data(magic); types.has_value())
                for(auto const& t : types.value())
                    if(t.label.str() == weapon)
                    {
                        held = &candidate;
                        type = &t;
                    }
        }
        if(type)
            if(auto refs = type->animations.data(magic); refs.has_value())
                for(size_t i = 0; i < out.weapon_slots.size() && i < refs.value().size(); i++)
                    out.weapon_slots[i] = refs.value()[i].animation;
        auto refs = held->animations.data(magic);
        if(!refs.has_value())
            return out;
        for(size_t i = 0; i < out.slots.size() && i < refs.value().size(); i++)
            out.slots[i] = refs.value()[i].animation;
        out.aim = held->aiming_bounds;
        return out;
    }

    /* A biped walks the way its camera moves and aims where it looks */
    void animate_bipeds(Proxy& p, f32 dt)
    {
        auto const& files   = p.template subsystem<BlamFiles<halo_version>>();
        auto const& loading = p.template subsystem<LoadingStatus>();
        bool const  loaded  = loading.loaded_map == LoadingStatus::loaded;
        std::set<u64> seen;

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
               !biped_shown(info, cam, net))
            {
                anim.stop_all();
                anim.graph = nullptr;
                continue;
            }
            anim.graph = biped.anim_graph;
            seen.insert(player.id());

            Vecf3 const look =
                glm::transpose(Matf3(cam.rotation)) * Vecf3{0.f, 0.f, -1.f};
            std::string const weapon = held_label(p, player.id(), files);
            auto const* equip = p.template get<BipedEquipment>(player.id());
            auto const* held =
                equip ? equip->held(files.load_generation) : nullptr;
            auto* in_hand = held ? p.template get<HeldObject>(held->held) : nullptr;
            if(info.riding.vehicle != 0)
            {
                animate_rider(p, player.id(), info, anim, look, weapon, in_hand, dt);
                continue;
            }
            if(in_hand)
                in_hand->shown = true;
            /* A new weapon is drawn */
            auto& drawn = m_walkers[player.id()].weapon;
            if(!drawn.empty() && drawn != weapon)
                if(i32 const ready = stance(p, biped.anim_graph, "stand", weapon)[Slot::ready];
                   ready >= 0)
                    apply(p, {.entity = player.id(), .animation = ready, .fade = .1f});
            drawn = weapon;
            /* Reloads, in both views */
            bool const reloading = equip && equip->reloading;
            if(reloading && !m_walkers[player.id()].reloading)
                reload(p, player.id(), biped.anim_graph, weapon, held, files);
            m_walkers[player.id()].reloading = reloading;
            /* And grenade throws */
            bool const throwing = equip && equip->throwing;
            if(throwing && !m_walkers[player.id()].throwing)
                toss(p, player.id(), biped.anim_graph, weapon, files);
            m_walkers[player.id()].throwing = throwing;
            m_walkers[player.id()].seat         = {};
            m_walkers[player.id()].exiting      = false;
            m_walkers[player.id()].seen_vehicle = false;
            for(u32 i = 0; i < 3; i++)
                anim.stop(AnimationPlayback::lean_slot + i);
            auto const* data = p.template get<PhysicsData>(player.id());
            /* Only a body in physics mode can leave the ground */
            bool const grounded =
                !info.mode.physics || !data || data->grounded;
            animate_unit(
                p,
                player.id(),
                anim,
                cam.camera.position - Vecf3{0.f, 0.f, biped.eye_height},
                look,
                weapon,
                true,
                grounded,
                dt,
                "stand",
                dead(p, player.id()));
        }

        for(auto npc : p.template select<Model, ObjectPhysics, AnimationPlayback>())
        {
            auto [model, physics, anim] = npc.components();
            if(!loaded || !physics.upright || !anim.graph)
                continue;
            seen.insert(npc.id());
            animate_unit(
                p,
                npc.id(),
                anim,
                model.position,
                model.rotation * Vecf3{1.f, 0.f, 0.f},
                unarmed,
                true,
                physics.grounded,
                dt,
                anim.stance,
                dead(p, npc.id()));
        }

        for(auto it = m_walkers.begin(); it != m_walkers.end();)
            it = seen.contains(it->first) ? std::next(it) : m_walkers.erase(it);
    }

    /* Base layers from how a unit moves */
    void animate_unit(
        Proxy&             p,
        u64                id,
        AnimationPlayback& anim,
        Vecf3 const&       feet,
        Vecf3 const&       look,
        std::string const& weapon,
        bool               strides,
        bool               grounded,
        f32                dt,
        std::string const& unit = "stand",
        bool               dead = false)
    {
        auto& walker = m_walkers[id];
        if(dead)
            return die(id, anim, stance(p, anim.graph, unit, weapon), grounded);
        /* Revived: back on its feet, the death clip fading as idle takes over */
        if(walker.state == walker_t::dead)
            walker.state = walker_t::ground;
        Vecf3 velocity{};
        if(walker.seen && dt > 0.f)
            velocity = (feet - walker.feet) / dt;
        walker.feet = feet;
        walker.seen = true;
        /* Smoothed: remote cameras arrive in steps */
        walker.velocity = glm::mix(walker.velocity, velocity, .3f);
        Vecf3 const flat{walker.velocity.x, walker.velocity.y, 0.f};
        f32 const   speed = strides ? glm::length(flat) : 0.f;

        Vecf3 forward{look.x, look.y, 0.f};
        forward = glm::dot(forward, forward) > 1e-6f ? glm::normalize(forward)
                                                     : Vecf3{1.f, 0.f, 0.f};
        Vecf3 const left{-forward.y, forward.x, 0.f};

        auto const* graph = anim.graph;
        auto const& st    = stance(p, graph, unit, weapon);
        /* 0 standing, 1 walking; between, idle and stride blend */
        f32 const moving =
            std::clamp((speed - walk_threshold) / walk_threshold, 0.f, 1.f);

        /* Brief gaps (steps, seams) are not a fall */
        walker.air_time = grounded ? 0.f : walker.air_time + dt;
        if(walker.state != walker_t::air && walker.air_time > air_delay &&
           st[Slot::airborne] >= 0)
        {
            walker.state      = walker_t::air;
            walker.fall_speed = 0.f;
            if(!custom_playing(id, anim))
                anim.crossfade(graph, st[Slot::airborne], blend_time);
        }
        if(walker.state == walker_t::air)
        {
            walker.fall_speed = std::max(walker.fall_speed, -velocity.z);
            if(grounded)
            {
                i32 const land = walker.fall_speed > hard_landing_speed
                                     ? st[Slot::land_hard]
                                     : st[Slot::land_soft];
                walker.state = walker_t::ground;
                if(land >= 0 && !custom_playing(id, anim))
                {
                    anim.crossfade(graph, land, land_blend_time, false);
                    walker.state          = walker_t::landing;
                    walker.land_animation = land;
                    walker.hard_landing   = land == st[Slot::land_hard];
                }
            }
        }
        if(walker.state == walker_t::landing)
        {
            /* Moving off cuts a soft landing short, not a hard one */
            bool done = moving > 0.f && !walker.hard_landing;
            for(u32 i = AnimationPlayback::base_slot;
                i < AnimationPlayback::base_slot + AnimationPlayback::base_slots;
                i++)
            {
                auto const& layer = anim.layers[i];
                if(layer.graph == graph &&
                   layer.animation == static_cast<u32>(walker.land_animation) &&
                   layer.target != 0.f)
                    done = done || layer.finished;
            }
            if(done)
                walker.state = walker_t::ground;
        }

        if(walker.state == walker_t::ground && !custom_playing(id, anim))
        {
            f32 const ahead = glm::dot(walker.velocity, forward);
            f32 const side  = glm::dot(walker.velocity, left);
            f32 const sum   = std::abs(ahead) + std::abs(side);
            f32 const share[4] = {
                sum > 0.f ? std::max(ahead, 0.f) / sum : 0.f,
                sum > 0.f ? std::max(-ahead, 0.f) / sum : 0.f,
                sum > 0.f ? std::max(side, 0.f) / sum : 0.f,
                sum > 0.f ? std::max(-side, 0.f) / sum : 0.f,
            };
            /* Whatever else is in the base slots fades out */
            for(u32 i = AnimationPlayback::base_slot;
                i < AnimationPlayback::base_slot + AnimationPlayback::base_slots;
                i++)
            {
                auto& layer = anim.layers[i];
                if(layer.graph && !owns(st, layer))
                {
                    layer.target = 0.f;
                    layer.fade   = 1.f / blend_time;
                }
            }
            if(st[Slot::idle] >= 0)
                anim.blend(graph, st[Slot::idle], 1.f - moving, blend_time);
            for(u32 i = 0; i < 4; i++)
            {
                i32 const move = st[moves[i]];
                if(move < 0 || move == st[Slot::idle])
                    continue;
                /* Jitter in the velocity must not keep strides alive */
                f32 const weight = share[i] > .05f ? moving * share[i] : 0.f;
                auto*     layer  = anim.blend(graph, move, weight, blend_time);
                if(!layer)
                    continue;
                layer->sync = true;
                layer->rate = std::clamp(speed / stride_speed, .5f, 2.f);
            }
        }

        f32 const pitch = std::asin(std::clamp(look.z, -1.f, 1.f));
        aim(anim.layers[AnimationPlayback::aim_slot],
            graph,
            st[Slot::aim_still],
            st.aim,
            pitch,
            1.f - moving);
        aim(anim.layers[AnimationPlayback::aim_move_slot],
            graph,
            st[Slot::aim_move],
            st.aim,
            pitch,
            moving);

        /* Feet are probed for once the clocks have moved */
        walker.running = speed > run_threshold;
    }

    static bool dead(Proxy& p, u64 id)
    {
        auto const* status = p.template get<BipedStatus>(id);
        return status && status->dead;
    }

    /* Falls while airborne, lands dead, then lies there. One found dead,
     * such as a body placed by the map, starts out lying down */
    void die(u64 id, AnimationPlayback& anim, stance_t const& st, bool grounded)
    {
        auto&     walker = m_walkers[id];
        i32 const clip = grounded || st[UnitSlot::airborne_dead] < 0
                             ? st[UnitSlot::landing_dead]
                             : st[UnitSlot::airborne_dead];
        bool const found = !walker.seen;
        walker.seen      = true;
        if(clip < 0 || (walker.state == walker_t::dead && walker.land_animation == clip))
            return;
        walker.state          = walker_t::dead;
        walker.land_animation = clip;
        anim.stop(AnimationPlayback::aim_slot);
        anim.stop(AnimationPlayback::aim_move_slot);
        if(auto* layer = anim.crossfade(anim.graph, clip, found ? 0.f : blend_time, false);
           layer && found)
            layer->time = std::numeric_limits<f32>::max();
    }

    /* A rider plays its seat's animations: the seat's label names a unit in
     * the rider's graph, entered once, then idled in, aiming where it looks
     * relative to the vehicle */
    void animate_rider(
        Proxy&             p,
        u64                id,
        PlayerInfo const&  info,
        AnimationPlayback& anim,
        Vecf3 const&       look,
        std::string const& weapon,
        HeldObject*        in_hand,
        f32                dt)
    {
        auto const* vehicle = p.template get<Model>(info.riding.vehicle);
        if(!vehicle || !vehicle->origin_object)
            return;
        auto const& magic =
            p.template subsystem<BlamFiles<halo_version>>().container.magic;
        auto unit = vehicle->origin_object->data<blam::scn::unit>(magic);
        if(!unit.has_value())
            return;
        auto seats = unit.value()[0].seats.data(magic);
        if(!seats.has_value() || info.riding.seat < 0 ||
           static_cast<size_t>(info.riding.seat) >= seats.value().size())
            return;
        using seat_flags_t = blam::scn::unit::seat_t::seat_flags_t;
        auto const& seat_tag = seats.value()[info.riding.seat];
        /* Drivers put their weapon away */
        bool const armed = (seat_tag.flags & seat_flags_t::allows_weapons) !=
                           seat_flags_t::none;
        if(in_hand)
            in_hand->shown = armed;
        auto const* graph = anim.graph;
        auto const& st    = stance(
            p, graph, std::string(seat_tag.label.str()), armed ? weapon : unarmed);

        auto&      walker = m_walkers[id];
        auto const seat   = std::pair{info.riding.vehicle, info.riding.seat};
        /* Velocity starts over once it stands again */
        walker.seen  = false;
        walker.state = walker_t::ground;
        if(walker.seat != seat)
        {
            walker.seat     = seat;
            walker.entering = st[UnitSlot::enter] >= 0;
            if(walker.entering)
                anim.crossfade(graph, st[UnitSlot::enter], blend_time, false);
        }
        if(walker.entering)
        {
            walker.entering = false;
            for(u32 i = AnimationPlayback::base_slot;
                i < AnimationPlayback::base_slot + AnimationPlayback::base_slots;
                i++)
            {
                auto const& layer = anim.layers[i];
                if(layer.graph == graph &&
                   layer.animation == static_cast<u32>(st[UnitSlot::enter]) &&
                   !layer.finished)
                    walker.entering = true;
            }
        }
        if(info.riding.exiting && st[UnitSlot::exit] >= 0)
        {
            /* Played once; Gameplay stands it up when it has run out */
            if(!walker.exiting)
                anim.crossfade(graph, st[UnitSlot::exit], blend_time, false);
            walker.exiting  = true;
            walker.entering = false;
        } else if(!walker.entering && st[Slot::idle] >= 0)
            anim.crossfade(graph, st[Slot::idle], blend_time);

        Vecf3 const ahead = vehicle->rotation * Vecf3{1.f, 0.f, 0.f};
        f32 const   yaw   = std::remainder(
            std::atan2(look.y, look.x) - std::atan2(ahead.y, ahead.x),
            2.f * glm::pi<f32>());
        f32 const pitch = std::asin(std::clamp(look.z, -1.f, 1.f));
        aim(anim.layers[AnimationPlayback::aim_slot],
            graph,
            st[Slot::aim_still],
            st.aim,
            pitch,
            1.f,
            yaw);
        anim.stop(AnimationPlayback::aim_move_slot);

        /* Leaning with the vehicle's acceleration, in its own frame */
        auto const* physics = p.template get<ObjectPhysics>(info.riding.vehicle);
        Vecf3 accel{};
        if(physics && dt > 0.f && walker.seen_vehicle)
            accel = glm::conjugate(vehicle->rotation) *
                    ((physics->linear_velocity - walker.vehicle_velocity) / dt);
        walker.vehicle_velocity = physics ? physics->linear_velocity : Vecf3{};
        walker.seen_vehicle     = physics != nullptr;
        walker.lean = glm::mix(walker.lean, accel, std::min(1.f, lean_rate * dt));
        UnitSlot const leans[3] = {
            UnitSlot::acc_front_back,
            UnitSlot::acc_left_right,
            UnitSlot::acc_up_down};
        for(u32 i = 0; i < 3; i++)
        {
            f32 const amount =
                std::clamp(walker.lean[i] / lean_full, -1.f, 1.f);
            frame_strip(
                anim.layers[AnimationPlayback::lean_slot + i],
                graph,
                st[leans[i]],
                .5f + .5f * amount,
                magic);
        }
    }

    /* Vehicle pose from physics. Layers: 0 canopy, 1 steering, 2 wheels,
     * 3.. suspension. */
    void animate_vehicles(Proxy& p, f32 dt)
    {
        auto const& magic =
            p.template subsystem<BlamFiles<halo_version>>().container.magic;
        std::set<u64> driven;
        for(auto rider : p.template select<PlayerInfo>())
            if(auto const& riding = rider.template get<PlayerInfo>().riding;
               riding.vehicle != 0 && riding.driver)
                driven.insert(riding.vehicle);

        for(auto ent : p.template select<Model, ObjectPhysics, AnimationPlayback>())
        {
            auto [model, physics, anim] = ent.components();
            if(!physics.drive || !physics.mass_points || !anim.graph)
                continue;
            auto const* graph    = anim.graph;
            auto        vehicles = graph->vehicles.data(magic);
            if(!vehicles.has_value() || vehicles.value().empty())
                continue;
            auto const& vehicle = vehicles.value()[0];
            auto        slots   = vehicle.animations.data(magic);
            auto slot = [&](u32 i) -> i32 {
                return slots.has_value() && i < slots.value().size()
                           ? slots.value()[i].animation
                           : -1;
            };
            using V = blam::antr::vehicle;

            /* Canopy shut with a driver. Banshee clip names are inverted:
             * "closing" ends open, "opening" ends shut */
            auto const& st = stance(p, graph, "stand", "");
            i32 const   canopy = driven.contains(ent.id())
                                     ? st[UnitSlot::opening]
                                     : st[UnitSlot::closing];
            auto& base = anim.layers[0];
            if(canopy < 0)
                base = AnimationLayer{};
            else if(base.graph != graph ||
                    base.animation != static_cast<u32>(canopy))
                base = AnimationLayer{
                    .graph         = graph,
                    .animation     = static_cast<u32>(canopy),
                    .animated_only = true,
                    .loop          = false,
                };

            /* Steering: the wheels' angle, or a hovercraft's turning */
            auto const& v     = physics.vehicle;
            f32 const   steer = physics.drive->type == VehicleDrive::human_jeep
                                    ? v.steering
                                    : physics.angular_velocity.z * hover_lean;
            aim(anim.layers[1],
                graph,
                slot(V::steering),
                vehicle.steering_bounds,
                0.f,
                1.f,
                steer);

            /* Wheels turn a frame per fraction of their circumference */
            Vecf3 const forward = model.rotation * Vecf3{1.f, 0.f, 0.f};
            auto&       spin    = m_wheel_turn[ent.id()];
            if(physics.drive->wheel_circumference > 0.f)
                spin = std::fmod(
                    spin + glm::dot(physics.linear_velocity, forward) * dt /
                               physics.drive->wheel_circumference + 1.f,
                    1.f);
            frame_strip(anim.layers[2], graph, slot(V::ground_speed), spin, magic);
            anim.layers[2].grid_wrap = true;

            /* Suspension; airborne wheel = fully extended */
            u32  layer = 3;
            auto suspension = vehicle.suspension_animations.data(magic);
            for(size_t i = 0; suspension.has_value() &&
                              i < suspension.value().size() &&
                              layer < AnimationPlayback::talk_slot;
                i++, layer++)
            {
                auto const& s     = suspension.value()[i];
                auto const& point = s.mass_point_index;
                /* Hanging wheels drop toward the ground, up to full
                 * extension; tag depth is the centre's height below it */
                f32 depth = s.full_extension_ground_depth;
                if(point >= 0 &&
                   static_cast<size_t>(point) < v.ground_height.size() &&
                   v.ground_height[point] >= 0.f)
                    depth = -v.ground_height[point];
                else if(
                    point >= 0 &&
                    static_cast<size_t>(point) < v.ground_depth.size() &&
                    static_cast<size_t>(point) <
                        physics.mass_points->points.size() &&
                    v.ground_depth[point] > 0.f)
                    depth = v.ground_depth[point] -
                            physics.mass_points->points[point].radius;
                f32 const span = s.full_compression_ground_depth -
                                 s.full_extension_ground_depth;
                f32 const travel =
                    span != 0.f
                        ? std::clamp(
                              (depth - s.full_extension_ground_depth) / span,
                              0.f,
                              1.f)
                        : 0.f;
                frame_strip(anim.layers[layer], graph, s.animation.animation, travel, magic);
            }
            for(; layer < AnimationPlayback::talk_slot; layer++)
                anim.layers[layer] = AnimationLayer{};
        }
    }

    /* A gunner's look turns the vehicle's gun: its aim-still grid, in the
     * vehicle's own frame */
    void animate_turrets(Proxy& p)
    {
        using seat_flags  = blam::scn::unit::seat_t::seat_flags_t;
        using unit_flags  = blam::scn::unit::unit_flags_t;
        auto const& magic = p.template subsystem<BlamFiles<halo_version>>().container.magic;
        m_aimed.clear();
        for(auto rider : p.template select<PlayerInfo, PlayerCamera>())
        {
            auto [info, cam] = rider.components();
            u64 const   id   = info.riding.vehicle;
            auto*       anim = id ? p.template get<AnimationPlayback>(id) : nullptr;
            auto const* model = id ? p.template get<Model>(id) : nullptr;
            if(!anim || !anim->graph || !model || !model->origin_object || info.riding.exiting)
                continue;
            auto unit = model->origin_object->template data<blam::scn::unit>(magic);
            if(!unit.has_value())
                continue;
            auto seats = unit.value()[0].seats.data(magic);
            if(!seats.has_value() || info.riding.seat < 0 ||
               static_cast<size_t>(info.riding.seat) >= seats.value().size() ||
               (seats.value()[info.riding.seat].flags & seat_flags::gunner) == seat_flags::none)
                continue;
            auto const& st = stance(p, anim->graph, "stand", "fixed");
            Vecf3 const look  = glm::transpose(Matf3(cam.rotation)) * Vecf3{0.f, 0.f, -1.f};
            Vecf3 const local = glm::conjugate(model->rotation) * look;
            auto&       layer = anim->layers[AnimationPlayback::turret_slot];
            aim(layer,
                anim->graph,
                st[Slot::aim_still],
                st.aim,
                std::asin(std::clamp(local.z, -1.f, 1.f)),
                1.f,
                std::atan2(local.y, local.x));
            layer.grid_wrap = (unit.value()[0].unit_flags & unit_flags::circular_aiming) !=
                              unit_flags::none;
            m_aimed.insert(id);
        }
    }

    /* An overlay posed at `at` (0..1) along its frames, by grid */
    static void frame_strip(
        AnimationLayer&           layer,
        blam::antr::header const* graph,
        i32                       animation,
        f32                       at,
        blam::map_ptr const&      magic)
    {
        auto anims = graph->animations.data(magic);
        if(animation < 0 || !anims.has_value() ||
           static_cast<size_t>(animation) >= anims.value().size())
        {
            layer = AnimationLayer{};
            return;
        }
        i32 const frames = anims.value()[animation].frame_count;
        if(frames <= 0)
        {
            layer = AnimationLayer{};
            return;
        }
        if(layer.graph != graph || layer.animation != static_cast<u32>(animation))
            layer = AnimationLayer{
                .graph        = graph,
                .animation    = static_cast<u32>(animation),
                .grid_columns = static_cast<u16>(frames),
            };
        layer.cell = {
            at * static_cast<f32>(layer.grid_wrap ? frames : frames - 1), 0.f};
    }

    /* Stride directions, in the order movement shares are worked out */
    static constexpr Slot moves[4] = {
        Slot::move_front, Slot::move_back, Slot::move_left, Slot::move_right};

    static bool owns(stance_t const& st, AnimationLayer const& layer)
    {
        auto const a = static_cast<i32>(layer.animation);
        if(a == st[Slot::idle])
            return true;
        for(auto move : moves)
            if(a == st[move])
                return true;
        return false;
    }

    /* Aim grids are columns right to left, rows bottom to top */
    static void aim(
        AnimationLayer&                  layer,
        blam::antr::header const*        graph,
        i32                              animation,
        blam::antr::screen_bounds const& bounds,
        f32                              pitch,
        f32                              weight,
        f32                              yaw = 0.f)
    {
        i32 const cols = bounds.right_frame_count + bounds.left_frame_count + 1;
        if(animation < 0 || cols <= 0)
        {
            layer = AnimationLayer{};
            return;
        }
        if(layer.graph != graph || layer.animation != static_cast<u32>(animation))
            layer = AnimationLayer{
                .graph        = graph,
                .animation    = static_cast<u32>(animation),
                .grid_columns = static_cast<u16>(cols),
            };
        f32 row = static_cast<f32>(bounds.down_pitch_frame_count);
        if(pitch >= 0.f && bounds.up_pitch_per_frame > 0.f)
            row += pitch / bounds.up_pitch_per_frame;
        else if(pitch < 0.f && bounds.down_pitch_per_frame > 0.f)
            row += pitch / bounds.down_pitch_per_frame;
        /* Yaw is counter-clockwise, toward the left columns */
        f32 col = static_cast<f32>(bounds.right_frame_count);
        if(yaw >= 0.f && bounds.left_yaw_per_frame > 0.f)
            col += yaw / bounds.left_yaw_per_frame;
        else if(yaw < 0.f && bounds.right_yaw_per_frame > 0.f)
            col += yaw / bounds.right_yaw_per_frame;
        layer.cell   = {col, row};
        layer.weight = weight;
    }

    /* Talking opens the mouth as far as the line says; emotions hold a
     * frame of their own overlay */
    void animate_faces(Proxy& p)
    {
        for(auto ent : p.template select<AnimationPlayback>())
        {
            auto& anim = ent.template get<AnimationPlayback>();
            if(!anim.graph)
                continue;
            auto const& st    = stance(p, anim.graph, "stand", unarmed);
            auto        frame = [&](u32 slot, i32 clip, f32 time, f32 weight) {
                auto& layer = anim.layers[slot];
                if(clip < 0 || weight <= 0.f)
                {
                    if(layer.graph)
                        anim.stop(slot);
                    return;
                }
                if(layer.graph != anim.graph || layer.animation != static_cast<u32>(clip))
                    layer = AnimationLayer{
                        .graph     = anim.graph,
                        .animation = static_cast<u32>(clip),
                        .loop      = false,
                        .paused    = true,
                    };
                layer.time   = time;
                layer.weight = weight;
            };
            frame(AnimationPlayback::talk_slot, st[UnitSlot::talk], 0.f, anim.mouth);
            frame(
                AnimationPlayback::emotion_slot,
                st[UnitSlot::emotions],
                std::max<f32>(anim.emotion, 0.f) / 30.f,
                anim.emotion >= 0 ? 1.f : 0.f);
        }
    }

    /* A custom base animation holds the unit's own movement off */
    bool custom_playing(u64 entity, AnimationPlayback const& anim)
    {
        auto it = m_custom.find(entity);
        if(it == m_custom.end())
            return false;
        for(u32 i = AnimationPlayback::base_slot;
            i < AnimationPlayback::base_slot + AnimationPlayback::base_slots;
            i++)
        {
            auto const& layer = anim.layers[i];
            if(layer.graph == it->second.graph &&
               layer.animation == it->second.animation && !layer.finished &&
               layer.target != 0.f)
                return true;
        }
        m_custom.erase(it);
        return false;
    }

    /* First-person models return to idle once a requested clip ends */
    void animate_first_person(Proxy& p, BlamFiles<halo_version> const& files)
    {
        using fp_slot     = blam::antr::first_person_weapon;
        auto const& magic = files.container.magic;
        for(auto ent : p.template select<Attachment, AnimationPlayback>())
        {
            auto& anim = ent.template get<AnimationPlayback>();
            if(!anim.graph || custom_playing(ent.id(), anim))
                continue;
            auto fp = anim.graph->first_person_weapons.data(magic);
            if(!fp.has_value() || fp.value().empty())
                continue;
            auto slots = fp.value()[0].animations.data(magic);
            if(!slots.has_value() || slots.value().size() <= fp_slot::idle)
                continue;
            i32 const idle = slots.value()[fp_slot::idle].animation;
            if(idle >= 0)
                anim.crossfade(anim.graph, static_cast<u32>(idle), blend_time);
        }
    }

    /* One-shot actions and overlays fade once they have played out */
    void end_actions(Proxy& p)
    {
        for(auto ent : p.template select<AnimationPlayback>())
        {
            auto& anim = ent.template get<AnimationPlayback>();
            for(auto slot :
                {AnimationPlayback::action_slot, AnimationPlayback::overlay_slot})
            {
                auto& layer = anim.layers[slot];
                if(layer.graph && layer.finished && layer.target != 0.f)
                {
                    layer.target = 0.f;
                    layer.fade   = 1.f / blend_time;
                }
            }
        }
    }

    void apply(Proxy& p, PlayModelAnimationEvent const& play)
    {
        auto* anim = p.template get<AnimationPlayback>(play.entity);
        if(!anim || !anim->graph)
            return;
        auto const* graph = play.graph ? play.graph : anim->graph;
        f32 const   fade  = play.fade > 0.f ? 1.f / play.fade : 0.f;

        if(play.animation < 0 && play.name.empty())
        {
            for(auto slot :
                {AnimationPlayback::action_slot, AnimationPlayback::overlay_slot})
                if(anim->layers[slot].graph)
                {
                    anim->layers[slot].target = 0.f;
                    anim->layers[slot].fade   = fade;
                }
            m_custom.erase(play.entity);
            return;
        }

        auto const& magic =
            p.template subsystem<BlamFiles<halo_version>>().container.magic;
        i32 const index =
            play.animation >= 0 ? play.animation : find(p, graph, play.name);
        auto anims = graph->animations.data(magic);
        if(index < 0 || !anims.has_value() ||
           static_cast<size_t>(index) >= anims.value().size())
        {
            cDebug("animation: no '{}' ({}) in graph", play.name, play.animation);
            return;
        }
        auto const animation = static_cast<u32>(index);

        AnimationLayer* layer = nullptr;
        switch(anims.value()[index].type)
        {
        case blam::antr::anim_type::base:
            layer = anim->crossfade(graph, animation, play.fade, play.loop);
            m_custom[play.entity] = {graph, animation};
            break;
        case blam::antr::anim_type::replacement:
            layer = &anim->layers[AnimationPlayback::action_slot];
            break;
        case blam::antr::anim_type::overlay:
            layer = &anim->layers[AnimationPlayback::overlay_slot];
            break;
        }
        if(!layer)
            return;
        if(layer->graph != graph || layer->animation != animation)
            *layer = AnimationLayer{
                .graph     = graph,
                .animation = animation,
                .weight    = fade > 0.f ? 0.f : play.weight,
            };
        layer->time     = 0.f;
        layer->finished = false;
        layer->loop     = play.loop;
        layer->rate     = play.rate;
        layer->target   = play.weight;
        layer->fade     = fade;
        if(layer->weight <= 0.f && fade <= 0.f)
            layer->weight = play.weight;
    }

    /* The label of what a player holds, which picks its animations */
    std::string held_label(Proxy& p, u64 id, BlamFiles<halo_version> const& files)
    {
        auto const* equip = p.template get<BipedEquipment>(id);
        auto const* held  = equip ? equip->held(files.load_generation) : nullptr;
        return held ? std::string(held->weapon->label.str()) : unarmed;
    }

    void toss(
        Proxy&                         p,
        u64                            id,
        blam::antr::header const*      graph,
        std::string const&             weapon,
        BlamFiles<halo_version> const& files)
    {
        if(i32 const clip = stance(p, graph, "stand", weapon)[Slot::throw_grenade]; clip >= 0)
            apply(p, {.entity = id, .animation = clip, .fade = .1f});
        first_person_clip(p, id, blam::antr::first_person_weapon::throw_grenade, files);
    }

    /* The same slot on each of a player's first-person models */
    void first_person_clip(
        Proxy&                                           p,
        u64                                              id,
        blam::antr::first_person_weapon::animation_idx_t slot,
        BlamFiles<halo_version> const&                   files)
    {
        auto const& magic = files.container.magic;
        for(auto ent : p.template select<Attachment, AnimationPlayback>())
        {
            auto const& anim = ent.template get<AnimationPlayback>();
            if(ent.template get<Attachment>().parent != id || !anim.graph)
                continue;
            auto fp = anim.graph->first_person_weapons.data(magic);
            if(!fp.has_value() || fp.value().empty())
                continue;
            auto slots = fp.value()[0].animations.data(magic);
            if(slots.has_value() && slots.value().size() > slot &&
               slots.value()[slot].animation >= 0)
                apply(p, {.entity = ent.id(), .animation = slots.value()[slot].animation, .fade = 0.f});
        }
    }

    void reload(
        Proxy&                          p,
        u64                             id,
        blam::antr::header const*       graph,
        std::string const&              weapon,
        BipedEquipment::weapon_t const* held,
        BlamFiles<halo_version> const&  files)
    {
        using fp_slot = blam::antr::first_person_weapon;
        if(i32 const clip = stance(p, graph, "stand", weapon)[WeaponSlot::reload_1];
           clip >= 0)
            apply(p, {.entity = id, .animation = clip, .fade = .1f});
        first_person_clip(
            p,
            id,
            held && held->rounds_loaded == 0 ? fp_slot::reload_empty : fp_slot::reload_full,
            files);
    }

    /* A body on foot kicks with each shot it fires, and swings its weapon's
     * own way with each strike */
    void recoil(Proxy& p, WeaponFireEvent const& shot, BlamFiles<halo_version> const& files)
    {
        auto const* info = p.template get<PlayerInfo>(shot.shooter);
        auto const* anim = p.template get<AnimationPlayback>(shot.shooter);
        if(shot.vehicle || shot.thrown || !info || info->riding.vehicle || !anim ||
           !anim->graph)
            return;
        i32 const clip = stance(p, anim->graph, "stand", held_label(p, shot.shooter, files))
            [shot.melee ? WeaponSlot::melee : WeaponSlot::fire_1];
        auto anims = anim->graph->animations.data(files.container.magic);
        /* A base clip would stop it walking */
        if(clip < 0 || !anims.has_value() ||
           static_cast<size_t>(clip) >= anims.value().size() ||
           anims.value()[clip].type == blam::antr::anim_type::base)
            return;
        apply(p, {.entity = shot.shooter, .animation = clip, .fade = 0.f});
    }

    /* The animation called `name`, cached per graph */
    i32 find(Proxy& p, blam::antr::header const* graph, std::string const& name)
    {
        auto key = std::pair{graph, name};
        if(auto it = m_names.find(key); it != m_names.end())
            return it->second;
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
        return index;
    }

    /* Where an entity's feet are, or its model's origin */
    Vecf3 position_of(Proxy& p, u64 entity)
    {
        auto const* info = p.template get<PlayerInfo>(entity);
        auto const* cam  = p.template get<PlayerCamera>(entity);
        if(info && cam)
            return cam->camera.position -
                   Vecf3{0.f, 0.f, info->biped.eye_height};
        if(auto const* model = p.template get<Model>(entity))
            return model->position;
        return {};
    }

    /* Sound keys the clocks just passed, and feet that came down */
    void hear(Proxy& p, u64 entity, AnimationPlayback& anim)
    {
        auto* sounds = p.template service<comp_app::EventBus<SoundEvent>>();
        auto const& magic =
            p.template subsystem<BlamFiles<halo_version>>().container.magic;
        for(u32 i = 0; i < anim.cue_count && sounds; i++)
        {
            auto const& cue  = anim.cues[i];
            auto        refs = cue.graph->sound_refs.data(magic);
            if(!refs.has_value() ||
               static_cast<size_t>(cue.sound) >= refs.value().size())
                continue;
            blam::tagref_t const& sound = refs.value()[cue.sound].sound;
            if(!sound.valid())
                continue;
            SoundEvent     ev{.type = SoundEvent::play_sound};
            PlaySoundEvent play{
                .sound    = &sound,
                .position = position_of(p, entity),
            };
            sounds->inject(ev, &play);
        }
        anim.cue_count = 0;

        auto const* info = p.template get<PlayerInfo>(entity);
        auto        walker = m_walkers.find(entity);
        if(anim.footsteps && info && info->biped.footsteps &&
           walker != m_walkers.end())
        {
            auto&       physics = p.template subsystem<PhysicsBus>();
            Vecf3 const feet    = position_of(p, entity);
            for(auto foot :
                {AnimationPlayback::left_foot, AnimationPlayback::right_foot})
            {
                if(!(anim.footsteps & foot))
                    continue;
                Physics::Event       ev{Physics::Event::GroundProbe};
                Physics::GroundProbe probe{
                    .entity_id = entity,
                    .from      = feet + Vecf3{0.f, 0.f, .3f},
                    .to        = feet - Vecf3{0.f, 0.f, .4f},
                    .user      = walker->second.running ? running : walking,
                };
                physics.process(ev, &probe);
            }
        }
        anim.footsteps = AnimationPlayback::no_feet;
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

    /* Foot tag effect slots, how fast a stride goes, how long blends take */
    static inline std::string const unarmed = "unarmed";
    static constexpr u32 walking        = 0;
    static constexpr u32 running        = 1;
    static constexpr f32 walk_threshold = .25f; /* wu/s */
    static constexpr f32 run_threshold  = 2.5f;
    static constexpr f32 stride_speed   = 1.5f;
    static constexpr f32 blend_time     = .15f; /* s */
    static constexpr f32 land_blend_time = .08f;
    static constexpr f32 air_delay       = .1f; /* s off the ground */
    /* About a 1.4 wu drop under Halo's gravity */
    static constexpr f32 hard_landing_speed = 3.f; /* wu/s */

    struct walker_t
    {
        enum state_t : u8
        {
            ground,
            air,
            landing,
            dead,
        } state{ground};

        Vecf3 feet{};
        Vecf3 velocity{};
        f32   air_time{0.f};
        f32   fall_speed{0.f}; /* fastest descent this fall, wu/s */
        i32   land_animation{-1};
        /* The seat it sits in, {0, -1} standing */
        std::pair<u64, i16> seat{0, -1};
        bool                entering{false};
        bool                exiting{false};
        /* Rider lean: last vehicle velocity, smoothed acceleration */
        Vecf3 vehicle_velocity{};
        Vecf3 lean{};
        bool  seen_vehicle{false};
        bool  hard_landing{false};
        bool  seen{false};
        bool  running{false};
        std::string weapon; /* the label its stance was last drawn for */
        bool        reloading{false};
        bool        throwing{false};
    };
    struct custom_t
    {
        blam::antr::header const* graph{nullptr};
        u32                       animation{0};
    };

    std::unique_ptr<TickQueue>                   m_ticks;
    u32                                          m_generation{0};
    std::map<
        std::tuple<blam::antr::header const*, std::string, std::string>,
        stance_t>
        m_stances;
    std::map<std::pair<blam::antr::header const*, std::string>, i32> m_names;
    std::map<u64, walker_t>                      m_walkers;
    std::map<u64, custom_t>                      m_custom;
    std::set<u64>                                m_aimed; /* turrets a gunner turns */
    std::vector<PlayModelAnimationEvent>         m_requests;
    std::vector<WeaponFireEvent>                 m_shots;
    std::shared_ptr<GameEventBus::queue_type<WeaponFireEvent>> m_fire_queue;
    std::map<u64, f32>                           m_wheel_turn;
    /* Steering pose per rad/s a hovercraft turns at */
    static constexpr f32 hover_lean = .3f;
    /* Acceleration that leans a rider all the way, and how fast it follows */
    static constexpr f32 lean_full = 4.f; /* wu/s^2 */
    static constexpr f32 lean_rate = 6.f; /* 1/s */
    std::vector<Physics::GroundHit>              m_steps;
};

void alloc_animation_controller(compo::EntityContainer& e)
{
    auto& controller   = e.register_subsystem_inplace<AnimationController>();
    controller.m_ticks = std::make_unique<TickQueue>(e.subsystem_cast<GameEventBus>());
    e.subsystem_cast<PhysicsBus>().addEventFunction<Physics::GroundHit>(
        0, [&controller](Physics::Event&, Physics::GroundHit* hit) {
            controller.m_steps.push_back(*hit);
        });
    e.subsystem_cast<GameEventBus>().addEventFunction<PlayModelAnimationEvent>(
        1024, [&controller](GameEvent&, PlayModelAnimationEvent* play) {
            controller.m_requests.push_back(*play);
        });
    controller.m_fire_queue =
        e.subsystem_cast<GameEventBus>().addQueuedEventFunction<WeaponFireEvent>(
            0, [&controller](GameEvent&, WeaponFireEvent* shot) {
                controller.m_shots.push_back(*shot);
            });
}
