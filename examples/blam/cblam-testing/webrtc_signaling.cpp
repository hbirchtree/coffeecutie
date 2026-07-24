#include "webrtc_signaling.h"

#if defined(USE_NETWORKING) && defined(USE_WEBRTC_TRANSPORT)

#include <GameNetworkingSockets/steam/steamnetworkingsockets.h>

#include <coffee/core/debug/formatting.h>
#include <nlohmann/json.hpp>
#include <peripherals/identify/system.h>
#include <peripherals/semantic/chunk.h>
#include <peripherals/stl/base64.h>

#include <algorithm>
#include <variant>

using namespace Coffee::Logging;

namespace webrtc_signaling {

GatewayConnectBootstrap::GatewayConnectBootstrap(
    std::string gatewayUrl, std::string serverId)
    : m_gatewayUrl(std::move(gatewayUrl))
    , m_serverId(std::move(serverId))
{
}

GatewayConnectBootstrap::~GatewayConnectBootstrap()
{
}

void GatewayConnectBootstrap::Start()
{
    rtc::Configuration config;
    /* Same STUN server Networking::connect_symmetric already uses for
     * GNS's own ICE transport -- kept consistent, not load-bearing here
     * beyond "a STUN server that works in this environment." */
    config.iceServers.emplace_back("stun:stun.l.google.com:19302");

    m_pc = std::make_shared<rtc::PeerConnection>(config);

    rtc::Reliability reliability;
    reliability.unordered      = true;
    reliability.maxRetransmits = 0;
    rtc::DataChannelInit dcInit;
    dcInit.reliability = reliability;
    m_dc               = m_pc->createDataChannel("gns", dcInit);

    m_dc->onOpen([this]() {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_dataChannelOpen = true;
    });
    m_dc->onError([this](std::string error) {
        cWarning("webrtc_signaling: data channel error: {}", error);
        std::lock_guard<std::mutex> lock(m_mutex);
        m_failed = true;
    });

    m_pc->onGatheringStateChange(
        [this](rtc::PeerConnection::GatheringState state) {
            if(state != rtc::PeerConnection::GatheringState::Complete)
                return;
            auto desc = m_pc->localDescription();
            if(!desc)
            {
                cWarning(
                    "webrtc_signaling: gathering complete but no local "
                    "description");
                std::lock_guard<std::mutex> lock(m_mutex);
                m_failed = true;
                return;
            }
            {
                std::lock_guard<std::mutex> lock(m_mutex);
                m_pendingOfferSdp  = std::string(*desc);
                m_gatheringComplete = true;
            }
            maybeSendOffer();
        });
    m_pc->onStateChange([this](rtc::PeerConnection::State state) {
        if(state == rtc::PeerConnection::State::Failed ||
           state == rtc::PeerConnection::State::Closed)
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_failed = true;
        }
    });

    m_ws = std::make_shared<rtc::WebSocket>();
    m_ws->onOpen([this]() { onWebSocketOpen(); });
    m_ws->onMessage([this](rtc::message_variant data) {
        if(!std::holds_alternative<std::string>(data))
        {
            cWarning("webrtc_signaling: ignoring unexpected binary signal "
                      "message");
            return;
        }
        onWebSocketMessage(std::get<std::string>(data));
    });
    m_ws->onError([this](std::string error) {
        cWarning("webrtc_signaling: signaling websocket error: {}", error);
        std::lock_guard<std::mutex> lock(m_mutex);
        m_failed = true;
    });

    m_ws->open(
        m_gatewayUrl + "/signal" +
        (m_serverId.empty() ? "" : "?server=" + m_serverId));
}

void GatewayConnectBootstrap::onWebSocketOpen()
{
#if !defined(COFFEE_WASM)
    /* Triggers offer creation + ICE gathering (see onGatheringStateChange
     * above for where the offer actually gets sent) -- libdatachannel
     * needs this explicit call; datachannel-wasm has no such method at
     * all, gathering starts implicitly as soon as createDataChannel() ran
     * in Start(), well before the socket is even open (its JS glue wires
     * the standard onnegotiationneeded -> createOffer ->
     * setLocalDescription dance internally). See maybeSendOffer()'s
     * comment for why checking both flags there, rather than relying on
     * a fixed ordering, is what actually makes this platform difference
     * safe to ignore everywhere else. */
    m_pc->setLocalDescription();
#endif
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_wsOpen = true;
    }
    maybeSendOffer();
}

