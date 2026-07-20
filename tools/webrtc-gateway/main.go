// webrtc-gateway is a WebRTC-terminating relay: it accepts a browser's
// RTCDataChannel connection (ICE/DTLS/SCTP, all real WebRTC) and re-emits
// each DataChannel message as a plain UDP datagram toward a destination
// server, and vice versa. It never touches whatever protocol rides inside
// those datagrams (GameNetworkingSockets, in this project's case) — this
// is a transport bridge, not a game-aware proxy.
//
// Phase 1 (see examples/blam/cblam-testing/WEBRTC_TRANSPORT.md): single
// hardcoded destination, no multi-tenancy, no poke/registry API yet. Just
// proves the relay mechanic end-to-end.
//
// Signaling is a minimal non-trickle-ICE WebSocket protocol: the client
// posts a complete offer (after its own ICE gathering finishes), the
// gateway answers once its own (ICE-lite) gathering finishes. No separate
// candidate messages in this phase.
package main

import (
	"flag"
	"log"
	"net"
	"net/http"
	"time"

	"github.com/gorilla/websocket"
	"github.com/pion/webrtc/v4"
)

type signalMessage struct {
	Type string `json:"type"`
	SDP  string `json:"sdp"`
}

var upgrader = websocket.Upgrader{
	// Phase 1 test harness only; a real deployment must restrict this.
	CheckOrigin: func(r *http.Request) bool { return true },
}

func main() {
	listenAddr := flag.String("listen", ":8088", "HTTP/WebSocket signaling listen address")
	dest := flag.String("dest", "127.0.0.1:9999", "hardcoded UDP destination to relay DataChannel payloads to")
	iceUDPPort := flag.Int("ice-udp-port", 0, "fixed local UDP port for the gateway's own ICE-lite candidate (0 = ephemeral)")
	flag.Parse()

	destAddr, err := net.ResolveUDPAddr("udp", *dest)
	if err != nil {
		log.Fatalf("bad -dest %q: %v", *dest, err)
	}

	http.HandleFunc("/signal", func(w http.ResponseWriter, r *http.Request) {
		handleSignal(w, r, destAddr, *iceUDPPort)
	})

	log.Printf("webrtc-gateway listening on %s, relaying to %s", *listenAddr, destAddr)
	if err := http.ListenAndServe(*listenAddr, nil); err != nil {
		log.Fatal(err)
	}
}

func handleSignal(w http.ResponseWriter, r *http.Request, dest *net.UDPAddr, iceUDPPort int) {
	conn, err := upgrader.Upgrade(w, r, nil)
	if err != nil {
		log.Printf("websocket upgrade failed: %v", err)
		return
	}
	defer conn.Close()

	var msg signalMessage
	if err := conn.ReadJSON(&msg); err != nil {
		log.Printf("signal read failed: %v", err)
		return
	}
	if msg.Type != "offer" {
		log.Printf("expected offer, got %q", msg.Type)
		return
	}

	pc, err := newPeerConnection(iceUDPPort)
	if err != nil {
		log.Printf("failed to create peer connection: %v", err)
		return
	}
	defer pc.Close()

	pc.OnConnectionStateChange(func(s webrtc.PeerConnectionState) {
		log.Printf("peer connection state: %s", s)
	})

	relayDone := make(chan struct{})
	pc.OnDataChannel(func(dc *webrtc.DataChannel) {
		log.Printf("data channel %q open request (id=%v)", dc.Label(), dc.ID())

		// dc.OnMessage must be registered synchronously here, not inside
		// the OnOpen callback (let alone a goroutine spawned from it):
		// the remote peer's own "open" can fire, and it can send,
		// before a handler registered asynchronously on this side gets
		// attached — Pion doesn't buffer messages for a callback that
		// isn't registered yet, so the first message(s) were silently
		// dropped. UDP dial is a local, non-blocking operation (no
		// network round trip), so doing it here too costs nothing.
		sock, err := net.DialUDP("udp", nil, dest)
		if err != nil {
			log.Printf("failed to dial UDP dest %s: %v", dest, err)
			return
		}
		dc.OnMessage(func(msg webrtc.DataChannelMessage) {
			if _, err := sock.Write(msg.Data); err != nil {
				log.Printf("udp write failed: %v", err)
			}
		})
		dc.OnClose(func() {
			log.Printf("data channel %q closed", dc.Label())
			sock.Close()
		})
		dc.OnOpen(func() {
			log.Printf("data channel %q open, relaying to %s", dc.Label(), dest)
			go runRelay(dc, sock, relayDone)
		})
	})

	if err := pc.SetRemoteDescription(webrtc.SessionDescription{
		Type: webrtc.SDPTypeOffer,
		SDP:  msg.SDP,
	}); err != nil {
		log.Printf("SetRemoteDescription failed: %v", err)
		return
	}

	answer, err := pc.CreateAnswer(nil)
	if err != nil {
		log.Printf("CreateAnswer failed: %v", err)
		return
	}

	gatherComplete := webrtc.GatheringCompletePromise(pc)
	if err := pc.SetLocalDescription(answer); err != nil {
		log.Printf("SetLocalDescription failed: %v", err)
		return
	}
	<-gatherComplete

	if err := conn.WriteJSON(signalMessage{
		Type: "answer",
		SDP:  pc.LocalDescription().SDP,
	}); err != nil {
		log.Printf("failed to send answer: %v", err)
		return
	}

	// Signaling's job is done once the answer is sent (non-trickle ICE,
	// no further messages expected in this phase) — but keep the
	// WebSocket open as a simple liveness signal for the connection;
	// closing it doesn't tear down the PeerConnection/DataChannel, only
	// ends this handler once the relay itself finishes.
	select {
	case <-relayDone:
	case <-connClosed(conn):
	}
}

