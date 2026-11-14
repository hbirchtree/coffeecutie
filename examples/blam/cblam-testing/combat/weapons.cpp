#include "combat/weapons.h"

#include "loading/blam_files.h"
#include "components.h"
#include "game/ctf.h"
#include "data.h"
#include "animation/model_markers.h"
#include "world/object_functions.h"
#include "network/networking.h"
#include "physics.h"
#include "combat/projectiles.h"
#include "selected_version.h"
#include "audio/sounds.h"
#include "ticks.h"
#include "world/game_maps.h"

#include <blam/volta/blam_antr.h>
#include <blam/volta/blam_globals.h>
#include <blam/volta/blam_unit.h>
#include <blam/volta/blam_weapon_firing.h>
#if defined(BLAM_DIMETER)
#include <blam/dimeter/firing.h>
#endif
#if defined(BLAM_ELEGY)
#include <blam/elegy/firing.h>
#endif

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
    world::with_game_maps_t<GameEventBus, PhysicsBus, const NetworkState, const LoadingStatus>,
    type_list_t<comp_app::EventBus<SoundEvent>>>;

struct WeaponSystem : compo::RestrictedSubsystem<WeaponSystem, WeaponManifest>
{
    using type  = WeaponSystem;
    using Proxy = compo::proxy_of<WeaponManifest>;

    using weapon_t  = blam::scn::weapon;
    using trigger_t = weapon_t::trigger_t;
    using input_t   = weapon_t::weapon_function_in_t;
    using weapon_id = GameTag<blam::tag_class_t::weap>;

    /* A body's presses this frame */
    struct intent_t
    {
        bool swap{false};
        bool reload{false};
        bool toss{false};
        bool cycle{false};
        bool strike{false};
    };

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
        f32  spin{0.f};         /* rate of fire wound up, 0 to 1 */
        bool spins{false};      /* the trigger winds up at all */
        i16  burst{0};          /* shots left of the last pull */
        weapon_id weapon{};     /* what it is for */
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
        if(!world::world_loaded(p))
            return;
        bool const halo1 =
            p.template subsystem<LoadingStatus>().loaded_map == LoadingStatus::loaded;

        std::set<u64> live;
        for(auto ent : p.template select<
                       PlayerInput,
                       PlayerInfo,
                       PlayerCamera,
                       NetworkInfo>())
        {
            auto [input, info, cam, net] = ent.components();
            /* Presses are taken whether or not they can be acted on */
            intent_t intent{
                .swap   = std::exchange(input.intent.swap, false),
                .reload = std::exchange(input.intent.reload, false),
                .toss   = std::exchange(input.intent.grenade, false),
                .cycle  = std::exchange(input.intent.grenade_switch, false),
                .strike = std::exchange(input.intent.melee, false),
            };
            /* A gunner fires the vehicle's weapon, a body its own, and so
             * does a passenger whose seat allows it; the free camera has
             * nothing to shoot with */
            u64 const   riding  = info.riding.vehicle;
            auto const* mounted = riding && halo1 ? mounted_weapon(p, info, files) : nullptr;
            bool const  armed   = riding && !mounted && info.riding.armed;
            auto*       equip   = p.template get<BipedEquipment>(ent.id());
            auto const* carried = equip ? equip->drawn(files.load_generation) : nullptr;
            weapon_id const weapon =
                mounted                         ? weapon_id::of(MapGameEvent::halo1, mounted->tag_id)
                : (!riding || armed) && carried ? carried->id
                                                : weapon_id{};
            /* A remote body the server simulates fires here too */
            if((info.is_remote() && !info.mode.simulated) || !weapon.valid() ||
               (riding ? info.riding.exiting
                       : !info.mode.physics || !biped_in_play(info, cam, net)))
                continue;
            world::with_map(p, weapon.game, [&](auto const& map) {
                operate(p, ent.id(), input, info, cam, equip, mounted, map, weapon, ticks, intent, live);
            });
        }
        std::erase_if(m_states, [&live](auto const& e) {
            return !live.contains(e.first);
        });
        if(!halo1)
            return;
        auto const& magic = files.container.magic;

        drive_held(p, files.load_generation, magic);
        hush(p);
        m_fire_queue->poll();
        for(auto const& shot : std::exchange(m_shots, {}))
            fire(p, shot, files);

