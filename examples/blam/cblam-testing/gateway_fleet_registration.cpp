#include "gateway_fleet_registration.h"

#if defined(USE_NETWORKING) && defined(USE_WEBRTC_TRANSPORT)

#include <coffee/core/debug/formatting.h>
#include <nlohmann/json.hpp>
#include <peripherals/stl/string/hex.h>

#include <variant>
#include <vector>

#if !defined(COFFEE_WASM) && !defined(_WIN32)
#include <arpa/inet.h>
#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#endif

using namespace Coffee::Logging;

namespace webrtc_signaling {

namespace {
/* Matches the gateway's default -registration-ttl (30s) / 3 -- no
 * override surface yet, nothing needs one. */
constexpr std::chrono::seconds kHeartbeatInterval{10};
/* How often to retry the registration return-routability punch while
 * not yet active -- matches the gateway's default -challenge-timeout
 * (5s) closely enough to get a couple of attempts in per timeout window
 * without being chatty. */
constexpr std::chrono::seconds kRegistrationPunchInterval{1};
/* How often each per-client relay re-punches to keep its NAT mapping
 * alive for the life of that client's connection. */
constexpr std::chrono::seconds kRelayPunchInterval{2};

const std::string kRegisterPunchPrefix = "COFFEE-REG-PUNCH:";
const std::string kRelayPunchPayload   = "COFFEE-NAT-PUNCH";

std::string hexDecode(std::string const& in)
{
    if(in.size() % 2 != 0)
        return {};
    auto nibble = [](char c) -> int {
        if(c >= '0' && c <= '9')
            return c - '0';
        if(c >= 'a' && c <= 'f')
            return c - 'a' + 10;
        if(c >= 'A' && c <= 'F')
            return c - 'A' + 10;
        return -1;
    };
    std::string out;
    out.reserve(in.size() / 2);
    for(size_t i = 0; i < in.size(); i += 2)
    {
        int hi = nibble(in[i]);
        int lo = nibble(in[i + 1]);
        if(hi < 0 || lo < 0)
            return {};
        out.push_back(static_cast<char>((hi << 4) | lo));
    }
    return out;
}

#if !defined(COFFEE_WASM) && !defined(_WIN32)
/* "ws(s)://host[:port][/path]" -> host. Only the host matters here: the
 * UDP punch PORT is told to us by the gateway (register-pending's
 * punchPort) rather than guessed from this URL, which behind a TLS
 * reverse proxy or docker port mapping bears no relation to the
 * gateway's actual UDP port. */
bool parseWsHost(std::string const& url, std::string& host)
{
    auto schemeEnd = url.find("://");
    auto rest =
        schemeEnd == std::string::npos ? url : url.substr(schemeEnd + 3);
    if(auto slash = rest.find('/'); slash != std::string::npos)
        rest = rest.substr(0, slash);
    if(auto colon = rest.rfind(':'); colon != std::string::npos)
        rest = rest.substr(0, colon);
    host = rest;
    return !host.empty();
}

bool resolveUDPAddr(
    std::string const& host, std::string const& port, sockaddr_in& out)
{
    addrinfo hints{};
    hints.ai_family   = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    addrinfo* result  = nullptr;
    if(::getaddrinfo(host.c_str(), port.c_str(), &hints, &result) != 0 ||
       !result)
        return false;
    out = *reinterpret_cast<sockaddr_in*>(result->ai_addr);
    ::freeaddrinfo(result);
    return true;
}

/* sockaddr_in (network byte order) -> SteamNetworkingIPAddr (SetIPv4 takes
 * host byte order, per its own doc comment). */
SteamNetworkingIPAddr toSteamAddr(sockaddr_in const& addr)
{
    SteamNetworkingIPAddr out;
    out.SetIPv4(ntohl(addr.sin_addr.s_addr), ntohs(addr.sin_port));
    return out;
}
#endif
} // namespace

GatewayFleetRegistration::GatewayFleetRegistration(
    std::string              registerUrl,
    std::string              serverId,
    ISteamNetworkingSockets* sockets,
    HSteamListenSocket       listenSocket)
    : m_registerUrl(std::move(registerUrl))
    , m_serverId(std::move(serverId))
    , m_sockets(sockets)
    , m_listenSocket(listenSocket)
{
}

GatewayFleetRegistration::~GatewayFleetRegistration()
{
    if(m_ws)
        m_ws->close();
#if !defined(COFFEE_WASM) && !defined(_WIN32)
    if(m_challengeSock >= 0)
        ::close(m_challengeSock);
#endif
}

void GatewayFleetRegistration::Start()
{
#if !defined(COFFEE_WASM) && !defined(_WIN32)
    m_challengeSock = ::socket(AF_INET, SOCK_DGRAM, 0);
    if(m_challengeSock < 0)
    {
        cWarning(
            "webrtc_signaling: gateway_fleet_registration: failed to open "
            "challenge socket: {}",
            std::strerror(errno));
        return;
    }

    sockaddr_in addr{};
    addr.sin_family      = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port        = 0;
    if(::bind(
           m_challengeSock, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) !=
       0)
    {
        cWarning(
            "webrtc_signaling: gateway_fleet_registration: failed to bind "
            "challenge socket: {}",
            std::strerror(errno));
        ::close(m_challengeSock);
        m_challengeSock = -1;
        return;
    }

    int flags = ::fcntl(m_challengeSock, F_GETFL, 0);
    ::fcntl(m_challengeSock, F_SETFL, flags | O_NONBLOCK);

    /* Host only -- the punch port arrives later via register-pending, so
     * m_gatewayAddr's port stays 0 until then (m_havePunchTarget). */
    std::string gatewayHost;
    if(!parseWsHost(m_registerUrl, gatewayHost) ||
       !resolveUDPAddr(gatewayHost, "0", m_gatewayAddr))
    {
        cWarning(
            "webrtc_signaling: gateway_fleet_registration: failed to "
            "resolve gateway host from {}",
            m_registerUrl);
        ::close(m_challengeSock);
        m_challengeSock = -1;
        return;
    }
#else
    cWarning(
        "webrtc_signaling: gateway_fleet_registration: not implemented on "
        "this platform, registration will never activate");
    return;
#endif

    m_ws = std::make_shared<rtc::WebSocket>();
    m_ws->onOpen([this]() { onWebSocketOpen(); });
    m_ws->onMessage([this](rtc::message_variant data) {
        if(!std::holds_alternative<std::string>(data))
        {
            cWarning(
                "webrtc_signaling: gateway_fleet_registration: ignoring "
                "unexpected binary message");
            return;
        }
        onWebSocketMessage(std::get<std::string>(data));
    });
    m_ws->onError([](std::string error) {
        cWarning(
            "webrtc_signaling: gateway_fleet_registration: websocket error: {}",
            error);
    });
    m_ws->open(m_registerUrl + "/server-signal");
}

void GatewayFleetRegistration::onWebSocketOpen()
{
    cDebug("Gateway WebSocket opened");
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_wsOpen        = true;
        m_lastHeartbeat = std::chrono::steady_clock::now();
    }
    sendRegister();
}

