//====== Added for coffeecutie: WebRTC-DataChannel-backed P2P transport =====
//
// See examples/blam/cblam-testing/WEBRTC_TRANSPORT.md for the full design
// discussion. Summary: a fifth sibling of CConnectionTransportP2PICE /
// CConnectionTransportP2PSDR, NOT a subclass of either -- deliberately does
// not use GNS's own ICE/rendezvous machinery (STEAMNETWORKINGSOCKETS_ENABLE_ICE
// stays off for builds using this transport). NAT traversal and the
// browser-facing SDP/ICE handshake are handled entirely outside GNS, by
// whatever signaling client constructs this object -- by the time this
// transport exists, the caller already has an *open* rtc::DataChannel
// (libdatachannel natively, datachannel-wasm on the wasm build; same C++
// API either way). This class's whole job is bridging that already-open
// channel to GNS's generic (non-UDP-specific) transport contract.
//
// Deliberately derives CConnectionTransport directly, not
// CConnectionTransportUDPBase: the UDP base class's SendPacket/
// SendPacketGather and UDPSendPacketContext_t/UDPRecvPacketContext_t exist
// to build/parse Valve's own UDPDataMsgHdr wire header (connection ID
// disambiguation for multiple connections sharing one socket, inline stats
// blob). None of that applies here -- this transport is already 1:1 with a
// single DataChannel, so the only framing needed on top of GNS's own
// encrypted chunk is a 2-byte wire sequence number (matches
// UDPDataMsgHdr::m_unSeqNum's width), needed because GNS's AEAD nonce
// construction is sequence-number-based. See .cpp for exact framing.

#pragma once

#include "steamnetworkingsockets_p2p.h"

#include <memory>

namespace rtc {
class PeerConnection;
class DataChannel;
} // namespace rtc

namespace SteamNetworkingSocketsLib {

class CConnectionTransportP2PWebRTC final
: public CConnectionTransport
, public CConnectionTransportP2PBase
{
public:
	// pDataChannel must already be open (onOpen already fired) -- this
	// class does not drive SDP/ICE negotiation itself, see file comment.
	// Holds both shared_ptrs explicitly rather than relying on the
	// DataChannel to keep its parent PeerConnection alive: libdatachannel's
	// public header doesn't document that as guaranteed (checked before
	// deciding this).
	CConnectionTransportP2PWebRTC(
		CSteamNetworkConnectionP2P &connection,
		std::shared_ptr<rtc::PeerConnection> pPeerConnection,
		std::shared_ptr<rtc::DataChannel> pDataChannel
	);
	virtual ~CConnectionTransportP2PWebRTC();

	inline CSteamNetworkConnectionP2P &Connection() const
	{
		return *assert_cast<CSteamNetworkConnectionP2P *>( &m_connection );
	}

	// CConnectionTransport overrides
	virtual bool SendDataPacket( SteamNetworkingMicroseconds usecNow ) override;
	virtual int SendEncryptedDataChunk( const void *pChunk, int cbChunk, SendPacketContext_t &ctx ) override;
	virtual bool BCanSendEndToEndData() const override;
	virtual void SendEndToEndStatsMsg( EStatsReplyRequest eRequest, SteamNetworkingMicroseconds usecNow, const char *pszReason ) override;
	virtual void TransportPopulateConnectionInfo( SteamNetConnectionInfo_t &info ) const override;
	virtual void TransportFreeResources() override;

	// CConnectionTransportP2PBase overrides
	virtual void P2PTransportUpdateRouteMetrics( SteamNetworkingMicroseconds usecNow ) override;

private:
	std::shared_ptr<rtc::PeerConnection> m_pPeerConnection;
	std::shared_ptr<rtc::DataChannel> m_pDataChannel;

	// Next outgoing wire sequence number's source of truth is GNS's own
	// per-connection counter (m_statsEndToEnd.ConsumeSendPacketNumberAndGetWireFmt),
	// same as the UDP transport uses -- not tracked here separately.

	void OnDataChannelMessage( const void *pData, size_t cbData );
	void OnDataChannelClosed();
};

// Added for coffeecutie's Phase 6 direct-UDP-over-DataChannel connect (a
// different mode from the P2P transport above, despite sharing this file
// -- kept here because it's the one port-owned injected source pair, so
// no CMakeLists/portfile changes are needed to add code): implementation
// behind ISteamNetworkingSockets::ConnectUDPWebRTCDataChannel. Creates a
// CSteamNetworkConnectionUDP whose IBoundUDPSocket is backed by the given
// DataChannel (see the .cpp), so the ORDINARY direct-UDP handshake and
// data protocol run over it unchanged against a plain CreateListenSocketIP
// server on the far side of a webrtc-gateway. Caller must hold the global
// lock (the API wrapper in csteamnetworkingsockets.cpp does).
class CSteamNetworkingSockets;
extern HSteamNetConnection ConnectUDPWebRTCDataChannelInternal(
	CSteamNetworkingSockets *pInterface,
	const SteamNetworkingIPAddr &addressRemote,
	std::shared_ptr<rtc::PeerConnection> pPeerConnection,
	std::shared_ptr<rtc::DataChannel> pDataChannel,
	int nOptions, const SteamNetworkingConfigValue_t *pOptions );

} // namespace SteamNetworkingSocketsLib
