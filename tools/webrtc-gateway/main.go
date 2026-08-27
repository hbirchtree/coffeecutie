// webrtc-gateway is a WebRTC-terminating relay: it accepts a browser's
// RTCDataChannel connection (ICE/DTLS/SCTP, all real WebRTC) and re-emits
// each DataChannel message as a plain UDP datagram toward a destination
// server, and vice versa. It never touches whatever protocol rides inside
// those datagrams (GameNetworkingSockets, in this project's case) — this
// is a transport bridge, not a game-aware proxy.
//
// One topology only (see examples/blam/cblam-testing/WEBRTC_TRANSPORT.md):
// "browser <-> gateway <-> native UDP server", where every destination
// comes from the fleet registry — a game server registers over
// /server-signal, proves return-routability by punching this gateway's
// challenge socket, and browsers reach it via "/signal?server=<id>".
// There is no hardcoded-destination mode: the gateway never sends to an
// address it hasn't first observed as a packet source.
//
// Each client gets its own relay socket, whose local port is ephemeral by
// default (-relay-port-min/-relay-port-max bound it to a range for hosting
// behind a firewall). Nothing ever dials *into* it — the registered server
// punches it first, and its observed source address becomes the relay
// destination.
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
// With -journal-db set, every lifecycle step above (registration,
// challenge, session, ICE, DataChannel, relay punch, teardown) is also
// written to a SQLite file, tagged with the tracking IDs from the logs,
// and searchable from the admin interface's Search tab. See journal.go.
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
	"encoding/json"
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

	ServerID string `json:"serverId,omitempty"`
	// Transport is "udp" (a real GNS listen socket behind a NAT punch) or
	// "webrtc" (the server is itself a DataChannel peer). Sent by the
	// server on "register", and reported back to each client on "answer"
	// so it knows which GNS connect mode to use.
	Transport  string `json:"transport,omitempty"`
	Nonce      string `json:"nonce,omitempty"`
	RelayPort  int    `json:"relayPort,omitempty"`
	RelayNonce string `json:"relayNonce,omitempty"`
	// MetadataPayload is the server's most recent signed metadata JSON,
	// forwarded verbatim so clients can verify the server's identity
	// independently of the gateway.
	MetadataPayload string `json:"metadata,omitempty"`
	// PunchPort is sent in the "register-pending" reply: the UDP port the
	// registering server must punch its return-routability probe at. Told
	// explicitly rather than guessed from the WS URL, since behind a TLS
	// reverse proxy or docker port mapping the URL's port has nothing to
	// do with the gateway's actual UDP port.
	PunchPort        int    `json:"punchPort,omitempty"`
	TrackingID       string `json:"trackingId,omitempty"`
	ServerTrackingID string `json:"serverTrackingId,omitempty"`
	// Metadata payload from server
	// Contains player count, game type, player count etc.
	// We can use this later to create a server browser
	Metadata map[string]string `json:"metadata,omitempty"`
}

const registerPunchPrefix = "COFFEE-REG-PUNCH:"

const (
	// transportUDP: the server owns a real UDP socket; the gateway relays
	// DataChannel <-> UDP and reaches it through a punched mapping.
	transportUDP = "udp"
	// transportWebRTC: the server is a WebRTC peer itself (browser host,
	// or anywhere UDP is unusable). The gateway bridges the client's
	// DataChannel straight to a second DataChannel the server opens for
	// that session -- no UDP, no punch, no relay port.
	transportWebRTC = "webrtc"
)

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
	writeMu          sync.Mutex
	serverID         string
	relayPort        int
	trackingID       string
	serverTrackingID string

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

	// hostDC is the other half of a DataChannel<->DataChannel bridge: the
	// channel a transportWebRTC server opened for THIS session (see
	// handleHostSignal). Nil for transportUDP servers, and until the host
	// dials in. hostClose tears that side down.
	hostDC    *webrtc.DataChannel
	hostClose func()

	// Bridged message counts, for the admin panel and teardown logging.
	clientToHost atomic.Uint64
	hostToClient atomic.Uint64
}

func (s *registeredServer) tag() string {
	if s == nil {
		return "[S-?]"
	}
	return "[" + s.trackingID + "]"
}

func (s *clientSession) tag() string {
	return "[" + s.serverTrackingID + "/" + s.trackingID + "]"
}

// peerOf returns the far side of a DataChannel bridge for one direction.
func (s *clientSession) peerOf(from *webrtc.DataChannel) *webrtc.DataChannel {
	s.mu.Lock()
	defer s.mu.Unlock()
	if from == s.hostDC {
		return s.dataChannel
	}
	return s.hostDC
}

// bridgeSend forwards one DataChannel message to the paired channel.
// Dropping while the far side isn't open yet is deliberate and safe: it
// mirrors the UDP relay dropping traffic before the server's punch lands,
// and GNS re-sends its handshake until it gets through.
func bridgeSend(tag string, to *webrtc.DataChannel, data []byte) {
	if to == nil {
		log.Printf("%s datachannel bridge: dropping %d bytes, no peer channel yet", tag, len(data))
		return
	}
	if to.ReadyState() != webrtc.DataChannelStateOpen {
		log.Printf("%s datachannel bridge: dropping %d bytes, peer channel is %s",
			tag, len(data), to.ReadyState())
		return
	}
	if err := to.Send(data); err != nil {
		log.Printf("%s datachannel bridge send failed: %v", tag, err)
	}
}

type PortPool struct {
	mu        sync.RWMutex
	minPort   int
	maxPort   int
	usedPorts map[int]struct{}
}

