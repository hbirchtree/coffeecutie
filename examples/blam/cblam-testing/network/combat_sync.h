#pragma once

/* Shots and hits between peers. Networking hands it what arrives and sends
 * what it queues; everything that decides what a shot or a hit means here
 * stays in this class. */

#include "components.h"
#include "game_events.h"
#include "network/network_events.h"
#include "selected_version.h"
#include "ticks.h"

#include <blam/volta/blam_globals.h>
#include <blam/volta/blam_tag_index.h>
#include <nlohmann/json.hpp>

#include "../tag_json.h"

#include <chrono>
#include <functional>
#include <map>
#include <optional>
#include <vector>

namespace net_combat {

using libc_types::f32;
using libc_types::u32;
using libc_types::u64;
using net_messages::ObjectHit;
using net_messages::UnitVitality;
using net_messages::WeaponFire;

/*! Who an outgoing message is for */
struct route_t
{
    enum to_t
    {
        server,  /*!< From a client */
        clients, /*!< From the server, every verified client */
    } to{server};

    std::optional<u32> except{}; /*!< A connection left out, e.g. the sender */
};

template<typename T>
struct outgoing_t
{
    T       message;
    route_t route;
};

/*! A client as the server knows it */
struct sender_t
{
    u32 connection{0};
    u64 player{0}; /*!< Its player entity here */
    u32 player_idx{0};
    u32 ping_ms{0};
};

class CombatSync
{
  public:
    using journal_t = std::function<void(std::string_view, nlohmann::json)>;

    /* How far from its eye a client may fire, or from the vehicle it rides */
    static constexpr f32 max_shot_offset  = 4.f;
    static constexpr f32 max_mount_offset = 8.f;
    /* How far back a client's shot may look: its ping and some slack */
    static constexpr auto rewind_slack = std::chrono::milliseconds(250);
    static constexpr auto max_rewind   = std::chrono::milliseconds(1000);
    /* A client's shots against its trigger's rate: twice it over time (a
     * speed powerup, jitter), in bursts of up to a second's worth */
    static constexpr f32 rate_slack    = 2.f;
    static constexpr f32 burst_seconds = 1.f;
    static constexpr f32 min_burst     = 4.f;
    static constexpr f32 default_rate  = 10.f; /* A trigger with none: as fast as one clicks */

    CombatSync(GameEventBus& bus, journal_t journal)
        : m_bus(bus)
        , m_journal(std::move(journal))
        , m_fire_queue(bus.addQueuedEventFunction<WeaponFireEvent>(
              0,
              [this](GameEvent&, WeaponFireEvent* shot) {
                  if(shot->origin == MapLoadEvent::Local)
                      m_local_shots.push_back(*shot);
              }))
        , m_hit_queue(bus.addQueuedEventFunction<ObjectHitEvent>(
              0,
              [this](GameEvent&, ObjectHitEvent* hit) {
                  if(hit->origin == MapLoadEvent::Local)
                      m_local_hits.push_back(*hit);
              }))
        , m_vitality_queue(bus.addQueuedEventFunction<UnitVitalityEvent>(
              0,
              [this](GameEvent&, UnitVitalityEvent* vitality) {
                  if(vitality->origin == MapLoadEvent::Local)
                      m_local_vitality.push_back(*vitality);
              }))
    {
    }

    void set_map(blam::map_container<halo_version> const* map)
    {
        m_map = map;
    }

    /*! The newest server tick others are shown at here */
    void viewing(u64 tick)
    {
        m_view_tick = std::max(m_view_tick, tick);
    }

    /*! Left the server */
    void reset()
    {
        m_view_tick = 0;
    }

