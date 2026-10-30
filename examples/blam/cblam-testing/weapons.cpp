#include "weapons.h"

#include "blam_files.h"
#include "components.h"
#include "data.h"
#include "model_markers.h"
#include "object_functions.h"
#include "network/networking.h"
#include "physics.h"
#include "projectiles.h"
#include "selected_version.h"
#include "sounds.h"
#include "ticks.h"

#include <blam/volta/blam_antr.h>
#include <blam/volta/blam_globals.h>
#include <blam/volta/blam_unit.h>

#include <coffee/components/proxy.h>
#include <coffee/components/restricted_subsystem.h>

#include <map>
#include <random>
#include <set>

using WeaponManifest = compo::SubsystemManifest<
    type_list_t<
        PlayerInput,
        const PlayerInfo,
        const PlayerCamera,
        const NetworkInfo,
        const Attachment,
        const AnimationPlayback,
        const BipedStatus,
        BipedEquipment,
        Model,
        const ObjectPhysics>,
    type_list_t<
        GameEventBus,
        PhysicsBus,
        const NetworkState,
        const BlamFiles<halo_version>>,
    type_list_t<comp_app::EventBus<SoundEvent>>>;

struct WeaponSystem : compo::RestrictedSubsystem<WeaponSystem, WeaponManifest>
{
    using type  = WeaponSystem;
    using Proxy = compo::proxy_of<WeaponManifest>;

    using weapon_t  = blam::scn::weapon;
    using trigger_t = weapon_t::trigger_t;
    using input_t   = weapon_t::weapon_function_in_t;

    struct state_t
    {
        f32  cooldown{0.f};
        f32  cooldown2{0.f};    /* a mounted gun's second trigger */
        f32  illumination{0.f}; /* the flash, 1 as it fires */
        bool firing{false};     /* held last frame */
        bool firing2{false};
        bool fired{false};      /* shot this frame */
        f32  reloading{0.f};    /* seconds left */
        f32  striking{0.f};     /* melee, seconds left */
        f32  charge{-1.f};      /* held while charging, s; <0 when not */
        blam::tag_t const* weapon{nullptr}; /* what it is for */
    };

    WeaponSystem()
    {
        this->priority = 850;
    }

    void start_restricted(Proxy& p, time_point const&)
    {
        auto const ticks = m_ticks->take();

        auto const& files = p.template subsystem<BlamFiles<halo_version>>();
        if(files.load_generation != m_generation)
        {
            m_generation = files.load_generation;
            m_states.clear();
            m_throws.clear();
            m_fire_queue->poll();
            m_shots.clear();
            m_grenades.clear();
        }
        auto const& magic = files.container.magic;

        std::set<u64> live;
        for(auto ent : p.template select<
                       PlayerInput,
                       PlayerInfo,
                       PlayerCamera,
                       NetworkInfo>())
        {
            auto [input, info, cam, net] = ent.components();
            /* Presses are taken whether or not they can be acted on */
            bool const swap   = std::exchange(input.intent.swap, false);
            bool       reload = std::exchange(input.intent.reload, false);
            bool const toss   = std::exchange(input.intent.grenade, false);
            bool const cycle  = std::exchange(input.intent.grenade_switch, false);
            bool const strike = std::exchange(input.intent.melee, false);
            /* A gunner fires the vehicle's weapon, a body its own, and so
             * does a passenger whose seat allows it; the free camera has
             * nothing to shoot with */
            u64 const   riding  = info.riding.vehicle;
            auto const* mounted = riding ? mounted_weapon(p, info, files) : nullptr;
            bool const  armed   = riding && !mounted && info.riding.armed;
            u64 const   vehicle = mounted ? riding : 0;
            auto*       equip   = p.template get<BipedEquipment>(ent.id());
            auto const* carried =
                equip ? equip->held(files.load_generation) : nullptr;
            auto const* weapon_tag = mounted                       ? mounted
                                     : (!riding || armed) && carried ? carried->tag
                                                                     : nullptr;
            /* A remote body the server simulates fires here too */
            if((info.is_remote() && !info.mode.simulated) || !weapon_tag ||
               (riding ? info.riding.exiting
                       : !info.mode.physics || !biped_in_play(info, cam, net)))
                continue;
            auto weapon = weapon_tag->template data<weapon_t>(magic);
            if(!weapon.has_value())
                continue;
            auto triggers = weapon.value()[0].triggers.data(magic);
            if(!triggers.has_value() || triggers.value().empty())
                continue;
            live.insert(ent.id());
            auto& state = m_states[ent.id()];
            /* Another weapon starts afresh */
            if(state.weapon != weapon_tag)
                state = {.weapon = weapon_tag};
            trigger_t const& trigger = triggers.value()[0];

            /* Grenades are thrown on foot, whatever is held */
            bool const throwing =
                !riding && grenade(p, ent.id(), info, cam, *equip, ticks, toss, cycle);
            /* A strike on foot stops the gun until it is done */
            for(auto const& tick : ticks)
                state.striking = std::max(state.striking - tick.seconds, 0.f);
            if(strike && !riding && !throwing && state.striking <= 0.f)
            {
                state.striking  = melee_time;
                state.reloading = 0.f;
                pull(p, ent.id(), info, cam, *weapon_tag, true);
            }
            bool const held = input.intent.fire && !throwing && state.striking <= 0.f;

            /* Bodies switch, reload and run dry; vehicle guns don't */
            auto* ammo = vehicle ? nullptr : &equip->weapons[0];
            if(ammo && swap && equip->weapons[1].weapon)
            {
                std::swap(equip->weapons[0], equip->weapons[1]);
                equip->reloading = false;
                continue;
            }
            auto const* magazine =
                ammo ? magazine_of(weapon.value()[0], trigger, magic) : nullptr;
            i16 const per_shot = std::max<i16>(trigger.rounds_per_shot, 1);

            /* A rate of zero fires once a pull */
            f32 const rate = trigger.maximum_rate_of_fire.y > 0.f ? trigger.maximum_rate_of_fire.y
                                                                  : trigger.maximum_rate_of_fire.x;
            bool const repeats =
                rate > 0.f &&
                (trigger.flags & trigger_t::flags_t::does_not_repeat_automatically) ==
                    trigger_t::flags_t::none;
            /* A trigger that charges fires as it is let go: its partner's
             * shot once charged */
            bool const charges = trigger.charging_time > 0.f && triggers.value().size() > 1;
            /* A mounted gun's second trigger fires alongside its first:
             * the tank's gun, the banshee's fuel rod */
            bool const held2 = mounted && triggers.value().size() > 1 && !charges &&
                               input.intent.fire2 && state.striking <= 0.f;
            auto const& gun_tag = weapon.value()[0];
            /* The trigger is looked at once a tick */
            for(auto const& tick : ticks)
            {
                f32 const dt    = tick.seconds;
                f32 const hurry = dt * info.speed_scale();
                state.fired     = false;
                state.cooldown  = std::max(state.cooldown - hurry, 0.f);
                if(state.reloading > 0.f &&
                   (state.reloading -= hurry) <= 0.f && magazine)
                    refill(*ammo, *magazine);
                bool const loaded =
                    !magazine || ammo->rounds_loaded >= per_shot;
                if(magazine && state.reloading <= 0.f && ammo->rounds_total > 0 &&
                   ammo->rounds_loaded < magazine->rounds_loaded_maximum &&
                   (reload || !loaded))
                {
                    state.reloading = std::max(magazine->reload_time, dt);
                    reload          = false;
                }
                if(ammo)
                    cool(*ammo, gun_tag, dt);
                bool const able = loaded && state.reloading <= 0.f && state.cooldown <= 0.f &&
                                  (!ammo || !spent(*ammo, gun_tag));
                i32 fires = -1;
                if(charges)
                {
                    if(held && !state.firing && able)
                        state.charge = 0.f;
                    else if(held && state.charge >= 0.f)
                        state.charge += dt;
                    else if(!held && state.charge >= 0.f)
                    {
                        fires        = state.charge >= trigger.charging_time ? 1 : 0;
                        state.charge = -1.f;
                    }
                } else if(held && (repeats || !state.firing))
                    fires = 0;
                if(fires >= 0 && able)
                {
                    trigger_t const& pulled = triggers.value()[fires];
                    if(rate > 0.f)
                        state.cooldown += 1.f / rate;
                    state.fired        = true;
                    state.illumination = 1.f;
                    if(magazine)
                        ammo->rounds_loaded -= per_shot;
                    if(ammo)
                        heat_up(p, ent.id(), *ammo, gun_tag, pulled);
                    pull(p, ent.id(), info, cam, *weapon_tag, false,
                         static_cast<u16>(fires));
                }
                state.firing = held;
                state.cooldown2 = std::max(state.cooldown2 - dt, 0.f);
                if(held2)
                {
                    trigger_t const& second = triggers.value()[1];
                    f32 const        rate2  = second.maximum_rate_of_fire.y > 0.f
                                                  ? second.maximum_rate_of_fire.y
                                                  : second.maximum_rate_of_fire.x;
                    bool const repeats2 =
                        rate2 > 0.f &&
                        (second.flags & trigger_t::flags_t::does_not_repeat_automatically) ==
                            trigger_t::flags_t::none;
                    if((repeats2 || !state.firing2) && state.cooldown2 <= 0.f)
                    {
                        if(rate2 > 0.f)
                            state.cooldown2 += 1.f / rate2;
                        state.fired        = true;
                        state.illumination = 1.f;
                        pull(p, ent.id(), info, cam, *weapon_tag, false, 1);
                    }
                }
                state.firing2 = held2;
                if(ammo)
                    ammo->charged = charges && state.charge >= trigger.charging_time;
                if(trigger.illumination_recovery_time > 0.f)
                    state.illumination = std::max(
                        state.illumination -
                            dt / trigger.illumination_recovery_time,
                        0.f);
                else if(!state.fired)
                    state.illumination = 0.f;
            }

            if(!vehicle)
            {
                equip->reloading = state.reloading > 0.f;
                drive_functions(
                    p, ent.id(), weapon.value()[0], state, held, magic, ammo,
                    charges ? trigger.charging_time : 0.f);
            }
        }
        std::erase_if(m_states, [&live](auto const& e) {
            return !live.contains(e.first);
        });

        hush(p);
        m_fire_queue->poll();
        for(auto const& shot : std::exchange(m_shots, {}))
            fire(p, shot, files);

        m_use_queue->poll();
        if(!p.template subsystem<NetworkState>().replica)
            pick_up(p, files);
    }

