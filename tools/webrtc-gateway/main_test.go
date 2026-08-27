package main

import (
	"encoding/json"
	"io"
	"net/http"
	"net/http/httptest"
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

func TestMetadataMessageBackwardCompatibility(t *testing.T) {
	srv := &registeredServer{active: true, trackingID: "S-TEST03"}
	m := signalMessage{
		Type:     "metadata",
		Metadata: map[string]string{"playerCount": "3", "map": "wizard"},
	}

	var encoded []byte
	if len(m.Data) > 0 {
		encoded = []byte(m.Data)
	} else if len(m.Metadata) > 0 {
		var err error
		encoded, err = json.Marshal(m.Metadata)
		if err != nil {
			t.Fatalf("marshal: %v", err)
		}
	}
	stashServerMetadata("test-server", srv, encoded)

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