const maxMetadataBytes = 4096

type serverMetadata struct {
	// raw is the JSON payload exactly as the server sent it. The gateway
	// does not interpret the contents beyond enforcing the size cap.
	raw []byte
	// receivedAt is when the payload arrived, for HTTP cache headers /
	// admin display.
	receivedAt time.Time
}

type registeredServer struct {
	conn    *websocket.Conn
	writeMu sync.Mutex

	// transportUDP or transportWebRTC; fixed at registration.
	transport string

	trackingID string

	mu           sync.Mutex
	active       bool
	expiresAt    time.Time
	pendingNonce []byte
	// challengeAddr is where the registration punch came from -- the
	// server's separate challenge socket, not its game socket.
	challengeAddr *net.UDPAddr
	// gameAddr is the most recent address a per-client relay punch came
	// from, i.e. the real GNS listen socket's mapping. Only learned once
	// a client connects, and under symmetric NAT the port differs per
	// client, so this is "last observed", not a stable identity.
	gameAddr *net.UDPAddr
	metadata *serverMetadata
}

type serverSettings struct {
	registrationTTL   time.Duration
	challengeTimeout  time.Duration
	relayPunchTimeout time.Duration
	// punchPortToAdvertise is what "register-pending" tells servers to
	// punch at -- the challenge socket's bound port unless
	// -advertise-punch-port overrides it (docker port mappings etc.).
	punchPortToAdvertise int
	publicIP             string

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

// journal is nil unless -journal-db is set; every Journal method is
// nil-safe, so the call sites don't branch on it.
var journal *Journal

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

func newTrackingID(prefix string) string {
	b := make([]byte, 3)
	if _, err := rand.Read(b); err != nil {
		return prefix + "-" + fmt.Sprintf("%06x", time.Now().UnixNano()&0xffffff)
	}
	return prefix + "-" + strings.ToUpper(hex.EncodeToString(b))
}

func signalOrigin(r *http.Request) string {
	if fwd := r.Header.Get("X-Forwarded-For"); fwd != "" {
		return r.RemoteAddr + " (forwarded for " + fwd + ")"
	}
	return r.RemoteAddr
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
	iceUDPPortMin := flag.Int("ice-udp-port-min", 0, "low end of the local UDP port range for ICE candidates (0 = fully ephemeral). Set both -min/-max to a small range for hosting behind a firewall that only opens a fixed set of ports -- avoids the single-port-mux throughput/isolation cost of forcing every connection onto one exact port.")
	iceUDPPortMax := flag.Int("ice-udp-port-max", 0, "high end of the local UDP port range for ICE candidates (0 = fully ephemeral)")
	relayPortMin := flag.Int("relay-port-min", 0, "low end of the local UDP port range for DataChannel<->UDP relay sockets (0 = fully ephemeral)")
	relayPortMax := flag.Int("relay-port-max", 0, "high end of the local UDP port range for DataChannel<->UDP relay sockets (0 = fully ephemeral)")
	registrationTTLFlag := flag.Duration("registration-ttl", 30*time.Second, "how long a fleet registration stays active without a heartbeat")
	challengeTimeoutFlag := flag.Duration("challenge-timeout", 5*time.Second, "how long a fleet registration waits for its return-routability punch before being dropped")
	relayPunchTimeoutFlag := flag.Duration("relay-punch-timeout", 5*time.Second, "how long a new per-client relay socket waits for the registered server's NAT punch before giving up on that client")
	challengeUDPPort := flag.Int("challenge-udp-port", 0, "UDP port to bind the registration challenge socket on (0 = same port number as -listen)")
	publicIP := flag.String("public-ip", "", "public IP to advertise in ICE candidates (SetNAT1To1IPs). REQUIRED when the gateway runs behind NAT or in a container: ICE-lite gathers host candidates only, so without this it advertises its own private/docker addresses and no outside browser can reach it. Pair with -ice-udp-port-min/-max and publish that range.")
	advertisePunchPort := flag.Int("advertise-punch-port", 0, "punch port to tell registering servers (register-pending's punchPort) when the externally reachable UDP port differs from the bound one, e.g. behind a docker port mapping (0 = advertise the bound port)")

	journalDB := flag.String("journal-db", "", "SQLite file to journal server registrations and client sessions to, searchable from the admin interface's Search tab (empty = no journaling)")
	journalRetention := flag.Duration("journal-retention", 30*24*time.Hour, "how long journal rows are kept (0 = forever)")

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
	settings.publicIP = *publicIP
	if settings.publicIP == "" {
		log.Printf("WARNING: -public-ip not set; ICE candidates will advertise this host's own addresses only. " +
			"Browsers outside this machine's network will fail to connect.")
	} else {
		log.Printf("advertising %s in ICE candidates", settings.publicIP)
	}
	settings.punchPortToAdvertise = *advertisePunchPort
	if settings.punchPortToAdvertise == 0 {
		settings.punchPortToAdvertise = probeSock.LocalAddr().(*net.UDPAddr).Port
	}
	log.Printf("registration challenge socket on udp :%d (advertising punch port %d)",
		probeSock.LocalAddr().(*net.UDPAddr).Port, settings.punchPortToAdvertise)

	if *journalDB != "" {
		j, err := openJournal(*journalDB, *journalRetention)
		if err != nil {
			// Non-fatal: the journal is an observability aid, and losing
			// it must not take a working relay down with it.
			log.Printf("WARNING: failed to open journal %q, continuing without it: %v", *journalDB, err)
		} else {
			journal = j
			defer journal.Close()
			log.Printf("journaling connections to %s (retention %s)", *journalDB, *journalRetention)
		}
	}

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
			journal:    journal,
		},
	)

	http.HandleFunc("/signal", func(w http.ResponseWriter, r *http.Request) {
		handleSignal(w, r, *iceUDPPortMin, *iceUDPPortMax)
	})
	http.HandleFunc("/server-signal", handleServerSignal)

	// Server browsing functions
	http.HandleFunc("/server-list", handleServerList)
	http.HandleFunc("/metadata", handleMetadataQuery)

	log.Printf("webrtc-gateway listening on %s", *listenAddr)
	if err := http.ListenAndServe(*listenAddr, nil); err != nil {
		log.Fatal(err)
	}
}

