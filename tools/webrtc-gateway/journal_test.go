package main

import (
	"path/filepath"
	"testing"
	"time"
)

func newTestJournal(t *testing.T) *Journal {
	t.Helper()
	j, err := openJournal(filepath.Join(t.TempDir(), "journal.db"), 0)
	if err != nil {
		t.Fatalf("openJournal: %v", err)
	}
	t.Cleanup(j.Close)
	return j
}

// record writes and waits for the row to be visible: the writer is
// asynchronous, so a test that queries immediately races it.
func record(t *testing.T, j *Journal, evs ...journalEvent) {
	t.Helper()
	for _, ev := range evs {
		j.Record(ev)
	}
	deadline := time.Now().Add(5 * time.Second)
	for {
		var n int
		if err := j.db.QueryRow(`SELECT count(*) FROM events`).Scan(&n); err != nil {
			t.Fatalf("counting rows: %v", err)
		}
		if n >= len(evs) {
			return
		}
		if time.Now().After(deadline) {
			t.Fatalf("only %d of %d events were written", n, len(evs))
		}
		time.Sleep(10 * time.Millisecond)
	}
}

func TestJournalQueryFiltersByTrackingID(t *testing.T) {
	j := newTestJournal(t)
	record(t, j,
		journalEvent{scope: "server", event: "register", outcome: "ok",
			serverID: "alpha", serverTrackingID: "S-AAA111"},
		journalEvent{scope: "client", event: "session-open", outcome: "ok",
			serverID: "alpha", serverTrackingID: "S-AAA111", clientTrackingID: "C-BBB222"},
		journalEvent{scope: "client", event: "session-open", outcome: "ok",
			serverID: "beta", serverTrackingID: "S-CCC333", clientTrackingID: "C-DDD444"},
	)

	// A client tracking ID matches only its own row.
	got, err := j.Query(journalFilter{trackingID: "C-BBB222"})
	if err != nil {
		t.Fatalf("Query: %v", err)
	}
	if len(got) != 1 || got[0].clientTrackingID != "C-BBB222" {
		t.Fatalf("client tracking filter returned %d rows: %+v", len(got), got)
	}

	// A server tracking ID matches the registration AND the sessions that
	// belong to it -- that is the point of matching either column.
	got, err = j.Query(journalFilter{trackingID: "S-AAA111"})
	if err != nil {
		t.Fatalf("Query: %v", err)
	}
	if len(got) != 2 {
		t.Fatalf("server tracking filter returned %d rows, want 2: %+v", len(got), got)
	}

	// Partial IDs work: they get read off screenshots and truncated logs.
	got, err = j.Query(journalFilter{trackingID: "BBB"})
	if err != nil {
		t.Fatalf("Query: %v", err)
	}
	if len(got) != 1 {
		t.Fatalf("partial tracking filter returned %d rows, want 1", len(got))
	}
}

func TestJournalQueryFiltersCombine(t *testing.T) {
	j := newTestJournal(t)
	record(t, j,
		journalEvent{scope: "client", event: "session-open",
			serverID: "alpha", serverTrackingID: "S-AAA111", clientTrackingID: "C-BBB222"},
		journalEvent{scope: "client", event: "session-open",
			serverID: "beta", serverTrackingID: "S-AAA111", clientTrackingID: "C-BBB222"},
	)

	got, err := j.Query(journalFilter{trackingID: "C-BBB222", serverID: "beta"})
	if err != nil {
		t.Fatalf("Query: %v", err)
	}
	if len(got) != 1 || got[0].serverID != "beta" {
		t.Fatalf("combined filter returned %d rows: %+v", len(got), got)
	}
}

// A typed % or _ must match itself; otherwise a stray wildcard silently
// widens the search and the operator trusts the wrong result.
func TestJournalQueryEscapesWildcards(t *testing.T) {
	j := newTestJournal(t)
	record(t, j,
		journalEvent{scope: "client", event: "session-open", serverID: "alpha"},
		journalEvent{scope: "client", event: "session-open", serverID: "a%b"},
	)

	got, err := j.Query(journalFilter{serverID: "a%b"})
	if err != nil {
		t.Fatalf("Query: %v", err)
	}
	if len(got) != 1 || got[0].serverID != "a%b" {
		t.Fatalf("wildcard-escaped filter returned %d rows: %+v", len(got), got)
	}
}

func TestJournalQueryOrdersNewestFirst(t *testing.T) {
	j := newTestJournal(t)
	base := time.Now().Add(-time.Hour)
	record(t, j,
		journalEvent{ts: base, scope: "client", event: "older", serverID: "alpha"},
		journalEvent{ts: base.Add(time.Minute), scope: "client", event: "newer", serverID: "alpha"},
	)

	got, err := j.Query(journalFilter{serverID: "alpha"})
	if err != nil {
		t.Fatalf("Query: %v", err)
	}
	if len(got) != 2 || got[0].event != "newer" {
		t.Fatalf("expected newest first, got %+v", got)
	}
}

// Record must stay safe once Close has run: pion callbacks keep firing
// while the process shuts down.
func TestJournalRecordAfterCloseDoesNotPanic(t *testing.T) {
	j, err := openJournal(filepath.Join(t.TempDir(), "journal.db"), 0)
	if err != nil {
		t.Fatalf("openJournal: %v", err)
	}
	j.Close()
	j.Record(journalEvent{scope: "client", event: "session-close"})
	// Close is called by both main's defer and the signal handler.
	j.Close()
}

func TestNilJournalIsSafe(t *testing.T) {
	var j *Journal
	j.Record(journalEvent{scope: "client", event: "session-open"})
	j.Close()
	if j.Dropped() != 0 {
		t.Fatal("nil journal reported drops")
	}
	if _, err := j.Query(journalFilter{}); err == nil {
		t.Fatal("expected an error querying a disabled journal")
	}
}
