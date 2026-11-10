#include "actors.h"
#include "blam_files.h"
#include "caching.h"
#include "allegiance.h"
#include "components.h"
#include "difficulty.h"
#include "game_events.h"
#include "game_mode.h"
#include "model_markers.h"
#include "nav_mesh.h"
#include "network/networking.h"
#include "physics.h"
#include "selected_version.h"
#include "sounds.h"
#include "ticks.h"
#include "save_state.h"
#include "weapons.h"

#include "animation.h"

#include <blam/volta/blam_actor.h>
#include <blam/volta/blam_collision.h>
#include <blam/volta/blam_globals.h>
#include <blam/volta/blam_scenario.h>
#include <coffee/components/proxy.h>
#include <coffee/components/restricted_subsystem.h>

#include <map>
#include <optional>
#include <queue>
#include <set>
#include <random>

using ActorManifest = compo::SubsystemManifest<
    type_list_t<
        Actor,
        Model,
        AnimationPlayback,
        BipedStatus,
        BipedEquipment,
        const ObjectPhysics,
        const PhysicsData,
        const PlayerInfo,
        const PlayerCamera,
        const Projectile>,
    type_list_t<
        GameEventBus,
        PhysicsBus,
        const BSPCache<halo_version>,
        const GameState,
        const NetworkState,
        const LoadingStatus,
        const BlamFiles<halo_version>,
        AllegianceState>,
    type_list_t<comp_app::EventBus<SoundEvent>>>;

/* Encounter units fight by their actor tags: they watch for enemies, take
 * firing positions, fire in bursts, crouch between, give ground when beaten
 * and look where a lost target was. Platoons switch between attacking and
 * defending positions by their strength, and their followers panic when
 * their leaders fall. The authority decides; others see the bodies move and
 * the shots fired */
struct ActorSystem : compo::RestrictedSubsystem<ActorSystem, ActorManifest>
{
    using type    = ActorSystem;
    using Proxy   = compo::proxy_of<ActorManifest>;
    using actv_t  = blam::scn::actor_variant;
    using actr_t  = blam::scn::actor;
    using state_t = Actor::state_t;
    using team_t  = blam::scn::unit::team_t;

    ActorSystem()
    {
        this->priority = 2100; /* after the scripts have placed and moved */
    }

    bool main_thread_only() const override
    {
        return true;
    }

    /* An object pathfinding sphere the AI walks round */
    struct obstacle_t
    {
        Vecf3 center{};
        f32   radius{0.f};
        u64   entity{0};
    };

    /* What an actor is, by its squad */
    struct kind_t
    {
        actv_t const* variant{nullptr};
        actr_t const* actor{nullptr};
        blam::scn::ai::encounter const* encounter{nullptr};
        blam::scn::ai::squad const*     squad{nullptr};
    };

    void start_restricted(Proxy& p, time_point const&)
    {
        f32 dt = 0.f;
        for(auto const& tick : m_ticks->take())
            dt += tick.seconds;
        m_allegiance = &p.template subsystem<AllegianceState>();
        if(dt <= 0.f || !m_scenario ||
           p.template subsystem<LoadingStatus>().loaded_map != LoadingStatus::loaded ||
           p.template subsystem<NetworkState>().replica)
            return;
        m_allegiance->advance(dt);
        auto const& files = p.template subsystem<BlamFiles<halo_version>>();
        if(m_generation != files.load_generation)
        {
            m_generation = files.load_generation;
            m_kinds.clear();
            m_links.clear();
            m_spheres.clear();
            m_difficulty = difficulty_block(files);
        }
        m_level = p.template subsystem<GameState>().difficulty;
        refresh_nav(p);
        hear(p);
        if((m_obstacles_in -= dt) <= 0.f)
        {
            m_obstacles_in = 1.f;
            gather_obstacles(p);
        }
        if((m_platoon_in -= dt) <= 0.f)
        {
            m_platoon_in = 1.f;
            muster(p);
        }
        for(auto ent : p.template select<Actor, Model, AnimationPlayback, BipedStatus, ObjectPhysics>())
        {
            auto [actor, model, anim, status, physics] = ent.components();
            /* Seated: it fights from where it sits */
            if(physics.seated && actor.active() && !actor.braindead && !status.dead)
            {
                if(auto const* kind = kind_of(p, actor); kind && kind->variant && kind->actor)
                    ride(p, ent.id(), actor, model, *kind, physics, dt);
                continue;
            }
            if(auto const* kind = kind_of(p, actor); kind && kind->variant)
            {
                if(!status.dead)
                    stock(p, ent.id(), actor, *kind);
                else if(!actor.dropped)
                    drop_belongings(p, ent.id(), actor, model, *kind);
            }
            actor.dropped = status.dead;
            if(!actor.active() || actor.braindead || status.dead || !physics.upright || physics.seated)
            {
                /* What it last walked at, it would keep walking at */
                stop(p, ent.id(), actor);
                actor.board = 0;
                continue;
            }
            auto const* kind = kind_of(p, actor);
            taunt(p, ent.id(), actor, model, dt);
            if(!kind || !kind->variant || !kind->actor)
                continue;
            /* Sent to a vehicle: it runs there, whatever else it was doing,
             * and waits to be seated */
            if(actor.board)
            {
                auto const* vehicle = p.template get<Model>(actor.board);
                if(!vehicle)
                    actor.board = 0;
                else if(glm::distance(Vecf2(model.position), Vecf2(vehicle->position)) > actor.board_radius)
                    move_toward(p, ent.id(), actor, model, anim, *kind, vehicle->position, dt, true);
                else
                    stop(p, ent.id(), actor);
                continue;
            }
            /* Under a command list it only watches, and asks to be let go
             * when the list allows a fight */
            if(actor.scripted)
            {
                /* A command list's go-to, by its routes */
                if(actor.walk_to &&
                   glm::distance(Vecf2(model.position), Vecf2(*actor.walk_to)) > actor.walk_radius)
                    move_toward(p, ent.id(), actor, model, anim, *kind, *actor.walk_to, dt, false);
                else if(actor.walk_to)
                    stop(p, ent.id(), actor);
                if(actor.may_fight && !actor.blind && (actor.look_in -= dt) <= 0.f)
                {
                    actor.look_in = look_period;
                    if(u64 const seen = look(p, ent.id(), actor, model, *kind->actor); seen)
                    {
                        actor.target    = seen;
                        actor.seen_ago  = 0.f;
                        actor.release   = true;
                        if(auto const* target = p.template get<Model>(seen))
                            actor.last_seen = target->position;
                        enter(actor, state_t::combat);
                    }
                }
                continue;
            }
            actor.walk_to.reset();
            toughen(actor, status, *kind->variant);
            think(p, ent.id(), actor, model, anim, status, *kind, dt);
        }
    }

    /* The difficulty's scale on a row, 1 without a globals block */
    f32 scaled(f32 const (blam::globals::difficulty::*row)[4]) const
    {
        return difficulty_scale(m_difficulty, row, m_level);
    }

    /* Its vitality, once: the variant's where it gives one (a stealth
     * elite's shield of none, a major's 150), else the coll's, scaled for
     * the difficulty by an enemy's or a friend's row */
    void toughen(Actor& actor, BipedStatus& status, actv_t const& actv)
    {
        if(actor.toughened || !status.coll)
            return;
        actor.toughened = true;
        bool const foe   = !m_allegiance->allied(static_cast<team_t>(actor.team), team_t::player);
        f32 const  scale = scaled(
            foe ? &blam::globals::difficulty::enemy_vitality : &blam::globals::difficulty::friend_vitality);
        f32 const body   = actv.body_vitality > 0.f ? actv.body_vitality : status.coll->maximum_body_vitality;
        f32 const shield = actv.body_vitality > 0.f ? actv.shield_vitality : status.coll->maximum_shield_vitality;
        if(body > 0.f)
            status.max_body = body * scale;
        status.max_shield = shield * scale;
        if(shield <= 0.f)
            status.shield = 0.f;
    }

  private:
    static constexpr f32 look_period     = .25f; /* s between looks around */
    static constexpr f32 arrive_radius   = .35f; /* wu */
    static constexpr f32 reposition_time = 4.f;  /* s between picking positions */
    static constexpr f32 default_speed   = 2.f;  /* wu/s, a graph without a stride */
    static constexpr f32 eye_height      = .6f;  /* wu above the feet, a biped without a camera height */
    static constexpr f32 route_height    = .5f;  /* wu above the feet a walk is checked at */
    static constexpr f32 route_knee      = .2f;  /* wu above the feet, for what is low */
    static constexpr f32 body_radius     = .3f;  /* wu either side of a walking body */
    static constexpr f32 steer_ahead     = 2.f;  /* wu ahead obstacles are steered round */
    static constexpr f32 fallback_obstacle = 3.f; /* wu, the most an object's bounds count for */
    static constexpr f32 default_windup  = .35f; /* s before a blow lands, a swing without a key frame */
    static constexpr f32 strike_reach    = 1.5f; /* wu, feet to feet, a blow lands within */
    static constexpr f32 grenade_danger  = 2.5f; /* wu, a grenade without a danger radius is dived from within */
    static constexpr f32 dodge_lookahead = 1.5f; /* s ahead a shot's path is judged */
    static constexpr f32 grenade_notice  = 8.f;  /* wu, a grenade's AI perception radius, for slow shots without one */
    static constexpr f32 slow_shot_speed = 20.f; /* wu/s and under, an explosive shot is seen coming */
    static constexpr f32 grenade_dodge_time = 2.f; /* s spent getting away from one */
    static constexpr f32 spread_weight   = 2.f;  /* wu of cost for a position on a platoon mate's bearing */
    static constexpr f32 flank_weight    = 1.5f; /* wu of cost for a position straight in the target's face */
    static constexpr f32 taunt_chance    = .6f;
    static constexpr f32 taunt_spacing   = 2.5f; /* s between any two taunts */
    static constexpr f32 halo_gravity    = 9.81f / 3.048f; /* wu/s^2 */
    static constexpr f32 tick_rate       = 30.f;
    static constexpr f32 lob_limit       = 1.2217f; /* rad, 70 degrees: steeper lobs fall to the flat shot */
    static constexpr f32 footstep_share  = .25f; /* of the hearing distance, footsteps carry */
    static constexpr f32 footstep_speed  = 1.f;  /* wu/s below which a unit is quiet */
    static constexpr f32 camouflage_sight = 2.f; /* wu within which camouflage is noticed */
    static constexpr f32 flyby_radius    = 3.f;  /* wu a shot passes a head by to be noticed */
    static constexpr f32 drop_lift       = .4f;  /* wu above the feet belongings fall from */
    static constexpr f32 drop_speed      = 1.5f; /* wu/s they are tossed out at */
    static constexpr f32 drop_spread     = 1.3f; /* rad between them */
    static constexpr f32 flyby_reach     = 60.f; /* wu out a passing shot is followed */
    static constexpr f32 link_reach      = 10.f; /* wu between positions a route steps */
    static constexpr u32 max_expansions  = 2000;
    static constexpr f32 stuck_time      = 2.f;  /* s between progress checks */
    static constexpr f32 stuck_distance  = .5f;  /* wu less than which is no progress */
    static constexpr f32 unstick_time    = .7f;  /* s spent stepping aside after stalling */
    static constexpr std::string_view trigger_marker = "primary trigger";

