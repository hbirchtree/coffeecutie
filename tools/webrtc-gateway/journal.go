package main

// journal is the gateway's persistent connection log: one SQLite row per
// interesting lifecycle moment of a server registration or a client
// session, so the admin panel's Search tab can answer "what happened to
// tracking ID C-A178E8 last Tuesday" after the process (and its stdout)
// is long gone.
//
// It is a log, never a source of truth: nothing in the relay path ever
// reads it back, so a failed or dropped write can never affect routing.
// That is what makes the fire-and-forget writer below safe.

import (
	"database/sql"
	"fmt"
	"log"
	"strings"
	"sync"
	"sync/atomic"
	"time"

	// Pure-Go SQLite (no cgo), so the gateway still cross-compiles and
	// still builds with CGO_ENABLED=0 the way CI does.
	_ "modernc.org/sqlite"
)

// journalEvent is one row. Every field is optional except ts/scope/event:
// a rejected registration has no tracking ID to attribute yet, and a
// server-scope row has no client.
type journalEvent struct {
	ts time.Time
	// scope is "server", "client" or "host" -- which side of the gateway
	// the row is about, used to filter the Search tab.
	scope string
	// event is the lifecycle point, e.g. "register", "session-open",
	// "peer-state", "datachannel-open", "relay-punch", "session-close".
	event string
	// outcome is "ok", "pending", "timeout", "error", "closed" or
	// "rejected" -- the coarse answer to "did this step work", kept
	// separate from detail so it can be filtered on later if needed.
	outcome          string
	serverID         string
	serverTrackingID string
	clientTrackingID string
	// origin is the signaling peer address, transport the server's mode.
	origin    string
	transport string
	// detail is free text: an error string, an ICE state, an address.
	detail string
}

// Deliberately absent: the session ID. It is a capability -- possessing
// it lets a host attach to a client's session (see handleHostSignal) --
// and a searchable log that outlives the process is exactly where it
// must not end up. Sessions are identified here by client tracking ID,
// which is inert by design (see newTrackingID).

type Journal struct {
	db *sql.DB
	// Writes are queued and drained by one goroutine: SQLite takes a
	// single writer anyway, and this keeps Record() off the relay path's
	// critical sections. A full queue drops rather than blocks -- see
	// Record.
	queue chan journalEvent
	done  chan struct{}
	// mu guards closed against queue sends. Shutdown is not quiescent:
	// pion callbacks keep firing (peer-state, datachannel-close) while
	// the process is on its way out, and a send on the closed queue
	// would panic in whichever callback goroutine got there last.
	// Record takes RLock (so concurrent recorders don't serialize),
	// Close takes Lock.
	mu      sync.RWMutex
	closed  bool
	dropped atomic.Uint64
}

const journalSchema = `
CREATE TABLE IF NOT EXISTS events (
    id                  INTEGER PRIMARY KEY AUTOINCREMENT,
    ts                  INTEGER NOT NULL,
    scope               TEXT    NOT NULL,
    event               TEXT    NOT NULL,
    outcome             TEXT    NOT NULL DEFAULT '',
    server_id           TEXT    NOT NULL DEFAULT '',
    server_tracking_id  TEXT    NOT NULL DEFAULT '',
    client_tracking_id  TEXT    NOT NULL DEFAULT '',
    origin              TEXT    NOT NULL DEFAULT '',
    transport           TEXT    NOT NULL DEFAULT '',
    detail              TEXT    NOT NULL DEFAULT ''
);
CREATE INDEX IF NOT EXISTS events_ts          ON events (ts);
CREATE INDEX IF NOT EXISTS events_server      ON events (server_id);
CREATE INDEX IF NOT EXISTS events_server_trk  ON events (server_tracking_id);
CREATE INDEX IF NOT EXISTS events_client_trk  ON events (client_tracking_id);
`

// openJournal opens (creating if needed) the journal database and starts
// its writer goroutine. Retention prunes rows older than the given age;
// zero keeps everything.
func openJournal(path string, retention time.Duration) (*Journal, error) {
	// WAL so the admin panel's reads never block the writer, and a busy
	// timeout so a slow read can't turn into a write error.
	db, err := sql.Open("sqlite", path+"?_pragma=journal_mode(WAL)&_pragma=busy_timeout(5000)")
	if err != nil {
		return nil, err
	}
	if _, err := db.Exec(journalSchema); err != nil {
		db.Close()
		return nil, fmt.Errorf("creating schema: %w", err)
	}

	j := &Journal{
		db: db,
		// Deep enough to swallow a burst of session churn; the writer
		// drains it far faster than sessions are created.
		queue: make(chan journalEvent, 4096),
		done:  make(chan struct{}),
	}
	go j.writeLoop(retention)
	return j, nil
}

// Record queues an event. Safe on a nil Journal (journaling disabled),
// and never blocks: if the queue is full the row is dropped and counted,
// because stalling a DataChannel callback to write a log line would turn
// a disk hiccup into dropped game traffic.
func (j *Journal) Record(ev journalEvent) {
	if j == nil {
		return
	}
	if ev.ts.IsZero() {
		ev.ts = time.Now()
	}
	j.mu.RLock()
	defer j.mu.RUnlock()
	if j.closed {
		return
	}
	select {
	case j.queue <- ev:
	default:
		j.dropped.Add(1)
	}
}

// Dropped reports how many events were discarded by a full queue, for
// the admin panel's settings pane.
func (j *Journal) Dropped() uint64 {
	if j == nil {
		return 0
	}
	return j.dropped.Load()
}

