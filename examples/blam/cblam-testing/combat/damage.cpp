#include "combat/damage.h"

#include "loading/blam_files.h"
#include "ai/allegiance.h"
#include "components.h"
#include "game/difficulty.h"
#include "game_events.h"
#include "game/game_mode.h"
#include "network/networking.h"
#include "world/object_functions.h"
#include "physics.h"
#include "selected_version.h"
#include "audio/sounds.h"
#include "ticks.h"

#include <blam/volta/blam_actor.h>
#include <blam/volta/blam_antr.h>
#include <blam/volta/blam_effects.h>

#include <coffee/components/proxy.h>
#include <coffee/components/restricted_subsystem.h>

#include <map>
#include <set>
#include <random>

using DamageManifest = compo::SubsystemManifest<
    type_list_t<
        BipedStatus,
        BipedEquipment,
        PlayerInfo,
        const PlayerCamera,
        Model,
        const ObjectPhysics,
        const PhysicsData,
        const Actor,
        const NetworkInfo>,
    type_list_t<
        GameEventBus,
        PhysicsBus,
        const GameState,
        const NetworkState,
        const BlamFiles<halo_version>,
        AllegianceState>,
    type_list_t<comp_app::EventBus<SoundEvent>>>;

/* Units lose shield and body to what strikes them. The authority (the
 * server, or a peer on its own) decides each blow and tells everyone what
 * is left; shields come back on every peer alike */
struct DamageSystem : compo::RestrictedSubsystem<DamageSystem, DamageManifest>
{
    using type   = DamageSystem;
    using Proxy  = compo::proxy_of<DamageManifest>;
    using jpt_t  = blam::scn::damage_effect;
    using flag_t = jpt_t::damage_flags_t;

    DamageSystem()
    {
        this->priority = 904;
    }

    void start_restricted(Proxy& p, time_point const&)
    {
        f32 dt = 0.f;
        for(auto const& tick : m_ticks->take())
            dt += tick.seconds;
        m_hit_queue->poll();
        m_area_queue->poll();
        m_vitality_queue->poll();
        m_struck_queue->poll();
        auto hits   = std::exchange(m_hits, {});
        auto areas  = std::exchange(m_areas, {});
        auto struck = std::exchange(m_struck, {});
        auto vitalities = std::exchange(m_vitality, {});
        for(auto const& vitality : vitalities)
            take(p, vitality);
        m_respawn_queue->poll();
        auto respawns = std::exchange(m_respawns, {});
        m_pickup_queue->poll();
        auto pickups = std::exchange(m_pickups, {});

        auto const& files = p.template subsystem<BlamFiles<halo_version>>();
        m_magic           = &files.container.magic;
        m_index.emplace(files.container);
        /* What was dying goes with the map it was on */
        if(std::exchange(m_generation, files.load_generation) != files.load_generation)
        {
            m_dying.clear();
            m_fall_grace.clear();
            m_corpse_age.clear();
            m_fall_top.clear();
            m_launched.clear();
            m_object_body.clear();
            m_gone.clear();
            m_destroyed.clear();
            m_broods.clear();
        }
        /* Looked up each frame: a map loading in replaces what a cached
         * pointer saw */
        m_difficulty = difficulty_block(files);
        m_falling    = falling_block(files);
        for(auto const& area : areas)
            blast(p, area);
        for(auto const& hit : hits)
            shove(p, hit);
        ting(p, hits, areas);
        feel(p, hits, areas, dt);
        bool const authority = !p.template subsystem<NetworkState>().replica;
        for(auto const& pickup : pickups)
            power_up(p, pickup, authority);
        if(authority)
        {
            for(auto const& hit : hits)
                if(auto const* jpt = damage_of(hit.damage))
                {
                    if(auto const* coll = breakable(p, hit.target))
                    {
                        break_object(p, hit.target, *coll, *jpt, 1.f);
                        continue;
                    }
                    /* Only a blow lethal to the unsuspecting kills from behind */
                    if(hit.behind &&
                       jpt->side_effect == blam::scn::damage_effect::side_effect_t::lethal_to_the_unsuspecting &&
                       assassinate(p, hit.target, player_of(p, hit.shooter)))
                        continue;
                    hurt(p, hit.target, *jpt, 1.f, hit.part, hit.head, player_of(p, hit.shooter), hit.direction, hit.point);
                }
            for(auto const& area : areas)
                splash(p, area);
            for(auto const& s : struck)
                run_down(p, s);
            for(auto const& respawn : respawns)
                revive(p, respawn);
            for(auto const& vitality : vitalities)
                put_down(p, vitality);
            fallen_out(p, dt);
            fallen_too_far(p);
            slammed(p, dt);
            fall_apart(p, dt);
            clear_corpses(p, dt);
            let_out(p, areas, dt);
        }
        recharge(p, dt);
        wear_off(p, dt);
        export_functions(p);
    }

    /* ---- Powerups ---- */

    /* Shield and health are the authority's to give; the timed ones run on
     * every peer that hears of the pickup */
    void power_up(Proxy& p, EquipWeaponEvent const& pickup, bool authority)
    {
        using powerup_t = blam::scn::equipment::powerup_type_t;
        auto it         = m_index->find(pickup.weapon);
        if(it == m_index->end() || !(*it).matches(blam::tag_class_t::eqip))
            return;
        auto eqip = (*it).template data<blam::scn::equipment>(*m_magic);
        auto* info   = p.template get<PlayerInfo>(pickup.entity_id);
        auto* status = p.template get<BipedStatus>(pickup.entity_id);
        if(!eqip.has_value() || !info || !status || status->dead)
            return;
        f32 const time = eqip.value()[0].powerup_time;
        switch(eqip.value()[0].powerup_type)
        {
        case powerup_t::over_shield:
            if(!authority || status->maximum_shield() <= 0.f)
                return;
            if(status->shield <= 0.f)
                shield_down(p, pickup.entity_id, false);
            status->shield           = BipedStatus::over_shield;
            status->overcharge_decay = time > 0.f ? (BipedStatus::over_shield - 1.f) / time : 0.f;
            status->stunned          = 0.f;
            break;
        case powerup_t::health:
            if(!authority)
                return;
            status->body = 1.f;
            break;
        case powerup_t::active_camouflage:
            info->powerups.camouflage = time;
            return;
        case powerup_t::double_speed:
            info->powerups.speed = time;
            return;
        case powerup_t::full_spectrum_vision:
            info->powerups.vision = time;
            return;
        default:
            return;
        }
        tell(p, pickup.entity_id, *status);
    }

    /* Timed powerups run out, and are lost with the body; a player's own
     * peer says whether it is camouflaged */
    void wear_off(Proxy& p, f32 dt)
    {
        for(auto ent : p.template select<PlayerInfo>())
        {
            auto& info   = ent.template get<PlayerInfo>();
            auto& left   = info.powerups;
            auto const* status = p.template get<BipedStatus>(ent.id());
            if(status && status->dead)
                left = {};
            for(f32* t : {&left.camouflage, &left.speed, &left.vision})
                *t = std::max(*t - dt, 0.f);
            /* Infection's infected fade from sight crouching */
            if(!info.is_remote())
                info.camouflaged = left.camouflage > 0.f ||
                                   (info.crouched && p.template subsystem<GameState>().infected(info.player_idx)) ||
                                   p.template subsystem<GameState>().unseen_by_game(info.player_idx);
        }
    }

    /* ---- Blows ---- */

    jpt_t const* damage_of(blam::tagref_t const& ref) const
    {
        if(!ref.valid())
            return nullptr;
        auto it = m_index->find(ref);
        if(it == m_index->end() || !(*it).matches(blam::tag_class_t::jpt))
            return nullptr;
        auto data = (*it).template data<jpt_t>(*m_magic);
        return data.has_value() ? data.value() : nullptr;
    }

    template<typename T>
    T const* tag_data(blam::tagref_t const& ref, blam::tag_class_t cls) const
    {
        if(!ref.valid())
            return nullptr;
        auto it = m_index->find(ref);
        if(it == m_index->end() || !(*it).matches(cls))
            return nullptr;
        auto data = (*it).template data<T>(*m_magic);
        return data.has_value() ? data.value() : nullptr;
    }