    void think(
        Proxy&               p,
        u64                  id,
        Actor&               actor,
        Model&               model,
        AnimationPlayback&   anim,
        BipedStatus const&   status,
        kind_t const&        kind,
        f32                  dt)
    {
        actr_t const& actr = *kind.actor;
        actv_t const& actv = *kind.variant;
        if(asleep(p, id, actor, anim, status, actr))
            return;
        actor.seen_ago += dt;
        actor.look_in -= dt;
        actor.fire_in -= dt;
        actor.stance_in -= dt;
        actor.state_in -= dt;
        actor.reposition_in -= dt;
        react(p, id, actor, status, actr);

        /* A look around now and then: the nearest enemy in view */
        if(actor.look_in <= 0.f)
        {
            actor.look_in  = look_period;
            u64 const seen = actor.blind ? 0 : actor.follow_players ? nearest_player(p, model) : look(p, id, actor, model, actr);
            if(seen)
            {
                if(seen != actor.target)
                {
                    actor.target  = seen;
                    actor.fire_in = uniform(actv.first_burst_delay_time) *
                                    scaled(&blam::globals::difficulty::new_target_delay);
                    actor.burst_left = 0;
                    alert_squad(p, id, actor, actr);
                }
                if(auto const* target = p.template get<Model>(seen))
                    actor.last_seen = target->position;
                actor.seen_ago = 0.f;
                if(actor.state == state_t::idle || actor.state == state_t::pursuit)
                    enter(actor, state_t::combat);
            }
            dodge_grenades(p, id, actor, model, actr);
        }
        if(actor.state != state_t::retreat)
            actor.danger.reset();
        /* Ordered back: away from its target until told otherwise */
        if(actor.order == Actor::order_t::retreat && actor.state != state_t::retreat)
        {
            enter(actor, state_t::retreat);
            actor.state_in = 1e9f;
        } else if(actor.order != Actor::order_t::retreat && actor.state == state_t::retreat &&
                  actor.state_in > 1e8f)
            actor.state_in = 0.f;
        /* Beaten: give ground for a while */
        if(actor.state == state_t::combat && status.shield <= 0.f && status.body < .5f &&
           actr.unopposable.retreat_time.y > 0.f)
        {
            enter(actor, state_t::retreat);
            actor.state_in = uniform(actr.unopposable.retreat_time);
        }
        /* Shields beaten down to its hiding fraction, it keeps its head
         * down behind its position until they are back to its attacking
         * fraction, or its hiding time is up */
        if(actor.state == state_t::combat && !actor.covering && actr.defensive.hide_shield_fraction > 0.f &&
           status.maximum_shield() > 0.f && status.shield < actr.defensive.hide_shield_fraction)
        {
            actor.covering      = true;
            actor.reposition_in = uniform(actr.defensive.hide_behind_cover_time, 2.f);
        }
        /* Stared down a barrel: a chance to dive for a spot the target
         * cannot see, now and then */
        if(actor.state == state_t::combat && actor.look_in >= look_period - 1e-3f &&
           actor.seen_ago < look_period * 2.f && actr.movement.dive_into_cover_change > 0.f &&
           aimed_at(p, id, actor, model) && chance(actr.movement.dive_into_cover_change * .5f))
        {
            if(i32 const hidden = pick_cover(p, actor, model, kind, actor.last_seen); hidden >= 0)
            {
                actor.position      = hidden;
                actor.goal          = position_of(kind, hidden);
                actor.reposition_in = 3.f + uniform(actr.pursuit.uncover_delay_time, 2.f);
                actor.covering      = true;
            }
        }

        switch(actor.state)
        {
        case state_t::idle:
            /* Enlisted with no post: where it stands is its post */
            if(actor.post == Vecf3{})
                actor.post = model.position;
            /* Migrated: to its new squad's guard positions, running */
            if(actor.migrated && actor.position < 0)
            {
                actor.position = guard_position(p, actor, model, kind);
                if(actor.position >= 0)
                    actor.goal = position_of(kind, actor.position);
            }
            if(actor.migrated && actor.position >= 0)
                move_toward(p, id, actor, model, anim, kind, actor.goal, dt, true);
            else if(!patrol(p, id, actor, model, anim, kind, dt))
                move_toward(p, id, actor, model, anim, kind, actor.post, dt, false);
            glance(actor, model, anim, actr, dt);
            break;
        case state_t::combat: {
            if(!alive(p, actor.target))
            {
                /* Seen fall, it may crow over it in a moment */
                if(actor.seen_ago < look_period * 2.f && chance(taunt_chance))
                {
                    actor.taunt_in     = uniform({.5f, 1.5f});
                    actor.taunt_player = p.template get<PlayerInfo>(actor.target) != nullptr;
                }
                actor.target = 0;
                enter(actor, state_t::idle);
                break;
            }
            if(actor.last_seen == Vecf3{})
                if(auto const* seen = p.template get<Model>(actor.target))
                    actor.last_seen = seen->position;
            if(actor.seen_ago > uniform_max(actr.pursuit.target_search_time))
            {
                enter(actor, state_t::pursuit);
                actor.state_in = uniform_max(actr.pursuit.pursuit_position_time);
                break;
            }
            /* A target within its melee range is charged to arm's reach, and
             * given up past its abort range; with nothing to shoot, or
             * nowhere to fight from, it closes in regardless */
            f32 const  apart  = glm::distance(model.position, actor.last_seen);
            bool const seen   = actor.seen_ago < look_period * 2.f;
            f32 const  begin  = actv.melee_range;
            f32 const  give_up = std::max(actv.melee_abort_range, begin * 1.25f);
            if(begin > 0.f && seen && apart < (actor.charging ? give_up : begin))
                actor.charging = true;
            else if(!seen || apart > give_up || begin <= 0.f)
                actor.charging = false;
            /* To arm's reach to strike; with a gun but nowhere to fight
             * from, only to the far edge of the range it fights at */
            bool const strikes = actor.charging || actor.berserk || !holds_gun(p, id);
            bool const charge  = strikes || actor.position < 0;
            if(!charge && actor.reposition_in <= 0.f)
            {
                actor.reposition_in = reposition_time;
                actor.position      = pick_position(p, actor, model, kind, actor.last_seen, false);
                if(actor.position >= 0)
                    actor.goal = position_of(kind, actor.position);
            }
            f32 const close = strikes ? strike_reach * .8f : std::max(actv.desired_combat_range.y, 2.f);
            bool const there  = charge ? glm::distance(model.position, actor.last_seen) <= close
                                       : glm::distance(model.position, actor.goal) <= arrive_radius;
            if(!there)
                move_toward(p, id, actor, model, anim, kind, charge ? actor.last_seen : actor.goal, dt, true);
            else
            {
                stop(p, id, actor);
                if(actor.covering)
                {
                    /* Down behind it until it dares to look again */
                    anim.stance = "crouch";
                    bool const recovered = actr.defensive.attack_shield_fraction > 0.f &&
                                           status.shield >= actr.defensive.attack_shield_fraction;
                    if(actor.reposition_in <= 0.f || recovered ||
                       (actor.look_in >= look_period - 1e-3f &&
                        chance(actr.movement.emerge_from_cover_chance * .25f)))
                    {
                        actor.covering      = false;
                        actor.reposition_in = 0.f;
                        anim.stance         = "stand";
                    }
                } else
                    crouch_cycle(actor, anim, actv, dt);
            }
            if(actor.seen_ago < look_period * 2.f && !(actor.covering && there))
            {
                /* Its body comes round as its biped turns; it only shoots
                 * what its aim can reach, within the turning threshold */
                turn(actor, model, actor.last_seen, dt, actor.moving);
                if(facing(model, actor.last_seen))
                {
                    if(!strike(p, id, actor, model, kind) && !actor.berserk)
                        fire(p, id, actor, model, kind, dt);
                    throw_grenade(p, id, actor, model, kind);
                }
            }
            break;
        }
        case state_t::pursuit:
            if(actor.seen_ago < look_period * 2.f)
            {
                enter(actor, state_t::combat);
                break;
            }
            /* It searches from its squad's positions nearest where the target
             * was, never off them: not into water under a hovering dropship */
            if(actor.position < 0)
            {
                actor.position = pick_position(p, actor, model, kind, actor.last_seen, false);
                if(actor.position >= 0)
                    actor.goal = position_of(kind, actor.position);
            }
            if(actor.position >= 0 && glm::distance(model.position, actor.goal) > arrive_radius)
                move_toward(p, id, actor, model, anim, kind, actor.goal, dt, true);
            else
                stop(p, id, actor);
            if(actor.state_in <= 0.f)
            {
                actor.target = 0;
                enter(actor, state_t::idle);
            }
            break;
        case state_t::retreat:
            if(actor.position < 0 || actor.reposition_in <= 0.f)
            {
                actor.reposition_in = reposition_time;
                actor.position =
                    pick_position(p, actor, model, kind, actor.danger.value_or(actor.last_seen), true);
                if(actor.position >= 0)
                    actor.goal = position_of(kind, actor.position);
            }
            if(actor.position >= 0 && glm::distance(model.position, actor.goal) > arrive_radius)
                move_toward(p, id, actor, model, anim, kind, actor.goal, dt, true);
            else
                stop(p, id, actor);
            if(actor.state_in <= 0.f)
                enter(actor, alive(p, actor.target) ? state_t::combat : state_t::idle);
            break;
        }
    }

    /* Placed asleep, it lies still and holds fire until it is hurt or roused
     * by gunfire or its squad; a sleeper hears but does not see */
    bool asleep(
        Proxy&             p,
        u64                id,
        Actor&             actor,
        AnimationPlayback& anim,
        BipedStatus const& status,
        actr_t const&      actr)
    {
        if(anim.stance != "asleep")
        {
            actor.rest = -1.f;
            return false;
        }
        f32 const health = status.body + status.shield;
        if(actor.rest < 0.f)
        {
            actor.rest = health;
        }
        bool const woken = actor.target != 0 || actor.state != state_t::idle || health < actor.rest;
        if(!woken)
        {
            stop(p, id, actor);
            return true;
        }
        anim.stance = "stand";
        actor.rest  = -1.f;
        actor.fire_in = std::max(actor.fire_in, 1.f); /* getting up */
        if(actor.target)
            alert_squad(p, id, actor, actr);
        return false;
    }

    /* Blows and stuck grenades change its mind. A grenade stuck on it
     * sends it berserk by its chance, else fleeing for its cowering time;
     * so does a blow past its panic threshold, and one leaving it beaten
     * below its berserk threshold. Berserk, it charges to strike until its
     * target is gone */
    void react(Proxy& p, u64 id, Actor& actor, BipedStatus const& status, actr_t const& actr)
    {
        f32 const took  = actor.hurt_body >= 0.f ? actor.hurt_body - status.body : 0.f;
        actor.hurt_body = status.body;
        bool charge = false, flee = false;
        u64  stuck  = 0;
        for(auto ent : p.template select<Projectile>())
            if(auto const& flight = ent.template get<Projectile>(); flight.attached && flight.attached_to == id)
            {
                stuck = ent.id();
                break;
            }
        if(stuck && stuck != actor.stuck_by)
        {
            if(chance(actr.berserk.berserk_grenade_chance))
                charge = true;
            else
                flee = true;
        }
        actor.stuck_by = stuck;
        if(took > 0.f && actr.panic.panic_damage_threshold > 0.f && took >= actr.panic.panic_damage_threshold)
            flee = true;
        if(took > 0.f && actr.berserk.berserk_damage_amount > 0.f &&
           status.body <= actr.berserk.berserk_damage_threshold &&
           1.f - status.body >= actr.berserk.berserk_damage_amount)
            charge = true;
        if(charge && alive(p, actor.target))
        {
            if(actor.state != state_t::combat)
                enter(actor, state_t::combat);
            actor.berserk = true;
        } else if(flee && !actor.berserk && actor.state != state_t::retreat && actr.panic.cowering_time.y > 0.f)
        {
            enter(actor, state_t::retreat);
            actor.state_in = uniform(actr.panic.cowering_time);
        }
    }

    void enter(Actor& actor, state_t state)
    {
        if(state != state_t::combat)
            actor.berserk = false;
        actor.state         = state;
        actor.position      = -1;
        actor.reposition_in = 0.f;
        actor.stance_in     = 0.f;
        actor.covering      = false;
    }

    bool chance(f32 probability)
    {
        return std::uniform_real_distribution<f32>(0.f, 1.f)(m_random) < probability;
    }

    /* The target looks straight at the actor, with nothing between */
    bool aimed_at(Proxy& p, u64 self, Actor const& actor, Model const& model)
    {
        auto const* target = p.template get<Model>(actor.target);
        if(!target)
            return false;
        Vecf3 forward;
        if(auto const* cam = p.template get<PlayerCamera>(actor.target))
            forward = glm::transpose(Matf3(cam->rotation)) * Vecf3{0.f, 0.f, -1.f};
        else
            forward = target->rotation * Vecf3{1, 0, 0};
        Vecf3 const eye = eye_of(p, actor.target);
        Vecf3 const to  = model.position + chest_of(p, self) - eye;
        f32 const   d   = glm::length(to);
        if(d < 1e-3f || glm::dot(to / d, glm::normalize(forward)) < std::cos(glm::radians(6.f)))
            return false;
        return clear(p, actor.target, eye, eye + to, self);
    }

    /* A firing position the target cannot see from where it stands, near
     * the actor */
    i32 pick_cover(Proxy& p, Actor const& actor, Model const& model, kind_t const& kind, Vecf3 const& from)
    {
        auto positions = kind.encounter->firing_positions.data(*s_magic);
        if(!positions.has_value())
            return -1;
        Vecf3 const eye = from + Vecf3{0, 0, eye_height};
        i32 best = -1;
        f32 best_d = 8.f;
        for(size_t i = 0; i < positions.value().size(); i++)
        {
            Vecf3 const at = positions.value()[i].position;
            f32 const   d  = glm::distance(at, model.position);
            if(d >= best_d || d < .5f || taken(p, actor, kind, static_cast<i32>(i)))
                continue;
            /* Seen there? Then it is no cover */
            if(clear(p, actor.target, eye, at + Vecf3{0, 0, eye_height * .5f}, 0))
                continue;
            best   = static_cast<i32>(i);
            best_d = d;
        }
        return best;
    }

    /* ---- Platoons ---- */

    struct platoon_t
    {
        bool defending{false};
        bool started{false};
        u32  alive{0};
        u32  total{0};
        u32  leaders{0}; /*!< Living actors of the leader type */
    };

