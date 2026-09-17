package main

import (
	"encoding/json"
	"io"
	"net/http"
	"net/http/httptest"
	"strconv"
	"strings"
	"testing"
	"time"
)

func TestMetadataQueryRoundTrip(t *testing.T) {
	// Seed a registered server with metadata.
	workingSet.servers.Lock()
	srv := &registeredServer{
		active:     true,
		trackingID: "S-TEST01",
		metadata: &serverMetadata{
			raw:        []byte(`{"map":"bloodgulch","player_count":4}`),
			receivedAt: time.Now(),
		},
	}
	workingSet.servers.registry["test-server"] = srv
	workingSet.servers.Unlock()
	t.Cleanup(func() {
		workingSet.servers.Lock()
		delete(workingSet.servers.registry, "test-server")
		workingSet.servers.Unlock()
	})

	req := httptest.NewRequest(http.MethodGet, "/metadata?server=test-server", nil)
	rec := httptest.NewRecorder()
	handleMetadataQuery(rec, req)

	if rec.Code != http.StatusOK {
		t.Fatalf("expected 200, got %d: %s", rec.Code, rec.Body.String())
	}
	if ct := rec.Header().Get("Content-Type"); ct != "application/json" {
		t.Fatalf("expected application/json, got %q", ct)
	}
	body, _ := io.ReadAll(rec.Body)
	if !strings.Contains(string(body), `"map":"bloodgulch"`) {
		t.Fatalf("unexpected metadata body: %s", body)
	}
}

func TestMetadataQueryMissingServer(t *testing.T) {
	req := httptest.NewRequest(http.MethodGet, "/metadata?server=does-not-exist", nil)
	rec := httptest.NewRecorder()
	handleMetadataQuery(rec, req)

	if rec.Code != http.StatusNotFound {
		t.Fatalf("expected 404, got %d", rec.Code)
	}
}

func TestMetadataQueryNoMetadata(t *testing.T) {
	workingSet.servers.Lock()
	workingSet.servers.registry["no-meta"] = &registeredServer{active: true}
	workingSet.servers.Unlock()
	t.Cleanup(func() {
		workingSet.servers.Lock()
		delete(workingSet.servers.registry, "no-meta")
		workingSet.servers.Unlock()
	})

	req := httptest.NewRequest(http.MethodGet, "/metadata?server=no-meta", nil)
	rec := httptest.NewRecorder()
	handleMetadataQuery(rec, req)

	if rec.Code != http.StatusNoContent {
		t.Fatalf("expected 204, got %d", rec.Code)
	}
}

func TestStashServerMetadataEnforcesSizeCap(t *testing.T) {
	srv := &registeredServer{active: true, trackingID: "S-TEST02"}
	big := make([]byte, maxMetadataBytes+1)
	stashServerMetadata("test-server", srv, big)
	if srv.metadata != nil {
		t.Fatal("expected oversized metadata to be rejected")
	}

	stashServerMetadata("test-server", srv, []byte(`{"ok":true}`))
	if srv.metadata == nil || string(srv.metadata.raw) != `{"ok":true}` {
		t.Fatal("expected valid metadata to be stored")
	}
}

// Decode a server's "metadata" message the way handleServerSignal does --
// off the wire, not from a hand-built struct -- so a field that cannot
// survive JSON is caught here instead of going quietly missing in CI.
func TestMetadataMessageFromWire(t *testing.T) {
	srv := &registeredServer{active: true, trackingID: "S-TEST03"}
	payload := `{"playerCount":"3","map":"wizard"}`

	var m signalMessage
	wire := `{"type":"metadata","data":` + strconv.Quote(payload) + `}`
	if err := json.Unmarshal([]byte(wire), &m); err != nil {
		t.Fatalf("unmarshal: %v", err)
	}
	if len(m.Data) == 0 {
		t.Fatalf("metadata message carried no data: %s", wire)
	}
	stashServerMetadata("test-server", srv, []byte(m.Data))

	if srv.metadata == nil {
		t.Fatal("expected metadata to be stored")
	}
	var parsed map[string]string
	if err := json.Unmarshal(srv.metadata.raw, &parsed); err != nil {
		t.Fatalf("unmarshal: %v", err)
	}
	if parsed["playerCount"] != "3" || parsed["map"] != "wizard" {
		t.Fatalf("unexpected parsed metadata: %v", parsed)
	}
}

// A second field tagged "metadata" makes encoding/json drop every one of
// them, in both directions and without an error -- which silently severed
// the whole end-to-end metadata path: servers were never read, clients
// never told. Guard the wire format rather than the struct layout.
func TestSignalMessageMetadataSurvivesJSON(t *testing.T) {
	payload := `{"map":"bloodgulch","identity":"ed25519:abc"}`

	encoded, err := json.Marshal(signalMessage{Type: "answer", MetadataPayload: payload})
	if err != nil {
		t.Fatalf("marshal failed: %v", err)
	}
	if !strings.Contains(string(encoded), `"metadata":`) {
		t.Fatalf("answer carried no metadata field: %s", encoded)
	}

	var decoded signalMessage
	if err := json.Unmarshal(encoded, &decoded); err != nil {
		t.Fatalf("unmarshal failed: %v", err)
	}
	if decoded.MetadataPayload != payload {
		t.Fatalf("metadata did not round-trip: got %q, want %q",
			decoded.MetadataPayload, payload)
	}
}

