#!/bin/bash
# Client/server replication integration test.
#
# Boots two BlamGraphics instances against the same build: one with
# --listen (server), one with --server (client), pointed at each other over
# loopback via the engine's existing CLI args (graphics.cpp / networking.cpp)
# — this script does not add any new connection plumbing. Each instance gets
# its own TMPDIR (already-overridable per url.cpp — with it set, RSCA::TempFile
# resolves to exactly that directory, no app-name subdir) so their profile.json /
# screenshot / state.json output don't collide, and its own dummy_plug config
# that fires the "dump_state" custom event (resource_creation.cpp) near the
# end, writing that process's view of the world to <its TMPDIR>/state.json.
# compare_state.py then diffs the two files for replication bugs
# (missing/duplicate players, name mismatches, stale "remote"/"connected"
# flags) — this is how issues like players not syncing between server and
# client show up.
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
#   SKIP_BUILD    set to 1 to skip the `./cb build` step

set -uo pipefail
HERE="$(cd "$(dirname "$0")" && pwd)"
SRCDIR="$(cd "$HERE/../../.." && pwd)"
cd "$SRCDIR"

OUT_DIR="${1:-/tmp/net_test}"
TARGET="${TARGET:-desktop:x86_64-buildroot-linux-gnu:multi/BlamGraphics}"
RESOURCE_DIR="${RESOURCE_DIR:-multi_build/desktop-x86_64-buildroot-linux-gnu-multi/examples/blam/cblam-testing/assets/}"
MAP="${MAP:-/mnt/blam/pc/bloodgulch.map}"
PORT="${PORT:-27105}"
BOOT_TIMEOUT="${BOOT_TIMEOUT:-30}"
RUN_TIMEOUT="${RUN_TIMEOUT:-40}"

SERVER_TMP="$OUT_DIR/server_tmp"
CLIENT_TMP="$OUT_DIR/client_tmp"
mkdir -p "$SERVER_TMP" "$CLIENT_TMP"
SERVER_LOG="$OUT_DIR/server.log"
CLIENT_LOG="$OUT_DIR/client.log"
SERVER_STATE="$SERVER_TMP/state.json"
CLIENT_STATE="$CLIENT_TMP/state.json"
rm -f "$SERVER_LOG" "$CLIENT_LOG" "$SERVER_STATE" "$CLIENT_STATE"

echo "Out dir : $OUT_DIR"
echo "Target  : $TARGET"
echo "Port    : $PORT"

if [ "${SKIP_BUILD:-0}" != "1" ]; then
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
DUMMY_PLUG_CONFIG="$HERE/dummy_plug_net_server.json" \
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

echo "Starting client..."
TMPDIR="$CLIENT_TMP" \
DUMMY_PLUG_CONFIG="$HERE/dummy_plug_net_client.json" \
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

if [ ! -s "$SERVER_STATE" ]; then
    echo "FAIL: $SERVER_STATE was never written (dump_state didn't fire before the process exited?)"
    exit 1
fi
if [ ! -s "$CLIENT_STATE" ]; then
    echo "FAIL: $CLIENT_STATE was never written (dump_state didn't fire before the process exited?)"
    exit 1
fi

echo
echo "=== State comparison ==="
echo "server: $SERVER_STATE"
echo "client: $CLIENT_STATE"
python3 "$HERE/compare_state.py" "$SERVER_STATE" "$CLIENT_STATE"
exit $?