    /* Each platoon counts its living, switches between attacking and
     * defending by its cause, and its followers panic without leaders */
    void muster(Proxy& p)
    {
        using cause_t = blam::scn::ai::platoon::cause_t;
        std::map<std::pair<i16, i16>, platoon_t> now;
        for(auto ent : p.template select<Actor, BipedStatus>())
        {
            auto [actor, status] = ent.components();
            if(!actor.active())
                continue;
            auto const* kind = kind_of(p, actor);
            if(!kind || !kind->squad)
                continue;
            auto key = std::pair{actor.encounter, static_cast<i16>(kind->squad->platoon)};
            auto& pl = now[key];
            pl.total++;
            if(!status.dead)
            {
                pl.alive++;
                if(kind->actor && leads(*kind->actor))
                    pl.leaders++;
            }
        }
        for(auto& [key, pl] : now)
        {
            auto& kept = m_platoons[key];
            kept.alive = pl.alive;
            kept.total = pl.total;
            auto platoons = platoons_of(p, key.first);
            if(!platoons.has_value() || key.second < 0 ||
               static_cast<size_t>(key.second) >= platoons.value().size())
                continue;
            auto const& tag = platoons.value()[key.second];
            using flags_t   = blam::scn::ai::platoon::flags_t;
            if(!kept.started)
            {
                kept.started   = true;
                kept.defending = (tag.flags & flags_t::start_in_defending_state) != 0;
                kept.leaders   = pl.leaders;
            }
            /* The cause watches a platoon: another, or this one */
            auto const& cause = tag.change_attacking_defending;
            auto const  watched_key = std::pair{key.first, cause.target >= 0 ? cause.target : key.second};
            auto        watched = now.find(watched_key);
            if(cause.event != cause_t::never && watched != now.end() && holds(cause.event, watched->second))
                kept.defending = !kept.defending;
            /* Leaders gone: the followers scatter */
            if(pl.leaders == 0 && kept.leaders > 0)
                panic(p, key);
            kept.leaders = pl.leaders;
        }
    }

    static bool holds(blam::scn::ai::platoon::cause_t::event_t event, platoon_t const& pl)
    {
        using event_t = blam::scn::ai::platoon::cause_t::event_t;
        f32 const strength = pl.total ? static_cast<f32>(pl.alive) / pl.total : 0.f;
        u32 const dead     = pl.total - pl.alive;
        switch(event)
        {
        case event_t::under_75_str: return strength < .75f;
        case event_t::under_50_str: return strength < .5f;
        case event_t::under_25_str: return strength < .25f;
        case event_t::any_dead: return dead >= 1;
        case event_t::dead_25: return dead * 4 >= pl.total;
        case event_t::dead_50: return dead * 2 >= pl.total;
        case event_t::dead_75: return dead * 4 >= pl.total * 3;
        case event_t::one_alive: return pl.alive == 1;
        case event_t::all_dead: return pl.alive == 0;
        default: return false;
        }
    }

    /* An actor whose type is its own leader type leads */
    static bool leads(actr_t const& actr)
    {
        return actr.panic.leader_type == actr.type || actr.panic.leader_type == blam::scn::actor_type_t::none;
    }

    void panic(Proxy& p, std::pair<i16, i16> const& key)
    {
        for(auto ent : p.template select<Actor, BipedStatus>())
        {
            auto [actor, status] = ent.components();
            if(!actor.active() || status.dead)
                continue;
            auto const* kind = kind_of(p, actor);
            if(!kind || !kind->squad || !kind->actor || actor.encounter != key.first ||
               static_cast<i16>(kind->squad->platoon) != key.second || leads(*kind->actor))
                continue;
            if(!chance(kind->actor->panic.leader_killed_panic_change))
                continue;
            enter(actor, state_t::retreat);
            actor.state_in = uniform(kind->actor->panic.cowering_time, 5.f);
        }
    }

    bool defending(Proxy& p, Actor const& actor, kind_t const& kind)
    {
        if(actor.order == Actor::order_t::defend)
            return true;
        if(actor.order == Actor::order_t::attack)
            return false;
        auto it = m_platoons.find(std::pair{actor.encounter, static_cast<i16>(kind.squad->platoon)});
        return it != m_platoons.end() && it->second.defending;
    }

    u64 nearest_player(Proxy& p, Model const& model)
    {
        u64 best   = 0;
        f32 best_d = std::numeric_limits<f32>::max();
        for(auto player : p.template select<PlayerInfo, Model, BipedStatus>())
        {
            auto [info, target, status] = player.components();
            if(!info.mode.physics || status.dead)
                continue;
            if(f32 const d = glm::distance(target.position, model.position); d < best_d)
            {
                best_d = d;
                best   = player.id();
            }
        }
        return best;
    }

    std::optional<semantic::Span<blam::scn::ai::platoon const>> platoons_of(Proxy&, i16 encounter)
    {
        auto encounters = m_scenario->ai.encounters.data(*s_magic);
        if(!encounters.has_value() || encounter < 0 || static_cast<size_t>(encounter) >= encounters.value().size())
            return std::nullopt;
        auto platoons = encounters.value()[encounter].platoons.data(*s_magic);
        if(!platoons.has_value())
            return std::nullopt;
        return platoons.value();
    }

    /* ---- Perception ---- */

    /* Where a unit looks from, as a player's camera sits: its biped's
     * camera height above its feet, lower when crouched */
    Vecf3 eye_of(Proxy& p, u64 id)
    {
        auto const* model = p.template get<Model>(id);
        if(!model)
            return {};
        if(auto const* cam = p.template get<PlayerCamera>(id); cam && p.template get<PlayerInfo>(id))
            return cam->camera.position;
        f32 height = eye_height;
        if(model->origin_object)
            if(auto biped = model->origin_object->template data<blam::scn::biped>(*s_magic); biped.has_value())
            {
                auto const* anim     = p.template get<AnimationPlayback>(id);
                bool const  crouched = anim && anim->stance == "crouch";
                f32 const   tag = crouched ? biped.value()[0].crouching_camera_height
                                           : biped.value()[0].standing_camera_height;
                if(tag > 0.f)
                    height = tag;
            }
        return model->position + Vecf3{0, 0, height};
    }

    /* What is aimed at on a unit, above its feet: its chest */
    Vecf3 chest_of(Proxy& p, u64 id)
    {
        auto const* model = p.template get<Model>(id);
        return model ? (eye_of(p, id) - model->position) * .8f : Vecf3{0, 0, eye_height * .8f};
    }

    /* The held gun's muzzle as drawn in its hand */
    std::optional<Vecf3> trigger_of(Proxy& p, BipedEquipment::weapon_t const& gun)
    {
        auto const* model = gun.held ? p.template get<Model>(gun.held) : nullptr;
        if(!model || !model->tag)
            return std::nullopt;
        auto marker = bind_marker_node(*model->tag, trigger_marker, *s_magic);
        if(!marker)
            return std::nullopt;
        Matf4       at   = model->transform;
        auto const* anim = p.template get<AnimationPlayback>(gun.held);
        if(anim && marker->node >= 0 && static_cast<size_t>(marker->node) < anim->pose.size())
            at = at * anim->pose[static_cast<size_t>(marker->node)];
        return Vecf3((at * marker->transform)[3]);
    }

    /* The nearest enemy in the actor's vision, with a clear line to it */
    u64 look(Proxy& p, u64 id, Actor const& actor, Model const& model, actr_t const& actr, bool all_round = false)
    {
        auto const& seen    = actr.perception;
        Vecf3 const eye     = eye_of(p, id);
        Vecf3 const forward = model.rotation * Vecf3{1, 0, 0};
        f32 const   reach   = seen.max_vision_dist > 0.f ? seen.max_vision_dist : 30.f;
        f32 const   central = seen.central_vision > 0.f ? seen.central_vision : glm::radians(45.f);
        f32 const   widest  = std::max(seen.max_vision, central);
        f32 const   corner  = std::max(seen.peripheral_vision, widest);
        f32 const   heard   = (seen.hearing_dist > 0.f ? seen.hearing_dist : 15.f) * footstep_share;
        u64   best     = 0;
        f32   best_d   = std::numeric_limits<f32>::max();
        auto  consider = [&](u64 other, Model const& target, u64 vehicle, bool quiet, bool cloaked) {
            Vecf3 const to = target.position + chest_of(p, other) - eye;
            f32 const   d  = glm::length(to);
            if(d >= best_d || d < 1e-3f)
                return;
            /* Camouflage goes unseen and unheard but up close; its shots
             * still give it away (hear) */
            if(cloaked && d > camouflage_sight)
                return;
            /* Out to full range ahead, less toward the edge of its sight,
             * a short way out of the corner of its eye; a known target is
             * kept a little wider */
            f32 const angle = all_round ? 0.f
                                        : std::acos(std::clamp(glm::dot(to / d, forward), -1.f, 1.f)) /
                                              (other == actor.target ? 1.5f : 1.f);
            f32 range = 0.f;
            if(angle <= central)
                range = reach;
            else if(angle <= widest)
                range = glm::mix(reach, reach * .5f, (angle - central) / std::max(widest - central, 1e-3f));
            else if(angle <= corner)
                range = seen.peripheral_dist;
            bool const sighted = d <= range;
            /* Footsteps close by give away who does not creep */
            bool const footsteps = !quiet && !vehicle && d <= heard;
            if(!sighted && !footsteps)
                return;
            /* Inside a vehicle it is the vehicle the eye meets, and a
             * vehicle is noticed by the actor's chance for it */
            if(sighted && !footsteps)
            {
                if(!clear(p, id, eye, eye + to, other, vehicle))
                    return;
                if(vehicle && other != actor.target && !chance(seen.vehicle_awareness > 0.f ? seen.vehicle_awareness : 1.f))
                    return;
            }
            best   = other;
            best_d = d;
        };
        auto moving = [&p](u64 other) {
            auto const* physics = p.template get<ObjectPhysics>(other);
            Vecf3 const v       = physics ? physics->linear_velocity : Vecf3{};
            return v.x * v.x + v.y * v.y > footstep_speed * footstep_speed;
        };
        for(auto player : p.template select<PlayerInfo, Model, BipedStatus>())
        {
            auto [info, target, status] = player.components();
            if(!info.mode.physics || status.dead || !enemies(actor.team, team_t::player))
                continue;
            consider(player.id(), target, info.riding.vehicle, info.crouched || !moving(player.id()),
                     info.camouflaged || target.camouflaged);
        }
        for(auto other : p.template select<Actor, Model, BipedStatus>())
        {
            auto [theirs, target, status] = other.components();
            if(other.id() == id || !theirs.active() || status.dead || !enemies(actor.team, static_cast<team_t>(theirs.team)))
                continue;
            auto const* physics = p.template get<ObjectPhysics>(other.id());
            auto const* anim    = p.template get<AnimationPlayback>(other.id());
            u64 const   vehicle = physics && physics->seated ? physics->seated_in : 0;
            consider(other.id(), target, vehicle, (anim && anim->stance == "crouch") || !moving(other.id()),
                     target.camouflaged);
        }
        return best;
    }

    /* A unit of its own side standing between its muzzle and its aim */
    bool friend_before(Proxy& p, u64 self, Actor const& actor, Vecf3 const& from, Vecf3 const& to)
    {
        Physics::Event   ev{Physics::Event::Raycast};
        Physics::Raycast ray{.from = from, .to = to, .ignore = self};
        p.template subsystem<PhysicsBus>().process(ev, &ray);
        if(!ray.hit || ray.entity_id == 0 || ray.entity_id == actor.target)
            return false;
        auto const* theirs = p.template get<Actor>(ray.entity_id);
        if(theirs)
            return !enemies(actor.team, static_cast<team_t>(theirs->team));
        return p.template get<PlayerInfo>(ray.entity_id) && !enemies(actor.team, team_t::player);
    }

    /* A projectile the AI notices: one its tag gives an AI perception
     * radius (grenades), or a slow explosive shot (fuel rod, rocket,
     * mortar) noticed as a grenade is. A grenade armed within its danger
     * radius, or a shot about to pass within it: by its chance to dive, it
     * gets away from where that will be for a moment */
    void dodge_grenades(Proxy& p, u64, Actor& actor, Model const& model, actr_t const& actr)
    {
        if(actr.movement.dive_from_grenade_chance <= 0.f || actor.state == state_t::retreat)
            return;
        for(auto ent : p.template select<Projectile, Model>())
        {
            auto [flight, where] = ent.components();
            if(!flight.tag || flight.attached)
                continue;
            bool const slow_shot = flight.tag->detonation_effect.valid() &&
                                   flight.tag->initial_velocity * tick_rate > 0.f &&
                                   flight.tag->initial_velocity * tick_rate <= slow_shot_speed;
            f32 const noticed = flight.tag->ai_perception_radius > 0.f ? flight.tag->ai_perception_radius
                                : slow_shot                             ? grenade_notice
                                                                        : 0.f;
            if(noticed <= 0.f)
                continue;
            f32 const danger = flight.tag->danger_radius > 0.f ? flight.tag->danger_radius : grenade_danger;
            Vecf3 const to    = model.position - where.position;
            if(glm::length(to) > noticed)
                continue;
            /* Where it comes nearest over the next while, on its way */
            Vecf3 nearest = where.position;
            f32 const speed2 = glm::dot(flight.velocity, flight.velocity);
            if(speed2 > 1.f)
            {
                f32 const t = std::clamp(glm::dot(to, flight.velocity) / speed2, 0.f, dodge_lookahead);
                nearest     = where.position + flight.velocity * t;
            } else if(flight.timer < 0.f)
                continue; /* lying there unarmed */
            if(glm::distance(nearest, model.position) > danger)
                continue;
            if(!chance(actr.movement.dive_from_grenade_chance))
                return;
            actor.danger = nearest;
            enter(actor, state_t::retreat);
            actor.state_in = grenade_dodge_time;
            return;
        }
    }

