#include "game_mode.h"
#include "devices.h"
#include "network/networking.h"
#include "network/relevance.h"
#include "network/network_events.h"

#include <magic_enum/magic_enum.hpp>

#include "types.h"

#include <blam/volta/blam_base_types.h>
#include <blam/volta/blam_scenario.h>
#include <coffee/core/CProfiling>
#include <coffee/core/debug/formatting.h>
#include <coffee/core/files/cfiles.h>
#include <deque>
#include <gsl/span_ext>
#include <peripherals/stl/base64.h>
#include <peripherals/stl/string/replace.h>
#include <peripherals/typing/vectors/glm_vector_types.h>
#include <url/url.h>

using Coffee::ProfContext;
using Coffee::Logging::cBasicPrint;

#if defined(USE_NETWORKING)

#include "components.h"
#include "blam_files.h"
#include "data.h"
#include "gateway_fleet_registration.h"
#include "journal.h"
#include "physics.h"
#include "selected_version.h"
#include "network/combat_sync.h"
#include "ticks.h"
#include "webrtc_identity.h"

#include <url/url.h>
#include "webrtc_signaling.h"

#include <GameNetworkingSockets/steam/isteamnetworkingsockets.h>
#include <GameNetworkingSockets/steam/isteamnetworkingutils.h>
#include <GameNetworkingSockets/steam/steamnetworkingsockets.h>
#include <coffee/components/restricted_subsystem.h>
#include <peripherals/identify/system.h>

/* Peer identity and certificate signing live here, and are used with or
 * without the DataChannel transport. */
using namespace webrtc_signaling;

using platform::url::constructors::MkUrl;

#include <fmt_extensions/format.h>
#include <fmt_extensions/url_types.h>

#if defined(USE_WEBRTC_TRANSPORT) && defined(COFFEE_WASM)
// Private APIs required for Wasm impl to work
extern "C" void SteamNetworkingSockets_SetManualPollMode(bool bFlag);
extern "C" void SteamNetworkingSockets_Poll(int msMaxWaitTime);
#endif

using NetworkingManifest = compo::SubsystemManifest<
    type_list_t<
        PlayerInfo,
        NetworkInfo,
        PlayerCamera,
        Model,
        ObjectPhysics,
        PhysicsData,
        const PlayerInput,
        BipedEquipment,
        const BipedStatus,
        const DeviceState,
        AnimationPlayback>,
    type_list_t<
        NetworkState,
        GameState,
        PhysicsBus,
        const DeviceGroups,
        const TickClock,
        const BlamFiles<halo_version>>,
    type_list_t<comp_app::ScreenshotProvider>>;

/* Lane 0 should be used for most common packets */
constexpr u32 FRAME_UPDATE_LANE = 0;
constexpr u32 EVENT_LANE        = 1;

struct Networking;

using libc_types::i64;
using typing::vector_types::Quatf;
using typing::vector_types::Vecf3;
using typing::vector_types::Vecf4;

namespace {

using namespace net_messages;

const std::map<std::string_view, std::pair<Vecf3, Quatf>> birds_eye_positions =
    {
        {
            "bloodgulch",
            {Vecf3{115.2, -153.9, 11.1},
             Quatf{0.96461415, 0.16558254, 0.20222986, 0.034714244}},
        },
        {
            "beavercreek",
            {Vecf3{31.6, 19.2, 7.6},
             Quatf{-0.9406504, -0.19067901, 0.27514988, 0.055775657}},
        },
        {
            "boardingaction",
            {Vecf3{10.1, -23.9, 9.2},
             Quatf{0.6651756, 0.23904572, 0.6657061, 0.2392364}},
        },
        {
            "carousel",
            {Vecf3{-9.4, -10.6, 2.1},
             Quatf{0.4567158, 0.12149616, 0.85165745, 0.22655913}},
        },
        {
            "chillout",
            {Vecf3{3.5, 12.5, 3.7},
             Quatf{0.7763298, 0.11336352, -0.6135422, -0.0895921}},
        },
        {
            "damnation",
            {Vecf3{-11.3, 6.9, 5.6},
             Quatf{0.251857, 0.07929034, -0.9199958, -0.28963608}},
        },
        {
            "hangemhigh",
            {Vecf3{31.7, 13.8, 5.8},
             Quatf{0.80502903, 0.30301514, -0.4773166, -0.17966329}},
        },
        {
            "longest",
            {Vecf3{-15.9, -15.4, 3.0},
             Quatf{0.06419813, 0.005146916, 0.99473226, 0.07974843}},
        },
        {
            "prisoner",
            {Vecf3{7.4, -1.8, 6.9},
             Quatf{-0.921312, -0.27995527, -0.25817588, -0.07845091}},
        },
        {
            "putput",
            {Vecf3{34.4, -27.0, 2.5},
             Quatf{0.9714654, 0.2070596, -0.113136195, -0.024113834}},
        },
        {
            "ratrace",
            {Vecf3{22.0, -24.6, -2.0},
             Quatf{0.9687177, 0.13157721, 0.20849791, 0.028319463}},
        },
        {
            "sidewinder",
            {Vecf3{2.6, 57.1, 2.6},
             Quatf{0.67849064, 0.09561429, -0.72123384, -0.101637624}},
        },
        {
            "wizard",
            {Vecf3{9.6, 9.2, -0.1},
             Quatf{0.91114575, 0.08682016, -0.40101838, -0.03821188}},
        },
};

std::pair<Vecf3, Quatf> birds_eye_for_map(std::string_view map)
{
    auto it = birds_eye_positions.find(map);
    if(it == birds_eye_positions.end())
    {
        cWarning("No birds-eye view for map {}", map);
        return {};
    }
    return it->second;
}

Networking* network_instance;

template<typename PtrType>
void* function_to_void(void (*func)(PtrType*))
{
    return reinterpret_cast<void*>(func);
}

MapFingerprint fingerprint_of(blam::map_container<halo_version> const& map)
{
    u64  hash   = 0xcbf29ce484222325;
    auto update = [&hash](void const* data, size_t size) {
        auto const* bytes = static_cast<u8 const*>(data);
        for(size_t i = 0; i < size; ++i)
            hash = (hash ^ bytes[i]) * 0x100000001b3;
    };

    blam::tag_index_view<halo_version> index(map);
    MapFingerprint                     out;
    for(blam::tag_t const& tag : index)
    {
        auto name = index.name_of(tag);
        update(tag.tagclass_e.data(), sizeof(tag.tagclass_e));
        update(&tag.tag_id, sizeof(tag.tag_id));
        update(name.data(), name.size());
        ++out.tag_count;
    }
    out.tag_hash[0] = static_cast<u32>(hash);
    out.tag_hash[1] = static_cast<u32>(hash >> 32);
    out.base_tag = map.tags->base_tag;
    return out;
}

std::string to_string(MapFingerprint const& fp)
{
    return fmt::format(
        "{:08x}{:08x}/{}/{:08x}",
        fp.tag_hash[1],
        fp.tag_hash[0],
        fp.tag_count,
        fp.base_tag);
}

CameraSync::flags_t camera_flags_of(PlayerInfo const& info)
{
    return info.mode.physics ? CameraSync::physics : CameraSync::none;
}


bool is_client_network_event(GameEvent const& event)
{
    switch(event.type)
    {
    case GameEvent::ServerCameraControl:
    case GameEvent::ServerPlayerStateUpdate:
    case GameEvent::ServerStateUpdate:
    case GameEvent::ServerJoinInfo:
        return true;
    default:
        return false;
    }
}

} // namespace

template<>
struct fmt::formatter<MessageBase::Type>
{
    template<typename ParseCtx>
    constexpr auto parse(ParseCtx& ctx)
    {
        return ctx.begin();
    }

    template<typename FormatCtx>
    auto format(const MessageBase::Type t, FormatCtx& ctx) const
    {
        return fmt::format_to(ctx.out(), "{}", magic_enum::enum_name(t));
    }
};

struct Networking : compo::RestrictedSubsystem<Networking, NetworkingManifest>
{
    using type  = Networking;
    using Proxy = compo::proxy_of<NetworkingManifest>;

    bool is_server() const
    {
        if(m_socket && m_socket != k_HSteamListenSocket_Invalid)
            return true;
#if defined(USE_WEBRTC_TRANSPORT)
        if(m_webrtcServer)
            return true;
#endif
        return false;
    }

    template<typename T>
    bool forward_game_event(GameEvent& event, const void* data)
    {
        if(event.type != T::event_type)
            return false;
        auto message = Message<GameEventWrapper<T>>({
            .event = event,
            .data  = *static_cast<T const*>(data),
        });
        if(is_server())
            send_all(std::move(message));
        else
            send_single(m_connection, std::move(message));
        return true;
    }

    auto generate_game_join()
    {
        return Message<GameJoin>({
            .map_name = m_map ? *blam::bl_string::from(m_map->internal_name())
                              : blam::bl_string{},
            .seed        = m_seed,
            .fingerprint = m_fingerprint,
        });
    }

#if defined(USE_WEBRTC_TRANSPORT)
    std::string build_server_metadata()
    {
        nlohmann::json meta = nlohmann::json::object();
        meta["map"]         = m_map ? std::string(m_map->internal_name()) : "";

        std::string map_type = "unknown";
        if(m_map)
        {
            if(auto scenario = m_map->scenario(); scenario.has_value())
            {
                switch(scenario.value()->info.type)
                {
                case blam::scn::scenario<halo_version>::scenario_type::solo:
                    map_type = "campaign";
                    break;
                case blam::scn::scenario<
                    halo_version>::scenario_type::multiplayer:
                    map_type = "multiplayer";
                    break;
                case blam::scn::scenario<
                    halo_version>::scenario_type::main_menu:
                    map_type = "main_menu";
                    break;
                }
            }
        }
        meta["map_type"] = map_type;

        u32 player_count = static_cast<u32>(m_local_player_info.size()) +
                           static_cast<u32>(m_connections.size());
        meta["player_count"]     = player_count;
        meta["player_count_max"] = 16;

        if(m_server_auth.type == AuthType::Ed25519 && m_server_key)
            meta = sign_metadata_ed25519(std::move(meta), m_server_key);

        return meta.dump();
    }

    void publish_server_metadata()
    {
        auto payload = build_server_metadata();
        if(payload.size() > 4096)
        {
            cWarning(
                "Server metadata payload too large ({} bytes), dropping",
                payload.size());
            return;
        }
        if(payload == m_last_metadata_sent)
            return;

        if(m_webrtcServer && m_webrtcServer->Active())
        {
            m_webrtcServer->SendMetadata(payload);
            m_last_metadata_sent = std::move(payload);
        } else if(m_fleetRegistration && m_fleetRegistration->Active())
        {
            m_fleetRegistration->SendMetadata(payload);
            m_last_metadata_sent = std::move(payload);
        }
    }
#endif

    void update_player_counts()
    {
        GameEvent         update{.type = GameEvent::ServerStateUpdate};
        ServerStateUpdate data = {
            .type      = ServerStateUpdate::PlayerCount,
            .num_field = static_cast<i32>(remote_player_count() + std::max<size_t>(m_local_player_info.size(), 1)),
        };
        m_game_bus.inject(update, &data);
        data = {
            .type      = ServerStateUpdate::PlayerMaxCount,
            .num_field = 16,
        };
        m_game_bus.inject(update, &data);
#if defined(USE_WEBRTC_TRANSPORT)
        publish_server_metadata();
#endif
    }

    size_t remote_player_count() const
    {
        size_t count = 0;
        for(auto const& [_, state] : m_connections)
            count += state.players.size();
        return count;
    }

    void apply_simulation()
    {
        auto const& sim = m_net_state.simulation;
        if(!m_utils || sim == m_applied_simulation)
            return;
        m_applied_simulation = sim;

        i32 const lag_send = static_cast<i32>(sim.lag_ms / 2);
        m_utils->SetGlobalConfigValueInt32(
            k_ESteamNetworkingConfig_FakePacketLag_Send, lag_send);
        m_utils->SetGlobalConfigValueInt32(
            k_ESteamNetworkingConfig_FakePacketLag_Recv,
            static_cast<i32>(sim.lag_ms) - lag_send);

        f32 const jitter_pct = sim.jitter_ms > 0.f ? 100.f : 0.f;
        for(auto [avg, max, pct] : {
                std::tuple{
                    k_ESteamNetworkingConfig_FakePacketJitter_Send_Avg,
                    k_ESteamNetworkingConfig_FakePacketJitter_Send_Max,
                    k_ESteamNetworkingConfig_FakePacketJitter_Send_Pct},
                std::tuple{
                    k_ESteamNetworkingConfig_FakePacketJitter_Recv_Avg,
                    k_ESteamNetworkingConfig_FakePacketJitter_Recv_Max,
                    k_ESteamNetworkingConfig_FakePacketJitter_Recv_Pct},
            })
        {
            m_utils->SetGlobalConfigValueFloat(avg, sim.jitter_ms);
            m_utils->SetGlobalConfigValueFloat(max, sim.jitter_ms * 4.f);
            m_utils->SetGlobalConfigValueFloat(pct, jitter_pct);
        }

        m_utils->SetGlobalConfigValueFloat(
            k_ESteamNetworkingConfig_FakePacketLoss_Send, sim.loss_pct);
        m_utils->SetGlobalConfigValueFloat(
            k_ESteamNetworkingConfig_FakePacketLoss_Recv, sim.loss_pct);

        cDebug(
            "Network simulation: +{} ms rtt, {} ms jitter, {}% loss",
            sim.lag_ms,
            sim.jitter_ms,
            sim.loss_pct);
        journal(
            "net_simulation",
            {{"lag_ms", sim.lag_ms},
             {"jitter_ms", sim.jitter_ms},
             {"loss_pct", sim.loss_pct}});
    }

    /* Clock sync: a burst at connect, then a steady trickle. Sampling
     * speeds up again for a while when the estimate jumps. */
    static constexpr u32 clock_burst_samples  = 8;
    static constexpr i64 clock_burst_interval = 150'000;
    static constexpr i64 clock_interval       = 1'000'000;
    static constexpr i64 clock_unsettled      = 250'000;
    static constexpr i64 clock_unsettled_for  = 3'000'000;
    static constexpr u32 clock_window         = 16;
    static constexpr i64 clock_window_age     = 16'000'000;
    static constexpr i64 clock_jump           = 5'000;
    static constexpr i64 clock_snap           = 250'000;
    static constexpr i64 clock_slew_per_mille = 5; /*!< 0.5% */

    struct clock_sample_t
    {
        i64 offset{0};
        i64 rtt{0};
        i64 taken{0};
    };

    struct clock_state_t
    {
        bool                       active{false};
        u32                        burst{0};
        i64                        next_ping{0};
        i64                        unsettled_until{0};
        i64                        last_slew{0};
        i64                        server_base{0};
        std::optional<i64>         estimate{};
        std::optional<i64>         applied{};
        std::deque<clock_sample_t> samples{};
    };

    /*! GNS local timestamp minus steady_clock, in us */
    i64 gns_minus_local() const
    {
        return m_utils->GetLocalTimestamp() -
               std::chrono::duration_cast<std::chrono::microseconds>(
                   std::chrono::steady_clock::now().time_since_epoch())
                   .count();
    }

    void clock_tick()
    {
        if(!m_utils)
            return;
        i64 const now  = m_utils->GetLocalTimestamp();
        i64 const base = gns_minus_local();

        if(!m_clock_base_journaled && (is_server() || m_clock.active))
        {
            /* Same-machine tests diff these to get the true GNS offset */
            journal("net_clock_base", {{"gns_minus_local", base}});
            m_clock_base_journaled = true;
        }

        m_net_state.clock_authority = is_server();
        if(is_server())
        {
            m_net_state.server_clock_offset =
                std::chrono::steady_clock::duration::zero();
            return;
        }
        if(!m_clock.active)
            return;

        if(now >= m_clock.next_ping)
        {
            send_single(
                m_connection,
                Message<ClockSync>({.client_send = now}),
                k_nSteamNetworkingSend_UnreliableNoNagle);
            i64 interval = clock_interval;
            if(m_clock.burst > 0)
            {
                --m_clock.burst;
                interval = clock_burst_interval;
            } else if(now < m_clock.unsettled_until)
                interval = clock_unsettled;
            m_clock.next_ping = now + interval;
        }

        if(m_clock.estimate && m_clock.applied)
        {
            i64 const step =
                (now - m_clock.last_slew) * clock_slew_per_mille / 1000;
            m_clock.applied = *m_clock.applied +
                              std::clamp(
                                  *m_clock.estimate - *m_clock.applied, -step, step);
        }
        m_clock.last_slew = now;

        /* local steady -> local GNS -> server GNS -> server steady */
        if(m_clock.applied)
            m_net_state.server_clock_offset =
                std::chrono::duration_cast<std::chrono::steady_clock::duration>(
                    std::chrono::microseconds(
                        base + *m_clock.applied - m_clock.server_base));
    }

    /* Where session time began, on this process's steady clock */
    void session_tick()
    {
        using namespace std::chrono;
        i64 const steady_us =
            duration_cast<microseconds>(steady_clock::now().time_since_epoch())
                .count();
        if(is_server() && m_session_start_us == 0)
            m_session_start_us = steady_us;
        i64 origin = 0;
        if(is_server())
            origin = m_session_start_us;
        else if(m_session_start_us != 0 && m_net_state.server_clock_offset)
            origin = m_session_start_us -
                     duration_cast<microseconds>(*m_net_state.server_clock_offset)
                         .count();
        m_net_state.session_origin_us = origin;
    }

    void clock_reply(
        HSteamNetConnection connection, ClockSync const& request, i64 received)
    {
        auto reply        = request;
        reply.server_recv = received;
        reply.server_base = gns_minus_local();
        reply.server_send = m_utils->GetLocalTimestamp();
        send_single(
            connection,
            Message<ClockSync>(std::move(reply)),
            k_nSteamNetworkingSend_UnreliableNoNagle);
    }

    void clock_sample(ClockSync const& sync, i64 received)
    {
        if(!m_clock.active)
            return;
        i64 const rtt = std::max<i64>(
            0,
            (received - sync.client_send) - (sync.server_send - sync.server_recv));
        i64 const offset = ((sync.server_recv - sync.client_send) +
                            (sync.server_send - received)) /
                           2;

        m_clock.server_base = sync.server_base;

        auto& samples = m_clock.samples;
        samples.push_back({.offset = offset, .rtt = rtt, .taken = received});
        while(samples.size() > clock_window ||
              received - samples.front().taken > clock_window_age)
            samples.pop_front();

        /* Queueing only ever adds delay, so the fastest sample lies least */
        auto const& best = *std::min_element(
            samples.begin(), samples.end(), [](auto const& a, auto const& b) {
                return a.rtt < b.rtt;
            });

        auto const previous = m_clock.estimate;
        m_clock.estimate    = best.offset;
        m_net_state.server_clock_rtt      = std::chrono::microseconds(best.rtt);
        m_net_state.server_clock_rtt_last = std::chrono::microseconds(rtt);

        if(!m_clock.applied ||
           std::abs(best.offset - *m_clock.applied) > clock_snap)
        {
            m_clock.applied = best.offset;
            journal("net_clock_snap", {{"offset", best.offset}, {"rtt", best.rtt}});
        } else if(previous && std::abs(best.offset - *previous) > clock_jump)
        {
            m_clock.unsettled_until = received + clock_unsettled_for;
            m_clock.next_ping = std::min(m_clock.next_ping, received + clock_unsettled);
        }
        journal(
            "net_clock",
            {{"offset", offset},
             {"rtt", rtt},
             {"estimate", *m_clock.estimate},
             {"applied", *m_clock.applied},
             {"steady_offset",
              gns_minus_local() + *m_clock.applied - m_clock.server_base}});
    }

    std::set<HSteamNetConnection> verified_connections() const
    {
        std::set<HSteamNetConnection> out;
        for(auto const& [connection, state] : m_connections)
            if(state.verified)
                out.insert(connection);
        return out;
    }

    void request_spawn(SpawnObjectEvent const& spawn)
    {
        u32 const request_id =
            spawn.net_id & ~SpawnObjectEvent::impostor_net_id_base;
        journal(
            "net_spawn_request",
            {{"request_id", request_id},
             {"tag_class", blam::to_string(spawn.object.class_id())},
             {"tag_id", spawn.object.tag_id}});
        send_single(
            m_connection,
            Message<SpawnRequest>({
                .position  = Vecf4(spawn.position, 1.f),
                .direction = spawn.rotation,
                .tag =
                    {
                        .tag_class = spawn.object.class_id(),
                        .tag_id    = spawn.object.tag_id,
                    },
                .request_id = request_id,
                .announce   = spawn.announce,
                .age        = spawn.age,
                .rounds     = spawn.rounds.value_or(std::array<u16, 2>{
                    SpawnRequest::no_rounds, SpawnRequest::no_rounds}),
            }));
    }

    /* Empty if a client may have this spawned */
    std::string_view check_spawn_request(
        bool                verified,
        SpawnRequest const& request,
        blam::tag_t const*& tag)
    {
        if(!verified || !m_map)
            return "not verified";
        blam::tag_index_view<halo_version> index(*m_map);
        auto it = index.find(request.tag.tag_id);
        if(it == index.end() || !(*it).valid() ||
           !(*it).matches(blam::tag_class_t::obje) ||
           !(*it).matches(request.tag.tag_class))
            return "not an object";
        tag = &(*it);
        if(SpawnObjectEvent::is_client_requestable(*tag))
            return {};
        return "class not requestable";
    }

    void replicate_spawn(
        SpawnObjectEvent const& spawn,
        HSteamNetConnection     requester  = k_HSteamNetConnection_Invalid,
        u32                     request_id = EntitySpawn::no_request)
    {
        /* Scenario loads repeat on BSP switches */
        for(auto const& existing : m_dynamic_spawns)
            if(existing.tag.instance_id == spawn.net_id)
                return;
        EntitySpawn wire{
            .position = Vecf4(spawn.position, 1.f),
            .rotation = spawn.rotation,
            .tag =
                {
                    .tag_class   = spawn.object.class_id(),
                    .tag_id      = spawn.object.tag_id,
                    .instance_id = spawn.net_id,
                },
            .held = spawn.held.valid() ? spawn.held.tag_id : blam::tag_index<>{},
        };
        /* Late joiners see it as already there */
        m_dynamic_spawns.push_back(wire);
        wire.announce = spawn.announce;
        journal(
            "net_object_spawn",
            {{"net_id", spawn.net_id},
             {"tag_class", blam::to_string(spawn.object.class_id())},
             {"tag_id", spawn.object.tag_id}});

        /* Unverified peers get the whole backlog once they verify */
        auto targets = verified_connections();
        if(requester != k_HSteamNetConnection_Invalid)
        {
            targets.erase(requester);
            EntitySpawn response = wire;
            response.response_id = request_id;
            /* Its impostor already played it */
            response.announce = 0;
            send_single(requester, Message<EntitySpawn>(std::move(response)));
        }
        if(targets.empty())
            return;
        send_all(
            Message<EntitySpawn>(std::move(wire)),
            k_nSteamNetworkingSend_Reliable,
            std::move(targets));
    }

    /*! Clients drop whoever the roster leaves out */
    void send_player_roster()
    {
        std::vector<PlayerSyncEntry> entries;
        for(auto const& local : m_local_player_info)
        {
            if(!local.exists())
                continue;
            auto const& info = (*local);
            entries.push_back({
                .name             = *blam::bl_string::from(info.name),
                .player_idx       = info.player_idx,
                .loading_progress = 100,
                .connected        = 0xFFFF,
                .spawned          = info.spawned ? 0xFFFFu : 0x0u,
            });
        }
        for(auto const& [_, state] : m_connections)
            for(auto const& [seat, player] : state.players)
            {
                if(!player.player_info.exists())
                    continue;
                auto const& info = (*player.player_info);
                entries.push_back({
                    .name             = *blam::bl_string::from(info.name),
                    .player_idx       = player.idx,
                    .loading_progress = state.loading_progress,
                    .connected =
                        player.biped.get<NetworkInfo>().connected ? 0xFFFFu : 0x0u,
                    .spawned = info.spawned ? 0xFFFFu : 0x0u,
                });
            }
        MessageBase header{.type = MessageBase::PlayerSync};
        send_all(std::move(header), gsl::make_span(entries));
    }

