// selftest drives a running webrtc-gateway end to end without a browser
// or a BlamGraphics build: it plays both roles the gateway sits between —
// a fleet-registering game server (/server-signal: register, punch the
// challenge, answer it, then punch each client's relay port) and a WebRTC
// client (/signal?server=<id>: SDP offer/answer, DataChannel) — and
// asserts a payload survives the round trip client -> gateway -> server
// -> gateway -> client.
//
// Covers the parts no other local test can reach without the wasm/native
// build: registry + return-routability challenge, per-client relay port
// allocation, and the nonce-authenticated relay punch.
//
// Usage: go run ./selftest [-gateway-bin ../gateway]
package main

import (
	"context"
	"encoding/hex"
	"flag"
	"fmt"
	"log"
	"net"
	"os"
	"os/exec"
	"strings"
	"time"

	"github.com/gorilla/websocket"
	"github.com/pion/webrtc/v4"
)

type signalMessage struct {
	Type       string `json:"type"`
	SDP        string `json:"sdp,omitempty"`
	SessionID  string `json:"sessionId,omitempty"`
	Data       string `json:"data,omitempty"`
	ServerID   string `json:"serverId,omitempty"`
	Nonce      string `json:"nonce,omitempty"`
	RelayPort  int    `json:"relayPort,omitempty"`
	RelayNonce string `json:"relayNonce,omitempty"`
	PunchPort  int    `json:"punchPort,omitempty"`
}

const (
	serverID           = "selftest"
	registerPunchPfx   = "COFFEE-REG-PUNCH:"
	payload            = "coffee-selftest-ping"
	relayPunchInterval = 500 * time.Millisecond
)

func main() {
	gatewayBin := flag.String("gateway-bin", "./gateway", "gateway binary to launch")
	httpPort := flag.Int("port", 8399, "gateway HTTP/WS port (also its challenge UDP port)")
	relayMin := flag.Int("relay-port-min", 19700, "gateway -relay-port-min")
	relayMax := flag.Int("relay-port-max", 19710, "gateway -relay-port-max")
	rounds := flag.Int("rounds", 1, "sequential client sessions to run (>1 with a one-port pool proves relay ports are released)")
	timeout := flag.Duration("timeout", 30*time.Second, "overall deadline")
	flag.Parse()

	ctx, cancel := context.WithTimeout(context.Background(), *timeout)
	defer cancel()

	gw := exec.CommandContext(ctx, *gatewayBin,
		"-listen", fmt.Sprintf(":%d", *httpPort),
		"-relay-port-min", fmt.Sprintf("%d", *relayMin),
		"-relay-port-max", fmt.Sprintf("%d", *relayMax),
		// The admin SSH listener would collide across repeat runs and
		// isn't under test here.
		"-admin-port", ":0",
	)
	gw.Stdout, gw.Stderr = os.Stdout, os.Stderr
	if err := gw.Start(); err != nil {
		fatal("failed to start gateway: %v", err)
	}
	defer func() {
		_ = gw.Process.Kill()
		_ = gw.Wait()
	}()

	waitForPort(*httpPort, 10*time.Second)

	gameSock := mustListenUDP()
	defer gameSock.Close()

	go runFleetServer(ctx, *httpPort, gameSock)

	for round := 1; round <= *rounds; round++ {
		received := make(chan string, 1)
		go runClient(ctx, *httpPort, received)
		select {
		case got := <-received:
			if got != strings.ToUpper(payload) {
				fatal("round %d returned %q, want %q", round, got, strings.ToUpper(payload))
			}
			log.Printf("round %d PASS: %q survived client -> gateway -> server -> gateway -> client",
				round, got)
		case <-ctx.Done():
			fatal("round %d timed out before the payload made the round trip", round)
		}
	}
	log.Printf("PASS: %d round(s)", *rounds)
}

