#pragma once

/* Shots and hits between peers. Networking hands it what arrives and sends
 * what it queues; everything that decides what a shot or a hit means here
 * stays in this class. */

#include "components.h"
#include "game_events.h"
#include "network/network_events.h"
#include "selected_version.h"
#include "ticks.h"

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
    template<typename Proxy>
    void collect(Proxy& p, bool server, bool joined)
    {
        m_fire_queue->poll();
        m_hit_queue->poll();
        m_vitality_queue->poll();
        for(auto const& vitality : std::exchange(m_local_vitality, {}))
            if(server)
                if(auto wire = vitality_of(p, vitality.unit))
                {
                    if(vitality.respawn_at)
                    {
                        wire->respawned = vitality.arrived ? 3 : 1;
                        wire->respawn   = Vecf4(*vitality.respawn_at, vitality.respawn_facing);
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
                        .flags      = shot.thrown ? WeaponFire::thrown : WeaponFire::none,
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
                .projectile     = hit.projectile.tag_id,
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
            fire(p, from.player, wire, rewind_for(p, from, wire.view_tick));
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
        if(projectile == index.end() || !(*projectile).valid() ||
           !(*projectile).matches(blam::tag_class_t::proj))
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
        auto const proj =
            (*projectile).template data<blam::scn::projectile>(m_map->magic);
        GameEvent      ev{.type = GameEvent::ObjectHit};
        ObjectHitEvent hit{
            .origin     = MapLoadEvent::Remote,
            .projectile = (*projectile).as_ref(),
            .damage     = proj.has_value()
                              ? blam::tagref_t(proj.value()->impact_damage)
                              : blam::tagref_t{},
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
            .shield        = status->shield,
            .dead          = status->dead,
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
            .dead   = wire.dead != 0,
        };
        if(wire.respawned)
        {
            vitality.respawn_at     = Vecf3(wire.respawn);
            vitality.respawn_facing = wire.respawn.w;
            vitality.arrived        = (wire.respawned & 2) != 0;
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

    /* Someone else's shot, fired here by `shooter` */
    template<typename Proxy>
    bool fire(
        Proxy&             p,
        u64                shooter,
        WeaponFire const&  wire,
        std::optional<u64> view_tick = {})
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
        u64 vehicle = 0;
        if(wire.vehicle != 0)
            for(auto object : p.template select<NetworkInfo>())
                if(object.template get<NetworkInfo>().instance_id == wire.vehicle)
                {
                    vehicle = object.id();
                    break;
                }
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
            .position   = Vecf3(wire.position),
            .direction  = glm::normalize(direction),
            .view_tick  = view_tick,
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

    std::shared_ptr<GameEventBus::queue_type<WeaponFireEvent>> m_fire_queue;
    std::shared_ptr<GameEventBus::queue_type<ObjectHitEvent>>  m_hit_queue;
    std::shared_ptr<GameEventBus::queue_type<UnitVitalityEvent>> m_vitality_queue;
    std::vector<UnitVitalityEvent>                               m_local_vitality;
    std::vector<outgoing_t<UnitVitality>>                        m_vitality;
    std::map<u32, UnitVitality>                                  m_unplaced;
    std::vector<WeaponFireEvent>                               m_local_shots;
    std::vector<ObjectHitEvent>                                m_local_hits;
    std::vector<outgoing_t<WeaponFire>>                        m_shots;
    std::vector<outgoing_t<ObjectHit>>                         m_hits;
};

} // namespace net_combat