    /*! This peer's shots and the hits it decided, as messages: a client's
     *  shots to the server, the server's shots and hits to its clients */
    template<typename Proxy, typename ShownAt>
    void collect(Proxy& p, bool server, bool joined, ShownAt&& shown_at)
    {
        m_fire_queue->poll();
        m_hit_queue->poll();
        m_vitality_queue->poll();
        for(auto const& vitality : std::exchange(m_local_vitality, {}))
            if(server)
                if(auto wire = vitality_of(p, vitality.unit))
                {
                    if(vitality.violent)
                        wire->dead |= 2u;
                    if(vitality.destroyed)
                        wire->dead |= 4u;
                    if(vitality.respawn_at)
                    {
                        wire->respawned = 1u | (vitality.arrived ? 2u : 0u) | (vitality.fresh ? 4u : 0u);
                        wire->respawn   = Vecf4(*vitality.respawn_at, vitality.respawn_facing);
                    } else if(vitality.dead || vitality.ping)
                    {
                        wire->respawn    = Vecf4(vitality.blow, 0.f);
                        wire->blow_flags = vitality.blow_flags | (vitality.ping ? ping_flag : 0u);
                    }
                    m_vitality.push_back({.message = *wire, .route = {route_t::clients}});
                }
        /* Units the server named before they were spawned here */
        for(auto const& [_, wire] : std::exchange(m_unplaced, {}))
            client_receive(p, wire);
        for(auto const& shot : std::exchange(m_local_shots, {}))
        {
            if(!server && !joined)
                continue;
            u32 vehicle = 0;
            if(auto const* net = shot.vehicle ? p.template get<NetworkInfo>(shot.vehicle)
                                              : nullptr)
                vehicle = net->instance_id;
            u32 target = 0, target_player = ObjectHitEvent::no_player;
            if(auto const* info = shot.target ? p.template get<PlayerInfo>(shot.target) : nullptr)
                target_player = info->player_idx;
            else if(auto const* net = shot.target ? p.template get<NetworkInfo>(shot.target) : nullptr)
                target = net->instance_id;
            /* An actor's shot goes out under its net id, from the server alone */
            u32 shooter = 0;
            if(shot.player_idx == ObjectHitEvent::no_player)
            {
                auto const* net = p.template get<NetworkInfo>(shot.shooter);
                if(!server || !net || net->instance_id == 0)
                    continue;
                shooter = net->instance_id;
            }
            m_shots.push_back({
                .message =
                    {
                        /* Others are shown where the latest sync put them */
                        .view_tick  = server ? 0 : m_view_tick,
                        .position   = Vecf4(shot.position, 1.f),
                        .direction  = Vecf4(shot.direction, 0.f),
                        .weapon     = shot.weapon.tag_id,
                        .player_idx = shot.player_idx,
                        .vehicle    = vehicle,
                        .seed       = shot.seed,
                        .trigger    = shot.trigger,
                        .flags      = static_cast<WeaponFire::flags_t>(
                            (shot.thrown  ? WeaponFire::thrown
                             : shot.melee ? WeaponFire::melee
                                          : WeaponFire::none) |
                            (shot.windup ? WeaponFire::windup : WeaponFire::none) |
                            (shot.zoomed ? WeaponFire::zoomed : WeaponFire::none)),
                        .target        = target,
                        .target_player = target_player,
                        .shooter       = shooter,
                        .error         = shot.error,
                        .target_tick   = server || target == 0 ? 0 : shown_at(target),
                    },
                .route = {server ? route_t::clients : route_t::server},
            });
        }
        for(auto const& hit : std::exchange(m_local_hits, {}))
        {
            if(!server)
                continue;
            auto const*     net = p.template get<NetworkInfo>(hit.target);
            ObjectHit const wire{
                .point          = Vecf4(hit.point, 1.f),
                .normal         = Vecf4(hit.normal, 0.f),
                .direction      = Vecf4(hit.direction, 0.f),
                .projectile     = hit.projectile.valid() ? hit.projectile.tag_id
                                                         : hit.damage.tag_id,
                .target         = net ? net->instance_id : 0,
                .target_player  = hit.target_player,
                .shooter_player = hit.shooter_player,
                .part           = hit.part,
                .head           = hit.head,
                .hitscan        = hit.hitscan,
            };
            journal_hit(wire);
            if(wire.target != 0 ||
               wire.target_player != ObjectHitEvent::no_player)
                m_hits.push_back({.message = wire, .route = {route_t::clients}});
        }
    }

