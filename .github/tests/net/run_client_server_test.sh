#!/bin/bash
# Client/server replication integration test.
#
# Boots two BlamGraphics instances against the same build: one with
# --listen (server), one with --server (client), pointed at each other over
# loopback via the engine's existing CLI args (graphics.cpp / networking.cpp)
# — this script does not add any new connection plumbing. Each instance gets
# its own TMPDIR (already-overridable per url.cpp — with it set, RSCA::TempFile
# resolves to exactly that directory, no app-name subdir) so their journal /
# screenshot output don't collide. Each process appends its test journal
# (journal.h: game events, net_* lifecycle, dummy events, state_dump roster
# snapshots from the "dump_state" custom event) to <its TMPDIR>/journal.jsonl.
# The journal comparison then diffs the final state_dump of the two
# for replication bugs (missing/duplicate players, name mismatches, stale
# "remote"/"connected" flags) — this is how issues like players not syncing
# between server and client show up. On failure the merged event timeline
# from both journals is printed for debugging.
#
# Usage:
#   run_client_server_test.sh [OUT_DIR]
#
# Env overrides:
#   TARGET        cb build/run target        (default: desktop:x86_64-buildroot-linux-gnu:multi/BlamGraphics)
#   RESOURCE_DIR  asset dir passed to both    (default: multi_build/desktop-x86_64-buildroot-linux-gnu-multi/examples/blam/cblam-testing/assets/)
#   MAP           map file passed to both     (default: /mnt/blam/pc/bloodgulch.map)
#   PORT          loopback port               (default: 27105)
#   BOOT_TIMEOUT  seconds to wait for the server's listen socket (default: 30)
#   RUN_TIMEOUT   hard cap per process, via `timeout`, before it's killed (default: 40)
#   CLIENT_DELAY  seconds to wait after the server is listening before
#                 starting the client (default: 0). Fabricates a client
#                 connecting well after the server has already finished
#                 loading — regression cover for the GameJoin race where a
#                 connection's handshake could complete before m_map was
#                 set, with no retry, leaving the client fully connected
#                 but with no map/player/camera ever created.
#   BUILD    set to 1 to build using `./cb build`
#   SERVER_DUMMY_PLUG_CONFIG  path to the server's dummy_plug config
#                 (default: dummy_plug_net_server.json). Swap this (and
#                 CLIENT_DUMMY_PLUG_CONFIG) to run a different scripted
#                 scenario against the same connect/compare plumbing, e.g.
#                 dummy_plug_net_{server,client}_scenario.json for the
#                 movement + server-triggered map-switch scenario.
#   CLIENT_DUMMY_PLUG_CONFIG  path to the client's dummy_plug config
#                 (default: dummy_plug_net_client.json)

set -uo pipefail
HERE="$(cd "$(dirname "$0")" && pwd)"
SRCDIR="$(cd "$HERE/../../.." && pwd)"
cd "$SRCDIR"

OUT_DIR="${1:-/tmp/net_test}"
TARGET="${TARGET:-desktop:x86_64-buildroot-linux-gnu:multi/BlamGraphics}"
RESOURCE_DIR="${RESOURCE_DIR:-multi_build/desktop-x86_64-buildroot-linux-gnu-multi/examples/blam/cblam-testing/assets/}"
MAP="${MAP:-/mnt/blam/pc/bloodgulch.map}"
PORT="${PORT:-27105}"
BOOT_TIMEOUT="${BOOT_TIMEOUT:-60}"
RUN_TIMEOUT="${RUN_TIMEOUT:-180}"
CLIENT_DELAY="${CLIENT_DELAY:-0}"

# The static server config's own dummy_plug end_time (11s) self-closes the
# server long before a large CLIENT_DELAY would let a client reach it —
# that's a harness bug, not a game bug (first found this the hard way: a
# CLIENT_DELAY=20 run failed with a UDP-level "timed out attempting to
# connect", not the GameJoin-race symptom it was meant to probe, because
# the server had already exited ~9s before the client even tried). When
# delaying, generate a server config whose schedule is shifted to actually
# outlive the delay, and give RUN_TIMEOUT enough room too.
SERVER_DUMMY_PLUG_CONFIG="${SERVER_DUMMY_PLUG_CONFIG:-$HERE/dummy_plug_net_server.json}"
CLIENT_DUMMY_PLUG_CONFIG="${CLIENT_DUMMY_PLUG_CONFIG:-$HERE/dummy_plug_net_client.json}"
if [ "$CLIENT_DELAY" -gt 0 ]; then
    mkdir -p "$OUT_DIR"
    SERVER_DUMMY_PLUG_CONFIG="$OUT_DIR/dummy_plug_net_server_delayed.json"
    python3 - "$HERE/dummy_plug_net_server.json" "$SERVER_DUMMY_PLUG_CONFIG" "$CLIENT_DELAY" <<'PYEOF'
import json, sys
src, dst, delay = sys.argv[1], sys.argv[2], int(sys.argv[3])
cfg = json.load(open(src))
margin_ms = 15000  # time after connect for join/load/dump/screenshot to finish
base_ms = delay * 1000
for e in cfg["events"]:
    e["time"] += base_ms
