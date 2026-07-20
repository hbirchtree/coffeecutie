#!/bin/bash
# WebRTC-transport client/server replication integration test.
#
# Same coverage as run_client_server_test.sh (two BlamGraphics instances,
# journal comparison via compare_journals.py) but routes the connection
# through two webrtc-gateway (tools/webrtc-gateway) processes instead of a
# direct UDP dial -- exercising the CConnectionTransportP2PWebRTC path end
# to end (see examples/blam/cblam-testing/WEBRTC_TRANSPORT.md). Both ends
# are native WebRTC/DataChannel peers (confirmed necessary: GNS's P2P
# custom-signaling accept path can't interoperate with a plain UDP listen
# socket, see the doc's "GNS-level signaling" section) -- gateway A hosts
# the client's DataChannel + /server-signal (GNS rendezvous relay),
# gateway B hosts the server's own per-connection DataChannel. The two
# gateways bridge those independently-negotiated DataChannels together via
# -relay-udp-port, each pointing -dest at the other's fixed relay port.
#
# Topology:
#   client --ws(/signal)--> gateway A <--UDP(relay-udp-port)--> gateway B <--ws(/signal)-- server (per-connection)
#                            gateway A <--ws(/server-signal)-- server (persistent, GNS rendezvous only)
#
# Usage:
#   run_webrtc_client_server_test.sh [OUT_DIR]
#
# Env overrides:
#   TARGET        cb build/run target        (default: desktop:x86_64-buildroot-linux-gnu:multi/BlamGraphics)
#   RESOURCE_DIR  asset dir passed to both    (default: multi_build/desktop-x86_64-buildroot-linux-gnu-multi/examples/blam/cblam-testing/assets/)
#   MAP           map file passed to both     (default: /mnt/blam/pc/bloodgulch.map)
#   GATEWAY_A_HTTP_PORT / GATEWAY_B_HTTP_PORT   gateway WebSocket signaling ports (default: 8098 / 8099)
#   GATEWAY_A_RELAY_PORT / GATEWAY_B_RELAY_PORT fixed UDP relay ports bridging the two gateways (default: 19501 / 19502)
#   BOOT_TIMEOUT  seconds to wait for each boot signal (gateways, server /server-signal registration) (default: 30)
#   RUN_TIMEOUT   hard cap per BlamGraphics process, via `timeout` (default: 60)
#   BUILD         set to 1 to build BlamGraphics using `./cb build` first
#   BUILD_GATEWAY set to 1 to `go build` the gateway first (default: 0, same
#                 opt-in convention as BUILD -- caller is responsible for the
#                 binary being current; a stale one from an earlier manual
#                 build fails at runtime with a confusing "flag provided but
#                 not defined" rather than here, so rebuild after touching
#                 tools/webrtc-gateway if you're not running from a fresh
#                 checkout/CI)
#   SERVER_DUMMY_PLUG_CONFIG  path to the server's dummy_plug config (default: dummy_plug_net_webrtc_server.json)
#   CLIENT_DUMMY_PLUG_CONFIG  path to the client's dummy_plug config (default: dummy_plug_net_webrtc_client.json)
#   GO            go binary to use for BUILD_GATEWAY (default: go, falling back to ~/local/go/bin/go if not on PATH)

set -uo pipefail
HERE="$(cd "$(dirname "$0")" && pwd)"
SRCDIR="$(cd "$HERE/../../.." && pwd)"
GATEWAY_DIR="$SRCDIR/tools/webrtc-gateway"
cd "$SRCDIR"

OUT_DIR="${1:-/tmp/webrtc_net_test}"
TARGET="${TARGET:-desktop:x86_64-buildroot-linux-gnu:multi/BlamGraphics}"
RESOURCE_DIR="${RESOURCE_DIR:-multi_build/desktop-x86_64-buildroot-linux-gnu-multi/examples/blam/cblam-testing/assets/}"
MAP="${MAP:-/mnt/blam/pc/bloodgulch.map}"
GATEWAY_A_HTTP_PORT="${GATEWAY_A_HTTP_PORT:-8098}"
GATEWAY_B_HTTP_PORT="${GATEWAY_B_HTTP_PORT:-8099}"
GATEWAY_A_RELAY_PORT="${GATEWAY_A_RELAY_PORT:-19501}"
GATEWAY_B_RELAY_PORT="${GATEWAY_B_RELAY_PORT:-19502}"
BOOT_TIMEOUT="${BOOT_TIMEOUT:-30}"
RUN_TIMEOUT="${RUN_TIMEOUT:-60}"

