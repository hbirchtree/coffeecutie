//====== Added for coffeecutie: WebRTC-DataChannel-backed P2P transport =====
// See steamnetworkingsockets_p2p_webrtc_datachannel.h for the design summary.

#include "steamnetworkingsockets_p2p_webrtc_datachannel.h"

#include "csteamnetworkingsockets.h"
#include "steamnetworkingsockets_udp.h"

#include <rtc/rtc.hpp>

namespace SteamNetworkingSocketsLib {

// Wire framing: [uint16 little-endian sequence number][encrypted chunk].
// Mirrors UDPDataMsgHdr::m_unSeqNum's width (see steamnetworkingsockets_udp.h)
// since DecryptDataChunk's nWireSeqNum parameter is uint16 -- this is the
// minimum framing GNS's AEAD nonce construction actually requires. No
// connection-ID field (this transport is already 1:1 with one DataChannel,
// nothing to disambiguate) and no inline stats blob (v1; GNS's SNP layer
// still does its own reliability/ordering on top, so this is a throughput
// optimization to add later, not a correctness requirement).
static constexpr size_t k_cbWireSeqNumHeader = 2;

CConnectionTransportP2PWebRTC::CConnectionTransportP2PWebRTC(
	CSteamNetworkConnectionP2P &connection,
	std::shared_ptr<rtc::PeerConnection> pPeerConnection,
	std::shared_ptr<rtc::DataChannel> pDataChannel
)
: CConnectionTransport( connection )
, CConnectionTransportP2PBase( "WebRTC", this )
, m_pPeerConnection( std::move( pPeerConnection ) )
, m_pDataChannel( std::move( pDataChannel ) )
{
	Assert( m_pDataChannel );
	Assert( m_pDataChannel->isOpen() );

	// Callbacks are bound separately, after the caller drops the global
	// lock -- see BindDataChannelCallbacks()'s comment.
}

void CConnectionTransportP2PWebRTC::BindDataChannelCallbacks()
{
	if ( !m_pDataChannel )
		return;

	m_pDataChannel->onMessage(
		[this]( rtc::binary data ) {
			OnDataChannelMessage( data.data(), data.size() );
		},
		[this]( std::string ) {
			// Text frames are never sent by either side of this transport
			// (see SendEncryptedDataChunk -- always binary); ignore rather
			// than assert, in case something upstream of us ever changes.
		}
	);
	m_pDataChannel->onClosed( [this]() { OnDataChannelClosed(); } );
}

CConnectionTransportP2PWebRTC::~CConnectionTransportP2PWebRTC()
{
}

void CConnectionTransportP2PWebRTC::TransportFreeResources()
{
	if ( m_pDataChannel )
	{
#ifndef __EMSCRIPTEN__
		// datachannel-wasm's DataChannel has no resetCallbacks() at all
		// (libdatachannel does) -- safe to just skip it there, not merely
		// a workaround: the coffeecutie wasm build runs GNS in
		// SetManualPollMode(true) specifically so this transport is only
		// ever touched from the single browser main thread (see
		// examples/blam/cblam-testing/WEBRTC_TRANSPORT.md's Phase 3
		// threading note), so there is no *other* thread that could still
		// be mid-callback (with a soon-to-be-dangling `this`) when this
		// runs, unlike the native multi-threaded case resetCallbacks()
		// guards against.
		m_pDataChannel->resetCallbacks();
#endif
		m_pDataChannel->close();
		m_pDataChannel.reset();
	}
	m_pPeerConnection.reset();

	CConnectionTransport::TransportFreeResources();
}

bool CConnectionTransportP2PWebRTC::BCanSendEndToEndData() const
{
	return m_pDataChannel && m_pDataChannel->isOpen();
}

bool CConnectionTransportP2PWebRTC::SendDataPacket( SteamNetworkingMicroseconds usecNow )
{
	SendPacketContext_t ctx( usecNow, "data" );
	// SendPacketContext_t::m_cbMaxEncryptedPayload has no default member
	// initializer and nothing else sets it for a bare (non-templated)
	// context like this one -- the UDP transport only ever constructs the
	// templated SendPacketContext<CMsgSteamSockets_UDP_Stats>, whose
	// Populate()/SlamFlagsAndCalcSize() computes it as
	// (MTU - header reserve - stats msg size) before SNP_SendPacket ever
	// runs. Left unset here, SNP_SendPacket's packer
	// (m_cbMaxPlaintextPayload = max(0, ctx.m_cbMaxEncryptedPayload -
	// encryption overhead), steamnetworkingsockets_snp.cpp) read
	// uninitialized stack garbage for how much room it had -- a real,
	// confirmed bug: caused GNS to believe it had a bogus (frequently ~0)
	// send budget, so ordinary application messages never actually made it
	// out despite the connection reporting Connected (see
	// WEBRTC_TRANSPORT.md). Fixed to mirror the UDP calculation with our
	// own (much smaller) header reserve.
	ctx.m_cbMaxEncryptedPayload = Connection().m_cbMTUPacketSize - (int)k_cbWireSeqNumHeader;
	return m_connection.SNP_SendPacket( this, ctx );
}

int CConnectionTransportP2PWebRTC::SendEncryptedDataChunk( const void *pChunk, int cbChunk, SendPacketContext_t &ctx )
{
	if ( !BCanSendEndToEndData() )
		return 0;

	uint16 nWireSeqNum = Connection().m_statsEndToEnd.ConsumeSendPacketNumberAndGetWireFmt( ctx.m_usecNow );

	std::vector<std::byte> pkt( k_cbWireSeqNumHeader + (size_t)cbChunk );
	pkt[0] = std::byte( nWireSeqNum & 0xff );
	pkt[1] = std::byte( ( nWireSeqNum >> 8 ) & 0xff );
	memcpy( pkt.data() + k_cbWireSeqNumHeader, pChunk, cbChunk );

	// DataChannel::send returns false if the message was buffered rather
	// than sent immediately due to backpressure (bufferedAmount() over
	// threshold) -- treated as a dropped packet here, matching plain UDP's
	// fire-and-forget semantics (see WEBRTC_TRANSPORT.md), not queued:
	// GNS's own SNP layer already owns retransmission/pacing decisions,
	// queuing again underneath it would just add unaccounted-for latency.
	if ( !m_pDataChannel->send( pkt.data(), pkt.size() ) )
		return 0;

	return (int)pkt.size();
}

void CConnectionTransportP2PWebRTC::OnDataChannelMessage( const void *pData, size_t cbData )
{
	if ( cbData < k_cbWireSeqNumHeader )
		return; // malformed -- too short to even hold the seqnum

	// Called on libdatachannel's own internal thread, NOT any GNS-owned
	// thread -- DecryptDataChunk/ProcessPlainTextDataChunk touch shared
	// connection state and require SteamNetworkingGlobalLock (process-wide)
	// plus a per-connection ConnectionScopeLock to already be held by the
	// caller; neither was taken here originally. Confirmed against Valve's
	// own precedent for this exact situation (a third-party WebRTC library
	// delivering data asynchronously from its own thread):
	// CConnectionTransportP2PICE_WebRTC::OnData in
	// steamnetworkingsockets_p2p_webrtc.cpp (STEAMNETWORKINGSOCKETS_ENABLE_WEBRTC,
	// Valve's Google-webrtc.org-backed ICE transport -- a different feature
	// from this one, but the closest real analogue) takes exactly these two
	// locks before touching the connection. Missing them caused a real,
	// reproducible full-process hang the first time this transport was
	// actually run end-to-end (not an assert -- a silent deadlock; see
	// examples/blam/cblam-testing/WEBRTC_TRANSPORT.md), consistent with
	// this thread and GNS's own service thread racing on unsynchronized
	// connection state.
	//
	// Simpler than ICE's version: no queue-if-contended fallback, because
	// this transport has no writable-state-changed/route-changed concept
	// to interleave with data delivery the way multi-candidate ICE does
	// (BCanSendEndToEndData() already covers the only connectivity state
	// that matters here) -- block-acquiring both locks is sufficient and
	// safe: this callback runs on a dedicated per-PeerConnection thread,
	// not one shared with unrelated connections' I/O.
	SteamNetworkingGlobalLock lock( "CConnectionTransportP2PWebRTC::OnDataChannelMessage" );
	ConnectionScopeLock connectionLock( m_connection, "CConnectionTransportP2PWebRTC::OnDataChannelMessage" );

	const uint8 *p = (const uint8 *)pData;
	uint16 nWireSeqNum = uint16( p[0] ) | ( uint16( p[1] ) << 8 );

	const void *pChunk = p + k_cbWireSeqNumHeader;
	int cbChunk = (int)( cbData - k_cbWireSeqNumHeader );

	SteamNetworkingMicroseconds usecNow = SteamNetworkingSockets_GetLocalTimestamp();

	RecvPacketContext_t ctx;
	ctx.m_usecNow = usecNow;
	ctx.m_pTransport = this;

	if ( !m_connection.DecryptDataChunk( nWireSeqNum, (int)cbData, pChunk, cbChunk, ctx ) )
		return;

	int usecTimeSinceLast = 0; // FIXME - plumb through for jitter measurement, same as UDP transport's own FIXME
	m_connection.ProcessPlainTextDataChunk( usecTimeSinceLast, ctx );
}

void CConnectionTransportP2PWebRTC::OnDataChannelClosed()
{
	// Let the P2P connection's transport-selection logic notice on its own
	// next think tick (BCanSendEndToEndData() now returns false) rather
	// than forcing an immediate re-evaluation here -- mirrors how the ICE
	// transport handles its own underlying connectivity loss.
}

void CConnectionTransportP2PWebRTC::SendEndToEndStatsMsg( EStatsReplyRequest eRequest, SteamNetworkingMicroseconds usecNow, const char *pszReason )
{
	// v1: no inline stats piggybacking (see file header comment) -- stats
	// still flow, just as their own ordinary SNP-layer messages rather than
	// piggybacked on a data chunk's header. Revisit if that's measurably
	// worse than the UDP transport's piggybacking once this is running.
	//
	// This used to be a complete no-op -- a real, confirmed bug: GNS calls
	// this specifically because it has a stats/keepalive send need that
	// isn't getting satisfied by ordinary data sends (or there's no data to
	// piggyback on), and an empty body meant that need could NEVER be
	// satisfied, ever -- observed as "Stats sending didn't clear stats
	// need to send reason E2EKeepAlive!" repeating forever in the log (see
	// WEBRTC_TRANSPORT.md). eRequest itself isn't threaded through (no
	// protobuf stats message to carry its urgency, unlike UDP's
	// CMsgSteamSockets_UDP_Stats) -- just giving GNS's SNP layer a send
	// opportunity, the same one SendDataPacket gives it, is what lets it
	// recognize the need is satisfied.
	(void)eRequest;
	SendPacketContext_t ctx( usecNow, pszReason );
	ctx.m_cbMaxEncryptedPayload = Connection().m_cbMTUPacketSize - (int)k_cbWireSeqNumHeader;
	m_connection.SNP_SendPacket( this, ctx );
}

void CConnectionTransportP2PWebRTC::TransportPopulateConnectionInfo( SteamNetConnectionInfo_t &info ) const
{
	CConnectionTransport::TransportPopulateConnectionInfo( info );
}

/////////////////////////////////////////////////////////////////////////////
//
// Direct-UDP-over-DataChannel connect (Phase 6 -- see header comment on
// ConnectUDPWebRTCDataChannelInternal for how this differs from the P2P
// transport above)
//
/////////////////////////////////////////////////////////////////////////////

namespace {

// The DataChannel-backed stand-in for a real connected UDP socket. GNS's
// ordinary UDP transport (CConnectionTransportUDP) only ever touches its
// socket through IBoundUDPSocket/IRawUDPSocket, so backing those with a
// DataChannel makes the whole existing handshake/data protocol run over
// it unmodified.
//
// Lifetime/threading: callbacks arrive on libdatachannel's own thread
// (or the browser main thread on wasm), so they take the global lock
// before feeding the transport -- the same contract the real service
// thread gives PacketReceived (which takes the connection lock itself).
// Shared State (not raw `this`) is captured by the callbacks so a
// Close() racing an in-flight callback can never leave it a dangling
// pointer: the callback re-checks m_bClosed under the global lock, and
// Close() flips it under that same lock before deleting the sockets.
struct DataChannelSocketState
{
	std::shared_ptr<rtc::PeerConnection> m_pPeerConnection;
	std::shared_ptr<rtc::DataChannel> m_pDataChannel;
	CRecvPacketCallback m_callback;
	netadr_t m_adrRemote;
	IRawUDPSocket *m_pRawSock = nullptr;
	bool m_bClosed = false;
};

class CRawSocketWebRTCDataChannel final : public IRawUDPSocket
{
public:
	CRawSocketWebRTCDataChannel( std::shared_ptr<DataChannelSocketState> pState )
	: m_pState( std::move( pState ) )
	{
		m_boundAddr.Clear();
		m_pState->m_pRawSock = this;

		std::shared_ptr<DataChannelSocketState> pStateRef = m_pState;
		m_pState->m_pDataChannel->onMessage(
			[pStateRef]( rtc::binary data ) {
				SteamNetworkingGlobalLock lock( "CRawSocketWebRTCDataChannel::onMessage" );
				if ( pStateRef->m_bClosed )
					return;
				RecvPktInfo_t info;
				info.m_pPkt = data.data();
				info.m_cbPkt = (int)data.size();
				info.m_usecNow = SteamNetworkingSockets_GetLocalTimestamp();
				info.m_adrFrom = pStateRef->m_adrRemote;
				info.m_pSock = pStateRef->m_pRawSock;
				pStateRef->m_callback( info );
			},
			[]( std::string ) {
				// Text frames are never sent by either side; ignore.
			}
		);
	}