    /*! For a peer that has just joined or loaded */
    void send_positions(Proxy& p, HSteamNetConnection connection)
    {
        auto& state = m_connections[connection];
        for(auto entity : p.select<PlayerInfo, PlayerCamera>())
        {
            auto [info, cam] = entity.components();
            if(state.own(info.player_idx) || !is_networked(entity.id(), info))
                continue;
            send_single(
                connection,
                Message<CameraSync>(
                    camera_sync_of(p, entity.id(), info, cam, tick_now(p))));
        }
    }

    /*! Remote players and the local seats in the roster */
    bool is_networked(u64 entity, PlayerInfo const& info) const
    {
        if(info.is_remote())
            return true;
        return std::any_of(
            m_local_player_info.begin(),
            m_local_player_info.end(),
            [entity](auto const& local) { return local.m_id == entity; });
    }

    /*! In physics mode the body drives the camera, so it has to move too */
    u64 entity_of(Proxy& p, u32 net_id)
    {
        if(net_id == 0)
            return 0;
        for(auto object : p.select<NetworkInfo>())
            if(object.get<NetworkInfo>().instance_id == net_id)
                return object.id();
        return 0;
    }

    /* What the census covers: the server-spawned objects. Static scenario
     * objects every peer loads itself, by BSP section */
    static bool census_covers(u32 net_id)
    {
        if(net_id == 0 || SpawnObjectEvent::is_impostor(net_id))
            return false;
        return net_id >= SpawnObjectEvent::dynamic_net_id_base ||
               SpawnObjectEvent::scenario_group(net_id) >= ScenarioGroup::Vehicle;
    }

    static u32 census_bucket(u32 net_id, u32 buckets)
    {
        return ((net_id * 2654435761u) >> 16) % buckets;
    }

    /* A camera as sent: on a deck, where it is on that deck, so a peer puts
     * it on the deck as it has it rather than where the deck was */
    CameraSync camera_sync_of(
        Proxy& p, u64 entity, PlayerInfo const& info, PlayerCamera const& cam, u64 tick)
    {
        CameraSync sync{
            .position      = Vecf4(cam.camera.position, 0),
            .rotation      = cam.camera.rotation,
            .target_player = info.player_idx,
            .flags         = camera_flags_of(info),
            .tick          = tick,
        };
        auto const* data = info.mode.physics && !info.riding.vehicle
                               ? p.get<PhysicsData>(entity)
                               : nullptr;
        auto const* net   = data && data->deck ? p.get<NetworkInfo>(data->deck) : nullptr;
        auto const* model = net && net->instance_id ? p.get<Model>(data->deck) : nullptr;
        if(model)
        {
            sync.deck     = net->instance_id;
            sync.position = glm::inverse(model->transform) * Vecf4(cam.camera.position, 1.f);
            sync.position.w = 0.f;
        }
        return sync;
    }

    /* Where a sent camera is in the world, off the deck as it is here */
    Vecf3 world_of(Proxy& p, CameraSync const& sync)
    {
        if(sync.deck)
            if(auto const* model = p.get<Model>(entity_of(p, sync.deck)))
                return Vecf3(model->transform * Vecf4(Vecf3(sync.position), 1.f));
        return Vecf3(sync.position);
    }

    void place_local_player(
        Proxy& p, u64 entity, Vecf3 const& position, Quatf const& rotation)
    {
        auto* cam  = p.get<PlayerCamera>(entity);
        auto* info = p.get<PlayerInfo>(entity);
        if(!cam || !info)
            return;
        cam->camera.position = position;
        cam->camera.rotation = rotation;
        if(!info->mode.physics)
            return;
        Physics::Event     ev{Physics::Event::Translate};
        Physics::Translate translate{
            .entity_id = entity,
            .position  = position + Vecf3{0, 0, info->biped.spawn_lift()},
        };
        p.subsystem<PhysicsBus>().process(ev, &translate);
    }

    bool players_ready() const
    {
        if(is_server() && !m_map)
            return false;
        if(m_connections.empty())
            return is_server() && m_map;
        for(auto const& [_, state] : m_connections)
        {
            if(state.loading_progress < 100 || !state.verified)
                return false;
        }
        return true;
    }

    void send_permission(
        HSteamNetConnection          connection,
        u32                          player_idx,
        UpdatePermission::Permission perm,
        u16                          mode)
    {
        send_single(
            connection,
            Message<UpdatePermission>({
                .player_idx = player_idx,
                .permission = perm,
                .mode       = mode,
            }));
    }

    gsl::span<const blam::scn::player_starting_location> player_spawn_locs()
    {
        if(!m_map)
            return {};
        auto scenario = m_map->tags->scenario(m_map->map, m_map->magic);
        auto spawns_ =
            scenario.value()->player_start.locations.data(m_map->magic);
        if(!spawns_.has_value())
            return {};
        auto spawns = spawns_.value();
        return spawns;
    }

    /* Locks the camera to the birds-eye view; release_held_players() lets
     * everyone go together once all players are ready */
    void player_init(compo::EntityContainer& e, PlayerInfo& player)
    {
        player.permissions.camera = false;
        player.spawned            = false;

        /* create_camera() set the map's basis before this player came */
        std::optional<glm::mat3> basis;
        for(auto entity : e.select<PlayerInfo, PlayerCamera>())
            if(!entity.get<PlayerInfo>().is_remote())
                basis = entity.get<PlayerCamera>().camera_opts.world_basis;

        /* Write birds-eye position to PlayerCamera + mark dirty */
        for(auto entity : e.select<PlayerInfo, PlayerCamera>())
        {
            auto [info, cam] = entity.components();
            if(&info != &player)
                continue;
            if(basis)
                cam.camera_opts.world_basis = *basis;
            std::tie(cam.camera.position, cam.camera.rotation) =
                birds_eye_for_map(m_map->internal_name());
            if(auto* net = e.get<NetworkInfo>(entity.id()))
            {
                net->changes.viewport    = true;
                net->changes.permissions = true;
            }
            break;
        }

        auto spawn_loc =
            [this]() -> std::optional<blam::scn::player_starting_location> {
            auto spawns = player_spawn_locs();
            if(spawns.empty())
                return std::nullopt;
            auto spawn_idx = m_local_random.rand<u32>(0, spawns.size() - 1);
            return spawns[spawn_idx];
        }();

        auto pidx = player.player_idx;
        std::erase_if(m_held_players, [pidx](held_player_t const& held) {
            return held.player_idx == pidx;
        });
        m_held_players.push_back({.player_idx = pidx, .spawn = spawn_loc});
        send_player_roster();
    }

    void release_held_players(Proxy& p)
    {
        nlohmann::json released = nlohmann::json::array();
        for(auto const& held : m_held_players)
        {
            for(auto player : p.select<PlayerInfo, PlayerCamera>())
            {
                auto [info, cam] = player.components();
                if(info.player_idx != held.player_idx)
                    continue;
                if(held.spawn)
                {
                    cam.camera.position =
                        held.spawn->pos + Vecf3{0, 0, info.biped.eye_height};
                    cam.camera.rotation = glm::angleAxis(
                        glm::pi<f32>() - held.spawn->rot,
                        Vecf3{0.f, 1.f, 0.f});
                }
                info.permissions.camera = true;
                info.spawned            = true;
                if(auto* net = p.get<NetworkInfo>(player.id()))
                {
                    net->changes.viewport    = true;
                    net->changes.permissions = true;
                }
                released.push_back(held.player_idx);
                break;
            }
        }
        journal("net_spawn_release", {{"players", std::move(released)}});
        m_held_players.clear();
        m_release_at.reset();
        send_player_roster();
    }

    auto get_random_name()
    {
        using Coffee::FileMap;
        using stl_types::str::split::spliterator;
        using namespace Coffee::resource_literals;
        using namespace std::string_literals;

        auto all_names = "names.txt"_rsc;

        if(!FileMap(all_names))
            return "John Chief"s;

        std::string_view name_list(all_names.data_ro.data());
        size_t           num_names{0};
        for(auto it = spliterator(name_list, '\n'); it != spliterator<char>();
            ++it)
        {
            num_names++;
        }
        auto             target_i = m_local_random.rand<u32>(0, num_names);
        size_t           i{0};
        std::string_view out{};
        for(auto it = spliterator(name_list, '\n'); it != spliterator<char>();
            ++it)
        {
            if(target_i == i)
            {
                out = *it;
                break;
            }
            i++;
        }
        return std::string(out);
    }

    /* Effectively unlimited. Nothing renews the certificate, and a server must
     * not stop accepting authenticated clients after some uptime; the verifier
     * does not check expiry either. A leaked key is handled by rotating it. */
    static constexpr int k_self_signed_cert_seconds = 60 * 60 * 24 * 365 * 10;

    /* Issue ourselves a GNS certificate over our own connection key, signed by
     * the same Ed25519 key we sign metadata with. A client that was handed the
     * matching public key out of band pins it for its connection (see
     * connect_server_webrtc), and GNS then refuses any cert that key did not
     * sign -- which is what stops a relay on the rendezvous path from
     * substituting its own cert and reading the session. Without a cert the
     * peer presents an unsigned one, which the pinning client rejects. */
    void install_self_signed_cert()
    {
        if(m_server_auth.type != AuthType::Ed25519 ||
           m_server_auth.ed25519_public_key.empty() || !m_server_key)
            return;

        SteamNetworkingErrMsg ec{};
        int                   blob_size = 0;
        if(!SteamNetworkingSockets_GetSelfSignedCertBlobToSign(
               m_impl, k_self_signed_cert_seconds, &blob_size, nullptr, ec))
        {
            cWarning("Failed to size self-signed certificate: {}", ec);
            return;
        }
        std::vector<u8> blob(static_cast<size_t>(blob_size));
        if(!SteamNetworkingSockets_GetSelfSignedCertBlobToSign(
               m_impl, k_self_signed_cert_seconds, &blob_size, blob.data(), ec))
        {
            cWarning("Failed to build self-signed certificate: {}", ec);
            return;
        }

        auto signature = m_server_key.sign(
            std::string_view(
                reinterpret_cast<const char*>(blob.data()),
                static_cast<size_t>(blob_size)));
        if(signature.empty())
        {
            cWarning("Failed to sign self-signed certificate");
            return;
        }

        if(!SteamNetworkingSockets_InstallSelfSignedCert(
               m_impl,
               blob.data(),
               blob_size,
               signature.data(),
               static_cast<int>(signature.size()),
               m_server_auth.ed25519_public_key.data(),
               static_cast<int>(m_server_auth.ed25519_public_key.size()),
               ec))
        {
            cWarning("Failed to install self-signed certificate: {}", ec);
            return;
        }
        cDebug(
            "Installed self-signed GNS certificate for identity {}",
            derive_identity_ed25519(m_server_auth.ed25519_public_key));
    }

    /* A server always has a keypair, so the join string it hands out
     * authenticates it by default. The key is held only in memory, so the
     * join string changes every run. Done at listen time: a client needs no
     * identity, and the Listen button reaches here without any CLI flags.
     * A browser-hosted server gets its key here too, so it can be pinned. */
    void ensure_server_identity()
    {
        if(m_server_auth.type != AuthType::None)
            return;
        m_server_key    = Ed25519Key::generate();
        auto public_key = m_server_key.public_key();
        if(public_key.empty())
        {
            cWarning(
                "Failed to generate a server key, clients cannot "
                "authenticate this server");
            return;
        }
        m_server_auth.type               = AuthType::Ed25519;
        m_server_auth.ed25519_public_key = std::move(public_key);
        m_identity.SetGenericString(
            derive_identity_ed25519(m_server_auth.ed25519_public_key).c_str());
        m_impl->ResetIdentity(&m_identity);
        install_self_signed_cert();
    }

    /* address is ip:port, or a gateway URL when server_id is set */
    void publish_join_string(
        std::string const& address, std::string const& server_id = {})
    {
        auto        auth = format_auth_param(m_server_auth);
        std::string join = address;
        if(!server_id.empty())
        {
            join += "#" + server_id;
            if(!auth.empty())
                join += ";" + auth;
        } else if(!auth.empty())
            join += "#" + auth;
        m_net_state.join_string = join;

        SteamNetworkingIPAddr bound;
        bool const            wildcard =
            server_id.empty() &&
            m_impl->GetListenSocketAddress(m_socket, &bound) &&
            (bound.IsIPv6AllZeros() || (bound.IsIPv4() && !bound.GetIPv4()));
        cDebug("Join this server with: --server {}", join);
        /* In the fragment: the key never reaches the page host or a Referer */
        cDebug(
            "Join this server from a browser: "
            "https://<server>/<path>/#map=<map>&server={}",
            stl_types::str::replace::str<char>(join, "#", "%23"));
        if(wildcard)
            cDebug(
                "  (listening on all interfaces: replace {} with an address "
                "clients can reach)",
                address.substr(0, address.rfind(':')));
    }

    Networking(
        GameEventBus&      game_bus,
        NetworkState&      net_state,
        std::string const& gateway_auth_key)
        : m_game_bus(game_bus)
        , m_net_state(net_state)
        , m_combat(
              game_bus,
              [this](std::string_view type, nlohmann::json data) {
                  journal(type, std::move(data));
              })
    {
        network_instance = this;

        m_local_random.seed_automatically();

        this->priority = 1280;
        m_identity.Clear();
        SteamNetworkingErrMsg ec;
#if defined(USE_WEBRTC_TRANSPORT) && defined(COFFEE_WASM)
        /* SetLocalHost() always renders as "ip:::1" -- every peer using it
         * collides on the same value, and GNS's P2P signal handler treats
         * IsLocalHost() as an "identity not learned yet" sentinel, not a
         * real identity. Give each instance a unique one instead. Scoped to
         * wasm: native builds go through plain UDP (--listen/--server),
         * which never hits this identity-matching path at all. */
        char randomIdentity[24];
        snprintf(
            randomIdentity,
            sizeof(randomIdentity),
            "peer-%08x",
            m_local_random.rand<u32>(0, 0xFFFFFFFFu));
        m_identity.SetGenericString(randomIdentity);
        if(!GameNetworkingSockets_Init(&m_identity, ec))
#else
        if(!gateway_auth_key.empty())
        {
            /* A persistent identity; without one, a server gets an in-memory
             * key when it starts listening. A bare file name lives in the
             * config directory, not wherever the process was started. */
            std::string key_path = gateway_auth_key;
            if(key_path.find_first_of("/\\") == std::string::npos)
                key_path = *platform::url::constructors::MkUrl(
                    key_path, semantic::RSCA::ConfigFile);
            m_server_key = Ed25519Key::load_or_generate(key_path);
            if(auto public_key = m_server_key.public_key(); !public_key.empty())
            {
                m_server_auth.type               = AuthType::Ed25519;
                m_server_auth.ed25519_public_key = std::move(public_key);
                m_identity.SetGenericString(
                    derive_identity_ed25519(m_server_auth.ed25519_public_key)
                        .c_str());
            } else
                cWarning("Failed to load server key {}", key_path);
        }
        if(m_identity.IsInvalid())
            m_identity.SetLocalHost();
        if(!GameNetworkingSockets_Init(
               m_identity.IsLocalHost() ? nullptr : &m_identity, ec))
#endif
        {
            cWarning("Failed to init networking: {}", ec);
            return;
        }
        m_impl  = SteamNetworkingSockets();
        m_utils = SteamNetworkingUtils();

        /* No-op unless an Ed25519 key was configured, which is the only mode
         * that can authenticate a server to a client it shares no secret
         * with. Not tied to a transport: the cert protects a direct or P2P
         * UDP session exactly as it protects a relayed one. */
        install_self_signed_cert();

#if defined(USE_WEBRTC_TRANSPORT) && defined(COFFEE_WASM)
        // On Wasm, we need to ensure we run network code on the main thread
        // This is a constraint related to browser WebRTC impl
        SteamNetworkingSockets_SetManualPollMode(true);
#endif

        m_game_bus.addEventFunction<ServerConnectEvent>(
            0, [this](GameEvent&, ServerConnectEvent* connect) {
                cDebug("Connection requested to server: {}", connect->remote);
                m_relay_only = connect->relay_only;
                if(connect->type == ServerConnectEvent::Peer)
                    connect_symmetric(connect->remote);
                else if(connect->type == ServerConnectEvent::Server)
                {
                    /* Per join: a key or identity from an earlier one must not
                     * carry over to a server that never offered one */
                    m_client_auth = {};
                    m_expected_server_identity.clear();
                    /* A gateway join URL carries the key in its fragment and
                     * parses it later; this is the plain-address route in. */
                    if(!connect->server_public_key.empty())
                    {
                        m_client_auth = parse_auth_param(
                            "auth=ed25519:" + connect->server_public_key);
                        if(m_client_auth.type != AuthType::Ed25519)
                        {
                            m_net_state.client_state =
                                NetworkState::ClientState::Error;
                            return;
                        }
                    }
                    connect_server(connect->remote);
                } else
#if defined(USE_WEBRTC_TRANSPORT)
                    if(connect->remote.starts_with("ws://") ||
                       connect->remote.starts_with("wss://"))
                {
                    create_server_webrtc(
                        connect->remote, connect->gateway_server_id);
                } else
#endif
                {
                    create_server(connect->remote);
                    bool registered = false;
#if defined(USE_WEBRTC_TRANSPORT)
                    if(!connect->gateway_register_url.empty() &&
                       m_socket != k_HSteamListenSocket_Invalid)
                    {
                        registered = true;
                        cDebug("Starting gateway fleet registration");
                        m_fleetRegistration = std::make_unique<
                            webrtc_signaling::GatewayFleetRegistration>(
                            connect->gateway_register_url,
                            connect->gateway_server_id,
                            m_server_key,
                            m_impl,
                            m_socket,
                            connect->relay_only);
                        m_fleetRegistration->Start();
                    }
#endif
                    if(m_net_state.server_state !=
                       NetworkState::ServerState::Listening)
                        return;
                    if(registered)
                        publish_join_string(
                            connect->gateway_register_url,
                            connect->gateway_server_id);
                    else
                        publish_join_string(local_name());
                }
            });
        /* Host drops clients and stops listening; client disconnects.
         * GNS reports no status change for a locally closed connection */
        m_game_bus.addEventFunction<ServerDisconnectEvent>(
            0, [this](GameEvent&, ServerDisconnectEvent*) {
                if(is_server())
                {
                    std::vector<HSteamNetConnection> clients;
                    for(auto const& [connection, _] : m_connections)
                        clients.push_back(connection);
                    cDebug("Stopping server, dropping {} clients", clients.size());
                    for(auto connection : clients)
                        server_close_peer_connection(
                            connection,
                            k_ESteamNetConnectionEnd_App_Generic,
                            true,
                            "Host left the game");
                    stop_server();
                    return;
                }
                if(m_connection == k_HSteamNetConnection_Invalid)
                    return;
                cDebug("Leaving server ({})", remote_name());
                close_server_connection(m_connection, true);
            });
        m_game_bus.addEventFunction<MapListingEvent>(
            0, [this](GameEvent&, MapListingEvent* listing) {
                m_map_directory = listing->directory;
            });
        m_game_bus.addEventFunction<MapLoadEvent>(
            0, [this](GameEvent&, MapLoadEvent* load) {
                /* Only use this callback if we're the server */
                if(!load->file || !is_server())
                    return;
                auto map_name = (*load->file).path().fileBasename().removeExt();
                send_all(
                    Message<GameEventWrapper<MapLoadByNameEvent>>({
                        .event = {GameEvent::MapLoadByName},
                        .data =
                            MapLoadByNameEvent{
                                .origin = MapLoadEvent::Remote,
                                .map_name =
                                    blam::bl_string::from(map_name.internUrl)
                                        .value(),
                            },
                    }));
                for(auto& [_, state] : m_connections)
                {
                    state.loading_progress = 0;
                    for(auto& [_, player] : state.players)
                        if(player.player_info.exists())
                            (*player.player_info).loading_progress = 0;
                }
            });
        /* Ahead of ResourceLoader's queue (prio 100), so the copy it spawns
         * from already carries the net id */
        m_game_bus.addEventFunction<SpawnObjectEvent>(
            0, [this](GameEvent&, SpawnObjectEvent* spawn) {
                if(spawn->origin != MapLoadEvent::Local || spawn->local_only)
                    return;
                bool const replica = !is_server() &&
                                     m_connection != k_HSteamNetConnection_Invalid;
                if(spawn->server_owned && replica)
                {
                    /* Scenario objects come from the server regardless */
                    if(spawn->net_id != 0)
                        spawn->deferred = true;
                    else
                        spawn->net_id = SpawnObjectEvent::impostor_net_id_base |
                                        (++m_next_request_id & 0x0FFFFFFF);
                } else if(spawn->net_id == 0 && !replica)
                    /* Offline too: what can be picked up goes by its id */
                    spawn->net_id =
                        SpawnObjectEvent::dynamic_net_id_base + m_next_net_id++;
            });
        m_spawn_queue = m_game_bus.addQueuedEventFunction<SpawnObjectEvent>(
            0, [this](GameEvent&, SpawnObjectEvent* spawn) {
                if(spawn->origin != MapLoadEvent::Local || spawn->net_id == 0 ||
                   spawn->deferred)
                    return;
                if(SpawnObjectEvent::is_impostor(spawn->net_id))
                    request_spawn(*spawn);
                else
                    replicate_spawn(*spawn);
            });
        m_despawn_queue = m_game_bus.addQueuedEventFunction<DespawnObjectEvent>(
            0, [this](GameEvent&, DespawnObjectEvent* gone) {
                if(!is_server() || gone->net_id == 0 ||
                   SpawnObjectEvent::is_impostor(gone->net_id))
                    return;
                m_despawns.push_back({.net_id = gone->net_id});
                /* Late joiners must not get it back with the backlog */
                std::erase_if(m_dynamic_spawns, [gone](EntitySpawn const& s) {
                    return s.tag.instance_id == gone->net_id;
                });
            });
        m_pickup_queue = m_game_bus.addQueuedEventFunction<EquipWeaponEvent>(
            0, [this](GameEvent&, EquipWeaponEvent* equip) {
                if(is_server() && equip->mode == EquipWeaponEvent::pick_up)
                    m_pickups.push_back(*equip);
            });
        m_input_queue = m_game_bus.addQueuedEventFunction<PlayerTickInputEvent>(
            0, [this](GameEvent&, PlayerTickInputEvent* input) {
                if(input->origin == MapLoadEvent::Local)
                    m_local_inputs.push_back(*input);
            });
        m_game_bus.addEventFunction<ServerCameraControl>(
            0, [this](GameEvent&, ServerCameraControl* cam) {
                if(!is_server())
                    return;
                cDebug("Setting camera to {}", cam->target_player);
                m_pending_focus =
                    cam->target_player == 0xFFFF ? 0 : cam->target_player;
            });
        m_game_bus.addEventData(
            {0, [this](GameEvent& event, const void* data) {
                 /* Each function will check the type of the event,
                  * only the matching one will send
                  */
                 if(event.type == GameEvent::Tick ||
                    event.type == GameEvent::PlayerTickInput)
                     return;
                 if(is_server())
                 {
                     cDebug(
                         "Distributing {} event",
                         magic_enum::enum_name(event.type));
                     forward_game_event<ServerStateUpdate>(event, data);
                     forward_game_event<ServerPlayerStateUpdate>(event, data);
                     return;
                 } else if(is_client_network_event(event))
                 {
                     cDebug(
                         "Forwarding {} event",
                         magic_enum::enum_name(event.type));
                     forward_game_event<ServerCameraControl>(event, data);
                 } else
                 {
                     cDebug(
                         "Not forwarding {} event",
                         magic_enum::enum_name(event.type));
                 }
             }});
        m_game_bus.addEventFunction<SyncPointEvent>(
            0, [this](GameEvent&, SyncPointEvent* sync) {
                if(sync->released)
                    return;
                if(is_server())
                {
                    auto& point = m_syncs[sync->name];
                    point.self  = true;
                    point.peers = std::max(point.peers, sync->peers);
                    release_sync(sync->name);
                } else if(m_connection != k_HSteamNetConnection_Invalid)
                {
                    SyncPoint reached{};
                    sync->name.copy(reached.name.data(), reached.name.size() - 1);
                    send_single(m_connection, Message<SyncPoint>(std::move(reached)));
                } else
                {
                    GameEvent      ev{.type = GameEvent::SyncPoint};
                    SyncPointEvent done{.name = sync->name, .released = true};
                    m_game_bus.inject(ev, &done);
                }
            });
        m_game_bus.addEventFunction<MapDataLoadEvent>(
            0, [this](GameEvent&, MapDataLoadEvent*) {
                if(!is_server())
                {
                    send_single(
                        m_connection,
                        Message<GameLoadState>({
                            .progress = 20,
                        }));
                }
            });
        m_game_bus.addEventFunction<MapLoadFinishedEvent<halo_version>>(
            0,
            [this](GameEvent&, MapLoadFinishedEvent<halo_version>* finished) {
                m_map         = finished->container;
                m_combat.set_map(m_map);
                m_impact_shaders.clear();
                m_fingerprint = fingerprint_of(*m_map);
#if defined(USE_WEBRTC_TRANSPORT)
                publish_server_metadata();
#endif
                if(!is_server())
                    return;
                m_dynamic_spawns.clear();
                m_held_players.clear();
                m_release_at.reset();
                m_latest_cameras.clear();
                for(auto& [_, state] : m_connections)
                {
                    state.verified   = false;
                    state.census_due = {};
                    state.cameras_sent.clear();
                    state.objects_sent.clear();
                }
                for(auto& [connection, state] : m_connections)
                {
                    if(state.invited)
                    {
                        send_single(
                            connection,
                            Message<MapVerify>({
                                .fingerprint = m_fingerprint,
                            }));
                        continue;
                    }
                    send_single(connection, generate_game_join());
                    state.invited          = true;
                    state.loading_progress = 0;
                    state.last_seen        = std::nullopt;
                }
            });
        m_game_bus.addEventData({
            0,
            [this](GameEvent& ev, const void*) {
                if(ev.type != GameEvent::MapAllLoaded)
                    return;
                if(is_server())
                    m_needs_local_init = true;
                if(!is_server())
                {
                    send_single(
                        m_connection,
                        Message<GameLoadState>({
                            .progress = 100,
                        }));
                    send_single(
                        m_connection,
                        Message<MapVerify>({
                            .fingerprint = m_fingerprint,
                        }));
                    if(m_expected_fingerprint &&
                       *m_expected_fingerprint != m_fingerprint)
                        cWarning(
                            "Loaded map differs from the server's: {} != {}",
                            to_string(m_fingerprint),
                            to_string(*m_expected_fingerprint));
                }
            },
        });

        m_utils->SetDebugOutputFunction(
            k_ESteamNetworkingSocketsDebugOutputType_Everything,
            [](ESteamNetworkingSocketsDebugOutputType, const char* message) {
                cDebug("Networking: {}", message);
            });

#if defined(USE_WEBRTC_TRANSPORT)
        m_utils->SetGlobalConfigValuePtr(
            k_ESteamNetworkingConfig_Callback_ConnectionStatusChanged,
            function_to_void<SteamNetConnectionStatusChangedCallback_t>(
                [](SteamNetConnectionStatusChangedCallback_t* info) {
                    network_instance->connection_status_change(info);
                }));
        m_utils->SetGlobalConfigValueInt32(
            k_ESteamNetworkingConfig_TimeoutInitial, 60000);
#endif
    }