// runFleetServer plays the registered game server: registers, proves
// return-routability by punching the challenge socket, then punches each
// client's relay port and echoes relayed datagrams back uppercased.
func runFleetServer(ctx context.Context, httpPort int, gameSock *net.UDPConn) {
	url := fmt.Sprintf("ws://127.0.0.1:%d/server-signal", httpPort)
	conn, _, err := websocket.DefaultDialer.Dial(url, nil)
	if err != nil {
		fatal("server: dial %s: %v", url, err)
	}
	defer conn.Close()

	if err := conn.WriteJSON(signalMessage{Type: "register", ServerID: serverID}); err != nil {
		fatal("server: register: %v", err)
	}

	challengeSock := mustListenUDP()
	defer challengeSock.Close()

	go echoRelayed(gameSock)

	for {
		var m signalMessage
		if err := conn.ReadJSON(&m); err != nil {
			if ctx.Err() == nil {
				log.Printf("server: read: %v", err)
			}
			return
		}
		switch m.Type {
		case "register-pending":
			nonce := doChallenge(challengeSock, m.PunchPort)
			if err := conn.WriteJSON(signalMessage{
				Type:  "challenge-response",
				Nonce: hex.EncodeToString(nonce),
			}); err != nil {
				fatal("server: challenge-response: %v", err)
			}
			log.Printf("server: challenge answered (punch port %d)", m.PunchPort)
		case "client-relay":
			nonce, err := hex.DecodeString(m.RelayNonce)
			if err != nil || len(nonce) == 0 {
				fatal("server: bad relayNonce %q: %v", m.RelayNonce, err)
			}
			log.Printf("server: punching relay port %d for session %s", m.RelayPort, m.SessionID)
			go punchRelay(ctx, gameSock, m.RelayPort, nonce)
		case "client-relay-closed":
			log.Printf("server: relay closed for session %s", m.SessionID)
		case "error":
			fatal("server: gateway rejected us: %s", m.Data)
		}
	}
}

// doChallenge punches the gateway's challenge socket and reads back the
// registration nonce that arrives through the mapping it just opened.
func doChallenge(sock *net.UDPConn, punchPort int) []byte {
	dst := &net.UDPAddr{IP: net.IPv4(127, 0, 0, 1), Port: punchPort}
	buf := make([]byte, 256)
	deadline := time.Now().Add(10 * time.Second)
	for time.Now().Before(deadline) {
		if _, err := sock.WriteToUDP([]byte(registerPunchPfx+serverID), dst); err != nil {
			fatal("server: challenge punch: %v", err)
		}
		sock.SetReadDeadline(time.Now().Add(time.Second))
		n, _, err := sock.ReadFromUDP(buf)
		if err != nil {
			continue // no reply yet, punch again
		}
		nonce := make([]byte, n)
		copy(nonce, buf[:n])
		return nonce
	}
	fatal("server: no challenge nonce came back")
	return nil
}

// punchRelay keeps the NAT mapping toward one client's relay port alive,
// the same way GatewayFleetRegistration::pollRelayKeepalives does.
func punchRelay(ctx context.Context, sock *net.UDPConn, relayPort int, nonce []byte) {
	dst := &net.UDPAddr{IP: net.IPv4(127, 0, 0, 1), Port: relayPort}
	for ctx.Err() == nil {
		if _, err := sock.WriteToUDP(nonce, dst); err != nil {
			return
		}
		time.Sleep(relayPunchInterval)
	}
}

// echoRelayed answers relayed game traffic, uppercased so the client can
// tell a real round trip from its own packet looping back.
func echoRelayed(sock *net.UDPConn) {
	buf := make([]byte, 65535)
	for {
		n, addr, err := sock.ReadFromUDP(buf)
		if err != nil {
			return
		}
		msg := string(buf[:n])
		if msg != payload {
			continue // our own punch nonces echoing around, or noise
		}
		log.Printf("server: got %q from %s, echoing back", msg, addr)
		if _, err := sock.WriteToUDP([]byte(strings.ToUpper(msg)), addr); err != nil {
			return
		}
	}
}

