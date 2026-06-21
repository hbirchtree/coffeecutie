#include "networking.h"
#include "blam/volta/blam_scenario.h"
#include "peripherals/typing/vectors/glm_vector_types.h"
#include "types.h"

#include <coffee/core/CProfiling>
#include <coffee/core/files/cfiles.h>

using Coffee::ProfContext;

#if defined(USE_NETWORKING)

#include "components.h"
#include "data.h"
#include "selected_version.h"

#include <GameNetworkingSockets/steam/isteamnetworkingsockets.h>
#include <GameNetworkingSockets/steam/isteamnetworkingutils.h>
#include <GameNetworkingSockets/steam/steamnetworkingsockets.h>
#include <coffee/components/restricted_subsystem.h>

#include <fmt_extensions/format.h>
#include <fmt_extensions/url_types.h>

using NetworkingManifest = compo::SubsystemManifest<
    type_list_t<PlayerInfo, NetworkInfo, PlayerCamera>,
    type_list_t<NetworkState>,
    type_list_t<comp_app::ScreenshotProvider>>;

/* Lane 0 should be used for most common packets */
constexpr u32 FRAME_UPDATE_LANE = 0;
constexpr u32 EVENT_LANE        = 1;

struct Networking;

namespace {

const std::map<std::string_view, std::pair<Vecf3, Quatf>> birds_eye_positions = {
    {"bloodgulch", {Vecf3{115.2, -153.9, 11.1}, Quatf{0.96461415, 0.16558254, 0.20222986, 0.034714244}}},
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

template<typename T>
struct Message;

struct MessageBase
{
    enum Type : u32
    {
        None,

        /* Control */
        GameJoin,
        GameLoadState,
        PlayerJoin,
        PlayerJoinConfirm,
        GameEvent,
        GameLeave,

        UpdatePermission,

        /* Replication */
        CameraSync,
        EntitySpawn,
        PlayerSpawn,
        ObjectSync,

        /* Debug */
        Screenshot,

        /* Roster */
        PlayerSync,
    } type{None};

    u32 request{};

    enum Flags : u32
    {
        NoFlags   = 0x0,
        RPC       = 0x1,
        Multiple  = 0x2,
        Replicate = 0x4,
        Enforce   = 0x8,
    } flags{NoFlags};

    u32 num_values{1};

    template<typename T>
    T const& value() const;

    template<typename T>
    gsl::span<const T> values() const;
};

C_FLAGS(MessageBase::Flags, u32)

template<typename T>
struct Message : MessageBase
{
    Message(T&& data)
        : MessageBase{T::message_type}
        , data(std::move(data))
    {
    }

    T data;
};

template<typename T>
T const& MessageBase::value() const
{
    Message<T> const* impl = static_cast<Message<T> const*>(this);
    return impl->data;
}

template<typename T>
gsl::span<const T> MessageBase::values() const
{
    Message<T> const* impl = static_cast<Message<T> const*>(this);
    return gsl::make_span(&impl->data, num_values);
}

struct GameJoin
{
    static constexpr auto message_type = MessageBase::GameJoin;

    blam::bl_string map_name;
    u32             seed{0};
};

struct GameLoadState
{
    static constexpr auto message_type = MessageBase::GameLoadState;

    u32 progress{0};
};

struct GameLeave
{
    static constexpr auto message_type = MessageBase::GameLeave;

    blam::bl_string_var<128> reason;
};

struct UpdatePermission
{
    static constexpr auto message_type = MessageBase::UpdatePermission;

    u32 player_idx{};

    enum Permission : libc_types::u16
    {
        Movement,
        Camera,
    } permission;

    u16 mode{0};
};

struct PlayerJoin
{
    static constexpr auto message_type = MessageBase::PlayerJoin;

    blam::bl_string player_name;
};

struct PlayerJoinConfirm
{
    static constexpr auto message_type = MessageBase::PlayerJoinConfirm;

    u32 player_idx{0};
};

template<typename T>
struct GameEventWrapper
{
    static constexpr auto message_type = MessageBase::GameEvent;

    GameEvent event;
    T         data;
};

struct alignas(8) CameraSync
{
    static constexpr auto message_type = MessageBase::CameraSync;
    static constexpr u32  self_id      = 0xFFFF;

    Vecf4 position;
    Quatf rotation;
    u32   target_player{0xFFFF};
    f32   fade{1.f};
};

struct alignas(8) Screenshot
{
    static constexpr auto message_type = MessageBase::Screenshot;

    pix_fmt format{pix_fmt::None};
};

struct alignas(8) EntityTag
{
    blam::tagref_t object;
    u32            instance_id;
};

struct alignas(8) EntitySpawn
{
    static constexpr auto message_type = MessageBase::EntitySpawn;

    EntityTag tag;
    i32       permutation{-1};
};

struct alignas(8) PlayerSpawn
{
    static constexpr auto message_type = MessageBase::PlayerSpawn;

    typing::vector_types::Vecf4 position;
    u32                         target_player{0xFFFF}; // Targets local players
    f32                         facing;
};

struct alignas(8) PlayerSyncEntry
{
    static constexpr auto message_type = MessageBase::PlayerSync;

    blam::bl_string name;
    u32             player_idx{0};
    u32             loading_progress{100};
    u32             connected{0x0};
};

static_assert(sizeof(PlayerSyncEntry) == 48);

/*
 * Some ground rules for networking:
 *  - We expect little-endian integers
 *  - We expect IEEE-754 floats/doubles
 *  - Padding should be the same on all platforms
 */
static_assert(sizeof(Message<GameJoin>) == 52);
static_assert(sizeof(Message<PlayerJoin>) == 48);
static_assert(sizeof(Message<CameraSync>) == 56);
static_assert(sizeof(Message<u32>) == 20);
static_assert(sizeof(Message<u64>) == 24);

bool is_client_network_event(GameEvent const& event)
{
    switch(event.type)
    {
    case GameEvent::ServerCameraControl:
    case GameEvent::ServerPlayerStateUpdate:
    case GameEvent::ServerStateUpdate:
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

    template<typename T>
    bool forward_game_event(GameEvent& event, const void* data)
    {
        if(event.type != T::event_type)
            return false;
        auto message = Message<GameEventWrapper<T>>({
            .event = event,
            .data  = *static_cast<T const*>(data),
        });
        if(m_socket)
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
            .seed     = m_seed,
        });
    }

    void update_player_counts()
    {
        GameEvent         update{.type = GameEvent::ServerStateUpdate};
        ServerStateUpdate data = {
            .type      = ServerStateUpdate::PlayerCount,
            .num_field = static_cast<i32>(m_connections.size() + 1),
        };
        m_game_bus.inject(update, &data);
        data = {
            .type      = ServerStateUpdate::PlayerMaxCount,
            .num_field = 16,
        };
        m_game_bus.inject(update, &data);
    }

    void send_player_roster(i32 player_idx = -1)
    {
        std::vector<PlayerSyncEntry> entries;
        for(auto const& local : m_local_player_info)
        {
            if(!local)
                continue;
            auto const& info = (*local);
            entries.push_back({
                .name             = *blam::bl_string::from(info.name),
                .player_idx       = info.player_idx,
                .loading_progress = 100,
                .connected        = 0xFFFF,
            });
        }
        for(auto const& [_, state] : m_connections)
        {
            if(!state.player_info)
                continue;
            if(player_idx != -1 && state.idx != player_idx)
                continue;
            auto const& info = (*state.player_info);
            entries.push_back({
                .name             = *blam::bl_string::from(info.name),
                .player_idx       = state.idx,
                .loading_progress = state.loading_progress,
                .connected =
                    state.biped.get<NetworkInfo>().connected ? 0xFFFFu : 0x0u,
            });
        }
        MessageBase header{.type = MessageBase::PlayerSync};
        send_all(std::move(header), gsl::make_span(entries));
    }

    bool players_ready() const
    {
        if(m_socket && !m_map)
            return false;
        if(m_connections.empty())
            return m_socket && m_map;
        for(auto const& [_, state] : m_connections)
        {
            if(state.loading_progress < 100)
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

    void player_init(compo::EntityContainer& e, PlayerInfo& player)
    {
        using namespace std::chrono_literals;

        player.permissions.camera = false;

        /* Write birds-eye position to PlayerCamera + mark dirty */
        for(auto& entity : e.select<PlayerInfo>())
        {
            auto* info = e.get<PlayerInfo>(entity.id);
            if(info != &player)
                continue;
            auto* cam = e.get<PlayerCamera>(entity.id);
            if(cam)
                std::tie(cam->camera->position, cam->camera->rotation) =
                    birds_eye_for_map(m_map->internal_name());
            auto* net = e.get<NetworkInfo>(entity.id);
            if(net)
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
            auto spawn_idx = m_local_random.rand<u32>(0, spawns.size());
            return spawns[spawn_idx];
        }();

        if(!spawn_loc)
            return;

        auto pidx = player.player_idx;
        rq::runtime_queue::QueueImmediate(
            rq::runtime_queue::GetCurrentQueue().value(),
            5s,
            [this, &e, pidx, spawn = *spawn_loc]() {
                for(auto& entity : e.select<PlayerInfo>())
                {
                    auto* info = e.get<PlayerInfo>(entity.id);
                    if(!info || info->player_idx != pidx)
                        continue;
                    auto* cam = e.get<PlayerCamera>(entity.id);
                    if(cam)
                    {
                        cam->camera->position = spawn.pos;
                        cam->camera->rotation =
                            glm::angleAxis(glm::pi<f32>() - spawn.rot, Vecf3{0.f, 1.f, 0.f});
                    }
                    info->permissions.camera = true;
                    auto* net                = e.get<NetworkInfo>(entity.id);
                    if(net)
                    {
                        net->changes.viewport    = true;
                        net->changes.permissions = true;
                    }
                    break;
                }
            })
            .assume_value();
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

    Networking(GameEventBus& game_bus, NetworkState& net_state)
        : m_game_bus(game_bus)
        , m_net_state(net_state)
    {
        network_instance = this;

        m_local_random.seed_automatically();

        this->priority = 1280;
        m_identity.Clear();
        m_identity.SetLocalHost();
        SteamNetworkingErrMsg ec;
        if(!GameNetworkingSockets_Init(nullptr, ec))
        {
            cWarning("Failed to init networking: {}", ec);
            return;
        }
        m_impl  = SteamNetworkingSockets();
        m_utils = SteamNetworkingUtils();

        m_game_bus.addEventFunction<ServerConnectEvent>(
            0, [this](GameEvent&, ServerConnectEvent* connect) {
                cDebug("Connection requested to server: {}", connect->remote);
                if(connect->type == ServerConnectEvent::Peer)
                    connect_symmetric(connect->remote);
                else if(connect->type == ServerConnectEvent::Server)
                    connect_server(connect->remote);
                else
                    create_server(connect->remote);
            });
        m_game_bus.addEventFunction<MapListingEvent>(
            0, [this](GameEvent&, MapListingEvent* listing) {
                m_map_directory = listing->directory;
            });
        m_game_bus.addEventFunction<MapLoadEvent>(
            0, [this](GameEvent&, MapLoadEvent* load) {
                /* Only use this callback if we're the server */
                if(!load->file || !m_socket)
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
                    if(state.player_info)
                        (*state.player_info).loading_progress = 0;
                }
            });
        m_game_bus.addEventFunction<ServerCameraControl>(
            0, [this](GameEvent&, ServerCameraControl* cam) {
                if(!m_socket)
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
                 if(m_socket)
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
        m_game_bus.addEventFunction<MapDataLoadEvent>(
            0, [this](GameEvent&, MapDataLoadEvent*) {
                if(!m_socket)
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
                m_map = finished->container;
            });
        m_game_bus.addEventData({
            0,
            [this](GameEvent& ev, const void*) {
                if(ev.type != GameEvent::MapAllLoaded)
                    return;
                if(m_socket)
                    m_needs_local_init = true;
                if(!m_socket)
                {
                    send_single(
                        m_connection,
                        Message<GameLoadState>({
                            .progress = 100,
                        }));
                }
            },
        });

        m_utils->SetDebugOutputFunction(
            k_ESteamNetworkingSocketsDebugOutputType_Everything,
            [](ESteamNetworkingSocketsDebugOutputType, const char* message) {
                cDebug("Networking: {}", message);
            });
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

    void connect_symmetric(std::string const& /*remote*/)
    {
        if(m_connection)
        {
            cWarning("Already connected to {}", remote_name());
            return;
        }

        m_utils->SetGlobalConfigValueString(
            k_ESteamNetworkingConfig_P2P_STUN_ServerList,
            "stun.l.google.com:19302");
        m_utils->SetGlobalConfigValueInt32(
            k_ESteamNetworkingConfig_P2P_Transport_ICE_Enable,
            k_nSteamNetworkingConfig_P2P_Transport_ICE_Enable_All);
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

        m_utils->SetGlobalConfigValueString(
            k_ESteamNetworkingConfig_P2P_STUN_ServerList,
            "stun.l.google.com:19302");
        m_utils->SetGlobalConfigValueInt32(
            k_ESteamNetworkingConfig_P2P_Transport_ICE_Enable,
            k_nSteamNetworkingConfig_P2P_Transport_ICE_Enable_All);
        SteamNetworkingIPAddr server;
        if(!server.ParseString(remote.c_str()))
        {
            cWarning("Failed to parse server IP: {}", remote);
            m_net_state.client_state = NetworkState::ClientState::Error;
            return;
        }
        auto config = create_callbacks();
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

    void stop_server()
    {
        m_impl->CloseListenSocket(m_socket);
        m_impl->DestroyPollGroup(m_poll_group);
        m_net_state.server_state = NetworkState::ServerState::Error;
    }

    void connection_status_change(
        SteamNetConnectionStatusChangedCallback_t* info)
    {
        cDebug(
            "Network state change: {}: {} -> {}",
            info->m_hConn,
            magic_enum::enum_name(info->m_eOldState),
            magic_enum::enum_name(info->m_info.m_eState));

        if(m_socket && m_socket != k_HSteamListenSocket_Invalid)
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
            /* If the server doesn't have an active map, don't send the game
             * invite yet */
            if(!m_map)
                break;
            send_single(info->m_hConn, generate_game_join());
            m_connections[info->m_hConn].loading_progress = 0;
            m_connections[info->m_hConn].last_seen        = std::nullopt;
            break;
        }

        case k_ESteamNetworkingConnectionState_ProblemDetectedLocally: {
            cDebug(
                "Problem detected with connection to {}",
                client_name(info->m_hConn));
            if(auto& player = m_connections[info->m_hConn].biped)
            {
                player.get<NetworkInfo>().connected = false;
                send_player_roster();
            }
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
            cDebug("Connection to server/peer established ({})", remote_name());
            m_connection_last_seen = std::nullopt;
            break;
        }
        case k_ESteamNetworkingConnectionState_ProblemDetectedLocally:
            cDebug(
                "Problem with connection to server {}",
                client_name(info->m_hConn));
            m_connection_last_seen = compo::clock::now();
            break;
        case k_ESteamNetworkingConnectionState_ClosedByPeer:
        case k_ESteamNetworkingConnectionState_Dead: {
            m_net_state.client_state = NetworkState::ClientState::Disconnecting;
            cDebug("Disonnected from server/peer ({})", remote_name());
            m_impl->CloseConnection(
                info->m_hConn,
                0,
                nullptr,
                info->m_info.m_eState ==
                    k_ESteamNetworkingConnectionState_ClosedByPeer);
            m_connection = {};
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
        m_impl->SendMessages(messages.size(), messages.data(), nullptr);
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
        if(m_socket)
        {
            if(m_local_player_info.empty())
            {
                for(auto& player : p.select<PlayerInfo>())
                {
                    auto  ref      = p.ref(player);
                    auto  info_ref = ref.ref<PlayerInfo>();
                    auto& info     = (*info_ref);
                    if(info.player_idx == 0)
                        info.name = m_host_name;
                    else
                        info.name = get_random_name();
                    m_local_player_info.push_back(info_ref);
                }
            }

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
                auto& pinfo = m_connections[message->m_conn].player_info;
                if(pinfo)
                    shared_receive_payload(p, *pinfo, payload);
                server_receive_payload(p, message->m_conn, payload);
                message->Release();
            }

            if(m_needs_local_init && !m_local_player_info.empty())
            {
                m_needs_local_init = false;
                for(auto& pi : m_local_player_info)
                {
                    if(pi)
                        player_init(p.underlying(), *pi);
                }
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
                for(auto& entity : p.select<PlayerInfo>())
                {
                    auto* info = p.get<PlayerInfo>(entity.id);
                    if(!info)
                        continue;
                    if(info->seat_idx == 0)
                    {
                        old_seat0     = info;
                        old_seat0_cam = p.get<PlayerCamera>(entity.id);
                        old_seat0_id  = entity.id;
                    }
                    if(info->player_idx == target_pidx)
                    {
                        target     = info;
                        target_cam = p.get<PlayerCamera>(entity.id);
                        target_id  = entity.id;
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
            for(auto& entity : p.select<PlayerInfo>())
            {
                auto* info = p.get<PlayerInfo>(entity.id);
                auto* net  = p.get<NetworkInfo>(entity.id);
                auto* cam  = p.get<PlayerCamera>(entity.id);
                if(!info || !net || !cam)
                    continue;

                if(net->changes.permissions && info->is_remote())
                {
                    for(auto& [conn, state] : m_connections)
                    {
                        if(state.idx != info->player_idx)
                            continue;
                        send_permission(
                            conn,
                            info->player_idx,
                            UpdatePermission::Camera,
                            info->permissions.camera ? 1 : 0);
                        break;
                    }
                    net->changes.permissions = false;
                }

                if(net->changes.viewport && info->is_remote())
                {
                    for(auto& [conn, state] : m_connections)
                    {
                        if(state.idx != info->player_idx)
                            continue;
                        send_single(
                            conn,
                            Message<CameraSync>({
                                .position = Vecf4(cam->camera->position, 0),
                                .rotation = cam->camera->rotation,
                                .target_player = info->player_idx,
                            }));
                        break;
                    }
                    net->changes.viewport = false;
                }
            }
        }
        if(m_connection)
        {
            if(!m_client_player)
            {
                for(auto& player : p.select<PlayerInfo>())
                {
                    auto  ref   = p.ref(player);
                    auto& pinfo = ref.get<PlayerInfo>();
                    if(pinfo.is_remote() || pinfo.seat_idx != 0)
                        continue;
                    m_client_player = ref;
                    break;
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
                if(m_client_player)
                    shared_receive_payload(
                        p, m_client_player.get<PlayerInfo>(), payload);
                client_receive_payload(p, payload);
                message->Release();
            }
        }
        m_impl->RunCallbacks();
    }

    void server_close_peer_connection(
        HSteamNetConnection connection, int code, bool linger)
    {
        if(auto& player = m_connections[connection].biped)
            player.get<NetworkInfo>().connected = false;
        cDebug(
            "peer={} disconnected ({})", connection, client_name(connection));
        if(auto it = m_connections.find(connection); it != m_connections.end())
        {
            auto& player = it->second.player_info;
            if(player)
            {
                player.m_ref->remove_entity_if(
                    [&player](compo::Entity const& e) {
                        return player.m_id == e.id;
                    });
            }
        }
        m_connections.erase(connection);
        send_player_roster();
        m_impl->CloseConnection(connection, code, nullptr, linger);
    }

    bool shared_receive_payload(
        Proxy& p, PlayerInfo& self, MessageBase const& payload)
    {
        switch(payload.type)
        {
        case MessageBase::CameraSync: {
            auto const& sync = payload.value<CameraSync>();
            for(auto const& pi : p.select<PlayerInfo>())
            {
                auto* player_info = p.get<PlayerInfo>(pi.id);
                bool  match =
                    (sync.target_player == CameraSync::self_id)
                         ? (player_info == &self)
                         : (player_info->player_idx == sync.target_player);
                if(!match)
                    continue;
                auto* cam = p.get<PlayerCamera>(pi.id);
                if(cam)
                {
                    cam->camera->position = Vecf3(sync.position);
                    cam->camera->rotation = sync.rotation;
                }
                break;
            }
            break;
        }
        default:
            return false;
        }
        return true;
    }

    void server_receive_payload(
        Proxy& p, HSteamNetConnection connection, MessageBase const& payload)
    {
        auto& player_info = m_connections[connection];
        switch(payload.type)
        {
        case MessageBase::PlayerJoin: {
            auto const& player_join = payload.value<PlayerJoin>();
            cDebug("Player joined: {}", player_join.player_name.str());
            auto ref = p.create_entity(
                compo::EntityRecipe{
                    {
                        compo::type_hash_v<PlayerInfo>(),
                        compo::type_hash_v<NetworkInfo>(),
                        compo::type_hash_v<PlayerCamera>(),
                        compo::type_hash_v<SoundEffects>(),
                    },
                    PlayerBiped,
                });
            player_info.player_info = ref.ref<PlayerInfo>();
            player_info.biped       = ref;

            auto& net_info     = ref.get<NetworkInfo>();
            net_info.connected = true;

            auto& info = (*player_info.player_info);
            info.name  = player_join.player_name.str();

            info.remote     = client_name(connection);
            info.player_idx = player_info.idx;

            send_single(
                connection,
                Message<PlayerJoinConfirm>({
                    .player_idx = player_info.idx,
                }));
            send_player_roster(player_info.idx);
            update_player_counts();
            send_player_roster();
            break;
        }
        case MessageBase::GameLoadState: {
            auto const& event = payload.value<GameLoadState>();
            cDebug(
                "Player {} is at {}% loaded",
                m_connections[connection].idx,
                event.progress);
            m_connections[connection].loading_progress = event.progress;
            if(auto& pi = m_connections[connection].player_info; pi)
                (*pi).loading_progress = event.progress;
            if(event.progress != 100)
                break;
            if(player_info.player_info)
                player_init(p.underlying(), *player_info.player_info);
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

    void client_receive_payload(Proxy& p, MessageBase const& payload)
    {
        switch(payload.type)
        {
        case MessageBase::GameJoin: {
            auto const&        join = payload.value<GameJoin>();
            GameEvent          ev{.type = GameEvent::MapLoadByName};
            MapLoadByNameEvent data{
                .origin   = MapLoadEvent::Remote,
                .map_name = join.map_name,
            };
            cDebug(
                "Loading map {} as requested by server({})",
                data.map_name.str(),
                remote_name());
            m_game_bus.inject(ev, &data);
            ev.type = GameEvent::ServerConnected;
            ServerConnectedEvent connect{
                .remote = remote_name(),
                .seed   = join.seed,
            };
            m_game_bus.inject(ev, &data);
            std::string player_name = get_random_name();
            // TODO: Create global storage for player name + save to disk?
            send_single(
                m_connection,
                Message<PlayerJoin>({
                    .player_name = *blam::bl_string::from(player_name),
                }));
            break;
        }
        case MessageBase::PlayerJoinConfirm: {
            auto const& confirm   = payload.value<PlayerJoinConfirm>();
            auto&       net_state = p.subsystem<NetworkState>();

            net_state.remote_player_idx = confirm.player_idx;

            // Map local player 0 to server-assigned index
            for(auto& player : p.select<PlayerInfo>())
            {
                auto* info = p.get<PlayerInfo>(player.id);
                if(info && info->seat_idx == 0 && !info->is_remote())
                {
                    info->player_idx = confirm.player_idx;
                    break;
                }
            }

            cDebug("Received join confirmation, player_id={}", confirm.player_idx);
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
            auto spawns = payload.values<EntitySpawn>();
            cDebug("Server is requesting that we spawn:");
            for(auto const& spawn : spawns)
                cDebug(
                    " - {}:{} / {}",
                    blam::to_string(spawn.tag.object.tag_class),
                    spawn.tag.object.tag_id,
                    spawn.tag.instance_id);
            break;
        }
        case MessageBase::PlayerSync: {
            cDebug("Player roster received");
            auto  players   = payload.values<PlayerSyncEntry>();
            auto& net_state = p.subsystem<NetworkState>();
            auto  self_idx  = net_state.remote_player_idx.value_or(0xFFFF);

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
            for(auto& entity : p.select<PlayerInfo>())
            {
                auto* info = p.get<PlayerInfo>(entity.id);
                if(info)
                    existing[info->player_idx] = entity.id;
            }

            for(auto const& player : players)
            {
                bool is_self = (player.player_idx == self_idx);

                if(auto it = existing.find(player.player_idx);
                   it != existing.end())
                {
                    // Update existing entity
                    auto* info = p.get<PlayerInfo>(it->second);
                    if(info)
                    {
                        info->name             = std::string(player.name.str());
                        info->loading_progress = player.loading_progress;
                    }
                    auto* net_info = p.get<NetworkInfo>(it->second);
                    if(net_info)
                    {
                        net_info->connected = player.connected == 0xFFFF;
                    }
                } else if(!is_self)
                {
                    // Create entity for remote player
                    auto ref = p.create_entity(
                        compo::EntityRecipe{
                            {
                                compo::type_hash_v<PlayerInfo>(),
                                compo::type_hash_v<NetworkInfo>(),
                                compo::type_hash_v<PlayerCamera>(),
                                compo::type_hash_v<SoundEffects>(),
                            },
                            ObjectGC | PlayerBiped,
                        });
                    auto& info            = ref.get<PlayerInfo>();
                    info.name             = std::string(player.name.str());
                    info.player_idx       = player.player_idx;
                    info.loading_progress = player.loading_progress;
                    info.remote           = "remote";
                    info.seat_idx         = 0xFFFF;
                    auto& netinfo         = ref.get<NetworkInfo>();
                    netinfo.connected     = player.connected == 0xFFFF;
                }
            }
            break;
        }
        case MessageBase::UpdatePermission: {
            auto const& perm = payload.value<UpdatePermission>();
            for(auto& entity : p.select<PlayerInfo>())
            {
                auto* info = p.get<PlayerInfo>(entity.id);
                if(!info || info->player_idx != perm.player_idx)
                    continue;
                switch(perm.permission)
                {
                case UpdatePermission::Camera:
                    info->permissions.camera = perm.mode != 0;
                    break;
                case UpdatePermission::Movement:
                    info->permissions.move = perm.mode != 0;
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

    void end_restricted(Proxy&, time_point const&)
    {
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

    GameEventBus&                     m_game_bus;
    NetworkState&                     m_net_state;
    ISteamNetworkingSockets*          m_impl{nullptr};
    ISteamNetworkingUtils*            m_utils{nullptr};
    SteamNetworkingIdentity           m_identity;
    HSteamListenSocket                m_socket{};
    HSteamNetPollGroup                m_poll_group{};
    HSteamNetConnection               m_connection{};
    std::optional<time_point>         m_connection_last_seen{};
    compo::EntityRef<EntityContainer> m_client_player{};

    struct connection_state_t
    {
        /*! Player info, not attached to the biped */
        ComponentRef<EntityContainer, PlayerInfo> player_info{};
        /*! Controlled by the associated player */
        compo::EntityRef<EntityContainer> biped{};
        u32                               idx{0};
        u32                               loading_progress{};
        std::optional<time_point>         last_seen{};
    };

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
    blam::map_container<halo_version>* m_map{nullptr};
    stl_types::math::rng               m_local_random{};
};

#endif

#include "components.h"

u32 PlayerRoster::player_count()
{
    u32 count = 0;
    for(auto& _ : m_container.select<PlayerInfo>())
        ++count;
    return count;
}

std::vector<NetworkState::RosterEntry> PlayerRoster::roster(
    std::optional<u32> self_idx)
{
    std::vector<NetworkState::RosterEntry> entries;
    for(auto& player : m_container.select<PlayerInfo>())
    {
        auto* info = m_container.get<PlayerInfo>(player.id);
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

void alloc_networking(compo::EntityContainer& e)
{
    ProfContext _;
    e.register_subsystem_inplace<NetworkState>();
    e.register_subsystem_inplace<PlayerRoster>(std::ref(e));
#if defined(USE_NETWORKING)
    e.register_subsystem_inplace<Networking>(
        std::ref(e.subsystem_cast<GameEventBus>()),
        std::ref(e.subsystem_cast<NetworkState>()));
#endif
}