    bool clear(Proxy& p, u64 self, Vecf3 const& from, Vecf3 const& to, u64 target, u64 vehicle = 0)
    {
        Physics::Event   ev{Physics::Event::Raycast};
        Physics::Raycast ray{.from = from, .to = to, .ignore = self};
        p.template subsystem<PhysicsBus>().process(ev, &ray);
        return !ray.hit || ray.entity_id == target || (vehicle && ray.entity_id == vehicle);
    }

    /* Squad mates within earshot take the target too */
    void alert_squad(Proxy& p, u64 id, Actor const& actor, actr_t const& actr)
    {
        auto const* model = p.template get<Model>(id);
        if(!model)
            return;
        f32 const reach = actr.perception.hearing_dist > 0.f ? actr.perception.hearing_dist : 15.f;
        for(auto other : p.template select<Actor, Model, BipedStatus>())
        {
            auto [theirs, target, status] = other.components();
            if(other.id() == id || theirs.encounter != actor.encounter || theirs.squad != actor.squad ||
               status.dead || theirs.target != 0 ||
               glm::distance(target.position, model->position) > reach)
                continue;
            theirs.target    = actor.target;
            theirs.last_seen = actor.last_seen;
            theirs.seen_ago  = look_period * 1.5f;
            theirs.fire_in   = std::max(theirs.fire_in, 1.f);
            if(theirs.state == state_t::idle)
                enter(theirs, state_t::combat);
        }
    }

    /* Shots fired nearby turn idle heads: the shooter becomes the target
     * where it is an enemy, else the actor goes to look */
    void hear(Proxy& p)
    {
        for(auto const& shot : std::exchange(m_heard, {}))
        {
            for(auto ent : p.template select<Actor, Model, BipedStatus>())
            {
                auto [actor, model, status] = ent.components();
                if(!actor.active() || actor.scripted || actor.deaf || actor.braindead || status.dead ||
                   ent.id() == shot.shooter)
                    continue;
                /* Busy with a target in view already */
                if(actor.target && actor.seen_ago < look_period * 4.f)
                    continue;
                auto const* kind = kind_of(p, actor);
                auto const* actr = kind ? kind->actor : nullptr;
                f32 const   reach = actr && actr->perception.hearing_dist > 0.f ? actr->perception.hearing_dist : 15.f;
                bool const  near  = glm::distance(model.position, shot.position) <= reach;
                /* A shot past its head, by its chance to notice one */
                Vecf3 const head  = model.position + Vecf3{0, 0, eye_height};
                f32 const   along = glm::dot(head - shot.position, shot.direction);
                bool const  past  = along > 0.f && along < flyby_reach &&
                                   glm::distance(shot.position + shot.direction * along, head) < flyby_radius &&
                                   chance(actr && actr->perception.projectile_awareness > 0.f
                                              ? actr->perception.projectile_awareness
                                              : .5f);
                if(!near && !past)
                    continue;
                actor.last_seen = shot.position;
                actor.seen_ago  = 1e9f;
                if(enemy_unit(p, actor, shot.shooter))
                {
                    actor.target = shot.shooter;
                    if(actor.state != state_t::combat)
                        enter(actor, state_t::combat);
                } else if(actor.state == state_t::idle)
                {
                    enter(actor, state_t::pursuit);
                    actor.state_in = 6.f;
                }
            }
        }
    }

    bool enemy_unit(Proxy& p, Actor const& actor, u64 unit)
    {
        if(auto const* info = p.template get<PlayerInfo>(unit))
            return info->mode.physics && enemies(actor.team, team_t::player);
        if(auto const* other = p.template get<Actor>(unit))
            return other->active() && enemies(actor.team, static_cast<team_t>(other->team));
        return false;
    }

    /* Teams fight unless allied, and a betrayed team fights the players */
    bool enemies(u16 mine, team_t theirs) const
    {
        return m_allegiance->hostile(static_cast<team_t>(mine), theirs);
    }

    bool alive(Proxy& p, u64 unit)
    {
        auto const* status = p.template get<BipedStatus>(unit);
        return unit != 0 && status && !status->dead;
    }

    /* ---- Positions ---- */

    static Vecf3 position_of(kind_t const& kind, i32 index)
    {
        auto positions = kind.encounter->firing_positions.data(*s_magic);
        if(!positions.has_value() || index < 0 || static_cast<size_t>(index) >= positions.value().size())
            return {};
        return positions.value()[index].position;
    }

    /* The encounter's firing positions the squad may use in its state, in
     * its variant's range of the target: the nearest to the actor among
     * the fit, or the farthest from the target when giving ground */
    i32 pick_position(
        Proxy& p, Actor const& actor, Model const& model, kind_t const& kind, Vecf3 const& target, bool away)
    {
        auto positions = kind.encounter->firing_positions.data(*s_magic);
        if(!positions.has_value() || positions.value().empty())
            return -1;
        bool const hold   = away || defending(p, actor, kind);
        u32 const  groups = hold ? kind.squad->defending | kind.squad->defending_search
                                 : kind.squad->attacking | kind.squad->attacking_search;
        Vecf2 const range = kind.variant->desired_combat_range.y > 0.f
                                ? kind.variant->desired_combat_range
                                : Vecf2{3.f, 12.f};
        /* Mates of its encounter spread out round the target rather than
         * all come at it one way, and a spot in the target's face costs
         * more than one off to its side */
        std::vector<Vecf2> bearings;
        if(!away)
            for(auto other : p.template select<Actor>())
            {
                auto const& theirs = other.template get<Actor>();
                if(&theirs != &actor && theirs.encounter == actor.encounter && theirs.position >= 0 &&
                   static_cast<size_t>(theirs.position) < positions.value().size())
                {
                    Vecf2 const b = Vecf2(positions.value()[static_cast<size_t>(theirs.position)].position) - Vecf2(target);
                    if(glm::dot(b, b) > 1e-4f)
                        bearings.push_back(glm::normalize(b));
                }
            }
        Vecf2 const faces = target_facing(p, actor.target);
        i32 best      = -1;
        f32 best_cost = std::numeric_limits<f32>::max();
        for(size_t i = 0; i < positions.value().size(); i++)
        {
            auto const& fp = positions.value()[i];
            if(groups != 0 && fp.group < 32 && ((groups >> fp.group) & 1u) == 0)
                continue;
            if(taken(p, actor, kind, static_cast<i32>(i)))
                continue;
            f32 const to_target = glm::distance(fp.position, target);
            f32 const to_me     = glm::distance(fp.position, model.position);
            f32 cost;
            if(away)
                cost = -to_target + to_me * .25f;
            else
            {
                f32 const outside = to_target < range.x ? range.x - to_target
                                    : to_target > range.y ? to_target - range.y
                                                          : 0.f;
                cost = outside * 3.f + to_me;
                Vecf2 const b = Vecf2(fp.position) - Vecf2(target);
                if(glm::dot(b, b) > 1e-4f)
                {
                    Vecf2 const dir = glm::normalize(b);
                    for(auto const& mate : bearings)
                        cost += spread_weight * std::max(glm::dot(dir, mate), 0.f);
                    cost += flank_weight * std::max(glm::dot(dir, faces), 0.f);
                }
            }
            if(cost < best_cost)
            {
                best_cost = cost;
                best      = static_cast<i32>(i);
            }
        }
        return best;
    }

    /* Which way the target looks, flat; zero for one with no facing */
    Vecf2 target_facing(Proxy& p, u64 target)
    {
        Vecf3 forward{};
        if(auto const* cam = p.template get<PlayerCamera>(target))
            forward = glm::transpose(Matf3(cam->rotation)) * Vecf3{0.f, 0.f, -1.f};
        else if(auto const* model = p.template get<Model>(target))
            forward = model->rotation * Vecf3{1.f, 0.f, 0.f};
        Vecf2 const flat(forward);
        return glm::dot(flat, flat) > 1e-4f ? glm::normalize(flat) : Vecf2{};
    }

    /* Placed to patrol: it walks its squad's move positions, in order, back
     * and forth or at random by their weights, staying at each its time.
     * False when it has no patrol */
    bool patrol(Proxy& p, u64 id, Actor& actor, Model& model, AnimationPlayback& anim, kind_t const& kind, f32 dt)
    {
        using ai_state_t = blam::scn::ai::state_t;
        auto const mode  = static_cast<ai_state_t>(actor.idle_state);
        if(mode < ai_state_t::moving_repeat_same_position || mode > ai_state_t::moving_randomly)
            return false;
        auto moves = kind.squad->move_positions.data(*s_magic);
        if(!moves.has_value() || moves.value().empty())
            return false;
        auto const& all = moves.value();
        i32 const   n   = static_cast<i32>(all.size());
        auto pick = [&]() -> i32 {
            switch(mode)
            {
            case ai_state_t::moving_repeat_same_position:
                return actor.move_at < 0 ? static_cast<i32>(m_random() % n) : actor.move_at;
            case ai_state_t::moving_loop:
                return (actor.move_at + 1) % n;
            case ai_state_t::moving_loop_back_and_forth: {
                if(n == 1)
                    return 0;
                if(actor.move_at < 0)
                    return 0;
                if(actor.move_at + (actor.move_back ? -1 : 1) < 0 || actor.move_at + (actor.move_back ? -1 : 1) >= n)
                    actor.move_back = !actor.move_back;
                return actor.move_at + (actor.move_back ? -1 : 1);
            }
            default: {
                f32 total = 0.f;
                for(auto const& m : all)
                    total += std::max(m.weight, 1e-3f);
                f32 roll = std::uniform_real_distribution<f32>(0.f, total)(m_random);
                for(i32 i = 0; i < n; i++)
                    if((roll -= std::max(all[i].weight, 1e-3f)) <= 0.f)
                        return i;
                return n - 1;
            }
            }
        };
        if(actor.move_at < 0)
            actor.move_at = pick();
        auto const& at = all[static_cast<size_t>(actor.move_at)];
        if(glm::distance(Vecf2(model.position), Vecf2(at.position)) > arrive_radius * 2.f)
        {
            move_toward(p, id, actor, model, anim, kind, at.position, dt, false);
            actor.move_wait = uniform(at.time, 2.f);
            return true;
        }
        stop(p, id, actor);
        if((actor.move_wait -= dt) <= 0.f && mode != ai_state_t::moving_repeat_same_position)
        {
            actor.move_at   = pick();
            actor.move_wait = 0.f;
        } else if(actor.move_wait > 0.f && actor.glance_in > 1.f)
            model.rotation = glm::angleAxis(at.facing, Vecf3{0.f, 0.f, 1.f});
        return true;
    }

    /* Idle, it looks somewhere else now and then */
    void glance(
        Actor&             actor,
        Model&             model,
        AnimationPlayback& anim,
        actr_t const&      actr,
        f32                dt)
    {
        /* The head wanders within its idle range more often than the body turns */
        if((actor.gaze_in -= dt) <= 0.f)
        {
            actor.gaze_in = uniform(actr.looking.noncombat_idle_looking, 2.f);
            auto const range = actr.looking.idle_looking_range;
            auto       pick  = [this](f32 r) {
                return r > 0.f ? std::uniform_real_distribution<f32>(-r, r)(
                                     m_random)
                                      : 0.f;
            };
            f32 const   yaw   = pick(std::min(range.yaw, glm::radians(70.f)));
            f32 const   pitch = pick(std::min(range.pitch, glm::radians(30.f)));
            Vecf3 const dir   = glm::angleAxis(yaw, Vecf3{0.f, 0.f, 1.f}) *
                              model.rotation *
                              Vecf3{std::cos(pitch), 0.f, std::sin(pitch)};
            anim.gaze(anim.aiming, dir, actor.gaze_in);
        }
        if(actor.moving || (actor.glance_in -= dt) > 0.f)
            return;
        actor.glance_in = uniform(actr.looking.noncombat_idle_facing, 4.f);
        f32 const turn  = std::uniform_real_distribution<f32>(-1.f, 1.f)(m_random) * glm::radians(60.f);
        model.rotation  = glm::normalize(glm::angleAxis(turn, Vecf3{0.f, 0.f, 1.f}) * model.rotation);
    }

    /* Out of a fight: the nearest free position of its squad's guard
     * groups, or of its fighting groups when it has none */
    i32 guard_position(Proxy& p, Actor const& actor, Model const& model, kind_t const& kind)
    {
        auto positions = kind.encounter->firing_positions.data(*s_magic);
        if(!positions.has_value())
            return -1;
        bool const hold   = defending(p, actor, kind);
        u32        groups = hold ? kind.squad->defending_guard : kind.squad->attacking_guard;
        if(groups == 0)
            groups = hold ? kind.squad->defending : kind.squad->attacking;
        i32 best    = -1;
        f32 nearest = std::numeric_limits<f32>::max();
        for(size_t i = 0; i < positions.value().size(); i++)
        {
            auto const& fp = positions.value()[i];
            if(groups != 0 && fp.group < 32 && ((groups >> fp.group) & 1u) == 0)
                continue;
            if(taken(p, actor, kind, static_cast<i32>(i)))
                continue;
            if(f32 const d = glm::distance(fp.position, model.position); d < nearest)
            {
                nearest = d;
                best    = static_cast<i32>(i);
            }
        }
        return best;
    }

