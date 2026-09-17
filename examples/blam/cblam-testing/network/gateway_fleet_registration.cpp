#include "gateway_fleet_registration.h"

#if defined(USE_NETWORKING) && defined(USE_WEBRTC_TRANSPORT)

#include <coffee/core/debug/formatting.h>
#include <nlohmann/json.hpp>
#include <peripherals/semantic/chunk.h>
#include <peripherals/stl/base64.h>
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
constexpr std::chrono::seconds kRegistrationRetryInterval{7};
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
    HSteamListenSocket       listenSocket,
    bool                     relayOnly)
    : m_registerUrl(std::move(registerUrl))
    , m_serverId(std::move(serverId))
    , m_sockets(sockets)
    , m_listenSocket(listenSocket)
    , m_relayOnly(relayOnly)
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
        m_wsOpen           = true;
        m_lastHeartbeat    = std::chrono::steady_clock::now();
        m_lastRegisterSent = m_lastHeartbeat;
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
        m_trackingId           = msg.value("serverTrackingId", std::string());
#endif
    } else if(type == "gns-rendezvous")
    {
        if(m_relayOnly)
        {
            cWarning(
                "webrtc_signaling: gateway_fleet_registration: refusing "
                "rendezvous, this server is relay-only");
            return;
        }
        /* A client wants to reach us over P2P. Queue it rather than feeding
         * GNS here. */
        auto sessionId = msg.value("sessionId", std::string());
        auto raw       = b64::decode(msg.value("data", std::string()));
        if(sessionId.empty() || raw.empty())
        {
            cWarning(
                "webrtc_signaling: gateway_fleet_registration: malformed "
                "gns-rendezvous");
            return;
        }
        std::lock_guard<std::mutex> lock(m_mutex);
        m_incomingSignals.emplace_back(
            std::move(sessionId),
            std::string(reinterpret_cast<char const*>(raw.data()), raw.size()));
    } else if(type == "client-relay")
    {
#if !defined(COFFEE_WASM) && !defined(_WIN32)
        onClientRelay(
            msg.value("sessionId", std::string()),
            msg.value("relayPort", 0),
            msg.value("relayNonce", std::string()),
            msg.value("trackingId", std::string()));
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
        {"serverTransports",
         m_relayOnly ? nlohmann::json::array({"relay"})
                     : nlohmann::json::array({"direct", "relay"})},
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
            "active for serverId={} [{}] (challenge passed)",
            m_serverId,
            m_trackingId.empty() ? "no tracking id" : m_trackingId);
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
    std::string const& relayNonceHex,
    std::string const& trackingId)
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
        "for [{}/{}] -> gateway relay port {}",
        m_trackingId.empty() ? "?" : m_trackingId,
        trackingId.empty() ? "?" : trackingId,
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
    drainIncomingSignals();
    pollDirectRouteTakeover();
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
        bool dueForRegister;
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            dueForRegister =
                now - m_lastRegisterSent >= kRegistrationRetryInterval;
            if(dueForRegister)
                m_lastRegisterSent = now;
        }
        if(dueForRegister)
            sendRegister();
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

void GatewayFleetRegistration::drainIncomingSignals()
{
    std::vector<std::pair<std::string, std::string>> pending;
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        pending.swap(m_incomingSignals);
    }
    for(auto& [sessionId, blob] : pending)
    {
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_pendingSessionId = sessionId;
        }
        m_sockets->ReceivedP2PCustomSignal(
            blob.data(), static_cast<int>(blob.size()), this);
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_pendingSessionId.clear();
        }
    }
}