    /* The coll material a collision node is made of, -1 if unknown */
    i16 material_of(Proxy& p, u64 unit, i16 part) const
    {
        std::shared_ptr<CollisionGeometry const> geometry;
        if(auto const* info = p.template get<PlayerInfo>(unit))
            geometry = info->biped.collision;
        else if(auto const* physics = p.template get<ObjectPhysics>(unit))
            geometry = physics->collision;
        if(!geometry || part < 0 ||
           static_cast<size_t>(part) >= geometry->nodes.size())
            return -1;
        return geometry->nodes[part].material;
    }

    u32 player_of(Proxy& p, u64 entity) const
    {
        auto const* info = entity ? p.template get<PlayerInfo>(entity) : nullptr;
        return info ? info->player_idx : ObjectHitEvent::no_player;
    }

    /* A player out of physics mode is forging, and nothing hurts it */
    bool forging(Proxy& p, u64 unit) const
    {
        auto const* info = p.template get<PlayerInfo>(unit);
        return info && !info->mode.physics;
    }

    /* A strike in the back kills, unless the unit's tag says it shrugs
     * such off (hunters) */
    bool assassinate(Proxy& p, u64 unit, u32 killer)
    {
        auto* status = p.template get<BipedStatus>(unit);
        if(!status || !status->coll || status->dead || status->invulnerable ||
           forging(p, unit))
            return false;
        using flags_t = blam::scn::unit::unit_flags_t;
        if(auto const* model = p.template get<Model>(unit); model && model->origin_object)
            if(auto u = model->origin_object->template data<blam::scn::unit>(*m_magic);
               u.has_value() && (u.value()[0].unit_flags & flags_t::not_instantly_killed_by_melee) !=
                                    flags_t::none)
                return false;
        bool const shielded = status->shield > 0.f;
        status->shield = 0.f;
        status->body   = 0.f;
        status->dead   = true;
        if(shielded)
            shield_down(p, unit, true);
        last_words(p, unit, true);
        tell(p, unit, *status, {}, 0.f, false, false, true);
        killed(p, unit, killer);
        return true;
    }

    /* A jpt! landing on a unit, at `scale` of its full strength */
    void hurt(
        Proxy& p, u64 unit, jpt_t const& jpt, f32 scale, i16 part, bool head, u32 killer, Vecf3 blow = {},
        std::optional<Vecf3> at = {})
    {
        auto* status = p.template get<BipedStatus>(unit);
        if(!status || !status->coll || status->dead || status->invulnerable ||
           forging(p, unit))
            return;
        auto const& coll   = *status->coll;
        f32         amount = uniform(jpt.upper_bound) * scale;
        /* An enemy's blow on a player is scaled for the difficulty */
        if(killer == ObjectHitEvent::no_player && p.template get<PlayerInfo>(unit))
            amount *= difficulty_scale(
                m_difficulty, &blam::globals::difficulty::enemy_damage, p.template subsystem<GameState>().difficulty);
        f32         shield_mult = 1.f, body_mult = 1.f, leak = 0.f;
        u16         struck      = blam::scn::material_type_count;
        if(i16 const material = material_of(p, unit, part); material >= 0)
            if(auto materials = coll.materials.data(*m_magic);
               materials.has_value() &&
               static_cast<size_t>(material) < materials.value().size())
            {
                auto const& m = materials.value()[material];
                shield_mult   = m.shield_damage_multiplier;
                body_mult     = m.body_damage_multiplier;
                leak          = m.shield_leak_percentage;
                struck        = m.material_type;
            }
        /* Up, the shield is what is struck: the damage is modified for its
         * material, not the part's behind it */
        bool const shielded = status->shield > 0.f && status->maximum_shield() > 0.f &&
                              shield_mult > 0.f;
        amount *= modifier_of(jpt, shielded ? coll.shield_material_type : struck);
        /* A player's blow on a friend is softened by its coll, and strains
         * its team's patience */
        auto const* actor = killer != ObjectHitEvent::no_player ? p.template get<Actor>(unit) : nullptr;
        if(actor)
        {
            using team_t = blam::scn::unit::team_t;
            auto&      allegiance = p.template subsystem<AllegianceState>();
            auto const team       = static_cast<team_t>(actor->team);
            if(allegiance.allied(team, team_t::player))
            {
                amount *= 1.f - std::clamp(coll.friendly_damage_resistance, 0.f, 1.f);
                allegiance.betray(team, amount / std::max(status->maximum_body(), 1.f));
            }
        }
        /* A variant's player health: as much more for the body to take */
        if(p.template get<PlayerInfo>(unit))
            body_mult /= std::max(p.template subsystem<GameState>().player_health, .01f);
        /* A spent arm shield is down: what struck it reaches the body */
        if(body_mult <= 0.f && status->shield <= 0.f)
            body_mult = 1.f;
        bool const headshot =
            head && (jpt.damage_flags & flag_t::can_cause_headshots) != flag_t::none;
        bool const violent = amount >= status->maximum_body();
        f32 const  before  = status->shield;
        apply(*status, amount, shield_mult, body_mult, jpt.damage_flags, headshot, leak);
        if(before > 0.f && status->shield <= 0.f)
            shield_down(p, unit, true);
        bool pinged = false;
        if(status->dead)
            mark_blow(p, unit, *status, amount, head || headshot, blow, at);
        else
            pinged = mark_ping(p, unit, *status, amount, head || headshot, blow, at);
        if(status->dead)
            last_words(p, unit, violent);
        tell(p, unit, *status, {}, 0.f, false, false, violent, false, pinged);
        if(status->dead)
            killed(p, unit, killer);
    }

    /* A blow past the unit's soft or hard ping threshold makes it flinch */
    bool mark_ping(
        Proxy& p, u64 unit, BipedStatus& status, f32 amount, bool head, Vecf3 const& blow, std::optional<Vecf3> at)
    {
        auto const* model = p.template get<Model>(unit);
        if(!model || glm::dot(blow, blow) < 1e-8f)
            return false;
        f32 soft = -1.f, hard = -1.f, height = blow_waist * 2.f;
        if(auto const* info = p.template get<PlayerInfo>(unit))
        {
            soft   = info->biped.soft_ping;
            hard   = info->biped.hard_ping;
            height = info->biped.height;
        } else if(model->origin_object)
            if(auto u = model->origin_object->template data<blam::scn::unit>(*m_magic); u.has_value())
            {
                using flags_t = blam::scn::unit::unit_flags_t;
                auto const& t = u.value()[0];
                if((t.unit_flags & flags_t::resists_pings) == flags_t::none)
                {
                    if(t.soft_ping_threshold > 0.f || t.soft_ping_interrupt_time > 0.f)
                        soft = t.soft_ping_threshold;
                    if(t.hard_ping_threshold > 0.f || t.hard_ping_interrupt_time > 0.f)
                        hard = t.hard_ping_threshold;
                }
            }
        f32 const share = amount / std::max(status.maximum_body(), 1e-3f);
        bool const hard_ping = hard >= 0.f && share >= hard;
        if(!hard_ping && !(soft >= 0.f && share >= soft))
            return false;
        status.blow       = blow;
        status.blow_flags = (hard_ping ? BipedStatus::blow_hard : 0) | (head ? BipedStatus::blow_head : 0);
        if(at && at->z > model->position.z + height * .5f)
            status.blow_flags |= BipedStatus::blow_high;
        status.pings++;
        return true;
    }

    /* How the killing blow struck: hard past the unit's hard death
     * threshold, high above its waist */
    void mark_blow(
        Proxy& p, u64 unit, BipedStatus& status, f32 amount, bool head, Vecf3 const& blow, std::optional<Vecf3> at)
    {
        status.blow       = blow;
        status.blow_flags = head ? BipedStatus::blow_head : 0;
        auto const* model = p.template get<Model>(unit);
        if(!model)
            return;
        /* A player's body keeps its biped's numbers; an AI's has its tag */
        std::optional<f32> hard_death;
        f32                height = blow_waist * 2.f;
        if(auto const* info = p.template get<PlayerInfo>(unit))
        {
            hard_death = info->biped.hard_death;
            height     = info->biped.height;
        } else if(model->origin_object)
        {
            if(auto u = model->origin_object->template data<blam::scn::unit>(*m_magic); u.has_value())
                hard_death = u.value()[0].hard_death_threshold;
            if(model->origin_object->matches(blam::tag_class_t::bipd))
                if(auto b = model->origin_object->template data<blam::scn::biped>(*m_magic);
                   b.has_value() && b.value()[0].standing_collision_height > 0.f)
                    height = b.value()[0].standing_collision_height;
        }
        if(hard_death && amount >= *hard_death * std::max(status.maximum_body(), 1e-3f))
            status.blow_flags |= BipedStatus::blow_hard;
        if(at && at->z > model->position.z + height * .5f)
            status.blow_flags |= BipedStatus::blow_high;
    }