    /* A squad mate already heading there */
    bool taken(Proxy& p, Actor const& actor, kind_t const&, i32 index)
    {
        for(auto other : p.template select<Actor>())
        {
            auto const& theirs = other.template get<Actor>();
            if(&theirs != &actor && theirs.encounter == actor.encounter && theirs.position == index)
                return true;
        }
        return false;
    }

    /* ---- Motion ---- */

    void move_toward(
        Proxy&              p,
        u64                 id,
        Actor&              actor,
        Model&              model,
        AnimationPlayback&  anim,
        kind_t const&       kind,
        Vecf3 const&        goal,
        f32                 dt,
        bool                running)
    {
        Vecf3 rest = goal - model.position;
        rest.z     = 0.f;
        if(glm::length(rest) <= arrive_radius)
        {
            stop(p, id, actor);
            return;
        }
        /* Around what is in the way, by its encounter's positions */
        Vecf3 to = waypoint(p, id, actor, model, kind, goal, dt) - model.position;
        to.z     = 0.f;
        /* And round an obstacle just ahead, on the side nearer its way */
        if(glm::dot(to, to) > 1e-6f)
            if(auto const* o = obstacle_on(
                   model.position, model.position + glm::normalize(to) * std::min(glm::length(to), steer_ahead), actor.board))
            {
                Vecf3 off = model.position - o->center;
                off.z     = 0.f;
                Vecf3 const round = glm::cross(Vecf3{0, 0, 1}, glm::normalize(off));
                bool const  left  = (glm::dot(round, to) >= 0.f) != actor.steer_other;
                to                = (left ? round : -round) * glm::length(to);
            }
        /* Standing in one, it edges out as it goes */
        if(auto const* o = obstacle_at(model.position, goal, actor.board); o && glm::dot(to, to) > 1e-6f)
        {
            Vecf3 out = model.position - o->center;
            out.z     = 0.f;
            if(glm::dot(out, out) > 1e-6f)
                to = glm::normalize(glm::normalize(to) + glm::normalize(out)) * glm::length(to);
        }
        /* Held by something unseen, it steps aside for a moment */
        if(actor.unstick_in > 0.f && glm::dot(to, to) > 1e-6f)
        {
            actor.unstick_in -= dt;
            Vecf3 const side = glm::cross(Vecf3{0, 0, 1}, glm::normalize(to));
            to               = (actor.steer_other ? -side : side) * std::max(glm::length(to), 1.f);
        }
        f32 const left = std::max(glm::length(to), 1e-3f);
        if(anim.stance != "stand")
            anim.stance = "stand";
        f32 const speed = stride(p, id, anim, kind) * (running ? 1.f : .6f);
        Vecf3 const dir = to / left;
        if(actor.seen_ago > look_period * 2.f || actor.state != state_t::combat ||
           !can_stride(p, id, anim, model, dir))
            turn(actor, model, model.position + dir, dt, true);
        Physics::Event    ev{Physics::Event::Velocity};
        Physics::Velocity walking{
            .entity_id = id, .velocity = dir * std::min(speed, left / std::max(dt, 1e-3f)), .preserve_z = true};
        p.template subsystem<PhysicsBus>().process(ev, &walking);
        actor.moving = true;
    }

    /* ---- Routes ---- */

    /* Nothing solid at waist height between two points; units in the way
     * move on */
    bool walkable(Proxy& p, u64 self, Vecf3 const& a, Vecf3 const& b, u64 aboard = 0)
    {
        /* A body's width, at the knee and the waist */
        Vecf3 flat = b - a;
        flat.z     = 0.f;
        Vecf3 const side = glm::dot(flat, flat) > 1e-6f ? glm::normalize(Vecf3{-flat.y, flat.x, 0.f}) : Vecf3{};
        for(f32 const height : {route_knee, route_height})
            for(f32 const across : {0.f, body_radius, -body_radius})
            {
                Vecf3 const lift = Vecf3{0, 0, height} + side * across;
                Physics::Event   ev{Physics::Event::Raycast};
                Physics::Raycast ray{.from = a + lift, .to = b + lift, .ignore = self};
                p.template subsystem<PhysicsBus>().process(ev, &ray);
                if(ray.hit && !(ray.entity_id != 0 && (ray.entity_id == aboard || p.template get<Actor>(ray.entity_id) ||
                                                       p.template get<PlayerInfo>(ray.entity_id))))
                    return false;
            }
        /* Around what the AI is told to keep clear of */
        return !obstacle_on(a, b, aboard);
    }

    /* The obstacle a point stands inside, unless the goal is in it too */
    obstacle_t const* obstacle_at(Vecf3 const& at, Vecf3 const& goal, u64 aboard = 0) const
    {
        for(auto const& o : m_obstacles)
            if(o.entity != aboard && std::abs(o.center.z - at.z) < o.radius + 1.f &&
               glm::distance(Vecf2(at), Vecf2(o.center)) < o.radius + body_radius &&
               glm::distance(Vecf2(goal), Vecf2(o.center)) >= o.radius + body_radius)
                return &o;
        return nullptr;
    }

    /* The first object pathfinding sphere a walk from a to b runs into */
    obstacle_t const* obstacle_on(Vecf3 const& a, Vecf3 const& b, u64 aboard = 0) const
    {
        Vecf2 const      from{a}, to{b};
        Vecf2 const      step = to - from;
        f32 const        len2 = glm::dot(step, step);
        obstacle_t const* first = nullptr;
        f32               along = std::numeric_limits<f32>::max();
        for(auto const& o : m_obstacles)
        {
            if(o.entity == aboard || o.center.z + o.radius < std::min(a.z, b.z) - .5f ||
               o.center.z - o.radius > std::max(a.z, b.z) + 1.5f)
                continue;
            Vecf2 const c{o.center};
            f32 const   t = len2 > 0.f ? std::clamp(glm::dot(c - from, step) / len2, 0.f, 1.f) : 0.f;
            f32 const   d = glm::distance(from + step * t, c);
            /* Already inside one, or going into one: only what lies
             * between counts */
            if(glm::distance(from, c) < o.radius + body_radius || glm::distance(to, c) < o.radius + body_radius)
                continue;
            if(d < o.radius + body_radius && t < along)
            {
                along = t;
                first = &o;
            }
        }
        return first;
    }

    /* Objects' pathfinding spheres, as they stand, every second */
    void gather_obstacles(Proxy& p)
    {
        m_obstacles.clear();
        auto const& files = p.template subsystem<BlamFiles<halo_version>>();
        blam::tag_index_view<halo_version> index(files.container);
        for(auto ent : p.template select<Model>())
        {
            auto const& model = ent.template get<Model>();
            auto const* tag   = model.origin_object;
            if(!tag || p.template get<Actor>(ent.id()) || p.template get<PlayerInfo>(ent.id()) ||
               !(tag->matches(blam::tag_class_t::scen) || tag->matches(blam::tag_class_t::vehi) ||
                 tag->matches(blam::tag_class_t::mach) || tag->matches(blam::tag_class_t::ctrl)))
                continue;
            auto [spheres, fresh] = m_spheres.try_emplace(tag);
            if(fresh)
                if(auto object = tag->template data<blam::scn::object>(*s_magic); object.has_value())
                {
                    if(auto coll_tag = index.find(object.value()[0].collider.tag_id); coll_tag != index.end())
                        if(auto coll = (*coll_tag).template data<blam::coll::header>(*s_magic); coll.has_value())
                            if(auto list = coll.value()[0].pathfinding_spheres.data(*s_magic); list.has_value())
                                for(auto const& s : list.value())
                                    spheres->second.push_back({s.center, s.radius});
                    /* Scenery with none, such as boulders and crates, is kept
                     * clear of by its bounds, which are generous */
                    auto const& o = object.value()[0];
                    if(spheres->second.empty() && !tag->matches(blam::tag_class_t::vehi) && o.bound_radius > 0.f)
                        spheres->second.push_back({o.bound_offset, std::min(o.bound_radius, fallback_obstacle) * .7f});
                }
            for(auto const& [center, radius] : spheres->second)
                m_obstacles.push_back({
                    .center = Vecf3(model.transform * Vecf4(center, 1.f)),
                    .radius = radius,
                    .entity = ent.id(),
                });
        }
    }

    /* The positions a position has a clear walk to, found once each */
    std::vector<i32> const& links(Proxy& p, kind_t const& kind, i16 encounter, i32 node)
    {
        auto positions = kind.encounter->firing_positions.data(*s_magic);
        auto& table    = m_links[encounter];
        if(table.size() != positions.value().size())
            table.assign(positions.value().size(), std::nullopt);
        auto& out = table[static_cast<size_t>(node)];
        if(!out)
        {
            out.emplace();
            Vecf3 const at = positions.value()[static_cast<size_t>(node)].position;
            for(size_t j = 0; j < positions.value().size(); j++)
                if(static_cast<i32>(j) != node &&
                   glm::distance(at, positions.value()[j].position) <= link_reach &&
                   walkable(p, 0, at, positions.value()[j].position))
                    out->push_back(static_cast<i32>(j));
        }
        return *out;
    }

    /* The shortest chain of positions from near `from` to near `to` */
    std::vector<i32> plan(Proxy& p, u64 id, kind_t const& kind, i16 encounter, Vecf3 const& from, Vecf3 const& to)
    {
        auto positions = kind.encounter->firing_positions.data(*s_magic);
        if(!positions.has_value() || positions.value().empty())
            return {};
        auto const& all = positions.value();
        auto near = [&](Vecf3 const& at) {
            std::set<i32> out;
            for(size_t i = 0; i < all.size(); i++)
                if(glm::distance(all[i].position, at) <= link_reach && walkable(p, id, at, all[i].position))
                    out.insert(static_cast<i32>(i));
            return out;
        };
        std::set<i32> const ends = near(to);
        if(ends.empty())
            return {};
        using entry_t = std::pair<f32, i32>;
        std::priority_queue<entry_t, std::vector<entry_t>, std::greater<>> open;
        std::map<i32, f32> cost;
        std::map<i32, i32> came;
        for(i32 start : near(from))
        {
            f32 const g = glm::distance(from, all[start].position);
            cost[start] = g;
            came[start] = -1;
            open.push({g + glm::distance(all[start].position, to), start});
        }
        for(u32 expanded = 0; !open.empty() && expanded < max_expansions; expanded++)
        {
            auto const [f, node] = open.top();
            open.pop();
            if(ends.contains(node))
            {
                std::vector<i32> route;
                for(i32 at = node; at >= 0; at = came[at])
                    route.insert(route.begin(), at);
                return route;
            }
            for(i32 next : links(p, kind, encounter, node))
            {
                f32 const g = cost[node] + glm::distance(all[node].position, all[next].position);
                if(auto it = cost.find(next); it != cost.end() && it->second <= g)
                    continue;
                cost[next] = g;
                came[next] = node;
                open.push({g + glm::distance(all[next].position, to), next});
            }
        }
        return {};
    }

    /* The section's floors as a graph, built again as the section changes */
    void refresh_nav(Proxy& p)
    {
        auto const&    cache = p.template subsystem<BSPCache<halo_version>>();
        BSPItem const* item  = nullptr;
        for(auto const& [_, candidate] : cache.m_cache)
            if(candidate.valid() && candidate.section_idx == cache.active_section && !candidate.coll_surfaces.empty())
                item = &candidate;
        if(item == m_nav_item)
            return;
        m_nav_item = item;
        m_nav.clear();
        if(item)
        {
            m_nav.build(*item);
            cDebug(
                "ai: nav mesh for section {}: {} floors, {} links", cache.active_section, m_nav.nodes.size(),
                m_nav.links.size());
        }
    }

    /* Where to head now: the goal itself, or the next point of a route
     * around what blocks the way: over the section's floors, else through
     * the encounter's positions; a walk that stalls is planned again */
    Vecf3 waypoint(Proxy& p, u64 id, Actor& actor, Model const& model, kind_t const& kind, Vecf3 const& goal, f32 dt)
    {
        /* The floors know nothing of what stands on them: a first leg the
         * rays find blocked hands over to the positions */
        auto const replan = [&] {
            actor.route.clear();
            actor.path = m_nav.route(model.position, goal);
            if(!actor.path.empty() && !walkable(p, id, model.position, actor.path.front(), actor.board))
                actor.path.clear();
            if(actor.path.empty() && !walkable(p, id, model.position, goal, actor.board))
                actor.route = plan(p, id, kind, actor.encounter, model.position, goal);
        };
        if(glm::distance(goal, actor.route_goal) > 1.f)
        {
            actor.route_goal = goal;
            replan();
            actor.stuck_at = model.position;
            actor.stuck_in = stuck_time;
        }
        if((actor.stuck_in -= dt) <= 0.f)
        {
            /* Jostled about near the goal is not stuck */
            if(glm::distance(model.position, actor.stuck_at) < stuck_distance &&
               glm::distance(Vecf2(model.position), Vecf2(goal)) > stuck_distance * 3.f)
            {
                actor.steer_other = !actor.steer_other;
                actor.unstick_in  = unstick_time;
                replan();
            }
            actor.stuck_at = model.position;
            actor.stuck_in = stuck_time;
        }
        /* Points reached, or that can be cut straight past, are dropped */
        while(actor.path.size() > 1 &&
              (glm::distance(Vecf2(model.position), Vecf2(actor.path.front())) < arrive_radius * 2.f ||
               walkable(p, id, model.position, actor.path[1], actor.board)))
            actor.path.erase(actor.path.begin());
        if(!actor.path.empty())
            return actor.path.front();
        while(!actor.route.empty() &&
              glm::distance(Vecf2(model.position), Vecf2(position_of(kind, actor.route.front()))) < arrive_radius * 2.f)
            actor.route.erase(actor.route.begin());
        return actor.route.empty() ? goal : position_of(kind, actor.route.front());
    }

