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
// The relay socket's local port is ephemeral by default, matching a plain
// "browser <-> gateway <-> native UDP server" deployment, where nothing
// ever needs to dial *into* it. -relay-udp-port pins it to a fixed port
// instead, for the case where the "destination" is itself another
// gateway's relay socket (two WebRTC-only peers bridged through a pair of
// gateways — e.g. this project's own native-to-native transport testing,
// see WEBRTC_TRANSPORT.md's Phase 2 section) — that only works if each
// side has a fixed, known port for the other to dial.
//
// Signaling is a minimal non-trickle-ICE WebSocket protocol: the client
// posts a complete offer (after its own ICE gathering finishes), the
// gateway answers once its own (ICE-lite) gathering finishes. No separate
// candidate messages in this phase.
//
// The gateway also relays GameNetworkingSockets' own rendezvous signaling
// (the ConnectRequest/ConnectOK handshake GNS always does through its
// ISteamNetworkingConnectionSignaling, independent of transport state —
// see WEBRTC_TRANSPORT.md's "GNS-level signaling" section). It never
// parses that payload, just as it never parses relayed UDP payloads: a
// single game server registers once over /server-signal, each browser
// client's /signal connection is assigned an opaque session ID (returned
// alongside the SDP answer), and "gns-rendezvous" messages tagged with
// that ID are routed client<->server by ID alone.
package main

import (
	"crypto/rand"
	"encoding/hex"
	"flag"
	"fmt"
	"log"
	"net"
	"net/http"
	"sync"
	"time"

	"github.com/gorilla/websocket"
	"github.com/pion/webrtc/v4"
)

type signalMessage struct {
	Type string `json:"type"`
	SDP  string `json:"sdp,omitempty"`

	// SessionID identifies a /signal client for gns-rendezvous routing.
	// Gateway-assigned, returned to the client on its "answer" message.
	SessionID string `json:"sessionId,omitempty"`
	// Data is an opaque (base64, by convention on the C++ side) blob for
	// "gns-rendezvous" messages — the gateway never inspects it.
	Data string `json:"data,omitempty"`
}

var upgrader = websocket.Upgrader{
	// Phase 1 test harness only; a real deployment must restrict this.
	CheckOrigin: func(r *http.Request) bool { return true },
}

// clientSession is a registered /signal connection, keyed by session ID
// so a server's gns-rendezvous reply can be routed back to the right
// browser client without the gateway understanding GNS's protocol.
type clientSession struct {
	conn *websocket.Conn
	// gorilla/websocket connections support one concurrent writer; the
	// initial answer and later server->client relay writes can race
	// without this.
	writeMu sync.Mutex
}

var (
	sessionsMu sync.RWMutex
	sessions   = make(map[string]*clientSession)
)

// serverConnection is the single game server registered over
// /server-signal. Phase-minimal: one destination, no auth, no
// multi-server routing — a pulled-forward slice of the full Phase 6
// registry, not that registry itself.
type serverConnection struct {
	conn    *websocket.Conn
	writeMu sync.Mutex
}

var (
	serverConnMu sync.RWMutex
	activeServer *serverConnection
)

func newSessionID() string {
	b := make([]byte, 16)
	if _, err := rand.Read(b); err != nil {
		// OS CSPRNG failure is not expected in practice; fall back to
		// something still unique enough for a dev-scale single-process
		// gateway rather than refusing the connection outright.
		return fmt.Sprintf("%x", time.Now().UnixNano())
	}
	return hex.EncodeToString(b)
}

func main() {
	listenAddr := flag.String("listen", ":8088", "HTTP/WebSocket signaling listen address")
	dest := flag.String("dest", "127.0.0.1:9999", "hardcoded UDP destination to relay DataChannel payloads to")
	iceUDPPort := flag.Int("ice-udp-port", 0, "fixed local UDP port for the gateway's own ICE-lite candidate (0 = ephemeral)")
	relayUDPPort := flag.Int("relay-udp-port", 0, "fixed local UDP port for the DataChannel<->UDP relay socket (0 = ephemeral; set this when -dest points at another gateway's relay socket, so it has a fixed port to dial back to)")
	flag.Parse()

	destAddr, err := net.ResolveUDPAddr("udp", *dest)
	if err != nil {
		log.Fatalf("bad -dest %q: %v", *dest, err)
	}

	http.HandleFunc("/signal", func(w http.ResponseWriter, r *http.Request) {
		handleSignal(w, r, destAddr, *iceUDPPort, *relayUDPPort)
	})
	http.HandleFunc("/server-signal", handleServerSignal)

	log.Printf("webrtc-gateway listening on %s, relaying to %s", *listenAddr, destAddr)
	if err := http.ListenAndServe(*listenAddr, nil); err != nil {
		log.Fatal(err)
	}
}

