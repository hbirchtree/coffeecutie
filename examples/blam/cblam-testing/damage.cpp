#include "damage.h"

#include "blam_files.h"
#include "components.h"
#include "game_events.h"
#include "network/networking.h"
#include "physics.h"
#include "selected_version.h"
#include "ticks.h"

#include <blam/volta/blam_effects.h>

#include <coffee/components/proxy.h>
#include <coffee/components/restricted_subsystem.h>

#include <map>
#include <random>

using DamageManifest = compo::SubsystemManifest<
    type_list_t<
        BipedStatus,
        const PlayerInfo,
        const PlayerCamera,
        const Model,
        const ObjectPhysics>,
    type_list_t<
        GameEventBus,
        PhysicsBus,
        const NetworkState,
        const BlamFiles<halo_version>>,
    empty_list_t>;

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
        for(auto const& vitality : std::exchange(m_vitality, {}))
            take(p, vitality);

        auto const& files = p.template subsystem<BlamFiles<halo_version>>();
        m_magic           = &files.container.magic;
        m_index.emplace(files.container);
        if(!p.template subsystem<NetworkState>().replica)
        {
            for(auto const& hit : hits)
                if(auto const* jpt = damage_of(hit.damage))
                    hurt(p, hit.target, *jpt, 1.f, hit.part, hit.head);
            for(auto const& area : areas)
                splash(p, area);
            for(auto const& s : struck)
                run_down(p, s);
            respawn(p, dt);
        }
        recharge(p, dt);
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

    /* A jpt! landing on a unit, at `scale` of its full strength */
    void hurt(Proxy& p, u64 unit, jpt_t const& jpt, f32 scale, i16 part, bool head)
    {
        auto* status = p.template get<BipedStatus>(unit);
        if(!status || !status->coll || status->dead || status->invulnerable)
            return;
        auto const& coll   = *status->coll;
        f32         amount = uniform(jpt.upper_bound) * scale;
        f32         shield_mult = 1.f, body_mult = 1.f;
        if(i16 const material = material_of(p, unit, part); material >= 0)
            if(auto materials = coll.materials.data(*m_magic);
               materials.has_value() &&
               static_cast<size_t>(material) < materials.value().size())
            {
                auto const& m = materials.value()[material];
                shield_mult   = m.shield_damage_multiplier;
                body_mult     = m.body_damage_multiplier;
                if(m.material_type < blam::scn::material_type_count &&
                   jpt.material_modifiers[m.material_type] > 0.f)
                    amount *= jpt.material_modifiers[m.material_type];
            }
        bool const headshot =
            head && (jpt.damage_flags & flag_t::can_cause_headshots) != flag_t::none;
        apply(*status, amount, shield_mult, body_mult, jpt.damage_flags, headshot);
        tell(p, unit, *status);
    }

    /* Shield first, then whatever gets past it into the body. Unshielded,
     * a headshot from a weapon that can make one kills */
    void apply(
        BipedStatus& status,
        f32          amount,
        f32          shield_mult,
        f32          body_mult,
        flag_t       flags,
        bool         headshot)
    {
        auto const& coll       = *status.coll;
        f32 const   max_shield = status.maximum_shield();
        f32 const   max_body   = std::max(status.maximum_body(), 1.f);
        shield_mult = shield_mult > 0.f ? shield_mult : 1.f;
        body_mult   = body_mult > 0.f ? body_mult : 1.f;
        bool const shielded = status.shield > 0.f && max_shield > 0.f;
        if(amount <= 0.f)
            return;
        status.stunned = coll.stun_time > 0.f ? coll.stun_time : default_stun;
        if(shielded && (flags & flag_t::skips_shields) == flag_t::none)
        {
            f32 const soak = status.shield * max_shield / shield_mult;
            status.shield  = std::max(status.shield - amount * shield_mult / max_shield, 0.f);
            amount         = std::max(amount - soak, 0.f);
        }
        if(amount <= 0.f || (flags & flag_t::only_hurts_shields) != flag_t::none)
            return;
        status.body = headshot && !shielded
                          ? 0.f
                          : status.body - amount * body_mult / max_body;
        if(status.body < dead_below)
            status.body = 0.f;
        status.dead = status.body <= 0.f;
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
            f32 const d = glm::distance(*centre, area.position);
            if(d > jpt->radius.y)
                continue;
            f32 const inner = std::min(jpt->radius.x, jpt->radius.y);
            f32 const scale =
                flat || d <= inner
                    ? 1.f
                    : 1.f - (d - inner) / std::max(jpt->radius.y - inner, 1e-3f);
            hurt(p, unit.id(), *jpt, scale, -1, false);
        }
    }

    /* A vehicle hurts what it runs into by how fast it came, but never kills
     * a unit in full health outright */
    void run_down(Proxy& p, Physics::UnitStruck const& struck)
    {
        auto* status = p.template get<BipedStatus>(struck.unit);
        if(!status || !status->coll || status->dead || status->invulnerable)
            return;
        f32 const   share = glm::mix(
            run_down_least,
            run_down_most,
            std::clamp(
                (struck.speed - run_down_slow) / (run_down_fast - run_down_slow),
                0.f,
                1.f));
        f32 const total = status->maximum_body() + std::max(status->maximum_shield(), 0.f);
        apply(*status, share * total, 1.f, 1.f, flag_t::none, false);
        tell(p, struck.unit, *status);
    }

    std::optional<Vecf3> centre_of(Proxy& p, u64 unit) const
    {
        if(auto const* info = p.template get<PlayerInfo>(unit))
        {
            auto const* cam = p.template get<PlayerCamera>(unit);
            if(!cam || info->riding.vehicle != 0)
                return std::nullopt;
            return cam->camera.position - Vecf3{0.f, 0.f, info->biped.eye_offset()};
        }
        if(auto const* model = p.template get<Model>(unit))
            return model->position + Vecf3{0.f, 0.f, unit_middle};
        return std::nullopt;
    }

    /* ---- What everyone sees ---- */

    void tell(Proxy& p, u64 unit, BipedStatus const& status)
    {
        auto const*       info = p.template get<PlayerInfo>(unit);
        GameEvent         ev{.type = GameEvent::UnitVitality};
        UnitVitalityEvent vitality{
            .unit   = unit,
            .player = info ? info->player_idx : ObjectHitEvent::no_player,
            .body   = status.body,
            .shield = status.shield,
            .dead   = status.dead,
        };
        p.template subsystem<GameEventBus>().inject(ev, &vitality);
        if(status.dead && info)
            m_respawns.try_emplace(unit, respawn_delay);
    }

    /* What the authority says is left; a player of ours that comes back to
     * life starts over at a spawn point */
    void take(Proxy& p, UnitVitalityEvent const& vitality)
    {
        auto* status = p.template get<BipedStatus>(vitality.unit);
        if(!status || vitality.origin != MapLoadEvent::Remote)
            return;
        bool const revived = status->dead && !vitality.dead;
        status->body       = vitality.body;
        status->shield     = vitality.shield;
        status->dead       = vitality.dead;
        if(vitality.shield < 1.f)
            status->stunned = status->coll ? status->coll->stun_time : default_stun;
        if(auto const* info = p.template get<PlayerInfo>(vitality.unit);
           revived && info && !info->is_remote())
            restart(p, vitality.unit, *info);
    }

    void restart(Proxy& p, u64 player, PlayerInfo const& info)
    {
        auto const& files    = p.template subsystem<BlamFiles<halo_version>>();
        auto const* scenario = files.container.scenario().value_or(nullptr);
        if(!scenario)
            return;
        auto spots = scenario->player_start.locations.data(*m_magic);
        if(!spots.has_value() || spots.value().empty())
            return;
        auto const& spot = spots.value()[std::uniform_int_distribution<size_t>(
            0, spots.value().size() - 1)(m_random)];
        GameEvent           ev{.type = GameEvent::PlayerTeleport};
        PlayerTeleportEvent move{
            .entity_id = player,
            .position  = spot.pos + Vecf3{
                                       0.f,
                                       0.f,
                                       info.mode.physics ? info.biped.height / 2 + spawn_lift
                                                         : info.biped.eye_height},
            .rotation = glm::angleAxis(
                glm::pi<f32>() - spot.rot, Vecf3{0.f, 1.f, 0.f}),
        };
        p.template subsystem<GameEventBus>().inject(ev, &move);
    }

    /* Players dead long enough come back at full health */
    void respawn(Proxy& p, f32 dt)
    {
        for(auto it = m_respawns.begin(); it != m_respawns.end();)
        {
            auto* status = p.template get<BipedStatus>(it->first);
            if(!status || !status->dead)
            {
                it = m_respawns.erase(it);
                continue;
            }
            if((it->second -= dt) > 0.f)
            {
                ++it;
                continue;
            }
            u64 const player = it->first;
            it               = m_respawns.erase(it);
            status->body     = 1.f;
            status->shield   = status->maximum_shield() > 0.f ? 1.f : 0.f;
            status->dead     = false;
            status->stunned  = 0.f;
            tell(p, player, *status);
            if(auto const* info = p.template get<PlayerInfo>(player);
               info && !info->is_remote())
                restart(p, player, *info);
        }
    }

    /* After a while unhurt, a shield fills back up */
    void recharge(Proxy& p, f32 dt)
    {
        if(dt <= 0.f)
            return;
        for(auto unit : p.template select<BipedStatus>())
        {
            auto& status = unit.template get<BipedStatus>();
            if(status.dead || !status.coll || status.maximum_shield() <= 0.f ||
               status.shield >= 1.f)
                continue;
            status.stunned -= dt;
            if(status.stunned > 0.f)
                continue;
            f32 const full = status.coll->recharge_time > 0.f
                                 ? status.coll->recharge_time
                                 : default_recharge;
            status.shield = std::min(status.shield + dt / full, 1.f);
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
    static constexpr f32   unit_middle      = .35f; /* wu above the feet */
    static constexpr f32   respawn_delay    = 3.f;  /* s */
    static constexpr f32   spawn_lift       = .05f; /* wu */
    static constexpr f32   default_stun     = 1.f;  /* s */
    static constexpr f32   default_recharge = 2.f;  /* s */
    static constexpr f32   dead_below       = 1e-4f;

    std::unique_ptr<TickQueue> m_ticks;
    std::shared_ptr<GameEventBus::queue_type<ObjectHitEvent>>    m_hit_queue;
    std::shared_ptr<GameEventBus::queue_type<AreaDamageEvent>>   m_area_queue;
    std::shared_ptr<GameEventBus::queue_type<UnitVitalityEvent>> m_vitality_queue;
    std::shared_ptr<PhysicsBus::queue_type<Physics::UnitStruck>> m_struck_queue;
    std::vector<ObjectHitEvent>      m_hits;
    std::vector<AreaDamageEvent>     m_areas;
    std::vector<UnitVitalityEvent>   m_vitality;
    std::vector<Physics::UnitStruck> m_struck;
    std::map<u64, f32>               m_respawns; /*!< Seconds left */

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