    /* The authority hands out what bodies walk over, local and remote alike:
     * ammo for what they carry, a weapon into a free slot, or a swap when
     * they press use */
    void pick_up(Proxy& p, BlamFiles<halo_version> const& files)
    {
        std::set<u64> taken;
        for(auto ent : p.template select<PlayerInfo, PlayerCamera, NetworkInfo, BipedEquipment>())
        {
            auto [info, cam, net, equip] = ent.components();
            bool const pressed           = std::exchange(m_use_pressed[ent.id()], false);
            /* Back from the dead, it is still where it fell for a moment:
             * what it dropped there is not its own again */
            auto& grace = m_revived[ent.id()];
            if(auto const* status = p.template get<BipedStatus>(ent.id()); status && status->dead)
                grace = revive_grace;
            else if(grace > 0)
                grace--;
            if(grace > 0)
                continue;
            if(info.riding.vehicle || !info.mode.physics || !biped_in_play(info, cam, net) ||
               equip.load_generation != files.load_generation)
                continue;
            for(auto item : p.template select<Model, NetworkInfo>())
            {
                auto [model, item_net] = item.components();
                auto const* tag        = model.origin_object;
                if(item_net.instance_id == 0 ||
                   SpawnObjectEvent::is_impostor(item_net.instance_id) || !tag ||
                   taken.contains(item.id()) || p.template get<PlayerInfo>(item.id()))
                    continue;
                /* A grenade of a kind carried, while there is room for it */
                bool const grenade = std::any_of(
                    equip.equipment.begin(), equip.equipment.end(), [tag](auto const& kind) {
                        return kind.tag == tag && kind.left < kind.most;
                    });
                bool const powerup = !grenade && wanted_powerup(p, ent.id(), *tag, files);
                if(!grenade && !powerup && !tag->matches(blam::tag_class_t::weap))
                    continue;
                if(!within_pickup_reach(info, cam, model.position))
                    continue;
                bool const carried = grenade || powerup || equip.weapons[0].tag == tag ||
                                     equip.weapons[1].tag == tag;
                if(!carried && equip.weapons[1].weapon && !pressed)
                    continue;
                taken.insert(item.id());
                auto&              game = p.template subsystem<GameEventBus>();
                DespawnObjectEvent gone{.net_id = item_net.instance_id};
                GameEvent          despawn{.type = GameEvent::DespawnObject};
                game.inject(despawn, &gone);
                EquipWeaponEvent equip_ev{
                    .entity_id = ent.id(),
                    .weapon    = tag->tag_id,
                    .mode      = EquipWeaponEvent::pick_up,
                    .age       = model.item_age,
                };
                GameEvent ev{.type = GameEvent::EquipWeapon};
                game.inject(ev, &equip_ev);
                break;
            }
        }
    }

