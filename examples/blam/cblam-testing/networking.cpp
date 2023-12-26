#include "networking.h"

#include "components.h"
#include "data.h"

#include <GameNetworkingSockets/steam/isteamnetworkingsockets.h>
#include <GameNetworkingSockets/steam/isteamnetworkingutils.h>
#include <GameNetworkingSockets/steam/steamnetworkingsockets.h>
#include <coffee/components/restricted_subsystem.h>

using NetworkingManifest
    = compo::SubsystemManifest<empty_list_t, empty_list_t, empty_list_t>;

/* Lane 0 should be used for most common packets */
constexpr u32 FRAME_UPDATE_LANE = 0;
constexpr u32 EVENT_LANE        = 1;

struct Networking : compo::RestrictedSubsystem<Networking, NetworkingManifest>
{
    using type  = Networking;
    using Proxy = compo::proxy_of<NetworkingManifest>;

    Networking(GameEventBus& game_bus) : m_game_bus(game_bus)
    {
        this->priority = 1280;
        m_identity.Clear();
        m_identity.SetLocalHost();
        SteamNetworkingErrMsg ec;
        if(!GameNetworkingSockets_Init(nullptr, ec))
        {
            cDebug("Failed to init networking: {}", ec);
            return;
        }
        m_impl  = SteamNetworkingSockets();
        m_utils = SteamNetworkingUtils();

        m_game_bus.addEventFunction<ServerConnectEvent>(
            0, [this](GameEvent&, ServerConnectEvent* connect) {
                cDebug("Connection requested to server: {}", connect->remote);
                if(connect->type == ServerConnectEvent::Peer)
                    connect_symmetric(connect->remote);
                else
                    connect_server(connect->remote);
            });
    }

    void connect_symmetric(std::string const& remote)
    {
        m_utils->SetGlobalConfigValueString(
            k_ESteamNetworkingConfig_P2P_STUN_ServerList,
            "stun.l.google.com:19302");
        m_utils->SetGlobalConfigValueInt32(
            k_ESteamNetworkingConfig_P2P_Transport_ICE_Enable,
            k_nSteamNetworkingConfig_P2P_Transport_ICE_Enable_All);
        m_utils->SetGlobalConfigValueInt32(
            k_ESteamNetworkingConfig_LogLevel_P2PRendezvous,
            k_ESteamNetworkingSocketsDebugOutputType_Verbose);
        SteamNetworkingConfigValue_t opt;
        opt.SetInt32(k_ESteamNetworkingConfig_SymmetricConnect, 1);
        m_socket     = m_impl->CreateListenSocketP2P(0, 1, &opt);
        m_poll_group = m_impl->CreatePollGroup();
    }

    void connect_server(std::string const& remote)
    {
        m_utils->SetGlobalConfigValueString(
            k_ESteamNetworkingConfig_P2P_STUN_ServerList,
            "stun.l.google.com:19302");
        m_utils->SetGlobalConfigValueInt32(
            k_ESteamNetworkingConfig_P2P_Transport_ICE_Enable,
            k_nSteamNetworkingConfig_P2P_Transport_ICE_Enable_All);
        SteamNetworkingIPAddr server;
        server.ParseString(remote.c_str());
        m_connection = m_impl->ConnectByIPAddress(server, 0, nullptr);
        if(m_connection == k_HSteamNetConnection_Invalid)
        {
            cDebug("Failed to set up connection to: {}", remote);
            return;
        }
        configure_weights(m_connection);
        SteamNetConnectionInfo_t connect_info;
        if(!m_impl->GetConnectionInfo(m_connection, &connect_info))
        {
            cDebug("Failed to get connection info");
            return;
        }
        cDebug(
            "Connection info: state={}, ",
            magic_enum::enum_name(connect_info.m_eState));
    }

    void create_server()
    {
        std::vector<SteamNetworkingConfigValue_t> config;
        // config.emplace_back();
        // config.back().SetInt32(ESteamNetworkingConfig_);
        SteamNetworkingIPAddr listen_interface;
        listen_interface.Clear();
        listen_interface.SetIPv4(0x00000000, 16000);
        m_socket = m_impl->CreateListenSocketIP(
            listen_interface, config.size(), config.data());
        m_poll_group = m_impl->CreatePollGroup();
    }

    void configure_weights(HSteamNetConnection connection)
    {
        std::array<int, 2> priorities = {{0, 2}};
        std::array<u16, 2> weights    = {{0, 10}};
        m_impl->ConfigureConnectionLanes(
            connection, priorities.size(), priorities.data(), weights.data());
    }

    void start_restricted(Proxy& p, time_point const&)
    {
        if(m_socket)
        {
            int                       num_msgs = -1;
            SteamNetworkingMessage_t* message  = nullptr;
            while(true)
            {
                num_msgs = m_impl->ReceiveMessagesOnPollGroup(
                    m_poll_group, &message, 1);
                if(num_msgs < 1)
                    break;
                cDebug(
                    "Received {} bytes from {}",
                    message->GetSize(),
                    message->GetConnection());
                message->Release();
            }
        }
        while(m_connection)
        {
            SteamNetConnectionInfo_t conninfo;
            if(!m_impl->GetConnectionInfo(m_connection, &conninfo))
                break;
            if(conninfo.m_eState != k_ESteamNetworkingConnectionState_Connected)
            {
                break;
            }

            int                       num_msgs = -1;
            SteamNetworkingMessage_t* message  = nullptr;
            while(true)
            {
                num_msgs = m_impl->ReceiveMessagesOnConnection(
                    m_connection, &message, 1);
                if(num_msgs < 1)
                    break;
                message->Release();
            }
            break;
        }
    }

    void end_restricted(Proxy&, time_point const&)
    {
    }

    GameEventBus&                    m_game_bus;
    ISteamNetworkingSockets*         m_impl{nullptr};
    ISteamNetworkingUtils*           m_utils{nullptr};
    SteamNetworkingIdentity          m_identity;
    HSteamListenSocket               m_socket{};
    HSteamNetPollGroup               m_poll_group{};
    HSteamNetConnection              m_connection{};
    std::vector<HSteamNetConnection> m_connections{};
};

void alloc_networking(compo::EntityContainer& e)
{
    e.register_subsystem_inplace<Networking>(
        std::ref(e.subsystem_cast<GameEventBus>()));
}
