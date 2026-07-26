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
//
// Either side may follow up with a "gns-connected" message (same session
// ID) once GNS's own rendezvous handshake lands a direct P2P connection
// between client and server: the WebRTC relay was only standing in until
// that happened, so the gateway retires it (DataChannel close, relay
// socket/port freed, signaling websocket closed) rather than keep
// relaying traffic GNS no longer needs it for.
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

	ServerID   string `json:"serverId,omitempty"`
	Dest       string `json:"dest,omitempty"`
	Nonce      string `json:"nonce,omitempty"`
	RelayPort  int    `json:"relayPort,omitempty"`
	RelayNonce string `json:"relayNonce,omitempty"`
	// PunchPort is sent in the "register-pending" reply: the UDP port the
	// registering server must punch its return-routability probe at. Told
	// explicitly rather than guessed from the WS URL, since behind a TLS
	// reverse proxy or docker port mapping the URL's port has nothing to
	// do with the gateway's actual UDP port.
	PunchPort int `json:"punchPort,omitempty"`
	// Metadata payload from server
	// Contains player count, game type, player count etc.
	// We can use this later to create a server browser
	Metadata map[string]string `json:"metadata,omitempty"`
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
	writeMu   sync.Mutex
	serverID  string
	relayPort int

	mu sync.Mutex
	// serverAddr is the resolved UDP address this session's relay socket
	// is talking to. Nil until the server's NAT punch arrives.
	serverAddr *net.UDPAddr
	// peerLocalPort/peerRemoteAddr are the selected ICE candidate pair
	// for the browser<->gateway WebRTC leg itself -- distinct from both
	// serverAddr (gateway<->game-server UDP leg) and conn.RemoteAddr()
	// (the signaling websocket's TCP address, not the media path). Zero
	// value / nil until ICE has connected and picked a pair.
	peerLocalPort  int
	peerRemoteAddr *net.UDPAddr
	// dataChannel is the relay DataChannel, set once OnDataChannel fires.
	// Closing it tears down the relay (see each branch's OnClose) -- this
	// is how a "gns-connected" signal (see closeSessionRelay) retires a
	// session once GNS's own direct P2P connection has made the relay
	// redundant.
	dataChannel *webrtc.DataChannel
	// protocol is "webrtc" (the default -- traffic still flows through
	// the relayed DataChannel) or "udp" once closeSessionRelay has
	// retired the relay in favor of GNS's own direct P2P connection.
	// Surfaced in the admin panel's Protocol column.
	protocol string
}

type PortPool struct {
	mu        sync.RWMutex
	minPort   int
	maxPort   int
	usedPorts map[int]struct{}
}

type serverMetadata struct {
	playerCountCurrent int
	playerCountMax     int
	timeLeft           int
	// gametype string
}

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
	metadata      *serverMetadata
}

type serverSettings struct {
	registrationTTL   time.Duration
	challengeTimeout  time.Duration
	relayPunchTimeout time.Duration
	// punchPortToAdvertise is what "register-pending" tells servers to
	// punch at -- the challenge socket's bound port unless
	// -advertise-punch-port overrides it (docker port mappings etc.).
	punchPortToAdvertise int

	websocketAddr   net.Addr
	challengeAddr   net.Addr
	datachannelAddr net.Addr
}

type serverRegistry struct {
	sync.RWMutex
	registry map[string]*registeredServer
}

type clientSessionRegistry struct {
	sync.RWMutex
	sessions map[string]*clientSession
}

type serverWorkingSet struct {
	servers       serverRegistry
	clients       clientSessionRegistry
	relayPortPool PortPool
}

var challengeSock *net.UDPConn

var settings serverSettings = serverSettings{}

var workingSet serverWorkingSet = serverWorkingSet{
	servers: serverRegistry{
		registry: make(map[string]*registeredServer),
	},
	clients: clientSessionRegistry{
		sessions: make(map[string]*clientSession),
	},
	relayPortPool: PortPool{
		minPort:   0,
		maxPort:   0,
		usedPorts: make(map[int]struct{}),
	},
}

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

