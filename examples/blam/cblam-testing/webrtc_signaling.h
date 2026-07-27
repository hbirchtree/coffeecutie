#pragma once

#if defined(USE_NETWORKING) && defined(USE_WEBRTC_TRANSPORT)

#include <GameNetworkingSockets/steam/isteamnetworkingsockets.h>
#include <GameNetworkingSockets/steam/steamnetworkingcustomsignaling.h>

#include <rtc/rtc.hpp>

#include <atomic>
#include <chrono>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace webrtc_signaling {

/*!
 * Bootstraps a DataChannel against a webrtc-gateway (tools/webrtc-gateway)
 * "/signal" endpoint (SDP offer/answer, non-trickle ICE), then serves as
 * the ISteamNetworkingConnectionSignaling for the resulting GNS P2P
 * connection's own rendezvous handshake -- multiplexed as
 * "gns-rendezvous" messages over the same WebSocket once the SDP exchange
 * completes (see WEBRTC_TRANSPORT.md's "GNS-level signaling" section for
 * why both are needed, and the concrete wire protocol).
 *
 * Async by necessity: SDP/ICE gathering and the WebSocket round trip both
 * take real wall-clock time, so this can't just block the calling thread.
 * Construct, call Start(), then poll Ready()/Failed() once per frame
 * until one becomes true -- mirrors this codebase's existing poll-driven
 * Networking::start_restricted loop; there's no promise/future or
 * coroutine machinery elsewhere in this codebase to hook into instead.
 * Once Ready(), pass `this` to
 * ISteamNetworkingSockets::ConnectP2PWebRTCDataChannel along with
 * TakePeerConnection()/TakeDataChannel() -- GNS takes ownership of `this`
 * from that call onward (calls Release() when it's done with it, per the
 * ISteamNetworkingConnectionSignaling contract), so don't call
 * Start()/poll this object again afterward.
 */
class GatewayConnectBootstrap final : public ISteamNetworkingConnectionSignaling
{
  public:
    explicit GatewayConnectBootstrap(
        std::string gatewayUrl, std::string serverId = "");
    ~GatewayConnectBootstrap();

    void Start();

    bool Ready() const;
    bool Failed() const;

    std::string ServerTransport() const;

    /*! Valid only once Ready(); each may only be taken once. */
    std::shared_ptr<rtc::PeerConnection> TakePeerConnection();
    std::shared_ptr<rtc::DataChannel>    TakeDataChannel();

    /* ISteamNetworkingConnectionSignaling */
    bool SendSignal(
        HSteamNetConnection             hConn,
        const SteamNetConnectionInfo_t& info,
        const void*                     pMsg,
        int                             cbMsg) override;
    void Release() override;

  private:
    void onWebSocketOpen();
    void onWebSocketMessage(std::string const& text);
    void sendJSON(std::string const& json);
    /*! Sends the offer once both ICE gathering and the WebSocket are
     * ready -- order isn't fixed: libdatachannel (native) only starts
     * gathering once setLocalDescription() is called from
     * onWebSocketOpen(), so gathering always finishes after the socket is
     * open; datachannel-wasm has no setLocalDescription() at all --
     * gathering starts implicitly as soon as createDataChannel() runs in
     * Start(), often finishing *before* the socket even opens. Checking
     * both flags here instead of relying on a fixed order handles either
     * platform identically. */
    void maybeSendOffer();

    std::string                          m_gatewayUrl;
    std::string                          m_serverId;
    std::shared_ptr<rtc::WebSocket>      m_ws;
    std::shared_ptr<rtc::PeerConnection> m_pc;
    std::shared_ptr<rtc::DataChannel>    m_dc;