    /*! A client's shot on the server: only ever its own player's, from
     *  about where it is or the vehicle it rides; fired here and passed on */
    template<typename Proxy>
    void server_receive(Proxy& p, sender_t const& from, WeaponFire wire)
    {
        wire.player_idx  = from.player_idx;
        auto const* info = p.template get<PlayerInfo>(from.player);
        auto const* cam  = p.template get<PlayerCamera>(from.player);
        if(!info || !cam)
            return;
        Vecf3 anchor = cam->camera.position;
        f32   reach  = max_shot_offset;
        if(wire.vehicle != 0)
        {
            u64 const   riding = info->riding.vehicle;
            auto const* net = riding ? p.template get<NetworkInfo>(riding) : nullptr;
            auto const* model = riding ? p.template get<Model>(riding) : nullptr;
            reach = net && model && net->instance_id == wire.vehicle
                        ? max_mount_offset
                        : -1.f;
            if(model)
                anchor = model->position;
        }
        bool const fired =
            glm::distance(Vecf3(wire.position), anchor) <= reach &&
            may_fire(p, from.player, wire) &&
            fire(p, from.player, wire, rewind_for(p, from, wire.view_tick),
                 rewind_for(p, from, wire.target_tick));
        m_journal(
            fired ? "net_weapon_fire" : "net_weapon_fire_reject",
            {{"player_idx", wire.player_idx}, {"weapon", wire.weapon}});
        if(fired)
            m_shots.push_back(
                {.message = wire,
                 .route   = {route_t::clients, from.connection}});
    }

    /*! Another player's shot, relayed by the server */
    template<typename Proxy>
    void client_receive(
        Proxy& p, WeaponFire const& wire, std::optional<u32> self_idx)
    {
        if(wire.player_idx == self_idx)
            return;
        /* An actor's: whichever unit the server named */
        if(wire.player_idx == ObjectHitEvent::no_player)
        {
            for(auto object : p.template select<NetworkInfo>())
                if(object.template get<NetworkInfo>().instance_id == wire.shooter &&
                   wire.shooter != 0)
                {
                    if(fire(p, object.id(), wire))
                        m_journal("net_actor_fire", {{"shooter", wire.shooter}, {"weapon", wire.weapon}});
                    return;
                }
            return;
        }
        for(auto entity : p.template select<PlayerInfo>())
        {
            auto const& info = entity.template get<PlayerInfo>();
            if(!info.is_remote() || info.player_idx != wire.player_idx)
                continue;
            if(fire(p, entity.id(), wire))
                m_journal(
                    "net_weapon_fire",
                    {{"player_idx", wire.player_idx}, {"weapon", wire.weapon}});
            return;
        }
    }

    /*! A hit the server decided, on whatever it names here */
    template<typename Proxy>
    void client_receive(Proxy& p, ObjectHit const& wire)
    {
        if(!m_map)
            return;
        blam::tag_index_view<halo_version> index(*m_map);
        auto projectile = index.find(wire.projectile);
        if(projectile == index.end() || !(*projectile).valid())
            return;
        /* A strike names its jpt! */
        bool const strike = (*projectile).matches(blam::tag_class_t::jpt);
        if(!strike && !(*projectile).matches(blam::tag_class_t::proj))
            return;
        u64 target = 0, shooter = 0;
        for(auto entity : p.template select<PlayerInfo>())
        {
            auto const idx = entity.template get<PlayerInfo>().player_idx;
            if(idx == wire.target_player)
                target = entity.id();
            if(idx == wire.shooter_player)
                shooter = entity.id();
        }
        if(target == 0 && wire.target != 0)
            for(auto object : p.template select<NetworkInfo>())
                if(object.template get<NetworkInfo>().instance_id == wire.target)
                {
                    target = object.id();
                    break;
                }
        if(target == 0)
            return;
        blam::tagref_t damage = strike ? (*projectile).as_ref() : blam::tagref_t{};
        if(!strike)
            if(auto proj = (*projectile).template data<blam::scn::projectile>(m_map->magic);
               proj.has_value())
                damage = proj.value()->impact_damage;
        GameEvent      ev{.type = GameEvent::ObjectHit};
        ObjectHitEvent hit{
            .origin     = MapLoadEvent::Remote,
            .projectile = strike ? blam::tagref_t{} : (*projectile).as_ref(),
            .damage     = damage,
            .target         = target,
            .shooter        = shooter,
            .target_player  = wire.target_player,
            .shooter_player = wire.shooter_player,
            .point          = Vecf3(wire.point),
            .normal         = Vecf3(wire.normal),
            .direction      = Vecf3(wire.direction),
            .part           = wire.part,
            .head           = wire.head != 0,
            .hitscan        = wire.hitscan != 0,
        };
        journal_hit(wire);
        m_bus.inject(ev, &hit);
    }