// newPeerConnection builds an ICE-lite PeerConnection: the gateway has a
// stable, reachable address, so it never needs to gather srflx/relay
// candidates itself — the browser peer does full ICE against it. This is
// the standard "media/data server" ICE pattern, and specifically avoids
// needing any TURN infrastructure for this topology (see
// WEBRTC_TRANSPORT.md's "NAT traversal simplifies too" note).
func newPeerConnection(fixedUDPPort int) (*webrtc.PeerConnection, error) {
	settingEngine := webrtc.SettingEngine{}
	settingEngine.SetLite(true)
	settingEngine.SetNetworkTypes([]webrtc.NetworkType{webrtc.NetworkTypeUDP4})
	if fixedUDPPort != 0 {
		if err := settingEngine.SetEphemeralUDPPortRange(uint16(fixedUDPPort), uint16(fixedUDPPort)); err != nil {
			return nil, err
		}
	}

	api := webrtc.NewAPI(webrtc.WithSettingEngine(settingEngine))
	return api.NewPeerConnection(webrtc.Configuration{
		// ICE-lite: no STUN/TURN servers needed on this side.
		ICEServers: nil,
	})
}

// runRelay pumps the UDP->DataChannel direction for an open channel; the
// DataChannel->UDP direction is already wired via dc.OnMessage at
// registration time (see OnDataChannel above — deliberately not here, to
// avoid the message-drop race). dc.Send delivers one DataChannel message
// per call, matching one UDP datagram per call here — GNS's own
// datagrams (the eventual payload once GNS is layered on top, later
// phases) are already sized well under DataChannel's practical
// per-message limits, so no extra fragmentation/reassembly belongs in
// this relay.
func runRelay(dc *webrtc.DataChannel, sock *net.UDPConn, done chan<- struct{}) {
	defer close(done)

	buf := make([]byte, 65535)
	for {
		sock.SetReadDeadline(time.Now().Add(30 * time.Second))
		n, err := sock.Read(buf)
		if err != nil {
			if netErr, ok := err.(net.Error); ok && netErr.Timeout() {
				if dc.ReadyState() != webrtc.DataChannelStateOpen {
					log.Printf("data channel no longer open, ending relay")
					return
				}
				continue
			}
			log.Printf("udp read failed, ending relay: %v", err)
			return
		}
		if err := dc.Send(buf[:n]); err != nil {
			log.Printf("data channel send failed: %v", err)
			return
		}
	}
}

func connClosed(conn *websocket.Conn) <-chan struct{} {
	ch := make(chan struct{})
	go func() {
		defer close(ch)
		for {
			if _, _, err := conn.NextReader(); err != nil {
				return
			}
		}
	}()
	return ch
}