    /* A jpt!'s modifier for a material type. A list with any entry set is
     * authored, and a zero in it means nothing gets through; a blank list
     * leaves the damage alone */
    static f32 modifier_of(jpt_t const& jpt, u16 material)
    {
        if(material >= blam::scn::material_type_count)
            return 1.f;
        bool authored = false;
        for(u32 i = 0; i < blam::scn::material_type_count; i++)
            authored = authored || jpt.material_modifiers[i] != 0.f;
        return authored ? jpt.material_modifiers[material] : 1.f;
    }

    /* Shield first, with `leak` of the blow passing through it, then
     * whatever gets past it into the body. Unshielded, a headshot from a
     * weapon that can make one kills */
    void apply(
        BipedStatus& status,
        f32          amount,
        f32          shield_mult,
        f32          body_mult,
        flag_t       flags,
        bool         headshot,
        f32          leak = 0.f)
    {
        auto const& coll       = *status.coll;
        f32 const   max_shield = status.maximum_shield();
        f32 const   max_body   = std::max(status.maximum_body(), 1.f);
        /* A part the shield does not cover is struck straight in the body;
         * one the body is not behind (a jackal's arm shield) only drains
         * the shield */
        bool const shielded =
            status.shield > 0.f && max_shield > 0.f && shield_mult > 0.f;
        if(amount <= 0.f)
            return;
        status.stunned = coll.stun_time > 0.f ? coll.stun_time : default_stun;
        if(shielded && (flags & flag_t::skips_shields) == flag_t::none)
        {
            status.recent_shield = 1.f;
            f32 const leaked = amount * std::clamp(leak, 0.f, 1.f);
            f32 const soak   = status.shield * max_shield / shield_mult;
            status.shield    = std::max(status.shield - amount * shield_mult / max_shield, 0.f);
            amount           = std::max(amount - soak, 0.f) + leaked;
        }
        if(amount <= 0.f || body_mult <= 0.f ||
           (flags & flag_t::only_hurts_shields) != flag_t::none)
            return;
        status.body = headshot && !shielded
                          ? 0.f
                          : status.body - amount * body_mult / max_body;
        if(status.body < dead_below)
            status.body = 0.f;
        status.dead = status.body <= 0.f;
        /* The dead keep no shield up, a jackal's gate included */
        if(status.dead)
            status.shield = 0.f;
    }

    /* Everything near enough takes some, less further out */
    void splash(Proxy& p, AreaDamageEvent const& area)
    {
        auto const* jpt = damage_of(area.damage);
        if(!jpt || jpt->radius.y <= 0.f)
            return;
        bool const flat =
            (jpt->flags & jpt_t::flags_t::do_not_scale_damage_by_distance) !=
            jpt_t::flags_t::none;
        for(auto unit : p.template select<BipedStatus>())
        {
            auto const centre = centre_of(p, unit.id());
            if(!centre)
                continue;
            /* Measured to the body's bounding sphere, as Halo does: a blast
             * at the muzzle reaches the one holding the gun */
            auto const* model  = p.template get<Model>(unit.id());
            auto const* info   = p.template get<PlayerInfo>(unit.id());
            f32         radius = model ? model->render_radius : 0.f;
            if(info)
                radius = std::max(radius, info->biped.height * .5f);
            f32 const d = std::max(glm::distance(*centre, area.position) - radius, 0.f);
            if(d > jpt->radius.y || shielded(p, area.position, *centre, unit.id()))
                continue;
            f32 const inner = std::min(jpt->radius.x, jpt->radius.y);
            f32 const scale =
                flat || d <= inner
                    ? 1.f
                    : 1.f - (d - inner) / std::max(jpt->radius.y - inner, 1e-3f);
            hurt(p, unit.id(), *jpt, scale, -1, false, player_of(p, area.shooter), *centre - area.position, area.position);
        }
        for(auto ent : p.template select<Model>())
        {
            auto const& model = ent.template get<Model>();
            f32 const   d = std::max(glm::distance(model.position, area.position) - model.render_radius, 0.f);
            if(d > jpt->radius.y)
                continue;
            auto const* coll = breakable(p, ent.id());
            if(!coll || shielded(p, area.position, model.position, ent.id()))
                continue;
            f32 const inner = std::min(jpt->radius.x, jpt->radius.y);
            f32 const scale =
                flat || d <= inner ? 1.f : 1.f - (d - inner) / std::max(jpt->radius.y - inner, 1e-3f);
            break_object(p, ent.id(), *coll, *jpt, scale);
        }
        std::erase_if(m_object_body, [&p](auto const& e) { return !p.template get<Model>(e.first); });
    }

    /* The level stands between a blast and what it would reach: walls and
     * floors shield, objects in the way do not */
    bool shielded(Proxy& p, Vecf3 from, Vecf3 const& to, u64 target)
    {
        Vecf3 const along = to - from;
        if(glm::dot(along, along) < 1e-6f)
            return false;
        Vecf3 const dir  = glm::normalize(along);
        from += dir * blast_start;
        u64 past = 0;
        for(i32 i = 0; i < blast_casts; i++)
        {
            Physics::Event   ev{Physics::Event::Raycast};
            Physics::Raycast ray{.from = from, .to = to, .ignore = target, .ignore_also = past};
            p.template subsystem<PhysicsBus>().process(ev, &ray);
            if(!ray.hit)
                return false;
            if(ray.entity_id == 0)
                return true;
            past = ray.entity_id;
            from = ray.point + dir * blast_start;
        }
        return false;
    }
    static constexpr f32 blast_start = .02f; /* wu off the surface a blast sits on */
    static constexpr i32 blast_casts = 4;    /* objects it looks past for a wall */

    /* ---- Objects that are not units ---- */

    /* Its coll, where the tag gives its body something to do when spent
     * (cursed TNT primes); other scenery and items take no harm */
    blam::coll::header const* breakable(Proxy& p, u64 entity) const
    {
        auto const* model = p.template get<Model>(entity);
        if(!model || !model->origin_object || p.template get<BipedStatus>(entity))
            return nullptr;
        auto obj = model->origin_object->template data<blam::scn::object>(*m_magic);
        if(!obj.has_value())
            return nullptr;
        auto const* coll = tag_data<blam::coll::header>(obj.value()[0].collider, blam::tag_class_t::coll);
        if(!coll || coll->maximum_body_vitality <= 0.f)
            return nullptr;
        if(!coll->body_depleted_effect.valid() && !coll->body_destroyed_effect.valid() &&
           !destroyed_dead(*model))
            return nullptr;
        return coll;
    }

    /* A vehicle gone as soon as it dies (the cursed D20 spawners) */
    bool destroyed_dead(Model const& model) const
    {
        using flags_t = blam::scn::unit::unit_flags_t;
        if(!model.origin_object->matches(blam::tag_class_t::vehi))
            return false;
        auto u = model.origin_object->template data<blam::scn::unit>(*m_magic);
        return u.has_value() && (u.value()[0].unit_flags & flags_t::destroyed_after_dying) != flags_t::none;
    }

    /* Spent, it plays its depleted effect on itself; past its destroyed
     * threshold it plays that one and is taken away */
    void break_object(Proxy& p, u64 entity, blam::coll::header const& coll, jpt_t const& jpt, f32 scale)
    {
        f32& body         = m_object_body.try_emplace(entity, 1.f).first->second;
        f32 const before  = body;
        auto const* model = p.template get<Model>(entity);
        f32 const   gone  = model && destroyed_dead(*model) ? 0.f : std::min(coll.body_destroyed_threshold, 0.f);
        if(before <= gone)
            return;
        body -= uniform(jpt.upper_bound) * scale / coll.maximum_body_vitality;
        auto const play = [&](blam::tagref_t const& effect, u64 parent) {
            auto const* model = p.template get<Model>(entity);
            if(!effect.valid() || !model)
                return;
            GameEvent        ev{.type = GameEvent::SpawnEffect};
            SpawnEffectEvent spawn{.effect = effect, .position = model->position, .direction = {0.f, 0.f, 1.f}, .parent = parent};
            p.template subsystem<GameEventBus>().inject(ev, &spawn);
        };
        if(before > 0.f && body <= 0.f)
            play(coll.body_depleted_effect, entity);
        if(body > gone)
            return;
        play(coll.body_destroyed_effect, 0);
        if(auto const* net = p.template get<NetworkInfo>(entity); net && net->instance_id)
            m_gone.push_back(net->instance_id);
    }