    std::vector<SteamNetworkingConfigValue_t> create_callbacks()
    {
        std::vector<SteamNetworkingConfigValue_t> config;
        config.emplace_back();
        config.back().SetPtr(
            k_ESteamNetworkingConfig_Callback_ConnectionStatusChanged,
            function_to_void<SteamNetConnectionStatusChangedCallback_t>(
                [](SteamNetConnectionStatusChangedCallback_t* info) {
                    network_instance->connection_status_change(info);
                }));
        return config;
    }

    /* Pin the server's public key for one outgoing connection, so GNS accepts
     * only a certificate that key signed. Deliberately not folded into
     * create_callbacks(): that is shared with the listen socket, and a server
     * pinning a root would demand certs from clients, which are anonymous. */
    void add_pinned_root_config(
        std::vector<SteamNetworkingConfigValue_t>& config)
    {
        if(m_client_auth.type != AuthType::Ed25519 ||
           m_client_auth.ed25519_public_key.empty())
            return;
        m_pinned_root_key_b64 = b64::encode(
            semantic::Span<const u8>(
                m_client_auth.ed25519_public_key.data(),
                m_client_auth.ed25519_public_key.size()));
        config.emplace_back();
        config.back().SetString(
            k_ESteamNetworkingConfig_PinnedRootCertPublicKey,
            m_pinned_root_key_b64.c_str());
        cDebug(
            "Requiring the server's certificate to be signed by the pinned "
            "key ({})",
            derive_identity_ed25519(m_client_auth.ed25519_public_key));
    }

    void connect_symmetric(std::string const& /*remote*/)
    {
        if(m_connection)
        {
            cWarning("Already connected to {}", remote_name());
            return;
        }

        m_utils->SetGlobalConfigValueInt32(
            k_ESteamNetworkingConfig_P2P_Transport_ICE_Enable,
            k_nSteamNetworkingConfig_P2P_Transport_ICE_Enable_All);
        m_utils->SetGlobalConfigValueString(
            k_ESteamNetworkingConfig_P2P_STUN_ServerList,
            "stun.l.google.com:19302");
        m_utils->SetGlobalConfigValueInt32(
            k_ESteamNetworkingConfig_LogLevel_P2PRendezvous,
            k_ESteamNetworkingSocketsDebugOutputType_Verbose);
        auto config = create_callbacks();
        config.emplace_back();
        config.back().SetInt32(k_ESteamNetworkingConfig_SymmetricConnect, 1);
        m_socket =
            m_impl->CreateListenSocketP2P(0, config.size(), config.data());
        m_poll_group = m_impl->CreatePollGroup();
    }

    void connect_server(std::string const& remote)
    {
        if(m_connection)
        {
            cWarning("Already connected to {}", remote_name());
            return;
        }

#if defined(USE_WEBRTC_TRANSPORT)
        /* Takes a value of the form:
         * <ws/wss>://<ip:port>%23<serverId>
         * In a URL it should look like server=ws://127.0.0.1:XXXX%23test
         * In order to avoid having the serverId interpreted as a fragment*/
        if(remote.starts_with("ws://") || remote.starts_with("wss://"))
        {
            connect_server_webrtc(remote);
            return;
        }
#endif

        m_utils->SetGlobalConfigValueString(
            k_ESteamNetworkingConfig_P2P_STUN_ServerList,
            "stun.l.google.com:19302");
        m_utils->SetGlobalConfigValueInt32(
            k_ESteamNetworkingConfig_P2P_Transport_ICE_Enable,
            k_nSteamNetworkingConfig_P2P_Transport_ICE_Enable_All);
        /* ip:port#auth=ed25519:<key>, as printed by the server */
        std::string address = remote;
        if(auto hash = remote.find('#'); hash != std::string::npos)
        {
            address   = remote.substr(0, hash);
            auto auth = parse_auth_param(remote.substr(hash + 1));
            if(auth.type != AuthType::Ed25519)
            {
                cWarning("Refusing to connect, bad server key in {}", remote);
                m_net_state.client_state = NetworkState::ClientState::Error;
                return;
            }
            m_client_auth = std::move(auth);
        }
        SteamNetworkingIPAddr server;
        if(!server.ParseString(address.c_str()))
        {
            cWarning("Failed to parse server IP: {}", address);
            m_net_state.client_state = NetworkState::ClientState::Error;
            return;
        }
        auto config = create_callbacks();
        add_pinned_root_config(config);
        m_connection =
            m_impl->ConnectByIPAddress(server, config.size(), config.data());
        if(m_connection == k_HSteamNetConnection_Invalid)
        {
            cWarning("Failed to set up connection to: {}", remote);
            m_net_state.client_state = NetworkState::ClientState::Error;
            return;
        }
        configure_weights(m_connection);
        SteamNetConnectionInfo_t connect_info;
        if(!m_impl->GetConnectionInfo(m_connection, &connect_info))
        {
            cWarning("Failed to get connection info");
            m_net_state.client_state = NetworkState::ClientState::Error;
            return;
        }
        m_connections[m_connection] = {};
        cDebug(
            "Connection info: state={}, remote={}",
            magic_enum::enum_name(connect_info.m_eState),
            remote_name());
        m_net_state.client_state = NetworkState::ClientState::Establishing;
    }

#if defined(USE_WEBRTC_TRANSPORT)
    void connect_server_webrtc(std::string const& gatewayUrl)
    {
        if(m_webrtcBootstrap)
        {
            cWarning("WebRTC gateway connection already in progress");
            return;
        }
        auto parsed = parse_webrtc_url(gatewayUrl);
        if(parsed.server_id.empty())
        {
            cWarning("Failed to parse gateway URL: {}", gatewayUrl);
            m_net_state.client_state = NetworkState::ClientState::Error;
            return;
        }
        m_client_auth = parsed.auth;
        if(m_client_auth.type == AuthType::Invalid)
        {
            cWarning("Refusing to connect, bad server key in {}", gatewayUrl);
            m_net_state.client_state = NetworkState::ClientState::Error;
            return;
        }
        cDebug(
            "Bootstrapping WebRTC DataChannel via gateway {} for serverId={} "
            "auth={}",
            parsed.gateway_url,
            parsed.server_id,
            m_client_auth.type == AuthType::Ed25519 ? "ed25519" : "none");
        m_utils->SetGlobalConfigValueInt32(
            k_ESteamNetworkingConfig_LogLevel_P2PRendezvous,
            k_ESteamNetworkingSocketsDebugOutputType_Verbose);
        m_webrtcBootstrap = new webrtc_signaling::GatewayConnectBootstrap(
            parsed.gateway_url, parsed.server_id, m_client_auth);
        m_webrtcBootstrap->Start();
        m_net_state.client_state = NetworkState::ClientState::Establishing;
    }

    /* Polls the async gateway bootstrap kicked off by connect_server_webrtc
     * (SDP/ICE + the WebSocket round trip take real wall-clock time, so
     * this can't be done synchronously inline there -- see
     * webrtc_signaling.h). Called once per tick from start_restricted,
     * regardless of m_socket/m_connection state, since neither is set up
     * yet during bootstrap. */
    void poll_webrtc_bootstrap()
    {
        if(!m_webrtcBootstrap)
            return;
        if(m_webrtcBootstrap->Failed())
        {
            cWarning("WebRTC gateway bootstrap failed");
            m_webrtcBootstrap->Release(); // deletes itself
            m_webrtcBootstrap        = nullptr;
            m_net_state.client_state = NetworkState::ClientState::Error;
            return;
        }
        if(!m_webrtcBootstrap->Ready())
            return;

        auto metadata = m_webrtcBootstrap->Metadata();
        if(m_client_auth.type != AuthType::None && !metadata.is_null())
        {
            bool verified = m_client_auth.type == AuthType::Ed25519 &&
                            verify_metadata_ed25519(
                                metadata, m_client_auth.ed25519_public_key);
            if(!verified)
            {
                cWarning("WebRTC server metadata verification failed");
                m_webrtcBootstrap->Release();
                m_webrtcBootstrap        = nullptr;
                m_net_state.client_state = NetworkState::ClientState::Error;
                return;
            }
            m_expected_server_identity = metadata.value("identity", "");
            cDebug(
                "WebRTC server metadata verified, identity={}",
                m_expected_server_identity);
        } else if(m_client_auth.type != AuthType::None)
        {
            cWarning(
                "WebRTC server did not provide signed metadata, but auth was "
                "expected");
            m_webrtcBootstrap->Release();
            m_webrtcBootstrap        = nullptr;
            m_net_state.client_state = NetworkState::ClientState::Error;
            return;
        }

        auto pc     = m_webrtcBootstrap->TakePeerConnection();
        auto dc     = m_webrtcBootstrap->TakeDataChannel();
        auto config = create_callbacks();
        /* Before the branch, so both the relayed-UDP and the P2P rendezvous
         * shapes are authenticated -- the gateway is on the key-exchange path
         * either way. */
        add_pinned_root_config(config);

        /* Which of the routes the server offers we can actually take */
        auto transports     = m_webrtcBootstrap->ServerTransports();
        bool serverIsWebrtc = m_webrtcBootstrap->ServerSupports("webrtc");
        bool canTryDirect   = false;
#if !defined(COFFEE_WASM)
        canTryDirect =
            !m_relay_only && m_webrtcBootstrap->ServerSupports("direct");
#endif
        /* Rendezvous covers both: reaching a DataChannel-only peer, and
         * reaching a socket directly. */
        bool useRendezvous = serverIsWebrtc || canTryDirect;
        cDebug(
            "Server offers [{}]; taking {}",
            fmt::join(transports, ", "),
            !useRendezvous ? "the gateway relay"
            : serverIsWebrtc
                ? "rendezvous to its DataChannel, through the gateway"
                : "the direct route, with the gateway relay as fallback");

        SteamNetworkingIdentity expected_identity;
        expected_identity.Clear();
        if(!m_expected_server_identity.empty())
            expected_identity.SetGenericString(
                m_expected_server_identity.c_str());

        if(!useRendezvous)
        {
            m_webrtcDirectKeepAlive = m_webrtcBootstrap;
            m_webrtcBootstrap       = nullptr;
            SteamNetworkingIPAddr placeholder;
            placeholder.Clear();
            placeholder.SetIPv4(0x7f000001 /* 127.0.0.1 */, 1);
            m_connection = m_impl->ConnectUDPWebRTCDataChannel(
                placeholder, pc, dc, config.size(), config.data());
        } else
        {
#if !defined(COFFEE_WASM)
            /* Enable STUN only when not requested to use relays */
            if(!m_relay_only)
                m_utils->SetGlobalConfigValueString(
                    k_ESteamNetworkingConfig_P2P_STUN_ServerList,
                    "stun.l.google.com:19302");
            config.emplace_back();
            config.back().SetInt32(
                k_ESteamNetworkingConfig_P2P_Transport_ICE_Enable,
                m_relay_only
                    ? k_nSteamNetworkingConfig_P2P_Transport_ICE_Enable_Disable
                    : k_nSteamNetworkingConfig_P2P_Transport_ICE_Enable_All);
#endif
            config.emplace_back();
            config.back().SetInt32(
                k_ESteamNetworkingConfig_TimeoutInitial, 180000);
            auto* bootstrap   = m_webrtcBootstrap;
            m_webrtcBootstrap = nullptr;
            m_connection      = m_impl->ConnectP2PWebRTCDataChannel(
                bootstrap,
                expected_identity.IsInvalid() ? nullptr : &expected_identity,
                0,
                pc,
                dc,
                config.size(),
                config.data());
        }
        if(m_connection == k_HSteamNetConnection_Invalid)
        {
            cWarning("Failed to establish WebRTC DataChannel connection");
            m_net_state.client_state = NetworkState::ClientState::Error;
            return;
        }
        configure_weights(m_connection);
        m_connections[m_connection] = {};
        cDebug(
            "WebRTC gateway connection established, connection={}",
            m_connection);
    }
#endif

    void create_server(std::string const& local)
    {
        if(m_socket)
        {
            cWarning("Server already started");
            return;
        }

        auto                  config = create_callbacks();
        SteamNetworkingIPAddr server;
        if(!server.ParseString(local.c_str()))
        {
            cWarning("Failed to parse server IP: {}", local);
            m_net_state.server_state = NetworkState::ServerState::Error;
            return;
        }
        ensure_server_identity();
        m_socket =
            m_impl->CreateListenSocketIP(server, config.size(), config.data());
        if(m_socket == k_HSteamListenSocket_Invalid)
        {
            cWarning("Failed to start listening on {}", local);
            m_net_state.server_state = NetworkState::ServerState::Error;
            return;
        }
        m_poll_group = m_impl->CreatePollGroup();
        m_host_name  = get_random_name();
        cDebug("Started server on {} as {}", local, m_host_name);
        // TODO: We could advertise join info with Discord?
        m_net_state.local_address = local_name();
        m_net_state.server_state  = NetworkState::ServerState::Listening;
    }

#if defined(USE_WEBRTC_TRANSPORT)
    void close_relayed_connections(
        std::vector<SteamNetworkingIPAddr> const& relays)
    {
        for(auto const& relay : relays)
        {
            std::vector<HSteamNetConnection> stale;
            for(auto const& [connection, _] : m_connections)
            {
                SteamNetConnectionInfo_t info;
                if(m_impl->GetConnectionInfo(connection, &info) &&
                   info.m_hListenSocket == m_socket &&
                   info.m_addrRemote == relay)
                    stale.push_back(connection);
            }
            for(auto connection : stale)
            {
                cDebug(
                    "Gateway relay to {} closed, dropping its connection",
                    client_name(connection));
                server_close_peer_connection(connection, 0, false);
            }
        }
    }
#endif

    void stop_server()
    {
        m_impl->CloseListenSocket(m_socket);
        m_impl->DestroyPollGroup(m_poll_group);
        m_session_start_us = 0;
        m_socket     = k_HSteamListenSocket_Invalid;
        m_poll_group = k_HSteamNetPollGroup_Invalid;
#if defined(USE_WEBRTC_TRANSPORT)
        m_webrtcServer.reset();
#endif
        m_net_state.server_state = NetworkState::ServerState::None;
        m_net_state.local_address.reset();
        m_net_state.join_string.reset();
    }

#if defined(USE_WEBRTC_TRANSPORT)
    /* A WebRTC-hosted server: no UDP listen socket at all, every client
     * arrives as a DataChannel the gateway bridges to one this side opens
     * for that session. local is <gatewayUrl>[#<serverId>], mirroring the
     * client's --server ws://gw#id; serverId also comes from
     * --gateway-server-id, which wins if both are given. */
    void create_server_webrtc(
        std::string const& local, std::string const& explicitServerId)
    {
        if(m_socket || m_webrtcServer)
        {
            cWarning("Server already started");
            return;
        }

        std::string gatewayUrl = local;
        std::string serverId   = explicitServerId;
        if(auto hash = local.find('#'); hash != std::string::npos)
        {
            gatewayUrl = local.substr(0, hash);
            if(serverId.empty())
                serverId = local.substr(hash + 1);
        }
        if(serverId.empty())
        {
            cWarning(
                "WebRTC-hosted server needs a server ID: pass "
                "--listen {}#<id> or --gateway-server-id <id>",
                gatewayUrl);
            return;
        }

        cDebug(
            "Starting WebRTC-hosted server: gateway={} serverId={}",
            gatewayUrl,
            serverId);
        /* Both ends have to offer ICE candidates for a direct UDP route to
         * exist at all; the client sets the same pair (see
         * connect_server_webrtc). When a native peer connects, that route
         * beats the relayed DataChannel and GNS switches to it, at which
         * point the relay is retired (pollDirectRouteTakeover). A browser
         * peer has no ICE of its own, so it just stays on the bridge. */
#if !defined(COFFEE_WASM)
        if(!m_relay_only)
            m_utils->SetGlobalConfigValueString(
                k_ESteamNetworkingConfig_P2P_STUN_ServerList,
                "stun.l.google.com:19302");
        m_utils->SetGlobalConfigValueInt32(
            k_ESteamNetworkingConfig_P2P_Transport_ICE_Enable,
            m_relay_only
                ? k_nSteamNetworkingConfig_P2P_Transport_ICE_Enable_Disable
                : k_nSteamNetworkingConfig_P2P_Transport_ICE_Enable_All);
#endif
        ensure_server_identity();
        m_webrtcServer =
            std::make_unique<webrtc_signaling::GatewayServerRegistration>(
                gatewayUrl, serverId, m_server_key, m_impl);
        m_webrtcServer->Start();
        // Same poll group create_server uses -- independent of any listen
        // socket (connections get attached to it individually via
        // SetConnectionPollGroup, see server_connection_status_changed's
        // Connecting case below), so it works identically here despite
        // there being no CreateListenSocketIP/P2P call in this path at all.
        m_poll_group = m_impl->CreatePollGroup();
        m_host_name  = get_random_name();
        // TODO: local_address isn't meaningful for a gateway-relayed
        // server the way it is for a real bound UDP socket (local_name()
        // reads GetListenSocketAddress, which m_socket doesn't have here)
        // -- leave unset until there's a sensible thing to show.
        m_net_state.server_state = NetworkState::ServerState::Listening;
        publish_join_string(gatewayUrl, serverId);
    }
#endif

    void connection_status_change(
        SteamNetConnectionStatusChangedCallback_t* info)
    {
        cDebug(
            "Network state change: {}: {} -> {}",
            info->m_hConn,
            magic_enum::enum_name(info->m_eOldState),
            magic_enum::enum_name(info->m_info.m_eState));

        bool isServer = m_socket && m_socket != k_HSteamListenSocket_Invalid;
#if defined(USE_WEBRTC_TRANSPORT)
        isServer = isServer || (bool)m_webrtcServer;
#endif
        if(isServer)
            server_connection_status_changed(info);
        if(m_connection && m_connection != k_HSteamNetConnection_Invalid)
            client_connection_status_changed(info);
    }

    void server_connection_status_changed(
        SteamNetConnectionStatusChangedCallback_t* info)
    {
        switch(info->m_info.m_eState)
        {
        case k_ESteamNetworkingConnectionState_Connecting: {
#if defined(USE_WEBRTC_TRANSPORT)
            if(m_webrtcServer)
            {
                if(!m_impl->SetConnectionPollGroup(info->m_hConn, m_poll_group))
                {
                    cWarning("Failed to set connection poll group");
                    break;
                }
                if(m_connections.size() >= 3)
                {
                    m_impl->CloseConnection(info->m_hConn, 10, nullptr, false);
                    break;
                }
                configure_weights(info->m_hConn);
                m_connections[info->m_hConn] = connection_state_t{
                    .idx = m_next_remote_idx++,
                };
                break;
            }
#endif
            if(m_impl->AcceptConnection(info->m_hConn) != k_EResultOK)
            {
                cWarning(
                    "Failed to accept incoming connection ({})",
                    client_name(info->m_hConn));
                m_impl->CloseConnection(info->m_hConn, 0, nullptr, false);
                break;
            }
            if(!m_impl->SetConnectionPollGroup(info->m_hConn, m_poll_group))
            {
                cWarning("Failed to set connection poll group");
                break;
            }
            if(m_connections.size() >= 3)
            {
                m_impl->CloseConnection(info->m_hConn, 10, nullptr, false);
                break;
            }
            configure_weights(info->m_hConn);
            m_connections[info->m_hConn] = connection_state_t{
                .idx = m_next_remote_idx++,
            };
            break;
        }
        case k_ESteamNetworkingConnectionState_Connected: {
            cDebug(
                "Connection to peer={} established ({})",
                info->m_hConn,
                client_name(info->m_hConn));
            journal(
                "net_peer_connected",
                {{"peer", client_name(info->m_hConn)},
                 {"player_idx", m_connections[info->m_hConn].idx},
                 {"map_ready", m_map != nullptr}});
#if defined(USE_WEBRTC_TRANSPORT)
            if(m_webrtcServer)
                m_webrtcServer->NotifyGNSConnected(info->m_hConn);
#endif
            /* If the server doesn't have an active map, don't send the
             * game invite yet — MapLoadFinishedEvent below catches this
             * connection up once m_map is set, so this isn't a permanent
             * miss. */
            if(!m_map)
                break;
            send_single(info->m_hConn, generate_game_join());
            m_connections[info->m_hConn].invited          = true;
            m_connections[info->m_hConn].loading_progress = 0;
            m_connections[info->m_hConn].last_seen        = std::nullopt;
            break;
        }

        case k_ESteamNetworkingConnectionState_ProblemDetectedLocally: {
            cDebug(
                "Problem detected with connection to {}",
                client_name(info->m_hConn));
            server_close_peer_connection(info->m_hConn, 0, false);
            break;
        }
        case k_ESteamNetworkingConnectionState_ClosedByPeer:
        case k_ESteamNetworkingConnectionState_Dead: {
            server_close_peer_connection(
                info->m_hConn,
                0,
                info->m_info.m_eState ==
                    k_ESteamNetworkingConnectionState_ClosedByPeer);
            break;
        }
        default:
            break;
        }
    }