type ServerListing struct {
	Name string `json:"name"`
	Private bool `json:"private"`
}

type ServerList struct {
	Servers []ServerListing `json:"servers"`
}

func handleServerList(w http.ResponseWriter, r *http.Request) {
	if r.Method != http.MethodGet {
		http.Error(w, "method not allowed", http.StatusMethodNotAllowed)
		return
	}
	servers := ServerList {
		Servers: make([]ServerListing, 0),
	};
	workingSet.servers.RLock()
	for id, srv := range workingSet.servers.registry {
		if !srv.active {
			continue
		}
		servers.Servers = append(servers.Servers, ServerListing{
			Name: id,
			Private: false,
		})
	}
	workingSet.servers.RUnlock()
	serialized, err := json.Marshal(servers)
	if err != nil {
		w.WriteHeader(http.StatusInternalServerError)
		return
	}
	w.Header().Set("Content-Type", "application/json")
	w.Header().Set("Content-Length", strconv.Itoa(len(serialized)))
	w.WriteHeader(http.StatusOK)
	if _, err := w.Write(serialized); err != nil {
		log.Printf("failed to write server list response: %v", err)
	}
}

// handleMetadataQuery returns the most recent metadata payload a registered
// server has published over its /server-signal websocket. Only the server
// may publish metadata; clients read it via this endpoint. The payload is
// capped at maxMetadataBytes on ingest.
func handleMetadataQuery(w http.ResponseWriter, r *http.Request) {
	if r.Method != http.MethodGet {
		w.Header().Set("Content-Type", "application/json")
		http.Error(w, "{\"message\":\"method not allowed\"}", http.StatusMethodNotAllowed)
		return
	}
	serverID := r.URL.Query().Get("server")
	if serverID == "" {
		w.Header().Set("Content-Type", "application/json")
		http.Error(w, "{\"message\":\"missing ?server=<id>\"}", http.StatusBadRequest)
		return
	}
	workingSet.servers.RLock()
	srv, ok := workingSet.servers.registry[serverID]
	workingSet.servers.RUnlock()
	if !ok {
		w.Header().Set("Content-Type", "application/json")
		http.Error(w, "{\"message\":\"unknown server\"}", http.StatusNotFound)
		return
	}

	srv.mu.Lock()
	meta := srv.metadata
	active := srv.active
	srv.mu.Unlock()
	if !active {
		http.Error(w, "{\"message\":\"server not active\"}", http.StatusServiceUnavailable)
		return
	}
	if meta == nil || len(meta.raw) == 0 {
		w.WriteHeader(http.StatusNoContent)
		return
	}

	w.Header().Set("Content-Type", "application/json")
	w.Header().Set("Content-Length", strconv.Itoa(len(meta.raw)))
	w.Header().Set("X-Metadata-Received-At", meta.receivedAt.UTC().Format(time.RFC3339))
	w.WriteHeader(http.StatusOK)
	if _, err := w.Write(meta.raw); err != nil {
		log.Printf("failed to write metadata response for %q: %v", serverID, err)
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
				log.Printf("%s registry entry %q expired (no heartbeat within TTL)", srv.tag(), id)
				journalServerEvent(srv, id, "register-expired", "timeout", "no heartbeat within TTL")
			}
		}
		workingSet.servers.Unlock()
		for _, srv := range expired {
			srv.conn.Close()
		}
	}
}