    /* Every peer throws what it moves itself, so this is not left to the
     * authority */
    void blast(Proxy& p, AreaDamageEvent const& area)
    {
        auto const* jpt = damage_of(area.damage);
        /* TODO: use_3d_instantaneous_acceleration, unused in stock maps */
        if(!jpt || jpt->instantaneous_acceleration.x == 0.f || jpt->radius.y <= 0.f)
            return;
        bool const flat =
            (jpt->flags & jpt_t::flags_t::do_not_scale_damage_by_distance) !=
            jpt_t::flags_t::none;
        Physics::Event ev{Physics::Event::Blast};
        Physics::Blast blast{
            .position = area.position,
            .radius   = {flat ? jpt->radius.y : jpt->radius.x, jpt->radius.y},
            .speed    = jpt->instantaneous_acceleration.x,
        };
        p.template subsystem<PhysicsBus>().inject(ev, &blast);
        for(auto ent : p.template select<BipedStatus, Model>())
            if(glm::distance(ent.template get<Model>().position, area.position) <= jpt->radius.y)
                launch(ent.id(), player_of(p, area.shooter));
    }

    /* In multiplayer a shot of a local player's that strikes another player
     * tings for them, once a frame however many pellets landed */
    /* What a damage effect does to those it reaches besides hurting them:
     * its sound where it lands, and its screen flash for a local player */
    void feel(Proxy& p, std::vector<ObjectHitEvent> const& hits, std::vector<AreaDamageEvent> const& areas, f32 dt)
    {
        for(auto player : p.template select<PlayerInfo>())
        {
            auto& flash = player.template get<PlayerInfo>().damage_flash;
            flash.left  = std::max(flash.left - dt, 0.f);
        }
        auto const flash = [&](u64 unit, blam::scn::damage_effect const& jpt, f32 scale) {
            auto* info = p.template get<PlayerInfo>(unit);
            auto const& sf = jpt.screen_flash;
            if(!info || info->is_remote() || sf.type == blam::scn::damage_effect::screen_flash_type_t::none ||
               sf.duration <= 0.f || sf.maximum_intensity * scale <= 0.f)
                return;
            auto& now = info->damage_flash;
            if(now.left > 0.f && static_cast<u16>(sf.priority) < now.priority)
                return;
            now = {
                .color     = Vecf3(sf.color.y, sf.color.z, sf.color.w), /* argb */
                .type      = static_cast<u16>(sf.type),
                .priority  = static_cast<u16>(sf.priority),
                .intensity = std::min(sf.maximum_intensity * scale, 1.f),
                .duration  = sf.duration,
                .left      = sf.duration,
            };
        };
        auto* sounds = p.template service<comp_app::EventBus<SoundEvent>>();
        auto const sound = [&](blam::scn::damage_effect const& jpt, Vecf3 const& at) {
            if(!sounds || !jpt.sound.valid())
                return;
            SoundEvent     ev{.type = SoundEvent::play_sound};
            PlaySoundEvent play{.sound = &static_cast<blam::tagref_t const&>(jpt.sound), .position = at};
            sounds->inject(ev, &play);
        };
        for(auto const& hit : hits)
            if(auto const* jpt = damage_of(hit.damage))
            {
                flash(hit.target, *jpt, 1.f);
                if(p.template get<BipedStatus>(hit.target))
                    sound(*jpt, hit.point);
            }
        for(auto const& area : areas)
        {
            auto const* jpt = damage_of(area.damage);
            if(!jpt || jpt->radius.y <= 0.f)
                continue;
            sound(*jpt, area.position);
            /* Full within the inner radius, none at the outer */
            for(auto unit : p.template select<PlayerInfo, BipedStatus>())
            {
                auto const centre = centre_of(p, unit.id());
                if(!centre || unit.template get<BipedStatus>().dead)
                    continue;
                f32 const d = glm::distance(*centre, area.position);
                if(d > jpt->radius.y || shielded(p, area.position, *centre, unit.id()))
                    continue;
                f32 const inner = std::min(jpt->radius.x, jpt->radius.y);
                f32 const scale = d <= inner ? 1.f : 1.f - (d - inner) / std::max(jpt->radius.y - inner, 1e-3f);
                flash(unit.id(), *jpt, scale);
            }
        }
    }

    void ting(Proxy& p, std::vector<ObjectHitEvent> const& hits, std::vector<AreaDamageEvent> const& areas)
    {
        if(p.template subsystem<GameState>().kind != GameState::kind_t::multiplayer)
            return;
        std::set<u32> heard;
        /* A blast of theirs reaching another player does too */
        for(auto const& area : areas)
        {
            auto const* shooter = p.template get<PlayerInfo>(area.shooter);
            auto const* jpt     = damage_of(area.damage);
            if(!shooter || shooter->is_remote() || !jpt || jpt->radius.y <= 0.f || jpt->upper_bound.y <= 0.f ||
               heard.contains(shooter->player_idx))
                continue;
            for(auto unit : p.template select<PlayerInfo, BipedStatus>())
            {
                auto const centre = centre_of(p, unit.id());
                if(unit.id() == area.shooter || unit.template get<BipedStatus>().dead || !centre ||
                   glm::distance(*centre, area.position) > jpt->radius.y ||
                   shielded(p, area.position, *centre, unit.id()))
                    continue;
                heard.insert(shooter->player_idx);
                GameEvent     ev{.type = GameEvent::Announce};
                AnnounceEvent say{.sound = ting_sound, .player = shooter->player_idx};
                p.template subsystem<GameEventBus>().inject(ev, &say);
                break;
            }
        }
        for(auto const& hit : hits)
        {
            auto const* shooter = p.template get<PlayerInfo>(hit.shooter);
            auto const* target  = p.template get<PlayerInfo>(hit.target);
            auto const* status  = p.template get<BipedStatus>(hit.target);
            if(!shooter || !target || shooter->is_remote() || hit.shooter == hit.target || !status ||
               status->dead || !heard.insert(shooter->player_idx).second)
                continue;
            GameEvent     ev{.type = GameEvent::Announce};
            AnnounceEvent say{.sound = ting_sound, .player = shooter->player_idx};
            p.template subsystem<GameEventBus>().inject(ev, &say);
        }
    }

    /* Thrown: for a while, what it slams into hurts it */
    void launch(u64 unit, u32 by)
    {
        m_launched[unit] = {.left = launch_window, .by = by};
    }

    /* A launched unit stopped hard by a wall, the floor or an object takes
     * the globals' falling damage by how fast it hit: from nothing at a
     * harmful fall's speed to all of it at a lethal one's, and on past it */
    void slammed(Proxy& p, f32 dt)
    {
        if(dt <= 0.f || !m_falling)
            return;
        auto const* falling = damage_of(m_falling->falling);
        Vecf2 const heights = m_falling->harmful_falling_distance;
        f32 const   harmful = std::sqrt(2.f * halo_gravity * heights.x);
        f32 const   lethal  = std::sqrt(2.f * halo_gravity * std::max(heights.y, heights.x + 1.f));
        for(auto it = m_launched.begin(); it != m_launched.end();)
        {
            auto* status      = p.template get<BipedStatus>(it->first);
            auto const* model = p.template get<Model>(it->first);
            auto&       l     = it->second;
            if(!status || !model || status->dead || (l.left -= dt) <= 0.f)
            {
                it = m_launched.erase(it);
                continue;
            }
            auto const* body     = p.template get<PhysicsData>(it->first);
            auto const* object   = p.template get<ObjectPhysics>(it->first);
            Vecf3 const velocity = body && body->enabled ? body->moving
                                   : object              ? object->linear_velocity
                                                         : Vecf3{};
            /* Moved faster than anything flies: teleported, start over */
            if(glm::length(velocity) > teleport_speed)
            {
                l.primed = false;
                ++it;
                continue;
            }
            f32 const lost = glm::length(l.velocity - velocity);
            if(l.primed && glm::length(velocity) < glm::length(l.velocity) && lost > harmful)
            {
                if(falling)
                    hurt(p, it->first, *falling, (lost - harmful) / (lethal - harmful), -1, false, l.by);
                it = m_launched.erase(it);
                continue;
            }
            l.velocity = velocity;
            l.primed   = true;
            ++it;
        }
    }