    void client_connection_status_changed(
        SteamNetConnectionStatusChangedCallback_t* info)
    {
        switch(info->m_info.m_eState)
        {
        case k_ESteamNetworkingConnectionState_Connecting: {
            m_net_state.client_state = NetworkState::ClientState::Connecting;
            break;
        }
        case k_ESteamNetworkingConnectionState_Connected: {
            m_net_state.client_state   = NetworkState::ClientState::Connected;
            m_net_state.remote_address = remote_name();
            bool const authenticated =
                !(info->m_info.m_nFlags &
                  k_nSteamNetworkConnectionInfoFlags_Unauthenticated);
            cDebug(
                "Connection to server/peer established ({}, {})",
                remote_name(),
                authenticated ? "authenticated" : "unauthenticated");
            journal("net_connected", {{"server", remote_name()}});
            m_connection_last_seen = std::nullopt;
            m_clock = {.active = true, .burst = clock_burst_samples};
#if defined(USE_WEBRTC_TRANSPORT)
            if(!m_expected_server_identity.empty())
            {
                std::string remote_identity;
                if(!info->m_info.m_identityRemote.IsInvalid())
                    remote_identity =
                        info->m_info.m_identityRemote.GetGenericString();
                if(remote_identity != m_expected_server_identity)
                {
                    cWarning(
                        "WebRTC server identity mismatch: expected {}, got {}",
                        m_expected_server_identity,
                        remote_identity.empty() ? "<invalid>"
                                                : remote_identity);
                    m_impl->CloseConnection(
                        info->m_hConn, 0, "identity mismatch", false);
                    m_net_state.client_state = NetworkState::ClientState::Error;
                    return;
                }
                cDebug(
                    "WebRTC server identity verified: {}",
                    m_expected_server_identity);
            }
#endif
            break;
        }
        case k_ESteamNetworkingConnectionState_ProblemDetectedLocally:
            if(info->m_eOldState != k_ESteamNetworkingConnectionState_Connected)
            {
                /* Never got in, e.g. a certificate the pinned key did not
                 * sign. GNS holds the connection until it is closed. */
                cWarning(
                    "Failed to connect to server {}: {}",
                    client_name(info->m_hConn),
                    info->m_info.m_szEndDebug);
                m_net_state.error        = info->m_info.m_szEndDebug;
                m_net_state.client_state = NetworkState::ClientState::Error;
                m_impl->CloseConnection(info->m_hConn, 0, nullptr, false);
                m_connections.erase(info->m_hConn);
                m_connection = {};
                server_gone(
                    ServerDisconnectedEvent::join_failed,
                    info->m_info.m_szEndDebug);
                break;
            }
            /* GNS has given up on it; nothing else closes the handle */
            cWarning(
                "Lost connection to server {}: {}",
                client_name(info->m_hConn),
                info->m_info.m_szEndDebug);
            close_server_connection(info->m_hConn, false);
            server_gone(
                ServerDisconnectedEvent::connection_lost,
                info->m_info.m_szEndDebug);
            break;
        case k_ESteamNetworkingConnectionState_ClosedByPeer:
        case k_ESteamNetworkingConnectionState_Dead: {
            auto const reason = info->m_info.m_eEndReason;
            bool const by_app = reason >= k_ESteamNetConnectionEnd_App_Min &&
                                reason <= k_ESteamNetConnectionEnd_App_Max;
            cDebug(
                "Disconnected from server/peer ({}): {}",
                remote_name(),
                info->m_info.m_szEndDebug);
            close_server_connection(
                info->m_hConn,
                info->m_info.m_eState ==
                    k_ESteamNetworkingConnectionState_ClosedByPeer);
            server_gone(
                by_app ? ServerDisconnectedEvent::host_left
                       : ServerDisconnectedEvent::closed,
                info->m_info.m_szEndDebug);
            break;
        }
        case k_ESteamNetworkingConnectionState_None: {
            m_net_state.client_state = NetworkState::ClientState::None;
            break;
        }
        default:
            break;
        }
    }

    void configure_weights(HSteamNetConnection connection)
    {
        std::array<int, 2> priorities = {{0, 2}};
        std::array<u16, 2> weights    = {{0, 10}};
        m_impl->ConfigureConnectionLanes(
            connection, priorities.size(), priorities.data(), weights.data());
    }

    template<typename T>
    void send_all(
        MessageBase&&                 data,
        gsl::span<T> const&           multi = {},
        int                           flags = k_nSteamNetworkingSend_Reliable,
        std::set<HSteamNetConnection> connections = {},
        u16                           lane        = 0)
    {
        if(m_connections.empty() || multi.empty())
            return;

        // TODO: Add data size validation

        auto const payload_size =
            sizeof(MessageBase) + multi.size() * sizeof(T);
        char* payload_buf = new char[payload_size];

        auto* header       = reinterpret_cast<MessageBase*>(payload_buf);
        *header            = *static_cast<MessageBase*>(&data);
        header->num_values = multi.size();
        header->flags |=
            multi.size() > 1 ? MessageBase::Multiple : MessageBase::NoFlags;

        auto* values = reinterpret_cast<T*>(payload_buf + sizeof(MessageBase));
        memcpy(values, multi.data(), multi.size_bytes());

        std::vector<SteamNetworkingMessage_t*> messages;
        for(auto const& [connection, info] : m_connections)
        {
            if(!connections.empty() && !connections.contains(connection))
                continue;
            messages.push_back(m_utils->AllocateMessage(0));
            auto& message      = messages.back();
            message->m_pData   = payload_buf;
            message->m_cbSize  = payload_size;
            message->m_conn    = connection;
            message->m_nFlags  = flags;
            message->m_idxLane = lane;
        }
        if(messages.empty())
        {
            cWarning("Was supposed to send message, but none generated");
            return;
        }
        messages.back()->m_pfnFreeData = [](SteamNetworkingMessage_t* msg) {
            delete static_cast<char*>(msg->m_pData);
        };
        /* bDeleteFailedMessages: added in GNS 1.6.0; true keeps the
         * previous behaviour here. */
        m_impl->SendMessages(
            messages.size(),
            messages.data(),
            nullptr,
            /*bDeleteFailedMessages*/ true);
    }

    template<typename T>
    void send_all(
        Message<T>&&                  data,
        int                           flags = k_nSteamNetworkingSend_Reliable,
        std::set<HSteamNetConnection> connections = {},
        u16                           lane        = 0)
    {
        return send_all<T>(
            static_cast<MessageBase&&>(data),
            gsl::span<T>(&data.data, 1),
            flags,
            connections,
            lane);
    }

    /* Once the server and every client it has (and at least as many as
     * asked) have reached it, everyone goes on */
    void release_sync(std::string const& name)
    {
        auto point = m_syncs.find(name);
        if(point == m_syncs.end() || !point->second.self)
            return;
        std::set<HSteamNetConnection> verified;
        for(auto const& [connection, state] : m_connections)
            if(state.verified)
                verified.insert(connection);
        if(verified.size() < point->second.peers)
            return;
        for(auto connection : verified)
            if(!point->second.reached.contains(connection))
                return;
        SyncPoint released{.released = 1};
        name.copy(released.name.data(), released.name.size() - 1);
        for(auto connection : verified)
            send_single(connection, Message<SyncPoint>(SyncPoint(released)));
        m_syncs.erase(point);
        cDebug("Sync point \"{}\" released for {} client(s)", name, verified.size());
        GameEvent      ev{.type = GameEvent::SyncPoint};
        SyncPointEvent done{.name = name, .released = true};
        m_game_bus.inject(ev, &done);
    }

    template<typename T>
    void send_single(
        HSteamNetConnection connection,
        Message<T>&&        data,
        int                 flags = k_nSteamNetworkingSend_Reliable)
    {
        return send_all(std::move(data), flags, {connection});
    }

    void start_restricted(Proxy& p, time_point const& t)
    {
#if defined(USE_WEBRTC_TRANSPORT)
        poll_webrtc_bootstrap();
        if(m_webrtcServer)
            m_webrtcServer->PollPendingAccepts();
        if(m_fleetRegistration)
        {
            m_fleetRegistration->Poll();
            close_relayed_connections(m_fleetRegistration->TakeClosedRelays());
        }
#endif
        if(m_spawn_queue)
            m_spawn_queue->poll();
        if(m_despawn_queue)
            m_despawn_queue->poll();
        if(m_pickup_queue)
            m_pickup_queue->poll();
        m_net_state.replica = !is_server() && m_connection;
        send_combat(p);
        apply_simulation();
        clock_tick();
        session_tick();
        if(is_server())
        {
#if defined(USE_WEBRTC_TRANSPORT)
            publish_server_metadata();
#endif
            /* Local seats join the roster as they become active */
            bool local_joined = false;
            for(auto player : p.select<PlayerInfo, PlayerCamera>())
            {
                auto [info, cam] = player.components();
                if(info.is_remote() || !cam.is_active() ||
                   is_networked(player.id(), info))
                    continue;

                info.name =
                    (info.seat_idx == 0) ? m_host_name : get_random_name();
                m_local_player_info.push_back(player.ref<PlayerInfo>());
                if(auto* net = p.get<NetworkInfo>(player.id()))
                    net->changes.viewport = true;
                local_joined = true;
            }
            if(local_joined)
                send_player_roster();

            int                       num_msgs = -1;
            SteamNetworkingMessage_t* message  = nullptr;
            while(true)
            {
                num_msgs = m_impl->ReceiveMessagesOnPollGroup(
                    m_poll_group, &message, 1);
                if(num_msgs < 1)
                    break;
                auto const& payload =
                    *reinterpret_cast<MessageBase const*>(message->GetData());
                if(payload.type == MessageBase::ClockSync)
                {
                    clock_reply(
                        message->m_conn,
                        payload.value<ClockSync>(),
                        message->m_usecTimeReceived);
                    message->Release();
                    continue;
                }
                server_receive_payload(p, message->m_conn, payload);
                message->Release();
            }

            if(m_needs_local_init && !m_local_player_info.empty())
            {
                m_needs_local_init = false;
                for(auto& pi : m_local_player_info)
                {
                    if(pi.exists())
                        player_init(p.unconstrained_container(), *pi);
                }
            }

            /* A player who isn't ready yet restarts the countdown */
            if(!m_held_players.empty())
            {
                if(!players_ready())
                    m_release_at.reset();
                else if(!m_release_at)
                    m_release_at = t + spawn_hold;
                else if(t >= *m_release_at)
                    release_held_players(p);
            }

            using namespace std::chrono_literals;

            // Look over connections and find timed out ones
            for(auto& [connection, player_info] : m_connections)
            {
                if(!player_info.last_seen || (t - *player_info.last_seen) < 30s)
                    continue;
                server_close_peer_connection(connection, 0, false);
            }

            /* Process pending focus change (swap seat_idx) */
            if(m_pending_focus.has_value())
            {
                u32 target_pidx = *m_pending_focus;
                m_pending_focus.reset();
                PlayerInfo*   old_seat0     = nullptr;
                PlayerInfo*   target        = nullptr;
                PlayerCamera* old_seat0_cam = nullptr;
                PlayerCamera* target_cam    = nullptr;
                u64           old_seat0_id = 0, target_id = 0;
                for(auto entity : p.select<PlayerInfo, PlayerCamera>())
                {
                    auto [info, cam] = entity.components();
                    if(info.seat_idx == 0)
                    {
                        old_seat0     = &info;
                        old_seat0_cam = &cam;
                        old_seat0_id  = entity.id();
                    }
                    if(info.player_idx == target_pidx)
                    {
                        target     = &info;
                        target_cam = &cam;
                        target_id  = entity.id();
                    }
                }
                if(old_seat0 && target && old_seat0 != target)
                {
                    std::swap(old_seat0->seat_idx, target->seat_idx);
                    if(old_seat0_cam)
                        old_seat0_cam->keyboard.enabled = false;
                    if(target_cam)
                        target_cam->keyboard.enabled = true;
                }
            }

            /* Sync dirty player components to network */
            for(auto entity : p.select<PlayerInfo, PlayerCamera, NetworkInfo>())
            {
                auto& info = entity.get<PlayerInfo>();
                auto& cam  = entity.get<PlayerCamera>();
                auto& net  = entity.get<NetworkInfo>();
                if(!is_networked(entity.id(), info))
                    continue;
                if(!info.is_remote() && info.mode.physics != net.sent_physics)
                    net.changes.viewport = true;

                // Server-enforced viewport/transform permissions
                auto send_permissions = [&] {
                    if(!net.changes.permissions || !info.is_remote())
                        return;
                    for(auto& [conn, state] : m_connections)
                    {
                        if(!state.own(info.player_idx))
                            continue;
                        send_permission(
                            conn,
                            info.player_idx,
                            UpdatePermission::Camera,
                            info.permissions.camera ? 1 : 0);
                        break;
                    }
                    net.changes.permissions = false;
                };

                // Server-pushed transform/viewport
                // Used for eg. birds-eye-view while locking viewport
                // TODO: At some point replace broadcast with check for whether
                // it's relevant for the player
                auto send_viewport = [&] {
                    if(!net.changes.viewport)
                        return;
                    if(!camera_moved(entity.id(), cam) &&
                       info.mode.physics == net.sent_physics)
                    {
                        net.changes.viewport = net.changes.transform = false;
                        return;
                    }
                    auto const sync = camera_sync_of(p, entity.id(), info, cam, tick_now(p));
                    /* Our own seats go out as send_streams schedules them;
                     * a remote player we moved is told, and everyone, at once */
                    if(info.is_remote())
                        send_all(Message<CameraSync>(CameraSync(sync)));
                    else
                        m_latest_cameras[info.player_idx] = sync;
                    net.changes.viewport = net.changes.transform = false;
                    net.sent_physics     = info.mode.physics;
                };

                /* Lock before moving to birds-eye, move to spawn before
                 * unlocking, so a body never drags the camera along */
                if(info.permissions.camera)
                {
                    send_viewport();
                    send_permissions();
                } else
                {
                    send_permissions();
                    send_viewport();
                }
            }
        }
        if(is_server())
        {
            send_impacts(p);
            send_seats(p);
            send_riders(p);
            send_equipment(p);
            send_vitality(p);
            send_game_state(p);
            send_devices(p);
            send_pickups();
            send_census(p);
            measure_pings();
            send_tick_sync(p, t);
            send_grab_states(p);
            send_object_sync(p, t);
        }
        equip_remote_players(p);
        /* Riders whose units had not arrived yet */
        std::erase_if(m_waiting_riders, [&](auto& waiting) {
            return seat_rider(p, waiting.first) || ++waiting.second > max_rider_wait;
        });
        if(m_left_server)
            leave_server(p);
        if(m_connection)
        {
            if(m_join_confirmed)
            {
                join_seats(p);
                for(auto player : p.select<PlayerInfo, PlayerCamera, NetworkInfo>())
                {
                    auto [info, cam, net] = player.components();
                    if(joined(info))
                        send_own(p, player.id(), info, cam, net);
                }
                send_inputs(p);
                if(auto now = std::chrono::steady_clock::now();
                   now - m_stats_journaled >= stats_period)
                {
                    m_stats_journaled = now;
                    SteamNetConnectionRealTimeStatus_t status{};
                    if(m_impl->GetConnectionRealTimeStatus(m_connection, &status, 0, nullptr) ==
                       k_EResultOK)
                        journal_stats(status, CameraSync::self_id);
                }
            }

            int                       num_msgs = -1;
            SteamNetworkingMessage_t* message  = nullptr;
            while(true)
            {
                num_msgs = m_impl->ReceiveMessagesOnConnection(
                    m_connection, &message, 1);
                if(num_msgs < 1)
                    break;
                auto const& payload =
                    *reinterpret_cast<MessageBase const*>(message->GetData());
                if(payload.type == MessageBase::ClockSync)
                {
                    clock_sample(
                        payload.value<ClockSync>(), message->m_usecTimeReceived);
                    message->Release();
                    continue;
                }
                client_receive_payload(p, payload);
                message->Release();
            }
            /* Others are shown where they were a few ticks ago */
            u64 const tick = tick_now(p);
            m_combat.viewing(tick > interp_ticks ? tick - interp_ticks : 0);
        }
#if defined(USE_WEBRTC_TRANSPORT) && defined(COFFEE_WASM)
        SteamNetworkingSockets_Poll(0);
#endif
        m_impl->RunCallbacks();
    }

    static u64 tick_now(Proxy& p)
    {
        return p.subsystem<TickClock>().tick();
    }

    /*! A local seat the server has given a player */
    bool joined(PlayerInfo const& info) const
    {
        if(info.is_remote())
            return false;
        auto const seat = m_net_state.remote_seats.find(info.seat_idx);
        return seat != m_net_state.remote_seats.end() && seat->second == info.player_idx;
    }

    /*! Whether `player_idx` is one of this client's joined seats */
    bool own_idx(u32 player_idx) const
    {
        return std::any_of(
            m_net_state.remote_seats.begin(),
            m_net_state.remote_seats.end(),
            [player_idx](auto const& seat) { return seat.second == player_idx; });
    }

    /*! The joined local seat playing `player_idx`, 0 if none */
    u64 own_player(Proxy& p, u32 player_idx) const
    {
        for(auto entity : p.select<PlayerInfo>())
            if(auto const& info = entity.get<PlayerInfo>();
               joined(info) && info.player_idx == player_idx)
                return entity.id();
        return 0;
    }

    /* Seats that become active join as players of their own */
    void join_seats(Proxy& p)
    {
        for(auto player : p.select<PlayerInfo, PlayerCamera>())
        {
            auto [info, cam] = player.components();
            if(info.is_remote() || !cam.is_active() || info.seat_idx >= max_client_seats ||
               m_net_state.remote_seats.contains(info.seat_idx) ||
               !m_pending_seats.insert(info.seat_idx).second)
                continue;
            if(info.name.empty())
                info.name = get_random_name();
            send_single(
                m_connection,
                Message<PlayerJoin>({
                    .player_name = *blam::bl_string::from(info.name),
                    .seat        = info.seat_idx,
                }));
        }
    }

    /* What the server hears of one of our seats, as it changes */
    void send_own(
        Proxy& p, u64 entity, PlayerInfo const& info, PlayerCamera const& camera, NetworkInfo& net)
    {
        auto& sent = m_own_seats[entity];
        bool const mode  = info.mode.physics != net.sent_physics;
        bool const moved = (net.changes.viewport || net.changes.transform) && camera_moved(entity, camera);
        /* Every frame it moves, so one lost is soon replaced; where it comes
         * to rest, or a change of mode, is not */
        bool const settled = !moved && std::exchange(sent.moving, moved);
        sent.moving        = moved;
        if(mode || moved || settled)
        {
            send_single(
                m_connection,
                Message<CameraSync>(camera_sync_of(p, entity, info, camera, tick_now(p))),
                mode || settled ? k_nSteamNetworkingSend_Reliable
                                : k_nSteamNetworkingSend_UnreliableNoNagle);
            net.changes.viewport = net.changes.transform = false;
            net.sent_physics     = info.mode.physics;
        }
        if(SeatSync const seat = seat_of(p, info); sent.seat != seat)
        {
            send_single(m_connection, Message<SeatSync>(SeatSync(seat)));
            sent.seat = seat;
        }
        if(EquipmentSync const held = equipment_of(p, entity, info);
           held.weapon != blam::tag_index<>{} && sent.equipment != held)
        {
            send_single(m_connection, Message<EquipmentSync>(EquipmentSync(held)));
            sent.equipment = held;
        }
        send_grab(p, entity, info, camera, sent.grab);
        send_driven(p, info);
        /* The server simulates the vehicle while nobody here drives it; the
         * driver's input still reaches it */
        if(auto const* input = p.get<PlayerInput>(entity);
           input && info.riding.driver && !info.riding.exiting)
            if(auto const* vehicle = p.get<NetworkInfo>(info.riding.vehicle);
               vehicle && vehicle->instance_id != 0)
                send_all(
                    Message<DriveInput>({
                        .aim      = Vecf4(camera.camera_.cached.forward, 0),
                        .vehicle  = vehicle->instance_id,
                        .throttle = input->intent.throttle,
                        .steer    = input->intent.steer,
                    }),
                    k_nSteamNetworkingSend_UnreliableNoNagle,
                    {m_connection},
                    FRAME_UPDATE_LANE);
    }

    /* The vehicle this client drives is simulated here, so it answers the
     * stick at once; the server follows the state it sends, each tick */
    void send_driven(Proxy& p, PlayerInfo const& info)
    {
        if(!info.riding.driver || info.riding.exiting)
            return;
        auto const* net     = p.get<NetworkInfo>(info.riding.vehicle);
        auto const* model   = p.get<Model>(info.riding.vehicle);
        auto const* physics = p.get<ObjectPhysics>(info.riding.vehicle);
        u64 const   tick    = tick_now(p);
        if(!net || net->instance_id == 0 || !model || !physics ||
           physics->authority != ObjectPhysics::Simulated ||
           std::exchange(m_driven_ticks[info.riding.vehicle], tick) == tick)
            return;
        send_all(
            Message<ObjectSync>({
                .tick             = tick,
                .rotation         = model->rotation,
                .position         = Vecf4(model->position, 0),
                .linear_velocity  = Vecf4(physics->linear_velocity, 0),
                .angular_velocity = Vecf4(physics->angular_velocity, 0),
                .net_id           = net->instance_id,
            }),
            k_nSteamNetworkingSend_UnreliableNoNagle,
            {m_connection},
            FRAME_UPDATE_LANE);
    }

    /* Shots and hits, where CombatSync says they go */
    void send_combat(Proxy& p)
    {
        m_combat.collect(p, is_server(), m_connection && m_join_confirmed);
        auto route = [this](net_combat::route_t const& route) {
            std::set<HSteamNetConnection> targets;
            if(route.to == net_combat::route_t::server)
                targets.insert(m_connection);
            else
                targets = verified_connections();
            if(route.except)
                targets.erase(*route.except);
            return targets;
        };
        for(auto& [message, to] : m_combat.take_shots())
            if(auto targets = route(to); !targets.empty())
                send_all(
                    Message<WeaponFire>(std::move(message)),
                    k_nSteamNetworkingSend_Reliable,
                    std::move(targets));
        for(auto& [message, to] : m_combat.take_hits())
            if(auto targets = route(to); !targets.empty())
                send_all(
                    Message<ObjectHit>(std::move(message)),
                    k_nSteamNetworkingSend_Reliable,
                    std::move(targets));
        for(auto& [message, to] : m_combat.take_vitality())
            if(auto targets = route(to); !targets.empty())
                send_all(
                    Message<UnitVitality>(std::move(message)),
                    k_nSteamNetworkingSend_Reliable,
                    std::move(targets));
    }

    /* The game's state and scores as they change, and all of it to a fresh
     * peer */
    void send_game_state(Proxy& p)
    {
        auto const&   game = p.subsystem<GameState>();
        GameStateSync state{
            .kind       = static_cast<libc_types::u8>(game.kind),
            .phase      = static_cast<libc_types::u8>(game.phase),
            .game_type  = static_cast<libc_types::u8>(game.game_type),
            .teams      = game.teams,
            .winner     = game.winner,
            .time_left  = game.time_left < 0.f ? -1.f : std::ceil(game.time_left),
            .team_scores = {game.team_scores[0], game.team_scores[1]},
            .difficulty = game.difficulty,
            .saves      = game.saves,
            .reverts    = game.reverts,
        };
        std::vector<GameScoreSync> scores;
        for(auto const& player : game.players)
            scores.push_back({
                .player_idx = player.player_idx,
                .team       = player.team,
                .score      = player.score,
                .kills      = player.kills,
                .deaths     = player.deaths,
            });
        std::set<HSteamNetConnection> fresh, current;
        for(auto& [connection, connection_state] : m_connections)
            if(connection_state.verified)
                (connection_state.objects_synced ? current : fresh).insert(connection);
        bool const changed = state != m_sent_game || scores != m_sent_scores;
        m_sent_game        = state;
        m_sent_scores      = scores;
        auto send = [&](std::set<HSteamNetConnection> const& to) {
            if(to.empty())
                return;
            send_all(Message<GameStateSync>(GameStateSync(state)), k_nSteamNetworkingSend_Reliable, to);
            if(!scores.empty())
                send_all<GameScoreSync>(
                    MessageBase{MessageBase::GameScoreSync},
                    gsl::span<GameScoreSync>(scores),
                    k_nSteamNetworkingSend_Reliable,
                    to);
        };
        send(fresh);
        if(changed)
            send(current);
    }