    /* A powerup is taken unless it would do nothing: health at full health,
     * an over shield already full */
    static bool wanted_powerup(
        Proxy& p, u64 player, blam::tag_t const& tag, BlamFiles<halo_version> const& files)
    {
        using powerup_t = blam::scn::equipment::powerup_type_t;
        if(!tag.matches(blam::tag_class_t::eqip))
            return false;
        auto eqip = tag.template data<blam::scn::equipment>(files.container.magic);
        if(!eqip.has_value())
            return false;
        auto const* status = p.template get<BipedStatus>(player);
        switch(eqip.value()[0].powerup_type)
        {
        case powerup_t::health:
            return status && status->body < 1.f;
        case powerup_t::over_shield:
            return status && status->maximum_shield() > 0.f && status->shield < BipedStatus::over_shield;
        case powerup_t::double_speed:
        case powerup_t::active_camouflage:
        case powerup_t::full_spectrum_vision:
            return true;
        default:
            return false;
        }
    }

    static BlamFiles<halo_version> const& files_of(Proxy& p)
    {
        return p.template subsystem<BlamFiles<halo_version>>();
    }

    /* The weapon of the vehicle seat a gunner sits in */
    blam::tag_t const* mounted_weapon(
        Proxy& p, PlayerInfo const& info, BlamFiles<halo_version> const& files)
    {
        using flags_t      = blam::scn::unit::seat_t::seat_flags_t;
        auto const* model  = p.template get<Model>(info.riding.vehicle);
        auto const& magic  = files.container.magic;
        if(!model || !model->origin_object)
            return nullptr;
        auto unit = model->origin_object->template data<blam::scn::unit>(magic);
        if(!unit.has_value())
            return nullptr;
        auto seats   = unit.value()[0].seats.data(magic);
        auto weapons = unit.value()[0].weapons.data(magic);
        if(!seats.has_value() || !weapons.has_value() ||
           weapons.value().empty() || info.riding.seat < 0 ||
           static_cast<size_t>(info.riding.seat) >= seats.value().size() ||
           (seats.value()[info.riding.seat].flags & flags_t::gunner) ==
               flags_t::none)
            return nullptr;
        blam::tag_index_view<halo_version> index(files.container);
        auto weapon = index.find(weapons.value()[0].weapon.tag_id);
        return weapon != index.end() ? &(*weapon) : nullptr;
    }

    /* Raises the shot, from the eye or a vehicle's gun; every peer fires it
     * from the event */
    void pull(
        Proxy&              p,
        u64                 player,
        PlayerInfo const&   info,
        PlayerCamera const& cam,
        blam::tag_t const&  weapon,
        bool                melee   = false,
        u16                 trigger = 0)
    {
        Matf3 const     to_world = glm::transpose(Matf3(cam.rotation));
        Vecf3 const     forward  = to_world * Vecf3{0.f, 0.f, -1.f};
        WeaponFireEvent shot{
            .weapon     = weapon.as_ref(),
            .shooter    = player,
            .player_idx = info.player_idx,
            .seed       = static_cast<u32>(m_rng()),
            .trigger    = trigger,
            .melee      = melee,
            .position   = melee ? cam.camera.position
                                : cam.camera.position + forward * muzzle_distance,
            .direction  = forward,
        };
        GameEvent ev{.type = GameEvent::WeaponFire};
        /* What the reticle rests on: a unit there is what guided shots seek */
        Vecf3 target = cam.camera.position + forward * aim_reach;
        if(!melee)
        {
            Physics::Event   ray_ev{Physics::Event::Raycast};
            Physics::Raycast ray{
                .from   = cam.camera.position,
                .to     = target,
                .ignore = info.riding.vehicle ? info.riding.vehicle : player};
            p.template subsystem<PhysicsBus>().process(ray_ev, &ray);
            if(ray.hit && ray.entity_id != player)
            {
                target = ray.point;
                if(ray.entity_id && is_unit(p, ray.entity_id))
                    shot.target = ray.entity_id;
            }
            if(!shot.target)
                shot.target = autoaimed(p, player, info, cam.camera.position, forward, weapon);
        }
        if(info.riding.vehicle != 0 && &weapon == mounted_weapon(p, info, files_of(p)))
        {
            /* Every barrel fires at once, each at what the reticle rests
             * on, however the gun itself is posed */
            auto const* model = p.template get<Model>(info.riding.vehicle);
            u32 const   barrels =
                model && model->tag
                    ? std::max(marker_instances(*model->tag, gun_marker, files_of(p).container.magic), 1u)
                    : 1u;
            for(u32 i = 0; i < barrels; i++)
            {
                WeaponFireEvent barrel = shot;
                mount(p, barrel, info.riding.vehicle, i);
                Vecf3 const to = target - barrel.position;
                if(glm::dot(to, forward) > 0.f)
                    barrel.direction = glm::normalize(to);
                p.template subsystem<GameEventBus>().inject(ev, &barrel);
            }
            return;
        }
        p.template subsystem<GameEventBus>().inject(ev, &shot);
    }

    /* From the gun's marker as the vehicle stands, along its barrel; a gun
     * the gunner turns points where they look */
    void mount(Proxy& p, WeaponFireEvent& shot, u64 vehicle, u32 instance)
    {
        auto const* model = p.template get<Model>(vehicle);
        if(!model || !model->tag)
            return;
        auto marker = bind_marker_node(
            *model->tag,
            gun_marker,
            p.template subsystem<BlamFiles<halo_version>>().container.magic,
            instance);
        if(!marker)
            return;
        Matf4       at   = model->transform;
        auto const* anim = p.template get<AnimationPlayback>(vehicle);
        auto const  node = static_cast<size_t>(marker->node);
        if(anim && marker->node >= 0 && node < anim->pose.size())
            at = at * anim->pose[node];
        at                 = at * marker->transform;
        Vecf3 const barrel = Vecf3(at[0]);
        shot.vehicle       = vehicle;
        shot.position      = Vecf3(at[3]);
        if(glm::dot(barrel, barrel) > 1e-6f)
            shot.direction = glm::normalize(barrel);
    }