	virtual bool BSendRawPacketGather( int nChunks, const iovec *pChunks, const netadr_t &adrTo ) const override
	{
		(void)adrTo; // 1:1 channel; the gateway already knows where this goes
		if ( m_pState->m_bClosed || !m_pState->m_pDataChannel->isOpen() )
			return false;

		size_t cbTotal = 0;
		for ( int i = 0 ; i < nChunks ; ++i )
			cbTotal += pChunks[i].iov_len;
		std::vector<std::byte> pkt( cbTotal );
		size_t off = 0;
		for ( int i = 0 ; i < nChunks ; ++i )
		{
			memcpy( pkt.data() + off, pChunks[i].iov_base, pChunks[i].iov_len );
			off += pChunks[i].iov_len;
		}
		return m_pState->m_pDataChannel->send( pkt.data(), pkt.size() );
	}

	virtual void Close() override
	{
		// Global lock is held by the GNS caller (TransportFreeResources).
		m_pState->m_bClosed = true;
		m_pState->m_pRawSock = nullptr;
#ifndef __EMSCRIPTEN__
		// See CConnectionTransportP2PWebRTC::TransportFreeResources for
		// why this is native-only (datachannel-wasm has no
		// resetCallbacks, and wasm is single-threaded anyway).
		m_pState->m_pDataChannel->resetCallbacks();
#endif
		m_pState->m_pDataChannel->close();
		delete this;
	}

private:
	virtual ~CRawSocketWebRTCDataChannel() {}
	std::shared_ptr<DataChannelSocketState> m_pState;
};

class CBoundSocketWebRTCDataChannel final : public IBoundUDPSocket
{
public:
	CBoundSocketWebRTCDataChannel( CRawSocketWebRTCDataChannel *pRawSock, const netadr_t &adr )
	: IBoundUDPSocket( pRawSock, adr ) {}

