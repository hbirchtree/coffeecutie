#pragma once

#if defined(USE_NETWORKING) && defined(USE_WEBRTC_TRANSPORT)

#include <GameNetworkingSockets/steam/isteamnetworkingsockets.h>
#include <GameNetworkingSockets/steam/steamnetworkingcustomsignaling.h>

#include <rtc/rtc.hpp>

#include <chrono>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

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
class GatewayFleetRegistration;

/*!
 * Outgoing half of one P2P connection accepted by a UDP-based server:
 * GNS hands rendezvous to this, and it goes back out over the fleet
 * registration's existing /server-signal socket, tagged with the session the
 * request arrived under.
 *
 * Deliberately far smaller than GatewayAcceptSignaling (webrtc_signaling.h),
 * which has to stand up a whole DataChannel because that IS its data plane.
 * Here the data plane is the server's own UDP socket, so the gateway only ever
 * carries signaling and nothing else needs building.
 *
 * Returned from GatewayFleetRegistration::OnConnectRequest, so GNS owns it from
 * construction onward (Release() -> delete this) -- never delete it yourself.
 */
class FleetAcceptSignaling final : public ISteamNetworkingConnectionSignaling
{
  public:
    FleetAcceptSignaling(
        GatewayFleetRegistration* owner, std::string sessionId);

    /* ISteamNetworkingConnectionSignaling */
    bool SendSignal(
        HSteamNetConnection             hConn,
        const SteamNetConnectionInfo_t& info,
        const void*                     pMsg,
        int                             cbMsg) override;
    void Release() override;

  private:
    GatewayFleetRegistration* m_owner;
    std::string               m_sessionId;
};

class GatewayFleetRegistration final
    : public ISteamNetworkingSignalingRecvContext
{
  public:
    GatewayFleetRegistration(
        std::string              registerUrl,
        std::string              serverId,
        ISteamNetworkingSockets* sockets,
        HSteamListenSocket       listenSocket,
        /*! Advertise the relay only, and refuse rendezvous, so no client can
         * reach this server directly and clients never learn each other's
         * addresses. */
        bool relayOnly = false);
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

    /*! Sends one GNS rendezvous blob back to the client on the far side of
     * sessionId. Called by FleetAcceptSignaling on GNS's own thread. */
    bool SendRendezvous(
        std::string const& sessionId, const void* msg, int msgLen);

    ISteamNetworkingConnectionSignaling* OnConnectRequest(
        HSteamNetConnection            hConn,
        const SteamNetworkingIdentity& identityPeer,
        int                            nLocalVirtualPort) override;
    void SendRejectionSignal(
        const SteamNetworkingIdentity& identityPeer,
        const void*                    pMsg,
        int                            cbMsg) override;

  private:
    void onWebSocketOpen();
    void onWebSocketMessage(std::string const& text);
    void sendRegister();
    void sendHeartbeat();
    void pollChallengeSocket();
    void sendRegistrationPunch();
    void drainIncomingSignals();
    /*! Once ICE has taken a connection off the relay, tell the gateway so it
     * can tear the relay down -- otherwise it stays bound and keeps being
     * punched for a path nothing is using. */
    void pollDirectRouteTakeover();

    std::string m_registerUrl;
    std::string m_serverId;

    ISteamNetworkingSockets* m_sockets;
    HSteamListenSocket       m_listenSocket;
    bool                     m_relayOnly{false};

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

    /* Rendezvous received from the gateway, waiting to be fed to GNS */
    std::vector<std::pair<std::string, std::string>> m_incomingSignals;
    /* Set immediately before each ReceivedP2PCustomSignal */
    std::string m_pendingSessionId;

    /* Connections accepted over rendezvous */
    std::unordered_map<HSteamNetConnection, std::string> m_sessionByConnection;
    std::unordered_map<HSteamNetConnection, int>         m_directRouteTicks;
    std::unordered_set<HSteamNetConnection>              m_relayRetired;
};

} // namespace webrtc_signaling

#endif
