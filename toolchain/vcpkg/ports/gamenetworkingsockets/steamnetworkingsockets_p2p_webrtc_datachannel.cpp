//====== Added for coffeecutie: WebRTC-DataChannel-backed P2P transport =====
// See steamnetworkingsockets_p2p_webrtc_datachannel.h for the design summary.

#include "steamnetworkingsockets_p2p_webrtc_datachannel.h"

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
		m_pDataChannel->resetCallbacks();
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
}

void CConnectionTransportP2PWebRTC::TransportPopulateConnectionInfo( SteamNetConnectionInfo_t &info ) const
{
	CConnectionTransport::TransportPopulateConnectionInfo( info );
}

void CConnectionTransportP2PWebRTC::P2PTransportUpdateRouteMetrics( SteamNetworkingMicroseconds usecNow )
{
	if ( !BCanSendEndToEndData() )
	{
		m_routeMetrics.SetInvalid();
		return;
	}

	// Unlike ICE (which scores multiple candidate pairs against each other
	// and needs real ping data before it can be trusted), this transport's
	// NAT traversal and connectivity were already fully established by the
	// browser's WebRTC stack before this object was even constructed (see
	// file header comment) -- there's nothing else to compare against, so
	// report the best possible score unconditionally rather than waiting
	// on m_pingEndToEnd to accumulate samples. Revisit if this transport
	// ever needs to compete against a simultaneously-available SDR route.
	m_routeMetrics.m_nScoreCurrent = 0;
	m_routeMetrics.m_nScoreMin = 0;
	m_routeMetrics.m_nScoreMax = 0;
	m_routeMetrics.m_nTotalPenalty = 0;
	m_routeMetrics.m_nBucketsValid = 1;
	m_bNeedToConfirmEndToEndConnectivity = false;
}

} // namespace SteamNetworkingSocketsLib