func handleSignal(w http.ResponseWriter, r *http.Request, iceUDPPortMin, iceUDPPortMax int) {
	// A transportWebRTC server dials the same endpoint to supply the far
	// half of one session's DataChannel bridge (see handleHostSignal).
	if r.URL.Query().Get("role") == "host" {
		handleHostSignal(w, r, r.URL.Query().Get("session"), iceUDPPortMin, iceUDPPortMax)
		return
	}

	serverID := r.URL.Query().Get("server")
	if serverID == "" {
		log.Printf("rejecting /signal: missing ?server=<id>")
		return
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
	srv.mu.Unlock()
	transport := srv.transport
	if !active {
		log.Printf("%s rejecting /signal: server %q not active (challenge pending/failed)", srv.tag(), serverID)
		return
	}

	conn, err := upgrader.Upgrade(w, r, nil)
	if err != nil {
		log.Printf("websocket upgrade failed: %v", err)
		return
	}
	defer conn.Close()

	sessionID := newSessionID()
	session := &clientSession{
		conn:             conn,
		serverID:         serverID,
		protocol:         "WebRTC",
		trackingID:       newTrackingID("C"),
		serverTrackingID: srv.trackingID,
	}
	log.Printf("[%s/%s] client session opened for server %q from %s",
		session.serverTrackingID, session.trackingID, serverID, signalOrigin(r))
	journal.Record(journalEvent{
		scope: "client", event: "session-open", outcome: "ok",
		serverID: serverID, serverTrackingID: session.serverTrackingID,
		clientTrackingID: session.trackingID,
		origin:           signalOrigin(r), transport: transport,
	})
	workingSet.clients.Lock()
	workingSet.clients.sessions[sessionID] = session
	workingSet.clients.Unlock()
	defer func() {
		workingSet.clients.Lock()
		delete(workingSet.clients.sessions, sessionID)
		workingSet.clients.Unlock()
		session.mu.Lock()
		protocol := session.protocol
		session.mu.Unlock()
		journalClientEvent(session, "session-close", "closed", "last protocol "+protocol)
	}()

	var msg signalMessage
	if err := conn.ReadJSON(&msg); err != nil {
		log.Printf("%s signal read failed: %v", session.tag(), err)
		return
	}
	if msg.Type != "offer" {
		log.Printf("%s expected offer, got %q", session.tag(), msg.Type)
		return
	}

	pc, err := newPeerConnection(iceUDPPortMin, iceUDPPortMax)
	if err != nil {
		log.Printf("%s failed to create peer connection: %v", session.tag(), err)
		journalClientEvent(session, "peer-connection", "error", err.Error())
		return
	}
	defer pc.Close()

	pc.OnConnectionStateChange(func(s webrtc.PeerConnectionState) {
		log.Printf("%s peer connection state: %s", session.tag(), s)
		// failed/closed are the states worth an outcome of their own: a
		// journal search for a connection that never worked should land
		// on the row that says so.
		outcome := "pending"
		switch s {
		case webrtc.PeerConnectionStateConnected:
			outcome = "ok"
		case webrtc.PeerConnectionStateFailed, webrtc.PeerConnectionStateDisconnected:
			outcome = "error"
		case webrtc.PeerConnectionStateClosed:
			outcome = "closed"
		}
		journalClientEvent(session, "peer-state", outcome, s.String())
		if s != webrtc.PeerConnectionStateConnected {
			return
		}
		pair, err := pc.SCTP().Transport().ICETransport().GetSelectedCandidatePair()
		if err != nil || pair == nil {
			log.Printf("%s failed to read selected ICE candidate pair: %v", session.tag(), err)
			journalClientEvent(session, "ice-pair", "error", fmt.Sprintf("%v", err))
			return
		}
		journalClientEvent(session, "ice-pair", "ok",
			fmt.Sprintf("local :%d <- remote %s:%d",
				pair.Local.Port, pair.Remote.Address, pair.Remote.Port))
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
		log.Printf("%s data channel %q open request (id=%v)", session.tag(), dc.Label(), dc.ID())
		// dc.ID() is a *uint16 and is nil until the channel is negotiated.
		dcID := "unassigned"
		if id := dc.ID(); id != nil {
			dcID = fmt.Sprintf("%d", *id)
		}
		journalClientEvent(session, "datachannel-request", "pending",
			fmt.Sprintf("label %q id %s", dc.Label(), dcID))

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

		if transport == transportWebRTC {
			// No UDP anywhere in this path: the server dials
			// /signal?role=host&session=<id> with its own DataChannel for
			// this session, and the two get spliced together.
			log.Printf("[%s/%s] awaiting host datachannel (webrtc-hosted server %q)", session.serverTrackingID, session.trackingID, serverID)
			journalClientEvent(session, "await-host-datachannel", "pending", "webrtc-hosted server")
			dc.OnMessage(func(msg webrtc.DataChannelMessage) {
				session.clientToHost.Add(1)
				bridgeSend(session.tag(), session.peerOf(dc), msg.Data)
			})
			var closeOnce sync.Once
			dc.OnClose(func() {
				log.Printf("%s data channel %q closed", session.tag(), dc.Label())
				journalClientEvent(session, "datachannel-close", "closed",
					fmt.Sprintf("bridged %d client->host, %d host->client",
						session.clientToHost.Load(), session.hostToClient.Load()))
				session.mu.Lock()
				hostClose := session.hostClose
				session.mu.Unlock()
				if hostClose != nil {
					hostClose()
				}
				closeOnce.Do(func() { close(relayDone) })
			})
			return
		}

		// Pooled port when -relay-port-min/-max bound the range, else 0 =
		// let the kernel pick. freePort is idempotent: several teardown
		// paths can run (punch timeout, then OnClose), and a second real
		// release could hand a still-live port to another session.
		pooledPort := newPortFromPool(&workingSet.relayPortPool)
		if pooledPort == nil {
			log.Printf("[%s/%s] relay port pool exhausted, refusing session", session.serverTrackingID, session.trackingID)
			journalClientEvent(session, "relay-socket", "error", "relay port pool exhausted")
			return
		}
		var releasePort sync.Once
		freePort := func() {
			releasePort.Do(func() {
				freePortFromPool(*pooledPort, &workingSet.relayPortPool)
			})
		}

		sock, err := net.ListenUDP("udp", &net.UDPAddr{Port: *pooledPort})
		if err != nil {
			log.Printf("%s failed to open relay socket: %v", session.tag(), err)
			journalClientEvent(session, "relay-socket", "error", err.Error())
			freePort()
			return
		}
		relayPort := sock.LocalAddr().(*net.UDPAddr).Port
		session.mu.Lock()
		session.relayPort = relayPort
		session.mu.Unlock()

		relayNonce := make([]byte, 16)
		if _, err := rand.Read(relayNonce); err != nil {
			log.Printf("%s failed to generate relay punch nonce: %v", session.tag(), err)
			freePort()
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
				log.Printf("%s udp write failed: %v", session.tag(), err)
			}
		})
		dc.OnClose(func() {
			log.Printf("%s data channel %q closed", session.tag(), dc.Label())
			journalClientEvent(session, "datachannel-close", "closed",
				fmt.Sprintf("relay port %d", relayPort))
			sock.Close()
			srv.writeMu.Lock()
			srv.conn.WriteJSON(signalMessage{
				Type:       "client-relay-closed",
				SessionID:  sessionID,
				TrackingID: session.trackingID,
			})
			srv.writeMu.Unlock()
			freePort()
		})

		srv.writeMu.Lock()
		sendErr := srv.conn.WriteJSON(signalMessage{
			Type:       "client-relay",
			SessionID:  sessionID,
			TrackingID: session.trackingID,
			RelayPort:  relayPort,
			RelayNonce: hex.EncodeToString(relayNonce),
		})
		srv.writeMu.Unlock()
		if sendErr != nil {
			log.Printf("%s failed to send client-relay to server: %v", session.tag(), sendErr)
			journalClientEvent(session, "client-relay", "error", sendErr.Error())
			freePort()
			sock.Close()
			return
		}

		dc.OnOpen(func() {
			journalClientEvent(session, "datachannel-open", "ok",
				fmt.Sprintf("relay port %d, awaiting server punch", relayPort))
			go func() {
				deadline := time.Now().Add(settings.relayPunchTimeout)
				buf := make([]byte, 64)
				for {
					sock.SetReadDeadline(deadline)
					n, addr, err := sock.ReadFromUDP(buf)
					if err != nil {
						log.Printf(
							"[%s/%s] no authentic punch received from server within %s, closing: %v",
							session.serverTrackingID, session.trackingID, settings.relayPunchTimeout, err)
						journalClientEvent(session, "relay-punch", "timeout",
							"no authentic punch within "+settings.relayPunchTimeout.String())
						sock.Close()
						freePort()
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
					log.Printf("[%s/%s] authentic punch received from %s, relaying to it", session.serverTrackingID, session.trackingID, addr)
					journalClientEvent(session, "relay-punch", "ok",
						fmt.Sprintf("punch from %s -> relay port %d", addr, relayPort))
					resolvedDest.Store(addr)
					session.mu.Lock()
					session.serverAddr = addr
					session.mu.Unlock()
					srv.mu.Lock()
					srv.gameAddr = addr
					srv.mu.Unlock()
					go runRelayUnconnected(session.tag(), dc, sock, addr, relayNonce, relayDone)
					return
				}
			}()
		})
	})

	if err := pc.SetRemoteDescription(webrtc.SessionDescription{
		Type: webrtc.SDPTypeOffer,
		SDP:  msg.SDP,
	}); err != nil {
		log.Printf("%s SetRemoteDescription failed: %v", session.tag(), err)
		return
	}

	answer, err := pc.CreateAnswer(nil)
	if err != nil {
		log.Printf("%s CreateAnswer failed: %v", session.tag(), err)
		return
	}

	gatherComplete := webrtc.GatheringCompletePromise(pc)
	if err := pc.SetLocalDescription(answer); err != nil {
		log.Printf("%s SetLocalDescription failed: %v", session.tag(), err)
		return
	}
	<-gatherComplete

	srv.mu.Lock()
	meta := srv.metadata
	srv.mu.Unlock()
	metadataPayload := ""
	if meta != nil && len(meta.raw) > 0 {
		metadataPayload = string(meta.raw)
	}

	session.writeMu.Lock()
	err = conn.WriteJSON(signalMessage{
		Type:             "answer",
		SDP:              pc.LocalDescription().SDP,
		SessionID:        sessionID,
		TrackingID:       session.trackingID,
		ServerTrackingID: session.serverTrackingID,
		// Which GNS connect mode this client should use: ordinary
		// direct-UDP-over-DataChannel for a UDP server, P2P rendezvous
		// for a WebRTC-hosted one.
		Transport:       transport,
		MetadataPayload: metadataPayload,
	})
	session.writeMu.Unlock()
	if err != nil {
		log.Printf("%s failed to send answer: %v", session.tag(), err)
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
			log.Printf("[%s/%s] GNS reports direct connection, retiring relay", session.serverTrackingID, session.trackingID)
			journalClientEvent(session, "relay-retired", "ok", "GNS reports a direct connection")
			closeSessionRelay(session)
		default:
			log.Printf("%s unexpected signal message type %q", session.tag(), m.Type)
		}
	}
}