SERVER_DUMMY_PLUG_CONFIG="${SERVER_DUMMY_PLUG_CONFIG:-$HERE/dummy_plug_net_webrtc_server.json}"
CLIENT_DUMMY_PLUG_CONFIG="${CLIENT_DUMMY_PLUG_CONFIG:-$HERE/dummy_plug_net_webrtc_client.json}"

GO="${GO:-go}"
if ! command -v "$GO" >/dev/null 2>&1; then
    if [ -x "$HOME/local/go/bin/go" ]; then
        GO="$HOME/local/go/bin/go"
    fi
fi

GATEWAY_BIN="$GATEWAY_DIR/gateway"
BUILD_GATEWAY="${BUILD_GATEWAY:-0}"

SERVER_TMP="$OUT_DIR/server_tmp"
CLIENT_TMP="$OUT_DIR/client_tmp"
mkdir -p "$SERVER_TMP" "$CLIENT_TMP"
SERVER_LOG="$OUT_DIR/server.log"
CLIENT_LOG="$OUT_DIR/client.log"
GATEWAY_A_LOG="$OUT_DIR/gateway_a.log"
GATEWAY_B_LOG="$OUT_DIR/gateway_b.log"
SERVER_JOURNAL="$SERVER_TMP/journal.jsonl"
CLIENT_JOURNAL="$CLIENT_TMP/journal.jsonl"
rm -f "$SERVER_LOG" "$CLIENT_LOG" "$GATEWAY_A_LOG" "$GATEWAY_B_LOG" "$SERVER_JOURNAL" "$CLIENT_JOURNAL"

echo "Out dir  : $OUT_DIR"
echo "Target   : $TARGET"
echo "Gateway A: :$GATEWAY_A_HTTP_PORT (relay :$GATEWAY_A_RELAY_PORT)"
echo "Gateway B: :$GATEWAY_B_HTTP_PORT (relay :$GATEWAY_B_RELAY_PORT)"

if [ "$BUILD_GATEWAY" != "0" ]; then
    echo "::group::Building webrtc-gateway"
    if ! command -v "$GO" >/dev/null 2>&1 && [ ! -x "$GO" ]; then
        echo "FAIL: no go toolchain found (set GO=/path/to/go)"
        exit 2
    fi
    (cd "$GATEWAY_DIR" && "$GO" build -o gateway .) || {
        echo "FAIL: gateway build failed"
        exit 2
    }
    echo "::endgroup::"
fi

if [ "${BUILD:-0}" != "0" ]; then
    echo "::group::Building $TARGET"
    BUILD_HOST_TOOLS=0 COFFEE_DISABLE_PROFILER=1 ./cb build "$TARGET" || {
        echo "FAIL: build failed"
        exit 2
    }
    echo "::endgroup::"
fi

GWA_PID=""
GWB_PID=""
SERVER_PID=""
CLIENT_PID=""
cleanup() {
    for pid in "$CLIENT_PID" "$SERVER_PID" "$GWA_PID" "$GWB_PID"; do
        [ -n "$pid" ] && kill "$pid" 2>/dev/null
    done
}
trap cleanup EXIT

# Wait for a TCP port to accept connections (gateway HTTP/WebSocket
# listeners) -- more reliable than grepping the "listening on" log line,
# which main.go prints just before ListenAndServe, not necessarily after
# the socket is actually bound.
wait_for_port() {
    local port="$1" timeout="$2" pid="$3" label="$4"
    for _ in $(seq 1 "$timeout"); do
        if (exec 3<>"/dev/tcp/127.0.0.1/$port") 2>/dev/null; then
            exec 3>&- 3<&-
            return 0
        fi
        if ! kill -0 "$pid" 2>/dev/null; then
            echo "FAIL: $label exited before its port opened"
            return 1
        fi
        sleep 1
    done
    echo "FAIL: $label did not open its port within ${timeout}s"
    return 1
}

echo "Starting gateway A..."
"$GATEWAY_BIN" -listen ":$GATEWAY_A_HTTP_PORT" \
    -dest "127.0.0.1:$GATEWAY_B_RELAY_PORT" -relay-udp-port "$GATEWAY_A_RELAY_PORT" \
    > "$GATEWAY_A_LOG" 2>&1 &
GWA_PID=$!