func handleSignal(w http.ResponseWriter, r *http.Request, dest *net.UDPAddr, iceUDPPort int, relayUDPPort int) {
	conn, err := upgrader.Upgrade(w, r, nil)
	if err != nil {
		log.Printf("websocket upgrade failed: %v", err)
		return
	}
	defer conn.Close()

	sessionID := newSessionID()
	session := &clientSession{conn: conn}
	sessionsMu.Lock()
	sessions[sessionID] = session
	sessionsMu.Unlock()
	defer func() {
		sessionsMu.Lock()
		delete(sessions, sessionID)
		sessionsMu.Unlock()
	}()

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
		//
		// laddr is nil (ephemeral) unless -relay-udp-port pinned it. A
		// fixed local port only makes sense for one relay at a time (a
		// second concurrent /signal connection would fail to bind the
		// same port) — fine given this gateway is already single-tenant
		// (see the package doc comment); would need per-session ports
		// from a real Phase 6 registry to lift that.
		var laddr *net.UDPAddr
		if relayUDPPort != 0 {
			laddr = &net.UDPAddr{Port: relayUDPPort}
		}
		sock, err := net.DialUDP("udp", laddr, dest)
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

	session.writeMu.Lock()
	err = conn.WriteJSON(signalMessage{
		Type:      "answer",
		SDP:       pc.LocalDescription().SDP,
		SessionID: sessionID,
	})
	session.writeMu.Unlock()
	if err != nil {
		log.Printf("failed to send answer: %v", err)
		return
	}

	// SDP/ICE exchange is done (non-trickle ICE, no candidate messages in
	// this phase), but the WebSocket stays open and keeps being read:
	// GNS's own rendezvous signaling (ConnectRequest/ConnectOK, etc.)
	// still needs to flow over it, tagged with this session's ID, for as
	// long as the DataChannel relay is alive. Force this loop to end once
	// the relay does, since nothing more needs relaying past that point.
	go func() {
		<-relayDone
		conn.Close()
	}()
	for {
		var m signalMessage
		if err := conn.ReadJSON(&m); err != nil {
			return
		}
		switch m.Type {
		case "gns-rendezvous":
			relayRendezvousToServer(sessionID, m.Data)
		default:
			log.Printf("session %s: unexpected signal message type %q", sessionID, m.Type)
		}
	}
}

// handleServerSignal is the game server's counterpart to /signal: a long-
// lived WebSocket the native server process registers once, at startup,
// used to carry GNS rendezvous signaling for every client session the
// gateway is bridging. Phase-minimal — one server, no auth, no
// re-registration/failover handling — a slice of Phase 6's registry
// pulled forward only as far as this signaling relay needs it.
func handleServerSignal(w http.ResponseWriter, r *http.Request) {
	conn, err := upgrader.Upgrade(w, r, nil)
	if err != nil {
		log.Printf("websocket upgrade failed: %v", err)
		return
	}
	defer conn.Close()

	serverConnMu.Lock()
	if activeServer != nil {
		serverConnMu.Unlock()
		log.Printf("rejecting /server-signal: a server is already registered")
		conn.WriteJSON(signalMessage{Type: "error", Data: "server already registered"})
		return
	}
	server := &serverConnection{conn: conn}
	activeServer = server
	serverConnMu.Unlock()
	log.Printf("server registered for GNS rendezvous signaling")

	defer func() {
		serverConnMu.Lock()
		if activeServer == server {
			activeServer = nil
		}
		serverConnMu.Unlock()
		log.Printf("server signaling connection closed")
	}()

	for {
		var m signalMessage
		if err := conn.ReadJSON(&m); err != nil {
			return
		}
		if m.Type != "gns-rendezvous" {
			log.Printf("server signal: unexpected message type %q", m.Type)
			continue
		}
		relayRendezvousToClient(m.SessionID, m.Data)
	}
}

func relayRendezvousToServer(sessionID, data string) {
	serverConnMu.RLock()
	server := activeServer
	serverConnMu.RUnlock()
	if server == nil {
		log.Printf("dropping gns-rendezvous from session %s: no server registered", sessionID)
		return
	}
	server.writeMu.Lock()
	defer server.writeMu.Unlock()
	if err := server.conn.WriteJSON(signalMessage{Type: "gns-rendezvous", SessionID: sessionID, Data: data}); err != nil {
		log.Printf("failed to relay rendezvous to server: %v", err)
	}
}

func relayRendezvousToClient(sessionID, data string) {
	sessionsMu.RLock()
	session, ok := sessions[sessionID]
	sessionsMu.RUnlock()
	if !ok {
		log.Printf("dropping gns-rendezvous for unknown session %s", sessionID)
		return
	}
	session.writeMu.Lock()
	defer session.writeMu.Unlock()
	if err := session.conn.WriteJSON(signalMessage{Type: "gns-rendezvous", SessionID: sessionID, Data: data}); err != nil {
		log.Printf("failed to relay rendezvous to client %s: %v", sessionID, err)
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