    /* A fresh peer hears of every unit already hurt or dead */
    void send_vitality(Proxy& p)
    {
        std::set<HSteamNetConnection> fresh;
        for(auto& [connection, state] : m_connections)
            if(state.verified && !state.objects_synced)
                fresh.insert(connection);
        if(fresh.empty())
            return;
        std::vector<UnitVitality> hurt;
        for(auto unit : p.select<BipedStatus>())
        {
            auto const& status = unit.get<BipedStatus>();
            if(status.dead || status.body < 1.f || status.shield != 1.f)
                if(auto wire = m_combat.vitality_of(p, unit.id()))
                    hurt.push_back(*wire);
        }
        for(auto& wire : hurt)
            send_all(Message<UnitVitality>(std::move(wire)), k_nSteamNetworkingSend_Reliable, fresh);
    }

    /* Our seat's input, a few ticks of it in each packet */
    void send_inputs(Proxy& p)
    {
        if(!m_input_queue)
            return;
        m_input_queue->poll();
        /* Every new tick goes out, with the last few before it again */
        for(auto& [_, sent] : m_own_seats)
            while(sent.inputs.size() > input_redundancy)
                sent.inputs.pop_front();
        bool fresh = false;
        for(auto const& input : std::exchange(m_local_inputs, {}))
        {
            auto const* info = p.get<PlayerInfo>(input.player);
            if(!info || !joined(*info))
                continue;
            m_own_seats[input.player].inputs.push_back({
                .tick       = input.tick,
                .view       = input.view,
                .move       = input.move,
                .look       = input.look,
                .intents    = input.intents,
                .flags      = input.resync ? TickInput::flags_t::resync
                                           : TickInput::flags_t::none,
                .player_idx = info->player_idx,
            });
            fresh = true;
        }
        if(!fresh)
            return;
        std::vector<TickInput> batch;
        for(auto const& [_, sent] : m_own_seats)
            batch.insert(batch.end(), sent.inputs.begin(), sent.inputs.end());
        send_all<TickInput>(
            MessageBase{MessageBase::TickInput},
            gsl::span<TickInput>(batch),
            k_nSteamNetworkingSend_UnreliableNoNagle,
            {m_connection},
            FRAME_UPDATE_LANE);
    }

    /* A client's inputs, each tick once; the gaps redundancy didn't cover */
    void receive_inputs(
        HSteamNetConnection connection, gsl::span<const TickInput> inputs)
    {
        auto& state = m_connections[connection];
        for(auto const& input : inputs)
        {
            auto* player = state.own(input.player_idx);
            if(!player || input.tick <= player->last_input)
                continue;
            /* A client that skipped ahead had no ticks to send */
            if(player->last_input != 0 &&
               (input.flags & TickInput::flags_t::resync) == TickInput::flags_t::none)
                state.inputs_lost += input.tick - player->last_input - 1;
            player->last_input = input.tick;
            state.inputs++;
            GameEvent            ev{.type = GameEvent::PlayerTickInput};
            PlayerTickInputEvent data{
                .origin     = MapLoadEvent::Remote,
                .player     = player->biped.id(),
                .player_idx = player->idx,
                .tick       = input.tick,
                .move       = input.move,
                .look       = input.look,
                .view       = input.view,
                .intents    = input.intents,
            };
            m_game_bus.inject(ev, &data);
        }
    }

    void measure_pings()
    {
        u32  worst  = 0;
        auto now    = std::chrono::steady_clock::now();
        bool report = now - m_stats_journaled >= stats_period;
        if(report)
            m_stats_journaled = now;
        for(auto& [connection, state] : m_connections)
        {
            SteamNetConnectionRealTimeStatus_t status{};
            if(m_impl->GetConnectionRealTimeStatus(connection, &status, 0, nullptr) !=
               k_EResultOK)
                continue;
            worst = std::max(worst, static_cast<u32>(std::max(status.m_nPing, 0)));
            if(report)
            {
                journal_stats(status, state.idx);
                journal(
                    "net_streams",
                    {{"player_idx", state.idx},
                     {"sent", std::exchange(state.streams_sent, 0)},
                     {"held", std::exchange(state.streams_held, 0)},
                     {"over_budget", std::exchange(state.streams_over_budget, 0)}});
            }
        }
        m_net_state.worst_ping_ms = worst;
    }

    /* What a connection carries, for tests to compare traffic by */
    void journal_stats(SteamNetConnectionRealTimeStatus_t const& status, u32 player_idx)
    {
        journal(
            "net_stats",
            {{"player_idx", player_idx},
             {"ping_ms", status.m_nPing},
             {"out_bytes_per_sec", status.m_flOutBytesPerSec},
             {"in_bytes_per_sec", status.m_flInBytesPerSec},
             {"out_packets_per_sec", status.m_flOutPacketsPerSec},
             {"pending_reliable", status.m_cbPendingReliable},
             {"pending_unreliable", status.m_cbPendingUnreliable},
             {"sent_unacked_reliable", status.m_cbSentUnackedReliable},
             {"queue_time_us", status.m_usecQueueTime}});
    }

    /* Impacts the server heard, for clients to hear too */
    void send_impacts(Proxy& p)
    {
        if(!m_impact_queue)
            return;
        m_impact_queue->poll();
        std::vector<ObjectImpact> impacts;
        for(auto const& impact : std::exchange(m_impacts, {}))
        {
            auto const* net   = p.get<NetworkInfo>(impact.entity_id);
            auto const* other = impact.other ? p.get<NetworkInfo>(impact.other)
                                             : nullptr;
            if(!net || net->instance_id == 0)
                continue;
            impacts.push_back({
                .point  = Vecf4(impact.point, 1.f),
                .object = net->instance_id,
                .other  = other ? other->instance_id : 0,
                .shader = impact.shader ? impact.shader->tag_id : blam::tag_index<>{},
                .speed  = impact.speed,
            });
        }
        if(!impacts.empty())
            journal("net_impacts", {{"count", impacts.size()}});
        if(auto targets = verified_connections(); !targets.empty() && !impacts.empty())
            send_all<ObjectImpact>(
                MessageBase{MessageBase::ObjectImpact},
                gsl::span<ObjectImpact>(impacts),
                k_nSteamNetworkingSend_UnreliableNoNagle,
                std::move(targets),
                FRAME_UPDATE_LANE);
    }

    /* An impact the server heard, on our copies of what it names */
    void receive_impacts(Proxy& p, gsl::span<const ObjectImpact> impacts)
    {
        if(!m_map)
            return;
        blam::tag_index_view<halo_version> index(*m_map);
        journal("net_impacts", {{"count", impacts.size()}});
        for(auto const& wire : impacts)
        {
            u64 const object = entity_of(p, wire.object);
            if(object == 0)
                continue;
            /* Sound lookups take the shader by reference, so keep one */
            blam::tagref_t const* shader = nullptr;
            if(auto tag = wire.shader != blam::tag_index<>{} ? index.find(wire.shader)
                                                             : index.end();
               tag != index.end() && (*tag).valid())
                shader = &m_impact_shaders.try_emplace(wire.shader, (*tag).as_ref())
                              .first->second;
            Physics::Event  ev{Physics::Event::Impact};
            Physics::Impact impact{
                .entity_id = object,
                .other     = entity_of(p, wire.other),
                .point     = Vecf3(wire.point),
                .speed     = wire.speed,
                .shader    = shader,
            };
            p.subsystem<PhysicsBus>().process(ev, &impact);
        }
    }

    /* The server's ticks to its clients: now and then, at once when it
     * pauses or resumes, and to each peer as it verifies */
    void send_tick_sync(Proxy& p, time_point const& t)
    {
        auto const& clock  = p.subsystem<TickClock>();
        bool const  due    = t >= m_next_tick_sync || clock.paused() != m_sent_paused ||
                         clock.pause_at() != m_sent_pause_at;
        std::set<HSteamNetConnection> targets;
        for(auto& [connection, state] : m_connections)
            if(state.verified && (due || !state.tick_synced))
            {
                targets.insert(connection);
                state.tick_synced = true;
            }
        if(due)
        {
            m_next_tick_sync = t + tick_sync_interval;
            m_sent_paused    = clock.paused();
            m_sent_pause_at  = clock.pause_at();
            for(auto& [_, state] : m_connections)
                if(state.inputs || state.inputs_lost)
                {
                    journal(
                        "net_inputs",
                        {{"player_idx", state.idx},
                         {"received", std::exchange(state.inputs, 0)},
                         {"lost", std::exchange(state.inputs_lost, 0)}});
                }
        }
        if(targets.empty())
            return;
        send_all(
            Message<TickSync>({
                .tick     = clock.tick(),
                .start_us   = clock.tick_began_us(),
                .session_us = m_session_start_us,
                .pause_at   = clock.pause_at().value_or(0),
                .paused     = clock.paused(),
                .speed_permille =
                    static_cast<u32>(std::clamp(clock.speed(), .05f, 1.f) * 1000),
            }),
            k_nSteamNetworkingSend_Reliable,
            std::move(targets));
    }

    /* Objects are simulated on the server, so forge grabs happen there */
    void send_grab(
        Proxy& p, u64 entity, PlayerInfo const& info, PlayerCamera const& cam, bool& sent)
    {
        auto const* input = p.get<PlayerInput>(entity);
        bool const  held  = input &&
                          input->input_mode == PlayerInput::input_mode_t::game &&
                          input->intent.grab && info.permissions.camera &&
                          !info.mode.physics && info.riding.vehicle == 0;
        if(!held && !sent)
            return;
        send_all(
            Message<GrabInput>({
                .origin     = Vecf4(cam.camera.position, 1.f),
                .to_world   = Quatf(glm::transpose(Matf3(cam.rotation))),
                .held       = held,
                .player_idx = info.player_idx,
            }),
            held ? k_nSteamNetworkingSend_UnreliableNoNagle
                 : k_nSteamNetworkingSend_Reliable,
            {m_connection},
            FRAME_UPDATE_LANE);
        sent = held;
    }

    /* Tells each client what it carries; lets go for those who left */
    void send_grab_states(Proxy& p)
    {
        std::set<u64> holders;
        for(auto& [connection, state] : m_connections)
            for(auto& [_, player] : state.players)
            {
                if(!player.biped.exists())
                    continue;
                u64 const id = player.biped.id();
                holders.insert(id);
                auto const* data = p.get<PhysicsData>(id);
                auto const* net  = data && data->grabbed
                                       ? p.get<NetworkInfo>(data->grabbed)
                                       : nullptr;
                u32 const object = net ? net->instance_id : 0;
                if(object == player.grabbed)
                    continue;
                player.grabbed = object;
                send_single(
                    connection,
                    Message<GrabState>({.object = object, .player_idx = player.idx}));
            }
        for(u64 holder : std::exchange(m_remote_holders, {}))
            if(!holders.contains(holder))
                remote_grab(p, holder, {}, false);
            else
                m_remote_holders.insert(holder);
    }

    void remote_grab(Proxy& p, u64 holder, GrabInput const& grab, bool held)
    {
        Physics::Event ev{Physics::Event::Grab};
        Physics::Grab  request{
             .entity_id = holder,
             .origin    = Vecf3(grab.origin),
             .to_world  = grab.to_world,
             .held      = held,
        };
        p.subsystem<PhysicsBus>().process(ev, &request);
        if(held)
            m_remote_holders.insert(holder);
        else
            m_remote_holders.erase(holder);
    }

    /* Moving objects are the server's: it sends what they do each tick,
     * and everything once to new peers */
    void send_object_sync(Proxy& p, time_point const&)
    {
        /* Once per tick, the state after it */
        u64 const tick = tick_now(p);
        if(tick == m_synced_tick)
            return;
        m_synced_tick = tick;
        std::erase_if(m_driven, [tick](auto const& e) {
            return tick - e.second.seen > 4 * driven_timeout;
        });

        std::vector<ObjectSync> changed, settled, all;
        std::vector<NetworkInfo const*> changed_nets;
        for(auto object : p.select<Model, ObjectPhysics, NetworkInfo>())
        {
            auto [model, physics, net] = object.components();
            if(physics.mass <= 0.f || net.instance_id == 0)
                continue;
            auto const* anim = p.get<AnimationPlayback>(object.id());
            ObjectSync sync{
                .tick             = tick,
                .rotation         = model.rotation,
                .position         = Vecf4(model.position, 0),
                .linear_velocity  = Vecf4(physics.linear_velocity, 0),
                .angular_velocity = Vecf4(physics.angular_velocity, 0),
                .net_id           = net.instance_id,
                .flags            = static_cast<ObjectSync::flags_t>(
                    (physics.sleeping ? ObjectSync::sleeping : ObjectSync::none) |
                    (anim && anim->stance == "crouch" ? ObjectSync::crouched : ObjectSync::none) |
                    (anim && anim->stance == "asleep" ? ObjectSync::asleep : ObjectSync::none)),
            };
            /* Where a unit aims and looks; one doing either is kept current */
            bool const aims  = anim && glm::dot(anim->aiming, anim->aiming) > 1e-6f;
            bool const looks = anim && glm::dot(anim->looking, anim->looking) > 1e-6f;
            bool const gazing = (aims || looks) && !p.get<PlayerInfo>(object.id());
            if(gazing && aims)
            {
                ObjectSync::pack(sync.aim, anim->aiming);
                sync.flags = static_cast<ObjectSync::flags_t>(sync.flags | ObjectSync::aiming);
            }
            if(gazing && looks)
            {
                ObjectSync::pack(sync.look, anim->looking);
                sync.flags = static_cast<ObjectSync::flags_t>(sync.flags | ObjectSync::looking);
            }
            all.push_back(sync);
            auto& was_asleep = m_object_asleep[net.instance_id];
            auto& was_gazing = m_object_gazing[net.instance_id];
            if(!physics.sleeping || gazing || was_gazing)
            {
                changed.push_back(sync);
                changed_nets.push_back(&net);
            }
            else if(!was_asleep)
                settled.push_back(sync);
            was_asleep = physics.sleeping;
            was_gazing = gazing;
        }

        std::set<HSteamNetConnection> fresh, current;
        for(auto& [connection, state] : m_connections)
        {
            if(!state.verified)
                continue;
            (state.objects_synced ? current : fresh).insert(connection);
            state.objects_synced = true;
        }
        if(!fresh.empty() && !all.empty())
            send_all<ObjectSync>(
                MessageBase{MessageBase::ObjectSync},
                gsl::span<ObjectSync>(all),
                k_nSteamNetworkingSend_Reliable,
                fresh);
        send_streams(p, changed, changed_nets);
        /* The last sync until it wakes, losing it leaves clients mid-motion */
        if(!current.empty() && !settled.empty())
            send_all<ObjectSync>(
                MessageBase{MessageBase::ObjectSync},
                gsl::span<ObjectSync>(settled),
                k_nSteamNetworkingSend_Reliable,
                current);
    }

    /* How much a connection's players care about a stream, and how many
     * ticks may pass between its states; unrated, it goes every tick */
    template<typename State>
    static std::pair<f32, u64> rating_of(NetworkInfo const& net, State const& state)
    {
        f32  priority = 0.f;
        u64  period   = max_period;
        bool rated    = false;
        for(auto const& [_, player] : state.players)
            if(auto const* r = net.relevance_for(player.idx))
            {
                rated    = true;
                priority = std::max(priority, r->priority);
                period   = std::min(period, static_cast<u64>(std::max<libc_types::i64>(
                    1, std::lround(r->update_period.count() / (TickClock::tick_seconds * 1000.f)))));
            }
        return rated ? std::pair{priority, period} : std::pair{1.f, u64(1)};
    }

    /* Each connection gets the players and moving objects due to it by
     * their rating, the most overdue for their worth first, within a byte
     * budget a tick. A player switching mode or deck is told reliably */
    void send_streams(
        Proxy& p, std::vector<ObjectSync> const& moving, std::vector<NetworkInfo const*> const& nets)
    {
        u64 const tick = tick_now(p);
        std::map<u32, NetworkInfo const*> player_nets;
        for(auto player : p.select<PlayerInfo, NetworkInfo>())
            player_nets[player.get<PlayerInfo>().player_idx] = &player.get<NetworkInfo>();

        struct due_t
        {
            f32    score;
            bool   camera;
            size_t index; /* into moving, or the camera's player index */
        };
        for(auto& [connection, state] : m_connections)
        {
            if(!state.verified)
                continue;
            std::vector<due_t> due;
            for(auto const& [idx, latest] : m_latest_cameras)
            {
                auto net = player_nets.find(idx);
                if(state.own(idx) || net == player_nets.end())
                    continue;
                auto const sent = state.cameras_sent.find(idx);
                if(sent != state.cameras_sent.end() && sent->second.tick >= latest.tick)
                    continue;
                /* New to it, changed mode or deck, or come to rest: reliably */
                bool const quiet = latest.tick + settle_ticks <= tick;
                if(sent == state.cameras_sent.end() || sent->second.flags != latest.flags ||
                   sent->second.deck != latest.deck || quiet)
                {
                    send_single(connection, Message<CameraSync>(CameraSync(latest)));
                    state.cameras_sent[idx] = latest;
                    continue;
                }
                auto const [priority, period] = rating_of(*net->second, state);
                u64 const age = tick - std::min(tick, sent->second.tick);
                if(age >= period)
                    due.push_back({priority * age / period, true, idx});
                else
                    state.streams_held++;
            }
            if(state.objects_synced)
                for(size_t i = 0; i < moving.size(); i++)
                {
                    if(driven_by(p, state, moving[i].net_id))
                        continue;
                    auto const [priority, period] = rating_of(*nets[i], state);
                    auto const sent = state.objects_sent.find(moving[i].net_id);
                    u64 const  age  = sent == state.objects_sent.end() ? period : tick - std::min(tick, sent->second);
                    if(age >= period)
                        due.push_back({priority * age / period, false, i});
                    else
                        state.streams_held++;
                }
            std::sort(due.begin(), due.end(), [](due_t const& a, due_t const& b) {
                return a.score > b.score;
            });

            size_t                  budget = tick_budget;
            std::vector<ObjectSync> objects;
            for(auto const& d : due)
            {
                size_t const bytes = d.camera ? sizeof(CameraSync) : sizeof(ObjectSync);
                if(bytes > budget)
                {
                    state.streams_over_budget++;
                    continue;
                }
                budget -= bytes;
                state.streams_sent++;
                if(d.camera)
                {
                    auto const& latest = m_latest_cameras.at(static_cast<u32>(d.index));
                    send_single(
                        connection,
                        Message<CameraSync>(CameraSync(latest)),
                        k_nSteamNetworkingSend_UnreliableNoNagle);
                    state.cameras_sent[static_cast<u32>(d.index)] = latest;
                } else
                {
                    objects.push_back(moving[d.index]);
                    state.objects_sent[moving[d.index].net_id] = tick;
                }
            }
            if(!objects.empty())
                send_all<ObjectSync>(
                    MessageBase{MessageBase::ObjectSync},
                    gsl::span<ObjectSync>(objects),
                    k_nSteamNetworkingSend_UnreliableNoNagle,
                    {connection},
                    FRAME_UPDATE_LANE);
        }
    }

    /* Whether a camera moved since last sent; records it if so */
    bool camera_moved(u64 entity, PlayerCamera const& cam)
    {
        auto it = m_sent_cameras.find(entity);
        bool const moved =
            it == m_sent_cameras.end() ||
            glm::distance(it->second.first, cam.camera.position) >
                camera_epsilon ||
            std::abs(glm::dot(it->second.second, cam.camera.rotation)) <
                1.f - rotation_epsilon;
        if(moved)
            m_sent_cameras[entity] = {cam.camera.position, cam.camera.rotation};
        return moved;
    }

    /* A player's seat as peers know it: the vehicle by its net id */
    static SeatSync seat_of(Proxy& p, PlayerInfo const& info)
    {
        SeatSync out{.player_idx = info.player_idx};
        if(info.riding.vehicle == 0)
            return out;
        auto const* net = p.template get<NetworkInfo>(info.riding.vehicle);
        if(!net || net->instance_id == 0)
            return out;
        out.vehicle = net->instance_id;
        out.seat    = info.riding.seat;
        out.flags   = static_cast<SeatSync::flags_t>(
            (info.riding.driver ? SeatSync::driver : 0) |
            (info.riding.exiting ? SeatSync::exiting : 0));
        return out;
    }

    /* Seats a remote player as another peer reported */
    static void apply_seat(Proxy& p, PlayerInfo& info, SeatSync const& seat)
    {
        info.riding = {};
        if(seat.vehicle == 0)
            return;
        for(auto object : p.template select<NetworkInfo, ObjectPhysics>())
        {
            auto [net, physics] = object.components();
            if(net.instance_id != seat.vehicle || !physics.drive)
                continue;
            info.riding = {
                .vehicle = object.id(),
                .seat    = seat.seat,
                .driver  = (seat.flags & SeatSync::driver) != 0,
                .exiting = (seat.flags & SeatSync::exiting) != 0,
            };
            return;
        }
    }

    /* Every player's seat as it changes, and all riders to a fresh peer */
    void send_seats(Proxy& p)
    {
        std::vector<SeatSync> changed, riders;
        for(auto player : p.select<PlayerInfo>())
        {
            auto const& info = player.get<PlayerInfo>();
            if(!is_networked(player.id(), info))
                continue;
            SeatSync const seat = seat_of(p, info);
            if(seat.vehicle != 0)
                riders.push_back(seat);
            auto& sent = m_sent_seats[info.player_idx];
            if(sent != seat)
                changed.push_back(seat);
            sent = seat;
        }
        std::set<HSteamNetConnection> fresh, current;
        for(auto& [connection, state] : m_connections)
            if(state.verified)
                (state.objects_synced ? current : fresh).insert(connection);
        if(!fresh.empty() && !riders.empty())
            send_all<SeatSync>(
                MessageBase{MessageBase::SeatSync},
                gsl::span<SeatSync>(riders),
                k_nSteamNetworkingSend_Reliable,
                fresh);
        if(!current.empty() && !changed.empty())
            send_all<SeatSync>(
                MessageBase{MessageBase::SeatSync},
                gsl::span<SeatSync>(changed),
                k_nSteamNetworkingSend_Reliable,
                current);
    }

    /* Units scripts sat in vehicles as they change, and all of them to a
     * fresh peer */
    void send_riders(Proxy& p)
    {
        std::vector<RiderSync> changed, riders;
        for(auto object : p.select<ObjectPhysics, NetworkInfo>())
        {
            auto [physics, net] = object.components();
            if(net.instance_id == 0 || p.get<PlayerInfo>(object.id()))
                continue;
            RiderSync rider{.unit = net.instance_id};
            if(physics.seated && physics.seated_in)
                if(auto const* vehicle = p.get<NetworkInfo>(physics.seated_in); vehicle && vehicle->instance_id)
                {
                    rider.vehicle = vehicle->instance_id;
                    rider.seat    = physics.seat;
                }
            auto const sent = m_sent_riders.find(net.instance_id);
            if(rider.vehicle == 0 && sent == m_sent_riders.end())
                continue;
            if(rider.vehicle != 0)
                riders.push_back(rider);
            if(sent == m_sent_riders.end() || sent->second != rider)
                changed.push_back(rider);
            if(rider.vehicle != 0)
                m_sent_riders[net.instance_id] = rider;
            else
                m_sent_riders.erase(sent);
        }
        std::set<HSteamNetConnection> fresh, current;
        for(auto& [connection, state] : m_connections)
            if(state.verified)
                (state.objects_synced ? current : fresh).insert(connection);
        if(!fresh.empty() && !riders.empty())
            send_all<RiderSync>(
                MessageBase{MessageBase::RiderSync},
                gsl::span<RiderSync>(riders),
                k_nSteamNetworkingSend_Reliable,
                fresh);
        if(!current.empty() && !changed.empty())
            send_all<RiderSync>(
                MessageBase{MessageBase::RiderSync},
                gsl::span<RiderSync>(changed),
                k_nSteamNetworkingSend_Reliable,
                current);
    }

