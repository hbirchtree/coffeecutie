/* Joins BlamGraphics servers over GameNetworkingSockets for consoles that
 * can't run GNS themselves (GameCube): each console talks plain UDP to this
 * process (see gc_gateway/protocol.h) and gets one GNS client connection,
 * whose messages are passed through untouched in both directions. */

#include <coffee/core/CApplication>
#include <coffee/core/CDebug>
#include <coffee/core/coffee.h>

#include <gc_gateway/protocol.h>

#include <GameNetworkingSockets/steam/isteamnetworkingsockets.h>
#include <GameNetworkingSockets/steam/isteamnetworkingutils.h>
#include <GameNetworkingSockets/steam/steamnetworkingsockets.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <chrono>
#include <cstring>
#include <map>
#include <string>
#include <thread>
#include <vector>

using namespace Coffee;
namespace gw     = gc_gateway;
using clock_type = std::chrono::steady_clock;

namespace {

/* MessageBase from BlamGraphics' networking.cpp: type, request, flags,
 * num_values, then num_values values of one size */
constexpr u32 message_base_size = 16;
constexpr u32 message_multiple  = 0x2;

struct session_t
{
    sockaddr_in            addr{};
    HSteamNetConnection    conn{k_HSteamNetConnection_Invalid};
    gw::status_t::state_t  state{gw::status_t::closed};
    clock_type::time_point last_seen{};
};

int                                g_socket = -1;
ISteamNetworkingSockets*           g_gns    = nullptr;
std::string                        g_default_server;
std::map<u64, session_t>           g_sessions;
std::map<HSteamNetConnection, u64> g_by_conn;

u64 key_of(sockaddr_in const& addr)
{
    return (static_cast<u64>(addr.sin_addr.s_addr) << 16) | addr.sin_port;
}

std::string name_of(sockaddr_in const& addr)
{
    char ip[INET_ADDRSTRLEN] = {};
    inet_ntop(AF_INET, &addr.sin_addr, ip, sizeof(ip));
    return std::string(ip) + ":" + std::to_string(ntohs(addr.sin_port));
}

void send_datagram(
    session_t const& s, u16 kind, u16 flags, void const* data, u32 size)
{
    std::vector<u8> buf(sizeof(gw::header_t) + size);
    gw::header_t    hdr{gw::magic, kind, flags};
    std::memcpy(buf.data(), &hdr, sizeof(hdr));
    if(size)
        std::memcpy(buf.data() + sizeof(hdr), data, size);
    sendto(
        g_socket,
        buf.data(),
        buf.size(),
        0,
        reinterpret_cast<sockaddr const*>(&s.addr),
        sizeof(s.addr));
}

void send_status(session_t const& s)
{
    gw::status_t st{s.state};
    send_datagram(s, gw::status, gw::none, &st, sizeof(st));
}

/* Batched server messages can be far larger than a datagram; split them into
 * smaller batches of the same type, which the receiver handles the same */
void forward_to_console(session_t const& s, u8 const* data, u32 size)
{
    constexpr u32 room = gw::max_datagram - sizeof(gw::header_t);
    if(size <= room)
        return send_datagram(s, gw::message, gw::none, data, size);

    u32 num_values = 0;
    u32 flags      = 0;
    if(size > message_base_size)
    {
        std::memcpy(&flags, data + 8, sizeof(flags));
        std::memcpy(&num_values, data + 12, sizeof(num_values));
    }
    u32 const value_size =
        num_values ? (size - message_base_size) / num_values : 0;
    if(num_values < 2 || value_size * num_values != size - message_base_size ||
       value_size > room - message_base_size)
    {
        cWarning("Dropping {} byte message, it doesn't split", size);
        return;
    }
    u32 const       per_datagram = (room - message_base_size) / value_size;
    std::vector<u8> chunk(room);
    for(u32 first = 0; first < num_values; first += per_datagram)
    {
        u32 const count = std::min(per_datagram, num_values - first);
        u32 const chunk_flags =
            (flags & ~message_multiple) | (count > 1 ? message_multiple : 0);
        std::memcpy(chunk.data(), data, message_base_size);
        std::memcpy(chunk.data() + 8, &chunk_flags, sizeof(chunk_flags));
        std::memcpy(chunk.data() + 12, &count, sizeof(count));
        std::memcpy(
            chunk.data() + message_base_size,
            data + message_base_size + first * value_size,
            count * value_size);
        send_datagram(
            s,
            gw::message,
            gw::none,
            chunk.data(),
            message_base_size + count * value_size);
    }
}

void close_session(session_t& s)
{
    if(s.conn != k_HSteamNetConnection_Invalid)
    {
        g_gns->CloseConnection(s.conn, 0, "console left", false);
        g_by_conn.erase(s.conn);
    }
    s.conn  = k_HSteamNetConnection_Invalid;
    s.state = gw::status_t::closed;
}

void on_status_changed(SteamNetConnectionStatusChangedCallback_t* info)
{
    auto it = g_by_conn.find(info->m_hConn);
    if(it == g_by_conn.end())
        return;
    session_t& s = g_sessions[it->second];
    switch(info->m_info.m_eState)
    {
    case k_ESteamNetworkingConnectionState_Connecting:
    case k_ESteamNetworkingConnectionState_FindingRoute:
        s.state = gw::status_t::connecting;
        break;
    case k_ESteamNetworkingConnectionState_Connected:
        s.state = gw::status_t::connected;
        cDebug("{} joined the server", name_of(s.addr));
        break;
    case k_ESteamNetworkingConnectionState_ClosedByPeer:
    case k_ESteamNetworkingConnectionState_ProblemDetectedLocally:
        cDebug(
            "{} lost the server: {}",
            name_of(s.addr),
            info->m_info.m_szEndDebug);
        close_session(s);
        break;
    default:
        return;
    }
    send_status(s);
}

void connect_session(session_t& s, std::string const& server)
{
    close_session(s);
    SteamNetworkingIPAddr addr;
    if(!addr.ParseString(server.c_str()))
    {
        cWarning(
            "{} asked for a bad server address: {}", name_of(s.addr), server);
        return send_status(s);
    }
    SteamNetworkingConfigValue_t config;
    config.SetPtr(
        k_ESteamNetworkingConfig_Callback_ConnectionStatusChanged,
        reinterpret_cast<void*>(&on_status_changed));
    s.conn = g_gns->ConnectByIPAddress(addr, 1, &config);
    if(s.conn == k_HSteamNetConnection_Invalid)
        return send_status(s);
    s.state           = gw::status_t::connecting;
    g_by_conn[s.conn] = key_of(s.addr);
    cDebug("{} -> {}", name_of(s.addr), server);
    send_status(s);
}

void handle_datagram(sockaddr_in const& from, u8 const* data, u32 size)
{
    gw::header_t hdr;
    if(size < sizeof(hdr))
        return;
    std::memcpy(&hdr, data, sizeof(hdr));
    if(hdr.magic != gw::magic)
        return;
    u8 const* payload      = data + sizeof(hdr);
    u32 const payload_size = size - sizeof(hdr);

    session_t& s = g_sessions[key_of(from)];
    s.addr       = from;
    s.last_seen  = clock_type::now();

    switch(hdr.kind)
    {
    case gw::hello: {
        gw::hello_t hello{};
        std::memcpy(
            &hello, payload, std::min<u32>(payload_size, sizeof(hello)));
        hello.server[sizeof(hello.server) - 1] = 0;
        connect_session(s, hello.server[0] ? hello.server : g_default_server);
        break;
    }
    case gw::keepalive:
        send_status(s);
        break;
    case gw::message:
        if(s.state != gw::status_t::connected)
            break;
        g_gns->SendMessageToConnection(
            s.conn,
            payload,
            payload_size,
            (hdr.flags & gw::reliable)
                ? k_nSteamNetworkingSend_Reliable
                : k_nSteamNetworkingSend_UnreliableNoNagle,
            nullptr);
        break;
    default:
        break;
    }
}

} // namespace

