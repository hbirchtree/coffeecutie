#include "actors.h"
#include "blam_files.h"
#include "components.h"
#include "game_events.h"
#include "network/networking.h"
#include "physics.h"
#include "selected_version.h"
#include "sounds.h"
#include "ticks.h"

#include "animation.h"

#include <blam/volta/blam_actor.h>
#include <blam/volta/blam_globals.h>
#include <blam/volta/blam_scenario.h>
#include <coffee/components/proxy.h>
#include <coffee/components/restricted_subsystem.h>

#include <map>
#include <random>

using ActorManifest = compo::SubsystemManifest<
    type_list_t<
        Actor,
        Model,
        AnimationPlayback,
        const BipedStatus,
        const BipedEquipment,
        const ObjectPhysics,
        const PlayerInfo,
        const PlayerCamera>,
    type_list_t<
        GameEventBus,
        PhysicsBus,
        const NetworkState,
        const LoadingStatus,
        const BlamFiles<halo_version>>,
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
        if(std::getenv("BLAM_DEBUG_AI") && (m_debug_top -= .033f) <= 0.f)
        {
            m_debug_top = 3.f;
            cDebug(
                "ai: tick dt {} scenario {} loaded {} replica {} actors {}",
                dt,
                m_scenario != nullptr,
                p.template subsystem<LoadingStatus>().loaded_map == LoadingStatus::loaded,
                p.template subsystem<NetworkState>().replica,
                [&] {
                    size_t n = 0;
                    for(auto ent : p.template select<Actor>())
                        n += ent.template get<Actor>().active() ? 1 : 0;
                    return n;
                }());
        }
        if(dt <= 0.f || !m_scenario ||
           p.template subsystem<LoadingStatus>().loaded_map != LoadingStatus::loaded ||
           p.template subsystem<NetworkState>().replica)
            return;
        auto const& files = p.template subsystem<BlamFiles<halo_version>>();
        if(m_generation != files.load_generation)
        {
            m_generation = files.load_generation;
            m_kinds.clear();
        }
        hear(p);
        if((m_platoon_in -= dt) <= 0.f)
        {
            m_platoon_in = 1.f;
            muster(p);
        }
        for(auto ent : p.template select<Actor, Model, AnimationPlayback, BipedStatus, ObjectPhysics>())
        {
            auto [actor, model, anim, status, physics] = ent.components();
            if(!actor.active() || actor.braindead || status.dead || !physics.upright || physics.seated)
            {
                /* What it last walked at, it would keep walking at */
                stop(p, ent.id(), actor);
                actor.board = 0;
                continue;
            }
            auto const* kind = kind_of(p, actor);
            if(std::getenv("BLAM_DEBUG_AI") && (m_debug_in -= dt) <= 0.f)
            {
                m_debug_in = 3.f;
                cDebug(
                    "ai: {} state {} scripted {} may_fight {} target {} kind {}/{}/{} team {}",
                    ent.id(),
                    static_cast<int>(actor.state),
                    actor.scripted,
                    actor.may_fight,
                    actor.target,
                    kind != nullptr,
                    kind && kind->variant,
                    kind && kind->actor,
                    actor.team);
            }
            if(!kind || !kind->variant || !kind->actor)
                continue;
            /* Sent to a vehicle: it runs there, whatever else it was doing,
             * and waits to be seated */
            if(actor.board)
            {
                auto const* vehicle = p.template get<Model>(actor.board);
                if(std::getenv("BLAM_DEBUG_AI") && vehicle && (actor.look_in -= dt) <= 0.f)
                {
                    actor.look_in = 2.f;
                    cDebug(
                        "ai: {} boarding {} distance {} at {} {} {} moving {}",
                        ent.id(),
                        actor.board,
                        glm::distance(Vecf2(model.position), Vecf2(vehicle->position)),
                        model.position.x,
                        model.position.y,
                        model.position.z,
                        actor.moving);
                }
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
            think(p, ent.id(), actor, model, anim, status, *kind, dt);
        }
    }

  private:
    static constexpr f32 look_period     = .25f; /* s between looks around */
    static constexpr f32 arrive_radius   = .35f; /* wu */
    static constexpr f32 reposition_time = 4.f;  /* s between picking positions */
    static constexpr f32 default_speed   = 2.f;  /* wu/s, a graph without a stride */
    static constexpr f32 eye_height      = .6f;  /* wu above the feet */

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
        actor.seen_ago += dt;
        actor.look_in -= dt;
        actor.fire_in -= dt;
        actor.stance_in -= dt;
        actor.state_in -= dt;
        actor.reposition_in -= dt;

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
                    actor.fire_in = uniform(actv.first_burst_delay_time);
                    actor.burst_left = 0;
                    alert_squad(p, id, actor, actr);
                }
                if(auto const* target = p.template get<Model>(seen))
                    actor.last_seen = target->position;
                actor.seen_ago = 0.f;
                if(actor.state == state_t::idle || actor.state == state_t::pursuit)
                    enter(actor, state_t::combat);
            }
        }
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
            move_toward(p, id, actor, model, anim, kind, actor.post, dt, false);
            break;
        case state_t::combat: {
            if(!alive(p, actor.target))
            {
                actor.target = 0;
                enter(actor, state_t::idle);
                break;
            }
            if(actor.seen_ago > uniform_max(actr.pursuit.target_search_time))
            {
                enter(actor, state_t::pursuit);
                actor.state_in = uniform_max(actr.pursuit.pursuit_position_time);
                break;
            }
            if(actor.reposition_in <= 0.f || actor.position < 0)
            {
                actor.reposition_in = reposition_time;
                actor.position      = pick_position(p, actor, model, kind, actor.last_seen, false);
                if(actor.position >= 0)
                    actor.goal = position_of(kind, actor.position);
            }
            bool const there = actor.position < 0 ||
                               glm::distance(model.position, actor.goal) <= arrive_radius;
            if(!there)
                move_toward(p, id, actor, model, anim, kind, actor.goal, dt, true);
            else
            {
                stop(p, id, actor);
                if(actor.covering)
                {
                    /* Down behind it until it dares to look again */
                    anim.stance = "crouch";
                    if(actor.reposition_in <= 0.f ||
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
                face(model, actor.last_seen);
                if(!strike(p, id, actor, model, kind))
                    fire(p, id, actor, model, kind, dt);
                throw_grenade(p, id, actor, model, kind);
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
                actor.position      = pick_position(p, actor, model, kind, actor.last_seen, true);
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

    void enter(Actor& actor, state_t state)
    {
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
        Vecf3 const eye = target->position + Vecf3{0, 0, eye_height};
        Vecf3 const to  = model.position + Vecf3{0, 0, eye_height * .8f} - eye;
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

    /* The nearest enemy in the actor's vision, with a clear line to it */
    u64 look(Proxy& p, u64 id, Actor const& actor, Model const& model, actr_t const& actr)
    {
        Vecf3 const eye     = model.position + Vecf3{0, 0, eye_height};
        Vecf3 const forward = model.rotation * Vecf3{1, 0, 0};
        f32 const   reach   = actr.perception.max_vision_dist > 0.f ? actr.perception.max_vision_dist : 30.f;
        f32 const   half    = actr.perception.max_vision > 0.f ? actr.perception.max_vision * .5f : glm::radians(90.f);
        u64   best     = 0;
        f32   best_d   = reach;
        auto  consider = [&](u64 other, Model const& target) {
            Vecf3 const to = target.position + Vecf3{0, 0, eye_height * .8f} - eye;
            f32 const   d  = glm::length(to);
            if(d >= best_d || d < 1e-3f)
                return;
            /* A known target is kept in the whole field; a new one must be
             * within the central cone */
            f32 const angle = std::acos(std::clamp(glm::dot(to / d, forward), -1.f, 1.f));
            if(angle > (other == actor.target ? half * 1.5f : half))
                return;
            if(!clear(p, id, eye, eye + to, other))
                return;
            best   = other;
            best_d = d;
        };
        for(auto player : p.template select<PlayerInfo, Model, BipedStatus>())
        {
            auto [info, target, status] = player.components();
            if(!info.mode.physics || status.dead || !enemies(actor.team, team_t::player))
                continue;
            consider(player.id(), target);
        }
        for(auto other : p.template select<Actor, Model, BipedStatus>())
        {
            auto [theirs, target, status] = other.components();
            if(other.id() == id || !theirs.active() || status.dead || !enemies(actor.team, static_cast<team_t>(theirs.team)))
                continue;
            consider(other.id(), target);
        }
        return best;
    }

    bool clear(Proxy& p, u64 self, Vecf3 const& from, Vecf3 const& to, u64 target)
    {
        Physics::Event   ev{Physics::Event::Raycast};
        Physics::Raycast ray{.from = from, .to = to, .ignore = self};
        p.template subsystem<PhysicsBus>().process(ev, &ray);
        return !ray.hit || ray.entity_id == target;
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
                   actor.state != state_t::idle)
                    continue;
                auto const* kind = kind_of(p, actor);
                f32 const reach  = kind && kind->actor && kind->actor->perception.hearing_dist > 0.f
                                       ? kind->actor->perception.hearing_dist
                                       : 15.f;
                if(glm::distance(model.position, shot.position) > reach)
                    continue;
                actor.last_seen = shot.position;
                actor.seen_ago  = 1e9f;
                if(shot.shooter != ent.id() && enemy_unit(p, actor, shot.shooter))
                {
                    actor.target = shot.shooter;
                    enter(actor, state_t::combat);
                } else
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

    /* Players and humans stand together; everyone else fights them and
     * each other */
    static bool enemies(u16 mine, team_t theirs)
    {
        auto const side = [](team_t t) {
            return t == team_t::player || t == team_t::human ? 1 : static_cast<int>(t) + 2;
        };
        return side(static_cast<team_t>(mine)) != side(theirs);
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
            }
            if(cost < best_cost)
            {
                best_cost = cost;
                best      = static_cast<i32>(i);
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
            if(&theirs != &actor && theirs.encounter == actor.encounter && theirs.position == index &&
               theirs.state != state_t::idle)
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
        Vecf3 to = goal - model.position;
        to.z     = 0.f;
        f32 const left = glm::length(to);
        if(left <= arrive_radius)
        {
            stop(p, id, actor);
            return;
        }
        if(anim.stance != "stand")
            anim.stance = "stand";
        f32 const speed = stride(p, id, anim, kind) * (running ? 1.f : .6f);
        Vecf3 const dir = to / left;
        if(actor.seen_ago > look_period * 2.f || actor.state != state_t::combat)
            face(model, model.position + dir);
        Physics::Event    ev{Physics::Event::Velocity};
        Physics::Velocity walking{
            .entity_id = id, .velocity = dir * std::min(speed, left / std::max(dt, 1e-3f)), .preserve_z = true};
        p.template subsystem<PhysicsBus>().process(ev, &walking);
        actor.moving = true;
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

    static void face(Model& model, Vecf3 const& at)
    {
        Vecf3 const to = at - model.position;
        if(glm::dot(Vecf2(to), Vecf2(to)) < 1e-6f)
            return;
        model.rotation = glm::angleAxis(std::atan2(to.y, to.x), Vecf3{0.f, 0.f, 1.f});
    }

    /* Its run, from its move clip holding what it holds */
    f32 stride(Proxy& p, u64 id, AnimationPlayback const& anim, kind_t const&)
    {
        using Slot = blam::antr::unit_weapon::animation_idx_t;
        if(!anim.graph)
            return default_speed;
        auto const& files = p.template subsystem<BlamFiles<halo_version>>();
        auto const* equip = p.template get<BipedEquipment>(id);
        auto const* gun   = equip ? equip->held(files.load_generation) : nullptr;
        std::string const weapon = gun ? std::string(gun->weapon->label.str()) : std::string("unarmed");
        auto [it, fresh] = m_strides.try_emplace(std::pair{anim.graph, weapon}, 0.f);
        if(fresh)
        {
            f32 front = move_speed(*anim.graph, Slot::move_front, *s_magic, "stand", weapon);
            it->second = front > 0.f ? front : default_speed;
        }
        return it->second;
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
        auto const* equip = p.template get<BipedEquipment>(id);
        auto const* gun   = equip ? equip->held(files.load_generation) : nullptr;
        if(!gun || !gun->tag || actor.fire_in > 0.f)
            return;
        f32 const reach = actv.maximum_firing_distance > 0.f ? actv.maximum_firing_distance : 20.f;
        if(glm::distance(model.position, actor.last_seen) > reach)
            return;
        f32 const rate = actv.rate_of_fire > 0.f ? actv.rate_of_fire : 3.f;
        if(actor.burst_left <= 0)
        {
            f32 const seconds = uniform(actv.burst_duration, 1.f);
            actor.burst_left  = std::max(1, static_cast<i32>(seconds * rate));
        }
        Vecf3 const offset = kind.actor->perception.stand_gun_offset;
        Vecf3 const muzzle = model.position + model.rotation * offset +
                             (glm::length(offset) < 1e-3f ? Vecf3{0, 0, eye_height} : Vecf3{});
        Vecf3 const aim = actor.last_seen + Vecf3{0, 0, eye_height * .7f};
        Vecf3       dir = glm::normalize(aim - muzzle);
        /* Its aim wanders by the variant's error */
        if(actv.projectile_error > 0.f)
        {
            std::uniform_real_distribution<f32> unit(-1.f, 1.f);
            Vecf3 const side = glm::normalize(glm::cross(dir, Vecf3{0, 0, 1}));
            Vecf3 const up   = glm::cross(side, dir);
            dir = glm::normalize(
                dir + side * (unit(m_random) * actv.projectile_error) + up * (unit(m_random) * actv.projectile_error));
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
            actor.fire_in = uniform(actv.burst_separation, 1.f);
    }

    /* Within arm's reach of its target it strikes with what it holds */
    bool strike(Proxy& p, u64 id, Actor& actor, Model const& model, kind_t const& kind)
    {
        actv_t const& actv = *kind.variant;
        actor.melee_in -= look_period / 8.f;
        if(actv.melee_range <= 0.f || glm::distance(model.position, actor.last_seen) > actv.melee_range)
            return false;
        if(actor.melee_in > 0.f)
            return true;
        auto const& files = p.template subsystem<BlamFiles<halo_version>>();
        auto const* equip = p.template get<BipedEquipment>(id);
        auto const* gun   = equip ? equip->held(files.load_generation) : nullptr;
        if(!gun || !gun->tag)
            return false;
        actor.melee_in = 1.5f;
        Vecf3 const eye = model.position + Vecf3{0, 0, eye_height};
        GameEvent       ev{.type = GameEvent::WeaponFire};
        WeaponFireEvent shot{
            .weapon     = gun->tag->as_ref(),
            .shooter    = id,
            .player_idx = ObjectHitEvent::no_player,
            .melee      = true,
            .position   = eye,
            .direction  = glm::normalize(actor.last_seen + Vecf3{0, 0, eye_height * .7f} - eye),
            .target     = actor.target,
        };
        p.template subsystem<GameEventBus>().inject(ev, &shot);
        if(std::getenv("BLAM_DEBUG_AI"))
            cDebug("ai: {} strikes {}", id, actor.target);
        return true;
    }

    /* Now and then, in range, a grenade lobbed at the target */
    void throw_grenade(Proxy& p, u64 id, Actor& actor, Model const& model, kind_t const& kind)
    {
        actv_t const& actv = *kind.variant;
        if(actor.grenades < 0)
            actor.grenades = static_cast<i16>(
                actv.grenade_count[1] > actv.grenade_count[0]
                    ? actv.grenade_count[0] + m_random() % (actv.grenade_count[1] - actv.grenade_count[0] + 1)
                    : std::max<i16>(actv.grenade_count[0], 0));
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
        Vecf3 const eye  = model.position + Vecf3{0, 0, eye_height};
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
            .position   = eye,
            .direction  = dir,
            .target     = actor.target,
        };
        p.template subsystem<GameEventBus>().inject(ev, &shot);
        if(std::getenv("BLAM_DEBUG_AI"))
            cDebug("ai: {} throws a grenade at {} ({} left)", id, actor.target, actor.grenades);
    }

    /* The globals' grenade of a type, by its projectile */
    blam::tagref_t const* grenade_projectile(Proxy& p, blam::scn::grenade_type_t type)
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
                    m_grenades.push_back(&static_cast<blam::tagref_t const&>(grenade.projectile));
        }
        size_t const i = static_cast<size_t>(type);
        return i < m_grenades.size() && m_grenades[i]->valid() ? m_grenades[i] : nullptr;
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
                            kind.actor = actr.value();
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
    };
    std::unique_ptr<TickQueue>                                       m_ticks;
    std::shared_ptr<GameEventBus::queue_type<WeaponFireEvent>>       m_fire_queue;
    std::vector<heard_t>                                             m_heard;
    blam::scn::scenario<halo_version> const*                         m_scenario{nullptr};
    static inline blam::map_ptr const*                               s_magic{nullptr};
    u32                                                              m_generation{0};
    std::map<std::tuple<i16, i16, i16>, kind_t>                      m_kinds;
    std::map<std::pair<i16, i16>, platoon_t>                         m_platoons;
    f32                                                              m_platoon_in{0.f};
    f32                                                              m_debug_in{0.f};
    f32                                                              m_debug_top{0.f};
    std::map<std::pair<blam::antr::header const*, std::string>, f32> m_strides;
    std::minstd_rand                                                 m_random{12345};
    std::vector<blam::tagref_t const*>                               m_grenades;
};

void alloc_actors(compo::EntityContainer& e)
{
    auto& actors   = e.register_subsystem_inplace<ActorSystem>();
    auto& gbus     = e.subsystem_cast<GameEventBus>();
    actors.m_ticks = std::make_unique<TickQueue>(gbus);
    gbus.addEventFunction<MapLoadEvent>(0, [&actors](auto&, auto*) {
        actors.m_scenario = nullptr;
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
                actors.m_heard.push_back({.shooter = shot->shooter, .position = shot->position});
        });
}