    void stop(Proxy& p, u64 id, Actor& actor)
    {
        if(!actor.moving)
            return;
        actor.moving = false;
        Physics::Event    ev{Physics::Event::Velocity};
        Physics::Velocity still{.entity_id = id, .velocity = {}, .preserve_z = true};
        p.template subsystem<PhysicsBus>().process(ev, &still);
    }

    /* How a body turns: its biped's turning speed, and how far a standing
     * body lets its facing stray before it comes round at all */
    struct turning_t
    {
        f32 speed{glm::two_pi<f32>()};
        f32 threshold{0.f};
    };

    turning_t turning_of(Model const& model)
    {
        auto [it, fresh] = m_turning.try_emplace(model.origin_object);
        if(fresh && model.origin_object)
            if(auto biped = model.origin_object->template data<blam::scn::biped>(*s_magic); biped.has_value())
            {
                if(biped.value()[0].moving_turning_speed > 0.f)
                    it->second.speed = biped.value()[0].moving_turning_speed;
                it->second.threshold = biped.value()[0].stationary_turning_threshold;
            }
        return it->second;
    }

    static f32 yaw_of(Model const& model)
    {
        Vecf3 const forward = model.rotation * Vecf3{1.f, 0.f, 0.f};
        return std::atan2(forward.y, forward.x);
    }

    /* The turn from its facing to `at`, -pi to pi */
    static f32 off_by(Model const& model, Vecf3 const& at)
    {
        Vecf3 const to = at - model.position;
        if(glm::dot(Vecf2(to), Vecf2(to)) < 1e-6f)
            return 0.f;
        return glm::mod(std::atan2(to.y, to.x) - yaw_of(model) + glm::pi<f32>(), glm::two_pi<f32>()) - glm::pi<f32>();
    }

    /* Its body comes round toward `at` at its turning speed. Standing, it
     * holds until its facing strays past the threshold, then turns all
     * the way: a hunter is slow to meet a flank */
    void turn(Actor& actor, Model& model, Vecf3 const& at, f32 dt, bool moving)
    {
        turning_t const t   = turning_of(model);
        f32 const       off = off_by(model, at);
        if(moving || std::abs(off) > t.threshold)
            actor.turning = true;
        if(!actor.turning)
            return;
        f32 const step = t.speed * dt;
        model.rotation = glm::angleAxis(yaw_of(model) + std::clamp(off, -step, step), Vecf3{0.f, 0.f, 1.f});
        actor.turning  = moving || std::abs(off) > step;
    }

    /* Whether its aim reaches `at` without the body turning further */
    bool facing(Model const& model, Vecf3 const& at)
    {
        turning_t const t = turning_of(model);
        return std::abs(off_by(model, at)) <= std::max(t.threshold, .35f) + .1f;
    }

    /* Stride directions, as the bits of stride_t::clips */
    static constexpr blam::antr::unit_weapon::animation_idx_t moves[4] = {
        blam::antr::unit_weapon::animation_idx_t::move_front,
        blam::antr::unit_weapon::animation_idx_t::move_back,
        blam::antr::unit_weapon::animation_idx_t::move_left,
        blam::antr::unit_weapon::animation_idx_t::move_right};

    struct stride_t
    {
        f32 speed{0.f};
        u8  clips{0}; /*!< Bits in moves order: which strides it has */
    };

    /* Its run and the strides it has, from its move clips holding what it
     * holds */
    stride_t const& strides(Proxy& p, u64 id, AnimationPlayback const& anim)
    {
        static stride_t const none{.speed = default_speed, .clips = 0xf};
        if(!anim.graph)
            return none;
        auto const& files = p.template subsystem<BlamFiles<halo_version>>();
        auto const* equip = p.template get<BipedEquipment>(id);
        auto const* gun   = equip ? equip->held(files.load_generation) : nullptr;
        std::string const weapon = gun ? std::string(gun->weapon->label.str()) : std::string("unarmed");
        auto [it, fresh] = m_strides.try_emplace(std::pair{anim.graph, weapon});
        if(fresh)
        {
            f32 front = move_speed(*anim.graph, moves[0], *s_magic, "stand", weapon);
            it->second.speed = front > 0.f ? front : default_speed;
            for(size_t i = 0; i < 4; i++)
                if(move_clip(*anim.graph, moves[i], *s_magic, "stand", weapon) >= 0)
                    it->second.clips |= static_cast<u8>(1u << i);
        }
        return it->second;
    }

    f32 stride(Proxy& p, u64 id, AnimationPlayback const& anim, kind_t const&)
    {
        return strides(p, id, anim).speed;
    }

    /* Whether it can stride a way while facing as it does: the clip for
     * that direction is in its graph. A hunter only walks forward */
    bool can_stride(Proxy& p, u64 id, AnimationPlayback const& anim, Model const& model, Vecf3 const& dir)
    {
        u8 const clips = strides(p, id, anim).clips;
        if(clips == 0xf)
            return true;
        Vecf3 const local = glm::conjugate(model.rotation) * dir;
        size_t const way  = std::abs(local.x) >= std::abs(local.y) ? (local.x >= 0.f ? 0 : 1)
                                                                   : (local.y >= 0.f ? 2 : 3);
        return (clips & (1u << way)) != 0;
    }

    /* At its position it ducks and rises by its variant's times */
    void crouch_cycle(Actor& actor, AnimationPlayback& anim, actv_t const& actv, f32)
    {
        if(actor.stance_in > 0.f)
            return;
        bool const crouched = anim.stance == "crouch";
        if(crouched)
        {
            anim.stance     = "stand";
            actor.stance_in = uniform(actv.run_time, 3.f);
        } else if(actv.crouch_time.y > 0.f && m_random() % 100 < static_cast<u32>(actv.initial_crouch_chance * 100.f))
        {
            anim.stance     = "crouch";
            actor.stance_in = uniform(actv.crouch_time, 2.f);
        } else
            actor.stance_in = uniform(actv.run_time, 3.f);
    }

    /* ---- Shooting ---- */

    void fire(Proxy& p, u64 id, Actor& actor, Model const& model, kind_t const& kind, f32)
    {
        actv_t const& actv = *kind.variant;
        auto const& files = p.template subsystem<BlamFiles<halo_version>>();
        auto*       equip = p.template get<BipedEquipment>(id);
        auto*       gun   = equip ? equip->held(files.load_generation) : nullptr;
        if(!gun || !gun->tag)
            return;
        firing_t const firing = firing_of(gun->tag);
        /* A charging trigger shows how far along it is */
        gun->charge = firing.single && firing.charging > 0.f && actor.burst_left == 1 && actor.fire_in > 0.f
                          ? 1.f - actor.fire_in / firing.charging
                          : 0.f;
        if(actor.fire_in > 0.f)
            return;
        f32 const reach = actv.maximum_firing_distance > 0.f ? actv.maximum_firing_distance : 20.f;
        if(glm::distance(model.position, actor.last_seen) > reach)
            return;
        /* None given: as fast as its weapon fires */
        f32 const rate = (actv.rate_of_fire > 0.f ? actv.rate_of_fire : firing.rate) *
                         scaled(&blam::globals::difficulty::rate_of_fire);
        bool const starting = actor.burst_left <= 0;
        if(!begin_burst(actor, actv, firing, rate))
            return;
        /* As a player's: from the gun's muzzle in its hand, at what it looks at */
        Vecf3 const eye    = eye_of(p, id);
        Vecf3 const target = actor.last_seen + chest_of(p, actor.target);
        Vecf3 const aim    = burst_aim(actor, actv, target, starting);
        Vecf3 const muzzle = trigger_of(p, *gun).value_or(
            eye + glm::normalize(aim - eye) * WeaponFireEvent::muzzle_distance);
        /* A friend in the line of fire: the burst waits, and it looks for
         * another spot */
        if(starting && friend_before(p, id, actor, muzzle, aim))
        {
            actor.burst_left    = 0;
            actor.fire_in       = look_period;
            actor.reposition_in = 0.f;
            return;
        }
        Vecf3       dir    = aim_direction(p, actor, muzzle, aim, arc_of(p, gun->tag, false), leading(actv));
        /* Its aim wanders by the variant's error, scaled for the difficulty */
        if(f32 const error = actv.projectile_error * scaled(&blam::globals::difficulty::projectile_error); error > 0.f)
        {
            std::uniform_real_distribution<f32> unit(-1.f, 1.f);
            Vecf3 const side = glm::normalize(glm::cross(dir, Vecf3{0, 0, 1}));
            Vecf3 const up   = glm::cross(side, dir);
            dir = glm::normalize(dir + side * (unit(m_random) * error) + up * (unit(m_random) * error));
        }
        GameEvent       ev{.type = GameEvent::WeaponFire};
        WeaponFireEvent shot{
            .weapon     = gun->tag->as_ref(),
            .shooter    = id,
            .player_idx = ObjectHitEvent::no_player,
            .seed       = static_cast<u32>(m_random()),
            .position   = muzzle,
            .direction  = dir,
            .target     = actor.target,
        };
        p.template subsystem<GameEventBus>().inject(ev, &shot);
        if(--actor.burst_left > 0)
            actor.fire_in = 1.f / rate;
        else
            actor.fire_in = uniform(actv.burst_separation, 1.f) * scaled(&blam::globals::difficulty::burst_separation);
    }

    /* How far ahead of a moving target it aims: its variant's leading, as
     * much as the difficulty allows (none on Normal) */
    f32 leading(actv_t const& actv) const
    {
        return actv.target_leading * scaled(&blam::globals::difficulty::target_leading);
    }

    /* Where a burst goes. It opens off the target by the variant's burst
     * origin radius and walks back onto it by the return length a shot,
     * following the target only as well as its tracking, scaled by the
     * difficulty, lets it: on Normal a burst stays on where the target was
     * when it began */
    Vecf3 burst_aim(Actor& actor, actv_t const& actv, Vecf3 const& target, bool starting)
    {
        f32 const tracking =
            glm::clamp(actv.target_tracking * scaled(&blam::globals::difficulty::target_tracking), 0.f, 1.f);
        if(starting)
        {
            actor.burst_from = target;
            std::uniform_real_distribution<f32> unit(-1.f, 1.f);
            Vecf3 const off{unit(m_random), unit(m_random), unit(m_random) * .5f};
            f32 const   radius = actv.burst_origin_radius * scaled(&blam::globals::difficulty::burst_error);
            actor.burst_aim    = target + (glm::dot(off, off) > 1e-6f ? glm::normalize(off) * radius : Vecf3{});
            return actor.burst_aim;
        }
        actor.burst_from = glm::mix(actor.burst_from, target, tracking);
        Vecf3 const to   = actor.burst_from - actor.burst_aim;
        f32 const   left = glm::length(to);
        f32 const   step = uniform(actv.burst_return_length);
        if(left > 1e-4f)
            actor.burst_aim += to / left * std::min(step, left);
        return actor.burst_aim;
    }

    /* How a weapon's first trigger's projectile flies, when gravity bends
     * it: its speed, what pulls it down, and whether the AI lobs it */
    struct arc_t
    {
        f32  speed;   /* wu/s */
        f32  gravity; /* wu/s^2 */
        bool lob;     /* the high arc, over cover */
    };

    std::optional<arc_t> arc_of(Proxy& p, blam::tag_t const* tag, bool mounted) const
    {
        using flags_t = blam::scn::projectile::flags_t;
        if(!tag)
            return std::nullopt;
        auto weapon = tag->template data<blam::scn::weapon>(*s_magic);
        if(!weapon.has_value())
            return std::nullopt;
        auto triggers = weapon.value()[0].triggers.data(*s_magic);
        if(!triggers.has_value() || triggers.value().empty() || !triggers.value()[0].projectile.valid())
            return std::nullopt;
        blam::tag_index_view<halo_version> index(p.template subsystem<BlamFiles<halo_version>>().container);
        auto proj_tag = index.find(triggers.value()[0].projectile.tag_id);
        if(proj_tag == index.end())
            return std::nullopt;
        auto proj = (*proj_tag).template data<blam::scn::projectile>(*s_magic);
        if(!proj.has_value() || proj.value()[0].air_gravity_scale <= 0.f || proj.value()[0].initial_velocity <= 0.f)
            return std::nullopt;
        auto const& tag_ = proj.value()[0];
        return arc_t{
            .speed   = tag_.initial_velocity * tick_rate,
            .gravity = halo_gravity * tag_.air_gravity_scale,
            .lob     = mounted && (tag_.projectile_flags & flags_t::ai_must_use_ballistic_aiming) != flags_t::none,
        };
    }

