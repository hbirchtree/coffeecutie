#!/bin/bash
# Wasm-client-through-the-gateway-to-native-server integration test.
#
# One local webrtc-gateway, a fleet-registered native server behind it,
# and a headless-Chromium wasm BlamGraphics client driven by Playwright
# (webrtc_client_smoke.mjs) -- the actual target scenario for this whole
# effort. See examples/blam/cblam-testing/WEBRTC_TRANSPORT.md. The
# harness itself is shared with the other webrtc tests
# (.github/tests/net/webrtc_test_lib.sh); the remote-gateway/NAT variant
# is .github/tests/net/run_webrtc_nat_punch_test.sh.
#
# Verification differs from the native-to-native test: the wasm client
# can't use dummy_plug/Journal (comp_app::dummy_plug is compiled out on
# emscripten, see .github/tests/web/README.md's Tier 2 section), so
# there's no client-side journal to diff. Success is decided by
# webrtc_client_smoke.mjs scanning the browser's own console output for
# the connect/join/roster-sync markers instead -- see that script for the
# exact regexes. The server's own journal is still written but isn't
# compared against anything here.
#
# Topology:
#   client(wasm,browser) --ws(/signal?server=<id>)--> gateway --UDP--> server (--listen)
#                                                     gateway <--ws(/server-signal)-- server (register, heartbeat, punches)
#
# Usage:
#   run_webrtc_web_client_test.sh [BUNDLE_DIR] [OUT_DIR]
#
# BUNDLE_DIR  directory containing BlamGraphics.html (the emscripten bundle).
#             Default: auto-detect under multi_build/web-*emscripten*.
#
# Env overrides:
#   TARGET        native server cb build/run target (default: desktop:x86_64-buildroot-linux-gnu:multi/BlamGraphics)
#   RESOURCE_DIR  asset dir passed to the server     (default: multi_build/desktop-x86_64-buildroot-linux-gnu-multi/examples/blam/cblam-testing/assets/)
#   MAP           map file passed to the server      (default: /mnt/blam/pc/bloodgulch.map)
#   SERVER_BINARY path to a pre-built BlamGraphics binary to run DIRECTLY,
#                 bypassing `./cb run $TARGET` entirely. For CI: a fresh
#                 checkout has no local multi_build/<preset>/ build tree
#                 (`cb run` needs one), so the downloaded release artifact
#                 is run directly instead, the same way
#                 .github/tests/test_blam_graphics.sh already does.
#   SERVER_LD     path to a specific dynamic linker/loader (ld-linux...) to
#                 invoke SERVER_BINARY through, e.g. the one bundled in a
#                 downloaded cross-toolchain sysroot tarball. Matches
#                 .github/tests/test_blam_graphics.sh's proven approach:
#                 run the whole stack (loader, libc, Mesa/EGL/the llvmpipe
#                 DRI driver) from ONE consistent sysroot rather than
#                 mixing in the CI runner's own system Mesa -- tried the
#                 runner's own loader once, and every eglCreateContext
#                 came back with degenerate configs (EGL_RENDERABLE_TYPE=0).
#   SERVER_LIB_PATH  colon-separated library search path for SERVER_BINARY
#                 (SERVER_LD's --library-path if set, otherwise exported as
#                 LD_LIBRARY_PATH, scoped to the server process either way).
#   GATEWAY_HTTP_PORT  gateway WebSocket signaling port (default: 8098)
#   SERVER_UDP_PORT    real GNS UDP listen port the server binds (default: 19601)
#   SERVER_ID          fleet registry serverId to register/connect under (default: test)
#   BOOT_TIMEOUT  seconds to wait for the gateway / server registration (default: 30)
#   RUN_TIMEOUT   hard cap on the server process, via `timeout` (default: 150 -- must stay
#                 above dummy_plug_net_webrtc_server_for_web_client.json's end_time (140s),
#                 which is sized for a cold wasm boot on a slow CI runner; keep the two in
#                 sync if either changes)
#   BOOT_TIMEOUT_MS / CONNECT_TIMEOUT_MS  passed through to webrtc_client_smoke.mjs (defaults there: 45000 / 60000)
#   BUILD         set to 1 to build the native server target using `./cb build` first
#   BUILD_GATEWAY set to 1 to `go build` the gateway first (default: 0, opt-in
#                 like BUILD -- the caller owns keeping the binary current)
#   SERVER_DUMMY_PLUG_CONFIG  path to the server's dummy_plug config
#                 (default: dummy_plug_net_webrtc_server_for_web_client.json)
#   GO            go binary to use for BUILD_GATEWAY (default: go, falling back to ~/local/go/bin/go)

set -uo pipefail
HERE="$(cd "$(dirname "$0")" && pwd)"
SRCDIR="$(cd "$HERE/../../.." && pwd)"
NET_TEST_DIR="$SRCDIR/.github/tests/net"
GATEWAY_DIR="$SRCDIR/tools/webrtc-gateway"
# shellcheck source=.github/tests/net/webrtc_test_lib.sh
. "$NET_TEST_DIR/webrtc_test_lib.sh"
cd "$SRCDIR"