    /* Projectiles from the shot's origin, the firing effect at the muzzle */
    void fire(
        Proxy&                         p,
        WeaponFireEvent const&         shot,
        BlamFiles<halo_version> const& files)
    {
        auto&       game  = p.template subsystem<GameEventBus>();
        auto const& magic = files.container.magic;
        if(shot.thrown)
            return thrown(p, shot, files);
        if(shot.melee)
            return struck(p, shot, files);

        blam::tag_index_view<halo_version> index(files.container);
        auto tag = index.find(shot.weapon.tag_id);
        if(tag == index.end() || !(*tag).matches(blam::tag_class_t::weap))
            return;
        auto weapon = (*tag).template data<weapon_t>(magic);
        if(!weapon.has_value())
            return;
        auto triggers = weapon.value()[0].triggers.data(magic);
        if(!triggers.has_value() || shot.trigger >= triggers.value().size())
            return;
        trigger_t const& trigger = triggers.value()[shot.trigger];

        std::minstd_rand rng(shot.seed);
        Vecf3 const      forward = shot.direction;
        if(auto projectile = index.find(trigger.projectile.tag_id);
           trigger.projectile.valid() && projectile != index.end())
        {
            auto const* proj =
                (*projectile).template data<blam::scn::projectile>(magic).value();
            /* Only the authority decides what bullets hit */
            bool const resolve = Projectile::hitscan(*proj) &&
                                 !p.template subsystem<NetworkState>().replica;
            i32 const count = std::max<i32>(trigger.projectiles_per_shot, 1);
            f32 const error = std::max(trigger.error_angle.x, trigger.minimum_error);
            for(i32 i = 0; i < count; i++)
            {
                f32 const spread =
                    count > 1 ? std::max(error, trigger.distribution_angle * .5f) : error;
                Vecf3 const dir = cone(rng, forward, spread);
                if(resolve)
                    trace(p, shot, (*projectile).as_ref(), *proj, dir);
                SpawnObjectEvent spawn{
                    .object   = trigger.projectile,
                    .position = shot.position,
                    .rotation = glm::angleAxis(std::atan2(dir.y, dir.x), Vecf3{0, 0, 1}) *
                                glm::angleAxis(
                                    -std::asin(std::clamp(dir.z, -1.f, 1.f)),
                                    Vecf3{0, 1, 0}),
                    .local_only = true,
                    .owner      = shot.vehicle ? shot.vehicle : shot.shooter,
                    .shooter    = shot.shooter,
                    .target     = shot.target,
                };
                GameEvent ev{.type = GameEvent::SpawnObject};
                game.inject(ev, &spawn);
            }
        }

        /* Its own seat sees the first-person weapon fire, every other the one
         * in its hand; one of them makes the sound and light */
        u64 const first = shot.vehicle ? shot.vehicle : first_person(p, shot.shooter, true);
        u64 const third = shot.vehicle ? 0 : in_hand(p, shot.shooter, files);
        if(auto effects = trigger.firing_effects.data(magic);
           effects.has_value() && !effects.value().empty())
        {
            size_t pick = 0;
            if((trigger.flags & trigger_t::flags_t::random_firing_effects) !=
               trigger_t::flags_t::none)
                pick = std::uniform_int_distribution<size_t>(
                    0, effects.value().size() - 1)(rng);
            auto const& effect = effects.value()[pick].firing_effect;
            auto const play = [&](u64 parent, bool visuals_only) {
                SpawnEffectEvent spawn{
                    .effect       = effect,
                    .position     = shot.position,
                    .direction    = forward,
                    .parent       = parent,
                    .visuals_only = visuals_only,
                };
                GameEvent ev{.type = GameEvent::SpawnEffect};
                game.inject(ev, &spawn);
            };
            if(effect.valid())
            {
                if(first || !third)
                    play(first, false);
                if(third)
                    play(third, first != 0);
            }
        }

        /* The first-person hands and weapon kick together */
        for(auto ent : p.template select<Attachment, AnimationPlayback>())
        {
            if(shot.vehicle)
                break;
            if(ent.template get<Attachment>().parent != shot.shooter)
                continue;
            auto const& anim = ent.template get<AnimationPlayback>();
            i32 const   clip = fire_clip(anim, magic);
            if(clip < 0)
                continue;
            PlayModelAnimationEvent play{
                .entity    = ent.id(),
                .animation = clip,
                .fade      = 0.f,
            };
            GameEvent ev{.type = GameEvent::PlayModelAnimation};
            game.inject(ev, &play);
        }
    }

    /* Its seat sees the hands swing and every peer hears the blow land; the
     * authority finds what is in reach, a little either side of the aim,
     * and lands the weapon's melee jpt! */
    void struck(Proxy& p, WeaponFireEvent const& shot, BlamFiles<halo_version> const& files)
    {
        auto const& magic = files.container.magic;
        auto&       game  = p.template subsystem<GameEventBus>();
        for(auto ent : p.template select<Attachment, AnimationPlayback>())
        {
            if(ent.template get<Attachment>().parent != shot.shooter)
                continue;
            i32 const clip =
                fp_clip(ent.template get<AnimationPlayback>(), magic, fp_slot::melee);
            if(clip < 0)
                continue;
            PlayModelAnimationEvent play{.entity = ent.id(), .animation = clip, .fade = 0.f};
            GameEvent ev{.type = GameEvent::PlayModelAnimation};
            game.inject(ev, &play);
        }
        bool const replica = p.template subsystem<NetworkState>().replica;
        blam::tag_index_view<halo_version> index(files.container);
        auto tag = index.find(shot.weapon.tag_id);
        if(tag == index.end() || !(*tag).matches(blam::tag_class_t::weap))
            return;
        auto weapon = (*tag).template data<weapon_t>(magic);
        if(!weapon.has_value() || !weapon.value()[0].player_melee_damage.valid())
            return;
        auto jpt = index.template data<blam::scn::damage_effect>(weapon.value()[0].player_melee_damage);
        /* A body in reach before a wall, else the wall */
        std::optional<std::pair<Physics::Raycast, Vecf3>> struck_at;
        /* Level and a little low, for the short: grunts and jackals */
        Vecf3 const side = glm::cross(shot.direction, Vecf3{0.f, 0.f, 1.f});
        Vecf3 const right =
            glm::dot(side, side) > 1e-6f ? glm::normalize(side) : Vecf3{0.f, 1.f, 0.f};
        for(f32 const dip : {0.f, -melee_dip})
            for(f32 const turn : {0.f, melee_spread, -melee_spread})
        {
            Vecf3 const dir = glm::angleAxis(turn, Vecf3{0.f, 0.f, 1.f}) *
                              (glm::angleAxis(dip, right) * shot.direction);
            Physics::Event   ev{Physics::Event::Raycast};
            Physics::Raycast ray{
                .from   = shot.position,
                .to     = shot.position + dir * melee_reach,
                .ignore = shot.shooter,
                .rewind = replica ? std::nullopt : shot.view_tick,
            };
            p.template subsystem<PhysicsBus>().process(ev, &ray);
            if(ray.hit && (!struck_at || (ray.entity_id != 0 && struck_at->first.entity_id == 0)))
                struck_at = std::pair{ray, dir};
        }
        if(!struck_at)
            return;
        auto const& [ray, dir] = *struck_at;
        landed(p, weapon.value()[0], jpt.has_value() ? jpt.value() : nullptr, ray, files);
        if(replica || ray.entity_id == 0)
            return;
        auto const* target = p.template get<PlayerInfo>(ray.entity_id);
        ObjectHitEvent hit{
            .damage         = weapon.value()[0].player_melee_damage,
            .target         = ray.entity_id,
            .shooter        = shot.shooter,
            .target_player  = target ? target->player_idx : ObjectHitEvent::no_player,
            .shooter_player = shot.player_idx,
            .point          = ray.point,
            .normal         = ray.normal,
            .direction      = dir,
            .part           = ray.part,
            .head           = ray.head,
            .hitscan        = true,
            .behind         = from_behind(p, ray.entity_id, dir),
        };
        GameEvent hit_ev{.type = GameEvent::ObjectHit};
        game.inject(hit_ev, &hit);
    }