void GatewayConnectBootstrap::maybeSendOffer()
{
    std::string sdp;
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        if(!m_gatheringComplete || !m_wsOpen || m_offerSent)
            return;
        m_offerSent = true;
        sdp         = m_pendingOfferSdp;
    }
    nlohmann::json offer{
        {"type", "offer"},
        {"sdp", sdp},
    };
    sendJSON(offer.dump());
}

void GatewayConnectBootstrap::onWebSocketMessage(std::string const& text)
{
    cDebug("Gateway message: {}", text);
    nlohmann::json msg;
    try
    {
        msg = nlohmann::json::parse(text);
    } catch(nlohmann::json::parse_error const& e)
    {
        cWarning("webrtc_signaling: failed to parse signal message: {}", e.what());
        return;
    }

    auto type = msg.value("type", std::string());
    if(type == "answer")
    {
        auto sdp       = msg.value("sdp", std::string());
        auto sessionId = msg.value("sessionId", std::string());
        if(sdp.empty() || sessionId.empty())
        {
            cWarning("webrtc_signaling: malformed answer message");
            std::lock_guard<std::mutex> lock(m_mutex);
            m_failed = true;
            return;
        }
        m_pc->setRemoteDescription(rtc::Description(sdp, "answer"));
        std::lock_guard<std::mutex> lock(m_mutex);
        m_sessionId    = std::move(sessionId);
        m_haveSessionId = true;
    } else if(type == "gns-rendezvous")
    {
        auto data = msg.value("data", std::string());
        auto raw  = b64::decode(data);
        if(raw.empty())
        {
            cWarning("webrtc_signaling: empty gns-rendezvous payload");
            return;
        }
        /* Threading: ReceivedP2PCustomSignal is documented as safe to
         * call any time, from any thread -- see
         * ISteamNetworkingConnectionSignaling::SendSignal's own doc
         * comment, which describes the same contract for the other
         * direction of this same rendezvous exchange. */
        SteamNetworkingSockets()->ReceivedP2PCustomSignal(
            raw.data(), static_cast<int>(raw.size()), nullptr);
    } else if(type == "error")
    {
        cWarning(
            "webrtc_signaling: gateway error: {}",
            msg.value("data", std::string()));
        std::lock_guard<std::mutex> lock(m_mutex);
        m_failed = true;
    } else
    {
        cWarning("webrtc_signaling: unexpected signal message type {}", type);
    }
}

void GatewayConnectBootstrap::sendJSON(std::string const& json)
{
    if(!m_ws->send(json))
        cWarning("webrtc_signaling: failed to send signal message");
}

bool GatewayConnectBootstrap::Ready() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_haveSessionId && m_dataChannelOpen && !m_failed;
}

bool GatewayConnectBootstrap::Failed() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_failed;
}

std::shared_ptr<rtc::PeerConnection> GatewayConnectBootstrap::TakePeerConnection()
{
    if(m_peerConnectionTaken)
        cWarning("webrtc_signaling: TakePeerConnection() called more than once");
    m_peerConnectionTaken = true;
    return m_pc;
}

std::shared_ptr<rtc::DataChannel> GatewayConnectBootstrap::TakeDataChannel()
{
    if(m_dataChannelTaken)
        cWarning("webrtc_signaling: TakeDataChannel() called more than once");
    m_dataChannelTaken = true;
    return m_dc;
}

bool GatewayConnectBootstrap::SendSignal(
    HSteamNetConnection /*hConn*/,
    const SteamNetConnectionInfo_t& /*info*/,
    const void* pMsg,
    int         cbMsg)
{
    std::string sessionId;
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        sessionId = m_sessionId;
    }
    if(sessionId.empty())
    {
        /* Can't happen in practice: GNS only calls SendSignal after
         * ConnectP2PWebRTCDataChannel has been handed this object, which
         * only happens once Ready() -- which requires m_haveSessionId. */
        cWarning("webrtc_signaling: SendSignal with no session ID yet");
        return false;
    }

    auto data = b64::encode<const char>(semantic::Span<const char>(
        reinterpret_cast<const char*>(pMsg), cbMsg));

    nlohmann::json rendezvous{
        {"type", "gns-rendezvous"},
        {"sessionId", sessionId},
        {"data", data},
    };
    return m_ws->send(rendezvous.dump());
}