i32 coffee_main(i32, cstring_w*)
{
    u16  port = gw::default_port;
    auto args = GetInitArgs();
    for(size_t i = 1; i + 1 < args.size(); i++)
    {
        std::string_view const arg = args.at(i);
        if(arg == "--port")
            port = static_cast<u16>(std::stoi(args.at(++i)));
        else if(arg == "--server")
            g_default_server = args.at(++i);
    }

    SteamNetworkingErrMsg err;
    if(!GameNetworkingSockets_Init(nullptr, err))
    {
        cWarning("Failed to init GameNetworkingSockets: {}", err);
        return 1;
    }
    g_gns = SteamNetworkingSockets();

    g_socket = socket(AF_INET, SOCK_DGRAM, 0);
    sockaddr_in local{};
    local.sin_family      = AF_INET;
    local.sin_port        = htons(port);
    local.sin_addr.s_addr = htonl(INADDR_ANY);
    if(g_socket < 0 ||
       bind(g_socket, reinterpret_cast<sockaddr*>(&local), sizeof(local)) < 0)
    {
        cWarning("Failed to listen on UDP port {}", port);
        return 1;
    }
    cDebug(
        "Gateway listening on UDP {}, default server {}",
        port,
        g_default_server.empty() ? "<none>" : g_default_server);

    std::vector<u8> buf(65536);
    for(;;)
    {
        for(;;)
        {
            sockaddr_in from{};
            socklen_t   from_len = sizeof(from);
            auto const  got      = recvfrom(
                g_socket,
                buf.data(),
                buf.size(),
                MSG_DONTWAIT,
                reinterpret_cast<sockaddr*>(&from),
                &from_len);
            if(got <= 0)
                break;
            handle_datagram(from, buf.data(), static_cast<u32>(got));
        }

        g_gns->RunCallbacks();

        auto const now = clock_type::now();
        for(auto it = g_sessions.begin(); it != g_sessions.end();)
        {
            session_t& s = it->second;
            if(now - s.last_seen > std::chrono::seconds(15))
            {
                cDebug("{} timed out", name_of(s.addr));
                close_session(s);
                it = g_sessions.erase(it);
                continue;
            }
            if(s.conn != k_HSteamNetConnection_Invalid)
            {
                SteamNetworkingMessage_t* msgs[32];
                int const                 n =
                    g_gns->ReceiveMessagesOnConnection(s.conn, msgs, 32);
                for(int i = 0; i < n; i++)
                {
                    forward_to_console(
                        s,
                        static_cast<u8 const*>(msgs[i]->m_pData),
                        static_cast<u32>(msgs[i]->m_cbSize));
                    msgs[i]->Release();
                }
            }
            ++it;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(2));
    }
}

COFFEE_APPLICATION_MAIN_CUSTOM(coffee_main, 0x1 | 0x2)
