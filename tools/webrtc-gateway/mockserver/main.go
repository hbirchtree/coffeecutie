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
	Type      string `json:"type"`
	SDP       string `json:"sdp,omitempty"`
	SessionID string `json:"sessionId,omitempty"`
	Data      string `json:"data,omitempty"`
}

func main() {
	gateway := flag.String("gateway", "ws://localhost:8088/server-signal", "gateway /server-signal WebSocket URL")
	flag.Parse()

	u, err := url.Parse(*gateway)
	if err != nil {
		log.Fatalf("bad -gateway %q: %v", *gateway, err)
	}

	conn, _, err := websocket.DefaultDialer.Dial(u.String(), nil)
	if err != nil {
		log.Fatalf("dial %s failed: %v", u, err)
	}
	defer conn.Close()
	log.Printf("registered with gateway at %s", u)

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
			case "gns-rendezvous":
				fmt.Printf("[recv] session=%s data=%s\n", msg.SessionID, msg.Data)
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