// handleHostSignal serves the server half of a DataChannel<->DataChannel
// bridge. A transportWebRTC server has no UDP socket to relay to, so once
// GNS's rendezvous tells it a client wants in, it dials this endpoint --
// /signal?role=host&session=<id> -- and offers a DataChannel of its own
// for that one session. The gateway answers it exactly like a client's,
// then splices the two channels together and copies messages across.
//
// The session ID is the capability here: it is 16 bytes of CSPRNG output
// known only to the gateway, that client, and the server it was routed
// to, and a session accepts exactly one host.
func handleHostSignal(w http.ResponseWriter, r *http.Request, sessionID string, iceUDPPortMin, iceUDPPortMax int) {
	if sessionID == "" {
		log.Printf("rejecting host /signal: missing ?session=<id>")
		return
	}
	workingSet.clients.RLock()
	session, ok := workingSet.clients.sessions[sessionID]
	workingSet.clients.RUnlock()
	if !ok {
		log.Printf("rejecting host /signal: unknown session %s", sessionID)
		return
	}

	workingSet.servers.RLock()
	srv, srvOK := workingSet.servers.registry[session.serverID]
	workingSet.servers.RUnlock()
	if !srvOK || srv.transport != transportWebRTC {
		log.Printf("%s rejecting host /signal: server %q is not webrtc-hosted",
			session.tag(), session.serverID)
		return
	}

	session.mu.Lock()
	taken := session.hostDC != nil || session.hostClose != nil
	session.mu.Unlock()
	if taken {
		log.Printf("%s rejecting host /signal: session already has a host", session.tag())
		journalHostEvent(session, "host-attach", "rejected", "session already has a host")
		return
	}
	journalHostEvent(session, "host-attach", "pending", "from "+signalOrigin(r))

	conn, err := upgrader.Upgrade(w, r, nil)
	if err != nil {
		log.Printf("%s host websocket upgrade failed: %v", session.tag(), err)
		return
	}
	defer conn.Close()

	var msg signalMessage
	if err := conn.ReadJSON(&msg); err != nil {
		log.Printf("%s host signal read failed: %v", session.tag(), err)
		return
	}
	if msg.Type != "offer" {
		log.Printf("%s host signal: expected offer, got %q", session.tag(), msg.Type)
		return
	}

	pc, err := newPeerConnection(iceUDPPortMin, iceUDPPortMax)
	if err != nil {
		log.Printf("%s failed to create host peer connection: %v", session.tag(), err)
		journalHostEvent(session, "peer-connection", "error", err.Error())
		return
	}
	defer pc.Close()

	hostDone := make(chan struct{})
	var hostCloseOnce sync.Once
	closeHost := func() {
		hostCloseOnce.Do(func() { close(hostDone) })
	}
	session.mu.Lock()
	session.hostClose = closeHost
	session.mu.Unlock()
	defer func() {
		session.mu.Lock()
		session.hostDC = nil
		session.hostClose = nil
		session.mu.Unlock()
	}()

	var hostDCOpen atomic.Bool
	// hostDCOpen goes false again on close, so it can't tell "never
	// arrived" from "arrived and finished" -- which is the difference
	// between a failed attach and an ordinary teardown.
	var hostDCEverOpened atomic.Bool
	pc.OnDataChannel(func(dc *webrtc.DataChannel) {
		log.Printf("%s host data channel %q open request", session.tag(), dc.Label())
		hostDCOpen.Store(true)
		hostDCEverOpened.Store(true)
		session.mu.Lock()
		session.hostDC = dc
		session.mu.Unlock()
		// Registered synchronously, for the same reason the client side
		// does it here rather than in OnOpen: Pion drops messages that
		// arrive before a handler exists.
		dc.OnMessage(func(m webrtc.DataChannelMessage) {
			session.hostToClient.Add(1)
			bridgeSend(session.tag(), session.peerOf(dc), m.Data)
		})
		dc.OnClose(func() {
			log.Printf("%s host data channel closed", session.tag())
			journalHostEvent(session, "datachannel-close", "closed", "")
			hostDCOpen.Store(false)
			closeHost()
		})
		dc.OnOpen(func() {
			log.Printf("[%s/%s] datachannel bridge established", session.serverTrackingID, session.trackingID)
			journalHostEvent(session, "bridge-established", "ok", "datachannel bridge established")
		})
	})

	if err := pc.SetRemoteDescription(webrtc.SessionDescription{
		Type: webrtc.SDPTypeOffer,
		SDP:  msg.SDP,
	}); err != nil {
		log.Printf("%s host SetRemoteDescription failed: %v", session.tag(), err)
		return
	}
	answer, err := pc.CreateAnswer(nil)
	if err != nil {
		log.Printf("%s host CreateAnswer failed: %v", session.tag(), err)
		return
	}
	gatherComplete := webrtc.GatheringCompletePromise(pc)
	if err := pc.SetLocalDescription(answer); err != nil {
		log.Printf("%s host SetLocalDescription failed: %v", session.tag(), err)
		return
	}
	<-gatherComplete

	if err := conn.WriteJSON(signalMessage{
		Type:      "answer",
		SDP:       pc.LocalDescription().SDP,
		SessionID: sessionID,
	}); err != nil {
		log.Printf("%s failed to send host answer: %v", session.tag(), err)
		return
	}

	// Nothing else flows over this socket -- the server's gns-rendezvous
	// goes over /server-signal, not here -- so the server is free to drop
	// it as soon as the SDP exchange is done, and does. The bridge's
	// lifetime is the DataChannel's, NOT this socket's; only treat the
	// socket closing as a teardown if no DataChannel ever arrived, which
	// is the "host gave up mid-handshake" case.
	go func() {
		for {
			var m signalMessage
			if err := conn.ReadJSON(&m); err != nil {
				if !hostDCOpen.Load() {
					if !hostDCEverOpened.Load() {
						log.Printf("%s host signaling closed before its datachannel opened", session.tag())
						journalHostEvent(session, "host-attach", "error", "signaling closed before the datachannel opened")
					}
					closeHost()
				}
				return
			}
			log.Printf("%s unexpected host signal message %q", session.tag(), m.Type)
		}
	}()
	<-hostDone
	log.Printf("%s host side torn down (bridged %d client->host, %d host->client messages)",
		session.tag(), session.clientToHost.Load(), session.hostToClient.Load())
	journalHostEvent(session, "host-close", "closed",
		fmt.Sprintf("bridged %d client->host, %d host->client",
			session.clientToHost.Load(), session.hostToClient.Load()))
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
	)

	defer func() {
		if myEntry == nil {
			return
		}
		workingSet.servers.Lock()
		if workingSet.servers.registry[myID] == myEntry {
			delete(workingSet.servers.registry, myID)
		}
		workingSet.servers.Unlock()
		log.Printf("%s server %q signaling connection closed", myEntry.tag(), myID)
		journalServerEvent(myEntry, myID, "signaling-closed", "closed", "")
	}()

	for {
		var m signalMessage
		if err := conn.ReadJSON(&m); err != nil {
			return
		}
		switch m.Type {
		case "register":
			id, entry, err := beginRegistration(conn, m, signalOrigin(r))
			if err != nil {
				log.Printf("rejecting registration for %q: %v", m.ServerID, err)
				// No entry exists yet, so this row has no tracking ID to
				// attribute -- searchable by serverID only.
				journalServerEvent(nil, m.ServerID, "register", "rejected", err.Error())
				conn.WriteJSON(signalMessage{Type: "error", Data: err.Error()})
				return
			}
			myID, myEntry = id, entry
			reply := signalMessage{
				Type:             "register-pending",
				PunchPort:        settings.punchPortToAdvertise,
				ServerTrackingID: entry.trackingID,
			}
			if entry.transport == transportWebRTC {
				// Nothing to punch -- it is routable the moment it
				// registers (see beginRegistration).
				reply = signalMessage{
					Type:             "register-active",
					Transport:        transportWebRTC,
					ServerTrackingID: entry.trackingID,
				}
			}
			// Tell a UDP server where to send its return-routability punch
			// -- explicitly, never guessed from URLs (see PunchPort's
			// field comment).
			myEntry.writeMu.Lock()
			err = conn.WriteJSON(reply)
			myEntry.writeMu.Unlock()
			if err != nil {
				log.Printf("%s failed to send register-pending to %q: %v", myEntry.tag(), myID, err)
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
			// m.Metadata is kept for backward compatibility with small
			// key/value maps. If a raw payload is present it takes
			// precedence and is subject to the size cap.
			if len(m.Data) > 0 {
				stashServerMetadata(myID, myEntry, []byte(m.Data))
			} else if len(m.Metadata) > 0 {
				encoded, err := json.Marshal(m.Metadata)
				if err != nil {
					log.Printf("%s failed to encode metadata map: %v", myEntry.tag(), err)
					continue
				}
				stashServerMetadata(myID, myEntry, encoded)
			}
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

func beginRegistration(conn *websocket.Conn, m signalMessage, origin string) (string, *registeredServer, error) {
	if m.ServerID == "" {
		return "", nil, fmt.Errorf("register: missing serverId")
	}
	transport := m.Transport
	if transport == "" {
		transport = transportUDP
	}
	if transport != transportUDP && transport != transportWebRTC {
		return "", nil, fmt.Errorf("register: unknown transport %q", transport)
	}

	nonce := make([]byte, 16)
	if _, err := rand.Read(nonce); err != nil {
		return "", nil, fmt.Errorf("register: failed to generate challenge nonce: %w", err)
	}

	entry := &registeredServer{
		conn:         conn,
		transport:    transport,
		trackingID:   newTrackingID("S"),
		active:       false,
		pendingNonce: nonce,
		expiresAt:    time.Now().Add(settings.challengeTimeout),
	}
	// A WebRTC-hosted server has no UDP presence to prove: the gateway
	// only ever reaches it back down this same WebSocket, and the
	// DataChannel it dials in with is itself ICE/DTLS-authenticated. The
	// return-routability challenge would be checking an address that is
	// never used, so it is skipped -- first-registered-wins below is the
	// whole squatting story for these.
	if transport == transportWebRTC {
		entry.active = true
		entry.pendingNonce = nil
		entry.expiresAt = time.Now().Add(settings.registrationTTL)
	}

	workingSet.servers.Lock()
	if existing, ok := workingSet.servers.registry[m.ServerID]; ok && existing.conn != conn {
		workingSet.servers.Unlock()
		return "", nil, fmt.Errorf("serverId %q already registered by another connection", m.ServerID)
	}
	workingSet.servers.registry[m.ServerID] = entry
	workingSet.servers.Unlock()

	if transport == transportWebRTC {
		log.Printf("[%s] server %q registration active from %s (webrtc-hosted, no UDP challenge)",
			entry.trackingID, m.ServerID, origin)
		journal.Record(journalEvent{
			scope: "server", event: "register", outcome: "ok",
			serverID: m.ServerID, serverTrackingID: entry.trackingID,
			origin: origin, transport: entry.transport,
			detail: "webrtc-hosted, no UDP challenge",
		})
		return m.ServerID, entry, nil
	}

	log.Printf("[%s] register %q from %s: waiting for return-routability punch",
		entry.trackingID, m.ServerID, origin)
	journal.Record(journalEvent{
		scope: "server", event: "register", outcome: "pending",
		serverID: m.ServerID, serverTrackingID: entry.trackingID,
		origin: origin, transport: entry.transport,
		detail: "awaiting return-routability punch",
	})

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
		log.Printf("%s register %q: no punch/challenge received in time, dropping registration", srv.tag(), id)
		journalServerEvent(srv, id, "register", "timeout",
			"no punch/challenge within "+settings.challengeTimeout.String())
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

		journalServerEvent(srv, id, "registration-punch", "ok", "punch from "+addr.String())
		if _, err := challengeSock.WriteToUDP(nonce, addr); err != nil {
			log.Printf("%s register %q: failed to reply to punch from %s: %v", srv.tag(), id, addr, err)
			journalServerEvent(srv, id, "registration-punch", "error", err.Error())
		}
	}
}

func completeChallenge(id string, srv *registeredServer, nonceHex string) {
	nonce, err := hex.DecodeString(nonceHex)
	if err != nil {
		log.Printf("%s challenge-response for %q: bad hex nonce: %v", srv.tag(), id, err)
		journalServerEvent(srv, id, "challenge-response", "error", "bad hex nonce: "+err.Error())
		return
	}
	srv.mu.Lock()
	defer srv.mu.Unlock()
	if srv.active {
		return
	}
	if subtle.ConstantTimeCompare(nonce, srv.pendingNonce) != 1 {
		log.Printf("%s challenge-response for %q: nonce mismatch (got %d bytes %x, want %d bytes %x)",
			srv.tag(), id, len(nonce), nonce, len(srv.pendingNonce), srv.pendingNonce)
		journalServerEvent(srv, id, "challenge-response", "error", "nonce mismatch")
		return
	}
	srv.active = true
	srv.pendingNonce = nil
	srv.expiresAt = time.Now().Add(settings.registrationTTL)
	log.Printf("%s server %q registration active (challenge passed)", srv.tag(), id)
	challengeAddr := ""
	if srv.challengeAddr != nil {
		challengeAddr = "challenge passed from " + srv.challengeAddr.String()
	}
	journalServerEvent(srv, id, "register-active", "ok", challengeAddr)
}

func stashServerMetadata(id string, srv *registeredServer, raw []byte) {
	if len(raw) > maxMetadataBytes {
		log.Printf("%s server metadata from %s rejected: %d bytes exceeds %d byte cap",
			srv.tag(), id, len(raw), maxMetadataBytes)
		return
	}
	log.Printf("%s server metadata received from %s (%d bytes)", srv.tag(), id, len(raw))
	srv.mu.Lock()
	srv.metadata = &serverMetadata{
		raw:        raw,
		receivedAt: time.Now(),
	}
	srv.mu.Unlock()
}

func relayRendezvousToServer(serverID, sessionID, data string) {
	workingSet.servers.RLock()
	srv, ok := workingSet.servers.registry[serverID]
	var conn *websocket.Conn
	if ok {
		// Read conn under the same lock that mutates it.
		conn = srv.conn
	}
	workingSet.servers.RUnlock()
	if !ok {
		log.Printf("dropping gns-rendezvous from session %s: server %q no longer registered", sessionID, serverID)
		return
	}
	if conn == nil {
		log.Printf("%s dropping gns-rendezvous: server %q has no signaling connection", srv.tag(), serverID)
		return
	}
	srv.writeMu.Lock()
	defer srv.writeMu.Unlock()
	if err := conn.WriteJSON(signalMessage{Type: "gns-rendezvous", SessionID: sessionID, Data: data}); err != nil {
		log.Printf("%s failed to relay rendezvous to server %q: %v", srv.tag(), serverID, err)
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
	// Senders only signal this once GNS has switched to a transport that
	// ISN'T the relayed DataChannel (see GatewayServerRegistration's
	// PollPendingAccepts), so the relay really is redundant by now.
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
	if settings.publicIP != "" {
		settingEngine.SetNAT1To1IPs([]string{settings.publicIP}, webrtc.ICECandidateTypeHost)
	}
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

// runRelayUnconnected pumps the UDP->DataChannel direction for an open
// channel; the DataChannel->UDP direction is already wired via
// dc.OnMessage at registration time (see OnDataChannel above —
// deliberately not here, to avoid the message-drop race). dc.Send
// delivers one DataChannel message per call, matching one UDP datagram
// per call here — GNS's own datagrams are already sized well under
// DataChannel's practical per-message limits, so no extra
// fragmentation/reassembly belongs in this relay. The socket is
// unconnected: dest is the punch-learned server address, and anything
// from another source is dropped.
//
// punchNonce is this session's relay nonce: the server keeps re-punching
// with it for the whole connection (NAT keepalive), so it keeps arriving
// on this socket long after the relay started and must never be
// forwarded as game data. Dropping it also stops a stale keepalive from
// an earlier session bleeding into this one when a pooled relay port is
// recycled.
func runRelayUnconnected(tag string, dc *webrtc.DataChannel, sock *net.UDPConn, dest *net.UDPAddr, punchNonce []byte, done chan<- struct{}) {
	defer close(done)

	buf := make([]byte, 65535)
	for {
		sock.SetReadDeadline(time.Now().Add(30 * time.Second))
		n, addr, err := sock.ReadFromUDP(buf)
		if err != nil {
			if netErr, ok := err.(net.Error); ok && netErr.Timeout() {
				if dc.ReadyState() != webrtc.DataChannelStateOpen {
					log.Printf("%s data channel no longer open, ending relay", tag)
					return
				}
				continue
			}
			log.Printf("%s udp read failed, ending relay: %v", tag, err)
			return
		}
		if !addr.IP.Equal(dest.IP) || addr.Port != dest.Port {
			continue // stray packet, not from the learned peer
		}
		if bytes.Equal(buf[:n], punchNonce) ||
			(n == len(relayPunchMarker) && bytes.Equal(buf[:n], relayPunchMarker)) {
			continue // NAT keepalive punch, not game data
		}
		if err := dc.Send(buf[:n]); err != nil {
			log.Printf("%s data channel send failed: %v", tag, err)
			return
		}
	}
}
