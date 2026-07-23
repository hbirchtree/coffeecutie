#!/bin/bash
# webrtc-gateway fleet-registration (Phase 6) integration test.
#
# Unlike run_webrtc_client_server_test.sh / run_webrtc_web_client_test.sh
# (topology "two gateways bridging two WebRTC-only peers", CI-testing
# only), this exercises the *other* topology those scripts' own header
# comments name but don't cover: "browser <-> gateway <-> native UDP
# server" -- a real game server with an ordinary bound GNS UDP listen
# socket (--listen 127.0.0.1:<port>), additionally registering itself
# with a single shared gateway's fleet registry (--gateway-register) so
# a browser can be routed to it via /signal?server=<id> instead of the
# gateway being locked to one -dest at startup. See
# examples/blam/cblam-testing/WEBRTC_TRANSPORT.md's Phase 6 section for
# the full registration/challenge-response protocol.
#
# Only one gateway process here (not two) -- there's no second WebRTC peer
# to bridge to, the far end is a plain UDP socket.
#
# Reuses webrtc_client_smoke.mjs (the wasm/Playwright driver already
# proven against the two-gateway topology) rather than a second
# browser-less test client -- see the plan doc's reasoning: exercising the
# registry/challenge logic (entirely gateway + native-server side) through
# the client path that's already known to speak WebRTC signaling
# correctly is both simpler and closer to real usage.
#
# Topology:
#   client(wasm,browser) --ws(/signal?server=<id>)--> gateway --UDP--> native server (--listen)
#                          gateway <--ws(/server-signal "register")-- server (persistent: register, heartbeat, challenge-response)
#
# Not CI-wired (manual only, per the implementation plan -- that's its own
# later Phase 7 pass): no dummy_plug/Journal comparison either, since the
# wasm client can't use it (see run_webrtc_web_client_test.sh's header for
# why) -- pass/fail comes entirely from webrtc_client_smoke.mjs's console
# marker scan, same as that script.
#
# Usage:
#   run_webrtc_gateway_fleet_test.sh [BUNDLE_DIR] [OUT_DIR]
#
# BUNDLE_DIR  directory containing BlamGraphics.html (the emscripten bundle).
#             Default: auto-detect under multi_build/web-*emscripten* (same
#             search as run_webrtc_web_client_test.sh).
#
# Env overrides:
#   TARGET        native server cb build/run target (default: desktop:x86_64-buildroot-linux-gnu:multi/BlamGraphics)
#   RESOURCE_DIR  asset dir passed to the server     (default: multi_build/desktop-x86_64-buildroot-linux-gnu-multi/examples/blam/cblam-testing/assets/)
#   MAP           map file passed to the server      (default: /mnt/blam/pc/bloodgulch.map)
#   GATEWAY_HTTP_PORT  gateway WebSocket signaling port (default: 8098)
#   SERVER_UDP_PORT    real GNS UDP listen port the server binds and registers as its dest (default: 19601)
#   SERVER_ID          fleet registry serverId to register/connect under (default: test)
#   BOOT_TIMEOUT  seconds to wait for the gateway / server registration (default: 30)
#   RUN_TIMEOUT   hard cap on the server process, via `timeout` (default: 150 -- see
#                 run_webrtc_web_client_test.sh's RUN_TIMEOUT note, same reasoning)
#   BOOT_TIMEOUT_MS / CONNECT_TIMEOUT_MS  passed through to webrtc_client_smoke.mjs (defaults there: 45000 / 60000)
#   BUILD         set to 1 to build the native server target using `./cb build` first
#   BUILD_GATEWAY set to 1 to `go build` the gateway first (default: 0, same opt-in
#                 convention as the other webrtc test scripts)
#   SERVER_DUMMY_PLUG_CONFIG  path to the server's dummy_plug config
#                 (default: dummy_plug_net_webrtc_server_for_web_client.json --
#                 reused as-is, this scenario needs nothing different from it)
#   GO            go binary to use for BUILD_GATEWAY (default: go, falling back to ~/local/go/bin/go)

set -uo pipefail
HERE="$(cd "$(dirname "$0")" && pwd)"
SRCDIR="$(cd "$HERE/../../.." && pwd)"
GATEWAY_DIR="$SRCDIR/tools/webrtc-gateway"
cd "$SRCDIR"

