#pragma once

#include <coffee/components/entity_container.h>

#include <coffee/components/types.h>

#include <atomic>
#include <map>
#include <optional>
#include <vector>

struct NetworkState : compo::SubsystemBase
{
    using type = NetworkState;

    enum class ClientState
    {
        None,
        Establishing,
        Connecting,
        Unstable,
        Connected,
        Error,
        Disconnecting,
    } client_state;
    enum class ServerState
    {
        None,
        Listening,
        Error,
    } server_state;
    std::optional<std::string> error;
    std::optional<std::string> local_address;
    std::optional<std::string> remote_address;
    /*! What a client passes to --server to join this server, carrying the
     *  server's public key so the join is authenticated */
    std::optional<std::string> join_string;

    std::optional<libc_types::u32> remote_player_idx; /*!< Seat 0's */
    /*! Each joined local seat's player index on the server */
    std::map<libc_types::u32, libc_types::u32> remote_seats;

    /* Server time is the server's steady_clock, so it never steps. On the
     * server the offset is zero, so both sides can use the same calls. */
    using server_time_point = std::chrono::steady_clock::time_point;

    bool clock_authority{false}; /*!< This process is the server */
    bool replica{false}; /*!< Joined to a server, which decides what is hit */

    /*! The server's tick clock, on a client: `tick` began at server time
     *  `start_us`, and none follow while paused */
    struct TickBase
    {
        libc_types::u64 tick{0};
        libc_types::i64 start_us{0};
        libc_types::u64 pause_at{0}; /*!< Last tick before a pause, 0 = none */
        libc_types::u32 speed_permille{1000}; /*!< Ticks per tick of time */
        bool            paused{false};
    };
    std::optional<TickBase> tick_base;

    /*! On a server: the longest round trip to any client, ms */
    libc_types::u32 worst_ping_ms{0};

    /*! Session time: the server's time since it began listening. This is
     *  the local steady time (us) it started at, 0 while there is none.
     *  Read from any thread. */
    std::atomic<libc_types::i64> session_origin_us{0};

    /*! GNS fake network conditions, applied when changed. They act on this
     *  process's own packets, so one side alone is enough. */
    struct Simulation
    {
        libc_types::u32 lag_ms{0};      /*!< Added round trip */
        libc_types::f32 jitter_ms{0.f}; /*!< Mean extra delay per packet */
        libc_types::f32 loss_pct{0.f};  /*!< Dropped, each direction */

        bool active() const
        {
            return lag_ms || jitter_ms > 0.f || loss_pct > 0.f;
        }

        bool operator==(Simulation const&) const = default;
    } simulation;

    /*! Test fault injection: server messages a client drops on arrival,
     *  leaving the object census something to put right */
    struct Faults
    {
        libc_types::u32 drop_spawns{0};
        libc_types::u32 drop_despawns{0};
    } faults;

    std::optional<std::chrono::steady_clock::duration> server_clock_offset;
    std::chrono::microseconds server_clock_rtt{};      /*!< Best in window */
    std::chrono::microseconds server_clock_rtt_last{}; /*!< Latest sample */

    std::optional<server_time_point> server_now() const
    {
        if(!server_clock_offset)
            return std::nullopt;
        return std::chrono::steady_clock::now() + *server_clock_offset;
    }

    /*! compo::clock is the system clock, so translate through "now" */
    std::optional<server_time_point> to_server_time(compo::time_point t) const
    {
        auto now = server_now();
        if(!now)
            return std::nullopt;
        return *now + std::chrono::duration_cast<
                          std::chrono::steady_clock::duration>(
                          t - compo::clock::now());
    }

    std::optional<std::string> local_player_name;

    /*! Server answers to this client's spawn requests, oldest first */
    struct SpawnResponse
    {
        libc_types::u32 request_id{0};
        libc_types::u32 net_id{0}; /*!< 0 = rejected */
    };
    static constexpr libc_types::szptr max_spawn_responses = 64;
    std::vector<SpawnResponse>         spawn_responses;

    struct RosterEntry
    {
        std::string     name;
        libc_types::u32 remote_idx{0};
        libc_types::u32 loading_progress{100};
        bool            is_self{false};
    };
};

struct PlayerInfo;

struct PlayerRoster : compo::SubsystemBase
{
    using type = PlayerRoster;

    PlayerRoster(compo::EntityContainer& container)
        : m_container(container)
    {
    }

    libc_types::u32 player_count();

    std::vector<NetworkState::RosterEntry> roster(
        std::optional<libc_types::u32> self_idx = std::nullopt);

  private:
    compo::EntityContainer& m_container;
};

void alloc_networking(
    compo::EntityContainer& e, std::string const& gateway_auth_key = {});
