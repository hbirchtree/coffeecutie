#pragma once

/* Game rules for the PoC. ServerGame owns health and decides hits; ClientGame
 * moves its own biped, shows everyone else interpolated from snapshots and
 * predicts its own shots. Both only talk through the Bus. */

#include "events.h"
#include "hit_model.h"
#include "look.h"
#include "wire.h"

#include <array>
#include <chrono>
#include <deque>
#include <map>
#include <optional>
#include <string>

namespace poc {

enum class Policy : std::uint8_t
{
    server_now,    /* server traces against its present: no lag comp */
    rewind,        /* server rewinds to what the shooter saw, own trace */
    shooter_right, /* rewind, and accept the shooter's claim if plausible */
};

char const* to_string(Policy p);
char const* to_string(WeaponKind w);
char const* to_string(wire::Verdict v);

struct Weapon
{
    bool  hitscan;
    float damage;
    float head_mult; /* body damage on an unshielded head */
    float speed;     /* wu/s */
    float gravity;
    float fuse;
    float splash_radius;
    float splash_damage;
    float range;
};

Weapon const& weapon(WeaponKind kind);

struct GameConfig
{
    Policy policy{Policy::shooter_right};
    float  interp_ms{100.f};
    /* A shot may rewind as far as the shooter's own latency explains
     * (server-measured ping + interpolation + slack), never past the cap */
    float  max_rewind_ms{1000.f};
    float  rewind_slack_ms{100.f};
    float  claim_tolerance{0.15f}; /* wu between claim and the server ray */
    float  catchup_max_ms{150.f};  /* projectile fast-forward cap */
    int    respawn_ticks{60};

    /* Aim from input: the server rebuilds every view from look input and
     * shoots along that, not along the direction a fire message names */
    bool       validate_input{true};
    LookConfig look;
    float snap_deg{3.f};     /* a turn this fast in one tick ... */
    float settle_deg{0.1f};  /* ... that stops dead the next, then a shot */
    int   constant_run{8};   /* identical non-zero mouse deltas in a row */
    float silent_deg{0.5f};  /* fire direction this far off the view */
    float rebase_deg_per_tick{25.f}; /* resync jump allowed per lost tick */
};

/* What happened, gathered from every peer in the process */
struct Report
{
    struct Shot
    {
        biped_id      shooter;
        std::uint32_t shot;
        WeaponKind    weapon;
        float         ping;
        float         view_tick;
        SurfaceHit    client;
        bool          resolved{false};
        wire::ShotResult server{};
    };

    struct Projectile
    {
        biped_id      owner;
        std::uint32_t client_ref;
        WeaponKind    weapon;
        float         ping;
        proj_id       id{0};
        float         catchup{0.f};
        float         spawn_divergence{-1.f}; /* predicted vs auth, wu */
        bool          predicted_done{false};
        SurfaceHit    predicted{};
        bool          server_done{false};
        vec3          server_point{0.f};
        biped_id      server_target{no_biped};
        bool          server_head{false};
    };

    /* Server-side look checks, per player */
    struct Aim
    {
        std::uint32_t inputs{0};
        std::uint32_t lost{0};    /* ticks of input never received */
        std::uint32_t rebases{0}; /* resyncs from the reported view */
        std::uint32_t suspicious_rebases{0};
        std::uint32_t snap_fires{0};
        std::uint32_t constant_runs{0};
        std::uint32_t silent_aims{0};
        std::uint32_t no_trigger{0};
        float         max_turn_deg{0.f}; /* fastest single tick */
        /* Reported view vs. rebuilt, for information only: a client can
         * report anything */
        float         drift_max_deg{0.f};
        double        drift_sum_deg{0.};
        std::uint32_t drift_n{0};
    };

    std::vector<Shot>         shots;
    std::vector<Projectile>   projectiles;
    std::vector<wire::Damage> damage; /* as applied by the server */
    std::map<biped_id, Aim>   aim;

    Shot*       find_shot(biped_id shooter, std::uint32_t shot);
    Projectile* find_projectile(biped_id owner, std::uint32_t ref);
    void        print(bool verbose) const;
};

class ServerGame
{
  public:
    ServerGame(GameConfig const& config, HitModel const& model, Report& report)
        : m_config(config)
        , m_model(model)
        , m_report(report)
    {
    }

    void handle(NetConnected const& ev, Bus& bus);
    void handle(NetDisconnected const& ev, Bus& bus);
    void handle(NetStats const& ev, Bus& bus);
    void handle(NetReceived const& ev, Bus& bus);
    void handle(PhysRayResult const& ev, Bus& bus);
    void handle(PhysProjectileImpact const& ev, Bus& bus);
    void handle(PhysSplashResult const& ev, Bus& bus);

    void tick(tick_t now, Bus& bus);

    /* Scenario setup: start with this much health */
    void set_vitality(float body, float shield);

  private:
    struct Player
    {
        conn_id   conn;
        biped_id  id;
        BipedPose spawn;
        BipedPose pose;
        float     body;
        float     shield;
        tick_t    death_tick{-1};
        float     ping{0.f};

        /* Look state rebuilt from input */
        struct Tick
        {
            std::uint32_t seq{0};
            View          before; /* view entering the tick */
            View          after;
            bool          fire{false};
            LookInput     pre_fire{};
            tick_t        arrived{0}; /* server tick the input came in */
        };

