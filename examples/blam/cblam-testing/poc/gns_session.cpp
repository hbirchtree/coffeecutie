#include "gns_session.h"

#include <GameNetworkingSockets/steam/isteamnetworkingsockets.h>
#include <GameNetworkingSockets/steam/isteamnetworkingutils.h>
#include <GameNetworkingSockets/steam/steamnetworkingsockets.h>
#include <fmt/format.h>

#include <algorithm>

namespace poc {
namespace {

std::vector<GnsSession*> g_sessions;
bool                     g_owns_library{false};

void status_changed(SteamNetConnectionStatusChangedCallback_t* info)
{
    for(auto* s : g_sessions)
        s->on_status(info);
}

SteamNetworkingConfigValue_t status_callback()
{
    SteamNetworkingConfigValue_t opt;
    opt.SetPtr(
        k_ESteamNetworkingConfig_Callback_ConnectionStatusChanged,
        reinterpret_cast<void*>(&status_changed));
    return opt;
}

} // namespace

bool GnsSession::init(std::string& error)
{
    /* Someone else (the app's Networking) may have it running already */
    if(SteamNetworkingSockets())
        return true;
    SteamNetworkingErrMsg msg;
    if(!GameNetworkingSockets_Init(nullptr, msg))
    {
        error = msg;
        return false;
    }
    SteamNetworkingUtils()->SetDebugOutputFunction(
        k_ESteamNetworkingSocketsDebugOutputType_Warning,
        [](ESteamNetworkingSocketsDebugOutputType, char const* message) {
            fmt::print(stderr, "gns: {}\n", message);
        });
    g_owns_library = true;
    return true;
}

void GnsSession::shutdown()
{
    if(g_owns_library)
        GameNetworkingSockets_Kill();
    g_owns_library = false;
}

void GnsSession::simulate(NetSimulation const& sim)
{
    auto* utils = SteamNetworkingUtils();
    utils->SetGlobalConfigValueInt32(
        k_ESteamNetworkingConfig_FakePacketLag_Send,
        static_cast<int>(sim.rtt_ms / 2.f));
    utils->SetGlobalConfigValueInt32(
        k_ESteamNetworkingConfig_FakePacketLag_Recv, 0);
    utils->SetGlobalConfigValueFloat(
        k_ESteamNetworkingConfig_FakePacketJitter_Send_Avg, sim.jitter_ms);
    utils->SetGlobalConfigValueFloat(
        k_ESteamNetworkingConfig_FakePacketJitter_Send_Max, sim.jitter_ms * 4.f);
    utils->SetGlobalConfigValueFloat(
        k_ESteamNetworkingConfig_FakePacketJitter_Send_Pct,
        sim.jitter_ms > 0.f ? 100.f : 0.f);
    utils->SetGlobalConfigValueFloat(
        k_ESteamNetworkingConfig_FakePacketLoss_Send, sim.loss_pct);
    utils->SetGlobalConfigValueFloat(
        k_ESteamNetworkingConfig_FakePacketLoss_Recv, 0.f);
}

void GnsSession::run_callbacks()
{
    SteamNetworkingSockets()->RunCallbacks();
}

GnsSession::GnsSession()
{
    g_sessions.push_back(this);
}

GnsSession::~GnsSession()
{
    auto* gns = SteamNetworkingSockets();
    for(auto c : m_clients)
        gns->CloseConnection(c, 0, "session end", false);
    if(m_conn)
        gns->CloseConnection(m_conn, 0, "session end", false);
    if(m_listen)
        gns->CloseListenSocket(m_listen);
    if(m_poll_group)
        gns->DestroyPollGroup(m_poll_group);
    std::erase(g_sessions, this);
}

std::unique_ptr<GnsSession> GnsSession::listen(std::uint16_t port)
{
    std::unique_ptr<GnsSession> s(new GnsSession());
    SteamNetworkingIPAddr       addr;
    addr.Clear();
    addr.SetIPv4(0x7f000001, port);
    auto opt        = status_callback();
    auto* gns       = SteamNetworkingSockets();
    s->m_listen     = gns->CreateListenSocketIP(addr, 1, &opt);
    s->m_poll_group = gns->CreatePollGroup();
    return s;
}

std::unique_ptr<GnsSession> GnsSession::connect(std::string const& address)
{
    std::unique_ptr<GnsSession> s(new GnsSession());
    SteamNetworkingIPAddr       addr;
    addr.Clear();
    if(!addr.ParseString(address.c_str()))
        return s;
    auto opt  = status_callback();
    s->m_conn = SteamNetworkingSockets()->ConnectByIPAddress(addr, 1, &opt);
    return s;
}

bool GnsSession::valid() const
{
    return m_listen != 0 || m_conn != 0;
}

void GnsSession::on_status(void* ptr)
{
    auto* info = static_cast<SteamNetConnectionStatusChangedCallback_t*>(ptr);
    auto* gns  = SteamNetworkingSockets();
    bool const ours =
        (m_listen && info->m_info.m_hListenSocket == m_listen) ||
        (m_conn && info->m_hConn == m_conn);
    if(!ours)
        return;

    switch(info->m_info.m_eState)
    {
    case k_ESteamNetworkingConnectionState_Connecting:
        if(m_listen)
        {
            gns->AcceptConnection(info->m_hConn);
            gns->SetConnectionPollGroup(info->m_hConn, m_poll_group);
        }
        break;
    case k_ESteamNetworkingConnectionState_Connected:
        if(m_listen)
            m_clients.push_back(info->m_hConn);
        m_pending.push_back(NetConnected{info->m_hConn});
        break;
    case k_ESteamNetworkingConnectionState_ClosedByPeer:
    case k_ESteamNetworkingConnectionState_ProblemDetectedLocally:
        gns->CloseConnection(info->m_hConn, 0, nullptr, false);
        std::erase(m_clients, info->m_hConn);
        if(info->m_hConn == m_conn)
            m_conn = 0;
        m_pending.push_back(NetDisconnected{info->m_hConn});
        break;
    default:
        break;
    }
}

void GnsSession::poll(Bus& bus)
{
    for(auto& ev : m_pending)
        bus.queue.push_back(std::move(ev));
    m_pending.clear();

    auto* gns = SteamNetworkingSockets();
    SteamNetworkingMessage_t* msgs[32];
    while(true)
    {
        int n = m_listen ? gns->ReceiveMessagesOnPollGroup(m_poll_group, msgs, 32)
                : m_conn ? gns->ReceiveMessagesOnConnection(m_conn, msgs, 32)
                         : 0;
        if(n <= 0)
            break;
        for(int i = 0; i < n; i++)
        {
            auto const* data = static_cast<std::uint8_t const*>(msgs[i]->GetData());
            bus.push(NetReceived{
                .from  = msgs[i]->m_conn,
                .bytes = {data, data + msgs[i]->GetSize()},
            });
            msgs[i]->Release();
        }
    }

    auto stats = [&](HSteamNetConnection c) {
        SteamNetConnectionRealTimeStatus_t st;
        if(gns->GetConnectionRealTimeStatus(c, &st, 0, nullptr) == k_EResultOK)
            bus.push(NetStats{c, static_cast<float>(st.m_nPing)});
    };
    for(auto c : m_clients)
        stats(c);
    if(m_conn)
        stats(m_conn);
}

void GnsSession::handle(NetSend const& ev)
{
    auto*     gns   = SteamNetworkingSockets();
    int const flags = ev.reliable ? k_nSteamNetworkingSend_ReliableNoNagle
                                  : k_nSteamNetworkingSend_UnreliableNoNagle;
    auto send = [&](HSteamNetConnection c) {
        gns->SendMessageToConnection(
            c, ev.bytes.data(), static_cast<uint32>(ev.bytes.size()), flags, nullptr);
    };
    if(ev.to != broadcast)
        send(ev.to);
    else if(m_listen)
        for(auto c : m_clients)
            send(c);
    else if(m_conn)
        send(m_conn);
}

} // namespace poc