void GatewayConnectBootstrap::Release()
{
    if(m_ws)
        m_ws->close();
    delete this;
}

/* ==================== GatewayAcceptSignaling ==================== */

GatewayAcceptSignaling::GatewayAcceptSignaling(
    GatewayServerRegistration* owner,
    std::string                sessionId,
    std::string                dataChannelGatewayUrl,
    HSteamNetConnection        hConn)
    : m_owner(owner)
    , m_sessionId(std::move(sessionId))
    , m_gatewayUrl(std::move(dataChannelGatewayUrl))
    , m_hConn(hConn)
{
}

GatewayAcceptSignaling::~GatewayAcceptSignaling()
{
}

void GatewayAcceptSignaling::Start()
{
    rtc::Configuration config;
    config.iceServers.emplace_back("stun:stun.l.google.com:19302");
    m_pc = std::make_shared<rtc::PeerConnection>(config);

    /* This object's own /signal dial plays the same "offerer" role
     * GatewayConnectBootstrap plays against gateway A -- it's the one
     * initiating a connection to (a possibly different) gateway B, purely
     * to bootstrap this GNS connection's data-plane DataChannel. No
     * gns-rendezvous ever flows over this socket; that goes over
     * m_owner's /server-signal instead (see SendSignal below). */
    rtc::Reliability reliability;
    reliability.unordered      = true;
    reliability.maxRetransmits = 0;
    rtc::DataChannelInit dcInit;
    dcInit.reliability = reliability;
    m_dc               = m_pc->createDataChannel("gns", dcInit);

    m_dc->onOpen([this]() {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_dataChannelOpen = true;
    });
    m_dc->onError([this](std::string error) {
        cWarning("webrtc_signaling (accept): data channel error: {}", error);
        std::lock_guard<std::mutex> lock(m_mutex);
        m_failed = true;
    });

    m_pc->onGatheringStateChange(
        [this](rtc::PeerConnection::GatheringState state) {
            if(state != rtc::PeerConnection::GatheringState::Complete)
                return;
            auto desc = m_pc->localDescription();
            if(!desc)
            {
                cWarning(
                    "webrtc_signaling (accept): gathering complete but no "
                    "local description");
                std::lock_guard<std::mutex> lock(m_mutex);
                m_failed = true;
                return;
            }
            {
                std::lock_guard<std::mutex> lock(m_mutex);
                m_pendingOfferSdp  = std::string(*desc);
                m_gatheringComplete = true;
            }
            maybeSendOffer();
        });
    m_pc->onStateChange([this](rtc::PeerConnection::State state) {
        if(state == rtc::PeerConnection::State::Failed ||
           state == rtc::PeerConnection::State::Closed)
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_failed = true;
        }
    });

    m_ws = std::make_shared<rtc::WebSocket>();
    m_ws->onOpen([this]() {
#if !defined(COFFEE_WASM)
        /* See GatewayConnectBootstrap::onWebSocketOpen's comment -- same
         * native-vs-wasm gathering-trigger divergence applies here. */
        m_pc->setLocalDescription();
#endif
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_wsOpen = true;
        }
        maybeSendOffer();
    });
    m_ws->onMessage([this](rtc::message_variant data) {
        if(!std::holds_alternative<std::string>(data))
        {
            cWarning("webrtc_signaling (accept): ignoring unexpected "
                      "binary transport message");
            return;
        }
        onWebSocketMessage(std::get<std::string>(data));
    });
    m_ws->onError([this](std::string error) {
        cWarning("webrtc_signaling (accept): transport websocket error: {}", error);
        std::lock_guard<std::mutex> lock(m_mutex);
        m_failed = true;
    });
    m_ws->open(m_gatewayUrl + "/signal");
}

void GatewayAcceptSignaling::maybeSendOffer()
{
    std::string sdp;
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        if(!m_gatheringComplete || !m_wsOpen || m_offerSent)
            return;
        m_offerSent = true;
        sdp         = m_pendingOfferSdp;
    }
    nlohmann::json offer{
        {"type", "offer"},
        {"sdp", sdp},
    };
    if(!m_ws->send(offer.dump()))
        cWarning("webrtc_signaling (accept): failed to send offer");
}

