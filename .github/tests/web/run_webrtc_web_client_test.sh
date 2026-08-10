#!/bin/bash
# Wasm-client-through-the-gateway-to-native-server integration test.
#
# One local webrtc-gateway, a fleet-registered native server behind it,
# and a headless-Chromium wasm BlamGraphics client driven by Playwright.
# See examples/blam/cblam-testing/WEBRTC_TRANSPORT.md.
#
# Verification differs from the native-to-native case: the wasm client
# can't use dummy_plug/Journal (comp_app::dummy_plug is compiled out on
# emscripten), so there's no client-side journal to diff. Success is
# decided by the Playwright driver scanning the browser's own console
# output for the connect/join/roster-sync markers instead. The server's
# own journal is still written but isn't compared against anything here.
#
# Topology (SERVER_ROLE=native, the default):
#   client(wasm,browser) --ws(/signal?server=<id>)--> gateway --UDP--> server (--listen)
#                                                     gateway <--ws(/server-signal)-- server (register, heartbeat, punches)
#
# Topology (SERVER_ROLE=wasm) -- both halves are browser pages, so there is
# no UDP anywhere: the host registers as transport=webrtc and the gateway
# splices the two DataChannels together (handleHostSignal, in the gateway).
# This is the only coverage of that bridge with a browser on BOTH sides.
#   client(wasm,browser) --ws(/signal?server=<id>)--> gateway <--DataChannel--> host(wasm,browser)
#                                                     gateway <--ws(/server-signal)-- host (register transport=webrtc)
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
#                 is run directly instead, the same way the other
#                 artifact-driven tests here do.
#   SERVER_LD     path to a specific dynamic linker/loader (ld-linux...) to
#                 invoke SERVER_BINARY through, e.g. the one bundled in a
#                 downloaded cross-toolchain sysroot tarball. The proven
#                 approach for headless Mesa/EGL in CI:
#                 run the whole stack (loader, libc, Mesa/EGL/the llvmpipe
#                 DRI driver) from ONE consistent sysroot rather than
#                 mixing in the CI runner's own system Mesa -- tried the
#                 runner's own loader once, and every eglCreateContext
#                 came back with degenerate configs (EGL_RENDERABLE_TYPE=0).
#   SERVER_LIB_PATH  colon-separated library search path for SERVER_BINARY
#                 (SERVER_LD's --library-path if set, otherwise exported as
#                 LD_LIBRARY_PATH, scoped to the server process either way).
#   SERVER_ROLE   native (default) or wasm. wasm boots a second browser page
#                 as the server instead of a native process, and ignores
#                 TARGET/SERVER_BINARY/SERVER_UDP_PORT/RESOURCE_DIR entirely.
#   HOST_MAP      map file the wasm host loads, WITH extension (default:
#                 MAP's basename, e.g. bloodgulch.map). It must be present
#                 in the bundle dir -- webrtc_link_maps symlinks whatever
#                 sits next to MAP, and both pages fetch it over the same
#                 static server.
#   HOST_READY_TIMEOUT_MS  how long to wait for the wasm host to register
#                 before starting the client (default: 120000 -- two cold
#                 wasm boots on a CI runner, not one)
#   HOST_WARM_FRAMES / HOST_WARM_GAP_MS / HOST_WARM_TIMEOUT_MS
#                 registration goes active seconds into boot, but the first
#                 frames that draw a loaded map compile its shaders and
#                 block the frame loop -- and the frame loop is what answers
#                 the gateway. The client waits for that many frame reports
#                 after the host's map load, the newest no older than the
#                 gap, before dialing (defaults: 3 / 8000 / 180000).
#   HOST_VIEWPORT / VIEWPORT  "WxH" render surface for the host page and for
#                 the client page (defaults: 160x120, and 640x480 -- except
#                 in SERVER_ROLE=wasm, where the client defaults to the host
#                 size too). Nothing here looks at what either page draws,
#                 and on a software rasterizer the surface is most of the
#                 frame time: 640x480 measured 14.7s per frame against
#                 160x120's 1.2s, which is signaling latency, not pixels.
#   GATEWAY_URL   ws(s):// base URL of an already-running gateway. Set it to
#                 skip starting a local one entirely (external deployment,
#                 or to keep the gateway's CPU off this machine).
#   GATEWAY_EXTRA_ARGS  extra flags for the LOCAL gateway process, e.g.
#                 "-registration-ttl 180s". SERVER_ROLE=wasm needs a TTL well
#                 above the default 30s: the host page sends its heartbeats
#                 from the frame loop, and two software-rendered wasm pages on
#                 a small CI runner starve each other for long enough that the
#                 registration would otherwise be swept mid-connect.
#   GATEWAY_HTTP_PORT  gateway WebSocket signaling port (default: 8098)
#   SERVER_UDP_PORT    real GNS UDP listen port the server binds (default: 19601)
#   SERVER_ID          fleet registry serverId to register/connect under (default: test)
#   BOOT_TIMEOUT  seconds to wait for the gateway / server registration (default: 30)
#   RUN_TIMEOUT   hard cap on the server process, via `timeout` (default: 150 -- must stay
#                 above SERVER_DUMMY_PLUG_CONFIG's end_time (140s), which is sized for a
#                 cold wasm boot on a slow CI runner; keep the two in sync if either changes)
#   BOOT_TIMEOUT_MS / CONNECT_TIMEOUT_MS  passed through to the Playwright driver (defaults there: 45000 / 60000)
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
# native: a fleet-registered native server behind the gateway (the default,
# and the only mode with a UDP leg). wasm: a second browser page hosts the
# server as a DataChannel peer, so the gateway bridges two DataChannels and
# no UDP is involved anywhere -- everything below that only concerns the
# native server is skipped in that mode.
SERVER_ROLE="${SERVER_ROLE:-native}"
GATEWAY_EXTRA_ARGS="${GATEWAY_EXTRA_ARGS:-}"
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
if [ "$SERVER_ROLE" = "wasm" ]; then
    echo "Server   : wasm host page (datachannel bridge), serverId=$SERVER_ID"