cfg["end_time"] = base_ms + margin_ms
json.dump(cfg, open(dst, "w"), indent=1)
PYEOF
    NEEDED_TIMEOUT=$((CLIENT_DELAY + 25))
    if [ "$NEEDED_TIMEOUT" -gt "$RUN_TIMEOUT" ]; then
        RUN_TIMEOUT="$NEEDED_TIMEOUT"
    fi
fi

SERVER_TMP="$OUT_DIR/server_tmp"
CLIENT_TMP="$OUT_DIR/client_tmp"
mkdir -p "$SERVER_TMP" "$CLIENT_TMP"
SERVER_LOG="$OUT_DIR/server.log"
CLIENT_LOG="$OUT_DIR/client.log"
SERVER_JOURNAL="$SERVER_TMP/journal.jsonl"
CLIENT_JOURNAL="$CLIENT_TMP/journal.jsonl"
rm -f "$SERVER_LOG" "$CLIENT_LOG" "$SERVER_JOURNAL" "$CLIENT_JOURNAL"

echo "Out dir : $OUT_DIR"
echo "Target  : $TARGET"
echo "Port    : $PORT"

if [ "${BUILD:-0}" != "0" ]; then
    echo "::group::Building $TARGET"
    BUILD_HOST_TOOLS=0 COFFEE_DISABLE_PROFILER=1 ./cb build "$TARGET" || {
        echo "FAIL: build failed"
        exit 2
    }
    echo "::endgroup::"
fi

BASE_RUN_ARGS=(run "$TARGET" -- "$RESOURCE_DIR" "$MAP")

echo "Starting server..."
TMPDIR="$SERVER_TMP" \
DUMMY_PLUG_CONFIG="$SERVER_DUMMY_PLUG_CONFIG" \
COFFEE_DISABLE_PROFILER=1 \
timeout "${RUN_TIMEOUT}s" ./cb "${BASE_RUN_ARGS[@]}" --listen "127.0.0.1:$PORT" \
    > "$SERVER_LOG" 2>&1 &
SERVER_PID=$!

echo "Waiting for server to start listening (up to ${BOOT_TIMEOUT}s)..."
booted=0
for _ in $(seq 1 "$BOOT_TIMEOUT"); do
    if grep -q "Started server on" "$SERVER_LOG" 2>/dev/null; then
        booted=1
        break
    fi
    if ! kill -0 "$SERVER_PID" 2>/dev/null; then
        echo "FAIL: server process exited before it started listening"
        echo "--- server.log tail ---"
        tail -n 40 "$SERVER_LOG"
        exit 1
    fi
    sleep 1
done
if [ "$booted" != "1" ]; then
    echo "FAIL: server did not report listening within ${BOOT_TIMEOUT}s"
    kill "$SERVER_PID" 2>/dev/null
    tail -n 40 "$SERVER_LOG"
    exit 1
fi
echo "Server is listening."

if [ "$CLIENT_DELAY" -gt 0 ]; then
    echo "Waiting ${CLIENT_DELAY}s before starting client (fabricating a delayed-connect race)..."
    sleep "$CLIENT_DELAY"
fi

echo "Starting client..."
TMPDIR="$CLIENT_TMP" \
DUMMY_PLUG_CONFIG="$CLIENT_DUMMY_PLUG_CONFIG" \
COFFEE_DISABLE_PROFILER=1 \
timeout "${RUN_TIMEOUT}s" ./cb "${BASE_RUN_ARGS[@]}" --server "127.0.0.1:$PORT" \
    > "$CLIENT_LOG" 2>&1 &
CLIENT_PID=$!

echo "Waiting for both processes to finish (dummy_plug end_time closes them)..."
wait "$CLIENT_PID"
CLIENT_EXIT=$?
wait "$SERVER_PID"
SERVER_EXIT=$?

echo "server exit=$SERVER_EXIT  client exit=$CLIENT_EXIT"
if [ "$SERVER_EXIT" = "124" ] || [ "$CLIENT_EXIT" = "124" ]; then
    echo "FAIL: a process hit the ${RUN_TIMEOUT}s timeout (hung — likely connection never established)"
fi

echo
echo "::group::server.log (connection + state-dump lines)"
grep -E "State dumped|Started server|Connection info|Player joined|Controller|Error" "$SERVER_LOG" || true
echo "::endgroup::"
echo "::group::client.log (connection + state-dump lines)"
grep -E "State dumped|join confirmation|roster received|Error" "$CLIENT_LOG" || true
echo "::endgroup::"

if [ ! -s "$SERVER_JOURNAL" ]; then
    echo "FAIL: $SERVER_JOURNAL was never written (journal disabled, or the process died at startup?)"
    exit 1
fi
if [ ! -s "$CLIENT_JOURNAL" ]; then
    echo "FAIL: $CLIENT_JOURNAL was never written (journal disabled, or the process died at startup?)"
    exit 1
fi

echo
echo "=== Journal comparison ==="
echo "server: $SERVER_JOURNAL"
echo "client: $CLIENT_JOURNAL"
python3 "$HERE/compare_journals.py" "$SERVER_JOURNAL" "$CLIENT_JOURNAL"
RESULT=$?
if [ "$RESULT" != "0" ]; then
    echo
    echo "::group::merged journal timeline"
    python3 "$HERE/compare_journals.py" --timeline "$SERVER_JOURNAL" "$CLIENT_JOURNAL" || true
    echo "::endgroup::"
fi
exit $RESULT