void GatewayAcceptSignaling::onWebSocketMessage(std::string const& text)
{
    nlohmann::json msg;
    try
    {
        msg = nlohmann::json::parse(text);
    } catch(nlohmann::json::parse_error const& e)
    {
        cWarning(
            "webrtc_signaling (accept): failed to parse transport message: {}",
            e.what());
        return;
    }

    auto type = msg.value("type", std::string());
    if(type == "answer")
    {
        auto sdp = msg.value("sdp", std::string());
        if(sdp.empty())
        {
            cWarning("webrtc_signaling (accept): malformed answer message");
            std::lock_guard<std::mutex> lock(m_mutex);
            m_failed = true;
            return;
        }
        m_pc->setRemoteDescription(rtc::Description(sdp, "answer"));
        /* This gateway's own per-connection session ID (returned
         * alongside the answer, same as the client role gets one) is
         * deliberately not kept: nothing ever needs to route a message
         * back to this specific /signal dial after the DataChannel opens
         * -- GNS rendezvous for this connection flows over
         * m_owner's /server-signal, tagged with the session ID gateway A
         * assigned the ORIGINAL client, not this one. */
    } else if(type == "error")
    {
        cWarning(
            "webrtc_signaling (accept): gateway error: {}",
            msg.value("data", std::string()));
        std::lock_guard<std::mutex> lock(m_mutex);
        m_failed = true;
    } else
    {
        cWarning(
            "webrtc_signaling (accept): unexpected transport message type {}",
            type);
    }
}

bool GatewayAcceptSignaling::Ready() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_dataChannelOpen && !m_failed;
}

bool GatewayAcceptSignaling::Failed() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_failed;
}

std::shared_ptr<rtc::PeerConnection> GatewayAcceptSignaling::TakePeerConnection()
{
    if(m_peerConnectionTaken)
        cWarning("webrtc_signaling (accept): TakePeerConnection() called more than once");
    m_peerConnectionTaken = true;
    return m_pc;
}

std::shared_ptr<rtc::DataChannel> GatewayAcceptSignaling::TakeDataChannel()
{
    if(m_dataChannelTaken)
        cWarning("webrtc_signaling (accept): TakeDataChannel() called more than once");
    m_dataChannelTaken = true;
    return m_dc;
}

bool GatewayAcceptSignaling::SendSignal(
    HSteamNetConnection /*hConn*/,
    const SteamNetConnectionInfo_t& /*info*/,
    const void* pMsg,
    int         cbMsg)
{
    auto data = b64::encode<const char>(semantic::Span<const char>(
        reinterpret_cast<const char*>(pMsg), cbMsg));
    return m_owner->SendOverServerSignal(m_sessionId, data);
}

void GatewayAcceptSignaling::Release()
{
    m_owner->RemovePendingAccept(this);
    if(m_ws)
        m_ws->close();
    delete this;
}

/* ==================== GatewayServerRegistration ==================== */

GatewayServerRegistration::GatewayServerRegistration(
    std::string              serverSignalGatewayUrl,
    std::string              dataChannelGatewayUrl,
    ISteamNetworkingSockets* sockets)
    : m_serverSignalUrl(std::move(serverSignalGatewayUrl))
    , m_dataChannelGatewayUrl(std::move(dataChannelGatewayUrl))
    , m_sockets(sockets)
{
}

GatewayServerRegistration::~GatewayServerRegistration()
{
    if(m_ws)
        m_ws->close();
}

void GatewayServerRegistration::Start()
{
    m_ws = std::make_shared<rtc::WebSocket>();
    m_ws->onOpen(
        [] { cDebug("webrtc_signaling: registered with gateway /server-signal"); });
    m_ws->onMessage([this](rtc::message_variant data) {
        if(!std::holds_alternative<std::string>(data))
        {
            cWarning("webrtc_signaling: ignoring unexpected binary "
                      "server-signal message");
            return;
        }
        onWebSocketMessage(std::get<std::string>(data));
    });
    m_ws->onError([](std::string error) {
        cWarning("webrtc_signaling: server-signal websocket error: {}", error);
    });
    m_ws->open(m_serverSignalUrl + "/server-signal");
}