    /* Where a target is heading, to lead it */
    Vecf3 velocity_of(Proxy& p, u64 id)
    {
        if(auto const* data = p.template get<PhysicsData>(id); data && data->enabled)
            return data->velocity;
        if(auto const* physics = p.template get<ObjectPhysics>(id))
            return physics->linear_velocity;
        return {};
    }

    /* The way to shoot from `muzzle` to land on `aim`: straight at it, or
     * along the arc that drops there, led by where the target will be by
     * then; out of reach, as far as it will go */
    Vecf3 aim_direction(
        Proxy& p, Actor const& actor, Vecf3 const& muzzle, Vecf3 const& aim, std::optional<arc_t> arc, f32 leading)
    {
        if(!arc || leading <= 0.f)
            return glm::normalize(aim - muzzle);
        Vecf3 const lead = velocity_of(p, actor.target) * leading;
        Vecf3       at   = aim;
        Vecf3       dir  = glm::normalize(aim - muzzle);
        for(u32 pass = 0; pass < 3; pass++)
        {
            Vecf3 const r = at - muzzle;
            f32 const   d = glm::length(Vecf2(r));
            f32 const   v = arc->speed, g = arc->gravity;
            if(d < 1e-3f)
                return glm::normalize(r);
            Vecf3 const flat = Vecf3{r.x, r.y, 0.f} / d;
            f32 const   disc = v * v * v * v - g * (g * d * d + 2.f * r.z * v * v);
            f32         angle, time;
            if(disc < 0.f)
            {
                angle = glm::quarter_pi<f32>();
                time  = d / (v * std::cos(angle));
            } else
            {
                /* Lobbed over cover where the arc stays reasonable, flat
                 * when the target is too close for that */
                f32 const root = std::sqrt(disc);
                f32 const high = std::atan((v * v + root) / (g * d));
                angle = arc->lob && high <= lob_limit ? high : std::atan((v * v - root) / (g * d));
                time  = d / (v * std::cos(angle));
            }
            dir = flat * std::cos(angle) + Vecf3{0.f, 0.f, std::sin(angle)};
            at  = aim + lead * time;
        }
        return dir;
    }

    /* How a weapon's first trigger fires: its top rate, and whether a pull
     * charges up to one shot (hunter cannon) rather than repeating */
    struct firing_t
    {
        f32  rate{3.f};
        f32  charging{0.f};
        bool single{false};
    };

    firing_t firing_of(blam::tag_t const* tag) const
    {
        using flags_t = blam::scn::weapon::trigger_t::flags_t;
        firing_t out;
        if(tag)
            if(auto weapon = tag->template data<blam::scn::weapon>(*s_magic); weapon.has_value())
                if(auto triggers = weapon.value()[0].triggers.data(*s_magic);
                   triggers.has_value() && !triggers.value().empty())
                {
                    auto const& trigger = triggers.value()[0];
                    if(trigger.maximum_rate_of_fire.y > 0.f)
                        out.rate = trigger.maximum_rate_of_fire.y;
                    out.charging = trigger.charging_time;
                    out.single   = (trigger.flags & flags_t::does_not_repeat_automatically) != flags_t::none;
                }
        return out;
    }

    /* Starts a burst: so many shots at the rate, or one shot a pull after
     * the trigger has charged. True once a shot may go */
    bool begin_burst(Actor& actor, actv_t const& actv, firing_t const& firing, f32 rate)
    {
        if(actor.burst_left > 0)
            return true;
        if(!firing.single)
        {
            actor.burst_left = std::max(1, static_cast<i32>(uniform(actv.burst_duration, 1.f) * rate));
            return true;
        }
        actor.burst_left = 1;
        actor.fire_in    = firing.charging;
        return firing.charging <= 0.f;
    }

    /* A weapon it can shoot with: not a sword, nor anything to one its
     * variant says only strikes */
    bool holds_gun(Proxy& p, u64 id)
    {
        auto const* equip = p.template get<BipedEquipment>(id);
        auto const* gun   = equip ? equip->held(p.template subsystem<BlamFiles<halo_version>>().load_generation) : nullptr;
        if(!gun || !gun->tag || !gun->weapon || gun->weapon->triggers.count == 0)
            return false;
        auto const* actor = p.template get<Actor>(id);
        auto const* kind  = actor ? kind_of(p, *actor) : nullptr;
        return !kind || !kind->variant ||
               (kind->variant->flags & actv_t::flags_t::cannot_use_ranged_weapons) == actv_t::flags_t::none;
    }

    /* ---- Riding ---- */

    /* From its seat it looks all round, and fights with the vehicle's gun
     * where it is the gunner, its own where the seat lets it */
    void ride(Proxy& p, u64 id, Actor& actor, Model const& model, kind_t const& kind, ObjectPhysics const& physics, f32 dt)
    {
        using flags_t = blam::scn::unit::seat_t::seat_flags_t;
        actor.look_in -= dt;
        actor.fire_in -= dt;
        actor.seen_ago += dt;
        if(actor.look_in <= 0.f)
        {
            actor.look_in = look_period;
            if(u64 const seen = actor.blind ? 0 : look(p, id, actor, model, *kind.actor, true))
            {
                if(seen != actor.target)
                {
                    actor.target     = seen;
                    actor.fire_in    = uniform(kind.variant->first_burst_delay_time);
                    actor.burst_left = 0;
                }
                if(auto const* target = p.template get<Model>(seen))
                    actor.last_seen = target->position;
                actor.seen_ago = 0.f;
            }
        }
        if(!actor.target || actor.seen_ago > look_period * 2.f || !alive(p, actor.target))
            return;
        auto const* vehicle = p.template get<Model>(physics.seated_in);
        if(!vehicle || !vehicle->origin_object)
            return;
        auto unit = vehicle->origin_object->template data<blam::scn::unit>(*s_magic);
        if(!unit.has_value())
            return;
        auto seats = unit.value()[0].seats.data(*s_magic);
        if(!seats.has_value() || physics.seat < 0 || static_cast<size_t>(physics.seat) >= seats.value().size())
            return;
        auto const flags = seats.value()[static_cast<size_t>(physics.seat)].flags;
        if((flags & flags_t::gunner) != flags_t::none)
            fire_mounted(p, id, actor, physics.seated_in, *vehicle, unit.value()[0], kind);
        else if((flags & flags_t::allows_weapons) != flags_t::none)
            fire(p, id, actor, model, kind, dt);
    }

    /* The vehicle's gun, from its trigger marker as the vehicle is posed */
    void fire_mounted(
        Proxy& p, u64 id, Actor& actor, u64 vehicle, Model const& body, blam::scn::unit const& unit, kind_t const& kind)
    {
        actv_t const& actv = *kind.variant;
        if(actor.fire_in > 0.f)
            return;
        f32 const reach = actv.maximum_firing_distance > 0.f ? actv.maximum_firing_distance : 20.f;
        if(glm::distance(body.position, actor.last_seen) > reach)
            return;
        auto weapons = unit.weapons.data(*s_magic);
        if(!weapons.has_value() || weapons.value().empty() || !body.tag)
            return;
        auto const& files = p.template subsystem<BlamFiles<halo_version>>();
        blam::tag_index_view<halo_version> index(files.container);
        auto weapon = index.find(weapons.value()[0].weapon.tag_id);
        auto marker = bind_marker_node(*body.tag, trigger_marker, *s_magic);
        if(weapon == index.end() || !marker)
            return;
        Matf4       at   = body.transform;
        auto const* anim = p.template get<AnimationPlayback>(vehicle);
        if(anim && marker->node >= 0 && static_cast<size_t>(marker->node) < anim->pose.size())
            at = at * anim->pose[static_cast<size_t>(marker->node)];
        Vecf3 const muzzle = Vecf3((at * marker->transform)[3]);
        Vecf3 const aim    = actor.last_seen + chest_of(p, actor.target);
        Vecf3       dir    = aim_direction(p, actor, muzzle, aim, arc_of(p, &(*weapon), true), leading(actv));
        if(f32 const error = actv.projectile_error * scaled(&blam::globals::difficulty::projectile_error); error > 0.f)
        {
            std::uniform_real_distribution<f32> unit_spread(-1.f, 1.f);
            Vecf3 const side = glm::normalize(glm::cross(dir, Vecf3{0, 0, 1}));
            Vecf3 const up   = glm::cross(side, dir);
            dir = glm::normalize(dir + side * (unit_spread(m_random) * error) + up * (unit_spread(m_random) * error));
        }
        /* The gun's own rate, not what the gunner carries */
        firing_t const firing = firing_of(&(*weapon));
        f32 const      rate   = firing.rate * scaled(&blam::globals::difficulty::rate_of_fire);
        if(!begin_burst(actor, actv, firing, rate))
            return;
        GameEvent       ev{.type = GameEvent::WeaponFire};
        WeaponFireEvent shot{
            .weapon     = (*weapon).as_ref(),
            .shooter    = id,
            .vehicle    = vehicle,
            .player_idx = ObjectHitEvent::no_player,
            .seed       = static_cast<u32>(m_random()),
            .position   = muzzle,
            .direction  = dir,
            .target     = actor.target,
        };
        p.template subsystem<GameEventBus>().inject(ev, &shot);
        if(--actor.burst_left > 0)
            actor.fire_in = 1.f / rate;
        else
            actor.fire_in = uniform(actv.burst_separation, 1.f) * scaled(&blam::globals::difficulty::burst_separation);
    }

    /* A kill it saw is crowed over, one voice at a time */
    void taunt(Proxy& p, u64 id, Actor& actor, Model const& model, f32 dt)
    {
        m_taunt_in -= dt;
        if(actor.taunt_in <= 0.f || (actor.taunt_in -= dt) > 0.f)
            return;
        actor.taunt_in = 0.f;
        if(m_taunt_in > 0.f)
            return;
        auto const* dialogue = dialogue_of(p, id);
        auto*       sounds   = p.template service<comp_app::EventBus<SoundEvent>>();
        if(!dialogue || !sounds)
            return;
        auto const& line = actor.taunt_player && dialogue->killed_enemy_player.valid()
                               ? dialogue->killed_enemy_player
                               : dialogue->killed_enemy;
        if(!line.valid())
            return;
        m_taunt_in = taunt_spacing;
        SoundEvent     ev{.type = SoundEvent::play_sound};
        PlaySoundEvent play{
            .sound    = &static_cast<blam::tagref_t const&>(line),
            .position = model.position + Vecf3{0.f, 0.f, .5f},
        };
        sounds->inject(ev, &play);
    }

    /* The unit's first dialogue variant */
    blam::scn::unit_dialogue const* dialogue_of(Proxy& p, u64 id)
    {
        auto const* model = p.template get<Model>(id);
        if(!model || !model->origin_object)
            return nullptr;
        auto unit = model->origin_object->template data<blam::scn::unit>(*s_magic);
        if(!unit.has_value())
            return nullptr;
        auto variants = unit.value()[0].dialogue_variants.data(*s_magic);
        if(!variants.has_value() || variants.value().empty() || !variants.value()[0].dialogue.valid())
            return nullptr;
        auto const& files = p.template subsystem<BlamFiles<halo_version>>();
        blam::tag_index_view<halo_version> index(files.container);
        auto it = index.find(variants.value()[0].dialogue.tag_id);
        if(it == index.end())
            return nullptr;
        auto data = (*it).template data<blam::scn::unit_dialogue>(*s_magic);
        return data.has_value() ? data.value() : nullptr;
    }

    /* Within arm's reach of its target it strikes with what it holds */
    bool strike(Proxy& p, u64 id, Actor& actor, Model const& model, kind_t const& kind)
    {
        actv_t const& actv = *kind.variant;
        actor.melee_in -= look_period / 8.f;
        if(actv.melee_range <= 0.f || glm::distance(model.position, actor.last_seen) > strike_reach * 1.3f)
        {
            /* Out of reach: a swing under way is called off */
            actor.swinging  = false;
            actor.strike_in = 0.f;
            return false;
        }
        if(!actor.swinging && glm::distance(model.position, actor.last_seen) > strike_reach)
            return false;
        /* A carrier form at its target falls over and bursts */
        if(kind.actor->type == blam::scn::actor_type_t::carrier_form)
        {
            GameEvent         ev{.type = GameEvent::UnitVitality};
            UnitVitalityEvent death{.unit = id, .body = 0.f, .shield = 0.f, .dead = true};
            p.template subsystem<GameEventBus>().inject(ev, &death);
            return true;
        }
        /* Winding up: the blow lands at the swing's key frame, if the target
         * is still within reach */
        if(actor.strike_in > 0.f)
        {
            if((actor.strike_in -= look_period / 8.f) > 0.f)
                return true;
            actor.strike_in = 0.f;
        } else if(actor.melee_in > 0.f)
            return true;
        bool const windup = actor.strike_in == 0.f && actor.melee_in <= 0.f && !actor.swinging;
        auto const& files = p.template subsystem<BlamFiles<halo_version>>();
        auto const* equip = p.template get<BipedEquipment>(id);
        auto const* gun   = equip ? equip->held(files.load_generation) : nullptr;
        /* Unarmed, or with a gun that has no blow of its own (a hunter's
         * fuel rod), it strikes with its own melee damage */
        blam::tagref_t with = gun && gun->tag && gun->weapon && gun->weapon->player_melee_damage.valid()
                                  ? gun->tag->as_ref()
                                  : blam::tagref_t{};
        if(!with.valid())
            if(auto const* body = p.template get<Model>(id); body && body->origin_object)
                if(auto unit = body->origin_object->template data<blam::scn::unit>(*s_magic); unit.has_value())
                    with = unit.value()[0].melee_damage;
        if(!with.valid())
            return false;
        Vecf3 const eye = eye_of(p, id);
        GameEvent       ev{.type = GameEvent::WeaponFire};
        WeaponFireEvent shot{
            .weapon     = with,
            .shooter    = id,
            .player_idx = ObjectHitEvent::no_player,
            .melee      = true,
            .windup     = windup,
            .position   = eye,
            .direction  = glm::normalize(actor.last_seen + chest_of(p, actor.target) - eye),
            .target     = actor.target,
        };
        p.template subsystem<GameEventBus>().inject(ev, &shot);
        if(windup)
        {
            /* The swing first, the blow at its key frame */
            auto const* anim = p.template get<AnimationPlayback>(id);
            std::string const weapon =
                gun && gun->weapon ? std::string(gun->weapon->label.str()) : std::string("unarmed");
            f32 const key = anim && anim->graph ? melee_key_time(*anim->graph, *s_magic, weapon) : 0.f;
            actor.strike_in = key > 0.f ? key : default_windup;
            actor.swinging  = true;
            return true;
        }
        actor.swinging = false;
        actor.melee_in = 1.5f;
        return true;
    }