        m_use_queue->poll();
        if(!p.template subsystem<NetworkState>().replica)
            pick_up(p, files);
    }

    /* One body's weapon over this frame's ticks: rate of fire, magazine,
     * heat and charge, read from any game's weapon through its firing
     * overloads. Spin-up, misfires, overcharge, the dry click, a carried
     * flag and mounted guns are Halo 1's so far */
    template<typename Map>
    void operate(
        Proxy&                        p,
        u64                           id,
        PlayerInput&                  input,
        PlayerInfo const&             info,
        PlayerCamera const&           cam,
        BipedEquipment*               equip,
        blam::tag_t const*            mounted,
        Map const&                    map,
        weapon_id                     weapon_tag,
        std::vector<TickEvent> const& ticks,
        intent_t&                     intent,
        std::set<u64>&                live)
    {
        constexpr bool halo1  = world::game_of<Map>::game == MapGameEvent::halo1;
        auto const     weapon = world::lookup<typename world::game_of<Map>::weapon>(map, weapon_tag);
        if(!weapon)
            return;
        auto const         modes  = firing_modes(*weapon, map);
        blam::tag_t const* h1_tag = nullptr;
        if constexpr(halo1)
            h1_tag = weapon.tag;
        live.insert(id);
        auto& state = m_states[id];
        /* Another weapon starts afresh */
        if(state.weapon != weapon_tag)
            state = {.weapon = weapon_tag};
        u64 const riding  = info.riding.vehicle;
        u64 const vehicle = mounted ? riding : 0;

        /* Grenades are thrown on foot, unless what is held forbids it */
        bool throwing = false;
        if constexpr(halo1)
        {
            using weapon_flags_t = blam::scn::weapon::weapon_flags_t;
            bool const no_grenades =
                (weapon->weapon_flags & weapon_flags_t::prevents_grenade_throwing) != weapon_flags_t::none;
            throwing = !riding &&
                       grenade(p, id, info, cam, *equip, ticks, intent.toss && !no_grenades, intent.cycle);
        }
        /* A strike on foot stops the gun until it is done */
        for(auto const& tick : ticks)
            state.striking = std::max(state.striking - tick.seconds, 0.f);
        if(intent.strike && !riding && !throwing && state.striking <= 0.f)
        {
            state.striking  = melee_time;
            state.reloading = 0.f;
            pull(p, id, info, cam, weapon_tag, h1_tag, mounted != nullptr, true);
        }
        /* Nothing to fire (a flag): melee alone, and a pull lets go of it */
        if(modes.empty())
        {
            if constexpr(halo1)
                if(input.intent.fire && !throwing && state.striking <= 0.f &&
                   equip->objective.tag == h1_tag)
                {
                    m_hold_fire.insert(id);
                    EquipWeaponEvent drop{
                        .entity_id = id, .weapon = h1_tag->tag_id, .mode = EquipWeaponEvent::drop};
                    GameEvent ev{.type = GameEvent::EquipWeapon};
                    p.template subsystem<GameEventBus>().inject(ev, &drop);
                }
            return;
        }
        auto const& trigger = modes[0];
        /* The pull that let go of a flag fires nothing until released */
        if(!input.intent.fire)
            m_hold_fire.erase(id);
        bool const pulled =
            input.intent.fire && !throwing && state.striking <= 0.f && !m_hold_fire.contains(id);

        /* Bodies switch, reload and run dry; vehicle guns don't */
        auto* ammo = vehicle ? nullptr : &equip->weapons[0];
        if(ammo && intent.swap && equip->weapons[1].id.valid())
        {
            std::swap(equip->weapons[0], equip->weapons[1]);
            equip->reloading = false;
            return;
        }
        auto const* magazine = ammo ? magazine_of(*weapon, trigger, map) : nullptr;
        i16 const   per_shot = std::max<i16>(rounds_per_shot(trigger), 1);

        /* A rate of zero fires once a pull. A trigger with a spin-up
         * winds from its first rate to its last over the acceleration
         * time; let go, it winds down without firing (the barrel spins on) */
        bool spins = false;
        if constexpr(halo1)
            spins = trigger.acceleration_time > 0.f &&
                    trigger.maximum_rate_of_fire.y > trigger.maximum_rate_of_fire.x;
        state.spins          = spins;
        bool const automatic = repeats(trigger);
        /* A trigger that charges fires as it is let go: its partner's
         * shot once charged */
        f32 const  charge_time = charging_time(trigger);
        bool const charges     = charge_time > 0.f && modes.size() > 1;
        /* A mounted gun's second trigger fires alongside its first:
         * the tank's gun, the banshee's fuel rod */
        bool const held2 = mounted && modes.size() > 1 && !charges && input.intent.fire2 &&
                           state.striking <= 0.f;
        /* The trigger is looked at once a tick */
        for(auto const& tick : ticks)
        {
            f32 const dt    = tick.seconds;
            f32 const hurry = dt * info.speed_scale();
            state.fired     = false;
            state.cooldown  = std::max(state.cooldown - hurry, 0.f);
            if(state.reloading > 0.f && (state.reloading -= hurry) <= 0.f && magazine)
                refill(*ammo, *magazine);
            bool const loaded = !magazine || ammo->rounds_loaded >= per_shot;
            if(magazine && state.reloading <= 0.f && ammo->rounds_total > 0 &&
               ammo->rounds_loaded < rounds_loaded_maximum(*magazine) && (intent.reload || !loaded))
            {
                state.reloading = std::max({reload_time(*magazine), ammo->reload_length, dt});
                intent.reload   = false;
            }
            if(ammo)
                cool(*ammo, *weapon, dt);
            bool const ready =
                loaded && state.reloading <= 0.f && (!ammo || !spent(*ammo, *weapon));
            bool const able = ready && state.cooldown <= 0.f;
            f32        rate = rate_of_fire(trigger);
            if constexpr(halo1)
                if(spins)
                {
                    if(pulled && ready)
                        state.spin = std::min(state.spin + dt / trigger.acceleration_time, 1.f);
                    else if(trigger.deceleration_time > 0.f)
                        state.spin = std::max(state.spin - dt / trigger.deceleration_time, 0.f);
                    else
                        state.spin = 0.f;
                    rate = glm::mix(
                        trigger.maximum_rate_of_fire.x, trigger.maximum_rate_of_fire.y, state.spin);
                }
            bool const held  = pulled;
            i32        fires = -1;
            /* A fresh pull on nothing to fire: the dry click */
            if constexpr(halo1)
            {
                bool const dry = magazine ? !loaded && ammo->rounds_total < per_shot
                                          : ammo && spent(*ammo, *weapon);
                if(held && !state.firing && dry && state.reloading <= 0.f)
                    dry_pull(p, id, trigger);
            }
            if(charges)
            {
                if(held && !state.firing && able)
                    state.charge = 0.f;
                else if(held && state.charge >= 0.f)
                {
                    state.charge += dt;
                    /* Held charged for too long: what the trigger then
                     * does, let off or blown */
                    if constexpr(halo1)
                    {
                        using action_t = trigger_t::overcharged_action_t;
                        if(trigger.overcharged_action != action_t::none &&
                           state.charge >= trigger.charging_time + trigger.charged_time)
                        {
                            if(trigger.overcharged_action == action_t::discharge)
                                fires = 1;
                            else if(ammo)
                                detonate(p, id, *ammo, *weapon);
                            state.charge = -1.f;
                        }
                    }
                } else if(!held && state.charge >= 0.f)
                {
                    fires        = state.charge >= charge_time ? 1 : 0;
                    state.charge = -1.f;
                }
            } else if(held && (automatic || !state.firing))
            {
                fires = 0;
                if(!automatic && able)
                    state.burst = shots_per_pull(trigger) - 1;
            } else if(state.burst > 0)
            {
                fires = 0;
                if(able)
                    state.burst--;
            }
            if(fires >= 0 && able)
            {
                auto const& mode = modes[fires];
                if(rate > 0.f)
                    state.cooldown += 1.f / rate;
                state.fired = true;
                /* A battery run low misfires some of its pulls */
                bool misfires = false;
                if constexpr(halo1)
                {
                    misfires = ammo && weapon->age_misfire_start > 0.f &&
                               ammo->age >= weapon->age_misfire_start &&
                               chance() < weapon->age_misfire_chance;
                    if(misfires)
                        misfire(p, id, mode);
                }
                if(!misfires)
                {
                    state.illumination = 1.f;
                    if(magazine)
                        ammo->rounds_loaded -= per_shot;
                    if(ammo)
                        heat_up(p, id, *ammo, *weapon, mode);
                    pull(p, id, info, cam, weapon_tag, h1_tag, mounted != nullptr, false,
                         static_cast<u16>(fires));
                }
            }
            state.firing    = held;
            state.cooldown2 = std::max(state.cooldown2 - dt, 0.f);
            if(held2)
            {
                f32 const  rate2      = rate_of_fire(modes[1]);
                bool const automatic2 = repeats(modes[1]);
                if((automatic2 || !state.firing2) && state.cooldown2 <= 0.f)
                {
                    if(rate2 > 0.f)
                        state.cooldown2 += 1.f / rate2;
                    state.fired        = true;
                    state.illumination = 1.f;
                    pull(p, id, info, cam, weapon_tag, h1_tag, true, false, 1);
                }
            }
            state.firing2 = held2;
            if(ammo)
            {
                ammo->charged  = charges && state.charge >= charge_time;
                ammo->charging = charges && state.charge >= 0.f;
            }
            if(f32 const recovery = illumination_recovery_time(trigger); recovery > 0.f)
                state.illumination = std::max(state.illumination - dt / recovery, 0.f);
            else if(!state.fired)
                state.illumination = 0.f;
        }

        if(!vehicle)
        {
            equip->reloading = state.reloading > 0.f;
            if constexpr(halo1)
                drive_functions(
                    p, id, *weapon, state, pulled || state.spin > 0.f, map, ammo,
                    charges ? charge_time : 0.f);
        } else if constexpr(halo1)
            drive_vehicle(
                p, vehicle, *weapon, state, pulled || state.spin > 0.f, map.magic,
                static_cast<u32>(ticks.size()));
    }

    /* The authority hands out what bodies walk over, local and remote alike:
     * ammo for what they carry, a weapon into a free slot, or a swap when
     * they press use. A weapon given up for ammo lies on with what is left */
    void pick_up(Proxy& p, BlamFiles<halo_version> const& files)
    {
        std::set<u64> taken;
        for(auto ent : p.template select<PlayerInfo, PlayerCamera, NetworkInfo, BipedEquipment>())
        {
            auto [info, cam, net, equip] = ent.components();
            bool const pressed           = std::exchange(m_use_pressed[ent.id()], false);
            /* Held, it takes the next weapon it walks over, one a press */
            bool& spent_press = m_swapped[ent.id()];
            if(!m_using[ent.id()])
                spent_press = false;
            bool const held = m_using[ent.id()] && !spent_press;
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
            /* Looked up once a body is equipped from this map */
            if(m_flag_generation != files.load_generation)
            {
                m_flag            = flag_weapon(files);
                m_flag_generation = files.load_generation;
            }
            for(auto item : p.template select<Model, NetworkInfo>())
            {
                auto [model, item_net] = item.components();
                auto const* tag        = model.origin_object;
                /* The flag goes by the game type's rules, not by walking over it */
                if(item_net.instance_id == 0 ||
                   SpawnObjectEvent::is_impostor(item_net.instance_id) || !tag || tag == m_flag ||
                   taken.contains(item.id()) || p.template get<PlayerInfo>(item.id()))
                    continue;
                /* A grenade of a kind carried, while there is room for it */
                bool const grenade = std::any_of(
                    equip.equipment.begin(), equip.equipment.end(), [tag](auto const& kind) {
                        return kind.tag == tag && kind.left < kind.most;
                    });
                bool const powerup = !grenade && wanted_powerup(p, ent.id(), info, *tag, files);
                /* Ammo for a carried weapon with room for it */
                bool const ammo =
                    !grenade && !powerup && tag->matches(blam::tag_class_t::eqip) &&
                    std::any_of(equip.weapons.begin(), equip.weapons.end(), [&](auto const& w) {
                        return ammo_pack_rounds(w, tag->tag_id, files.container.magic) > 0;
                    });
                if(!grenade && !powerup && !ammo && !tag->matches(blam::tag_class_t::weap))
                    continue;
                if(!within_pickup_reach(info, cam, model.position))
                    continue;
                auto const* same = equip.weapons[0].tag == tag   ? &equip.weapons[0]
                                   : equip.weapons[1].tag == tag ? &equip.weapons[1]
                                                                 : nullptr;
                bool const carried = grenade || powerup || ammo || same;
                /* A swap takes a press, and only for the weapon on offer:
                 * a seat nearer the centre of the view wins the press */
                if(!carried && equip.weapons[1].weapon &&
                   (!(pressed || held) || info.offered.weapon != item.id()))
                    continue;
                if(!carried && equip.weapons[1].weapon)
                    spent_press = true;
                auto rounds = model.item_rounds.value_or(std::array<u16, 2>{});
                if(tag->matches(blam::tag_class_t::weap) && !model.item_rounds)
                    if(auto weapon = tag->template data<weapon_t>(files.container.magic);
                       weapon.has_value())
                        rounds = initial_rounds(weapon.value(), files.container.magic);
                bool emptied = true;
                if(same)
                {
                    /* Into the reserve as much as fits, its reserve given first */
                    i32 const room = reserve_room(*same, files.container.magic);
                    if(room <= 0 || rounds[0] + rounds[1] == 0)
                        continue;
                    u16 const from_reserve = static_cast<u16>(std::min<i32>(room, rounds[1]));
                    u16 const from_loaded =
                        static_cast<u16>(std::min<i32>(room - from_reserve, rounds[0]));
                    model.item_rounds = std::array<u16, 2>{
                        static_cast<u16>(rounds[0] - from_loaded),
                        static_cast<u16>(rounds[1] - from_reserve)};
                    emptied = *model.item_rounds == std::array<u16, 2>{};
                    rounds  = {from_loaded, from_reserve};
                }
                taken.insert(item.id());
                auto& game = p.template subsystem<GameEventBus>();
                if(emptied)
                {
                    DespawnObjectEvent gone{.net_id = item_net.instance_id};
                    GameEvent          despawn{.type = GameEvent::DespawnObject};
                    game.inject(despawn, &gone);
                }
                EquipWeaponEvent equip_ev{
                    .entity_id = ent.id(),
                    .weapon    = tag->tag_id,
                    .mode      = EquipWeaponEvent::pick_up,
                    .age       = model.item_age,
                    .rounds    = rounds,
                };
                GameEvent ev{.type = GameEvent::EquipWeapon};
                game.inject(ev, &equip_ev);
                break;
            }
        }
    }

    /* A powerup is taken unless it would do nothing or its kind is still
     * active: health at full health, an over shield while overcharged */
    static bool wanted_powerup(
        Proxy&                         p,
        u64                            player,
        PlayerInfo const&              info,
        blam::tag_t const&             tag,
        BlamFiles<halo_version> const& files)
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
            return status && status->maximum_shield() > 0.f && status->shield <= 1.f;
        case powerup_t::double_speed:
            return info.powerups.speed <= 0.f;
        case powerup_t::active_camouflage:
            return info.powerups.camouflage <= 0.f;
        case powerup_t::full_spectrum_vision:
            return info.powerups.vision <= 0.f;
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
        weapon_id           weapon,
        blam::tag_t const*  h1_tag,
        bool                on_mount,
        bool                melee   = false,
        u16                 trigger = 0)
    {
        Matf3 const     to_world = glm::transpose(Matf3(cam.rotation));
        Vecf3 const     forward  = to_world * Vecf3{0.f, 0.f, -1.f};
        WeaponFireEvent shot{
            .weapon     = h1_tag ? h1_tag->as_ref() : blam::tagref_t{},
            .id         = weapon,
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
        if(auto* equip = !melee && !on_mount ? p.template get<BipedEquipment>(player) : nullptr)
            equip->shots++;
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
            if(!shot.target && h1_tag)
                shot.target = autoaimed(p, player, info, cam.camera.position, forward, *h1_tag);
        }
        if(on_mount)
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
                Vecf3           bore{};
                mount(p, barrel, info.riding.vehicle, i, &bore);
                Vecf3 const to = target - barrel.position;
                /* A barrel set off the gun's own line keeps that offset */
                if(glm::dot(to, forward) > 0.f)
                {
                    Quatf const spread = glm::normalize(Quatf(
                        1.f + glm::dot(bore, barrel.direction), glm::cross(bore, barrel.direction)));
                    barrel.direction = spread * glm::normalize(to);
                }
                p.template subsystem<GameEventBus>().inject(ev, &barrel);
            }
            return;
        }
        p.template subsystem<GameEventBus>().inject(ev, &shot);
    }

    /* From the gun's marker as the vehicle stands, along its barrel; a gun
     * the gunner turns points where they look */
    /* `bore` gets the gun's own forward, before the marker's offset */
    void mount(
        Proxy& p, WeaponFireEvent& shot, u64 vehicle, u32 instance, Vecf3* bore = nullptr)
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
        if(bore)
            *bore = glm::normalize(Vecf3(at[0]));
        at                 = at * marker->transform;
        Vecf3 const barrel = Vecf3(at[0]);
        shot.vehicle       = vehicle;
        shot.position      = Vecf3(at[3]);
        if(glm::dot(barrel, barrel) > 1e-6f)
            shot.direction = glm::normalize(barrel);
        else if(bore)
            shot.direction = *bore;
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
        if(shot.windup)
            return;
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
                    .effect        = effect,
                    .position      = shot.position,
                    .direction     = forward,
                    .parent        = parent,
                    .marker_parent = parent == first ? third : 0,
                    .visuals_only  = visuals_only,
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
        if(tag == index.end())
            return;
        /* A weapon's melee, or a unit's own (claws, the Flood's lunge) */
        blam::tagref_t melee{};
        if((*tag).matches(blam::tag_class_t::jpt))
            melee = shot.weapon;
        else if(auto weapon = (*tag).template data<weapon_t>(magic);
                (*tag).matches(blam::tag_class_t::weap) && weapon.has_value())
            melee = weapon.value()[0].player_melee_damage;
        if(!melee.valid())
            return;
        auto jpt = index.template data<blam::scn::damage_effect>(melee);
        /* A body in reach before a wall, else the wall */
        std::optional<std::pair<Physics::Raycast, Vecf3>> struck_at;
        /* Level and a little low, for the short: grunts and jackals */
        Vecf3 const side = glm::cross(shot.direction, Vecf3{0.f, 0.f, 1.f});
        Vecf3 const right =
            glm::dot(side, side) > 1e-6f ? glm::normalize(side) : Vecf3{0.f, 1.f, 0.f};
        /* A unit's blow reaches the target it swung at: a hunter stands
         * further off than a player's arm */
        f32 reach = melee_reach;
        if(shot.player_idx == ObjectHitEvent::no_player && shot.target)
            if(auto const* aim = p.template get<Model>(shot.target))
                reach = std::clamp(glm::distance(shot.position, aim->position) + .25f, melee_reach, unit_melee_reach);
        for(f32 const dip : {0.f, -melee_dip})
            for(f32 const turn : {0.f, melee_spread, -melee_spread})
        {
            Vecf3 const dir = glm::angleAxis(turn, Vecf3{0.f, 0.f, 1.f}) *
                              (glm::angleAxis(dip, right) * shot.direction);
            Physics::Event   ev{Physics::Event::Raycast};
            Physics::Raycast ray{
                .from   = shot.position,
                .to     = shot.position + dir * reach,
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
        landed(p, jpt.has_value() ? jpt.value() : nullptr, ray, files);
        if(replica || ray.entity_id == 0)
            return;
        auto const* target = p.template get<PlayerInfo>(ray.entity_id);
        ObjectHitEvent hit{
            .damage         = melee,
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
        Proxy& p, blam::scn::damage_effect const* jpt,
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

    /* Heat bleeds off; a vented weapon fires again once cool enough */
    template<typename Weapon>
    static void cool(BipedEquipment::weapon_t& gun, Weapon const& tag, f32 dt)
    {
        gun.heat = std::max(gun.heat - heat_loss_rate(tag) * dt, 0.f);
        if(gun.overheated && gun.heat <= heat_recovery_threshold(tag))
            gun.overheated = false;
    }

    /* Venting, or a battery run flat where that stops it */
    template<typename Weapon>
    static bool spent(BipedEquipment::weapon_t const& gun, Weapon const& tag)
    {
        return gun.overheated || (stops_at_maximum_age(tag) && gun.age >= 1.f);
    }

    f32 chance()
    {
        return std::uniform_real_distribution<f32>(0.f, 1.f)(m_rng);
    }

    /* An effect off the weapon in hand, following the first-person one */
    void hand_effect(Proxy& p, u64 player, blam::tagref_t const& effect)
    {
        if(!effect.valid())
            return;
        auto const* cam = p.template get<PlayerCamera>(player);
        SpawnEffectEvent spawn{
            .effect    = effect,
            .position  = cam ? cam->camera.position : Vecf3{},
            .direction = cam ? glm::transpose(Matf3(cam->rotation)) * Vecf3{0.f, 0.f, -1.f}
                             : Vecf3{1.f, 0.f, 0.f},
            .parent    = first_person(p, player, true),
        };
        GameEvent ev{.type = GameEvent::SpawnEffect};
        p.template subsystem<GameEventBus>().inject(ev, &spawn);
    }

    /* The weapon blows up in hand: its detonation effect, then it vents */
    void detonate(
        Proxy& p, u64 player, BipedEquipment::weapon_t& gun, weapon_t const& tag)
    {
        gun.heat       = std::max(gun.heat, tag.overheated_threshold);
        gun.overheated = true;
        hand_effect(p, player, tag.overheat_detonation);
    }

    /* A shot warms the weapon and drains its battery; too hot, it vents, or
     * past its detonation threshold it may blow instead */
    template<typename Weapon, typename Mode>
    void heat_up(
        Proxy& p, u64 player, BipedEquipment::weapon_t& gun, Weapon const& tag,
        Mode const& pulled)
    {
        gun.heat += heat_per_round(pulled);
        gun.age = std::min(gun.age + age_per_round(pulled), 1.f);
        if(overheated_threshold(tag) <= 0.f || gun.heat < overheated_threshold(tag) ||
           gun.overheated)
            return;
        if constexpr(!std::is_same_v<Weapon, weapon_t>)
        {
            gun.overheated = true;
            return;
        } else
            vent(p, player, gun, tag);
    }

    void vent(Proxy& p, u64 player, BipedEquipment::weapon_t& gun, weapon_t const& tag)
    {
        bool const detonates = tag.heat_detonation_threshold > 0.f &&
                               gun.heat >= tag.heat_detonation_threshold &&
                               chance() < tag.heat_detonation_fraction &&
                               tag.overheat_detonation.valid();
        if(detonates)
            detonate(p, player, gun, tag);
        else
        {
            gun.overheated = true;
            hand_effect(p, player, tag.overheated);
        }
    }

    /* The pull fizzles: the trigger's misfire effect, and its damage to
     * whoever holds it */
    void misfire(Proxy& p, u64 player, trigger_t const& pulled)
    {
        auto effects = pulled.firing_effects.data(
            p.template subsystem<BlamFiles<halo_version>>().container.magic);
        if(!effects.has_value() || effects.value().empty())
            return;
        auto const& effect = effects.value()[0];
        hand_effect(p, player, effect.misfire_effect);
        if(!effect.misfire_damage.valid())
            return;
        auto const* cam = p.template get<PlayerCamera>(player);
        AreaDamageEvent area{
            .damage   = effect.misfire_damage,
            .position = cam ? cam->camera.position : Vecf3{},
            .shooter  = player,
        };
        GameEvent ev{.type = GameEvent::AreaDamage};
        p.template subsystem<GameEventBus>().inject(ev, &area);
    }

    void dry_pull(Proxy& p, u64 player, trigger_t const& trigger)
    {
        auto effects = trigger.firing_effects.data(
            p.template subsystem<BlamFiles<halo_version>>().container.magic);
        if(effects.has_value() && !effects.value().empty())
            hand_effect(p, player, effects.value()[0].empty_effect);
    }

    /* Tops the magazine up from what is carried */
    template<typename Magazine>
    static void refill(BipedEquipment::weapon_t& ammo, Magazine const& magazine)
    {
        u16 const room = static_cast<u16>(
            std::max<i32>(rounds_loaded_maximum(magazine) - ammo.rounds_loaded, 0));
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
        /* Out of the chosen type, the next one carried is chosen */
        bool const empty = equip.grenade >= kinds.size() || kinds[equip.grenade].left == 0;
        if((cycle || empty) && !kinds.empty() && state.windup < 0.f)
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
        blam::map_container<halo_version> const& map,
        BipedEquipment::weapon_t const* gun,
        f32                  charge_time)
    {
        auto const& magic = map.magic;
        u64 const id = first_person(p, player, true);
        if(id == 0)
            return;
        auto* model = p.template get<Model>(id);

        f32 const charged =
            charge_time > 0.f && state.charge >= 0.f ? std::min(state.charge / charge_time, 1.f) : 0.f;
        /* What is loaded, as a share of the magazine */
        auto const  modes    = firing_modes(weapon, map);
        auto const* magazine = gun && !modes.empty() ? magazine_of(weapon, modes[0], map) : nullptr;
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
        if(auto* equip = gun ? p.template get<BipedEquipment>(player) : nullptr)
            equip->weapons[0].exports = out;
        hum(p, player, id, *model, out, magic);
    }

    /* A vehicle's object functions as its gun fires: its seats' power and
     * engine, and for an input it leaves unnamed its gun's export, so the
     * gun's rate of fire can drive the hull (the cursed ghost spins). An
     * additive function keeps what it summed, wrapping, a tick at a time */
    void drive_vehicle(
        Proxy&               p,
        u64                  vehicle,
        weapon_t const&      weapon,
        state_t const&       state,
        bool                 held,
        blam::map_ptr const& magic,
        u32                  ticks)
    {
        auto* model = p.template get<Model>(vehicle);
        if(!model || !model->origin_object)
            return;
        auto vehi = model->origin_object->template data<blam::scn::vehicle>(magic);
        if(!vehi.has_value())
            return;
        auto const& v = vehi.value()[0];

        std::array<f32, 4> gun{};
        for(size_t i = 0; i < gun.size(); i++)
            gun[i] = input_value(weapon.weapon_inputs[i], state, held, nullptr, 0.f, 1.f);
        std::array<f32, 4> gun_out = gun;
        if(auto functions = weapon.functions.data(magic); functions.has_value())
            for(size_t i = 0; i < gun_out.size() && i < functions.value().size(); i++)
                gun_out[i] = blam_functions::evaluate(functions.value()[i], gun, gun_out);

        auto const* body   = p.template get<ObjectPhysics>(vehicle);
        f32 const   engine = body ? std::min(std::abs(body->vehicle.throttle), 1.f) : 0.f;
        /* Speed along the hull, as a share of the tag's top speeds (wu per tick) */
        f32 const along = body ? glm::dot(body->linear_velocity, model->rotation * Vecf3{1.f, 0.f, 0.f}) /
                                     static_cast<f32>(TickClock::rate)
                               : 0.f;
        auto share = [](f32 speed, f32 top) { return top > 0.f ? std::clamp(speed / top, 0.f, 1.f) : 0.f; };
        using unit_in_t = blam::scn::unit::unit_function_in_t;
        using vehi_in_t = blam::scn::vehicle::vehicle_function_in_t;
        std::array<f32, 4> in{};
        for(size_t i = 0; i < in.size(); i++)
        {
            std::optional<f32> value;
            switch(v.unit_inputs[i])
            {
            case unit_in_t::driver_seat_power:
            case unit_in_t::gunner_seat_power:
                value = 1.f;
                break;
            default:
                break;
            }
            if(!value)
                switch(v.vehicle_inputs[i])
                {
                case vehi_in_t::engine_hack:
                case vehi_in_t::thrust:
                case vehi_in_t::hover:
                    value = engine;
                    break;
                case vehi_in_t::speed_absolute:
                    value = share(std::abs(along), along >= 0.f ? v.maximum_forward_speed : std::abs(v.maximum_reverse_speed));
                    break;
                case vehi_in_t::speed_forward:
                    value = share(along, v.maximum_forward_speed);
                    break;
                case vehi_in_t::speed_backward:
                    value = share(-along, std::abs(v.maximum_reverse_speed));
                    break;
                default:
                    break;
                }
            in[i] = value ? *value : gun_out[i];
        }
        std::array<f32, 4> out = in;
        if(auto functions = v.functions.data(magic); functions.has_value())
            for(size_t i = 0; i < out.size() && i < functions.value().size(); i++)
            {
                f32 const value = blam_functions::evaluate(functions.value()[i], in, out);
                if(!blam_functions::additive(functions.value()[i]))
                {
                    out[i] = value;
                    continue;
                }
                f32 const was = std::max(model->object_function[i], 0.f);
                f32 const sum = was + value * static_cast<f32>(ticks);
                out[i]        = sum - std::floor(sum);
            }
        model->object_function = out;
    }

    /* What a unit other than a player holds exports as well: its charge,
     * heat and age through its functions, onto the model in its hand and
     * for the holder to import (a hunter's cannon flare) */
    void drive_held(Proxy& p, u32 generation, blam::map_ptr const& magic)
    {
        for(auto ent : p.template select<BipedEquipment>())
        {
            if(p.template get<PlayerInfo>(ent.id()))
                continue;
            auto* gun = ent.template get<BipedEquipment>().held(generation);
            if(!gun || !gun->weapon)
                continue;
            auto const& weapon = *gun->weapon;
            std::array<f32, 4> in{};
            for(size_t i = 0; i < in.size(); i++)
                in[i] = input_value(weapon.weapon_inputs[i], state_t{}, false, gun, gun->charge, 1.f);
            std::array<f32, 4> out = in;
            auto functions         = weapon.functions.data(magic);
            if(functions.has_value())
                for(size_t i = 0; i < out.size() && i < functions.value().size(); i++)
                    out[i] = blam_functions::evaluate(functions.value()[i], in, out);
            gun->exports = out;
            if(auto* held = gun->held ? p.template get<Model>(gun->held) : nullptr)
                held->object_function = out;
        }
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
            return state.spins ? state.spin : held ? 1.f : 0.f;
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
    static constexpr f32 unit_melee_reach = 3.f; /* wu, an NPC's longest blow */
    static constexpr f32 melee_spread    = .15f; /* rad, the side rays */
    static constexpr f32 melee_dip       = .35f; /* rad, the low rays */
    static constexpr f32 aim_reach       = 100.f; /* wu, a reticle on nothing */
    static constexpr f32 autoaim_height  = .6f;   /* wu above a unit's origin */
    static constexpr f32 behind_cos      = .5f;  /* within 60 deg of its facing */
    /* Vehicle guns: where they fire from, and how far the crosshair looks */
    static constexpr std::string_view gun_marker = "primary trigger";

    u32                       m_generation{0};
    blam::tag_t const*        m_flag{nullptr}; /*!< Taken by the game type's rules, not walked over */
    std::set<u64>             m_hold_fire;     /*!< Let go of a flag with the trigger still held */
    u32                       m_flag_generation{0};
    std::map<u64, state_t>    m_states;
    std::map<u64, throw_t>    m_throws;
    std::vector<grenade_t>    m_grenades;
    std::minstd_rand          m_rng{std::random_device{}()};

    std::shared_ptr<GameEventBus::queue_type<WeaponFireEvent>> m_fire_queue;
    /* Use presses per player, from every seat's and client's tick input */
    std::shared_ptr<GameEventBus::queue_type<PlayerTickInputEvent>> m_use_queue;
    std::map<u64, bool> m_swapped; /* this hold of use already swapped */
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