    /* A direct hit with an acceleration pushes what it struck along the
     * shot, or pulls it back when negative (the cursed toolgun) */
    void shove(Proxy& p, ObjectHitEvent const& hit)
    {
        auto const* jpt = damage_of(hit.damage);
        if(!jpt || !hit.target || jpt->instantaneous_acceleration.x == 0.f || jpt->radius.y > 0.f ||
           glm::length(hit.direction) <= 0.f)
            return;
        Physics::Event ev{Physics::Event::Blast};
        Physics::Blast blast{
            .position  = hit.point,
            .speed     = jpt->instantaneous_acceleration.x,
            .target    = hit.target,
            .direction = glm::normalize(hit.direction),
        };
        p.template subsystem<PhysicsBus>().inject(ev, &blast);
        if(p.template get<BipedStatus>(hit.target))
            launch(hit.target, player_of(p, hit.shooter));
    }

    /* A vehicle hurts what it runs into by how fast it came. A player in full
     * health is never killed outright; the AI's are run over */
    void run_down(Proxy& p, Physics::UnitStruck const& struck)
    {
        auto* status = p.template get<BipedStatus>(struck.unit);
        if(!status || !status->coll || status->dead || status->invulnerable ||
           forging(p, struck.unit))
            return;
        bool const  player = p.template get<PlayerInfo>(struck.unit) != nullptr;
        f32 const   share = glm::mix(
            player ? run_down_least : run_down_npc_least,
            player ? run_down_most : run_down_npc_most,
            std::clamp(
                (struck.speed - run_down_slow) / (run_down_fast - run_down_slow),
                0.f,
                1.f));
        f32 const total = status->maximum_body() + std::max(status->maximum_shield(), 0.f);
        f32 const before = status->shield;
        apply(*status, share * total, 1.f, 1.f, flag_t::none, false);
        if(before > 0.f && status->shield <= 0.f)
            shield_down(p, struck.unit, true);
        if(status->dead)
            last_words(p, struck.unit, true);
        tell(p, struck.unit, *status, {}, 0.f, false, false, true);
        if(!status->dead)
            return;
        /* The driver did it */
        u32 driver = ObjectHitEvent::no_player;
        for(auto player : p.template select<PlayerInfo>())
            if(auto const& riding = player.template get<PlayerInfo>().riding;
               riding.vehicle == struck.vehicle && riding.driver)
                driver = player.template get<PlayerInfo>().player_idx;
        killed(p, struck.unit, driver, UnitKilledEvent::vehicle);
    }

    std::optional<Vecf3> centre_of(Proxy& p, u64 unit) const
    {
        if(auto const* info = p.template get<PlayerInfo>(unit))
        {
            /* A rider is where it sits */
            if(info->riding.vehicle != 0)
            {
                auto const* model = p.template get<Model>(unit);
                return model ? std::optional(model->position + Vecf3{0.f, 0.f, info->biped.height * .5f})
                             : std::nullopt;
            }
            auto const* cam = p.template get<PlayerCamera>(unit);
            if(!cam)
                return std::nullopt;
            return cam->camera.position - Vecf3{0.f, 0.f, info->eye_offset()};
        }
        if(auto const* model = p.template get<Model>(unit))
            return model->position + Vecf3{0.f, 0.f, unit_middle};
        return std::nullopt;
    }

    /* ---- Destroyed after dying ---- */

    blam::scn::unit const* unit_of(Proxy& p, u64 unit) const
    {
        auto const* model = p.template get<Model>(unit);
        if(!model || !model->origin_object)
            return nullptr;
        auto u = model->origin_object->template data<blam::scn::unit>(*m_magic);
        return u.has_value() ? &u.value()[0] : nullptr;
    }

    /* A unit its tag destroys after dying (flood carriers and infection
     * forms) goes once its kill animation has played */
    void doom(Proxy& p, u64 unit)
    {
        using flags_t = blam::scn::unit::unit_flags_t;
        auto const* u = unit_of(p, unit);
        if(u && (u->unit_flags & flags_t::destroyed_after_dying) != flags_t::none &&
           !m_destroyed.contains(unit))
            m_dying.emplace(unit, kill_time(*u));
    }

    f32 kill_time(blam::scn::unit const& u) const
    {
        auto const* graph = tag_data<blam::antr::header>(u.anim_graph, blam::tag_class_t::antr);
        if(!graph)
            return 0.f;
        auto animations = graph->animations.data(*m_magic);
        if(!animations.has_value())
            return 0.f;
        for(auto const& animation : animations.value())
            if(animation.name.str().find("kill") != std::string_view::npos)
                return static_cast<f32>(frame_count(animation)) / frame_rate(animation);
        return 0.f;
    }

    /* A death raised elsewhere: a script's, done already, or one asked for
     * (a carrier at its target) */
    void put_down(Proxy& p, UnitVitalityEvent const& vitality)
    {
        auto* status = p.template get<BipedStatus>(vitality.unit);
        if(!status || vitality.origin != MapLoadEvent::Local || !vitality.dead ||
           vitality.destroyed)
            return;
        if(status->dead)
            return doom(p, vitality.unit);
        status->shield = status->body = 0.f;
        status->dead   = true;
        last_words(p, vitality.unit, false);
        tell(p, vitality.unit, *status);
        killed(p, vitality.unit, ObjectHitEvent::no_player);
    }

    /* A living unit that fell out of the level dies there; one just brought
     * back is left a moment to be moved to its spawn */
    void fallen_out(Proxy& p, f32 dt)
    {
        for(auto ent : p.template select<BipedStatus, Model>())
        {
            auto [status, model] = ent.components();
            f32& grace           = m_fall_grace[ent.id()];
            if(status.dead)
            {
                grace = revive_grace;
                continue;
            }
            if(grace > 0.f)
            {
                grace -= dt;
                continue;
            }
            /* A forge camera passes through walls and voids at will */
            if(auto const* info = p.template get<PlayerInfo>(ent.id());
               info && (!info->mode.physics || info->riding.vehicle))
                continue;
            Physics::Event      ev{Physics::Event::OutOfWorld};
            Physics::OutOfWorld query{.position = model.position};
            p.template subsystem<PhysicsBus>().process(ev, &query);
            if(!query.out)
                continue;
            status.shield = status.body = 0.f;
            status.dead   = true;
            last_words(p, ent.id(), false);
            tell(p, ent.id(), status);
            killed(p, ent.id(), ObjectHitEvent::no_player);
        }
    }

    /* No landing hurts, but a fall past the globals' maximum falling
     * distance is death before the ground is reached */
    void fallen_too_far(Proxy& p)
    {
        f32 const most = m_falling ? m_falling->maximum_falling_distance : 0.f;
        if(most <= 0.f)
            return;
        for(auto ent : p.template select<BipedStatus, Model>())
        {
            auto [status, model] = ent.components();
            auto const* info     = p.template get<PlayerInfo>(ent.id());
            auto const* object   = p.template get<ObjectPhysics>(ent.id());
            auto const* body     = p.template get<PhysicsData>(ent.id());
            bool const  held     = (info && (info->riding.vehicle || !info->mode.physics || info->suspended)) ||
                              (object && (object->scripted || object->seated || object->teleported));
            bool const grounded = (body && body->grounded) || (object && object->grounded);
            /* A fall is counted from where the unit was first seen */
            f32& top = m_fall_top.try_emplace(ent.id(), model.position.z).first->second;
            if(status.dead || held || grounded || !(body || object))
            {
                top = model.position.z;
                continue;
            }
            top = std::max(top, model.position.z);
            if(top - model.position.z <= most)
                continue;
            status.shield = status.body = 0.f;
            status.dead   = true;
            last_words(p, ent.id(), true);
            tell(p, ent.id(), status);
            killed(p, ent.id(), ObjectHitEvent::no_player);
        }
    }