void GatewayFleetRegistration::onWebSocketMessage(std::string const& text)
{
    cDebug("GatewayFleetRegistration::onWebSocketMessage: {}", text);
    nlohmann::json msg;
    try
    {
        msg = nlohmann::json::parse(text);
    } catch(nlohmann::json::parse_error const& e)
    {
        cWarning(
            "webrtc_signaling: gateway_fleet_registration: failed to parse "
            "server-signal message: {}",
            e.what());
        return;
    }

    auto type = msg.value("type", std::string());
    if(type == "error")
    {
        cWarning(
            "webrtc_signaling: gateway_fleet_registration: gateway rejected "
            "registration: {}",
            msg.value("data", std::string()));
        std::lock_guard<std::mutex> lock(m_mutex);
        m_active = false;
    } else if(type == "register-pending")
    {
#if !defined(COFFEE_WASM) && !defined(_WIN32)
        int punchPort = msg.value("punchPort", 0);
        if(punchPort <= 0 || punchPort > 65535)
        {
            cWarning(
                "webrtc_signaling: gateway_fleet_registration: "
                "register-pending with bad punchPort {}",
                punchPort);
            return;
        }
        std::lock_guard<std::mutex> lock(m_mutex);
        m_gatewayAddr.sin_port = htons(static_cast<uint16_t>(punchPort));
        m_havePunchTarget      = true;
#endif
    } else if(type == "client-relay")
    {
#if !defined(COFFEE_WASM) && !defined(_WIN32)
        onClientRelay(
            msg.value("sessionId", std::string()),
            msg.value("relayPort", 0),
            msg.value("relayNonce", std::string()));
#endif
    } else if(type == "client-relay-closed")
    {
#if !defined(COFFEE_WASM) && !defined(_WIN32)
        onClientRelayClosed(msg.value("sessionId", std::string()));
#endif
    } else
    {
        cWarning(
            "webrtc_signaling: gateway_fleet_registration: unexpected "
            "server-signal message type {}",
            type);
    }
}