echo "Starting gateway B..."
"$GATEWAY_BIN" -listen ":$GATEWAY_B_HTTP_PORT" \
    -dest "127.0.0.1:$GATEWAY_A_RELAY_PORT" -relay-udp-port "$GATEWAY_B_RELAY_PORT" \
    > "$GATEWAY_B_LOG" 2>&1 &
GWB_PID=$!

wait_for_port "$GATEWAY_A_HTTP_PORT" "$BOOT_TIMEOUT" "$GWA_PID" "gateway A" || { tail -n 40 "$GATEWAY_A_LOG"; exit 1; }
wait_for_port "$GATEWAY_B_HTTP_PORT" "$BOOT_TIMEOUT" "$GWB_PID" "gateway B" || { tail -n 40 "$GATEWAY_B_LOG"; exit 1; }
echo "Both gateways are listening."

BASE_RUN_ARGS=(run "$TARGET" -- "$RESOURCE_DIR" "$MAP")
SERVER_SIGNAL_URL="ws://127.0.0.1:$GATEWAY_A_HTTP_PORT"
DATACHANNEL_GATEWAY_URL="ws://127.0.0.1:$GATEWAY_B_HTTP_PORT"
CLIENT_URL="ws://127.0.0.1:$GATEWAY_A_HTTP_PORT"

echo "Starting server (--listen $SERVER_SIGNAL_URL#$DATACHANNEL_GATEWAY_URL)..."
TMPDIR="$SERVER_TMP" \
DUMMY_PLUG_CONFIG="$SERVER_DUMMY_PLUG_CONFIG" \
COFFEE_DISABLE_PROFILER=1 \
timeout "${RUN_TIMEOUT}s" ./cb "${BASE_RUN_ARGS[@]}" --listen "${SERVER_SIGNAL_URL}#${DATACHANNEL_GATEWAY_URL}" \
    > "$SERVER_LOG" 2>&1 &
SERVER_PID=$!

echo "Waiting for server to register with gateway A's /server-signal (up to ${BOOT_TIMEOUT}s)..."
booted=0
for _ in $(seq 1 "$BOOT_TIMEOUT"); do
    if grep -q "registered with gateway /server-signal" "$SERVER_LOG" 2>/dev/null; then
        booted=1
        break
    fi
    if ! kill -0 "$SERVER_PID" 2>/dev/null; then
        echo "FAIL: server process exited before registering with the gateway"
        echo "--- server.log tail ---"
        tail -n 40 "$SERVER_LOG"
        exit 1
    fi
    sleep 1
done
if [ "$booted" != "1" ]; then
    echo "FAIL: server did not register within ${BOOT_TIMEOUT}s"
    tail -n 40 "$SERVER_LOG"
    exit 1
fi
echo "Server is registered."

echo "Starting client (--server $CLIENT_URL)..."
TMPDIR="$CLIENT_TMP" \
DUMMY_PLUG_CONFIG="$CLIENT_DUMMY_PLUG_CONFIG" \
COFFEE_DISABLE_PROFILER=1 \
timeout "${RUN_TIMEOUT}s" ./cb "${BASE_RUN_ARGS[@]}" --server "$CLIENT_URL" \
    > "$CLIENT_LOG" 2>&1 &
CLIENT_PID=$!

echo "Waiting for both processes to finish (dummy_plug end_time closes them)..."
wait "$CLIENT_PID"
CLIENT_EXIT=$?
wait "$SERVER_PID"
SERVER_EXIT=$?

echo "server exit=$SERVER_EXIT  client exit=$CLIENT_EXIT"
if [ "$SERVER_EXIT" = "124" ] || [ "$CLIENT_EXIT" = "124" ]; then
    echo "FAIL: a process hit the ${RUN_TIMEOUT}s timeout (hung -- likely connection never established)"
fi

echo
echo "::group::gateway_a.log"
cat "$GATEWAY_A_LOG" || true
echo "::endgroup::"
echo "::group::gateway_b.log"
cat "$GATEWAY_B_LOG" || true
echo "::endgroup::"
echo "::group::server.log (connection + state-dump lines)"
grep -E "State dumped|registered with gateway|WebRTC gateway|Connection info|Player joined|Controller|Error|error" "$SERVER_LOG" || true
echo "::endgroup::"
echo "::group::client.log (connection + state-dump lines)"
grep -E "State dumped|join confirmation|roster received|WebRTC|Error|error" "$CLIENT_LOG" || true
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