func newPortFromPool(portPool *PortPool) *int {
	portPool.mu.Lock()
	defer portPool.mu.Unlock()
	if portPool.minPort == 0 {
		p := 0
		return &p
	}
	for p := portPool.minPort; p <= portPool.maxPort; p++ {
		_, exists := portPool.usedPorts[p]
		if !exists {
			portPool.usedPorts[p] = struct{}{}
			return &p
		}
	}
	return nil
}

func freePortFromPool(port int, portPool *PortPool) {
	portPool.mu.Lock()
	defer portPool.mu.Unlock()
	delete(portPool.usedPorts, port)
}

func main() {
	listenAddr := flag.String("listen", ":8088", "HTTP/WebSocket signaling listen address")
	dest := flag.String("dest", "", "legacy: hardcoded UDP destination, seeded into the registry as an always-active \"default\" entry (no challenge). Leave unset for a pure fleet-registry deployment.")
	iceUDPPortMin := flag.Int("ice-udp-port-min", 0, "low end of the local UDP port range for ICE candidates (0 = fully ephemeral). Set both -min/-max to a small range for hosting behind a firewall that only opens a fixed set of ports -- avoids the single-port-mux throughput/isolation cost of forcing every connection onto one exact port.")
	iceUDPPortMax := flag.Int("ice-udp-port-max", 0, "high end of the local UDP port range for ICE candidates (0 = fully ephemeral)")
	relayPortMin := flag.Int("relay-port-min", 0, "low end of the local UDP port range for DataChannel<->UDP relay sockets (0 = fully ephemeral)")
	relayPortMax := flag.Int("relay-port-max", 0, "high end of the local UDP port range for DataChannel<->UDP relay sockets (0 = fully ephemeral)")
	relayUDPPort := flag.Int("relay-udp-port", 0, "fixed local UDP port for the DataChannel<->UDP relay socket (0 = ephemeral; set this when -dest points at another gateway's relay socket, so it has a fixed port to dial back to)")
	registrationTTLFlag := flag.Duration("registration-ttl", 30*time.Second, "how long a fleet registration stays active without a heartbeat")
	challengeTimeoutFlag := flag.Duration("challenge-timeout", 5*time.Second, "how long a fleet registration waits for its return-routability punch before being dropped")
	relayPunchTimeoutFlag := flag.Duration("relay-punch-timeout", 5*time.Second, "how long a new per-client relay socket waits for the registered server's NAT punch before giving up on that client")
	challengeUDPPort := flag.Int("challenge-udp-port", 0, "UDP port to bind the registration challenge socket on (0 = same port number as -listen)")
	advertisePunchPort := flag.Int("advertise-punch-port", 0, "punch port to tell registering servers (register-pending's punchPort) when the externally reachable UDP port differs from the bound one, e.g. behind a docker port mapping (0 = advertise the bound port)")

	adminAddr := flag.String("admin-port", ":2222", "SSH port for admin interface")
	adminPrivateKey := flag.String("admin-private-key", "id_ed25519", "Private key to use as host key for admin server")
	flag.Parse()

	settings.registrationTTL = *registrationTTLFlag
	settings.challengeTimeout = *challengeTimeoutFlag
	settings.relayPunchTimeout = *relayPunchTimeoutFlag

	if (*iceUDPPortMin == 0) != (*iceUDPPortMax == 0) {
		log.Fatalf("-ice-udp-port-min and -ice-udp-port-max must be set together")
	}
	if *iceUDPPortMin != 0 && *iceUDPPortMin > *iceUDPPortMax {
		log.Fatalf("-ice-udp-port-min (%d) must be <= -ice-udp-port-max (%d)", *iceUDPPortMin, *iceUDPPortMax)
	}
	if (*relayPortMin == 0) != (*relayPortMax == 0) {
		log.Fatalf("-relay-port-min and -relay-port-max must be set together")
	}
	if *relayPortMin != 0 && *relayPortMin > *relayPortMax {
		log.Fatalf("-relay-port-min (%d) must be <= -relay-port-max (%d)", *relayPortMin, *relayPortMax)
	}

	if *dest != "" {
		destAddr, err := net.ResolveUDPAddr("udp", *dest)
		if err != nil {
			log.Fatalf("bad -dest %q: %v", *dest, err)
		}
		workingSet.servers.registry["default"] = &registeredServer{
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
	settings.challengeAddr = probeSock.LocalAddr()
	challengeSock = probeSock
	settings.punchPortToAdvertise = *advertisePunchPort
	if settings.punchPortToAdvertise == 0 {
		settings.punchPortToAdvertise = probeSock.LocalAddr().(*net.UDPAddr).Port
	}
	log.Printf("registration challenge socket on udp :%d (advertising punch port %d)",
		probeSock.LocalAddr().(*net.UDPAddr).Port, settings.punchPortToAdvertise)

	workingSet.relayPortPool.minPort = *relayPortMin
	workingSet.relayPortPool.maxPort = *relayPortMax

	go sweepExpiredRegistrations()
	go challengeListener()
	adminHost, adminPort, err := net.SplitHostPort(*adminAddr)
	go startAdminInterface(
		adminInterfaceOpts{
			host:       adminHost,
			port:       adminPort,
			privateKey: *adminPrivateKey,
		},
		adminInterfaceModel{
			workingSet: &workingSet,
			settings:   &settings,
		},
	)

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
		workingSet.servers.Lock()
		for id, srv := range workingSet.servers.registry {
			srv.mu.Lock()
			stale := srv.active && now.After(srv.expiresAt)
			srv.mu.Unlock()
			if stale {
				expired = append(expired, srv)
				delete(workingSet.servers.registry, id)
				log.Printf("registry entry %q expired (no heartbeat within TTL)", id)
			}
		}
		workingSet.servers.Unlock()
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
	workingSet.servers.RLock()
	srv, ok := workingSet.servers.registry[serverID]
	workingSet.servers.RUnlock()
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
	session := &clientSession{conn: conn, serverID: serverID, protocol: "WebRTC"}
	workingSet.clients.Lock()
	workingSet.clients.sessions[sessionID] = session
	workingSet.clients.Unlock()
	defer func() {
		workingSet.clients.Lock()
		delete(workingSet.clients.sessions, sessionID)
		workingSet.clients.Unlock()
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
		if s != webrtc.PeerConnectionStateConnected {
			return
		}
		pair, err := pc.SCTP().Transport().ICETransport().GetSelectedCandidatePair()
		if err != nil || pair == nil {
			log.Printf("failed to read selected ICE candidate pair for session %s: %v", sessionID, err)
			return
		}
		session.mu.Lock()
		session.peerLocalPort = int(pair.Local.Port)
		session.peerRemoteAddr = &net.UDPAddr{
			IP:   net.ParseIP(pair.Remote.Address),
			Port: int(pair.Remote.Port),
		}
		session.mu.Unlock()
	})

	relayDone := make(chan struct{})
	pc.OnDataChannel(func(dc *webrtc.DataChannel) {
		log.Printf("data channel %q open request (id=%v)", dc.Label(), dc.ID())

		session.mu.Lock()
		session.dataChannel = dc
		session.mu.Unlock()

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

		relayPort := newPortFromPool(&workingSet.relayPortPool)
		if relayPort == nil {
			log.Printf("failed to allocate relay port for incoming datachannel")
			return
		}
		var laddr *net.UDPAddr = &net.UDPAddr{Port: *relayPort}
		sock, err := net.ListenUDP("udp", laddr)
		if err != nil {
			log.Printf("failed to open relay socket: %v", err)
			freePortFromPool(*relayPort, &workingSet.relayPortPool)
			return
		}
		if *relayPort == 0 {
			relayPort = &sock.LocalAddr().(*net.UDPAddr).Port
		}
		session.relayPort = *relayPort

		relayNonce := make([]byte, 16)
		if _, err := rand.Read(relayNonce); err != nil {
			log.Printf("failed to generate relay punch nonce: %v", err)
			freePortFromPool(*relayPort, &workingSet.relayPortPool)
			sock.Close()
			return
		}

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
			freePortFromPool(*relayPort, &workingSet.relayPortPool)
			srv.writeMu.Unlock()
		})

		srv.writeMu.Lock()
		sendErr := srv.conn.WriteJSON(signalMessage{
			Type:       "client-relay",
			SessionID:  sessionID,
			RelayPort:  *relayPort,
			RelayNonce: hex.EncodeToString(relayNonce),
		})
		srv.writeMu.Unlock()
		if sendErr != nil {
			log.Printf("failed to send client-relay to server: %v", sendErr)
			freePortFromPool(*relayPort, &workingSet.relayPortPool)
			sock.Close()
			return
		}

		dc.OnOpen(func() {
			go func() {
				deadline := time.Now().Add(settings.relayPunchTimeout)
				buf := make([]byte, 64)
				for {
					sock.SetReadDeadline(deadline)
					n, addr, err := sock.ReadFromUDP(buf)
					if err != nil {
						log.Printf(
							"no authentic punch received from server for session %s within %s, closing: %v",
							sessionID, settings.relayPunchTimeout, err)
						sock.Close()
						close(relayDone)
						return
					}
					// Only trust a punch carrying this session's exact
					// nonce -- otherwise whichever UDP packet reaches this
					// ephemeral port first (from anyone who guesses it)
					// would get adopted as "the server."
					if !bytes.Equal(buf[:n], relayNonce) {
						continue
					}
					sock.SetReadDeadline(time.Time{})
					log.Printf("authentic punch received from %s for session %s, relaying to it", addr, sessionID)
					resolvedDest.Store(addr)
					session.mu.Lock()
					session.serverAddr = addr
					session.mu.Unlock()
					go runRelayUnconnected(dc, sock, addr, relayDone)
					return
				}
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
	// the relay does -- unless it ended because closeSessionRelay retired
	// it in favor of GNS's own direct P2P connection (session.protocol ==
	// "udp"), in which case the session and its signaling socket stay up:
	// nothing more needs relaying, but the admin panel still wants to
	// show this session (now tagged "udp") until it actually disconnects.
	go func() {
		<-relayDone
		session.mu.Lock()
		retiredForDirectP2P := session.protocol == "UDP"
		session.mu.Unlock()
		if !retiredForDirectP2P {
			conn.Close()
		}
	}()
	for {
		var m signalMessage
		if err := conn.ReadJSON(&m); err != nil {
			return
		}
		switch m.Type {
		case "gns-rendezvous":
			relayRendezvousToServer(serverID, sessionID, m.Data)
		case "gns-connected":
			log.Printf("session %s: GNS reports direct connection, retiring relay", sessionID)
			closeSessionRelay(session)
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
		myID          string
		myEntry       *registeredServer
		legacyAdopted bool
	)

	workingSet.servers.Lock()
	if def, ok := workingSet.servers.registry["default"]; ok && def.conn == nil {
		def.conn = conn
		myID, myEntry, legacyAdopted = "default", def, true
	}
	workingSet.servers.Unlock()
	if legacyAdopted {
		log.Printf("server signaling connection adopted by legacy \"default\" entry")
	}

	defer func() {
		if myEntry == nil {
			return
		}
		workingSet.servers.Lock()
		if legacyAdopted {
			if myEntry.conn == conn {
				myEntry.conn = nil
			}
		} else if workingSet.servers.registry[myID] == myEntry {
			delete(workingSet.servers.registry, myID)
		}
		workingSet.servers.Unlock()
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
			if legacyAdopted {
				// This connection provisionally claimed the legacy
				// "default" entry before we knew it was about to send a
				// real registration. Undo that claim -- otherwise the
				// disconnect cleanup below still thinks myEntry is
				// "default" and, instead of deleting the real entry from
				// the registry, just nils its conn, leaving a stale
				// active entry for sweepExpiredRegistrations to later
				// Close() a nil conn on.
				workingSet.servers.Lock()
				if def, ok := workingSet.servers.registry["default"]; ok && def.conn == conn {
					def.conn = nil
				}
				workingSet.servers.Unlock()
				legacyAdopted = false
			}
			myID, myEntry = id, entry
			// Tell the server where to send its return-routability punch
			// -- explicitly, never guessed from URLs (see PunchPort's
			// field comment).
			myEntry.writeMu.Lock()
			err = conn.WriteJSON(signalMessage{Type: "register-pending", PunchPort: settings.punchPortToAdvertise})
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
				myEntry.expiresAt = time.Now().Add(settings.registrationTTL)
			}
			myEntry.mu.Unlock()
		case "challenge-response":
			if myEntry == nil {
				log.Printf("server signal: challenge-response before a successful register")
				continue
			}
			completeChallenge(myID, myEntry, m.Nonce)
		case "metadata":
			stashServerMetadata(myID, myEntry, m.Metadata)
		case "gns-rendezvous":
			relayRendezvousToClient(m.SessionID, m.Data)
		case "gns-connected":
			log.Printf("server signal: GNS reports direct connection for session %s, retiring relay", m.SessionID)
			relayGNSConnected(m.SessionID)
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
		expiresAt:    time.Now().Add(settings.challengeTimeout),
	}

	workingSet.servers.Lock()
	if existing, ok := workingSet.servers.registry[m.ServerID]; ok && existing.conn != conn {
		workingSet.servers.Unlock()
		return "", nil, fmt.Errorf("serverId %q already registered by another connection", m.ServerID)
	}
	workingSet.servers.registry[m.ServerID] = entry
	workingSet.servers.Unlock()

	log.Printf("register %q: waiting for return-routability punch", m.ServerID)

	go func(id string, srv *registeredServer) {
		time.Sleep(settings.challengeTimeout)
		srv.mu.Lock()
		expired := !srv.active
		srv.mu.Unlock()
		if !expired {
			return
		}
		workingSet.servers.Lock()
		if workingSet.servers.registry[id] == srv {
			delete(workingSet.servers.registry, id)
		}
		workingSet.servers.Unlock()
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

		workingSet.servers.RLock()
		srv, ok := workingSet.servers.registry[id]
		workingSet.servers.RUnlock()
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
	srv.expiresAt = time.Now().Add(settings.registrationTTL)
	log.Printf("server %q registration active (challenge passed)", id)
}

func mapToInt(data map[string]string, key string) int {
	value, ok := data[key]
	if !ok {
		return 0
	}
	parsed, err := strconv.Atoi(value)
	if err != nil {
		return 0
	}
	return parsed
}

func stashServerMetadata(id string, srv *registeredServer, metadata map[string]string) {
	log.Printf("server metadata received from %s", id)
	var parsedMeta = serverMetadata{}
	parsedMeta.playerCountCurrent = mapToInt(metadata, "playerCount")
	parsedMeta.playerCountMax = mapToInt(metadata, "playerCountMax")
	parsedMeta.timeLeft = mapToInt(metadata, "timeLeft")
	srv.metadata = &parsedMeta
}

func relayRendezvousToServer(serverID, sessionID, data string) {
	workingSet.servers.RLock()
	srv, ok := workingSet.servers.registry[serverID]
	var conn *websocket.Conn
	if ok {
		// conn can be nil (legacy "default" entry with no /server-signal
		// connection attached) or swapped by legacy adoption -- read it
		// under the same lock that mutates it.
		conn = srv.conn
	}
	workingSet.servers.RUnlock()
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
	workingSet.clients.RLock()
	session, ok := workingSet.clients.sessions[sessionID]
	workingSet.clients.RUnlock()
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

// closeSessionRelay tears down a session's WebRTC relay because GNS's own
// rendezvous handshake (relayed opaquely above) has completed a direct
// P2P connection between client and server, making the relay redundant.
// Closing the DataChannel cascades through each branch's existing
// OnClose (socket close, port release, server notification). The
// session itself (and its signaling websocket) stays registered --
// tagged protocol "udp" -- so the admin panel keeps showing it rather
// than the session just vanishing (see handleSignal's relayDone
// goroutine, which checks this flag before closing the socket).
func closeSessionRelay(session *clientSession) {
	session.mu.Lock()
	dc := session.dataChannel
	session.protocol = "UDP"
	session.mu.Unlock()
	if dc == nil {
		return
	}
	if err := dc.Close(); err != nil {
		log.Printf("failed to close relay data channel: %v", err)
	}
}

// relayGNSConnected looks up a session by ID for closeSessionRelay, for
// callers (the server's /server-signal connection) that don't already
// have the session in scope the way handleSignal's own read loop does.
func relayGNSConnected(sessionID string) {
	workingSet.clients.RLock()
	session, ok := workingSet.clients.sessions[sessionID]
	workingSet.clients.RUnlock()
	if !ok {
		log.Printf("dropping gns-connected for unknown session %s", sessionID)
		return
	}
	closeSessionRelay(session)
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