    /*! A unit's body and shield as a message, if it can be named on the
     *  wire */
    template<typename Proxy>
    std::optional<UnitVitality> vitality_of(Proxy& p, u64 unit) const
    {
        auto const* status = p.template get<BipedStatus>(unit);
        if(!status)
            return std::nullopt;
        UnitVitality wire{
            .target_player = ObjectHitEvent::no_player,
            .body          = status->body,
            .shield           = status->shield,
            .dead             = status->dead,
            .overcharge_decay = status->overcharge_decay,
        };
        if(auto const* info = p.template get<PlayerInfo>(unit))
            wire.target_player = info->player_idx;
        else if(auto const* net = p.template get<NetworkInfo>(unit))
            wire.target = net->instance_id;
        if(wire.target == 0 && wire.target_player == ObjectHitEvent::no_player)
            return std::nullopt;
        return wire;
    }

    /*! What the server says a unit has left */
    template<typename Proxy>
    void client_receive(Proxy& p, UnitVitality const& wire)
    {
        u64 unit = 0;
        if(wire.target_player != ObjectHitEvent::no_player)
        {
            for(auto entity : p.template select<PlayerInfo>())
                if(entity.template get<PlayerInfo>().player_idx == wire.target_player)
                    unit = entity.id();
        } else
            for(auto object : p.template select<NetworkInfo>())
                if(object.template get<NetworkInfo>().instance_id == wire.target)
                {
                    unit = object.id();
                    break;
                }
        if(unit == 0)
        {
            if(wire.target != 0)
                m_unplaced[wire.target] = wire;
            return;
        }
        GameEvent         ev{.type = GameEvent::UnitVitality};
        UnitVitalityEvent vitality{
            .origin = MapLoadEvent::Remote,
            .unit   = unit,
            .player = wire.target_player,
            .body   = wire.body,
            .shield = wire.shield,
            .dead    = (wire.dead & 1) != 0,
            .violent = (wire.dead & 2) != 0,
            .overcharge_decay = wire.overcharge_decay,
            .destroyed        = (wire.dead & 4) != 0,
        };
        if(!wire.respawned && (vitality.dead || (wire.blow_flags & ping_flag)))
        {
            vitality.blow       = Vecf3(wire.respawn);
            vitality.blow_flags = static_cast<u8>(wire.blow_flags);
            vitality.ping       = !vitality.dead && (wire.blow_flags & ping_flag);
        }
        if(wire.respawned)
        {
            vitality.respawn_at     = Vecf3(wire.respawn);
            vitality.respawn_facing = wire.respawn.w;
            vitality.arrived        = (wire.respawned & 2) != 0;
            vitality.fresh          = (wire.respawned & 4) != 0;
        }
        m_bus.inject(ev, &vitality);
    }

    std::vector<outgoing_t<UnitVitality>> take_vitality()
    {
        return std::exchange(m_vitality, {});
    }

    std::vector<outgoing_t<WeaponFire>> take_shots()
    {
        return std::exchange(m_shots, {});
    }

    std::vector<outgoing_t<ObjectHit>> take_hits()
    {
        return std::exchange(m_hits, {});
    }