// runClient plays the browser: offer/answer over /signal?server=<id>,
// then send the payload down the DataChannel and report what comes back.
func runClient(ctx context.Context, httpPort int, received chan<- string) {
	url := fmt.Sprintf("ws://127.0.0.1:%d/signal?server=%s", httpPort, serverID)
	var conn *websocket.Conn
	// The registration has to go active before /signal will accept us.
	for deadline := time.Now().Add(15 * time.Second); time.Now().Before(deadline); {
		var err error
		conn, _, err = websocket.DefaultDialer.Dial(url, nil)
		if err == nil {
			break
		}
		time.Sleep(500 * time.Millisecond)
	}
	if conn == nil {
		fatal("client: gateway never accepted /signal (registration not active?)")
	}
	defer conn.Close()

	pc, err := webrtc.NewPeerConnection(webrtc.Configuration{})
	if err != nil {
		fatal("client: new peer connection: %v", err)
	}
	defer pc.Close()

	ordered, retransmits := false, uint16(0)
	dc, err := pc.CreateDataChannel("gns", &webrtc.DataChannelInit{
		Ordered:        &ordered,
		MaxRetransmits: &retransmits,
	})
	if err != nil {
		fatal("client: create data channel: %v", err)
	}
	dc.OnOpen(func() {
		log.Printf("client: data channel open, sending %q", payload)
		for ctx.Err() == nil {
			if err := dc.SendText(payload); err != nil {
				return
			}
			// Retry: the relay only forwards once the server's punch has
			// landed, and anything sent before that is dropped by design.
			time.Sleep(relayPunchInterval)
		}
	})
	done := make(chan struct{})
	dc.OnMessage(func(msg webrtc.DataChannelMessage) {
		select {
		case received <- string(msg.Data):
			close(done)
		default:
		}
	})

	offer, err := pc.CreateOffer(nil)
	if err != nil {
		fatal("client: create offer: %v", err)
	}
	gatherComplete := webrtc.GatheringCompletePromise(pc)
	if err := pc.SetLocalDescription(offer); err != nil {
		fatal("client: set local description: %v", err)
	}
	<-gatherComplete

	if err := conn.WriteJSON(signalMessage{Type: "offer", SDP: pc.LocalDescription().SDP}); err != nil {
		fatal("client: send offer: %v", err)
	}
	var answer signalMessage
	if err := conn.ReadJSON(&answer); err != nil {
		fatal("client: read answer: %v", err)
	}
	if answer.Type != "answer" {
		fatal("client: expected answer, got %q", answer.Type)
	}
	if err := pc.SetRemoteDescription(webrtc.SessionDescription{
		Type: webrtc.SDPTypeAnswer,
		SDP:  answer.SDP,
	}); err != nil {
		fatal("client: set remote description: %v", err)
	}

	// Keep the signaling socket alive until this round is done: the
	// gateway tears the session down when /signal drops.
	go func() {
		for {
			var m signalMessage
			if err := conn.ReadJSON(&m); err != nil {
				return
			}
		}
	}()
	select {
	case <-done:
	case <-ctx.Done():
	}
}

func mustListenUDP() *net.UDPConn {
	sock, err := net.ListenUDP("udp4", &net.UDPAddr{IP: net.IPv4(127, 0, 0, 1)})
	if err != nil {
		fatal("failed to open UDP socket: %v", err)
	}
	return sock
}

func waitForPort(port int, timeout time.Duration) {
	for deadline := time.Now().Add(timeout); time.Now().Before(deadline); {
		conn, err := net.DialTimeout("tcp", fmt.Sprintf("127.0.0.1:%d", port), time.Second)
		if err == nil {
			conn.Close()
			return
		}
		time.Sleep(200 * time.Millisecond)
	}
	fatal("gateway never opened port %d", port)
}

func fatal(format string, args ...any) {
	log.Printf("FAIL: "+format, args...)
	os.Exit(1)
}
