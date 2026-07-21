#!/bin/bash
# Wasm-client-through-the-gateway-to-native-server integration test.
#
# Same topology as .github/tests/net/run_webrtc_client_server_test.sh
# (two webrtc-gateway processes bridging a client and a server, see that
# script's header + examples/blam/cblam-testing/WEBRTC_TRANSPORT.md), but
# the client half is a headless-Chromium wasm BlamGraphics build driven by
# Playwright (webrtc_client_smoke.mjs) instead of a second native process
# -- the actual target scenario for this whole effort.
#
# Verification differs from the native-to-native test too: the wasm client
# can't use dummy_plug/Journal (comp_app::dummy_plug is compiled out on
# emscripten, see .github/tests/web/README.md's Tier 2 section), so there's
# no client-side journal to diff. Success is decided by webrtc_client_smoke.mjs
# scanning the browser's own console output for the connect/join/roster-sync
# markers instead -- see that script for the exact regexes. The server's
# own journal is still written (native side keeps working as before) but
# isn't compared against anything here.
#
# Topology:
#   client(wasm,browser) --ws(/signal)--> gateway A <--UDP(relay-udp-port)--> gateway B <--ws(/signal)-- server
#                                          gateway A <--ws(/server-signal)-- server (persistent, GNS rendezvous only)
#
# Usage:
#   run_webrtc_web_client_test.sh [BUNDLE_DIR] [OUT_DIR]
#
# BUNDLE_DIR  directory containing BlamGraphics.html (the emscripten bundle).
#             Default: auto-detect under multi_build/web-*emscripten* (same
#             search as .github/tests/web/run_webgl_test.sh).
#
# Env overrides:
#   TARGET        native server cb build/run target (default: desktop:x86_64-buildroot-linux-gnu:multi/BlamGraphics)
#   RESOURCE_DIR  asset dir passed to the server     (default: multi_build/desktop-x86_64-buildroot-linux-gnu-multi/examples/blam/cblam-testing/assets/)
#   MAP           map file passed to the server      (default: /mnt/blam/pc/bloodgulch.map)
#   SERVER_BINARY path to a pre-built BlamGraphics binary to run DIRECTLY,
#                 bypassing `./cb run $TARGET` entirely. For CI: a fresh
#                 checkout has no local multi_build/<preset>/ build tree
#                 (`cb run` needs one -- it just looks for a binary in
#                 multi_build/<preset>/bin, no CMake cache required, but
#                 that directory has to exist), so the downloaded release
#                 artifact is run directly instead, the same way
#                 .github/tests/test_blam_graphics.sh already does for the
#                 single-process software-render smoke test. Unset by
#                 default -- local dev keeps using `./cb run`.
#   SERVER_LD     path to a specific dynamic linker/loader (ld-linux...) to
#                 invoke SERVER_BINARY through, e.g. the one bundled in a
#                 downloaded cross-toolchain sysroot tarball. Matches
#                 .github/tests/test_blam_graphics.sh's own proven approach:
#                 run the whole stack (loader, libc, Mesa/EGL/the llvmpipe
#                 DRI driver) from ONE consistent sysroot rather than
#                 mixing in the CI runner's own system Mesa -- tried
#                 running SERVER_BINARY directly with just the runner's
#                 own loader once, and every eglCreateContext call came
#                 back with degenerate configs (EGL_RENDERABLE_TYPE=0, no
#                 surface types), unlike the sysroot-based harness which
#                 is already proven to work headless in this exact CI
#                 environment. If unset, SERVER_BINARY runs as an ordinary
#                 executable (relying on the runner's own loader) --
#                 fine for local dev, where the runner's own Mesa isn't
#                 in play.
#   SERVER_LIB_PATH  colon-separated library search path for SERVER_BINARY
#                 (passed to SERVER_LD's --library-path if set, otherwise
#                 exported as LD_LIBRARY_PATH -- scoped to just the server
#                 process either way, see the comment at its use site
#                 below). Only used when SERVER_BINARY is set.
#   GATEWAY_A_HTTP_PORT / GATEWAY_B_HTTP_PORT   gateway WebSocket signaling ports (default: 8098 / 8099)
#   GATEWAY_A_RELAY_PORT / GATEWAY_B_RELAY_PORT fixed UDP relay ports bridging the two gateways (default: 19501 / 19502)
#   BOOT_TIMEOUT  seconds to wait for gateways / server registration (default: 30)
#   RUN_TIMEOUT   hard cap on the server process, via `timeout` (default: 150 -- must stay
#                 above dummy_plug_net_webrtc_server_for_web_client.json's end_time (140s),
#                 which is sized for a cold wasm boot on a slow CI runner; keep the two in
#                 sync if either changes)
#   BOOT_TIMEOUT_MS / CONNECT_TIMEOUT_MS  passed through to webrtc_client_smoke.mjs (defaults there: 45000 / 60000)
#   BUILD         set to 1 to build the native server target using `./cb build` first
#   BUILD_GATEWAY set to 1 to `go build` the gateway first (default: 0, same opt-in
#                 convention as BUILD -- see run_webrtc_client_server_test.sh)
#   SERVER_DUMMY_PLUG_CONFIG  path to the server's dummy_plug config
#                 (default: dummy_plug_net_webrtc_server_for_web_client.json)
#   GO            go binary to use for BUILD_GATEWAY (default: go, falling back to ~/local/go/bin/go)