    /* Seats a unit as the server has it: touching nothing, in the seat's
     * stance, or back in its own. False until the unit is here */
    bool seat_rider(Proxy& p, RiderSync const& rider)
    {
        u64 unit = 0, vehicle = 0;
        for(auto object : p.select<NetworkInfo>())
        {
            u32 const id = object.get<NetworkInfo>().instance_id;
            if(id == rider.unit)
                unit = object.id();
            else if(rider.vehicle != 0 && id == rider.vehicle)
                vehicle = object.id();
        }
        auto* physics = unit ? p.get<ObjectPhysics>(unit) : nullptr;
        if(!physics || (rider.vehicle != 0 && !vehicle))
            return false;
        physics->seated    = vehicle != 0;
        physics->seated_in = vehicle;
        physics->seat      = vehicle ? rider.seat : -1;
        auto* anim = p.get<AnimationPlayback>(unit);
        if(!anim)
            return true;
        /* Its own stance comes back when it gets out */
        auto own = m_rider_stances.find(rider.unit);
        if(!vehicle)
        {
            if(own != m_rider_stances.end())
            {
                anim->stance = own->second;
                m_rider_stances.erase(own);
            }
        } else if(auto const* model = p.get<Model>(vehicle); model && model->origin_object)
            if(auto vehi = model->origin_object->template data<blam::scn::unit>(m_map->magic); vehi.has_value())
                if(auto seats = vehi.value()[0].seats.data(m_map->magic);
                   seats.has_value() && rider.seat >= 0 && static_cast<size_t>(rider.seat) < seats.value().size())
                {
                    if(own == m_rider_stances.end())
                        m_rider_stances[rider.unit] = anim->stance;
                    anim->stance = std::string(seats.value()[rider.seat].label.str());
                }
        journal("net_rider", {{"unit", rider.unit}, {"vehicle", rider.vehicle}, {"stance", anim->stance}});
        return true;
    }

    /* What a player carries, by tag ids; nothing before the map gives it any */
    static EquipmentSync equipment_of(Proxy& p, u64 entity, PlayerInfo const& info)
    {
        EquipmentSync out{.player_idx = info.player_idx};
        auto const*   equip = p.get<BipedEquipment>(entity);
        if(!equip || !equip->held(p.subsystem<BlamFiles<halo_version>>().load_generation))
            return out;
        auto const& [held, slung] = equip->weapons;
        out.weapon = held.tag ? held.tag->tag_id : blam::tag_index<>{};
        out.slung  = slung.tag ? slung.tag->tag_id : blam::tag_index<>{};
        out.flags  = static_cast<EquipmentSync::flags_t>(
            (equip->reloading ? EquipmentSync::reloading : 0) |
            (equip->throwing ? EquipmentSync::throwing : 0) |
            (info.camouflaged ? EquipmentSync::camouflaged : 0));
        for(size_t i = 0; i < out.grenades.size() && i < equip->equipment.size(); i++)
            out.grenades[i] = static_cast<u8>(std::min<u16>(equip->equipment[i].left, 255));
        out.reserve = {held.rounds_total, slung.rounds_total};
        return out;
    }

    /* What devices and groups are told, as it changes, and all of it to a
     * fresh peer */
    void send_devices(Proxy& p)
    {
        std::vector<DeviceSync> changed, all;
        auto note = [&](u64 key, DeviceSync const& sync) {
            all.push_back(sync);
            if(auto& sent = m_sent_devices[key]; sent != sync)
            {
                changed.push_back(sync);
                sent = sync;
            }
        };
        auto const& groups = p.subsystem<DeviceGroups>();
        for(i16 i = 0; i < static_cast<i16>(groups.values.size()); i++)
            note(u64(1) << 32 | static_cast<u16>(i), {.group = i, .value = groups.values[i]});
        for(auto entity : p.select<DeviceState, NetworkInfo>())
        {
            auto [device, net] = entity.components();
            if(net.instance_id != 0)
                note(net.instance_id,
                     {.net_id          = net.instance_id,
                      .position_target = device.position_target,
                      .power_target    = device.power_target,
                      .snap            = device.snaps});
        }
        std::set<HSteamNetConnection> fresh, current;
        for(auto& [connection, state] : m_connections)
            if(state.verified)
                (state.objects_synced ? current : fresh).insert(connection);
        if(!fresh.empty() && !all.empty())
            send_all<DeviceSync>(
                MessageBase{MessageBase::DeviceSync},
                gsl::span<DeviceSync>(all),
                k_nSteamNetworkingSend_Reliable,
                fresh);
        if(!current.empty() && !changed.empty())
            send_all<DeviceSync>(
                MessageBase{MessageBase::DeviceSync},
                gsl::span<DeviceSync>(changed),
                k_nSteamNetworkingSend_Reliable,
                current);
    }

    /* Every player's weapon as it changes, and all of them to a fresh peer */
    void send_equipment(Proxy& p)
    {
        std::vector<EquipmentSync> changed, all;
        for(auto player : p.select<PlayerInfo>())
        {
            auto const& info = player.get<PlayerInfo>();
            if(!is_networked(player.id(), info))
                continue;
            EquipmentSync const held = equipment_of(p, player.id(), info);
            if(held.weapon == blam::tag_index<>{})
                continue;
            all.push_back(held);
            auto& sent = m_sent_equipment[info.player_idx];
            if(sent != held)
                changed.push_back(held);
            sent = held;
        }
        std::set<HSteamNetConnection> fresh, current;
        for(auto& [connection, state] : m_connections)
            if(state.verified)
                (state.objects_synced ? current : fresh).insert(connection);
        if(!fresh.empty() && !all.empty())
            send_all<EquipmentSync>(
                MessageBase{MessageBase::EquipmentSync},
                gsl::span<EquipmentSync>(all),
                k_nSteamNetworkingSend_Reliable,
                fresh);
        if(!current.empty() && !changed.empty())
            send_all<EquipmentSync>(
                MessageBase{MessageBase::EquipmentSync},
                gsl::span<EquipmentSync>(changed),
                k_nSteamNetworkingSend_Reliable,
                current);
    }

    /* What the server took off the ground: gone for everyone, and a weapon
     * for the client whose player picked it up */
    void send_pickups()
    {
        std::set<HSteamNetConnection> verified;
        for(auto& [connection, state] : m_connections)
            if(state.verified)
                verified.insert(connection);
        if(!verified.empty() && !m_despawns.empty())
            send_all<ObjectDespawn>(
                MessageBase{MessageBase::ObjectDespawn},
                gsl::span<ObjectDespawn>(m_despawns),
                k_nSteamNetworkingSend_Reliable,
                verified);
        m_despawns.clear();
        for(auto const& pickup : std::exchange(m_pickups, {}))
            for(auto& [connection, state] : m_connections)
                for(auto const& [_, player] : state.players)
                    if(player.biped.exists() && player.biped.id() == pickup.entity_id)
                        send_single(
                            connection,
                            Message<WeaponPickup>({
                                .player_idx = player.idx,
                                .weapon     = pickup.weapon,
                                .age        = pickup.age,
                                .rounds     = pickup.rounds,
                            }));
    }

    /* Every census_period a peer hears which replicated objects live here,
     * so a spawn or despawn it missed gets put right (ObjectCensus) */
    void send_census(Proxy& p)
    {
        auto const now = std::chrono::steady_clock::now();
        std::set<HSteamNetConnection> due;
        for(auto& [connection, state] : m_connections)
        {
            if(!state.verified || !state.objects_synced)
                continue;
            /* The backlog goes first, and gets a period to land */
            if(state.census_due == std::chrono::steady_clock::time_point{})
                state.census_due = now + census_period;
            if(now < state.census_due)
                continue;
            state.census_due = now + census_period;
            due.insert(connection);
        }
        if(due.empty())
            return;
        std::vector<ObjectCensus> live;
        for(auto object : p.select<NetworkInfo>())
        {
            auto const& net = object.get<NetworkInfo>();
            if(census_covers(net.instance_id) && !p.get<PlayerInfo>(object.id()))
                live.push_back({.net_id = net.instance_id, .tag_id = net.object.tag_id});
        }
        u32 const buckets =
            std::max<u32>(1, (live.size() + census_batch - 1) / census_batch);
        size_t bytes = 0;
        for(u32 bucket = 0; bucket < buckets; bucket++)
        {
            std::vector<ObjectCensus> part;
            for(auto const& entry : live)
                if(census_bucket(entry.net_id, buckets) == bucket)
                    part.push_back(entry);
            /* An empty bucket still has to say so */
            if(part.empty())
                part.push_back({});
            bytes += sizeof(MessageBase) + part.size() * sizeof(ObjectCensus);
            send_all<ObjectCensus>(
                MessageBase{
                    .type    = MessageBase::ObjectCensus,
                    .request = bucket | buckets << 16,
                },
                gsl::span<ObjectCensus>(part),
                k_nSteamNetworkingSend_UnreliableNoNagle,
                due);
        }
        journal(
            "net_census",
            {{"objects", live.size()},
             {"buckets", buckets},
             {"bytes", bytes},
             {"peers", due.size()}});
    }

    static bool is_weapon(BlamFiles<halo_version> const& files, blam::tag_index<> weapon)
    {
        blam::tag_index_view<halo_version> index(files.container);
        auto const tag = index.find(weapon);
        return tag != index.end() && (*tag).valid() && (*tag).matches(blam::tag_class_t::weap);
    }

    /* Remote players hold what their peer last reported, through a map load
     * and before their body exists */
    void equip_remote_players(Proxy& p)
    {
        for(auto player : p.select<PlayerInfo>())
        {
            auto const& info = player.get<PlayerInfo>();
            auto        it   = m_remote_weapons.find(info.player_idx);
            if(!info.is_remote() || it == m_remote_weapons.end())
                continue;
            /* The dead hold nothing, whatever was last reported */
            if(auto const* status = p.get<BipedStatus>(player.id()); status && status->dead)
                continue;
            if(auto* equip = p.get<BipedEquipment>(player.id()))
            {
                equip->reloading = it->second.flags & EquipmentSync::reloading;
                equip->throwing  = it->second.flags & EquipmentSync::throwing;
                /* Its own peer runs its camouflage */
                p.get<PlayerInfo>(player.id())->camouflaged =
                    it->second.flags & EquipmentSync::camouflaged;
                /* Thrown where it is played, so counted there */
                for(size_t i = 0; i < it->second.grenades.size() && i < equip->equipment.size(); i++)
                    equip->equipment[i].left = it->second.grenades[i];
                /* Likewise reserves, which the authority's ammo pickups go by */
                for(size_t i = 0; i < equip->weapons.size(); i++)
                    if(auto& weapon = equip->weapons[i];
                       weapon.tag && weapon.tag->tag_id == (i ? it->second.slung : it->second.weapon))
                        weapon.rounds_total = it->second.reserve[i];
            }
            auto const known = equipment_of(p, player.id(), info);
            if(known.weapon == it->second.weapon && known.slung == it->second.slung)
                continue;
            /* Tag ids are the map's own: a report from the last map waits for
             * the next */
            auto const& files = p.subsystem<BlamFiles<halo_version>>();
            if(!is_weapon(files, it->second.weapon) ||
               (it->second.slung != blam::tag_index<>{} && !is_weapon(files, it->second.slung)))
                continue;
            /* A body not shown yet takes it when it is; once is enough */
            u32 const generation = files.load_generation;
            auto*     equipped   = p.get<BipedEquipment>(player.id());
            auto&     asked      = m_equip_asked[player.id()];
            if(!(equipped && equipped->held(generation)) && asked.first == generation &&
               asked.second == it->second)
                continue;
            asked = {generation, it->second};
            EquipWeaponEvent equip{
                .entity_id = player.id(),
                .weapon    = it->second.weapon,
                .slung     = it->second.slung,
                .mode      = EquipWeaponEvent::replicate,
            };
            GameEvent        ev{.type = GameEvent::EquipWeapon};
            m_game_bus.inject(ev, &equip);
        }
    }

    /* Every correction of a followed object goes through here */
    static void follow(
        Model&            model,
        ObjectPhysics&    physics,
        ObjectSync const& from,
        ObjectSync const& to,
        f32               alpha)
    {
        model.position = glm::mix(Vecf3(from.position), Vecf3(to.position), alpha);
        model.rotation = glm::slerp(from.rotation, to.rotation, alpha);
        model.update_matrix();
        physics.linear_velocity  = Vecf3(to.linear_velocity);
        physics.angular_velocity = Vecf3(to.angular_velocity);
        physics.sleeping         = to.flags & ObjectSync::sleeping;
    }

    /* The connection to the server is over, whoever ended it */
    void close_server_connection(HSteamNetConnection connection, bool linger)
    {
        m_net_state.client_state = NetworkState::ClientState::Disconnecting;
        journal("net_disconnected", {{"server", remote_name()}});
        m_clock                         = {};
        m_sync_lag                      = 0.f;
        m_sync_newest                   = 0;
        m_net_state.server_clock_offset = std::nullopt;
        m_impl->CloseConnection(connection, 0, nullptr, linger);
        m_connections.erase(connection);
        m_connection  = {};
        m_left_server = true;
    }

    void server_gone(
        ServerDisconnectedEvent::reason_t reason, char const* detail)
    {
        GameEvent               ev{GameEvent::ServerDisconnected};
        ServerDisconnectedEvent gone{
            .reason = reason,
            .detail = detail ? detail : "",
        };
        m_game_bus.inject(ev, &gone);
    }

    void server_close_peer_connection(
        HSteamNetConnection connection,
        int                 code,
        bool                linger,
        char const*         reason = nullptr)
    {
        cDebug(
            "peer={} disconnected ({})", connection, client_name(connection));
        if(auto it = m_connections.find(connection); it != m_connections.end())
            for(auto& [_, seat] : it->second.players)
            {
                if(seat.biped.exists())
                    seat.biped.get<NetworkInfo>().connected = false;
                auto& player = seat.player_info;
                if(player.exists())
                    player.m_ref->remove_entity_if(
                        [&player](compo::Entity const& e) {
                            return player.m_id == e.id;
                        });
            }
        m_connections.erase(connection);
#if defined(USE_WEBRTC_TRANSPORT)
        if(m_webrtcServer)
            m_webrtcServer->ForgetConnection(connection);
#endif
        update_player_counts();
        send_player_roster();
        m_impl->CloseConnection(connection, code, reason, linger);
    }

    void leave_server(Proxy& p)
    {
        m_object_history.clear();
        m_object_spacing.clear();
        m_player_history.clear();
        m_driven.clear();
        for(auto object : p.select<ObjectPhysics>())
            object.get<ObjectPhysics>().authority = ObjectPhysics::Simulated;
        m_left_server    = false;
        m_join_confirmed = false;
        m_combat.reset();
        m_session_start_us = 0;
        m_net_state.tick_base.reset();
        m_own_seats.clear();
        m_pending_seats.clear();
        for(auto holder : p.select<PlayerInfo, PhysicsData>())
            if(!holder.get<PlayerInfo>().is_remote())
                holder.get<PhysicsData>().grabbed = 0;
        p.subsystem<NetworkState>().client_state = NetworkState::ClientState::None;
        p.subsystem<NetworkState>().remote_player_idx.reset();
        p.subsystem<NetworkState>().remote_seats.clear();
        p.remove_entity_if([&p](compo::Entity const& e) {
            auto* info = p.get<PlayerInfo>(e.id);
            return info && info->is_remote();
        });
        for(auto player : p.select<PlayerInfo>())
        {
            auto& info              = player.get<PlayerInfo>();
            info.player_idx         = info.seat_idx;
            info.permissions.camera = true;
            info.spawned            = true;
        }
    }

    /*! The vehicle `net_id`, if one of the sender's players drives it */
    u64 driven_by(Proxy& p, auto& state, u32 net_id)
    {
        for(auto& [_, player] : state.players)
        {
            if(!player.biped.exists())
                continue;
            auto const& riding = player.biped.template get<PlayerInfo>().riding;
            auto const* net =
                riding.vehicle != 0 ? p.get<NetworkInfo>(riding.vehicle) : nullptr;
            if(riding.driver && !riding.exiting && net && net->instance_id == net_id)
                return riding.vehicle;
        }
        return 0;
    }

    void server_receive_payload(
        Proxy& p, HSteamNetConnection connection, MessageBase const& payload)
    {
        auto& player_info = m_connections[connection];
        switch(payload.type)
        {
        case MessageBase::CameraSync: {
            /* Only ever the sender's own players */
            auto const& sync   = payload.value<CameraSync>();
            auto*       player = player_info.own(sync.target_player);
            if(!player)
                break;
            auto& info = player->biped.get<PlayerInfo>();
            if(!info.permissions.camera)
                break;
            /* Sent unreliably: one older than what has come is stale */
            auto& newest = m_camera_ticks[player->idx];
            if(sync.tick != 0 && sync.tick <= newest)
                break;
            newest = sync.tick;
            /* A body walked here goes where its inputs take it, once in */
            bool const simulated = sync.flags & CameraSync::simulated;
            bool const walked    = info.mode.simulated && info.mode.physics;
            info.mode.physics    = sync.flags & CameraSync::physics;
            info.mode.simulated  = simulated;
            if(simulated && walked)
                break;
            auto& cam           = player->biped.get<PlayerCamera>();
            cam.camera.position = world_of(p, sync);
            cam.camera.rotation = sync.rotation;

            /* Passed on to the others as send_streams schedules it */
            m_latest_cameras[player->idx] = {
                .position      = sync.position,
                .rotation      = sync.rotation,
                .target_player = player->idx,
                .flags         = sync.flags,
                .deck          = sync.deck,
                /* Its body moves there on the next tick here */
                .tick = tick_now(p) + 1,
            };
            break;
        }
        case MessageBase::TickInput: {
            receive_inputs(connection, payload.values<TickInput>());
            break;
        }
        case MessageBase::SyncPoint: {
            auto const& reached = payload.value<SyncPoint>();
            std::string name(reached.name.data(), strnlen(reached.name.data(), reached.name.size()));
            m_syncs[name].reached.insert(connection);
            release_sync(name);
            break;
        }
        case MessageBase::GrabInput: {
            /* Only the sender's own players, from their free cameras */
            auto const& grab   = payload.value<GrabInput>();
            auto*       player = player_info.own(grab.player_idx);
            if(!player)
                break;
            auto const& info = player->biped.get<PlayerInfo>();
            auto const& eye  = player->biped.get<PlayerCamera>().camera.position;
            bool const held =
                grab.held && info.permissions.camera && !info.mode.physics &&
                info.riding.vehicle == 0 &&
                glm::distance(Vecf3(grab.origin), eye) <= max_grab_offset;
            if(held || m_remote_holders.contains(player->biped.id()))
                remote_grab(p, player->biped.id(), grab, held);
            break;
        }
        case MessageBase::ObjectSync: {
            /* Only vehicles the sender's own players drive */
            u64 const now = tick_now(p);
            for(auto const& sync : payload.values<ObjectSync>())
            {
                if(!driven_by(p, player_info, sync.net_id))
                    continue;
                auto& driven = m_driven[sync.net_id];
                if(!driven.history.empty() && driven.history.back().tick >= sync.tick)
                    continue;
                f32 const lag = static_cast<f32>(
                    now > sync.tick ? now - sync.tick : 0);
                driven.lag = driven.history.empty()
                                 ? lag
                                 : glm::mix(driven.lag, lag, lag_smoothing);
                driven.seen = now;
                driven.history.push_back(sync);
                while(driven.history.size() > object_history_size)
                    driven.history.pop_front();
            }
            break;
        }
        case MessageBase::DriveInput: {
            /* Only vehicles the sender's own players drive */
            auto const& drive   = payload.value<DriveInput>();
            u64 const   vehicle = driven_by(p, player_info, drive.vehicle);
            if(!vehicle)
                break;
            Physics::Event ev{Physics::Event::Drive};
            Physics::Drive input{
                .vehicle  = vehicle,
                .throttle = drive.throttle,
                .steer    = drive.steer,
                .aim      = Vecf3(drive.aim),
            };
            p.subsystem<PhysicsBus>().process(ev, &input);
            break;
        }
        case MessageBase::WeaponFire: {
            auto const& fire   = payload.value<WeaponFire>();
            auto*       player = player_info.own(fire.player_idx);
            if(!player || !player_info.verified)
                break;
            SteamNetConnectionRealTimeStatus_t status{};
            bool const measured = m_impl->GetConnectionRealTimeStatus(
                                      connection, &status, 0, nullptr) ==
                                  k_EResultOK;
            m_combat.server_receive(
                p,
                net_combat::sender_t{
                    .connection = connection,
                    .player     = player->biped.id(),
                    .player_idx = player->idx,
                    .ping_ms =
                        measured ? static_cast<u32>(std::max(status.m_nPing, 0)) : 0,
                },
                fire);
            break;
        }
        case MessageBase::SeatSync: {
            /* Only ever the sender's own players */
            for(auto const& seat : payload.values<SeatSync>())
                if(auto* player = player_info.own(seat.player_idx))
                    apply_seat(p, player->biped.get<PlayerInfo>(), seat);
            break;
        }
        case MessageBase::EquipmentSync: {
            /* Likewise only the sender's own */
            for(auto const& held : payload.values<EquipmentSync>())
                if(player_info.own(held.player_idx))
                    m_remote_weapons[held.player_idx] = held;
            break;
        }
        case MessageBase::PlayerJoin: {
            auto const& player_join = payload.value<PlayerJoin>();
            if(player_join.seat >= max_client_seats)
                break;
            auto&      player = player_info.players[player_join.seat];
            bool const rejoin = player.player_info.exists();
            if(!rejoin)
                player.idx = player_join.seat == 0 ? player_info.idx : m_next_remote_idx++;
            cDebug("Player joined: {}", player_join.player_name.str());
            journal(
                "net_player_join",
                {{"name", player_join.player_name.str()},
                 {"player_idx", player.idx},
                 {"seat", player_join.seat},
                 {"rejoin", rejoin}});
            if(!rejoin)
            {
                auto ref = p.create_entity(shared_recipes::player_recipe);
                player.player_info = ref.ref<PlayerInfo>();
                player.biped       = ref;

                auto& net_info     = ref.get<NetworkInfo>();
                net_info.connected = true;

                auto& info      = (*player.player_info);
                info.remote     = client_name(connection);
                info.player_idx = player.idx;
                info.seat_idx   = 0xFFFF; /* Not one of our local seats */
            }

            (*player.player_info).name = player_join.player_name.str();

            send_single(
                connection,
                Message<PlayerJoinConfirm>({
                    .player_idx = player.idx,
                    .seat       = player_join.seat,
                }));
            update_player_counts();
            send_player_roster();
            send_positions(p, connection);
            /* A seat joining after the map loaded spawns as the rest did */
            if(!rejoin && player_info.loading_progress == 100)
                player_init(p.unconstrained_container(), *player.player_info);
            break;
        }
        case MessageBase::GameLoadState: {
            auto const& event = payload.value<GameLoadState>();
            cDebug(
                "Player {} is at {}% loaded",
                m_connections[connection].idx,
                event.progress);
            m_connections[connection].loading_progress = event.progress;
            for(auto& [_, player] : player_info.players)
            {
                if(!player.player_info.exists())
                    continue;
                (*player.player_info).loading_progress = event.progress;
                if(event.progress == 100)
                    player_init(p.unconstrained_container(), *player.player_info);
            }
            break;
        }
        case MessageBase::SpawnRequest: {
            auto const&        request = payload.value<SpawnRequest>();
            blam::tag_t const* tag     = nullptr;
            auto reason = check_spawn_request(player_info.verified, request, tag);
            journal(
                reason.empty() ? "net_spawn_accept" : "net_spawn_reject",
                {{"player_idx", player_info.idx},
                 {"request_id", request.request_id},
                 {"tag_id", request.tag.tag_id},
                 {"reason", reason}});
            if(!reason.empty())
            {
                cDebug(
                    "Rejected spawn request {} from player {}: {}",
                    request.request_id,
                    player_info.idx,
                    reason);
                send_single(
                    connection,
                    Message<EntitySpawn>({
                        .tag =
                            {
                                .tag_class = request.tag.tag_class,
                                .tag_id    = request.tag.tag_id,
                            },
                        .response_id = request.request_id,
                    }));
                break;
            }
            /* Remote, so the spawn queue doesn't replicate it a second time */
            GameEvent        ev{.type = GameEvent::SpawnObject};
            SpawnObjectEvent spawn{
                .origin   = MapLoadEvent::Remote,
                .object   = tag->as_ref(),
                .position = Vecf3(request.position),
                .rotation = request.direction,
                .net_id = SpawnObjectEvent::dynamic_net_id_base + m_next_net_id++,
                .age      = request.age,
                .rounds   = request.rounds[0] == SpawnRequest::no_rounds
                                ? std::nullopt
                                : std::optional(request.rounds),
                .announce = request.announce != 0,
            };
            m_game_bus.inject(ev, &spawn);
            replicate_spawn(spawn, connection, request.request_id);
            break;
        }
        case MessageBase::ObjectRequest: {
            if(!player_info.verified)
                break;
            std::vector<EntitySpawn> answer;
            std::vector<u32>         unknown;
            for(auto const& request : payload.values<ObjectRequest>())
            {
                auto it = std::find_if(
                    m_dynamic_spawns.begin(),
                    m_dynamic_spawns.end(),
                    [&request](EntitySpawn const& s) {
                        return s.tag.instance_id == request.net_id;
                    });
                if(it == m_dynamic_spawns.end())
                {
                    unknown.push_back(request.net_id);
                    continue;
                }
                answer.push_back(*it);
                /* Where it is now, not where it was spawned */
                if(auto const* model = p.get<Model>(entity_of(p, request.net_id)))
                {
                    answer.back().position = Vecf4(model->position, 1.f);
                    answer.back().rotation = model->rotation;
                }
            }
            journal(
                "net_census_answer",
                {{"player_idx", player_info.idx},
                 {"sent", answer.size()},
                 {"unknown", unknown}});
            if(!answer.empty())
                send_all<EntitySpawn>(
                    MessageBase{MessageBase::EntitySpawn},
                    gsl::span<EntitySpawn>(answer),
                    k_nSteamNetworkingSend_Reliable,
                    {connection});
            break;
        }
        case MessageBase::MapVerify: {
            auto const& theirs = payload.value<MapVerify>().fingerprint;
            bool const  match  = theirs == m_fingerprint;
            journal(
                match ? "net_map_verified" : "net_map_mismatch",
                {{"player_idx", player_info.idx},
                 {"fingerprint", to_string(theirs)},
                 {"expected", to_string(m_fingerprint)}});
            if(!match)
            {
                cWarning(
                    "Player {} has different map data: {} != {}, kicking",
                    player_info.idx,
                    to_string(theirs),
                    to_string(m_fingerprint));
                send_single(
                    connection,
                    Message<GameLeave>({
                        .reason = *blam::bl_string_var<128>::from(
                            "map data differs from server"),
                    }));
                server_close_peer_connection(connection, 0, true);
                break;
            }
            player_info.verified = true;
            send_positions(p, connection);
            if(!m_dynamic_spawns.empty())
            {
                MessageBase header{.type = MessageBase::EntitySpawn};
                send_all(
                    std::move(header),
                    gsl::make_span(m_dynamic_spawns),
                    k_nSteamNetworkingSend_Reliable,
                    {connection});
            }
            break;
        }
        case MessageBase::GameEvent: {
            auto const& event  = payload.value<GameEventWrapper<char>>();
            GameEvent   event_ = event.event;
            cDebug(
                "Received {} from client {}",
                magic_enum::enum_name(event_.type),
                connection);
            if(!is_client_network_event(event_))
                break;
            m_game_bus.inject(event_, const_cast<char*>(&event.data));
            break;
        }
        default:
            break;
        }
    }