void GatewayFleetRegistration::pollDirectRouteTakeover()
{
    std::vector<std::pair<HSteamNetConnection, std::string>> watched;
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        for(auto const& [hConn, sessionId] : m_sessionByConnection)
        {
            if(m_relayRetired.count(hConn))
                continue;
            watched.emplace_back(hConn, sessionId);
        }
    }
    if(watched.empty())
        return;

    std::vector<std::string> retired;
    for(auto const& [hConn, sessionId] : watched)
    {
        SteamNetConnectionInfo_t info{};
        if(!m_sockets->GetConnectionInfo(hConn, &info))
            continue;
        /* GNS names the transport in the description; "WebRTC" there means
         * packets are still going through the relayed DataChannel. */
        std::string_view description(info.m_szConnectionDescription);
        bool onRelay = description.find("WebRTC") != std::string_view::npos;

        std::lock_guard<std::mutex> lock(m_mutex);
        auto                        tick = m_directRouteTicks.find(hConn);
        if(tick == m_directRouteTicks.end() || m_relayRetired.count(hConn))
            continue;
        if(info.m_eState != k_ESteamNetworkingConnectionState_Connected ||
           onRelay)
        {
            tick->second = 0;
            continue;
        }
        /* Briefly transport-less mid-switch, so require two readings before
         * calling it settled. */
        if(++tick->second < 2)
            continue;
        m_relayRetired.insert(hConn);
        retired.push_back(sessionId);
    }

    for(auto const& sessionId : retired)
    {
        if(!m_ws)
            break;
        nlohmann::json msg{
            {"type", "gns-connected"},
            {"sessionId", sessionId},
        };
        if(m_ws->send(msg.dump()))
            cDebug(
                "webrtc_signaling: gateway_fleet_registration: connection is "
                "direct, retiring the gateway relay");
    }
}

ISteamNetworkingConnectionSignaling* GatewayFleetRegistration::OnConnectRequest(
    HSteamNetConnection hConn, const SteamNetworkingIdentity&, int)
{
    std::string sessionId;
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        sessionId = m_pendingSessionId;
    }
    if(sessionId.empty())
    {
        /* Per the interface's own doc comment, nullptr is the safe default:
         * it silently ignores a request we cannot route a reply to, rather
         * than actively rejecting it. */
        cWarning(
            "webrtc_signaling: gateway_fleet_registration: OnConnectRequest "
            "with no pending session ID");
        return nullptr;
    }
    {
        /* Remember which session this arrived under, so the relay standing in
         * until ICE connects can be retired afterwards. */
        std::lock_guard<std::mutex> lock(m_mutex);
        m_sessionByConnection[hConn] = sessionId;
        m_directRouteTicks[hConn]    = 0;
    }
    /* Ownership passes to GNS; the connection itself is accepted by the
     * ordinary incoming-connection path, which needs no listen socket. */
    return new FleetAcceptSignaling(this, std::move(sessionId));
}

void GatewayFleetRegistration::SendRejectionSignal(
    const SteamNetworkingIdentity&, const void*, int)
{
    /* OnConnectRequest only returns nullptr when it has no session to reply
     * on, which GNS treats as a silent ignore rather than a rejection. */
}

bool GatewayFleetRegistration::SendRendezvous(
    std::string const& sessionId, const void* msg, int msgLen)
{
    if(!m_ws || msgLen <= 0)
        return false;
    nlohmann::json out{
        {"type", "gns-rendezvous"},
        {"sessionId", sessionId},
        {"data",
         b64::encode(
             semantic::Span<const uint8_t>(
                 reinterpret_cast<const uint8_t*>(msg),
                 static_cast<size_t>(msgLen)))},
    };
    return m_ws->send(out.dump());
}

FleetAcceptSignaling::FleetAcceptSignaling(
    GatewayFleetRegistration* owner, std::string sessionId)
    : m_owner(owner)
    , m_sessionId(std::move(sessionId))
{
}

bool FleetAcceptSignaling::SendSignal(
    HSteamNetConnection,
    const SteamNetConnectionInfo_t&,
    const void* pMsg,
    int         cbMsg)
{
    return m_owner && m_owner->SendRendezvous(m_sessionId, pMsg, cbMsg);
}

void FleetAcceptSignaling::Release()
{
    delete this;
}

void GatewayFleetRegistration::SendMetadata(std::string_view jsonPayload)
{
    if(!m_ws)
        return;
    nlohmann::json msg{
        {"type", "metadata"},
        {"data", std::string(jsonPayload)},
    };
    if(!m_ws->send(msg.dump()))
        cWarning(
            "webrtc_signaling: gateway_fleet_registration: failed to send "
            "metadata");
}

} // namespace webrtc_signaling

#endif