  private:
    /* The tick a client's shot is traced back to: what it saw, but no
     * further back than its ping and some slack */
    template<typename Proxy>
    std::optional<u64> rewind_for(Proxy& p, sender_t const& from, u64 view_tick)
    {
        if(view_tick == 0)
            return std::nullopt;
        auto const back = std::min(
            rewind_slack + std::chrono::milliseconds(from.ping_ms), max_rewind);
        u64 const now = p.template subsystem<TickClock>().tick();
        u64 const ticks =
            static_cast<u64>(back.count()) * TickClock::rate / 1000 + 1;
        return std::clamp(view_tick, now > ticks ? now - ticks : 0, now);
    }

    /* A client's shot from a living body, with a weapon it carries, no
     * faster than the weapon fires */
    template<typename Proxy>
    bool may_fire(Proxy& p, u64 player, WeaponFire const& wire)
    {
        if(auto const* status = p.template get<BipedStatus>(player); status && status->dead)
            return false;
        bool const thrown = wire.flags & WeaponFire::thrown;
        bool const melee  = wire.flags & (WeaponFire::melee | WeaponFire::windup);
        if(!thrown && wire.vehicle == 0)
        {
            auto const* equip   = p.template get<BipedEquipment>(player);
            auto const  carries = [&wire](auto const& carried) {
                return carried.tag && carried.tag->tag_id == wire.weapon;
            };
            if(!equip ||
               !(carries(equip->weapons[0]) || carries(equip->weapons[1]) || carries(equip->objective)))
                return false;
        }
        if(thrown)
            return spend_grenade(p, player, wire.weapon);
        if(melee)
            return true;
        f32 const rate     = rate_of(wire);
        f32 const capacity = std::max(rate * burst_seconds, min_burst);
        auto const now     = std::chrono::steady_clock::now();
        auto [it, fresh]   = m_fire_budget.try_emplace(wire.player_idx, fire_budget_t{capacity, now});
        auto& budget       = it->second;
        f32 const dt       = std::chrono::duration<f32>(now - budget.at).count();
        budget.tokens      = std::min(capacity, budget.tokens + dt * rate * rate_slack);
        budget.at          = now;
        if(budget.tokens < 1.f)
            return false;
        budget.tokens -= 1.f;
        return true;
    }

    /* The server counts a client's grenades: one of the kind that throws
     * `projectile`, if it has one */
    template<typename Proxy>
    bool spend_grenade(Proxy& p, u64 player, blam::tag_index<> projectile)
    {
        auto* equip = p.template get<BipedEquipment>(player);
        if(!equip || !m_map)
            return false;
        blam::tag_index_view<halo_version> index(*m_map);
        auto globals_tag = index.tag_of("globals\\globals");
        if(!globals_tag.has_value())
            return false;
        auto globals = (*globals_tag)->template data<blam::globals::globals>(m_map->magic);
        if(!globals.has_value())
            return false;
        auto grenades = globals.value()->grenades.data(m_map->magic);
        if(!grenades.has_value())
            return false;
        for(auto const& grenade : grenades.value())
            if(grenade.projectile.tag_id == projectile)
                for(auto& kind : equip->equipment)
                    if(kind.tag && kind.tag->tag_id == grenade.equipment.tag_id && kind.left > 0)
                    {
                        kind.left--;
                        return true;
                    }
        return false;
    }

    /* Shots a second, at the trigger's fastest */
    f32 rate_of(WeaponFire const& wire) const
    {
        if(!m_map)
            return default_rate;
        blam::tag_index_view<halo_version> index(*m_map);
        auto tag = index.find(wire.weapon);
        if(tag == index.end() || !(*tag).valid() || !(*tag).matches(blam::tag_class_t::weap))
            return default_rate;
        auto weapon = (*tag).template data<blam::scn::weapon>(m_map->magic);
        if(!weapon.has_value())
            return default_rate;
        auto triggers = weapon.value()[0].triggers.data(m_map->magic);
        if(!triggers.has_value() || wire.trigger >= triggers.value().size())
            return default_rate;
        auto const& rates = triggers.value()[wire.trigger].maximum_rate_of_fire;
        f32 const   rate  = std::max(rates.x, rates.y);
        return rate > 0.f ? rate : default_rate;
    }