BUNDLE_DIR="$(webrtc_find_bundle "$SRCDIR" "${1:-}")" || exit 2
OUT_DIR="${2:-/tmp/webrtc_web_client_test}"
mkdir -p "$OUT_DIR"
OUT_DIR="$(realpath "$OUT_DIR")"

TARGET="${TARGET:-desktop:x86_64-buildroot-linux-gnu:multi/BlamGraphics}"
RESOURCE_DIR="${RESOURCE_DIR:-multi_build/desktop-x86_64-buildroot-linux-gnu-multi/examples/blam/cblam-testing/assets/}"
MAP="${MAP:-/mnt/blam/pc/bloodgulch.map}"
GATEWAY_HTTP_PORT="${GATEWAY_HTTP_PORT:-8098}"
SERVER_UDP_PORT="${SERVER_UDP_PORT:-19601}"
SERVER_ID="${SERVER_ID:-test}"
BOOT_TIMEOUT="${BOOT_TIMEOUT:-30}"
RUN_TIMEOUT="${RUN_TIMEOUT:-150}"
SERVER_DUMMY_PLUG_CONFIG="${SERVER_DUMMY_PLUG_CONFIG:-$NET_TEST_DIR/dummy_plug_net_webrtc_server_for_web_client.json}"

webrtc_link_maps "$MAP" "$BUNDLE_DIR"

GATEWAY_BIN="$GATEWAY_DIR/gateway"
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

webrtc_build_gateway "$GATEWAY_DIR" "$(webrtc_find_go)" || exit 2

if [ "${BUILD:-0}" != "0" ]; then
    echo "::group::Building $TARGET"
    BUILD_HOST_TOOLS=0 COFFEE_DISABLE_PROFILER=1 ./cb build "$TARGET" || {
        echo "FAIL: build failed"
        echo "::endgroup::"
        exit 2
    }
    echo "::endgroup::"
fi

webrtc_install_playwright "$HERE"

cleanup() {
    for pid in "${WEBRTC_SERVER_PID:-}" "${WEBRTC_GW_PID:-}"; do
        [ -n "$pid" ] && kill "$pid" 2>/dev/null
    done
}
trap cleanup EXIT

webrtc_start_gateway "$GATEWAY_BIN" "$GATEWAY_HTTP_PORT" "$GATEWAY_LOG" "$BOOT_TIMEOUT" || exit 1

GATEWAY_URL="ws://127.0.0.1:$GATEWAY_HTTP_PORT"

echo "Starting native server (--listen 127.0.0.1:$SERVER_UDP_PORT --gateway-register $GATEWAY_URL --gateway-server-id $SERVER_ID)..."
webrtc_server_command "$TARGET" \
    "$RESOURCE_DIR" "$MAP" \
    --listen "127.0.0.1:$SERVER_UDP_PORT" \
    --gateway-register "$GATEWAY_URL" \
    --gateway-server-id "$SERVER_ID"
webrtc_start_server "$SERVER_LOG" "$SERVER_TMP" "$SERVER_DUMMY_PLUG_CONFIG" "$RUN_TIMEOUT"

webrtc_wait_for_registration "$SERVER_LOG" "$SERVER_ID" "$BOOT_TIMEOUT" "$WEBRTC_SERVER_PID" || {
    webrtc_dump "gateway.log" "$GATEWAY_LOG"
    exit 1
}

echo "Starting wasm client (Playwright, $GATEWAY_URL#$SERVER_ID)..."
webrtc_run_wasm_client "$BUNDLE_DIR" "$OUT_DIR/client" \
    "${GATEWAY_URL}#${SERVER_ID}" "$HERE/webrtc_client_smoke.mjs"
CLIENT_EXIT=$?

# Don't block on the server's own dummy_plug end_time (140s, sized for a
# worst-case slow CI boot): this test's pass/fail signal comes entirely
# from the client's console markers, not from anything the server produces
# at its end_time. If the server already exited on its own (crashed, or
# hit RUN_TIMEOUT) that's still worth surfacing; otherwise leave it
# running and let the EXIT trap kill it after the logs below.
SERVER_EXIT=0
if ! kill -0 "$WEBRTC_SERVER_PID" 2>/dev/null; then
    wait "$WEBRTC_SERVER_PID"
    SERVER_EXIT=$?
fi

echo "server exit=$SERVER_EXIT  client exit=$CLIENT_EXIT"
if [ "$SERVER_EXIT" = "124" ]; then
    echo "FAIL: server hit the ${RUN_TIMEOUT}s timeout"
fi

echo
webrtc_dump "gateway.log" "$GATEWAY_LOG"
webrtc_dump_server_lines "$SERVER_LOG"
webrtc_dump "client (browser console) output.log" "$OUT_DIR/client/output.log"

if [ "$CLIENT_EXIT" != "0" ]; then
    echo "FAIL: wasm client did not complete the connect/join/roster-sync sequence (see client output.log above)"
    exit 1
fi
if [ "$SERVER_EXIT" = "124" ]; then
    exit 1
fi
echo "PASS"
exit 0
