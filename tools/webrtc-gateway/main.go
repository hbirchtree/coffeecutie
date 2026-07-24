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
// parses that payload, just as it never parses relayed UDP payloads: each
// registered game server keeps a persistent /server-signal connection,
// each browser client's /signal connection is assigned an opaque session
// ID (returned alongside the SDP answer) and targets one registered
// server via "?server=<id>", and "gns-rendezvous" messages tagged with
// that session ID are routed client<->server by ID alone.
package main

import (
	"bytes"
	"crypto/rand"
	"crypto/subtle"
	"encoding/hex"
	"flag"
	"fmt"
	"log"
	"net"
	"net/http"
	"strconv"
	"strings"
	"sync"
	"sync/atomic"
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

	ServerID  string `json:"serverId,omitempty"`
	Dest      string `json:"dest,omitempty"`
	Nonce     string `json:"nonce,omitempty"`
	RelayPort int    `json:"relayPort,omitempty"`
	PunchPort int `json:"punchPort,omitempty"`
}

const registerPunchPrefix = "COFFEE-REG-PUNCH:"

var relayPunchMarker = []byte("COFFEE-NAT-PUNCH")

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
	serverID string
}

var (
	sessionsMu sync.RWMutex
	sessions   = make(map[string]*clientSession)
)

type registeredServer struct {
	conn    *websocket.Conn
	writeMu sync.Mutex

	legacy bool

	dest *net.UDPAddr

	mu            sync.Mutex
	active        bool
	expiresAt     time.Time
	pendingNonce  []byte
	challengeAddr *net.UDPAddr
}

var (
	registryMu sync.RWMutex
	registry   = make(map[string]*registeredServer)
)

var challengeSock *net.UDPConn

var (
	registrationTTL   time.Duration
	challengeTimeout  time.Duration
	relayPunchTimeout time.Duration
	punchPortToAdvertise int
)

func mustAtoi(s string) int {
	n, err := strconv.Atoi(s)
	if err != nil {
		log.Fatalf("expected a numeric port, got %q: %v", s, err)
	}
	return n
}

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
	dest := flag.String("dest", "", "legacy: hardcoded UDP destination, seeded into the registry as an always-active \"default\" entry (no challenge). Leave unset for a pure fleet-registry deployment.")
	iceUDPPortMin := flag.Int("ice-udp-port-min", 0, "low end of the local UDP port range for ICE candidates (0 = fully ephemeral). Set both -min/-max to a small range for hosting behind a firewall that only opens a fixed set of ports -- avoids the single-port-mux throughput/isolation cost of forcing every connection onto one exact port.")
	iceUDPPortMax := flag.Int("ice-udp-port-max", 0, "high end of the local UDP port range for ICE candidates (0 = fully ephemeral)")
	relayUDPPort := flag.Int("relay-udp-port", 0, "fixed local UDP port for the DataChannel<->UDP relay socket (0 = ephemeral; set this when -dest points at another gateway's relay socket, so it has a fixed port to dial back to)")
	registrationTTLFlag := flag.Duration("registration-ttl", 30*time.Second, "how long a fleet registration stays active without a heartbeat")
	challengeTimeoutFlag := flag.Duration("challenge-timeout", 5*time.Second, "how long a fleet registration waits for its return-routability punch before being dropped")
	relayPunchTimeoutFlag := flag.Duration("relay-punch-timeout", 5*time.Second, "how long a new per-client relay socket waits for the registered server's NAT punch before giving up on that client")
	challengeUDPPort := flag.Int("challenge-udp-port", 0, "UDP port to bind the registration challenge socket on (0 = same port number as -listen)")
	advertisePunchPort := flag.Int("advertise-punch-port", 0, "punch port to tell registering servers (register-pending's punchPort) when the externally reachable UDP port differs from the bound one, e.g. behind a docker port mapping (0 = advertise the bound port)")
	flag.Parse()

	registrationTTL = *registrationTTLFlag
	challengeTimeout = *challengeTimeoutFlag
	relayPunchTimeout = *relayPunchTimeoutFlag

	if (*iceUDPPortMin == 0) != (*iceUDPPortMax == 0) {
		log.Fatalf("-ice-udp-port-min and -ice-udp-port-max must be set together")
	}
	if *iceUDPPortMin != 0 && *iceUDPPortMin > *iceUDPPortMax {
		log.Fatalf("-ice-udp-port-min (%d) must be <= -ice-udp-port-max (%d)", *iceUDPPortMin, *iceUDPPortMax)
	}

	if *dest != "" {
		destAddr, err := net.ResolveUDPAddr("udp", *dest)
		if err != nil {
			log.Fatalf("bad -dest %q: %v", *dest, err)
		}
		registry["default"] = &registeredServer{
			legacy:    true,
			dest:      destAddr,
			active:    true,
			expiresAt: time.Now().Add(365 * 24 * time.Hour),
		}
		log.Printf("legacy -dest %s seeded as always-active registry entry \"default\"", destAddr)
	}

	bindPunchPort := *challengeUDPPort
	if bindPunchPort == 0 {
		_, listenPortStr, err := net.SplitHostPort(*listenAddr)
		if err != nil {
			log.Fatalf("bad -listen %q: %v", *listenAddr, err)
		}
		bindPunchPort = mustAtoi(listenPortStr)
	}
	probeSock, err := net.ListenUDP("udp", &net.UDPAddr{Port: bindPunchPort})
	if err != nil {
		log.Fatalf("failed to open challenge probe socket: %v", err)
	}
	challengeSock = probeSock
	punchPortToAdvertise = *advertisePunchPort
	if punchPortToAdvertise == 0 {
		punchPortToAdvertise = probeSock.LocalAddr().(*net.UDPAddr).Port
	}
	log.Printf("registration challenge socket on udp :%d (advertising punch port %d)",
		probeSock.LocalAddr().(*net.UDPAddr).Port, punchPortToAdvertise)

	go sweepExpiredRegistrations()
	go challengeListener()

	http.HandleFunc("/signal", func(w http.ResponseWriter, r *http.Request) {
		handleSignal(w, r, *iceUDPPortMin, *iceUDPPortMax, relayUDPPort)
	})
	http.HandleFunc("/server-signal", handleServerSignal)

	log.Printf("webrtc-gateway listening on %s", *listenAddr)
	if err := http.ListenAndServe(*listenAddr, nil); err != nil {
		log.Fatal(err)
	}
}