    /* Its variant's grenades, dealt the first time they are asked for */
    void deal_grenades(Actor& actor, actv_t const& actv)
    {
        if(actor.grenades >= 0)
            return;
        actor.grenades = static_cast<i16>(between(actv.grenade_count));
    }

    /* Within a min, max pair of counts */
    i32 between(i16 const (&range)[2])
    {
        return range[1] > range[0] ? range[0] + static_cast<i32>(m_random() % (range[1] - range[0] + 1))
                                   : std::max<i16>(range[0], 0);
    }

    /* What its weapon holds when dropped, by its variant: a share of the
     * magazine (or battery) and some reserve. Actors don't spend rounds,
     * so this is set once for each weapon taken in hand */
    void stock(Proxy& p, u64 id, Actor& actor, kind_t const& kind)
    {
        auto* equip = p.template get<BipedEquipment>(id);
        if(!equip || !kind.variant)
            return;
        auto& gun = equip->weapons[0];
        if(!gun.weapon || gun.held == actor.stocked)
            return;
        actor.stocked      = gun.held;
        actv_t const& actv = *kind.variant;
        if(actv.drop_weapon_loaded.y <= 0.f && actv.drop_weapon_ammo[1] <= 0)
            return;
        f32 const loaded = std::clamp(uniform(actv.drop_weapon_loaded, actv.drop_weapon_loaded.x), 0.f, 1.f);
        if(auto const* magazine = first_magazine(gun.weapon, *s_magic); magazine && magazine->rounds_loaded_maximum > 0)
        {
            gun.rounds_loaded = static_cast<u16>(std::lround(loaded * std::max<i16>(magazine->rounds_loaded_maximum, 0)));
            gun.rounds_total  = static_cast<u16>(between(actv.drop_weapon_ammo));
        } else
            gun.age = 1.f - loaded;
    }

    /* Dead: what grenades it had left fall out, unless its variant says
     * they don't this time, and so does its variant's equipment */
    void drop_belongings(Proxy& p, u64 id, Actor& actor, Model const& model, kind_t const& kind)
    {
        actv_t const& actv = *kind.variant;
        f32           turn = 0.f;
        auto          toss = [&](blam::tagref_t const& what) {
            turn += drop_spread;
            Vecf3 const out = glm::angleAxis(turn, Vecf3{0.f, 0.f, 1.f}) * (model.rotation * Vecf3{1.f, 0.f, 0.f});
            SpawnObjectEvent spawn{
                .object       = what,
                .position     = model.position + out * .2f + Vecf3{0.f, 0.f, drop_lift},
                .server_owned = true,
                .velocity     = out * drop_speed + Vecf3{0.f, 0.f, drop_speed},
            };
            GameEvent ev{.type = GameEvent::SpawnObject};
            p.template subsystem<GameEventBus>().inject(ev, &spawn);
        };
        deal_grenades(actor, actv);
        u32 dropped = 0;
        if(actor.grenades > 0 && !chance(actv.dont_drop_grenades_chance))
            if(auto const* grenade = grenade_of(p, actv.grenade_type); grenade && grenade->equipment.valid())
                for(; dropped < static_cast<u32>(actor.grenades); dropped++)
                    toss(static_cast<blam::tagref_t const&>(grenade->equipment));
        if(actv.equipment.valid())
            toss(static_cast<blam::tagref_t const&>(actv.equipment));
        actor.grenades = 0;
    }

    /* Now and then, in range, a grenade lobbed at the target */
    void throw_grenade(Proxy& p, u64 id, Actor& actor, Model const& model, kind_t const& kind)
    {
        actv_t const& actv = *kind.variant;
        deal_grenades(actor, actv);
        if(actor.grenades == 0 || actv.grenade_chance <= 0.f)
            return;
        actor.grenade_in -= look_period / 8.f;
        if(actor.grenade_in > 0.f)
            return;
        actor.grenade_in = actv.grenade_check_time > 0.f ? actv.grenade_check_time : 2.f;
        f32 const d = glm::distance(model.position, actor.last_seen);
        if(d < actv.grenade_ranges.x || d > actv.grenade_ranges.y || !chance(actv.grenade_chance))
            return;
        blam::tagref_t const* projectile = grenade_projectile(p, actv.grenade_type);
        if(!projectile)
            return;
        actor.grenades--;
        Vecf3 const eye  = eye_of(p, id);
        Vecf3       flat = actor.last_seen - eye;
        f32 const   len  = glm::length(flat);
        if(len < 1e-3f)
            return;
        /* Lofted a little, more the farther it goes */
        Vecf3 const dir = glm::normalize(flat / len + Vecf3{0, 0, std::clamp(len / 20.f, .15f, .5f)});
        GameEvent       ev{.type = GameEvent::WeaponFire};
        WeaponFireEvent shot{
            .weapon     = *projectile,
            .shooter    = id,
            .player_idx = ObjectHitEvent::no_player,
            .seed       = static_cast<u32>(m_random()),
            .thrown     = true,
            .position   = eye + dir * WeaponFireEvent::muzzle_distance,
            .direction  = dir,
            .target     = actor.target,
        };
        p.template subsystem<GameEventBus>().inject(ev, &shot);
    }

    /* The globals' grenade of a type, by its projectile */
    blam::tagref_t const* grenade_projectile(Proxy& p, blam::scn::grenade_type_t type)
    {
        auto const* grenade = grenade_of(p, type);
        return grenade && grenade->projectile.valid() ? &static_cast<blam::tagref_t const&>(grenade->projectile)
                                                      : nullptr;
    }

    /* The globals' grenade of a type */
    blam::globals::grenade const* grenade_of(Proxy& p, blam::scn::grenade_type_t type)
    {
        if(m_grenades.empty())
        {
            auto const& files = p.template subsystem<BlamFiles<halo_version>>();
            blam::tag_index_view<halo_version> index(files.container);
            auto globals_tag = index.tag_of("globals\\globals");
            if(!globals_tag.has_value())
                return nullptr;
            auto globals = (*globals_tag)->template data<blam::globals::globals>(*s_magic);
            if(!globals.has_value())
                return nullptr;
            if(auto grenades = globals.value()->grenades.data(*s_magic); grenades.has_value())
                for(auto const& grenade : grenades.value())
                    m_grenades.push_back(&grenade);
        }
        size_t const i = static_cast<size_t>(type);
        return i < m_grenades.size() ? m_grenades[i] : nullptr;
    }

    /* ---- Tags ---- */

    kind_t const* kind_of(Proxy& p, Actor const& actor)
    {
        auto const key = std::tuple{actor.encounter, actor.squad, actor.variant};
        if(auto it = m_kinds.find(key); it != m_kinds.end())
            return &it->second;
        kind_t kind;
        auto encounters = m_scenario->ai.encounters.data(*s_magic);
        auto palette    = m_scenario->ai.actor_palette.data(*s_magic);
        if(!encounters.has_value() || !palette.has_value() || actor.encounter < 0 ||
           static_cast<size_t>(actor.encounter) >= encounters.value().size())
            return nullptr;
        kind.encounter = &encounters.value()[actor.encounter];
        auto squads    = kind.encounter->squads.data(*s_magic);
        if(!squads.has_value() || actor.squad < 0 || static_cast<size_t>(actor.squad) >= squads.value().size())
            return nullptr;
        kind.squad     = &squads.value()[actor.squad];
        /* Its own type, from where it was placed, outlasts a migration */
        i16 const type = actor.variant >= 0 ? actor.variant : kind.squad->actor_type;
        auto const& files = p.template subsystem<BlamFiles<halo_version>>();
        blam::tag_index_view<halo_version> index(files.container);
        if(type >= 0 && static_cast<size_t>(type) < palette.value().size())
        {
            auto const& ref = static_cast<blam::tagref_t const&>(palette.value()[type]);
            if(auto tag = index.find(ref.tag_id); tag != index.end())
                if(auto variant = (*tag).template data<actv_t>(*s_magic); variant.has_value())
                {
                    kind.variant = variant.value();
                    if(auto actor_tag = index.find(kind.variant->actor_definition.tag_id);
                       actor_tag != index.end())
                        if(auto actr = (*actor_tag).template data<actr_t>(*s_magic); actr.has_value())
                        {
                            kind.actor = actr.value();
                        }
                }
        }
        return &m_kinds.emplace(key, kind).first->second;
    }

    f32 uniform(Vecf2 const& range, f32 fallback = 0.f)
    {
        if(range.y <= 0.f)
            return fallback;
        if(range.y <= range.x)
            return range.x;
        return std::uniform_real_distribution<f32>(range.x, range.y)(m_random);
    }
    static f32 uniform_max(Vecf2 const& range)
    {
        return range.y > 0.f ? range.y : 4.f;
    }

  public:
    struct heard_t
    {
        u64   shooter{0};
        Vecf3 position{};
        Vecf3 direction{};
    };
    std::unique_ptr<TickQueue>                                       m_ticks;
    std::shared_ptr<GameEventBus::queue_type<WeaponFireEvent>>       m_fire_queue;
    std::vector<heard_t>                                             m_heard;
    blam::scn::scenario<halo_version> const*                         m_scenario{nullptr};
    static inline blam::map_ptr const*                               s_magic{nullptr};
    u32                                                              m_generation{0};
    std::map<std::tuple<i16, i16, i16>, kind_t>                      m_kinds;
    std::map<i16, std::vector<std::optional<std::vector<i32>>>>      m_links; /* encounter: per position */
    std::vector<obstacle_t>                                          m_obstacles;
    std::map<blam::tag_t const*, std::vector<std::pair<Vecf3, f32>>> m_spheres; /* object tag: its spheres */
    f32                                                              m_obstacles_in{0.f};
    std::map<std::pair<i16, i16>, platoon_t>                         m_platoons;
    f32                                                              m_platoon_in{0.f};
    std::map<std::pair<blam::antr::header const*, std::string>, stride_t> m_strides;
    std::map<blam::tag_t const*, turning_t>                               m_turning;
    std::minstd_rand                                                 m_random{12345};
    std::vector<blam::globals::grenade const*>                       m_grenades;
    blam::globals::difficulty const*                                 m_difficulty{nullptr};
    u16                                                              m_level{1};
    f32                                                              m_taunt_in{0.f};
    NavMesh                                                          m_nav;
    BSPItem const*                                                   m_nav_item{nullptr};
    AllegianceState*                                                 m_allegiance{nullptr};
};

void alloc_actors(compo::EntityContainer& e)
{
    auto& actors     = e.register_subsystem_inplace<ActorSystem>();
    auto& allegiance = e.register_subsystem_inplace<AllegianceState>();
    e.subsystem_cast<SaveState>()
        .bind<Actor>(e)
        .entity(&Actor::target)
        .entity(&Actor::board)
        .entity(&Actor::stocked)
        .create();
    auto& gbus     = e.subsystem_cast<GameEventBus>();
    actors.m_ticks = std::make_unique<TickQueue>(gbus);
    gbus.addEventFunction<MapLoadEvent>(0, [&actors, &allegiance](auto&, auto*) {
        actors.m_scenario = nullptr;
        allegiance.reset();
        actors.m_heard.clear();
        actors.m_platoons.clear();
        actors.m_grenades.clear();
    });
    gbus.addEventFunction<MapChangedEvent<halo_version>>(
        0, [&actors](auto&, MapChangedEvent<halo_version>* changed) {
            actors.m_scenario    = changed->scenario;
            ActorSystem::s_magic = &changed->container.magic;
        });
    actors.m_fire_queue = gbus.addQueuedEventFunction<WeaponFireEvent>(
        0, [&actors](GameEvent&, WeaponFireEvent* shot) {
            if(!shot->melee)
                actors.m_heard.push_back(
                    {.shooter = shot->shooter, .position = shot->position, .direction = shot->direction});
        });
}