    /* A test fault: the next `left` messages of this kind never arrived */
    bool drop_for_test(u32& left, MessageBase const& payload)
    {
        if(left == 0)
            return false;
        left--;
        journal(
            "net_fault_drop",
            {{"type", std::string(magic_enum::enum_name(payload.type))},
             {"count", payload.num_values}});
        return true;
    }

    /* One bucket of the server's objects: drop what it does not list, ask
     * for what it lists and we lack. What arrived within the last period
     * may still be crossing the census, and is left alone */
    void receive_census(Proxy& p, MessageBase const& payload)
    {
        u32 const  bucket  = payload.request & 0xFFFF;
        u32 const  buckets = std::max<u32>(1, payload.request >> 16);
        auto const now     = std::chrono::steady_clock::now();
        std::erase_if(m_recent_spawns, [now](auto const& e) {
            return now - e.second > census_period;
        });
        std::erase_if(m_requested, [now](auto const& e) {
            return now - e.second > census_period;
        });
        std::map<u32, blam::tag_index<>> listed;
        for(auto const& entry : payload.values<ObjectCensus>())
            if(entry.net_id != 0)
                listed[entry.net_id] = entry.tag_id;
        for(auto object : p.select<NetworkInfo>())
        {
            auto const& net = object.get<NetworkInfo>();
            if(!census_covers(net.instance_id) ||
               census_bucket(net.instance_id, buckets) != bucket ||
               p.get<PlayerInfo>(object.id()))
                continue;
            auto const it       = listed.find(net.instance_id);
            bool const mismatch = it != listed.end() && it->second != net.object.tag_id;
            if(it != listed.end() && !mismatch)
            {
                listed.erase(it);
                continue;
            }
            if(m_recent_spawns.contains(net.instance_id))
            {
                if(it != listed.end())
                    listed.erase(it);
                continue;
            }
            cDebug(
                "Census: dropping {:#x} ({}:{}), {}",
                net.instance_id,
                blam::to_string(net.object.class_id()),
                net.object.tag_id,
                mismatch ? "the server has another object there" : "the server has not got it");
            journal(
                "net_census_drop",
                {{"net_id", net.instance_id},
                 {"tag_id", net.object.tag_id},
                 {"mismatch", mismatch}});
            DespawnObjectEvent gone{.net_id = net.instance_id};
            GameEvent          ev{.type = GameEvent::DespawnObject};
            m_game_bus.inject(ev, &gone);
        }
        std::vector<ObjectRequest> missing;
        for(auto const& [net_id, _] : listed)
        {
            if(m_requested.contains(net_id))
                continue;
            m_requested[net_id] = now;
            missing.push_back({.net_id = net_id});
            cDebug("Census: asking for {:#x}", net_id);
        }
        if(missing.empty())
            return;
        std::vector<u32> ids;
        for(auto const& request : missing)
            ids.push_back(request.net_id);
        journal("net_census_request", {{"count", missing.size()}, {"net_ids", ids}});
        send_all<ObjectRequest>(
            MessageBase{MessageBase::ObjectRequest},
            gsl::span<ObjectRequest>(missing),
            k_nSteamNetworkingSend_Reliable,
            {m_connection});
    }

    void client_receive_payload(Proxy& p, MessageBase const& payload)
    {
        switch(payload.type)
        {
        case MessageBase::EquipmentSync: {
            for(auto const& held : payload.values<EquipmentSync>())
                if(!own_idx(held.player_idx))
                    m_remote_weapons[held.player_idx] = held;
            break;
        }
        case MessageBase::WeaponPickup: {
            auto const& pickup = payload.value<WeaponPickup>();
            u64 const   holder = own_player(p, pickup.player_idx);
            if(!holder)
                break;
            EquipWeaponEvent equip{
                .entity_id = holder,
                .weapon    = pickup.weapon,
                .mode      = EquipWeaponEvent::pick_up,
                .age       = pickup.age,
                .rounds    = pickup.rounds,
            };
            GameEvent ev{.type = GameEvent::EquipWeapon};
            m_game_bus.inject(ev, &equip);
            break;
        }
        case MessageBase::DeviceSync: {
            std::map<u32, u64> devices;
            for(auto entity : p.select<DeviceState, NetworkInfo>())
                devices[entity.get<NetworkInfo>().instance_id] = entity.id();
            for(auto const& sync : payload.values<DeviceSync>())
            {
                auto const      device = devices.find(sync.net_id);
                DeviceSyncEvent event{
                    .origin          = MapLoadEvent::Remote,
                    .group           = sync.group,
                    .value           = sync.value,
                    .device          = device != devices.end() ? device->second : 0,
                    .position_target = sync.position_target,
                    .power_target    = sync.power_target,
                    .snap            = sync.snap,
                };
                GameEvent ev{.type = GameEvent::DeviceSync};
                m_game_bus.inject(ev, &event);
            }
            break;
        }
        case MessageBase::SyncPoint: {
            auto const& released = payload.value<SyncPoint>();
            if(!released.released)
                break;
            GameEvent      ev{.type = GameEvent::SyncPoint};
            SyncPointEvent done{
                .name = std::string(
                    released.name.data(), strnlen(released.name.data(), released.name.size())),
                .released = true,
            };
            m_game_bus.inject(ev, &done);
            break;
        }
        case MessageBase::ObjectCensus: {
            if(m_expected_fingerprint != m_fingerprint)
                break;
            receive_census(p, payload);
            break;
        }
        case MessageBase::ObjectDespawn: {
            if(drop_for_test(m_net_state.faults.drop_despawns, payload))
                break;
            for(auto const& gone : payload.values<ObjectDespawn>())
            {
                DespawnObjectEvent despawn{.net_id = gone.net_id};
                GameEvent          ev{.type = GameEvent::DespawnObject};
                m_game_bus.inject(ev, &despawn);
            }
            break;
        }
        case MessageBase::RiderSync:
            for(auto const& rider : payload.values<RiderSync>())
            {
                std::erase_if(m_waiting_riders, [&](auto const& r) { return r.first.unit == rider.unit; });
                if(!seat_rider(p, rider))
                    m_waiting_riders.emplace_back(rider, 0);
            }
            break;
        case MessageBase::SeatSync: {
            for(auto const& seat : payload.values<SeatSync>())
            {
                if(own_idx(seat.player_idx))
                    continue;
                for(auto entity : p.select<PlayerInfo>())
                {
                    auto& info = entity.get<PlayerInfo>();
                    if(info.is_remote() && info.player_idx == seat.player_idx)
                    {
                        apply_seat(p, info, seat);
                        break;
                    }
                }
            }
            break;
        }
        case MessageBase::WeaponFire: {
            m_combat.client_receive(
                p,
                payload.value<WeaponFire>(),
                p.subsystem<NetworkState>().remote_player_idx);
            break;
        }
        case MessageBase::ObjectHit: {
            m_combat.client_receive(p, payload.value<ObjectHit>());
            break;
        }
        case MessageBase::GameStateSync: {
            auto const& sync = payload.value<GameStateSync>();
            auto&       game = p.subsystem<GameState>();
            game.kind        = static_cast<GameState::kind_t>(sync.kind);
            game.phase       = static_cast<GameState::phase_t>(sync.phase);
            game.game_type   = static_cast<GameSetup::game_type_t>(sync.game_type);
            game.teams       = sync.teams != 0;
            game.winner      = sync.winner;
            game.time_left   = sync.time_left;
            game.team_scores = {sync.team_scores[0], sync.team_scores[1]};
            game.difficulty  = sync.difficulty;
            game.saves       = sync.saves;
            game.reverts     = sync.reverts;
            break;
        }
        case MessageBase::GameScoreSync: {
            auto& game = p.subsystem<GameState>();
            game.players.clear();
            for(auto const& score : payload.values<GameScoreSync>())
                game.players.push_back({
                    .player_idx = score.player_idx,
                    .team       = score.team,
                    .score      = score.score,
                    .kills      = score.kills,
                    .deaths     = score.deaths,
                });
            break;
        }
        case MessageBase::UnitVitality: {
            m_combat.client_receive(p, payload.value<UnitVitality>());
            break;
        }
        case MessageBase::ObjectImpact: {
            receive_impacts(p, payload.values<ObjectImpact>());
            break;
        }
        case MessageBase::TickSync: {
            auto const& sync      = payload.value<TickSync>();
            m_net_state.tick_base = NetworkState::TickBase{
                .tick     = sync.tick,
                .start_us = sync.start_us,
                .pause_at       = sync.pause_at,
                .speed_permille = sync.speed_permille,
                .paused         = sync.paused != 0,
            };
            m_session_start_us = sync.session_us;
            break;
        }
        case MessageBase::GrabState: {
            auto const& grab   = payload.value<GrabState>();
            u64 const   holder = own_player(p, grab.player_idx);
            if(!holder)
                break;
            u32 const object  = grab.object;
            u64       carried = 0;
            if(object != 0)
                for(auto entity : p.select<NetworkInfo>())
                    if(entity.get<NetworkInfo>().instance_id == object)
                    {
                        carried = entity.id();
                        break;
                    }
            if(auto* data = p.get<PhysicsData>(holder))
                data->grabbed = carried;
            journal("net_grab", {{"object", object}});
            break;
        }
        case MessageBase::ObjectSync: {
            u64 const now = tick_now(p);
            for(auto const& sync : payload.values<ObjectSync>())
            {
                auto& history = m_object_history[sync.net_id];
                /* Unreliable: late arrivals are dropped, not reordered */
                if(!history.empty() && history.back().tick >= sync.tick)
                    continue;
                note_lag(now, sync.tick);
                if(!history.empty())
                    note_spacing(m_object_spacing[sync.net_id], sync.tick - history.back().tick);
                history.push_back(sync);
                while(history.size() > object_history_size)
                    history.pop_front();
            }
            break;
        }
        case MessageBase::CameraSync: {
            auto const& sync = payload.value<CameraSync>();
            for(auto entity : p.select<PlayerInfo, PlayerCamera>())
            {
                auto [info, cam] = entity.components();
                if(!info.is_remote())
                {
                    bool const to_this = sync.target_player == CameraSync::self_id
                                             ? info.seat_idx == 0
                                             : joined(info) && info.player_idx == sync.target_player;
                    if(!to_this)
                        continue;
                    place_local_player(p, entity.id(), world_of(p, sync), sync.rotation);
                    break;
                }
                if(info.player_idx != sync.target_player)
                    continue;
                /* Shown a few ticks behind, between the syncs around then */
                auto& history = m_player_history[sync.target_player];
                if(history.empty() || history.back().tick < sync.tick)
                {
                    note_lag(tick_now(p), sync.tick);
                    if(!history.empty())
                        note_spacing(m_player_spacing[sync.target_player], sync.tick - history.back().tick);
                    history.push_back(sync);
                }
                while(history.size() > object_history_size)
                    history.pop_front();
                // TODO: With a fresh viewport, we should compute relevance of
                // entities Distance, visibility in frustum and projectile type
                break;
            }
            break;
        }
        case MessageBase::MapVerify: {
            /* The server switched maps */
            m_expected_fingerprint = payload.value<MapVerify>().fingerprint;
            journal(
                "net_map_expected",
                {{"fingerprint", to_string(*m_expected_fingerprint)}});
            break;
        }
        case MessageBase::GameJoin: {
            auto const&        join = payload.value<GameJoin>();
            m_expected_fingerprint  = join.fingerprint;
            m_join_confirmed        = false;
            GameEvent          ev{.type = GameEvent::MapLoadByName};
            MapLoadByNameEvent data{
                .origin   = MapLoadEvent::Remote,
                .map_name = join.map_name,
            };
            cDebug(
                "Loading map {} as requested by server({})",
                data.map_name.str(),
                remote_name());
            journal(
                "net_game_join",
                {{"map", data.map_name.str()},
                 {"server", remote_name()},
                 {"seed", join.seed}});
            m_game_bus.inject(ev, &data);
            ev.type = GameEvent::ServerConnected;
            ServerConnectedEvent connect{
                .remote = remote_name(),
                .seed   = join.seed,
            };
            m_game_bus.inject(ev, &data);
            auto& net_state = p.subsystem<NetworkState>();
            if(!net_state.local_player_name)
                net_state.local_player_name = get_random_name();
            std::string player_name = *net_state.local_player_name;
            // TODO: Create global storage for player name + save to disk?
            for(auto player : p.select<PlayerInfo>())
            {
                auto* info = p.get<PlayerInfo>(player.id());
                if(info && info->seat_idx == 0 && !info->is_remote())
                {
                    info->name = player_name;
                    break;
                }
            }
            send_single(
                m_connection,
                Message<PlayerJoin>({
                    .player_name = *blam::bl_string::from(player_name),
                }));
            m_pending_seats = {0};
            break;
        }
        case MessageBase::PlayerJoinConfirm: {
            auto const& confirm   = payload.value<PlayerJoinConfirm>();
            auto&       net_state = p.subsystem<NetworkState>();

            net_state.remote_seats[confirm.seat] = confirm.player_idx;
            m_pending_seats.erase(confirm.seat);
            if(confirm.seat == 0)
            {
                net_state.remote_player_idx = confirm.player_idx;
                m_join_confirmed            = true;
            }

            /* Seats not yet joined stay out of the server's index space */
            for(auto player : p.select<PlayerInfo>())
            {
                auto* info = p.get<PlayerInfo>(player.id());
                if(!info || info->is_remote())
                    continue;
                auto const seat  = net_state.remote_seats.find(info->seat_idx);
                info->player_idx = seat != net_state.remote_seats.end()
                                       ? seat->second
                                       : PlayerInfo::local_only_idx_base + info->seat_idx;
            }

            cDebug(
                "Received join confirmation, player_id={} seat={}",
                confirm.player_idx,
                confirm.seat);
            journal(
                "net_join_confirm",
                {{"player_idx", confirm.player_idx}, {"seat", confirm.seat}});
            break;
        }
        case MessageBase::GameEvent: {
            auto const& event  = payload.value<GameEventWrapper<char>>();
            GameEvent   event_ = event.event;
            cDebug(
                "Received GameEvent: {}", magic_enum::enum_name(event_.type));
            m_game_bus.inject(event_, const_cast<char*>(&event.data));
            break;
        }
        case MessageBase::EntitySpawn: {
            /* Tag ids are only valid against the server's tag table */
            if(m_expected_fingerprint != m_fingerprint)
            {
                cWarning("Ignoring spawns, map data differs from server");
                break;
            }
            if(drop_for_test(m_net_state.faults.drop_spawns, payload))
                break;
            for(auto const& spawn : payload.values<EntitySpawn>())
            {
                m_recent_spawns[spawn.tag.instance_id] = std::chrono::steady_clock::now();
                if(spawn.response_id != EntitySpawn::no_request)
                {
                    journal(
                        spawn.tag.instance_id ? "net_spawn_accepted"
                                              : "net_spawn_rejected",
                        {{"request_id", spawn.response_id},
                         {"net_id", spawn.tag.instance_id}});
                    auto& responses = m_net_state.spawn_responses;
                    if(responses.size() >= NetworkState::max_spawn_responses)
                        responses.erase(responses.begin());
                    responses.push_back({
                        .request_id = spawn.response_id,
                        .net_id     = spawn.tag.instance_id,
                    });
                    /* Queued behind the replacement, if there is one */
                    GameEvent          ev{.type = GameEvent::DespawnObject};
                    DespawnObjectEvent impostor{
                        .net_id = SpawnObjectEvent::impostor_net_id_base |
                                  spawn.response_id,
                    };
                    m_game_bus.inject(ev, &impostor);
                    if(spawn.tag.instance_id == 0)
                        continue;
                }
                cDebug(
                    "Server spawned {}:{} as {}",
                    blam::to_string(spawn.tag.tag_class),
                    spawn.tag.tag_id,
                    spawn.tag.instance_id);
                journal(
                    "net_object_spawn",
                    {{"net_id", spawn.tag.instance_id},
                     {"tag_class", blam::to_string(spawn.tag.tag_class)},
                     {"tag_id", spawn.tag.tag_id}});
                GameEvent        ev{.type = GameEvent::SpawnObject};
                SpawnObjectEvent local{
                    .origin   = MapLoadEvent::Remote,
                    .position = Vecf3(spawn.position),
                    .rotation = spawn.rotation,
                    .net_id   = spawn.tag.instance_id,
                };
                local.object.tag_class = blam::basic_tag_class<std::endian::little>::of(spawn.tag.tag_class);
                local.object.tag_id    = {spawn.tag.tag_id};
                local.announce         = spawn.announce != 0;
                if(spawn.held.value() != 0)
                {
                    local.held.tag_class =
                        blam::basic_tag_class<std::endian::little>::of(blam::tag_class_t::weap);
                    local.held.tag_id = spawn.held;
                }
                m_game_bus.inject(ev, &local);
            }
            break;
        }
        case MessageBase::GameLeave: {
            auto reason = std::string(payload.value<GameLeave>().reason.str());
            cWarning("Server asked us to leave: {}", reason);
            journal("net_game_leave", {{"reason", reason}});
            break;
        }
        case MessageBase::PlayerSync: {
            auto  players   = payload.values<PlayerSyncEntry>();
            auto& net_state = p.subsystem<NetworkState>();
            auto  self_idx  = net_state.remote_player_idx.value_or(0xFFFF);
            u32   existing_before = 0;
            for(auto _ : p.select<PlayerInfo>())
                ++existing_before;
            cDebug(
                "Player roster received: {} entries, self_idx={}, "
                "{} local PlayerInfo entities exist so far",
                players.size(),
                self_idx,
                existing_before);
            {
                nlohmann::json entries = nlohmann::json::array();
                for(auto const& player : players)
                    entries.push_back({
                        {"player_idx", player.player_idx},
                        {"name", player.name.str()},
                        {"loading_progress", player.loading_progress},
                        {"connected", player.connected == 0xFFFF},
                    });
                journal(
                    "net_roster",
                    {{"self_idx", self_idx},
                     {"existing_before", existing_before},
                     {"entries", std::move(entries)}});
            }

            // Build set of server-known player indices
            std::set<u32> server_indices;
            for(auto const& player : players)
                server_indices.insert(player.player_idx);

            // Remove remote entities for players no longer on server
            p.remove_entity_if([&](compo::Entity const& e) {
                auto* info = p.get<PlayerInfo>(e.id);
                if(!info || !info->is_remote())
                    return false;
                return server_indices.find(info->player_idx) ==
                       server_indices.end();
            });

            // Collect existing player indices in ECS
            std::map<u32, u64> existing; // player_idx -> entity id
            for(auto entity : p.select<PlayerInfo>())
            {
                auto* info = p.get<PlayerInfo>(entity.id());
                if(info)
                    existing[info->player_idx] = entity.id();
            }

            for(auto const& player : players)
            {
                bool is_self = own_idx(player.player_idx);

                if(auto it = existing.find(player.player_idx);
                   it != existing.end())
                {
                    // Update existing entity
                    auto* info = p.get<PlayerInfo>(it->second);
                    if(info && !info->is_remote() && !is_self)
                    {
                        cWarning(
                            "Roster player {} collides with a local seat",
                            player.player_idx);
                        continue;
                    }
                    if(info)
                    {
                        info->name             = std::string(player.name.str());
                        info->loading_progress = player.loading_progress;
                        if(info->is_remote())
                            info->spawned = player.spawned == 0xFFFF;
                    }
                    auto* net_info = p.get<NetworkInfo>(it->second);
                    if(net_info)
                    {
                        net_info->connected = player.connected == 0xFFFF;
                    }
                } else if(!is_self)
                {
                    // Create entity for remote player
                    auto  ref  = p.create_entity(shared_recipes::player_recipe);
                    auto& info = ref.get<PlayerInfo>();
                    info.name  = std::string(player.name.str());
                    info.player_idx       = player.player_idx;
                    info.loading_progress = player.loading_progress;
                    info.remote           = "remote";
                    info.seat_idx         = 0xFFFF;
                    info.spawned          = player.spawned == 0xFFFF;
                    auto& netinfo         = ref.get<NetworkInfo>();
                    netinfo.connected     = player.connected == 0xFFFF;
                }
            }
            break;
        }
        case MessageBase::UpdatePermission: {
            auto const& perm = payload.value<UpdatePermission>();
            for(auto entity : p.select<PlayerInfo>())
            {
                auto& info = entity.get<PlayerInfo>();
                if(info.player_idx != perm.player_idx)
                    continue;
                switch(perm.permission)
                {
                case UpdatePermission::Camera:
                    info.permissions.camera = perm.mode != 0;
                    info.spawned            = perm.mode != 0;
                    break;
                case UpdatePermission::Movement:
                    info.permissions.move = perm.mode != 0;
                    break;
                }
                break;
            }
            break;
        }
        default:
            break;
        }
    }