func sweepExpiredRegistrations() {
	ticker := time.NewTicker(5 * time.Second)
	defer ticker.Stop()
	for range ticker.C {
		now := time.Now()
		var expired []*registeredServer
		registryMu.Lock()
		for id, srv := range registry {
			srv.mu.Lock()
			stale := srv.active && now.After(srv.expiresAt)
			srv.mu.Unlock()
			if stale {
				expired = append(expired, srv)
				delete(registry, id)
				log.Printf("registry entry %q expired (no heartbeat within TTL)", id)
			}
		}
		registryMu.Unlock()
		for _, srv := range expired {
			srv.conn.Close()
		}
	}
}

func handleSignal(w http.ResponseWriter, r *http.Request, iceUDPPortMin, iceUDPPortMax int, relayUDPPort *int) {
	serverID := r.URL.Query().Get("server")
	if serverID == "" {
		serverID = "default"
	}
	registryMu.RLock()
	srv, ok := registry[serverID]
	registryMu.RUnlock()
	if !ok {
		log.Printf("rejecting /signal: unknown server %q", serverID)
		return
	}
	srv.mu.Lock()
	active := srv.active
	dest := srv.dest
	srv.mu.Unlock()
	if !active {
		log.Printf("rejecting /signal: server %q not active (challenge pending/failed)", serverID)
		return
	}

	conn, err := upgrader.Upgrade(w, r, nil)
	if err != nil {
		log.Printf("websocket upgrade failed: %v", err)
		return
	}
	defer conn.Close()

	sessionID := newSessionID()
	session := &clientSession{conn: conn, serverID: serverID}
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

	pc, err := newPeerConnection(iceUDPPortMin, iceUDPPortMax)
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
		// dropped.

		if srv.legacy {
			var laddr *net.UDPAddr
			if *relayUDPPort != 0 {
				laddr = &net.UDPAddr{Port: *relayUDPPort}
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
			return
		}

		sock, err := net.ListenUDP("udp", &net.UDPAddr{})
		if err != nil {
			log.Printf("failed to open relay socket: %v", err)
			return
		}
		relayPort := sock.LocalAddr().(*net.UDPAddr).Port

		var resolvedDest atomic.Pointer[net.UDPAddr]
		dc.OnMessage(func(msg webrtc.DataChannelMessage) {
			d := resolvedDest.Load()
			if d == nil {
				return
			}
			if _, err := sock.WriteToUDP(msg.Data, d); err != nil {
				log.Printf("udp write failed: %v", err)
			}
		})
		dc.OnClose(func() {
			log.Printf("data channel %q closed", dc.Label())
			sock.Close()
			srv.writeMu.Lock()
			srv.conn.WriteJSON(signalMessage{Type: "client-relay-closed", SessionID: sessionID})
			srv.writeMu.Unlock()
		})

		srv.writeMu.Lock()
		sendErr := srv.conn.WriteJSON(signalMessage{
			Type:      "client-relay",
			SessionID: sessionID,
			RelayPort: relayPort,
		})
		srv.writeMu.Unlock()
		if sendErr != nil {
			log.Printf("failed to send client-relay to server: %v", sendErr)
			sock.Close()
			return
		}

		dc.OnOpen(func() {
			go func() {
				sock.SetReadDeadline(time.Now().Add(relayPunchTimeout))
				buf := make([]byte, 64)
				_, addr, err := sock.ReadFromUDP(buf)
				sock.SetReadDeadline(time.Time{})
				if err != nil {
					log.Printf(
						"no punch received from server for session %s within %s, closing: %v",
						sessionID, relayPunchTimeout, err)
					sock.Close()
					close(relayDone)
					return
				}
				log.Printf("punch received from %s for session %s, relaying to it", addr, sessionID)
				resolvedDest.Store(addr)
				go runRelayUnconnected(dc, sock, addr, relayDone)
			}()
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
			relayRendezvousToServer(serverID, sessionID, m.Data)
		default:
			log.Printf("session %s: unexpected signal message type %q", sessionID, m.Type)
		}
	}
}

func handleServerSignal(w http.ResponseWriter, r *http.Request) {
	conn, err := upgrader.Upgrade(w, r, nil)
	if err != nil {
		log.Printf("websocket upgrade failed: %v", err)
		return
	}
	defer conn.Close()

	var (
		myID    string
		myEntry *registeredServer
		legacyAdopted bool
	)

	registryMu.Lock()
	if def, ok := registry["default"]; ok && def.conn == nil {
		def.conn = conn
		myID, myEntry, legacyAdopted = "default", def, true
	}
	registryMu.Unlock()
	if legacyAdopted {
		log.Printf("server signaling connection adopted by legacy \"default\" entry")
	}

	defer func() {
		if myEntry == nil {
			return
		}
		registryMu.Lock()
		if legacyAdopted {
			if myEntry.conn == conn {
				myEntry.conn = nil
			}
		} else if registry[myID] == myEntry {
			delete(registry, myID)
		}
		registryMu.Unlock()
		log.Printf("server %q signaling connection closed", myID)
	}()

	for {
		var m signalMessage
		if err := conn.ReadJSON(&m); err != nil {
			return
		}
		switch m.Type {
		case "register":
			id, entry, err := beginRegistration(conn, m)
			if err != nil {
				log.Printf("rejecting registration for %q: %v", m.ServerID, err)
				conn.WriteJSON(signalMessage{Type: "error", Data: err.Error()})
				return
			}
			myID, myEntry = id, entry
			// Tell the server where to send its return-routability punch
			// -- explicitly, never guessed from URLs (see PunchPort's
			// field comment).
			myEntry.writeMu.Lock()
			err = conn.WriteJSON(signalMessage{Type: "register-pending", PunchPort: punchPortToAdvertise})
			myEntry.writeMu.Unlock()
			if err != nil {
				log.Printf("failed to send register-pending to %q: %v", myID, err)
				return
			}
		case "heartbeat":
			if myEntry == nil {
				log.Printf("server signal: heartbeat before a successful register")
				continue
			}
			myEntry.mu.Lock()
			if myEntry.active {
				myEntry.expiresAt = time.Now().Add(registrationTTL)
			}
			myEntry.mu.Unlock()
		case "challenge-response":
			if myEntry == nil {
				log.Printf("server signal: challenge-response before a successful register")
				continue
			}
			completeChallenge(myID, myEntry, m.Nonce)
		case "gns-rendezvous":
			relayRendezvousToClient(m.SessionID, m.Data)
		default:
			log.Printf("server signal: unexpected message type %q", m.Type)
		}
	}
}

func beginRegistration(conn *websocket.Conn, m signalMessage) (string, *registeredServer, error) {
	if m.ServerID == "" {
		return "", nil, fmt.Errorf("register: missing serverId")
	}

	nonce := make([]byte, 16)
	if _, err := rand.Read(nonce); err != nil {
		return "", nil, fmt.Errorf("register: failed to generate challenge nonce: %w", err)
	}

	entry := &registeredServer{
		conn:         conn,
		active:       false,
		pendingNonce: nonce,
		expiresAt:    time.Now().Add(challengeTimeout),
	}

	registryMu.Lock()
	if existing, ok := registry[m.ServerID]; ok && existing.conn != conn {
		registryMu.Unlock()
		return "", nil, fmt.Errorf("serverId %q already registered by another connection", m.ServerID)
	}
	registry[m.ServerID] = entry
	registryMu.Unlock()

	log.Printf("register %q: waiting for return-routability punch", m.ServerID)

	go func(id string, srv *registeredServer) {
		time.Sleep(challengeTimeout)
		srv.mu.Lock()
		expired := !srv.active
		srv.mu.Unlock()
		if !expired {
			return
		}
		registryMu.Lock()
		if registry[id] == srv {
			delete(registry, id)
		}
		registryMu.Unlock()
		log.Printf("register %q: no punch/challenge received in time, dropping registration", id)
	}(m.ServerID, entry)

	return m.ServerID, entry, nil
}

func challengeListener() {
	buf := make([]byte, 256)
	for {
		n, addr, err := challengeSock.ReadFromUDP(buf)
		if err != nil {
			log.Printf("challenge listener read failed: %v", err)
			continue
		}
		msg := string(buf[:n])
		if !strings.HasPrefix(msg, registerPunchPrefix) {
			continue
		}
		id := strings.TrimPrefix(msg, registerPunchPrefix)

		registryMu.RLock()
		srv, ok := registry[id]
		registryMu.RUnlock()
		if !ok {
			continue
		}

		srv.mu.Lock()
		alreadyActive := srv.active
		nonce := srv.pendingNonce
		srv.challengeAddr = addr
		srv.mu.Unlock()
		if alreadyActive {
			continue
		}

		if _, err := challengeSock.WriteToUDP(nonce, addr); err != nil {
			log.Printf("register %q: failed to reply to punch from %s: %v", id, addr, err)
		}
	}
}

func completeChallenge(id string, srv *registeredServer, nonceHex string) {
	nonce, err := hex.DecodeString(nonceHex)
	if err != nil {
		log.Printf("challenge-response for %q: bad hex nonce: %v", id, err)
		return
	}
	srv.mu.Lock()
	defer srv.mu.Unlock()
	if srv.active {
		return
	}
	if subtle.ConstantTimeCompare(nonce, srv.pendingNonce) != 1 {
		log.Printf("challenge-response for %q: nonce mismatch (got %d bytes %x, want %d bytes %x)",
			id, len(nonce), nonce, len(srv.pendingNonce), srv.pendingNonce)
		return
	}
	srv.active = true
	srv.pendingNonce = nil
	srv.expiresAt = time.Now().Add(registrationTTL)
	log.Printf("server %q registration active (challenge passed)", id)
}

func relayRendezvousToServer(serverID, sessionID, data string) {
	registryMu.RLock()
	srv, ok := registry[serverID]
	var conn *websocket.Conn
	if ok {
		// conn can be nil (legacy "default" entry with no /server-signal
		// connection attached) or swapped by legacy adoption -- read it
		// under the same lock that mutates it.
		conn = srv.conn
	}
	registryMu.RUnlock()
	if !ok {
		log.Printf("dropping gns-rendezvous from session %s: server %q no longer registered", sessionID, serverID)
		return
	}
	if conn == nil {
		log.Printf("dropping gns-rendezvous from session %s: server %q has no signaling connection", sessionID, serverID)
		return
	}
	srv.writeMu.Lock()
	defer srv.writeMu.Unlock()
	if err := conn.WriteJSON(signalMessage{Type: "gns-rendezvous", SessionID: sessionID, Data: data}); err != nil {
		log.Printf("failed to relay rendezvous to server %q: %v", serverID, err)
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
func newPeerConnection(udpPortMin, udpPortMax int) (*webrtc.PeerConnection, error) {
	settingEngine := webrtc.SettingEngine{}
	settingEngine.SetLite(true)
	settingEngine.SetNetworkTypes([]webrtc.NetworkType{webrtc.NetworkTypeUDP4})
	if udpPortMin != 0 {
		if err := settingEngine.SetEphemeralUDPPortRange(uint16(udpPortMin), uint16(udpPortMax)); err != nil {
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

func runRelayUnconnected(dc *webrtc.DataChannel, sock *net.UDPConn, dest *net.UDPAddr, done chan<- struct{}) {
	defer close(done)

	buf := make([]byte, 65535)
	for {
		sock.SetReadDeadline(time.Now().Add(30 * time.Second))
		n, addr, err := sock.ReadFromUDP(buf)
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
		if !addr.IP.Equal(dest.IP) || addr.Port != dest.Port {
			continue // stray packet, not from the learned peer
		}
		if n == len(relayPunchMarker) && bytes.Equal(buf[:n], relayPunchMarker) {
			continue // NAT keepalive punch, not game data
		}
		if err := dc.Send(buf[:n]); err != nil {
			log.Printf("data channel send failed: %v", err)
			return
		}
	}
}