// A session ID arrives inside the sending server's own message, so the only
// thing standing between one server and another server's clients is this
// ownership check -- not the secrecy of the ID, which any future debug
// endpoint or echoed error could give away.
func TestLookupOwnedSessionRejectsOtherServers(t *testing.T) {
	const (
		ownerID = "owner-server"
		otherID = "other-server"
		sessID  = "0123456789abcdef0123456789abcdef"
	)
	workingSet.clients.Lock()
	workingSet.clients.sessions[sessID] = &clientSession{
		serverID:         ownerID,
		trackingID:       "C-OWN001",
		serverTrackingID: "S-OWN001",
	}
	workingSet.clients.Unlock()
	t.Cleanup(func() {
		workingSet.clients.Lock()
		delete(workingSet.clients.sessions, sessID)
		workingSet.clients.Unlock()
	})

	for _, tc := range []struct {
		name     string
		serverID string
		session  string
		wantOK   bool
	}{
		{"owner is allowed", ownerID, sessID, true},
		{"another registered server is refused", otherID, sessID, false},
		{"unknown session is refused", ownerID, "deadbeef", false},
		{"unregistered sender is refused", "", sessID, false},
	} {
		t.Run(tc.name, func(t *testing.T) {
			got, ok := lookupOwnedSession(tc.serverID, tc.session, "gns-rendezvous")
			if ok != tc.wantOK {
				t.Fatalf("ok = %v, want %v", ok, tc.wantOK)
			}
			if ok && got == nil {
				t.Fatal("returned a nil session alongside ok")
			}
			if !ok && got != nil {
				t.Fatal("returned a session despite refusing it")
			}
		})
	}
}

// relayGNSConnected tears a session's relay down, so reaching it with someone
// else's session ID would be a disconnect primitive needing no cryptography
// at all. Check the teardown does not happen, rather than only that the
// lookup refuses.
func TestRelayGNSConnectedIgnoresForeignSession(t *testing.T) {
	const (
		ownerID = "teardown-owner"
		otherID = "teardown-other"
		sessID  = "fedcba9876543210fedcba9876543210"
	)
	session := &clientSession{
		serverID:         ownerID,
		trackingID:       "C-TDN001",
		serverTrackingID: "S-TDN001",
		protocol:         "WebRTC",
	}
	workingSet.clients.Lock()
	workingSet.clients.sessions[sessID] = session
	workingSet.clients.Unlock()
	t.Cleanup(func() {
		workingSet.clients.Lock()
		delete(workingSet.clients.sessions, sessID)
		workingSet.clients.Unlock()
	})

	// No DataChannel is attached, so a teardown would show up as the
	// protocol being switched off "WebRTC" by closeSessionRelay.
	relayGNSConnected(otherID, sessID)

	session.mu.Lock()
	protocol := session.protocol
	session.mu.Unlock()
	if protocol != "WebRTC" {
		t.Fatalf("a foreign server retired the relay: protocol is now %q", protocol)
	}
}

// The list is how a server states what it is, so normalizing it is where a
// bad or unknown advertisement has to be caught.
func TestParseTransports(t *testing.T) {
	for _, tc := range []struct {
		name string
		msg  signalMessage
		want []string
		bad  bool
	}{
		{"list is taken in order",
			signalMessage{ServerTransports: []string{"relay", "webrtc"}},
			[]string{"relay", "webrtc"}, false},
		{"an unset list means udp",
			signalMessage{}, []string{"relay"}, false},
		{"duplicates collapse",
			signalMessage{ServerTransports: []string{"relay", "relay"}},
			[]string{"relay"}, false},
		{"unknown entries are ignored, not fatal",
			signalMessage{ServerTransports: []string{"carrier-pigeon", "relay"}},
			[]string{"relay"}, false},
		{"nothing usable is an error",
			signalMessage{ServerTransports: []string{"carrier-pigeon"}}, nil, true},
	} {
		t.Run(tc.name, func(t *testing.T) {
			got, err := parseTransports(tc.msg)
			if tc.bad {
				if err == nil {
					t.Fatalf("expected an error, got %v", got)
				}
				return
			}
			if err != nil {
				t.Fatalf("unexpected error: %v", err)
			}
			if len(got) != len(tc.want) {
				t.Fatalf("got %v, want %v", got, tc.want)
			}
			for i := range got {
				if got[i] != tc.want[i] {
					t.Fatalf("got %v, want %v", got, tc.want)
				}
			}
		})
	}
}

// The answer is what tells a client which kinds of server it may choose
// between, so the list has to survive the wire intact and in order.
func TestAnswerCarriesTransportList(t *testing.T) {
	encoded, err := json.Marshal(signalMessage{
		Type:             "answer",
		ServerTransports: []string{"webrtc", "relay"},
	})
	if err != nil {
		t.Fatalf("marshal failed: %v", err)
	}
	body := string(encoded)
	if !strings.Contains(body, `"serverTransports":["webrtc","relay"]`) {
		t.Fatalf("answer lost the transport list: %s", body)
	}

	var decoded signalMessage
	if err := json.Unmarshal(encoded, &decoded); err != nil {
		t.Fatalf("unmarshal failed: %v", err)
	}
	if len(decoded.ServerTransports) != 2 || decoded.ServerTransports[0] != "webrtc" {
		t.Fatalf("transports did not round-trip: %v", decoded.ServerTransports)
	}
}

func TestRegisteredServerSupports(t *testing.T) {
	srv := &registeredServer{transports: []string{"relay", "webrtc"}}
	if !srv.supports("relay") || !srv.supports("webrtc") {
		t.Fatal("advertised transports reported as unsupported")
	}
	if srv.supports("carrier-pigeon") {
		t.Fatal("unadvertised transport reported as supported")
	}
	if firstTransport(srv.transports) != "relay" {
		t.Fatalf("preferred should be the first entry, got %q",
			firstTransport(srv.transports))
	}
	var nilSrv *registeredServer
	if nilSrv.supports("relay") {
		t.Fatal("nil server should be inert, not panic or claim support")
	}
}
