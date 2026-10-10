// mockserver is a manual/scripted stand-in for the native GNS server's
// /server-signal connection — there's no curl/websocat in this
// environment to poke the gateway's WebSocket endpoints by hand, so this
// exists instead. It registers with the gateway exactly like the real
// server will, prints every "gns-rendezvous" message it receives, and
// lets you send one back by typing a line on stdin.
//
// Usage:
//
//	go run ./mockserver -gateway ws://localhost:8088/server-signal
//	<sessionID> <data>   # sends {"type":"gns-rendezvous","sessionId":...,"data":...}
package main

import (
	"bufio"
	"crypto/ed25519"
	"crypto/rand"
	"encoding/base64"
	"encoding/hex"
	"encoding/json"
	"flag"
	"fmt"
	"log"
	"net/url"
	"os"
	"strings"

	"github.com/gorilla/websocket"
)

type signalMessage struct {
	Type             string   `json:"type"`
	SDP              string   `json:"sdp,omitempty"`
	SessionID        string   `json:"sessionId,omitempty"`
	Data             string   `json:"data,omitempty"`
	ServerID         string   `json:"serverId,omitempty"`
	ServerTransports []string `json:"serverTransports,omitempty"`
	Nonce            string   `json:"nonce,omitempty"`
	PublicKey        string   `json:"publicKey,omitempty"`
	Signature        string   `json:"signature,omitempty"`
	HostToken        string   `json:"hostToken,omitempty"`
}

func main() {
	gateway := flag.String("gateway", "ws://localhost:8088/server-signal", "gateway /server-signal WebSocket URL")
	serverID := flag.String("server-id", "mock", "serverId to register under")
	flag.Parse()

	// Registers as WebRTC-hosted: that kind of server is challenged over
	// the websocket alone (no UDP punch to perform by hand), so the
	// registration can actually go active from here.
	pub, priv, err := ed25519.GenerateKey(rand.Reader)
	if err != nil {
		log.Fatalf("keygen: %v", err)
	}

	u, err := url.Parse(*gateway)
	if err != nil {
		log.Fatalf("bad -gateway %q: %v", *gateway, err)
	}

	conn, _, err := websocket.DefaultDialer.Dial(u.String(), nil)
	if err != nil {
		log.Fatalf("dial %s failed: %v", u, err)
	}
	defer conn.Close()
	if err := conn.WriteJSON(signalMessage{
		Type:             "register",
		ServerID:         *serverID,
		ServerTransports: []string{"webrtc"},
		PublicKey:        base64.StdEncoding.EncodeToString(pub),
	}); err != nil {
		log.Fatalf("register: %v", err)
	}
	log.Printf("registering %q with gateway at %s", *serverID, u)

	done := make(chan struct{})
	go func() {
		defer close(done)
		for {
			var msg signalMessage
			if err := conn.ReadJSON(&msg); err != nil {
				log.Printf("read failed, exiting: %v", err)
				return
			}
			switch msg.Type {
			case "register-pending":
				nonce, err := hex.DecodeString(msg.Nonce)
				if err != nil {
					log.Printf("bad nonce in register-pending: %v", err)
					return
				}
				signed := append([]byte("coffee-gateway-register-v1\n"+*serverID+"\n"), nonce...)
				if err := conn.WriteJSON(signalMessage{
					Type:      "challenge-response",
					Nonce:     msg.Nonce,
					Signature: base64.StdEncoding.EncodeToString(ed25519.Sign(priv, signed)),
				}); err != nil {
					log.Printf("challenge-response: %v", err)
					return
				}
			case "register-active":
				log.Printf("registration active (%v)", msg.ServerTransports)
			case "gns-rendezvous":
				fmt.Printf("[recv] session=%s hostToken=%s data=%s\n", msg.SessionID, msg.HostToken, msg.Data)
			case "error":
				log.Printf("gateway error: %s", msg.Data)
			default:
				log.Printf("unexpected message type %q", msg.Type)
			}
		}
	}()

	fmt.Println("type: <sessionID> <data>   (data is sent verbatim as the \"data\" field)")
	scanner := bufio.NewScanner(os.Stdin)
	for scanner.Scan() {
		line := strings.TrimSpace(scanner.Text())
		if line == "" {
			continue
		}
		parts := strings.SplitN(line, " ", 2)
		if len(parts) != 2 {
			fmt.Println("expected: <sessionID> <data>")
			continue
		}
		out, _ := json.Marshal(signalMessage{Type: "gns-rendezvous", SessionID: parts[0], Data: parts[1]})
		if err := conn.WriteMessage(websocket.TextMessage, out); err != nil {
			log.Printf("write failed: %v", err)
			break
		}
	}

	<-done
}
