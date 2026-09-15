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