    /* A blow is heard twice: the weapon's thump, and the globals' melee
     * sound for the material struck */
    void landed(
        Proxy& p, weapon_t const& weapon, blam::scn::damage_effect const* jpt,
        Physics::Raycast const& ray, BlamFiles<halo_version> const& files)
    {
        auto const& magic = files.container.magic;
        if(auto* sounds = p.template service<comp_app::EventBus<SoundEvent>>();
           sounds && jpt && jpt->sound.valid())
        {
            SoundEvent     sound_ev{.type = SoundEvent::play_sound};
            PlaySoundEvent play{
                .sound    = &static_cast<blam::tagref_t const&>(jpt->sound),
                .position = ray.point,
            };
            sounds->inject(sound_ev, &play);
        }
        blam::tag_index_view<halo_version> index(files.container);
        auto globals_tag = index.tag_of("globals\\globals");
        if(!globals_tag.has_value())
            return;
        auto globals = (*globals_tag)->template data<blam::globals::globals>(magic);
        if(!globals.has_value())
            return;
        auto materials = globals.value()->materials.data(magic);
        auto const material = static_cast<size_t>(struck_material(p, ray, files));
        auto*      sounds   = p.template service<comp_app::EventBus<SoundEvent>>();
        if(!sounds || !materials.has_value() || material >= materials.value().size() ||
           !materials.value()[material].melee_hit_sound.valid())
            return;
        SoundEvent     sound_ev{.type = SoundEvent::play_sound};
        PlaySoundEvent play{
            .sound    = &static_cast<blam::tagref_t const&>(materials.value()[material].melee_hit_sound),
            .position = ray.point,
        };
        sounds->inject(sound_ev, &play);
    }

    /* Struck along where it faces, i.e. in the back */
    bool from_behind(Proxy& p, u64 target, Vecf3 const& direction)
    {
        Vecf3 facing{};
        if(auto const* cam = p.template get<PlayerCamera>(target))
            facing = glm::transpose(Matf3(cam->rotation)) * Vecf3{0.f, 0.f, -1.f};
        else if(auto const* model = p.template get<Model>(target))
            facing = model->rotation * Vecf3{1.f, 0.f, 0.f};
        Vecf2 const a{facing}, b{direction};
        if(glm::length(a) < 1e-3f || glm::length(b) < 1e-3f)
            return false;
        return glm::dot(glm::normalize(a), glm::normalize(b)) > behind_cos;
    }

    /* A bullet's flight as one ray, against bipeds as the shooter saw them */
    void trace(
        Proxy&                       p,
        WeaponFireEvent const&       shot,
        blam::tagref_t const&        projectile,
        blam::scn::projectile const& proj,
        Vecf3 const&                 direction)
    {
        Physics::Event   ev{Physics::Event::Raycast};
        Physics::Raycast ray{
            .from   = shot.position,
            .to     = shot.position + direction * Projectile::reach(proj),
            .ignore = shot.vehicle ? shot.vehicle : shot.shooter,
            .rewind = shot.view_tick,
        };
        p.template subsystem<PhysicsBus>().process(ev, &ray);
        if(!ray.hit || ray.entity_id == 0)
            return;
        auto const*    target = p.template get<PlayerInfo>(ray.entity_id);
        ObjectHitEvent hit{
            .projectile     = projectile,
            .damage         = proj.impact_damage,
            .target         = ray.entity_id,
            .shooter        = shot.shooter,
            .target_player  = target ? target->player_idx : ObjectHitEvent::no_player,
            .shooter_player = shot.player_idx,
            .point          = ray.point,
            .normal         = ray.normal,
            .direction      = direction,
            .part           = ray.part,
            .head           = ray.head,
            .hitscan        = true,
        };
        GameEvent hit_ev{.type = GameEvent::ObjectHit};
        p.template subsystem<GameEventBus>().inject(hit_ev, &hit);
    }

    using fp_slot = blam::antr::first_person_weapon;

    static i32 fp_clip(AnimationPlayback const& anim, blam::map_ptr const& magic, size_t slot)
    {
        if(!anim.graph)
            return -1;
        auto fp = anim.graph->first_person_weapons.data(magic);
        if(!fp.has_value() || fp.value().empty())
            return -1;
        auto slots = fp.value()[0].animations.data(magic);
        if(!slots.has_value() || slots.value().size() <= slot)
            return -1;
        return slots.value()[slot].animation;
    }

    static i32 fire_clip(AnimationPlayback const& anim, blam::map_ptr const& magic)
    {
        return fp_clip(anim, magic, fp_slot::fire_1);
    }

    /* The player's first-person weapon model, or its hands */
    u64 first_person(Proxy& p, u64 player, bool weapon)
    {
        for(auto ent : p.template select<Attachment, Model>())
        {
            if(ent.template get<Attachment>().parent != player)
                continue;
            if(weapon == (ent.template get<Model>().origin_object != nullptr))
                return ent.id();
        }
        return 0;
    }

    static weapon_t::magazine_t const* magazine_of(
        weapon_t const& weapon, trigger_t const& trigger, blam::map_ptr const& magic)
    {
        auto magazines = weapon.magazines.data(magic);
        if(trigger.magazine < 0 || !magazines.has_value() ||
           static_cast<size_t>(trigger.magazine) >= magazines.value().size())
            return nullptr;
        /* Energy weapons name one that loads no rounds: they fire on charge */
        auto const& magazine = magazines.value()[trigger.magazine];
        return magazine.rounds_loaded_maximum > 0 ? &magazine : nullptr;
    }

    /* Heat bleeds off; a vented weapon fires again once cool enough */
    static void cool(BipedEquipment::weapon_t& gun, weapon_t const& tag, f32 dt)
    {
        gun.heat = std::max(gun.heat - tag.heat_loss_rate * dt, 0.f);
        if(gun.overheated && gun.heat <= tag.heat_recovery_threshold)
            gun.overheated = false;
    }

    /* Venting, or a battery run flat where that stops it */
    static bool spent(BipedEquipment::weapon_t const& gun, weapon_t const& tag)
    {
        using flags_t = weapon_t::weapon_flags_t;
        return gun.overheated ||
               ((tag.weapon_flags & flags_t::cannot_fire_at_maximum_age) != flags_t::none &&
                gun.age >= 1.f);
    }