    /* Its time up, a dead unit bursts and is taken away the frame after,
     * once everyone has been told */
    void fall_apart(Proxy& p, f32 dt)
    {
        for(u32 net_id : std::exchange(m_gone, {}))
        {
            GameEvent          ev{.type = GameEvent::DespawnObject};
            DespawnObjectEvent gone{.net_id = net_id};
            p.template subsystem<GameEventBus>().inject(ev, &gone);
        }
        std::erase_if(m_destroyed, [&p](u64 unit) { return !p.template get<BipedStatus>(unit); });
        for(auto it = m_dying.begin(); it != m_dying.end();)
        {
            auto const* status = p.template get<BipedStatus>(it->first);
            if(status && status->dead && (it->second -= dt) > 0.f)
            {
                ++it;
                continue;
            }
            if(status && status->dead)
            {
                burst(p, it->first);
                hatch(p, it->first);
                tell(p, it->first, *status, {}, 0.f, false, false, false, true);
                if(auto const* net = p.template get<NetworkInfo>(it->first); net && net->instance_id)
                    m_gone.push_back(net->instance_id);
                m_destroyed.insert(it->first);
            }
            it = m_dying.erase(it);
        }
    }

    /* Dead NPCs are taken away quietly: in multiplayer after a while, and
     * anywhere the oldest past a cap; never from under a player's feet */
    void clear_corpses(Proxy& p, f32 dt)
    {
        std::vector<Vecf3> players;
        for(auto ent : p.template select<PlayerInfo, Model>())
            if(ent.template get<PlayerInfo>().spawned)
                players.push_back(ent.template get<Model>().position);
        bool const timed = p.template subsystem<GameState>().kind == GameState::kind_t::multiplayer;
        std::vector<std::pair<f32, u64>> bodies;
        std::set<u64>                    lying;
        for(auto ent : p.template select<BipedStatus, Model>())
        {
            auto const& [status, model] = ent.components();
            auto const* net             = p.template get<NetworkInfo>(ent.id());
            if(!status.dead || !net || net->instance_id == 0 || !p.template get<Actor>(ent.id()) ||
               m_dying.contains(ent.id()) || m_destroyed.contains(ent.id()))
                continue;
            lying.insert(ent.id());
            f32& age = m_corpse_age[ent.id()];
            age += dt;
            bool const near = std::any_of(players.begin(), players.end(), [&](Vecf3 const& at) {
                return glm::distance(at, model.position) < corpse_clear_distance;
            });
            if(!near)
                bodies.emplace_back(age, ent.id());
        }
        std::erase_if(m_corpse_age, [&](auto const& kv) { return !lying.contains(kv.first); });
        std::sort(bodies.begin(), bodies.end(), std::greater<>());
        size_t const over = lying.size() > max_corpses ? lying.size() - max_corpses : 0;
        for(size_t i = 0; i < bodies.size(); i++)
        {
            if(i >= over && !(timed && bodies[i].first >= corpse_time))
                continue;
            auto const id = bodies[i].second;
            GameEvent          ev{.type = GameEvent::DespawnObject};
            DespawnObjectEvent gone{.net_id = p.template get<NetworkInfo>(id)->instance_id};
            p.template subsystem<GameEventBus>().inject(ev, &gone);
            m_destroyed.insert(id);
        }
    }

    /* Its coll's body destroyed effect, where it lies */
    void burst(Proxy& p, u64 unit)
    {
        if(auto const* status = p.template get<BipedStatus>(unit); status && status->coll)
            play_at(p, unit, status->coll->body_destroyed_effect);
    }

    void play_at(Proxy& p, u64 unit, blam::tagref_t const& effect)
    {
        auto centre = centre_of(p, unit);
        if(!effect.valid() || !centre)
            return;
        GameEvent        ev{.type = GameEvent::SpawnEffect};
        SpawnEffectEvent play{
            .effect    = effect,
            .position  = *centre,
            .direction = {0.f, 0.f, 1.f},
        };
        p.template subsystem<GameEventBus>().inject(ev, &play);
    }

    /* What a unit carries, its tag's spawned actors, kept in until its
     * burst has gone off so that it does not kill them */
    void hatch(Proxy& p, u64 unit)
    {
        auto const* u     = unit_of(p, unit);
        auto const* model = p.template get<Model>(unit);
        if(!u || !model)
            return;
        auto const* actv =
            tag_data<blam::scn::actor_variant>(u->spawned_actor, blam::tag_class_t::actv);
        if(!actv || !actv->unit.valid())
            return;
        i32 const least = std::max<i32>(u->spawned_actor_count[0], 0);
        i32 const most  = std::max<i32>(u->spawned_actor_count[1], least);
        brood_t   brood{
              .at      = model->position,
              .burst   = centre_of(p, unit).value_or(model->position),
              .variant = u->spawned_actor,
              .unit    = actv->unit,
              .weapon  = actv->weapon,
              .count   = std::uniform_int_distribution<i32>(least, most)(m_random),
              .speed   = u->spawned_velocity,
              .wait    = hatch_wait,
        };
        if(auto const* actor = p.template get<Actor>(unit))
        {
            brood.encounter = actor->encounter;
            brood.squad     = actor->squad;
            brood.team      = actor->team;
        }
        if(auto const* net = p.template get<NetworkInfo>(unit))
            brood.like = net->instance_id;
        if(brood.count > 0)
            m_broods.push_back(brood);
    }

    /* Thrown out at the unit tag's spawned velocity, they join its squad */
    void let_out(Proxy& p, std::vector<AreaDamageEvent> const& areas, f32 dt)
    {
        auto& bus = p.template subsystem<GameEventBus>();
        for(auto it = m_broods.begin(); it != m_broods.end();)
        {
            /* Out the frame after the burst, once its blast has thrown what it will */
            bool const went_off = std::any_of(areas.begin(), areas.end(), [&it](AreaDamageEvent const& a) {
                return glm::distance(a.position, it->burst) < 1e-2f;
            });
            if(went_off)
                it->wait = 0.f;
            if(went_off || (it->wait -= dt) > 0.f)
            {
                ++it;
                continue;
            }
            for(i32 i = 0; i < it->count; i++)
            {
                f32 const   turn = std::uniform_real_distribution<f32>(0.f, glm::two_pi<f32>())(m_random);
                Vecf3 const out{std::cos(turn), std::sin(turn), 0.f};
                SpawnObjectEvent spawn{
                    .object       = it->unit,
                    .position     = it->at + out * hatch_spread + Vecf3{0.f, 0.f, spawn_lift},
                    .rotation     = glm::angleAxis(turn, Vecf3{0.f, 0.f, 1.f}),
                    .server_owned = true,
                    .velocity     = glm::normalize(out + Vecf3{0.f, 0.f, 1.f}) * it->speed,
                    .held         = it->weapon,
                };
                GameEvent ev{.type = GameEvent::SpawnObject};
                bus.inject(ev, &spawn);
                if(spawn.net_id == 0 || it->encounter < 0)
                    continue;
                GameEvent        enlist_ev{.type = GameEvent::EnlistActor};
                EnlistActorEvent enlist{
                    .net_id    = spawn.net_id,
                    .variant   = it->variant,
                    .like      = it->like,
                    .encounter = it->encounter,
                    .squad     = it->squad,
                    .team      = it->team,
                    .post      = spawn.position,
                };
                bus.inject(enlist_ev, &enlist);
            }
            it = m_broods.erase(it);
        }
    }

    /* ---- What everyone sees ---- */

    void tell(Proxy& p, u64 unit, BipedStatus const& status,
              std::optional<Vecf3> respawn_at = {}, f32 facing = 0.f, bool arrived = false,
              bool fresh = false, bool violent = false, bool destroyed = false, bool ping = false)
    {
        auto const*       info = p.template get<PlayerInfo>(unit);
        GameEvent         ev{.type = GameEvent::UnitVitality};
        UnitVitalityEvent vitality{
            .unit           = unit,
            .player         = info ? info->player_idx : ObjectHitEvent::no_player,
            .body           = status.body,
            .shield         = status.shield,
            .dead           = status.dead,
            .violent        = violent,
            .respawn_at     = respawn_at,
            .respawn_facing = facing,
            .arrived        = arrived,
            .fresh          = fresh,
            .destroyed      = destroyed,
            .blow           = status.dead || ping ? status.blow : Vecf3{},
            .blow_flags     = status.dead || ping ? status.blow_flags : libc_types::u8(0),
            .ping           = ping && !status.dead,
        };
        p.template subsystem<GameEventBus>().inject(ev, &vitality);
    }