    /* Someone else's shot, fired here by `shooter` */
    template<typename Proxy>
    bool fire(
        Proxy&             p,
        u64                shooter,
        WeaponFire const&  wire,
        std::optional<u64> view_tick   = {},
        std::optional<u64> target_tick = {})
    {
        if(!m_map)
            return false;
        blam::tag_index_view<halo_version> index(*m_map);
        auto        weapon    = index.find(wire.weapon);
        Vecf3 const direction = Vecf3(wire.direction);
        bool const  thrown    = wire.flags & WeaponFire::thrown;
        if(weapon == index.end() || !(*weapon).valid() ||
           !(*weapon).matches(thrown ? blam::tag_class_t::proj : blam::tag_class_t::weap) ||
           !std::isfinite(glm::dot(direction, Vecf3(wire.position))) ||
           glm::length(direction) < .5f)
            return false;
        u64 vehicle = 0, target = 0;
        if(wire.vehicle != 0 || wire.target != 0)
            for(auto object : p.template select<NetworkInfo>())
            {
                u32 const id = object.template get<NetworkInfo>().instance_id;
                if(wire.vehicle != 0 && id == wire.vehicle)
                    vehicle = object.id();
                if(wire.target != 0 && id == wire.target)
                    target = object.id();
            }
        if(wire.target_player != ObjectHitEvent::no_player)
            for(auto entity : p.template select<PlayerInfo>())
                if(entity.template get<PlayerInfo>().player_idx == wire.target_player)
                    target = entity.id();
        GameEvent       ev{.type = GameEvent::WeaponFire};
        WeaponFireEvent shot{
            .origin     = MapLoadEvent::Remote,
            .weapon     = (*weapon).as_ref(),
            .shooter    = shooter,
            .vehicle    = vehicle,
            .player_idx = wire.player_idx,
            .seed       = wire.seed,
            .trigger    = wire.trigger,
            .thrown     = thrown,
            .melee      = (wire.flags & WeaponFire::melee) != 0,
            .windup     = (wire.flags & WeaponFire::windup) != 0,
            .zoomed     = (wire.flags & WeaponFire::zoomed) != 0,
            .error      = std::clamp(wire.error, 0.f, 1.f),
            .position   = Vecf3(wire.position),
            .direction  = glm::normalize(direction),
            .target     = target,
            .view_tick  = view_tick,
            .target_tick = target != 0 ? target_tick : std::nullopt,
        };
        m_bus.inject(ev, &shot);
        return true;
    }

    void journal_hit(ObjectHit const& hit)
    {
        m_journal(
            "net_object_hit",
            {{"target", hit.target},
             {"target_player", hit.target_player},
             {"shooter_player", hit.shooter_player},
             {"part", hit.part},
             {"head", hit.head != 0},
             {"hitscan", hit.hitscan != 0}});
    }

    GameEventBus& m_bus;
    journal_t     m_journal;
    blam::map_container<halo_version> const* m_map{nullptr};
    u64 m_view_tick{0};

    struct fire_budget_t
    {
        f32                                   tokens;
        std::chrono::steady_clock::time_point at;
    };
    std::map<u32, fire_budget_t> m_fire_budget; /*!< Per client player */

    std::shared_ptr<GameEventBus::queue_type<WeaponFireEvent>> m_fire_queue;
    std::shared_ptr<GameEventBus::queue_type<ObjectHitEvent>>  m_hit_queue;
    std::shared_ptr<GameEventBus::queue_type<UnitVitalityEvent>> m_vitality_queue;
    /*! A living unit's vitality that carries a blow it flinched from */
    static constexpr u32 ping_flag = 0x100;
    std::vector<UnitVitalityEvent>                               m_local_vitality;
    std::vector<outgoing_t<UnitVitality>>                        m_vitality;
    std::map<u32, UnitVitality>                                  m_unplaced;
    std::vector<WeaponFireEvent>                               m_local_shots;
    std::vector<ObjectHitEvent>                                m_local_hits;
    std::vector<outgoing_t<WeaponFire>>                        m_shots;
    std::vector<outgoing_t<ObjectHit>>                         m_hits;
};

} // namespace net_combat
