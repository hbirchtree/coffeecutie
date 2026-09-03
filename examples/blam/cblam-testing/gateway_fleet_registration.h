#pragma once

#if defined(USE_NETWORKING) && defined(USE_WEBRTC_TRANSPORT)

#include <GameNetworkingSockets/steam/isteamnetworkingsockets.h>

#include <rtc/rtc.hpp>

#include <chrono>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>

#if !defined(COFFEE_WASM) && !defined(_WIN32)
#include <netinet/in.h>
#endif

namespace webrtc_signaling {

/*!
 * Registers a real, already-bound GNS UDP listen socket with a
 * webrtc-gateway fleet registry (tools/webrtc-gateway's /server-signal
 * "register" message), so browsers can be routed to it via
 * "/signal?server=<id>" without the gateway being locked to one -dest at
 * startup.
 *
 * Per-client relay punches go out directly from the real GNS listen
 * socket (ISteamNetworkingSockets::SendRawPacketOnListenSocket
 */
class GatewayFleetRegistration
{
  public:
    GatewayFleetRegistration(
        std::string              registerUrl,
        std::string              serverId,
        ISteamNetworkingSockets* sockets,
        HSteamListenSocket       listenSocket);
    ~GatewayFleetRegistration();

    void Start();
    void Poll();

    /*! True once the return-routability challenge has passed and the
     * gateway is actively routing browsers to this server. */
    bool Active() const;

    /*! Sends an opaque metadata payload to the gateway over the persistent
     * /server-signal websocket. The gateway caps this at 4096 bytes and
     * exposes it via GET /metadata?server=<id>. Only valid once Active(). */
    void SendMetadata(std::string_view jsonPayload);

  private:
    void onWebSocketOpen();
    void onWebSocketMessage(std::string const& text);
    void sendRegister();
    void sendHeartbeat();
    void pollChallengeSocket();
    void sendRegistrationPunch();

    std::string m_registerUrl;
    std::string m_serverId;

    ISteamNetworkingSockets* m_sockets;
    HSteamListenSocket       m_listenSocket;

    std::shared_ptr<rtc::WebSocket> m_ws;

    /* Raw POSIX UDP socket for the registration challenge only -- see the
     * class comment for why this one can't go through the GNS listen
     * socket. Unrelated to GNS/libdatachannel. Not wired up on Windows
     * yet (no winsock init here); Start() no-ops with a warning there
     * instead of a half-working registration. */
#if !defined(COFFEE_WASM) && !defined(_WIN32)
    int         m_challengeSock{-1};
    sockaddr_in m_gatewayAddr{}; /* IP resolved once in Start() from the
                                  * register URL's host */

    /*! Per browser client currently relaying to this server, keyed by
     * the gateway-assigned session ID (see "client-relay" in
     * onWebSocketMessage): just the gateway relay port to keep punching,
     * and when we last did. onClientRelay/onClientRelayClosed run on the
     * WebSocket's own callback thread; pollRelayKeepalives runs on
     * whatever thread calls Poll() (the main/render thread) -- guards
     * the map itself. */
    struct ClientRelay
    {
        int                                   relayPort{0};
        std::chrono::steady_clock::time_point lastPunch{};
    };

    std::mutex                                   m_relaysMutex;
    std::unordered_map<std::string, ClientRelay> m_relays;

    std::string m_trackingId;

    void onClientRelay(
        std::string const& sessionId,
        int                relayPort,
        std::string const& relayNonceHex,
        std::string const& trackingId);
    void onClientRelayClosed(std::string const& sessionId);
    void pollRelayKeepalives();
    void sendRelayPunch(int relayPort, std::string const& payload);
#endif

    mutable std::mutex m_mutex;
    bool               m_wsOpen{false};
    bool               m_active{false};
    /* True once register-pending delivered the punch port; no punches go
     * out before that. */
    bool                                  m_havePunchTarget{false};
    std::chrono::steady_clock::time_point m_lastHeartbeat{};
    std::chrono::steady_clock::time_point m_lastRegistrationPunch{};
    std::chrono::steady_clock::time_point m_lastRegisterSent{};
};

} // namespace webrtc_signaling

#endif