    /* Its coll's body depleted effect, and its dialogue's death line, loud
     * or quiet by how it went */
    void last_words(Proxy& p, u64 unit, bool violent)
    {
        if(auto const* status = p.template get<BipedStatus>(unit); status && status->coll)
            play_at(p, unit, status->coll->body_depleted_effect);
        auto const* model  = p.template get<Model>(unit);
        auto*       sounds = p.template service<comp_app::EventBus<SoundEvent>>();
        if(!model || !model->origin_object || !sounds)
            return;
        auto u = model->origin_object->template data<blam::scn::unit>(*m_magic);
        if(!u.has_value())
            return;
        auto variants = u.value()[0].dialogue_variants.data(*m_magic);
        if(!variants.has_value() || variants.value().empty())
            return;
        auto const* dialogue = tag_data<blam::scn::unit_dialogue>(
            variants.value()[0].dialogue, blam::tag_class_t::udlg);
        if(!dialogue)
            return;
        auto const& line = violent && dialogue->death_violent.valid()
                               ? dialogue->death_violent
                               : dialogue->death_quiet;
        if(!line.valid())
            return;
        SoundEvent     ev{.type = SoundEvent::play_sound};
        PlaySoundEvent play{
            .sound    = &static_cast<blam::tagref_t const&>(line),
            .position = model->position + Vecf3{0.f, 0.f, .5f},
        };
        sounds->inject(ev, &play);
    }

    /* The game mode decides what a death is worth and when it ends */
    void killed(Proxy& p, u64 unit, u32 killer, std::optional<UnitKilledEvent::cause_t> cause = std::nullopt)
    {
        GameEvent       ev{.type = GameEvent::UnitKilled};
        UnitKilledEvent kill{
            .unit   = unit,
            .player = player_of(p, unit),
            .killer = killer,
            .cause  = cause.value_or(
                killer != ObjectHitEvent::no_player ? UnitKilledEvent::player_cause : UnitKilledEvent::other),
        };
        p.template subsystem<GameEventBus>().inject(ev, &kill);
        doom(p, unit);
    }

    /* What the authority says is left; a player of ours that comes back to
     * life starts over at a spawn point */
    void take(Proxy& p, UnitVitalityEvent const& vitality)
    {
        auto* status = p.template get<BipedStatus>(vitality.unit);
        if(!status || vitality.origin != MapLoadEvent::Remote)
            return;
        status->body       = vitality.body;
        status->overcharge_decay = vitality.overcharge_decay;
        if(vitality.shield < status->shield)
            status->recent_shield = 1.f;
        bool const was_up = status->shield > 0.f;
        status->shield     = vitality.shield;
        if(was_up != (status->shield > 0.f))
            shield_down(p, vitality.unit, was_up);
        if(vitality.dead && !status->dead)
            last_words(p, vitality.unit, vitality.violent);
        status->dead       = vitality.dead;
        if(vitality.dead || vitality.ping)
        {
            status->blow       = vitality.blow;
            status->blow_flags = vitality.blow_flags;
        }
        if(vitality.ping)
            status->pings++;
        if(vitality.destroyed)
            burst(p, vitality.unit);
        if(vitality.shield < 1.f)
            status->stunned = status->coll ? status->coll->stun_time : default_stun;
        if(auto const* info = p.template get<PlayerInfo>(vitality.unit);
           info && !info->is_remote() && vitality.respawn_at)
        {
            restart(p, vitality.unit, *info, *vitality.respawn_at, vitality.respawn_facing);
            if(vitality.fresh)
                reequip(p, vitality.unit);
        }
        if(vitality.respawn_at && vitality.arrived)
            arrive(p, vitality.unit, *vitality.respawn_at);
    }

    /* Its shield gone, with the coll's depletion effect, or back: the
     * loader turns off and on what the tags tie to it */
    void shield_down(Proxy& p, u64 unit, bool down)
    {
        auto const* status = p.template get<BipedStatus>(unit);
        if(down && status && status->coll && status->coll->shield_depleted_effect.valid())
            if(auto centre = centre_of(p, unit))
            {
                GameEvent        ev{.type = GameEvent::SpawnEffect};
                SpawnEffectEvent play{
                    .effect    = status->coll->shield_depleted_effect,
                    .position  = *centre,
                    .direction = {0.f, 0.f, 1.f},
                };
                p.template subsystem<GameEventBus>().inject(ev, &play);
            }
        GameEvent           ev{.type = GameEvent::ShieldDepleted};
        ShieldDepletedEvent state{.unit = unit, .depleted = down};
        p.template subsystem<GameEventBus>().inject(ev, &state);
    }

    /* The players' respawn effect where it comes back, standing up */
    void arrive(Proxy& p, u64 unit, Vecf3 feet)
    {
        auto const* info = p.template get<PlayerInfo>(unit);
        if(!info || !info->biped.respawn_effect ||
           info->biped.load_generation != p.template subsystem<BlamFiles<halo_version>>().load_generation)
            return;
        GameEvent        ev{.type = GameEvent::SpawnEffect};
        SpawnEffectEvent play{
            .effect    = *info->biped.respawn_effect,
            .position  = feet,
            .direction = {0.f, 0.f, 1.f},
        };
        p.template subsystem<GameEventBus>().inject(ev, &play);
    }

    /* Onto its feet there, facing that way */
    void restart(Proxy& p, u64 player, PlayerInfo const& info, Vecf3 feet, f32 facing)
    {
        GameEvent           ev{.type = GameEvent::PlayerTeleport};
        PlayerTeleportEvent move{
            .entity_id = player,
            .position  = feet + Vecf3{
                                   0.f,
                                   0.f,
                                   info.mode.physics ? info.biped.height / 2 + spawn_lift
                                                     : info.biped.eye_height},
            .rotation = glm::angleAxis(glm::pi<f32>() - facing, Vecf3{0.f, 1.f, 0.f}),
        };
        p.template subsystem<GameEventBus>().inject(ev, &move);
    }

    /* A respawn starts with the map's loadout again, as the loader hands it
     * out to a new player; the server keeps its own count of a client's */
    void reequip(Proxy& p, u64 player)
    {
        if(auto* equip = p.template get<BipedEquipment>(player))
            equip->load_generation = 0;
    }

    /* Back at full health where the game mode put it, or as it was at a
     * checkpoint */
    void revive(Proxy& p, RespawnUnitEvent const& respawn)
    {
        auto* status = p.template get<BipedStatus>(respawn.unit);
        if(!status || (!status->dead && !respawn.restore && !respawn.place))
            return;
        bool const shielded = status->maximum_shield() > 0.f;
        bool const was_down = status->shield <= 0.f;
        status->body        = respawn.restore ? respawn.body : 1.f;
        status->shield      = shielded ? (respawn.restore ? respawn.shield : 1.f) : 0.f;
        if(was_down && status->shield > 0.f)
            shield_down(p, respawn.unit, false);
        status->dead        = status->body <= 0.f;
        status->stunned = 0.f;
        tell(p, respawn.unit, *status, respawn.position, respawn.facing, respawn.arrive,
             !respawn.restore);
        if(auto const* info = p.template get<PlayerInfo>(respawn.unit))
        {
            if(!info->is_remote())
                restart(p, respawn.unit, *info, respawn.position, respawn.facing);
            if(!respawn.restore && (!info->is_remote() || !p.template subsystem<NetworkState>().replica))
                reequip(p, respawn.unit);
        }
        if(respawn.arrive)
            arrive(p, respawn.unit, respawn.position);
    }

    /* After a while unhurt, a shield fills back up */
    void recharge(Proxy& p, f32 dt)
    {
        if(dt <= 0.f)
            return;
        for(auto unit : p.template select<BipedStatus>())
        {
            auto& status = unit.template get<BipedStatus>();
            status.recent_shield = std::max(status.recent_shield - dt / recent_fade, 0.f);
            /* An over shield runs down to a full one */
            if(status.shield > 1.f)
                status.shield = std::max(status.shield - status.overcharge_decay * dt, 1.f);
            if(status.shield <= 1.f)
                status.overcharge_decay = 0.f;
            if(status.dead || !status.coll || status.maximum_shield() <= 0.f ||
               status.shield >= 1.f)
                continue;
            status.stunned -= dt;
            if(status.stunned > 0.f)
                continue;
            f32 const full = status.coll->recharge_time > 0.f
                                 ? status.coll->recharge_time
                                 : default_recharge;
            bool const was_down = status.shield <= 0.f;
            status.shield       = std::min(status.shield + dt / full, 1.f);
            if(was_down && status.shield > 0.f)
                shield_down(p, unit.id(), false);
        }
    }