void GatewayFleetRegistration::sendRegister()
{
    /* No address is declared -- the gateway learns where to relay from
     * this server's own NAT punches (observed source addresses), so a
     * declared one could only mislead. */
    nlohmann::json reg{
        {"type", "register"},
        {"serverId", m_serverId},
    };
    if(!m_ws->send(reg.dump()))
        cWarning(
            "webrtc_signaling: gateway_fleet_registration: failed to send "
            "register message");
}

void GatewayFleetRegistration::sendHeartbeat()
{
    nlohmann::json hb{
        {"type", "heartbeat"},
        {"serverId", m_serverId},
    };
    if(!m_ws->send(hb.dump()))
        cWarning(
            "webrtc_signaling: gateway_fleet_registration: failed to send "
            "heartbeat");
}

void GatewayFleetRegistration::sendRegistrationPunch()
{
#if !defined(COFFEE_WASM) && !defined(_WIN32)
    if(m_challengeSock < 0)
        return;
    /* m_gatewayAddr's port is filled in by register-pending on the WS
     * callback thread -- copy under the same lock. */
    sockaddr_in target{};
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        if(!m_havePunchTarget)
            return;
        target = m_gatewayAddr;
    }
    std::string payload = kRegisterPunchPrefix + m_serverId;
    ::sendto(
        m_challengeSock,
        payload.data(),
        payload.size(),
        0,
        reinterpret_cast<sockaddr const*>(&target),
        sizeof(target));
#endif
}

void GatewayFleetRegistration::pollChallengeSocket()
{
#if !defined(COFFEE_WASM) && !defined(_WIN32)
    if(m_challengeSock < 0)
        return;

    /* Reply to the registration punch above -- see the gateway's
     * challengeListener, which sends this registration's nonce back to
     * whatever source address the punch arrived from. */
    char        buf[64];
    sockaddr_in from{};
    socklen_t   fromLen = sizeof(from);
    auto        n       = ::recvfrom(
        m_challengeSock,
        buf,
        sizeof(buf),
        0,
        reinterpret_cast<sockaddr*>(&from),
        &fromLen);
    if(n <= 0)
        return;

    auto nonce = hex::encode(std::string(buf, static_cast<size_t>(n)));
    nlohmann::json response{
        {"type", "challenge-response"},
        {"serverId", m_serverId},
        {"nonce", nonce},
    };
    if(m_ws && m_ws->send(response.dump()))
    {
        /* Optimistic: the gateway sends no separate "you're active now"
         * ack, so echoing the nonce successfully is the best local signal
         * available without extending the wire protocol further. */
        cDebug(
            "webrtc_signaling: gateway_fleet_registration: registration "
            "active for serverId={} (challenge passed)",
            m_serverId);
        std::lock_guard<std::mutex> lock(m_mutex);
        m_active = true;
    }
#endif
}

