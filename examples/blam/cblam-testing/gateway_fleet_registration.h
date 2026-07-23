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
 * The usual real target is behind NAT with no port forwarding, so the
 * gateway can never reach a declared address by blindly sending to it --
 * both the registration challenge and the per-client relay work by
 * having THIS side punch first: send a small UDP probe to the gateway,
 * let it observe the real source address, and treat its reply as arriving
 * over a mapping that's now open. See WEBRTC_TRANSPORT.md's Phase 6
 * section for the full protocol.
 *
 * Per-client relay punches go out directly from the real GNS listen
 * socket (ISteamNetworkingSockets::SendRawPacketOnListenSocket, a small
 * coffeecutie-added GNS API -- see
 * toolchain/vcpkg/ports/gamenetworkingsockets/add-listen-socket-raw-send.patch)
 * rather than a separate local socket: since that's the exact port/mapping
 * real relayed traffic needs to land on anyway, GNS receives it directly
 * and demuxes each client the same way it already demuxes any normal UDP
 * peer, by source address -- no local forwarder/loopback hop needed at
 * all. The registration challenge can't use this same trick: its nonce
 * reply has to reach app code, not get silently dropped by GNS's own
 * protocol parser as unrecognized noise, so it keeps its own small
 * dedicated raw socket (below).
 *
 * Unrelated to GatewayServerRegistration above: that class bootstraps a
 * per-connection DataChannel for GNS rendezvous signaling (topology "two
 * gateways bridging two WebRTC-only peers"); this class targets a real
 * native UDP GNS listen socket (topology "browser <-> gateway <-> native
 * UDP server") and carries no DataChannel/rendezvous traffic at all --
 * construct both side by side when a server should be reachable both
 * ways.
 *
 * Poll-driven like the rest of this file's classes: construct, call
 * Start(), then call Poll() once per tick for as long as the
 * registration should stay alive (handles the registration punch,
 * heartbeats, and every active per-client relay keepalive internally).
 */
class GatewayFleetRegistration
{
public:
    GatewayFleetRegistration(
        std::string registerUrl,
        std::string serverId,
        ISteamNetworkingSockets* sockets,
        HSteamListenSocket       listenSocket);
    ~GatewayFleetRegistration();

    void Start();
    void Poll();

    /*! True once the return-routability challenge has passed and the
     * gateway is actively routing browsers to this server. */
    bool Active() const;

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
    sockaddr_in m_gatewayAddr{}; /* resolved once in Start(); port is the
                                  * registration-punch target (same
                                  * number as -listen's HTTP port, see
                                  * .cpp) -- per-client relay punches below
                                  * reuse the IP with the client's own
                                  * relay port instead. */

    /*! Per browser client currently relaying to this server, keyed by
     * the gateway-assigned session ID (see "client-relay" in
     * onWebSocketMessage): just the gateway relay port to keep punching,
     * and when we last did. onClientRelay/onClientRelayClosed run on the
     * WebSocket's own callback thread; pollRelayKeepalives runs on
     * whatever thread calls Poll() (the main/render thread) -- guards
     * the map itself. */
    struct ClientRelay
    {
        int                                    relayPort{0};
        std::chrono::steady_clock::time_point lastPunch{};
    };
    std::mutex                                     m_relaysMutex;
    std::unordered_map<std::string, ClientRelay> m_relays;

    void onClientRelay(std::string const& sessionId, int relayPort);
    void onClientRelayClosed(std::string const& sessionId);
    void pollRelayKeepalives();
    void sendRelayPunch(int relayPort);
#endif

    mutable std::mutex                    m_mutex;
    bool                                   m_wsOpen{false};
    bool                                   m_active{false};
    std::chrono::steady_clock::time_point m_lastHeartbeat{};
    std::chrono::steady_clock::time_point m_lastRegistrationPunch{};
};

} // namespace webrtc_signaling

#endif