        View                 view;
        std::uint32_t        seq{0};
        bool                 need_rebase{false};
        int                  lost_run{0};
        bool                 fired_fast{false};
        std::array<Tick, 128> ticks{};
        float                last_turn_deg{0.f};
        LookInput            last_look{};
        int                  same_look{0};
        /* Fire messages that beat their input here */
        std::vector<std::vector<std::uint8_t>> waiting;
    };

    struct PendingShot
    {
        biped_id          shooter;
        conn_id           conn;
        wire::FireHitscan fire;
    };

    struct LiveProjectile
    {
        biped_id      owner;
        WeaponKind    weapon;
        std::uint32_t client_ref;
    };

    struct PendingSplash
    {
        biped_id   owner;
        WeaponKind weapon;
        biped_id   direct;
    };

    Player* by_conn(conn_id conn);
    float   ticks(float ms) const
    {
        return ms / 1000.f * tick_rate;
    }

    void on_fire(Player& p, wire::FireHitscan const& msg, Bus& bus);
    void on_fire(Player& p, wire::FireProjectile const& msg, Bus& bus);
    void on_input(
        Player&                              p,
        std::span<wire::InputEntry const>    entries,
        Bus&                                 bus);
    void on_pose(Player& p, wire::Pose const& msg);
    /* Direction the input at `seq` fired along, if it fired at all. Flags
     * a fire message that disagrees with it. */
    std::optional<vec3> fired_along(
        Player& p, std::uint32_t seq, vec3 const& claimed);
    void dispatch(Player& p, std::span<std::uint8_t const> bytes, Bus& bus);
    void apply_damage(
        biped_id   target,
        biped_id   source,
        WeaponKind weapon,
        float      amount,
        bool       head,
        float      shield_mult,
        float      body_mult,
        bool       splash,
        Bus&       bus);

    GameConfig const& m_config;
    HitModel const&   m_model;
    Report&           m_report;

    tick_t                            m_now{0};
    biped_id                          m_next_id{1};
    float                             m_start_body{-1.f};
    float                             m_start_shield{-1.f};
    std::map<biped_id, Player>        m_players;
    std::uint32_t                     m_next_query{1};
    std::map<std::uint32_t, PendingShot>   m_shots;
    proj_id                           m_next_proj{1};
    std::map<proj_id, LiveProjectile> m_projectiles;
    std::map<std::uint32_t, PendingSplash> m_splashes;
};

class ClientGame
{
  public:
    using clock = std::chrono::steady_clock;

    ClientGame(
        GameConfig const& config,
        HitModel const&   model,
        Report&           report,
        BipedPose         spawn)
        : m_config(config)
        , m_model(model)
        , m_report(report)
        , m_pose(spawn)
        , m_view{spawn.yaw, 0.f}
    {
    }

    void handle(NetConnected const& ev, Bus& bus);
    void handle(NetDisconnected const& ev, Bus& bus);
    void handle(NetStats const& ev, Bus& bus);
    void handle(NetReceived const& ev, Bus& bus);
    void handle(PhysRayResult const& ev, Bus& bus);
    void handle(PhysProjectileImpact const& ev, Bus& bus);
    void handle(PhysSplashResult const&, Bus&)
    {
    }
    void handle(InputFrame const& ev, Bus& bus);

    void tick(Bus& bus);

    /* What a player (or bot) gets to look at */
    bool                     joined() const
    {
        return m_self != no_biped && !m_snapshots.empty();
    }
    biped_id                 self() const
    {
        return m_self;
    }
    bool                     alive() const;
    BipedPose const&         pose() const
    {
        return m_pose;
    }
    View const& view() const
    {
        return m_view;
    }
    LookConfig const& look_config() const
    {
        return m_config.look;
    }
    vec3 eye() const;
    std::vector<biped_id>    remotes() const;
    std::optional<BipedPose> displayed(biped_id id) const;
    float                    view_tick() const
    {
        return m_view_tick;
    }
    float present_tick() const;
    float ping() const
    {
        return m_ping;
    }

  private:
    struct Snapshot
    {
        tick_t                           tick;
        clock::time_point                received;
        std::vector<wire::SnapshotEntry> entries;
    };

    struct PendingShot
    {
        std::uint32_t shot;
        std::uint32_t seq;
        WeaponKind    weapon;
        float         view_tick;
        vec3          origin;
        vec3          direction;
    };

    struct Predicted
    {
        std::uint32_t     ref;
        float             speed;
        clock::time_point fired;
    };

    void on_snapshot(wire::SnapshotHeader const& hdr, std::vector<wire::SnapshotEntry> entries);
    std::optional<BipedPose> interpolate(biped_id id, float tick) const;

    GameConfig const& m_config;
    HitModel const&   m_model;
    Report&           m_report;

    BipedPose                m_pose;
    View                     m_view;
    std::uint32_t            m_seq{0};
    std::deque<wire::InputEntry> m_recent; /* resent every tick */
    biped_id                 m_self{no_biped};
    float                    m_ping{0.f};
    std::deque<Snapshot>     m_snapshots;
    float                    m_view_tick{-1.f};
    tick_t                   m_local_tick{0};
    std::map<biped_id, BipedPose> m_displayed;

    std::uint32_t                           m_next_query{1};
    std::uint32_t                           m_next_shot{1};
    std::map<std::uint32_t, PendingShot>    m_shots;
    /* Own projectiles fly as predicted (cosmetic) copies under
     * predicted_bit | ref; everyone else's are copies under the server id */
    static constexpr proj_id                predicted_bit = 0x80000000u;
    std::uint32_t                           m_next_ref{1};
    std::map<std::uint32_t, Predicted>      m_predicted;
};

} // namespace poc