set -uo pipefail
HERE="$(cd "$(dirname "$0")" && pwd)"
SRCDIR="$(cd "$HERE/../../.." && pwd)"
NET_TEST_DIR="$SRCDIR/.github/tests/net"
GATEWAY_DIR="$SRCDIR/tools/webrtc-gateway"
cd "$SRCDIR"

BUNDLE_DIR="${1:-}"
OUT_DIR="${2:-/tmp/webrtc_web_client_test}"

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
GATEWAY_A_HTTP_PORT="${GATEWAY_A_HTTP_PORT:-8098}"
GATEWAY_B_HTTP_PORT="${GATEWAY_B_HTTP_PORT:-8099}"
GATEWAY_A_RELAY_PORT="${GATEWAY_A_RELAY_PORT:-19501}"
GATEWAY_B_RELAY_PORT="${GATEWAY_B_RELAY_PORT:-19502}"
BOOT_TIMEOUT="${BOOT_TIMEOUT:-30}"
RUN_TIMEOUT="${RUN_TIMEOUT:-150}"

SERVER_DUMMY_PLUG_CONFIG="${SERVER_DUMMY_PLUG_CONFIG:-$NET_TEST_DIR/dummy_plug_net_webrtc_server_for_web_client.json}"

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
GATEWAY_A_LOG="$OUT_DIR/gateway_a.log"
GATEWAY_B_LOG="$OUT_DIR/gateway_b.log"
SERVER_JOURNAL="$SERVER_TMP/journal.jsonl"
rm -f "$SERVER_LOG" "$GATEWAY_A_LOG" "$GATEWAY_B_LOG" "$SERVER_JOURNAL"

echo "Bundle   : $BUNDLE_DIR"
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

echo "::group::Installing NPM/Playwright dependencies"
(
    cd "$HERE"
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

GWA_PID=""
GWB_PID=""
SERVER_PID=""
cleanup() {
    for pid in "$SERVER_PID" "$GWA_PID" "$GWB_PID"; do
        [ -n "$pid" ] && kill "$pid" 2>/dev/null
    done
}
trap cleanup EXIT

# Same port-poll helper as run_webrtc_client_server_test.sh -- more
# reliable than grepping the "listening on" log line.
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

SERVER_SIGNAL_URL="ws://127.0.0.1:$GATEWAY_A_HTTP_PORT"
DATACHANNEL_GATEWAY_URL="ws://127.0.0.1:$GATEWAY_B_HTTP_PORT"
CLIENT_URL="ws://127.0.0.1:$GATEWAY_A_HTTP_PORT"

if [ -n "${SERVER_BINARY:-}" ]; then
    if [ -n "${SERVER_LD:-}" ]; then
        SERVER_CMD=("$SERVER_LD" --library-path "${SERVER_LIB_PATH:-}" "$SERVER_BINARY")
    else
        SERVER_CMD=("$SERVER_BINARY")
    fi
    SERVER_CMD+=("$RESOURCE_DIR" "$MAP" --listen "${SERVER_SIGNAL_URL}#${DATACHANNEL_GATEWAY_URL}")
else
    SERVER_CMD=(./cb run "$TARGET" -- "$RESOURCE_DIR" "$MAP" --listen "${SERVER_SIGNAL_URL}#${DATACHANNEL_GATEWAY_URL}")
fi

SERVER_ENV_CMD=()
if [ -n "${SERVER_LIB_PATH:-}" ]; then
    SERVER_ENV_CMD=(env "LD_LIBRARY_PATH=$SERVER_LIB_PATH")
fi

echo "Starting native server (--listen $SERVER_SIGNAL_URL#$DATACHANNEL_GATEWAY_URL)..."
TMPDIR="$SERVER_TMP" \
DUMMY_PLUG_CONFIG="$SERVER_DUMMY_PLUG_CONFIG" \
COFFEE_DISABLE_PROFILER=1 \
timeout "${RUN_TIMEOUT}s" "${SERVER_ENV_CMD[@]}" "${SERVER_CMD[@]}" \
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

echo "Starting wasm client (Playwright, --server $CLIENT_URL)..."
BUNDLE_DIR="$BUNDLE_DIR" \
OUT_DIR="$OUT_DIR/client" \
SERVER_URL="$CLIENT_URL" \
BOOT_TIMEOUT_MS="${BOOT_TIMEOUT_MS:-45000}" \
CONNECT_TIMEOUT_MS="${CONNECT_TIMEOUT_MS:-60000}" \
node "$HERE/webrtc_client_smoke.mjs"
CLIENT_EXIT=$?

# Don't block on the server's own dummy_plug end_time (140s, sized for a
# worst-case slow CI boot -- see dummy_plug_net_webrtc_server_for_web_client.json's
# comment): this test's pass/fail signal comes entirely from the client's
# console markers (webrtc_client_smoke.mjs), not from anything the server
# produces at its own end_time. If the server already exited on its own by
# now (crashed, or hit RUN_TIMEOUT) that's still worth surfacing; otherwise
# leave it running and let the EXIT trap (cleanup()) kill it once this
# script exits, right after the log-dumping below.
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
echo "::group::gateway_a.log"
cat "$GATEWAY_A_LOG" || true
echo "::endgroup::"
echo "::group::gateway_b.log"
cat "$GATEWAY_B_LOG" || true
echo "::endgroup::"
echo "::group::server.log (connection lines)"
grep -E "registered with gateway|WebRTC gateway|Connection info|Player joined|Controller|Error|error" "$SERVER_LOG" || true
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