#if !defined(COFFEE_WASM) && !defined(_WIN32)
void GatewayFleetRegistration::sendRelayPunch(
    int relayPort, std::string const& payload)
{
    sockaddr_in target{};
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        target = m_gatewayAddr;
    }
    target.sin_port            = htons(static_cast<uint16_t>(relayPort));
    SteamNetworkingIPAddr addr = toSteamAddr(target);
    m_sockets->SendRawPacketOnListenSocket(
        m_listenSocket, payload.data(), static_cast<int>(payload.size()), addr);
}

void GatewayFleetRegistration::onClientRelay(
    std::string const& sessionId,
    int                relayPort,
    std::string const& relayNonceHex)
{
    if(sessionId.empty() || relayPort <= 0 || relayPort > 65535)
    {
        cWarning(
            "webrtc_signaling: gateway_fleet_registration: malformed "
            "client-relay message (sessionId={}, relayPort={})",
            sessionId,
            relayPort);
        return;
    }

    {
        std::lock_guard<std::mutex> lock(m_relaysMutex);
        m_relays[sessionId] =
            ClientRelay{relayPort, std::chrono::steady_clock::now()};
    }
    std::string nonce = hexDecode(relayNonceHex);
    sendRelayPunch(relayPort, nonce.empty() ? kRelayPunchPayload : nonce);
    cDebug(
        "webrtc_signaling: gateway_fleet_registration: relay punch started "
        "for session {} -> gateway relay port {}",
        sessionId,
        relayPort);
}

void GatewayFleetRegistration::onClientRelayClosed(std::string const& sessionId)
{
    std::lock_guard<std::mutex> lock(m_relaysMutex);
    m_relays.erase(sessionId);
}

void GatewayFleetRegistration::pollRelayKeepalives()
{
    auto now = std::chrono::steady_clock::now();

    /* Collect the ports due for a re-punch under the lock, then send
     * outside it -- SendRawPacketOnListenSocket takes GNS's own global
     * lock internally, no need to also hold ours for that. */
    std::vector<int> due;
    {
        std::lock_guard<std::mutex> lock(m_relaysMutex);
        for(auto& [sessionId, relay] : m_relays)
        {
            if(now - relay.lastPunch >= kRelayPunchInterval)
            {
                relay.lastPunch = now;
                due.push_back(relay.relayPort);
            }
        }
    }
    for(int relayPort : due)
        sendRelayPunch(relayPort, kRelayPunchPayload);
}
#endif

void GatewayFleetRegistration::Poll()
{
    pollChallengeSocket();

    bool wsOpen, active;
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        wsOpen = m_wsOpen;
        active = m_active;
    }
    if(!wsOpen)
        return;

    if(!active)
    {
        auto now = std::chrono::steady_clock::now();
        if(now - m_lastRegistrationPunch >= kRegistrationPunchInterval)
        {
            m_lastRegistrationPunch = now;
            sendRegistrationPunch();
        }
    }

    bool dueForHeartbeat;
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        auto                        now = std::chrono::steady_clock::now();
        dueForHeartbeat = now - m_lastHeartbeat >= kHeartbeatInterval;
        if(dueForHeartbeat)
            m_lastHeartbeat = now;
    }
    if(dueForHeartbeat)
        sendHeartbeat();

#if !defined(COFFEE_WASM) && !defined(_WIN32)
    pollRelayKeepalives();
#endif
}

bool GatewayFleetRegistration::Active() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_active;
}

} // namespace webrtc_signaling

#endif