	virtual void Close() override
	{
		m_pRawSock->Close();
		m_pRawSock = nullptr;
		delete this;
	}
};

struct CreateDataChannelSocketContext
{
	std::shared_ptr<rtc::PeerConnection> m_pPeerConnection;
	std::shared_ptr<rtc::DataChannel> m_pDataChannel;
	netadr_t m_adrRemote;
};

IBoundUDPSocket *CreateDataChannelBoundSocket( CRecvPacketCallback callback, void *pContext )
{
	auto *ctx = static_cast<CreateDataChannelSocketContext *>( pContext );
	auto pState = std::make_shared<DataChannelSocketState>();
	pState->m_pPeerConnection = std::move( ctx->m_pPeerConnection );
	pState->m_pDataChannel = std::move( ctx->m_pDataChannel );
	pState->m_callback = callback;
	pState->m_adrRemote = ctx->m_adrRemote;
	auto *pRawSock = new CRawSocketWebRTCDataChannel( std::move( pState ) );
	return new CBoundSocketWebRTCDataChannel( pRawSock, ctx->m_adrRemote );
}

} // anonymous namespace

HSteamNetConnection ConnectUDPWebRTCDataChannelInternal(
	CSteamNetworkingSockets *pInterface,
	const SteamNetworkingIPAddr &addressRemote,
	std::shared_ptr<rtc::PeerConnection> pPeerConnection,
	std::shared_ptr<rtc::DataChannel> pDataChannel,
	int nOptions, const SteamNetworkingConfigValue_t *pOptions )
{
	if ( !pDataChannel )
	{
		SpewError( "ConnectUDPWebRTCDataChannel: null DataChannel" );
		return k_HSteamNetConnection_Invalid;
	}

	// Mirrors ConnectByIPAddress's creation shape (global lock already
	// held by our API wrapper, matching its scopeLock).
	ConnectionScopeLock connectionLock;
	CSteamNetworkConnectionUDP *pConn = new CSteamNetworkConnectionUDP( pInterface, connectionLock );
	if ( !pConn )
		return k_HSteamNetConnection_Invalid;

	CreateDataChannelSocketContext ctx;
	ctx.m_pPeerConnection = std::move( pPeerConnection );
	ctx.m_pDataChannel = std::move( pDataChannel );
	SteamNetworkingIPAddrToNetAdr( ctx.m_adrRemote, addressRemote );

	SteamDatagramErrMsg errMsg;
	if ( !pConn->BInitConnectBoundSocket( CreateDataChannelBoundSocket, &ctx, nOptions, pOptions, errMsg ) )
	{
		SpewError( "Cannot create UDP-over-DataChannel connection.  %s", errMsg );
		pConn->ConnectionQueueDestroy();
		return k_HSteamNetConnection_Invalid;
	}

	return pConn->m_hConnectionSelf;
}

void CConnectionTransportP2PWebRTC::P2PTransportUpdateRouteMetrics( SteamNetworkingMicroseconds usecNow )
{
	if ( !BCanSendEndToEndData() )
	{
		m_routeMetrics.SetInvalid();
		return;
	}

	// Unlike ICE (which scores candidate pairs against each other and needs
	// real ping data before it can be trusted), this transport's NAT
	// traversal and connectivity were already established by the WebRTC
	// stack before this object existed (see file header comment) -- there
	// is nothing to measure, so the score is a constant rather than a ping.
	//
	// That constant is a deliberate handicap: every byte here is relayed
	// through the gateway, so a direct UDP route is better whenever one
	// exists, and between two native peers one usually does.
	//
	// It has to clear k_nRoutePenaltyNeedToConfirmConnectivity (10000).
	// GNS adds that penalty to a route it has not yet confirmed
	// end-to-end, and ICE cannot confirm without being selected and
	// carrying traffic -- so a handicap below 10000 leaves ICE stuck at
	// ~10000+ping, permanently losing to this transport, which is exactly
	// what was observed: both peers negotiated a working ICE candidate
	// pair and GNS still never switched. Sitting above it means GNS tries
	// the direct route as soon as one exists; if it turns out not to
	// work, ICE's metrics go invalid and selection comes straight back
	// here. When no UDP route exists at all (browser clients, UDP-blocked
	// networks) this transport is the only valid one and wins regardless
	// of the number.
	constexpr int k_nRelayedTransportPenalty = 12000;
	m_routeMetrics.m_nScoreCurrent = 0;
	m_routeMetrics.m_nScoreMin = 0;
	m_routeMetrics.m_nScoreMax = 0;
	m_routeMetrics.m_nTotalPenalty = k_nRelayedTransportPenalty;
	m_routeMetrics.m_nBucketsValid = 1;
	m_bNeedToConfirmEndToEndConnectivity = false;
}

} // namespace SteamNetworkingSocketsLib