    /* A shot warms the weapon and drains its battery; too hot, it vents */
    void heat_up(
        Proxy& p, u64 player, BipedEquipment::weapon_t& gun, weapon_t const& tag,
        trigger_t const& pulled)
    {
        gun.heat += pulled.heat_generated_per_round;
        gun.age = std::min(gun.age + pulled.age_generated_per_round, 1.f);
        if(tag.overheated_threshold <= 0.f || gun.heat < tag.overheated_threshold ||
           gun.overheated)
            return;
        gun.overheated = true;
        if(!tag.overheated.valid())
            return;
        auto const* cam = p.template get<PlayerCamera>(player);
        SpawnEffectEvent vent{
            .effect    = tag.overheated,
            .position  = cam ? cam->camera.position : Vecf3{},
            .direction = cam ? glm::transpose(Matf3(cam->rotation)) * Vecf3{0.f, 0.f, -1.f}
                             : Vecf3{1.f, 0.f, 0.f},
            .parent    = first_person(p, player, true),
        };
        GameEvent ev{.type = GameEvent::SpawnEffect};
        p.template subsystem<GameEventBus>().inject(ev, &vent);
    }

    /* Tops the magazine up from what is carried */
    static void refill(BipedEquipment::weapon_t& ammo, weapon_t::magazine_t const& magazine)
    {
        u16 const room = static_cast<u16>(
            std::max<i32>(magazine.rounds_loaded_maximum - ammo.rounds_loaded, 0));
        u16 const taken = std::min(room, ammo.rounds_total);
        ammo.rounds_loaded += taken;
        ammo.rounds_total -= taken;
    }

    /* ---- Grenades ---- */

    struct throw_t
    {
        f32 windup{-1.f}; /* seconds to the release, negative when not throwing */
        f32 recover{0.f}; /* seconds until the hand is free again */
        blam::tagref_t const* projectile{nullptr};
    };

    struct grenade_t
    {
        blam::scn::equipment const* equipment{nullptr};
        blam::tagref_t const*       projectile{nullptr};
        blam::tagref_t const*       throwing_effect{nullptr};
    };

    void load_grenades(BlamFiles<halo_version> const& files)
    {
        m_grenades.clear();
        auto const&                        magic = files.container.magic;
        blam::tag_index_view<halo_version> index(files.container);
        auto globals_tag = index.tag_of("globals\\globals");
        if(!globals_tag.has_value())
            return;
        auto globals = (*globals_tag)->template data<blam::globals::globals>(magic);
        if(!globals.has_value())
            return;
        auto grenades = globals.value()->grenades.data(magic);
        if(!grenades.has_value())
            return;
        for(auto const& grenade : grenades.value())
        {
            auto tag = index.find(grenade.equipment.tag_id);
            if(!grenade.equipment.valid() || tag == index.end())
                continue;
            auto eqip = (*tag).template data<blam::scn::equipment>(magic);
            if(!eqip.has_value())
                continue;
            m_grenades.push_back({
                .equipment       = eqip.value(),
                .projectile      = &static_cast<blam::tagref_t const&>(grenade.projectile),
                .throwing_effect = &static_cast<blam::tagref_t const&>(grenade.throwing_effect),
            });
        }
    }

    grenade_t const* grenade_of(blam::scn::equipment const* equipment) const
    {
        for(auto const& grenade : m_grenades)
            if(grenade.equipment == equipment)
                return &grenade;
        return nullptr;
    }

    /* Switches type, starts a throw and lets go at the animation's key
     * frame; true while the hand is busy */
    bool grenade(
        Proxy&                         p,
        u64                            player,
        PlayerInfo const&              info,
        PlayerCamera const&            cam,
        BipedEquipment&                equip,
        std::vector<TickEvent> const&  ticks,
        bool                           toss,
        bool                           cycle)
    {
        auto& state = m_throws[player];
        auto& kinds = equip.equipment;
        /* Once the map's globals can be read */
        if(m_grenades.empty())
            load_grenades(p.template subsystem<BlamFiles<halo_version>>());
        if(cycle && !kinds.empty() && state.windup < 0.f)
            /* The next type there are any of */
            for(size_t i = 1; i <= kinds.size(); i++)
                if(kinds[(equip.grenade + i) % kinds.size()].left > 0)
                {
                    equip.grenade = static_cast<u16>((equip.grenade + i) % kinds.size());
                    break;
                }
        if(toss && state.windup < 0.f && state.recover <= 0.f && !equip.reloading &&
           equip.grenade < kinds.size() && kinds[equip.grenade].left > 0)
            if(auto const* type = grenade_of(kinds[equip.grenade].equip))
            {
                kinds[equip.grenade].left--;
                state.projectile = type->projectile;
                state.windup     = release_time(p, player);
                state.recover    = state.windup + throw_recovery;
                equip.throwing   = true;
            }
        for(auto const& tick : ticks)
        {
            state.recover = std::max(state.recover - tick.seconds, 0.f);
            if(state.windup < 0.f || (state.windup -= tick.seconds) > 0.f)
                continue;
            state.windup = -1.f;
            if(state.projectile)
                toss_grenade(p, player, info, cam, equip, *state.projectile);
        }
        equip.throwing = state.windup >= 0.f || state.recover > 0.f;
        return equip.throwing;
    }

    /* A grenade leaving the hand, at the thrower's grenade velocity */
    void thrown(Proxy& p, WeaponFireEvent const& shot, BlamFiles<halo_version> const& files)
    {
        auto&       game  = p.template subsystem<GameEventBus>();
        auto const* equip = p.template get<BipedEquipment>(shot.shooter);
        f32 const   speed = equip && equip->biped && equip->biped->grenade_velocity > 0.f
                                ? equip->biped->grenade_velocity
                                : default_throw_speed;
        Vecf3 const dir   = shot.direction;
        SpawnObjectEvent spawn{
            .object   = shot.weapon,
            .position = shot.position,
            .rotation = glm::angleAxis(std::atan2(dir.y, dir.x), Vecf3{0, 0, 1}) *
                        glm::angleAxis(-std::asin(std::clamp(dir.z, -1.f, 1.f)), Vecf3{0, 1, 0}),
            .local_only = true,
            .owner      = shot.shooter,
            .shooter    = shot.shooter,
            .velocity   = dir * speed,
        };
        GameEvent ev{.type = GameEvent::SpawnObject};
        game.inject(ev, &spawn);

        for(auto const& grenade : m_grenades)
            if(grenade.projectile->tag_id == shot.weapon.tag_id && grenade.throwing_effect->valid())
            {
                u64 const first = first_person(p, shot.shooter, false);
                u64 const third = in_hand(p, shot.shooter, files);
                auto const play = [&](u64 parent, bool visuals_only) {
                    SpawnEffectEvent effect{
                        .effect       = *grenade.throwing_effect,
                        .position     = shot.position,
                        .direction    = dir,
                        .parent       = parent,
                        .visuals_only = visuals_only,
                    };
                    GameEvent fx{.type = GameEvent::SpawnEffect};
                    game.inject(fx, &effect);
                };
                if(first || !third)
                    play(first, false);
                if(third)
                    play(third, first != 0);
                break;
            }
    }