    /* The unit's vitality, through its object functions, onto its model:
     * what lights the shield hit overlay */
    void export_functions(Proxy& p)
    {
        using object = blam::scn::object;
        using input  = object::export_functions_t::input_t;
        for(auto ent : p.template select<BipedStatus, Model>())
        {
            auto [status, model] = ent.components();
            /* Players have no origin object, only the biped they wear */
            auto const* tag = model.origin_object ? model.origin_object : model.colors_of;
            if(!tag)
                continue;
            auto obj = tag->template data<object>(*m_magic);
            if(!obj.has_value())
                continue;
            auto functions = obj.value()[0].functions.data(*m_magic);
            if(!functions.has_value() || functions.value().empty())
                continue;
            /* An input left unnamed takes what the held weapon exports:
             * a hunter's biped lights its cannon by the gun's charge */
            auto const* equip = p.template get<BipedEquipment>(ent.id());
            auto const* gun   = equip ? equip->held(m_generation) : nullptr;
            std::array<f32, 4> in{};
            for(size_t i = 0; i < in.size(); i++)
                switch(obj.value()[0].export_.inputs[i])
                {
                case input::none:
                    if(gun && gun->exports[i] >= 0.f)
                        in[i] = gun->exports[i];
                    break;
                case input::body_vitality:
                    in[i] = status.body;
                    break;
                case input::shield_vitality:
                    in[i] = status.shield;
                    break;
                case input::recent_shield_damage:
                    in[i] = status.recent_shield;
                    break;
                case input::alive:
                    in[i] = status.dead ? 0.f : 1.f;
                    break;
                default:
                    break;
                }
            std::array<f32, 4> out = in;
            for(size_t i = 0; i < out.size() && i < functions.value().size(); i++)
                out[i] = blam_functions::evaluate(functions.value()[i], in, out);
            model.object_function = out;
        }
    }

    f32 uniform(Vecf2 const& range)
    {
        if(range.y <= range.x)
            return range.x;
        return std::uniform_real_distribution<f32>(range.x, range.y)(m_random);
    }

    /* Of everything a unit has, a vehicle takes the least share at the
     * slower speed and the most at the faster one; slower than that it is
     * only a nudge */
    static constexpr f32   run_down_slow    = 1.f; /* wu/s */
    static constexpr f32   run_down_fast    = 3.f; /* wu/s */
    static constexpr f32   run_down_least   = .35f;
    static constexpr f32   run_down_most    = .7f;
    static constexpr f32   run_down_npc_least = .5f;
    static constexpr f32   run_down_npc_most  = 1.5f;
    static constexpr f32   unit_middle      = .35f; /* wu above the feet */
    static constexpr f32   spawn_lift       = .05f; /* wu */
    static constexpr f32   default_stun     = 1.f;  /* s */
    static constexpr f32   blow_waist       = .35f; /* wu above the feet, without a biped height */
    static constexpr u16   ting_sound       = 43;   /* matg's multiplayer sounds: game_modes::announcer::ting */
    static constexpr f32 recent_fade = .75f; /* seconds a shield hit glows */
    static constexpr f32   default_recharge = 2.f;  /* s */
    static constexpr f32   dead_below       = 1e-4f;
    static constexpr f32   hatch_wait       = .5f;  /* s, at most, for the burst */
    static constexpr f32   hatch_spread     = .3f;  /* wu */

    struct brood_t
    {
        Vecf3          at{};
        Vecf3          burst{};
        blam::tagref_t variant{};
        blam::tagref_t unit{};
        blam::tagref_t weapon{};
        i32            count{0};
        f32            speed{0.f};
        f32            wait{0.f};
        i16            encounter{-1};
        i16            squad{-1};
        u16            team{0};
        u32            like{0};
    };

    std::unique_ptr<TickQueue> m_ticks;
    std::shared_ptr<GameEventBus::queue_type<ObjectHitEvent>>    m_hit_queue;
    std::shared_ptr<GameEventBus::queue_type<AreaDamageEvent>>   m_area_queue;
    std::shared_ptr<GameEventBus::queue_type<UnitVitalityEvent>> m_vitality_queue;
    std::shared_ptr<PhysicsBus::queue_type<Physics::UnitStruck>> m_struck_queue;
    std::vector<ObjectHitEvent>      m_hits;
    std::vector<AreaDamageEvent>     m_areas;
    std::vector<UnitVitalityEvent>   m_vitality;
    std::map<u64, f32>               m_fall_grace; /*!< s until out of the world kills again */
    std::map<u64, f32>               m_corpse_age; /*!< s each dead NPC has lain */
    static constexpr f32             corpse_time           = 45.f;
    static constexpr size_t          max_corpses           = 48;
    static constexpr f32             corpse_clear_distance = 3.f;
    std::map<u64, f32>               m_fall_top;   /*!< Highest point since last on the ground */
    struct launched_t
    {
        f32   left{0.f}; /*!< s it still counts as thrown */
        u32   by{ObjectHitEvent::no_player};
        Vecf3 velocity{};
        bool  primed{false};
    };
    std::map<u64, launched_t>        m_launched;
    static constexpr f32             launch_window = 3.f;
    static constexpr f32             teleport_speed = 50.f; /* wu/s */
    static constexpr f32             halo_gravity  = 9.81f / 3.048f; /* wu/s^2 */
    blam::globals::falling_damage const* m_falling{nullptr};
    static constexpr f32             revive_grace = 1.f;
    std::vector<Physics::UnitStruck> m_struck;
    std::shared_ptr<GameEventBus::queue_type<RespawnUnitEvent>> m_respawn_queue;
    std::vector<RespawnUnitEvent>    m_respawns;
    std::shared_ptr<GameEventBus::queue_type<EquipWeaponEvent>> m_pickup_queue;
    std::vector<EquipWeaponEvent>    m_pickups;
    std::map<u64, f32>               m_dying; /* unit: seconds until destroyed */
    std::vector<u32>                 m_gone;  /* net ids, despawned next frame */
    std::map<u64, f32>               m_object_body; /* breakable objects' body left, by entity */
    std::set<u64>                    m_destroyed;
    u32                              m_generation{0};
    blam::globals::difficulty const* m_difficulty{nullptr};
    std::vector<brood_t>             m_broods;

    blam::map_ptr const*                              m_magic{nullptr};
    std::optional<blam::tag_index_view<halo_version>> m_index;
    std::mt19937                                      m_random{std::random_device{}()};
};

void alloc_damage(compo::EntityContainer& e)
{
    auto& damage   = e.register_subsystem_inplace<DamageSystem>();
    auto& game     = e.subsystem_cast<GameEventBus>();
    damage.m_ticks = std::make_unique<TickQueue>(game);
    damage.m_hit_queue = game.addQueuedEventFunction<ObjectHitEvent>(
        0, [&damage](GameEvent&, ObjectHitEvent* hit) {
            if(hit->origin == MapLoadEvent::Local)
                damage.m_hits.push_back(*hit);
        });
    damage.m_respawn_queue = game.addQueuedEventFunction<RespawnUnitEvent>(
        0, [&damage](GameEvent&, RespawnUnitEvent* respawn) {
            damage.m_respawns.push_back(*respawn);
        });
    damage.m_pickup_queue = game.addQueuedEventFunction<EquipWeaponEvent>(
        0, [&damage](GameEvent&, EquipWeaponEvent* equip) {
            if(equip->mode == EquipWeaponEvent::pick_up)
                damage.m_pickups.push_back(*equip);
        });
    damage.m_area_queue = game.addQueuedEventFunction<AreaDamageEvent>(
        0, [&damage](GameEvent&, AreaDamageEvent* area) {
            damage.m_areas.push_back(*area);
        });
    damage.m_vitality_queue = game.addQueuedEventFunction<UnitVitalityEvent>(
        0, [&damage](GameEvent&, UnitVitalityEvent* vitality) {
            damage.m_vitality.push_back(*vitality);
        });
    damage.m_struck_queue =
        e.subsystem_cast<PhysicsBus>().addQueuedEventFunction<Physics::UnitStruck>(
            0, [&damage](Physics::Event&, Physics::UnitStruck* struck) {
                damage.m_struck.push_back(*struck);
            });
}