    /* Guards the fields below -- SendSignal() (called by GNS's own
     * networking thread, any time, per the interface contract) and the
     * libdatachannel callbacks (their own internal thread) both touch
     * these concurrently with Ready()/Failed() polling from the main
     * thread. */
    mutable std::mutex m_mutex;
    std::string        m_sessionId;
    bool               m_haveSessionId{false};
    bool               m_dataChannelOpen{false};
    bool               m_failed{false};
    bool               m_peerConnectionTaken{false};
    bool               m_dataChannelTaken{false};
    bool               m_gatheringComplete{false};
    bool               m_wsOpen{false};
    bool               m_offerSent{false};
    std::string        m_pendingOfferSdp;
    std::string        m_serverTransport;
};

class GatewayServerRegistration;

/*!
 * Accept-side counterpart to GatewayConnectBootstrap, for one incoming P2P
 * connection. Unlike the connect side, GNS rendezvous for this connection
 * does NOT flow over this object's own WebSocket -- it goes over the
 * owning GatewayServerRegistration's persistent "/server-signal"
 * connection instead, tagged with the session ID that same rendezvous
 * arrived under from gateway A. This object's own WebSocket dials a
 * (possibly different) gateway's "/signal" purely to bootstrap this
 * connection's own DataChannel (the actual data-plane transport) -- see
 * WEBRTC_TRANSPORT.md's "-relay-udp-port" note for why a second gateway
 * is needed to bridge two native WebRTC peers' DataChannels together.
 *
 * Returned from GatewayServerRegistration::OnConnectRequest, so GNS owns
 * it from construction onward (Release() -> delete this, same contract as
 * GatewayConnectBootstrap) -- never delete it yourself.
 */
class GatewayAcceptSignaling final : public ISteamNetworkingConnectionSignaling
{
  public:
    GatewayAcceptSignaling(
        GatewayServerRegistration* owner,
        std::string                sessionId,
        std::string                dataChannelGatewayUrl,
        HSteamNetConnection        hConn);
    ~GatewayAcceptSignaling();

    void Start();

    /*! False until Start() has run -- see GatewayServerRegistration::
     * PollPendingAccepts, which starts accepts on the tick thread. */
    bool Started() const
    {
        return m_started;
    }

    bool Ready() const;
    bool Failed() const;

    HSteamNetConnection Connection() const
    {
        return m_hConn;
    }

    std::string const& SessionId() const
    {
        return m_sessionId;
    }

    /*! Valid only once Ready(); each may only be taken once. */
    std::shared_ptr<rtc::PeerConnection> TakePeerConnection();
    std::shared_ptr<rtc::DataChannel>    TakeDataChannel();

    /* ISteamNetworkingConnectionSignaling */
    bool SendSignal(
        HSteamNetConnection             hConn,
        const SteamNetConnectionInfo_t& info,
        const void*                     pMsg,
        int                             cbMsg) override;
    void Release() override;

  private:
    void onWebSocketMessage(std::string const& text);
    /*! See GatewayConnectBootstrap::maybeSendOffer's comment -- same
     * platform-ordering divergence applies here. */
    void maybeSendOffer();

    GatewayServerRegistration* m_owner;
    std::string                m_sessionId;
    std::string                m_gatewayUrl;
    HSteamNetConnection        m_hConn;

    std::shared_ptr<rtc::WebSocket>      m_ws;
    std::shared_ptr<rtc::PeerConnection> m_pc;
    std::shared_ptr<rtc::DataChannel>    m_dc;

    mutable std::mutex m_mutex;
    bool               m_dataChannelOpen{false};
    bool               m_failed{false};
    bool               m_peerConnectionTaken{false};
    bool               m_dataChannelTaken{false};
    bool               m_gatheringComplete{false};
    bool               m_wsOpen{false};
    bool               m_offerSent{false};
    std::string        m_pendingOfferSdp;
    std::atomic<bool>  m_started{false};
};