    /* When the first-person hands' throw lets go */
    f32 release_time(Proxy& p, u64 player)
    {
        using fp_slot     = blam::antr::first_person_weapon;
        auto const& magic = p.template subsystem<BlamFiles<halo_version>>().container.magic;
        for(auto ent : p.template select<Attachment, AnimationPlayback>())
        {
            auto const& anim = ent.template get<AnimationPlayback>();
            if(ent.template get<Attachment>().parent != player || !anim.graph)
                continue;
            auto fp    = anim.graph->first_person_weapons.data(magic);
            auto clips = anim.graph->animations.data(magic);
            if(!fp.has_value() || fp.value().empty() || !clips.has_value())
                continue;
            auto slots = fp.value()[0].animations.data(magic);
            if(!slots.has_value() || slots.value().size() <= fp_slot::throw_grenade)
                continue;
            i32 const clip = slots.value()[fp_slot::throw_grenade].animation;
            if(clip >= 0 && static_cast<size_t>(clip) < clips.value().size() &&
               clips.value()[clip].key_frame > 0)
                return clips.value()[clip].key_frame / animation_rate;
        }
        return default_release;
    }

    /* Out of the eye along the view, every peer flying its own */
    void toss_grenade(
        Proxy&                p,
        u64                   player,
        PlayerInfo const&     info,
        PlayerCamera const&   cam,
        BipedEquipment const& equip,
        blam::tagref_t const& projectile)
    {
        Vecf3 const forward =
            glm::transpose(Matf3(cam.rotation)) * Vecf3{0.f, 0.f, -1.f};
        WeaponFireEvent shot{
            .weapon     = projectile,
            .shooter    = player,
            .player_idx = info.player_idx,
            .seed       = static_cast<u32>(m_rng()),
            .thrown     = true,
            .position   = cam.camera.position + forward * muzzle_distance,
            .direction  = forward,
        };
        GameEvent ev{.type = GameEvent::WeaponFire};
        p.template subsystem<GameEventBus>().inject(ev, &shot);
    }

    /* The living unit nearest the reticle within the weapon's autoaim
     * cone and range, and in sight */
    u64 autoaimed(
        Proxy&             p,
        u64                player,
        PlayerInfo const&  info,
        Vecf3 const&       eye,
        Vecf3 const&       forward,
        blam::tag_t const& weapon)
    {
        auto data = weapon.template data<weapon_t>(files_of(p).container.magic);
        if(!data.has_value())
            return 0;
        f32 const angle = data.value()[0].autoaim_angle;
        f32 const range = data.value()[0].autoaim_range;
        if(angle <= 0.f || range <= 0.f)
            return 0;
        u64 best      = 0;
        f32 best_cos  = std::cos(angle);
        for(auto unit : p.template select<Model, BipedStatus>())
        {
            u64 const id = unit.id();
            if(id == player || id == info.riding.vehicle ||
               unit.template get<BipedStatus>().dead)
                continue;
            Vecf3 const to = unit.template get<Model>().position + Vecf3{0.f, 0.f, autoaim_height} - eye;
            f32 const   d  = glm::length(to);
            if(d < 1e-3f || d > range)
                continue;
            f32 const c = glm::dot(to / d, forward);
            if(c <= best_cos)
                continue;
            Physics::Event   ray_ev{Physics::Event::Raycast};
            Physics::Raycast ray{.from = eye, .to = eye + to, .ignore = player};
            p.template subsystem<PhysicsBus>().process(ray_ev, &ray);
            if(ray.hit && ray.entity_id != id)
                continue;
            best     = id;
            best_cos = c;
        }
        return best;
    }

    static bool is_unit(Proxy& p, u64 id)
    {
        if(p.template get<PlayerInfo>(id))
            return true;
        auto const* physics = p.template get<ObjectPhysics>(id);
        return physics && (physics->upright || physics->drive);
    }

    /* The shooter's weapon as others see it */
    u64 in_hand(Proxy& p, u64 shooter, BlamFiles<halo_version> const& files)
    {
        auto const* equip = p.template get<BipedEquipment>(shooter);
        auto const* held  = equip ? equip->held(files.load_generation) : nullptr;
        return held && p.template get<Model>(held->held) ? held->held : 0;
    }

    /* The weapon's export inputs, through its object functions, onto its
     * models: what turns its muzzle flash on, the needles it has left */
    void drive_functions(
        Proxy&               p,
        u64                  player,
        weapon_t const&      weapon,
        state_t const&       state,
        bool                 held,
        blam::map_ptr const& magic,
        BipedEquipment::weapon_t const* gun,
        f32                  charging_time)
    {
        u64 const id = first_person(p, player, true);
        if(id == 0)
            return;
        auto* model = p.template get<Model>(id);

        f32 const charged =
            charging_time > 0.f && state.charge >= 0.f ? std::min(state.charge / charging_time, 1.f) : 0.f;
        /* What is loaded, as a share of the magazine */
        auto        triggers = weapon.triggers.data(magic);
        auto const* magazine = gun && triggers.has_value() && !triggers.value().empty()
                                   ? magazine_of(weapon, triggers.value()[0], magic)
                                   : nullptr;
        f32 const loaded = magazine && magazine->rounds_loaded_maximum > 0
                               ? std::min(
                                     static_cast<f32>(gun->rounds_loaded) /
                                         magazine->rounds_loaded_maximum,
                                     1.f)
                               : 1.f;
        std::array<f32, 4> in{};
        for(size_t i = 0; i < in.size(); i++)
            in[i] = input_value(weapon.weapon_inputs[i], state, held, gun, charged, loaded);

        std::array<f32, 4> out = in;
        auto functions         = weapon.functions.data(magic);
        if(functions.has_value())
            for(size_t i = 0; i < out.size() && i < functions.value().size(); i++)
                out[i] = blam_functions::evaluate(functions.value()[i], in, out);
        model->object_function = out;
        if(auto* held = gun && gun->held ? p.template get<Model>(gun->held) : nullptr)
            held->object_function = out;
        hum(p, player, id, *model, out, magic);
    }