    std::string client_name(HSteamNetConnection connection)
    {
        SteamNetConnectionInfo_t conn_info;
        if(!m_impl->GetConnectionInfo(connection, &conn_info))
            return {};
        std::string out(256, '\0');
        conn_info.m_addrRemote.ToString(out.data(), out.size(), true);
        if(auto end = out.find('\0'); end != std::string::npos)
            out.resize(end);
        return out;
    }

    std::string remote_name()
    {
        return client_name(m_connection);
    }

    std::string local_name()
    {
        SteamNetworkingIPAddr addr;
        if(!m_impl->GetListenSocketAddress(m_socket, &addr))
            return {};
        std::string out(256, '\0');
        addr.ToString(out.data(), out.size(), true);
        if(auto end = out.find('\0'); end != std::string::npos)
            out.resize(end);
        return out;
    }

    /* Local test/debug journal (journal.h); wired by alloc_networking,
     * no-op when journaling is disabled. Message-level events (joins,
     * rosters, connection changes) are invisible to the GameEventBus
     * catch-all recorder, so they're recorded here at the handler sites. */
    Journal* m_journal{nullptr};

    void journal(std::string_view type, nlohmann::json data = {})
    {
        if(m_journal)
            m_journal->record(type, std::move(data));
    }

    GameEventBus&                     m_game_bus;
    NetworkState&                     m_net_state;
    net_combat::CombatSync            m_combat;
    ISteamNetworkingSockets*          m_impl{nullptr};
    ISteamNetworkingUtils*            m_utils{nullptr};
    SteamNetworkingIdentity           m_identity;
    HSteamListenSocket                m_socket{};
    HSteamNetPollGroup                m_poll_group{};
    HSteamNetConnection               m_connection{};
    std::optional<time_point>         m_connection_last_seen{};
    bool                              m_join_confirmed{false};
    bool                              m_left_server{false};
#if defined(USE_WEBRTC_TRANSPORT)
    webrtc_signaling::GatewayConnectBootstrap* m_webrtcBootstrap{nullptr};
    webrtc_signaling::GatewayConnectBootstrap* m_webrtcDirectKeepAlive{nullptr};
    std::unique_ptr<webrtc_signaling::GatewayServerRegistration> m_webrtcServer;
    std::unique_ptr<webrtc_signaling::GatewayFleetRegistration>
        m_fleetRegistration;
#endif

    /*! One of a client's seats, as a player here */
    struct remote_player_t
    {
        /*! Player info, not attached to the biped */
        ComponentRef<EntityContainer, PlayerInfo> player_info{};
        /*! Controlled by the associated player */
        compo::EntityRef<EntityContainer> biped{};
        u32                               idx{0};
        u32 grabbed{0};    /*!< Net id it was last told it carries */
        u64 last_input{0}; /*!< Newest tick of its input stream */
    };

    struct connection_state_t
    {
        std::map<u32, remote_player_t>    players; /*!< By the client's seat */
        u32                               idx{0};  /*!< Its seat 0's */
        u32                               loading_progress{};
        std::optional<time_point>         last_seen{};
        bool                              invited{false};
        bool verified{false}; /*!< Map fingerprint matched ours */
        bool tick_synced{false}; /*!< Has had the server's tick clock */
        /* Its input streams, counted since last journaled */
        u64 inputs{0};
        u64 inputs_lost{0};
        bool objects_synced{false}; /*!< Has had every moving object */
        std::chrono::steady_clock::time_point census_due{}; /*!< Unset until synced */
        /* What it was last sent of each stream, by the tick of that state:
         * moving objects by net id, players' cameras by index */
        std::map<u32, u64>        objects_sent;
        std::map<u32, CameraSync> cameras_sent;
        /* Stream states sent, held back as not yet due, and over budget,
         * since last journaled */
        u64 streams_sent{0};
        u64 streams_held{0};
        u64 streams_over_budget{0};

        /*! Its own player that `player_idx` names; messages about anyone
         *  else are ignored */
        remote_player_t* own(u32 player_idx)
        {
            for(auto& [_, player] : players)
                if(player.idx == player_idx && player.biped.exists())
                    return &player;
            return nullptr;
        }
    };
    static constexpr u32 max_client_seats = 4;

    std::map<HSteamNetConnection, connection_state_t>      m_connections{};
    std::vector<ComponentRef<EntityContainer, PlayerInfo>> m_local_player_info;
    u32                                                    m_next_remote_idx{4};

    /* For loading maps requested by the server */
    platform::url::Url m_map_directory;
    /* Game/map state */
    std::string        m_host_name;
    u32                m_seed{164829}; /*!< Randomly typed number */
    bool               m_needs_local_init{false};
    std::optional<u32> m_pending_focus{};

    static constexpr auto spawn_hold = std::chrono::seconds(5);

    /* Moving objects */
    /* How many ticks behind the server a replica shows what moves */
    static constexpr u64    interp_ticks        = 3;
    /* Bytes of players and moving objects a connection is sent a tick, ~60 KB/s */
    static constexpr size_t tick_budget = 2048;
    /* The longest a stream waits for its next state, in ticks */
    static constexpr u64    max_period  = 30;
    /* Ticks without a new state before a player counts as come to rest */
    static constexpr u64    settle_ticks = 15;
    /* Players' newest cameras, which send_streams passes on, and the
     * client tick of the newest each sender sent */
    std::map<u32, CameraSync> m_latest_cameras;
    std::map<u32, u64>        m_camera_ticks;
    /* How often each connection's traffic is journaled */
    static constexpr auto                 stats_period = std::chrono::seconds(2);
    std::chrono::steady_clock::time_point m_stats_journaled{};
    static constexpr size_t object_history_size = 32;
    u64                     m_synced_tick{0};
    std::map<u32, bool>                      m_object_asleep;
    std::map<u32, bool>                      m_object_gazing;
    /* Last seat sent per player index; the client's own, to the server */
    std::map<u32, SeatSync>                  m_sent_seats;
    std::map<u32, RiderSync>                 m_sent_riders; /* unit net id */
    static constexpr u32                     max_rider_wait = 600; /* frames */
    std::vector<std::pair<RiderSync, u32>>   m_waiting_riders;
    std::map<u32, std::string>               m_rider_stances; /* unit net id: its stance on foot */
    /* Camera last sent per player entity, and what counts as moving */
    std::map<u64, std::pair<Vecf3, Quatf>>   m_sent_cameras;
    static constexpr f32                     camera_epsilon   = .002f;
    static constexpr f32                     rotation_epsilon = 1e-6f;
    /* Weapons sent per player index, and those peers reported */
    std::map<u32, EquipmentSync>             m_sent_equipment;
    GameStateSync                            m_sent_game{};
    std::vector<GameScoreSync>               m_sent_scores;
    std::map<u64, DeviceSync>                m_sent_devices;
    std::map<u32, EquipmentSync>             m_remote_weapons;
    /* What was last asked of each remote body not shown yet, and in which load */
    std::map<u64, std::pair<u32, EquipmentSync>> m_equip_asked;
    std::vector<ObjectDespawn>               m_despawns;
    /* The object census: how often, and at most how many objects a message */
    static constexpr auto   census_period = std::chrono::seconds(2);
    static constexpr size_t census_batch  = 128;
    /* On a client: when each object arrived, and what was asked for */
    std::map<u32, std::chrono::steady_clock::time_point> m_recent_spawns;
    std::map<u32, std::chrono::steady_clock::time_point> m_requested;

    struct sync_point_t
    {
        bool                          self{false};
        u32                           peers{0};
        std::set<HSteamNetConnection> reached;
    };
    std::map<std::string, sync_point_t> m_syncs;
    std::vector<EquipWeaponEvent>            m_pickups;
    std::shared_ptr<GameEventBus::queue_type<DespawnObjectEvent>> m_despawn_queue;
    std::shared_ptr<GameEventBus::queue_type<EquipWeaponEvent>>   m_pickup_queue;
    std::map<u32, std::deque<ObjectSync>>    m_object_history;
    /* On the server, vehicles their client drivers simulate: what they sent,
     * and how many ticks it arrives behind */
    struct driven_t
    {
        std::deque<ObjectSync> history;
        f32                    lag{0.f};
        u64                    seen{0};
    };
    std::map<u32, driven_t>                  m_driven;
    std::map<u64, u64>                       m_driven_ticks; /*!< Last sent, by vehicle */
    static constexpr f32                     lag_smoothing = .1f;
    /* Ticks without word before the server takes a vehicle back */
    static constexpr u64                     driven_timeout = 30;
    /* Remote players' cameras by player index, newest last */
    std::map<u32, std::deque<CameraSync>>    m_player_history;
    /* Ticks a server state is old on arrival: raised at once, lowered
     * slowly, so interpolation stays behind what has come. Only the
     * newest state yet says, a sleeping object's is sent as it was */
    f32 m_sync_lag{0.f};
    u64 m_sync_newest{0};
    /* Ticks between the states of each object and player, see note_spacing */
    std::map<u32, f32>           m_object_spacing;
    std::map<u32, f32>           m_player_spacing;
    static constexpr f32         spacing_smoothing = .1f;
    static constexpr f32         spacing_margin    = 1.5f;
    static constexpr u64         max_spacing       = 15;
    /* Past its newest state, a stream carries on along its velocity this long */
    static constexpr libc_types::f64 max_extrapolation = 10.;
    /* A unit's gaze outlasts its last sync this long, s */
    static constexpr libc_types::f32 gaze_hold = .3f;

    /* Ticks between a stream's states, smoothed: a slow sender, or one
     * sent to rarely, is shown further behind so there is always a next
     * state to move toward. A pause (nothing moved) counts only as a slow
     * one */
    static void note_spacing(f32& spacing, u64 gap)
    {
        f32 const g = static_cast<f32>(std::min<u64>(gap, max_spacing));
        spacing     = spacing <= 0.f ? g : glm::mix(spacing, g, spacing_smoothing);
    }

    /* How far behind its newest state a stream is drawn */
    static libc_types::f64 stream_delay(std::map<u32, f32> const& spacing, u32 key)
    {
        auto it = spacing.find(key);
        f32 const gap = it == spacing.end() ? 0.f : it->second;
        return std::max(static_cast<libc_types::f64>(interp_ticks), static_cast<libc_types::f64>(gap) * spacing_margin);
    }

    void note_lag(u64 now, u64 tick)
    {
        if(tick <= m_sync_newest)
            return;
        m_sync_newest = tick;
        f32 const lag = static_cast<f32>(now > tick ? now - tick : 0);
        m_sync_lag    = lag > m_sync_lag ? lag : glm::mix(m_sync_lag, lag, lag_smoothing);
    }
    friend struct ObjectFollower;
    struct held_player_t
    {
        u32                                                 player_idx{};
        std::optional<blam::scn::player_starting_location> spawn{};
    };
    std::vector<held_player_t> m_held_players;
    std::optional<time_point>  m_release_at{};
    blam::map_container<halo_version>* m_map{nullptr};
    MapFingerprint                     m_fingerprint{};
    std::optional<MapFingerprint>      m_expected_fingerprint{};
    stl_types::math::rng               m_local_random{};

    /* Dynamic objects; the server replays these to each peer as it verifies */
    std::shared_ptr<GameEventBus::queue_type<SpawnObjectEvent>> m_spawn_queue;
    i64 m_session_start_us{0}; /*!< Server time the session began at */
    /* How far from its eye a client may grab */
    static constexpr f32 max_grab_offset = 4.f;
    /* Forge grabs: ours sent to the server, clients' carried here */
    /* What we last sent about each of our joined seats, by entity */
    struct own_seat_t
    {
        std::optional<SeatSync>      seat;
        std::optional<EquipmentSync> equipment;
        bool                         grab{false};
        bool                         moving{false}; /*!< Its camera, last frame */
        std::deque<TickInput>        inputs;
    };
    std::map<u64, own_seat_t> m_own_seats;
    std::set<u32>             m_pending_seats; /*!< Asked to join, unconfirmed */
    /* Our seat's input stream, the last few ticks of it resent */
    static constexpr size_t input_redundancy = 4;
    std::shared_ptr<GameEventBus::queue_type<PlayerTickInputEvent>> m_input_queue;
    std::vector<PlayerTickInputEvent> m_local_inputs;
    /* Physics impacts heard on the server; shaders named by clients' */
    std::shared_ptr<PhysicsBus::queue_type<Physics::Impact>> m_impact_queue;
    std::vector<Physics::Impact>                             m_impacts;
    std::map<blam::tag_index<>, blam::tagref_t>              m_impact_shaders;
    /* Tick clock sync, to clients */
    static constexpr auto tick_sync_interval = std::chrono::seconds(1);
    time_point            m_next_tick_sync{};
    bool                  m_sent_paused{false};
    std::optional<u64>    m_sent_pause_at;
    std::set<u64> m_remote_holders;
    std::vector<EntitySpawn>                                    m_dynamic_spawns;
    std::atomic<u32> m_next_net_id{0}; /*!< Stamped on whichever thread spawns */
    std::atomic<u32> m_next_request_id{0};

    clock_state_t            m_clock{};
    NetworkState::Simulation   m_applied_simulation{};
    bool          m_clock_base_journaled{false};
#if defined(USE_WEBRTC_TRANSPORT)
    std::string m_last_metadata_sent;
#endif

    /* Peer authentication, independent of how packets get there: the server's
     * keypair issues its GNS certificate, and a client that was given the
     * public key pins it for the connection. A gateway-relayed session is the
     * case that most obviously needs it, but a direct or P2P UDP session is
     * where it carries the whole guarantee, having no second encrypted hop to
     * fall back on. */
    webrtc_signaling::WebrtcAuth m_server_auth;
    Ed25519Key                   m_server_key;

    /* Client-side auth, from the join URL fragment or --server-key. */
    webrtc_signaling::WebrtcAuth m_client_auth;
    std::string                  m_expected_server_identity;
    /* Base64 of the key pinned for the next connection. SetString keeps the
     * pointer rather than copying, so this has to outlive the connect call. */
    std::string m_pinned_root_key_b64;
    bool m_relay_only{false};
};

using libc_types::f64;

using FollowerManifest = compo::SubsystemManifest<
    type_list_t<
        Model,
        ObjectPhysics,
        PlayerCamera,
        PlayerInfo,
        AnimationPlayback,
        const NetworkInfo>,
    type_list_t<const TickClock, const Networking>,
    empty_list_t>;

/*! On a client, puts what the server moves where the server had it a few
 *  ticks ago, between the snapshots around then. Ahead of input, physics
 *  and seating, so they all see this frame's pose */
struct ObjectFollower
    : compo::RestrictedSubsystem<ObjectFollower, FollowerManifest>
{
    using type  = ObjectFollower;
    using Proxy = compo::proxy_of<FollowerManifest>;

    ObjectFollower()
    {
        this->priority = 790;
    }

    static Vecf3 world_of(Proxy& p, Networking const& net, CameraSync const& sync)
    {
        if(sync.deck)
            for(auto object : p.select<Model, NetworkInfo>())
                if(object.get<NetworkInfo>().instance_id == sync.deck)
                    return Vecf3(
                        object.get<Model>().transform * Vecf4(Vecf3(sync.position), 1.f));
        return Vecf3(sync.position);
    }

    /* The two snapshots around `at`, and how far between them */
    template<typename T>
    static std::optional<std::tuple<T const&, T const&, f32>> around(
        std::deque<T> const& history, f64 at)
    {
        if(history.empty())
            return std::nullopt;
        auto next = std::find_if(history.begin(), history.end(), [at](T const& s) {
            return static_cast<f64>(s.tick) > at;
        });
        T const& to   = next == history.end() ? history.back() : *next;
        T const& from = next == history.begin() || next == history.end()
                            ? to
                            : *(next - 1);
        f64 const span = static_cast<f64>(to.tick) - static_cast<f64>(from.tick);
        f32 const alpha =
            span > 0. ? static_cast<f32>(std::clamp(
                            (at - static_cast<f64>(from.tick)) / span, 0., 1.))
                      : 1.f;
        return std::tuple<T const&, T const&, f32>{from, to, alpha};
    }

    /* Vehicles driven by players of this peer */
    static std::set<u64> driven_here(Proxy& p, bool remote)
    {
        std::set<u64> out;
        for(auto player : p.select<PlayerInfo>())
        {
            auto const& info = player.get<PlayerInfo>();
            if(info.is_remote() == remote && info.riding.driver && !info.riding.exiting)
                out.insert(info.riding.vehicle);
        }
        return out;
    }

    /* The server follows what client drivers simulate, as far behind as
     * their states arrive, and simulates the rest */
    void follow_drivers(Proxy& p, Networking const& net, TickClock const& clock)
    {
        auto const drivers = driven_here(p, true);
        u64 const  now     = clock.tick();
        for(auto object : p.select<Model, ObjectPhysics, NetworkInfo>())
        {
            auto [model, physics, info] = object.components();
            auto it = net.m_driven.find(info.instance_id);
            bool const following = info.instance_id != 0 &&
                                   drivers.contains(object.id()) &&
                                   it != net.m_driven.end() &&
                                   now - it->second.seen <= Networking::driven_timeout;
            if(!following)
            {
                if(physics.authority == ObjectPhysics::Follower)
                    physics.authority = ObjectPhysics::Simulated;
                continue;
            }
            physics.authority = ObjectPhysics::Follower;
            f64 const at = static_cast<f64>(now) + clock.alpha() -
                           static_cast<f64>(Networking::interp_ticks) - it->second.lag;
            if(auto pose = around(it->second.history, at))
            {
                auto const& [from, to, alpha] = *pose;
                Networking::follow(model, physics, from, to, alpha);
            }
        }
    }

    void start_restricted(Proxy& p, time_point const&)
    {
        auto const& net   = p.subsystem<Networking>();
        auto const& clock = p.subsystem<TickClock>();
        if(net.is_server())
            return follow_drivers(p, net, clock);
        if(!net.m_join_confirmed)
            return;
        f64 const now = static_cast<f64>(clock.tick()) + clock.alpha() - net.m_sync_lag;

        /* What this client drives, it simulates */
        auto const mine = driven_here(p, false);
        for(auto object : p.select<Model, ObjectPhysics, NetworkInfo>())
        {
            auto [model, physics, info] = object.components();
            if(physics.mass <= 0.f)
                continue;
            if(mine.contains(object.id()))
            {
                physics.authority = ObjectPhysics::Simulated;
                continue;
            }
            physics.authority = ObjectPhysics::Follower;
            auto it = net.m_object_history.find(info.instance_id);
            if(info.instance_id == 0 || it == net.m_object_history.end())
                continue;
            f64 const at = now - Networking::stream_delay(net.m_object_spacing, info.instance_id);
            if(auto pose = around(it->second, at))
            {
                auto const& [from, to, alpha] = *pose;
                Networking::follow(model, physics, from, to, alpha);
                /* A unit's stance, between standing, crouching and asleep only */
                if(auto* anim = p.get<AnimationPlayback>(object.id());
                   anim && !p.get<PlayerInfo>(object.id()) &&
                   (anim->stance == "stand" || anim->stance == "crouch" || anim->stance == "asleep"))
                    anim->stance = (to.flags & ObjectSync::asleep)     ? "asleep"
                                   : (to.flags & ObjectSync::crouched) ? "crouch"
                                                                       : "stand";
                /* And its gaze, which lapses unless the server keeps it up */
                if(auto* anim = p.get<AnimationPlayback>(object.id());
                   anim && !p.get<PlayerInfo>(object.id()) &&
                   (to.flags & (ObjectSync::aiming | ObjectSync::looking)))
                {
                    auto const gaze = [&](ObjectSync::flags_t flag, i16 const(&a)[2], i16 const(&b)[2]) {
                        if(!(to.flags & flag))
                            return Vecf3{};
                        Vecf3 const v = (from.flags & flag)
                                            ? glm::mix(ObjectSync::unpack(a), ObjectSync::unpack(b), alpha)
                                            : ObjectSync::unpack(b);
                        return glm::dot(v, v) > 1e-6f ? glm::normalize(v) : ObjectSync::unpack(b);
                    };
                    anim->gaze(
                        gaze(ObjectSync::aiming, from.aim, to.aim),
                        gaze(ObjectSync::looking, from.look, to.look),
                        Networking::gaze_hold);
                }
                /* Run dry: carry on as it was going, a little way */
                if(f64 const past = at - static_cast<f64>(to.tick); past > 0. && !(to.flags & ObjectSync::sleeping))
                {
                    model.position += Vecf3(to.linear_velocity) * static_cast<f32>(
                        std::min(past, Networking::max_extrapolation) * TickClock::tick_seconds);
                    model.update_matrix();
                }
            }
        }

        for(auto player : p.select<PlayerInfo, PlayerCamera>())
        {
            auto [info, cam] = player.components();
            if(!info.is_remote())
                continue;
            auto it = net.m_player_history.find(info.player_idx);
            if(it == net.m_player_history.end())
                continue;
            f64 const at = now - Networking::stream_delay(net.m_player_spacing, info.player_idx);
            if(auto pose = around(it->second, at))
            {
                auto const& [from, to, alpha] = *pose;
                /* Each off its deck as the deck stands this frame */
                cam.camera.position =
                    glm::mix(world_of(p, net, from), world_of(p, net, to), alpha);
                /* Run dry: carry on along its last step, a little way */
                if(f64 const past = at - static_cast<f64>(to.tick);
                   past > 0. && it->second.size() >= 2 && to.deck == 0)
                {
                    auto const& prev = it->second[it->second.size() - 2];
                    f64 const   span = static_cast<f64>(to.tick - prev.tick);
                    if(span > 0. && span <= static_cast<f64>(Networking::max_spacing))
                        cam.camera.position += (Vecf3(to.position) - Vecf3(prev.position)) *
                                               static_cast<f32>(std::min(past, Networking::max_extrapolation) / span);
                }
                cam.camera.rotation = glm::slerp(from.rotation, to.rotation, alpha);
                info.mode.physics   = to.flags & CameraSync::physics;
            }
        }
    }
};

#endif

#include "components.h"

u32 PlayerRoster::player_count()
{
    u32 count = 0;
    for(auto _ : m_container.select<PlayerInfo>())
        ++count;
    return count;
}

std::vector<NetworkState::RosterEntry> PlayerRoster::roster(
    std::optional<u32> self_idx)
{
    std::vector<NetworkState::RosterEntry> entries;
    for(auto player : m_container.select<PlayerInfo>())
    {
        auto* info = m_container.get<PlayerInfo>(player.id());
        if(!info)
            continue;
        entries.push_back(
            NetworkState::RosterEntry{
                .name             = info->name,
                .remote_idx       = info->player_idx,
                .loading_progress = info->loading_progress,
                .is_self =
                    self_idx.has_value() && (info->player_idx == *self_idx),
            });
    }
    return entries;
}

void alloc_networking(
    compo::EntityContainer& e, std::string const& gateway_auth_key)
{
    ProfContext _;
    e.register_subsystem_inplace<NetworkState>();
    e.register_subsystem_inplace<PlayerRoster>(std::ref(e));
#if defined(USE_NETWORKING)
    auto& networking = e.register_subsystem_inplace<Networking>(
        std::ref(e.subsystem_cast<GameEventBus>()),
        std::ref(e.subsystem_cast<NetworkState>()),
        gateway_auth_key);
    networking.m_journal = &e.subsystem_cast<Journal>();
    e.register_subsystem_inplace<ObjectFollower>();
    alloc_network_relevance(e);
    networking.m_impact_queue =
        e.subsystem_cast<PhysicsBus>().addQueuedEventFunction<Physics::Impact>(
            0, [&networking](Physics::Event&, Physics::Impact* impact) {
                if(networking.is_server())
                    networking.m_impacts.push_back(*impact);
            });
#endif
}