void GatewayServerRegistration::onWebSocketMessage(std::string const& text)
{
    nlohmann::json msg;
    try
    {
        msg = nlohmann::json::parse(text);
    } catch(nlohmann::json::parse_error const& e)
    {
        cWarning(
            "webrtc_signaling: failed to parse server-signal message: {}",
            e.what());
        return;
    }

    auto type = msg.value("type", std::string());
    if(type == "gns-rendezvous")
    {
        auto sessionId = msg.value("sessionId", std::string());
        auto raw       = b64::decode(msg.value("data", std::string()));
        if(sessionId.empty() || raw.empty())
        {
            cWarning("webrtc_signaling: malformed gns-rendezvous from server-signal");
            return;
        }
        /* Consumed by OnConnectRequest below if this rendezvous message
         * turns out to be a brand-new connection request -- see the
         * member's own doc comment for why a single scratch field is
         * enough (GNS itself routes to an existing connection's own
         * signaling object without needing OnConnectRequest at all). */
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_pendingSessionId = std::move(sessionId);
        }
        m_sockets->ReceivedP2PCustomSignal(
            raw.data(), static_cast<int>(raw.size()), this);
    } else if(type == "error")
    {
        cWarning(
            "webrtc_signaling: gateway error: {}",
            msg.value("data", std::string()));
    } else
    {
        cWarning("webrtc_signaling: unexpected server-signal message type {}", type);
    }
}

ISteamNetworkingConnectionSignaling* GatewayServerRegistration::OnConnectRequest(
    HSteamNetConnection hConn, const SteamNetworkingIdentity&, int)
{
    std::string sessionId;
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        sessionId = m_pendingSessionId;
    }
    if(sessionId.empty())
    {
        /* Shouldn't happen in practice (see onWebSocketMessage), but per
         * the interface's own doc comment, returning nullptr here is the
         * SAFE default anyway -- silently ignoring an unroutable request
         * rather than actively rejecting it. */
        cWarning("webrtc_signaling: OnConnectRequest with no pending session ID");
        return nullptr;
    }

    auto* accept = new GatewayAcceptSignaling(
        this, sessionId, m_dataChannelGatewayUrl, hConn);
    accept->Start();
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_pendingAccepts.push_back(accept);
    }
    return accept;
}

void GatewayServerRegistration::SendRejectionSignal(
    const SteamNetworkingIdentity&, const void*, int)
{
    /* Not implemented in v1 -- OnConnectRequest only ever returns nullptr
     * on the "no session ID" edge case above, which GNS treats as silent
     * ignore, not active rejection (this callback is for the latter). */
}

bool GatewayServerRegistration::SendOverServerSignal(
    std::string const& sessionId, std::string const& data)
{
    if(!m_ws)
        return false;
    nlohmann::json rendezvous{
        {"type", "gns-rendezvous"},
        {"sessionId", sessionId},
        {"data", data},
    };
    return m_ws->send(rendezvous.dump());
}

void GatewayServerRegistration::RemovePendingAccept(GatewayAcceptSignaling* accept)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = std::find(m_pendingAccepts.begin(), m_pendingAccepts.end(), accept);
    if(it != m_pendingAccepts.end())
        m_pendingAccepts.erase(it);
}

void GatewayServerRegistration::PollPendingAccepts()
{
    std::vector<GatewayAcceptSignaling*> ready;
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        for(auto it = m_pendingAccepts.begin(); it != m_pendingAccepts.end();)
        {
            auto* accept = *it;
            if(accept->Failed())
            {
                cWarning(
                    "webrtc_signaling: accept-side DataChannel bootstrap "
                    "failed for connection {}",
                    accept->Connection());
                m_sockets->CloseConnection(accept->Connection(), 0, nullptr, false);
                it = m_pendingAccepts.erase(it);
                continue;
            }
            if(accept->Ready())
            {
                ready.push_back(accept);
                it = m_pendingAccepts.erase(it);
                continue;
            }
            ++it;
        }
    }
    for(auto* accept : ready)
    {
        auto hConn = accept->Connection();
        auto pc    = accept->TakePeerConnection();
        auto dc    = accept->TakeDataChannel();
        if(!m_sockets->AcceptP2PWebRTCDataChannel(hConn, pc, dc))
        {
            cWarning("webrtc_signaling: AcceptP2PWebRTCDataChannel failed for connection {}", hConn);
            m_sockets->CloseConnection(hConn, 0, nullptr, false);
            continue;
        }
        if(m_sockets->AcceptConnection(hConn) != k_EResultOK)
        {
            cWarning("webrtc_signaling: AcceptConnection failed for connection {}", hConn);
            m_sockets->CloseConnection(hConn, 0, nullptr, false);
        }
    }
}

} // namespace webrtc_signaling

#endif