    /* Looping sounds on the weapon in hand, each as loud as its function,
     * such as a charging hum */
    void hum(
        Proxy& p, u64 player, u64 fp_model, Model const& model, std::array<f32, 4> const& out,
        blam::map_ptr const& magic)
    {
        auto* sounds = p.template service<comp_app::EventBus<SoundEvent>>();
        auto const* cam = p.template get<PlayerCamera>(player);
        if(!sounds || !cam || !model.origin_object)
            return;
        auto object = model.origin_object->template data<blam::scn::object>(magic);
        if(!object.has_value())
            return;
        auto attachments = object.value()[0].attachments.data(magic);
        auto& loops       = m_hums[fp_model];
        for(size_t j = 0; attachments.has_value() && j < attachments.value().size() && j < 16; j++)
        {
            auto const& a     = attachments.value()[j];
            auto const& sound = static_cast<blam::tagref_t const&>(a.type);
            if(!sound.matches(blam::tag_class_t::lsnd))
                continue;
            u64 const loop  = hum_base | (fp_model << 4) | j;
            auto const scale = static_cast<size_t>(a.primary_scale);
            f32 const gain  = scale == 0 ? 1.f : scale <= out.size() ? out[scale - 1] : 0.f;
            if(!loops.contains(loop))
            {
                SoundEvent     ev{.type = SoundEvent::play_sound, .entity_id = loop};
                PlaySoundEvent play{.sound = &sound, .position = cam->camera.position, .looping = true};
                sounds->inject(ev, &play);
                loops.insert(loop);
            }
            SoundEvent       ev{.type = SoundEvent::update_sound, .entity_id = loop};
            UpdateSoundEvent update{.position = cam->camera.position, .gain = std::clamp(gain, 0.f, 1.f)};
            sounds->inject(ev, &update);
        }
    }

    /* A weapon put away or dropped takes its hum with it */
    void hush(Proxy& p)
    {
        auto* sounds = p.template service<comp_app::EventBus<SoundEvent>>();
        for(auto it = m_hums.begin(); it != m_hums.end();)
        {
            if(p.template get<Model>(it->first))
            {
                ++it;
                continue;
            }
            for(u64 loop : it->second)
                if(sounds)
                {
                    SoundEvent stop{.type = SoundEvent::stop_sound, .entity_id = loop};
                    sounds->inject(stop, nullptr);
                }
            it = m_hums.erase(it);
        }
    }

    static f32 input_value(
        input_t input, state_t const& state, bool held, BipedEquipment::weapon_t const* gun,
        f32 charged, f32 loaded)
    {
        switch(input)
        {
        case input_t::heat:
            return gun ? std::min(gun->heat, 1.f) : 0.f;
        case input_t::overheated:
            return gun && gun->overheated ? 1.f : 0.f;
        case input_t::age:
            return gun ? gun->age : 0.f;
        case input_t::primary_charged:
            return charged;
        case input_t::illumination:
        case input_t::primary_ejection_port:
            return state.illumination;
        case input_t::primary_firing:
        case input_t::primary_firing_on:
            return state.fired ? 1.f : 0.f;
        case input_t::primary_rate_of_fire:
            return held ? 1.f : 0.f;
        case input_t::primary_ammunition:
        case input_t::secondary_ammunition:
            return loaded;
        case input_t::ready:
            return 1.f;
        default:
            return 0.f;
        }
    }

    static Vecf3 cone(std::minstd_rand& rng, Vecf3 const& dir, f32 angle)
    {
        if(angle <= 0.f)
            return dir;
        std::uniform_real_distribution<f32> unit(0.f, 1.f);
        f32 const   z   = glm::mix(std::cos(angle), 1.f, unit(rng));
        f32 const   phi = unit(rng) * glm::two_pi<f32>();
        f32 const   r   = std::sqrt(std::max(0.f, 1.f - z * z));
        Vecf3 const up  = std::abs(dir.z) < .99f ? Vecf3{0, 0, 1} : Vecf3{1, 0, 0};
        Vecf3 const x   = glm::normalize(glm::cross(up, dir));
        Vecf3 const y   = glm::cross(dir, x);
        return x * (r * std::cos(phi)) + y * (r * std::sin(phi)) + dir * z;
    }

    /* Antr frames per second, and throws without a key frame */
    static constexpr f32 animation_rate  = 30.f;
    static constexpr f32 default_release     = .3f;
    static constexpr f32 default_throw_speed = 6.f; /* wu/s */
    /* Hand busy after letting go */
    static constexpr f32 throw_recovery = .4f;

    /* Ahead of the eye, clear of the body that fires */
    static constexpr f32 muzzle_distance = WeaponFireEvent::muzzle_distance;
    static constexpr f32 melee_time      = .8f;  /* s, a strike to the next */
    static constexpr f32 melee_reach     = 1.2f; /* wu, from the eye */
    static constexpr f32 melee_spread    = .15f; /* rad, the side rays */
    static constexpr f32 melee_dip       = .35f; /* rad, the low rays */
    static constexpr f32 aim_reach       = 100.f; /* wu, a reticle on nothing */
    static constexpr f32 autoaim_height  = .6f;   /* wu above a unit's origin */
    static constexpr f32 behind_cos      = .5f;  /* within 60 deg of its facing */
    /* Vehicle guns: where they fire from, and how far the crosshair looks */
    static constexpr std::string_view gun_marker = "primary trigger";

    u32                       m_generation{0};
    std::map<u64, state_t>    m_states;
    std::map<u64, throw_t>    m_throws;
    std::vector<grenade_t>    m_grenades;
    std::minstd_rand          m_rng{std::random_device{}()};

    std::shared_ptr<GameEventBus::queue_type<WeaponFireEvent>> m_fire_queue;
    /* Use presses per player, from every seat's and client's tick input */
    std::shared_ptr<GameEventBus::queue_type<PlayerTickInputEvent>> m_use_queue;
    std::map<u64, bool> m_using, m_use_pressed;
    std::map<u64, u32>  m_revived; /*!< Frames until it may pick things up */
    static constexpr u32 revive_grace = 30;
    std::unique_ptr<TickQueue>                                 m_ticks;
    std::vector<WeaponFireEvent>                               m_shots;
    std::map<u64, std::set<u64>>                               m_hums; /*!< By fp model */
    static constexpr u64 hum_base = 0x48554d0000000000ULL; /* 'HUM' */

    friend void alloc_weapons(compo::EntityContainer& e);
};

void alloc_weapons(compo::EntityContainer& e)
{
    auto& weapons   = e.register_subsystem_inplace<WeaponSystem>();
    weapons.m_ticks = std::make_unique<TickQueue>(e.subsystem_cast<GameEventBus>());
    weapons.m_fire_queue =
        e.subsystem_cast<GameEventBus>().addQueuedEventFunction<WeaponFireEvent>(
            0, [&weapons](GameEvent&, WeaponFireEvent* shot) {
                weapons.m_shots.push_back(*shot);
            });
    weapons.m_use_queue =
        e.subsystem_cast<GameEventBus>().addQueuedEventFunction<PlayerTickInputEvent>(
            0, [&weapons](GameEvent&, PlayerTickInputEvent* input) {
                bool const use = (input->intents & InputIntent::use) != InputIntent::none;
                if(!std::exchange(weapons.m_using[input->player], use) && use)
                    weapons.m_use_pressed[input->player] = true;
            });
}