BUNDLE_DIR="${1:-}"
OUT_DIR="${2:-/tmp/webrtc_gateway_fleet_test}"

if [ -z "$BUNDLE_DIR" ]; then
    BUNDLE_DIR="$(find "$SRCDIR/multi_build" -type d -name BlamGraphics.bundle 2>/dev/null \
        | grep -E 'web-.*emscripten' | grep -v '\.web_scratch' | head -n 1)"
fi
if [ -z "$BUNDLE_DIR" ] || [ ! -f "$BUNDLE_DIR/BlamGraphics.html" ]; then
    echo "ERROR: could not find a BlamGraphics.bundle. Pass BUNDLE_DIR explicitly." >&2
    exit 2
fi
BUNDLE_DIR="$(realpath "$BUNDLE_DIR")"
mkdir -p "$OUT_DIR"
OUT_DIR="$(realpath "$OUT_DIR")"

TARGET="${TARGET:-desktop:x86_64-buildroot-linux-gnu:multi/BlamGraphics}"
RESOURCE_DIR="${RESOURCE_DIR:-multi_build/desktop-x86_64-buildroot-linux-gnu-multi/examples/blam/cblam-testing/assets/}"
MAP="${MAP:-/mnt/blam/pc/bloodgulch.map}"

# Same reasoning as run_webrtc_web_client_test.sh: webrtc_client_smoke.mjs
# only serves BUNDLE_DIR itself, so symlink whatever .map files sit
# alongside $MAP into it.
for mapfile in "$(dirname "$MAP")"/*.map; do
    [ -f "$mapfile" ] && ln -sf "$mapfile" "$BUNDLE_DIR/$(basename "$mapfile")"
done

GATEWAY_HTTP_PORT="${GATEWAY_HTTP_PORT:-8098}"
SERVER_UDP_PORT="${SERVER_UDP_PORT:-19601}"
SERVER_ID="${SERVER_ID:-test}"
BOOT_TIMEOUT="${BOOT_TIMEOUT:-30}"
RUN_TIMEOUT="${RUN_TIMEOUT:-150}"

SERVER_DUMMY_PLUG_CONFIG="${SERVER_DUMMY_PLUG_CONFIG:-$HERE/dummy_plug_net_webrtc_server_for_web_client.json}"

GO="${GO:-go}"
if ! command -v "$GO" >/dev/null 2>&1; then
    if [ -x "$HOME/local/go/bin/go" ]; then
        GO="$HOME/local/go/bin/go"
    fi
fi

GATEWAY_BIN="$GATEWAY_DIR/gateway"
BUILD_GATEWAY="${BUILD_GATEWAY:-0}"

SERVER_TMP="$OUT_DIR/server_tmp"
mkdir -p "$SERVER_TMP"
SERVER_LOG="$OUT_DIR/server.log"
GATEWAY_LOG="$OUT_DIR/gateway.log"
SERVER_JOURNAL="$SERVER_TMP/journal.jsonl"
rm -f "$SERVER_LOG" "$GATEWAY_LOG" "$SERVER_JOURNAL"

echo "Bundle   : $BUNDLE_DIR"
echo "Out dir  : $OUT_DIR"
echo "Target   : $TARGET"
echo "Gateway  : :$GATEWAY_HTTP_PORT"
echo "Server   : udp 127.0.0.1:$SERVER_UDP_PORT, serverId=$SERVER_ID"

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

echo "::group::Installing NPM/Playwright dependencies"
(
    cd "$HERE/../web"
    if [ ! -d node_modules/playwright ]; then
        echo "Installing playwright..."
        npm install --no-audit --no-fund
    fi
    if [ "${CI:-}" = "true" ]; then
        npx playwright install --with-deps chromium
    else
        npx playwright install chromium
    fi
)
echo "::endgroup::"

GW_PID=""
SERVER_PID=""
cleanup() {
    for pid in "$SERVER_PID" "$GW_PID"; do
        [ -n "$pid" ] && kill "$pid" 2>/dev/null
    done
}
trap cleanup EXIT

# Same port-poll helper as the other webrtc test scripts.
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

echo "Starting gateway (no -dest -- pure fleet-registry mode)..."
"$GATEWAY_BIN" -listen ":$GATEWAY_HTTP_PORT" > "$GATEWAY_LOG" 2>&1 &
GW_PID=$!

wait_for_port "$GATEWAY_HTTP_PORT" "$BOOT_TIMEOUT" "$GW_PID" "gateway" || { tail -n 40 "$GATEWAY_LOG"; exit 1; }
echo "Gateway is listening."

GATEWAY_URL="ws://127.0.0.1:$GATEWAY_HTTP_PORT"

echo "Starting native server (--listen 127.0.0.1:$SERVER_UDP_PORT --gateway-register $GATEWAY_URL --gateway-server-id $SERVER_ID)..."
TMPDIR="$SERVER_TMP" \
DUMMY_PLUG_CONFIG="$SERVER_DUMMY_PLUG_CONFIG" \
COFFEE_DISABLE_PROFILER=1 \
timeout "${RUN_TIMEOUT}s" ./cb run "$TARGET" -- "$RESOURCE_DIR" "$MAP" \
    --listen "127.0.0.1:$SERVER_UDP_PORT" \
    --gateway-register "$GATEWAY_URL" \
    --gateway-server-id "$SERVER_ID" \
    > "$SERVER_LOG" 2>&1 &
SERVER_PID=$!

echo "Waiting for the registration challenge to pass (up to ${BOOT_TIMEOUT}s)..."
booted=0
for _ in $(seq 1 "$BOOT_TIMEOUT"); do
    if grep -q "registration active for serverId=$SERVER_ID" "$SERVER_LOG" 2>/dev/null; then
        booted=1
        break
    fi
    if ! kill -0 "$SERVER_PID" 2>/dev/null; then
        echo "FAIL: server process exited before its registration became active"
        echo "--- server.log tail ---"
        tail -n 40 "$SERVER_LOG"
        exit 1
    fi
    sleep 1
done
if [ "$booted" != "1" ]; then
    echo "FAIL: registration did not become active within ${BOOT_TIMEOUT}s"
    tail -n 40 "$SERVER_LOG"
    exit 1
fi
echo "Server registration is active."

echo "Starting wasm client (Playwright, --server $GATEWAY_URL#$SERVER_ID)..."
BUNDLE_DIR="$BUNDLE_DIR" \
OUT_DIR="$OUT_DIR/client" \
SERVER_URL="${GATEWAY_URL}#${SERVER_ID}" \
BOOT_TIMEOUT_MS="${BOOT_TIMEOUT_MS:-45000}" \
CONNECT_TIMEOUT_MS="${CONNECT_TIMEOUT_MS:-60000}" \
node "$HERE/../web/webrtc_client_smoke.mjs"
CLIENT_EXIT=$?

# Same reasoning as run_webrtc_web_client_test.sh: don't block on the
# server's own dummy_plug end_time, the client's console markers are the
# pass/fail signal here.
SERVER_EXIT=0
if ! kill -0 "$SERVER_PID" 2>/dev/null; then
    wait "$SERVER_PID"
    SERVER_EXIT=$?
fi

echo "server exit=$SERVER_EXIT  client exit=$CLIENT_EXIT"
if [ "$SERVER_EXIT" = "124" ]; then
    echo "FAIL: server hit the ${RUN_TIMEOUT}s timeout"
fi

echo
echo "::group::gateway.log"
cat "$GATEWAY_LOG" || true
echo "::endgroup::"
echo "::group::server.log (registration + connection lines)"
grep -E "gateway_fleet_registration|WebRTC gateway|Connection info|Player joined|Controller|Error|error" "$SERVER_LOG" || true
echo "::endgroup::"
echo "::group::client (browser console) output.log"
cat "$OUT_DIR/client/output.log" 2>/dev/null || true
echo "::endgroup::"

if [ "$CLIENT_EXIT" != "0" ]; then
    echo "FAIL: wasm client did not complete the connect/join/roster-sync sequence (see client output.log above)"
    exit 1
fi
if [ "$SERVER_EXIT" = "124" ]; then
    exit 1
fi
echo "PASS"
exit 0