else
    echo "Target   : $TARGET"
    echo "Server   : udp 127.0.0.1:$SERVER_UDP_PORT, serverId=$SERVER_ID"
fi

# An external gateway (GATEWAY_URL) replaces the local one entirely: no
# build, no process, no gateway.log to dump. Useful for reproducing
# against a real deployment, and for taking the gateway's own CPU off a
# small runner.
if [ -n "${GATEWAY_URL:-}" ]; then
    USE_LOCAL_GATEWAY=0
    echo "Gateway  : external (from GATEWAY_URL)"
else
    USE_LOCAL_GATEWAY=1
    GATEWAY_URL="ws://127.0.0.1:$GATEWAY_HTTP_PORT"
    echo "Gateway  : local :$GATEWAY_HTTP_PORT"
fi

[ "$USE_LOCAL_GATEWAY" = "1" ] && { webrtc_build_gateway "$GATEWAY_DIR" "$(webrtc_find_go)" || exit 2; }

if [ "${BUILD:-0}" != "0" ] && [ "$SERVER_ROLE" != "wasm" ]; then
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

if [ "$USE_LOCAL_GATEWAY" = "1" ]; then
    # Word-split on purpose: GATEWAY_EXTRA_ARGS carries whole flags.
    # shellcheck disable=SC2086
    webrtc_start_gateway "$GATEWAY_BIN" "$GATEWAY_HTTP_PORT" "$GATEWAY_LOG" "$BOOT_TIMEOUT" \
        $GATEWAY_EXTRA_ARGS || exit 1
fi

if [ "$SERVER_ROLE" = "wasm" ]; then
    # The host is just another page for the Playwright driver to boot:
    # it registers itself (--listen ws://gw#id) and waits for its own
    # registration to go active before the client is allowed to dial.
    # Same registration wait as the native path, just observed through the
    # browser console instead of a log file.
    export HOST_URL="${GATEWAY_URL}#${SERVER_ID}"
    # Keep the .map extension: graphics.cpp feeds --map straight to MkUrl
    # as a filename (only the server-sent MapLoadByName path appends the
    # extension itself), and webrtc_link_maps symlinked it under exactly
    # this name into the bundle the pages are served from.
    export HOST_MAP="${HOST_MAP:-$(basename "$MAP")}"
    echo "Server is a second wasm page (serverId=$SERVER_ID, map=$HOST_MAP)"
else
    echo "Starting native server (--listen 127.0.0.1:$SERVER_UDP_PORT --gateway-register $GATEWAY_URL --gateway-server-id $SERVER_ID)..."
    webrtc_server_command "$TARGET" \
        "$RESOURCE_DIR" "$MAP" \
        --listen "127.0.0.1:$SERVER_UDP_PORT" \
        --gateway-register "$GATEWAY_URL" \
        --gateway-server-id "$SERVER_ID"
    webrtc_start_server "$SERVER_LOG" "$SERVER_TMP" "$SERVER_DUMMY_PLUG_CONFIG" "$RUN_TIMEOUT"

    webrtc_wait_for_registration "$SERVER_LOG" "$SERVER_ID" "$BOOT_TIMEOUT" "$WEBRTC_SERVER_PID" || {
        [ "$USE_LOCAL_GATEWAY" = "1" ] && webrtc_dump "gateway.log" "$GATEWAY_LOG"
        exit 1
    }
fi

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
if [ "$SERVER_ROLE" = "wasm" ]; then
    : # no native server process in this mode
elif ! kill -0 "$WEBRTC_SERVER_PID" 2>/dev/null; then
    wait "$WEBRTC_SERVER_PID"
    SERVER_EXIT=$?
fi

echo "server exit=$SERVER_EXIT  client exit=$CLIENT_EXIT"
if [ "$SERVER_EXIT" = "124" ]; then
    echo "FAIL: server hit the ${RUN_TIMEOUT}s timeout"
fi

# Logs are only echoed on failure -- they all live in OUT_DIR either way,
# which CI uploads as an artifact. In wasm mode both roles' console output
# is in the one client log, tagged [host]/[client].
if [ "$CLIENT_EXIT" != "0" ] || [ "$SERVER_EXIT" = "124" ]; then
    echo
    [ "$USE_LOCAL_GATEWAY" = "1" ] && webrtc_dump "gateway.log" "$GATEWAY_LOG"
    [ "$SERVER_ROLE" = "wasm" ] || webrtc_dump_server_lines "$SERVER_LOG"
    webrtc_dump "client (browser console)" "$WEBRTC_CLIENT_LOG"
fi

if [ "$CLIENT_EXIT" != "0" ]; then
    echo "FAIL: wasm client did not complete the connect/join/roster-sync sequence"
    exit 1
fi
if [ "$SERVER_EXIT" = "124" ]; then
    exit 1
fi
echo "PASS  (logs in $OUT_DIR)"
exit 0
