#include "weapons.h"

#include "blam_files.h"
#include "components.h"
#include "data.h"
#include "model_markers.h"
#include "network/networking.h"
#include "physics.h"
#include "selected_version.h"
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
        Model>,
    type_list_t<
        GameEventBus,
        PhysicsBus,
        const NetworkState,
        const BlamFiles<halo_version>>,
    empty_list_t>;

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
        f32  illumination{0.f}; /* the flash, 1 as it fires */
        u32  shots{0};          /* alternates twin barrels */
        bool firing{false};     /* held last frame */
        bool fired{false};      /* shot this frame */
        f32  reloading{0.f};    /* seconds left */
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
            /* A gunner fires the vehicle's weapon, a body its own; the free
             * camera has nothing to shoot with */
            u64 const vehicle = info.riding.vehicle;
            auto*     equip   = p.template get<BipedEquipment>(ent.id());
            auto const* carried =
                equip ? equip->held(files.load_generation) : nullptr;
            auto const* weapon_tag = vehicle   ? mounted_weapon(p, info, files)
                                     : carried ? carried->tag
                                               : nullptr;
            if(info.is_remote() || !weapon_tag ||
               (vehicle ? info.riding.exiting
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
                !vehicle && grenade(p, ent.id(), info, cam, *equip, ticks, toss, cycle);
            bool const held = input.intent.fire && !throwing;

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

            bool const repeats =
                (trigger.flags & trigger_t::flags_t::does_not_repeat_automatically) ==
                trigger_t::flags_t::none;
            /* The trigger is looked at once a tick */
            for(auto const& tick : ticks)
            {
                f32 const dt   = tick.seconds;
                state.fired    = false;
                state.cooldown = std::max(state.cooldown - dt, 0.f);
                if(state.reloading > 0.f &&
                   (state.reloading -= dt) <= 0.f && magazine)
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
                if(held && loaded && state.reloading <= 0.f &&
                   state.cooldown <= 0.f && (repeats || !state.firing))
                {
                    f32 const rate = std::max(
                        trigger.maximum_rate_of_fire.y > 0.f
                            ? trigger.maximum_rate_of_fire.y
                            : trigger.maximum_rate_of_fire.x,
                        .5f);
                    state.cooldown += 1.f / rate;
                    state.fired        = true;
                    state.illumination = 1.f;
                    if(magazine)
                        ammo->rounds_loaded -= per_shot;
                    pull(p, ent.id(), info, cam, *weapon_tag, state.shots++);
                }
                state.firing = held;
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
                    p, ent.id(), weapon.value()[0], state, held, magic);
            }
        }
        std::erase_if(m_states, [&live](auto const& e) {
            return !live.contains(e.first);
        });

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
               !equip.held(files.load_generation))
                continue;
            Vecf3 const body =
                cam.camera.position - Vecf3{0.f, 0.f, info.biped.eye_height * .5f};
            for(auto item : p.template select<Model, NetworkInfo>())
            {
                auto [model, item_net] = item.components();
                auto const* tag        = model.origin_object;
                if(item_net.instance_id == 0 ||
                   SpawnObjectEvent::is_impostor(item_net.instance_id) ||
                   !tag || !tag->matches(blam::tag_class_t::weap) ||
                   taken.contains(item.id()) || p.template get<PlayerInfo>(item.id()))
                    continue;
                /* Items rest against the body, below its middle */
                Vecf3 const to = model.position - body;
                if(glm::length(Vecf2(to)) > pickup_reach ||
                   std::abs(to.z) > info.biped.height)
                    continue;
                bool const carried =
                    equip.weapons[0].tag == tag || equip.weapons[1].tag == tag;
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
                };
                GameEvent ev{.type = GameEvent::EquipWeapon};
                game.inject(ev, &equip_ev);
                break;
            }
        }
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
        u32                 shot_index)
    {
        Matf3 const     to_world = glm::transpose(Matf3(cam.rotation));
        Vecf3 const     forward  = to_world * Vecf3{0.f, 0.f, -1.f};
        WeaponFireEvent shot{
            .weapon     = weapon.as_ref(),
            .shooter    = player,
            .player_idx = info.player_idx,
            .seed       = static_cast<u32>(m_rng()),
            .position   = cam.camera.position + forward * muzzle_distance,
            .direction  = forward,
        };
        if(info.riding.vehicle != 0)
            mount(p, shot, info.riding.vehicle, shot_index);
        GameEvent ev{.type = GameEvent::WeaponFire};
        p.template subsystem<GameEventBus>().inject(ev, &shot);
    }

    /* From the gun's marker as the vehicle stands, along its barrel; a gun
     * the gunner turns points where they look */
    void mount(Proxy& p, WeaponFireEvent& shot, u64 vehicle, u32 shot_index)
    {
        auto const* model = p.template get<Model>(vehicle);
        if(!model || !model->tag)
            return;
        auto marker = bind_marker_node(
            *model->tag,
            gun_marker,
            p.template subsystem<BlamFiles<halo_version>>().container.magic,
            shot_index);
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

    static i32 fire_clip(AnimationPlayback const& anim, blam::map_ptr const& magic)
    {
        if(!anim.graph)
            return -1;
        auto fp = anim.graph->first_person_weapons.data(magic);
        if(!fp.has_value() || fp.value().empty())
            return -1;
        auto slots = fp.value()[0].animations.data(magic);
        using fp_slot = blam::antr::first_person_weapon;
        if(!slots.has_value() || slots.value().size() <= fp_slot::fire_1)
            return -1;
        return slots.value()[fp_slot::fire_1].animation;
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
        return &magazines.value()[trigger.magazine];
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

    /* The shooter's weapon as others see it */
    u64 in_hand(Proxy& p, u64 shooter, BlamFiles<halo_version> const& files)
    {
        auto const* equip = p.template get<BipedEquipment>(shooter);
        auto const* held  = equip ? equip->held(files.load_generation) : nullptr;
        return held && p.template get<Model>(held->held) ? held->held : 0;
    }

    /* The weapon's export inputs, through its object functions, onto the
     * first-person model: what turns its muzzle flash on */
    void drive_functions(
        Proxy&               p,
        u64                  player,
        weapon_t const&      weapon,
        state_t const&       state,
        bool                 held,
        blam::map_ptr const& magic)
    {
        u64 const id = first_person(p, player, true);
        if(id == 0)
            return;
        auto* model = p.template get<Model>(id);

        std::array<f32, 4> in{};
        for(size_t i = 0; i < in.size(); i++)
            in[i] = input_value(weapon.weapon_inputs[i], state, held);

        std::array<f32, 4> out = in;
        auto functions         = weapon.functions.data(magic);
        if(functions.has_value())
            for(size_t i = 0; i < out.size() && i < functions.value().size(); i++)
                out[i] = evaluate(functions.value()[i], in, out);
        model->object_function = out;
    }

    static f32 input_value(input_t input, state_t const& state, bool held)
    {
        switch(input)
        {
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
        case input_t::ready:
            return 1.f;
        default:
            return 0.f;
        }
    }

    /* Constant base, scaled and offset by the inputs it names */
    static f32 evaluate(
        blam::scn::object::function_t const& fn,
        std::array<f32, 4> const&            in,
        std::array<f32, 4> const&            out)
    {
        using src = blam::shader::param_src;
        auto value_of = [&](src s) -> std::optional<f32> {
            auto const i = static_cast<u32>(s);
            if(i >= 1 && i <= 4)
                return in[i - 1];
            if(i >= 5 && i <= 8)
                return out[i - 5];
            return std::nullopt;
        };
        f32 v = fn.function == blam::shader::animation_function::zero ? 0.f : 1.f;
        if(auto s = value_of(fn.scale_function_by))
            v *= *s;
        if(auto s = value_of(fn.add))
            v += *s;
        if(auto s = value_of(fn.scale_result_by))
            v *= *s;
        return std::clamp(v, 0.f, 1.f);
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

    /* Across the ground from the middle of the body */
    static constexpr f32 pickup_reach = .8f;

    /* Ahead of the eye, clear of the body that fires */
    static constexpr f32 muzzle_distance = .4f;
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