func (j *Journal) writeLoop(retention time.Duration) {
	defer close(j.done)

	insert, err := j.db.Prepare(`INSERT INTO events
        (ts, scope, event, outcome, server_id, server_tracking_id,
         client_tracking_id, origin, transport, detail)
        VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?)`)
	if err != nil {
		log.Printf("journal: failed to prepare insert, journaling disabled: %v", err)
		return
	}
	defer insert.Close()

	var prune <-chan time.Time
	if retention > 0 {
		j.pruneOlderThan(retention)
		ticker := time.NewTicker(time.Hour)
		defer ticker.Stop()
		prune = ticker.C
	}

	for {
		select {
		case ev, ok := <-j.queue:
			if !ok {
				return
			}
			_, err := insert.Exec(
				ev.ts.UnixMilli(), ev.scope, ev.event, ev.outcome,
				ev.serverID, ev.serverTrackingID, ev.clientTrackingID,
				ev.origin, ev.transport, ev.detail)
			if err != nil {
				log.Printf("journal: insert failed: %v", err)
			}
		case <-prune:
			j.pruneOlderThan(retention)
		}
	}
}

func (j *Journal) pruneOlderThan(retention time.Duration) {
	cutoff := time.Now().Add(-retention).UnixMilli()
	res, err := j.db.Exec(`DELETE FROM events WHERE ts < ?`, cutoff)
	if err != nil {
		log.Printf("journal: retention prune failed: %v", err)
		return
	}
	if n, _ := res.RowsAffected(); n > 0 {
		log.Printf("journal: pruned %d events older than %s", n, retention)
	}
}

// Close drains the queue and closes the database. Idempotent: both
// main's defer and the admin interface's signal handler call it, and
// whichever runs second must be a no-op rather than a double close.
func (j *Journal) Close() {
	if j == nil {
		return
	}
	j.mu.Lock()
	if j.closed {
		j.mu.Unlock()
		return
	}
	j.closed = true
	close(j.queue)
	j.mu.Unlock()

	<-j.done
	j.db.Close()
}

// journalFilter is the Search tab's query. Empty fields match everything;
// the ID fields match case-insensitively on any substring, so a partial
// tracking ID typed from a screenshot still finds its session.
type journalFilter struct {
	trackingID string
	serverID   string
	limit      int
}

// Query returns matching events, newest first.
func (j *Journal) Query(f journalFilter) ([]journalEvent, error) {
	if j == nil {
		return nil, fmt.Errorf("journaling is disabled (-journal-db not set)")
	}

	where := []string{}
	args := []any{}
	if t := strings.TrimSpace(f.trackingID); t != "" {
		// One box for both ID kinds: a client tracking ID and the server
		// tracking ID of the registration it belongs to are the two ways
		// anyone actually holds one of these, and matching either means
		// pasting a server's ID gets its clients' rows too.
		where = append(where,
			"(server_tracking_id LIKE ? ESCAPE '\\' OR client_tracking_id LIKE ? ESCAPE '\\')")
		pattern := "%" + escapeLike(t) + "%"
		args = append(args, pattern, pattern)
	}
	if s := strings.TrimSpace(f.serverID); s != "" {
		where = append(where, "server_id LIKE ? ESCAPE '\\'")
		args = append(args, "%"+escapeLike(s)+"%")
	}

	query := `SELECT ts, scope, event, outcome, server_id, server_tracking_id,
                     client_tracking_id, origin, transport, detail FROM events`
	if len(where) > 0 {
		query += " WHERE " + strings.Join(where, " AND ")
	}
	limit := f.limit
	if limit <= 0 {
		limit = 500
	}
	query += " ORDER BY ts DESC, id DESC LIMIT ?"
	args = append(args, limit)

	rows, err := j.db.Query(query, args...)
	if err != nil {
		return nil, err
	}
	defer rows.Close()

	events := make([]journalEvent, 0, 64)
	for rows.Next() {
		var (
			ev     journalEvent
			tsMsec int64
		)
		if err := rows.Scan(&tsMsec, &ev.scope, &ev.event, &ev.outcome,
			&ev.serverID, &ev.serverTrackingID, &ev.clientTrackingID,
			&ev.origin, &ev.transport, &ev.detail); err != nil {
			return nil, err
		}
		ev.ts = time.UnixMilli(tsMsec)
		events = append(events, ev)
	}
	return events, rows.Err()
}

// escapeLike neutralizes LIKE's wildcards so a typed % or _ matches
// itself rather than silently widening the search.
func escapeLike(s string) string {
	r := strings.NewReplacer(`\`, `\\`, `%`, `\%`, `_`, `\_`)
	return r.Replace(s)
}

// The helpers below are what the gateway body actually calls -- they keep
// the call sites to one line so instrumenting a branch stays cheap.

func journalServerEvent(srv *registeredServer, serverID, event, outcome, detail string) {
	ev := journalEvent{
		scope:    "server",
		event:    event,
		outcome:  outcome,
		serverID: serverID,
		detail:   detail,
	}
	if srv != nil {
		ev.serverTrackingID = srv.trackingID
		ev.transport = srv.transport
	}
	journal.Record(ev)
}

func journalClientEvent(session *clientSession, event, outcome, detail string) {
	journalSessionEvent("client", session, event, outcome, detail)
}

// journalHostEvent records the server half of a DataChannel<->DataChannel
// bridge. Same session identity as the client half -- scope is what tells
// the two sides of one bridge apart in the Search tab.
func journalHostEvent(session *clientSession, event, outcome, detail string) {
	journalSessionEvent("host", session, event, outcome, detail)
}

func journalSessionEvent(scope string, session *clientSession, event, outcome, detail string) {
	if session == nil {
		return
	}
	journal.Record(journalEvent{
		scope:            scope,
		event:            event,
		outcome:          outcome,
		serverID:         session.serverID,
		serverTrackingID: session.serverTrackingID,
		clientTrackingID: session.trackingID,
		detail:           detail,
	})
}