/*!
 * Server-role signaling: owns the persistent "/server-signal" WebSocket
 * registration (see tools/webrtc-gateway) and implements
 * ISteamNetworkingSignalingRecvContext, so incoming GNS P2P connect
 * requests (routed by the gateway from any client's "/signal" session)
 * land here. Construct once at server startup, call Start(), then call
 * PollPendingAccepts() once per tick (mirrors GatewayConnectBootstrap's
 * poll-driven design): it finalizes any accepted connection whose own
 * per-connection DataChannel bootstrap (GatewayAcceptSignaling above) has
 * finished, calling AcceptP2PWebRTCDataChannel then AcceptConnection --
 * deferred, not done synchronously in OnConnectRequest, since that
 * DataChannel bootstrap takes real wall-clock time (see
 * ISteamNetworkingSignalingRecvContext::OnConnectRequest's own doc
 * comment: returning a signaling object leaves the connection in
 * "connecting" state rather than auto-accepting, precisely so this kind
 * of deferred accept is possible).
 *
 * Owned outright by Networking (unlike the two signaling classes above,
 * which GNS owns) -- construct as e.g. a member unique_ptr.
 */
class GatewayServerRegistration final
    : public ISteamNetworkingSignalingRecvContext
{
  public:
    GatewayServerRegistration(
        std::string              gatewayUrl,
        std::string              serverId,
        ISteamNetworkingSockets* sockets);
    ~GatewayServerRegistration();

    void Start();
    /*! Also sends the registration heartbeat, so call it once per tick for
     * as long as the registration should stay alive. */
    void PollPendingAccepts();

    /*! True once the gateway has confirmed the registration. */
    bool Active() const;

    /* ISteamNetworkingSignalingRecvContext */
    ISteamNetworkingConnectionSignaling* OnConnectRequest(
        HSteamNetConnection            hConn,
        const SteamNetworkingIdentity& identityPeer,
        int                            nLocalVirtualPort) override;
    void SendRejectionSignal(
        const SteamNetworkingIdentity& identityPeer,
        const void*                    pMsg,
        int                            cbMsg) override;

    /*! Used by GatewayAcceptSignaling; not for other callers. */
    bool SendOverServerSignal(
        std::string const& sessionId, std::string const& data);
    void RemovePendingAccept(GatewayAcceptSignaling* accept);

    /*! Marks a connection as established, which starts watching it for a
     * direct-route takeover (see PollPendingAccepts). */
    void NotifyGNSConnected(HSteamNetConnection hConn);
    void ForgetConnection(HSteamNetConnection hConn);

  private:
    void onWebSocketMessage(std::string const& text);
    void sendRegister();
    void sendHeartbeat();
    void flushOutgoingSignals();
    void drainIncomingSignals();
    void pollDirectRouteTakeover();

    std::string                     m_gatewayUrl;
    std::string                     m_serverId;
    ISteamNetworkingSockets*        m_sockets;
    std::shared_ptr<rtc::WebSocket> m_ws;

    std::mutex m_mutex;
    bool       m_active{false};
    std::string m_trackingId;
    std::chrono::steady_clock::time_point m_lastHeartbeat{};
    /* Set just before each ReceivedP2PCustomSignal call, consumed by
     * OnConnectRequest if that call triggers one synchronously (it always
     * does in practice -- GNS has no other opportunity to ask for a
     * signaling object). Scoped to a single call, not a session->object
     * map: GNS itself handles routing an incoming rendezvous message to
     * an EXISTING connection's signaling object once one exists; this is
     * only needed for the "brand new connection" case. */
    std::string                          m_pendingSessionId;
    std::vector<GatewayAcceptSignaling*> m_pendingAccepts;
    std::unordered_map<HSteamNetConnection, std::string>
        m_sessionIdByConnection;
    std::unordered_map<HSteamNetConnection, int> m_directRouteTicks;
    std::unordered_set<HSteamNetConnection>      m_relayRetired;
    std::vector<std::pair<std::string, std::string>> m_incoming;
    std::vector<std::string> m_outgoing;
};

} // namespace webrtc_signaling

#endif
