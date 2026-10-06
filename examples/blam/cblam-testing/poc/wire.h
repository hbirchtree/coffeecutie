#pragma once

/* PoC wire messages: one type byte, then a trivially copyable struct, then
 * (for Snapshot) an array of entries. Same-process, same-ABI only. */

#include "events.h"

#include <cstring>
#include <optional>
#include <span>
#include <type_traits>

namespace poc::wire {

enum class Type : std::uint8_t
{
    hello = 1,
    welcome,
    pose,
    snapshot,
    fire_hitscan,
    shot_result,
    fire_projectile,
    projectile_spawn,
    projectile_impact,
    damage,
    input,
};

struct Hello
{
    static constexpr Type type = Type::hello;
    BipedPose             spawn;
    View                  view;
};

struct Welcome
{
    static constexpr Type type = Type::welcome;
    biped_id              you;
    tick_t                tick;
};

/* Client-authoritative movement, unreliable, every tick. The view is
 * what the client says it ended up looking at after input `seq`; the
 * server only uses it to resync after losing input. */
struct Pose
{
    static constexpr Type type = Type::pose;
    BipedPose             pose;
    View                  view;
    std::uint32_t         seq;
};

/* Look input, unreliable, every tick. Each message repeats the latest few
 * entries so a lost packet costs nothing unless that many are lost in a
 * row. */
struct InputHeader
{
    static constexpr Type type = Type::input;
    std::uint8_t          count;
};

struct InputEntry
{
    std::uint32_t seq;
    LookInput     look;
    LookInput     pre_fire;
    bool          fire;
    WeaponKind    weapon;
};

struct SnapshotHeader
{
    static constexpr Type type = Type::snapshot;
    tick_t                tick;
    std::uint16_t         count;
};

struct SnapshotEntry
{
    biped_id  id;
    BipedPose pose;
    float     body;
    float     shield;
};

struct FireHitscan
{
    static constexpr Type type = Type::fire_hitscan;
    std::uint32_t         shot;
    std::uint32_t         seq; /* input tick that pulled the trigger */
    WeaponKind            weapon;
    float                 view_tick; /* server tick the shooter was seeing */
    vec3                  origin;
    vec3                  direction;
    RayClaim              claim;
    bool                  claim_head;
};

enum class Verdict : std::uint8_t
{
    hit,
    miss,               /* nothing to claim, server agrees */
    server_miss,        /* claim dropped, server's own trace missed */
    claim_off_ray,      /* claimed point too far from the ray */
    claim_occluded,     /* terrain between shooter and claimed point */
    claim_target_gone,  /* target dead/absent at the view tick */
    rewind_too_far,     /* view tick older than max rewind */
    shooter_dead,
    no_trigger, /* the input for that tick never pulled the trigger */
};

struct ShotResult
{
    static constexpr Type type = Type::shot_result;
    std::uint32_t         shot;
    Verdict               verdict;
    biped_id              target;
    bool                  head;
    float                 rewind_ticks;   /* how far back the server looked */
    float                 claim_distance; /* claim vs. server ray, wu */
    vec3                  server_point;
};

struct FireProjectile
{
    static constexpr Type type = Type::fire_projectile;
    std::uint32_t         client_ref;
    std::uint32_t         seq;
    WeaponKind            weapon;
    float                 present_tick; /* shooter's estimate of server now */
    float                 view_tick;    /* what the shooter was seeing */
    vec3                  origin;
    vec3                  velocity;
};

struct ProjectileSpawn
{
    static constexpr Type type = Type::projectile_spawn;
    proj_id               id;
    biped_id              owner;
    std::uint32_t         client_ref;
    WeaponKind            weapon;
    tick_t                tick;    /* server tick it entered the world */
    float                 catchup; /* ticks the server advanced it at once */
    vec3                  origin;
    vec3                  velocity;
};

struct ProjectileImpact
{
    static constexpr Type type = Type::projectile_impact;
    proj_id               id;
    tick_t                tick;
    vec3                  point;
    biped_id              target;
    bool                  head;
};

struct Damage
{
    static constexpr Type type = Type::damage;
    biped_id              target;
    biped_id              source;
    WeaponKind            weapon;
    bool                  head;
    bool                  splash;
    bool                  killed;
    float                 amount;
    float                 body;
    float                 shield;
};

template<typename T>
std::vector<std::uint8_t> encode(T const& msg)
{
    static_assert(std::is_trivially_copyable_v<T>);
    std::vector<std::uint8_t> out(1 + sizeof(T));
    out[0] = static_cast<std::uint8_t>(T::type);
    std::memcpy(out.data() + 1, &msg, sizeof(T));
    return out;
}

template<typename Header, typename Entry>
std::vector<std::uint8_t> encode_array(
    Header const& hdr, std::span<Entry const> entries)
{
    auto out = encode(hdr);
    auto at  = out.size();
    out.resize(at + entries.size_bytes());
    std::memcpy(out.data() + at, entries.data(), entries.size_bytes());
    return out;
}

inline std::optional<Type> type_of(std::span<std::uint8_t const> bytes)
{
    if(bytes.empty())
        return std::nullopt;
    return static_cast<Type>(bytes[0]);
}

template<typename T>
std::optional<T> decode(std::span<std::uint8_t const> bytes)
{
    if(bytes.size() < 1 + sizeof(T) || bytes[0] != std::uint8_t(T::type))
        return std::nullopt;
    T out;
    std::memcpy(&out, bytes.data() + 1, sizeof(T));
    return out;
}

template<typename Entry, typename Header>
std::vector<Entry> decode_array(
    std::span<std::uint8_t const> bytes, Header const& hdr)
{
    std::vector<Entry> out(hdr.count);
    size_t const at = 1 + sizeof(Header);
    if(bytes.size() < at + out.size() * sizeof(Entry))
        return {};
    std::memcpy(out.data(), bytes.data() + at, out.size() * sizeof(Entry));
    return out;
}

} // namespace poc::wire
