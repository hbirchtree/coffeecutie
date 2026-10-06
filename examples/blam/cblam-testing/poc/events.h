#pragma once

/* Every interaction between the PoC modules is one of these events. Game
 * logic never calls physics or the transport directly: it pushes a Phys* or
 * NetSend event and gets a *Result / NetReceived back on the same bus. That
 * keeps each side replaceable (Bullet for Physics, the app's Networking for
 * GnsSession) without touching the game rules. */

#include <glm/glm.hpp>

#include <optional>

#include <cstdint>
#include <deque>
#include <variant>
#include <vector>

namespace poc {

using vec3     = glm::vec3;
using mat4     = glm::mat4;
using tick_t   = std::int32_t;
using biped_id = std::uint16_t;
using proj_id  = std::uint32_t;
using conn_id  = std::uint32_t;

constexpr biped_id no_biped     = 0xFFFF;
constexpr conn_id  broadcast    = 0;
constexpr float    tick_rate    = 30.f;
constexpr float    tick_seconds = 1.f / tick_rate;
constexpr float    gravity      = 3.22f; /* 1 g in world units (10 ft) / s^2 */

struct BipedPose
{
    vec3  pos{0.f}; /* feet */
    float yaw{0.f}; /* radians about +Z, model faces +X */
    bool  alive{true};
};

/* ---- towards physics ---- */

/* Current pose of a biped. With keep_history the physics side also records
 * it under `tick` so later queries can rewind. */
struct PhysPose
{
    biped_id  id;
    tick_t    tick;
    BipedPose pose;
};

struct PhysRemoveBiped
{
    biped_id id;
};

/* Optional hit claim from a client, checked against the same (rewound) pose
 * the ray is traced against. */
struct RayClaim
{
    biped_id target{no_biped};
    std::int16_t node{-1};
    vec3     local{0.f}; /* hit point in the claimed node's space */
};

struct PhysRay
{
    std::uint32_t query;
    vec3          origin;
    vec3          end;
    float         rewind_tick{-1.f}; /* < 0: current poses */
    biped_id      ignore{no_biped};
    RayClaim      claim{};
};

struct PhysSpawnProjectile
{
    proj_id  id;
    biped_id owner;
    vec3     origin;
    vec3     velocity;
    float    gravity{0.f};
    float    fuse{10.f};   /* seconds until it goes off by itself */
    float    catchup{0.f}; /* ticks to simulate immediately on spawn */
    float    lag{0.f};     /* trace bipeds this many ticks in the past */
    bool     cosmetic{false};
};

struct PhysRemoveProjectile
{
    proj_id id;
};

struct PhysSplash
{
    std::uint32_t query;
    vec3          center;
    float         radius;
};

struct PhysStep
{
    tick_t tick;
};

/* ---- from physics ---- */

struct SurfaceHit
{
    enum Kind : std::uint8_t
    {
        none,
        terrain,
        biped,
    };

    Kind         kind{none};
    float        t{1.f};
    vec3         point{0.f};
    vec3         normal{0.f};
    biped_id     target{no_biped};
    std::int16_t node{-1};
    vec3         local{0.f}; /* point in node space, what a claim sends */
    bool         head{false};
    float        body_mult{1.f};
    float        shield_mult{1.f};
};

struct PhysRayResult
{
    std::uint32_t query;
    SurfaceHit    hit;
    float         rewound_to{-1.f};

    /* Filled when the query carried a claim */
    bool  claim_valid{false};       /* target existed and was alive then */
    vec3  claim_world{0.f};         /* claimed point at the rewound pose */
    float claim_ray_distance{1e9f}; /* how far that point is off the ray */
    bool  claim_occluded{false};    /* terrain between origin and claim */
};

struct PhysProjectileImpact
{
    proj_id    id;
    biped_id   owner;
    tick_t     tick;
    bool       cosmetic;
    bool       fuse_expired;
    SurfaceHit hit;
};

struct PhysSplashResult
{
    struct Victim
    {
        biped_id id;
        float    distance;
    };

    std::uint32_t       query;
    std::vector<Victim> victims; /* only ones with terrain line of sight */
};

/* ---- transport ---- */

struct NetSend
{
    conn_id                   to{broadcast};
    bool                      reliable{true};
    std::vector<std::uint8_t> bytes;
};

struct NetReceived
{
    conn_id                   from;
    std::vector<std::uint8_t> bytes;
};

struct NetConnected
{
    conn_id conn;
};

struct NetDisconnected
{
    conn_id conn;
};

struct NetStats
{
    conn_id conn;
    float   ping_ms;
};

/* ---- input (what a player or bot does) ---- */

enum class WeaponKind : std::uint8_t
{
    pistol,
    sniper,
    plasma,
    rocket,
    grenade,
};

/* Raw look input over one network tick, summed over every frame in it */
struct LookInput
{
    std::int16_t mouse_dx{0}; /* counts */
    std::int16_t mouse_dy{0};
    std::int8_t  stick_x{0}; /* deflection, -127..127, averaged over the tick */
    std::int8_t  stick_y{0};

    bool operator==(LookInput const&) const = default;
};

struct View
{
    float yaw{0.f}; /* about +Z, 0 = +X */
    float pitch{0.f};
};

/* One network tick of a player's (or bot's) input. pre_fire is the part of
 * `look` summed before the trigger was pulled, so the shot leaves where the
 * view was at that moment, not where it ended the tick. */
struct InputFrame
{
    vec3       position{0.f}; /* feet; movement is client-authoritative */
    LookInput  look{};
    bool       fire{false};
    WeaponKind weapon{WeaponKind::pistol};
    LookInput  pre_fire{};

    /* Cheats, to exercise the server's checks */
    std::optional<View> set_view;       /* write the view, skipping input */
    std::optional<vec3> fire_direction; /* shoot away from the view */
};

using Event = std::variant<
    PhysPose,
    PhysRemoveBiped,
    PhysRay,
    PhysSpawnProjectile,
    PhysRemoveProjectile,
    PhysSplash,
    PhysStep,
    PhysRayResult,
    PhysProjectileImpact,
    PhysSplashResult,
    NetSend,
    NetReceived,
    NetConnected,
    NetDisconnected,
    NetStats,
    InputFrame>;

/* FIFO, drained by Peer::pump(). Handlers may push while it drains. */
struct Bus
{
    std::deque<Event> queue;

    template<typename T>
    void push(T&& ev)
    {
        queue.emplace_back(std::forward<T>(ev));
    }
};

} // namespace poc
