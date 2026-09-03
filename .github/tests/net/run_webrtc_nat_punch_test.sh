#!/bin/bash
# NAT punch-through integration test against a real, internet-hosted
# webrtc-gateway.
#
# Same client/server roles as the loopback wasm-client test (fleet
# registration, wasm browser client via /signal?server=<id>,
# direct-UDP-over-DataChannel -- see
# The gateway is a separate deployment out on the internet and the native
# server runs on this machine behind whatever NAT the runner sits behind,
# with no port forwarding.
#
# Pass criteria:
#   - the server's registration reaches active against the remote gateway
#     (return-routability across the NAT, both directions)
#   - the server logs a per-client relay punch for the browser's session
#   - the browser completes the four connect/join/roster-sync markers
#
# Topology:
#   client(wasm,browser,here) --wss--> gateway(internet) --UDP--> [NAT] --> server(here)
#                                      gateway(internet) <--UDP punches-- server(here)
#
# Usage:
#   run_webrtc_nat_punch_test.sh [BUNDLE_DIR] [OUT_DIR]
#
# Env overrides (plus the shared SERVER_BINARY/SERVER_LD/SERVER_LIB_PATH
# and BOOT_TIMEOUT_MS/CONNECT_TIMEOUT_MS the harness documents):
#   The gateway itself must be reachable for WebRTC, not just for signaling:
#   run it with -public-ip <its public address> and publish the
#   -ice-udp-port-min/-max range. It is ICE-lite, so it advertises only the
#   host candidates it gathers -- behind NAT or docker those are private
#   addresses no browser can reach, and ICE fails after signaling succeeds.
#
#   GATEWAY_URL    REQUIRED -- ws(s):// base URL of the remote gateway
#                  (CI passes the WEBRTC_GATEWAY_SERVER secret). Never
#                  echoed by this script; note the server's own log does
#                  print it, and that log is uploaded as an artifact.
#   SERVER_ID      fleet registry serverId (default: nat-<run id>-<attempt>).
#                  Must be unique per concurrent run -- the gateway is shared.
#   TARGET / RESOURCE_DIR / MAP / SERVER_UDP_PORT  as in the other tests
#                  (SERVER_UDP_PORT default 19601 -- nothing dials into it,
#                  it is never forwarded; the punch is what makes it
#                  reachable)
#   BOOT_TIMEOUT   seconds to wait for registration (default: 60 -- a real
#                  round trip to the internet, not loopback)
#   RUN_TIMEOUT    cap on the server process (default: 150)
#   SERVER_DUMMY_PLUG_CONFIG  default: dummy_plug_net_webrtc_server_for_web_client.json

set -uo pipefail
HERE="$(cd "$(dirname "$0")" && pwd)"
SRCDIR="$(cd "$HERE/../../.." && pwd)"
# shellcheck source=.github/tests/net/webrtc_test_lib.sh
. "$HERE/webrtc_test_lib.sh"
cd "$SRCDIR"

GATEWAY_URL="${GATEWAY_URL:-}"
if [ -z "$GATEWAY_URL" ]; then
    echo "ERROR: GATEWAY_URL is required (CI supplies it from the WEBRTC_GATEWAY_SERVER secret)" >&2
    exit 2
fi

BUNDLE_DIR="$(webrtc_find_bundle "$SRCDIR" "${1:-}")" || exit 2
OUT_DIR="${2:-/tmp/webrtc_nat_punch_test}"
mkdir -p "$OUT_DIR"
OUT_DIR="$(realpath "$OUT_DIR")"

TARGET="${TARGET:-desktop:x86_64-buildroot-linux-gnu:multi/BlamGraphics}"
RESOURCE_DIR="${RESOURCE_DIR:-multi_build/desktop-x86_64-buildroot-linux-gnu-multi/examples/blam/cblam-testing/assets/}"
MAP="${MAP:-/mnt/blam/pc/bloodgulch.map}"
SERVER_UDP_PORT="${SERVER_UDP_PORT:-19601}"
# The gateway is shared, so the ID has to be unique per concurrent run.
SERVER_ID="${SERVER_ID:-nat-${GITHUB_RUN_ID:-local$$}-${GITHUB_RUN_ATTEMPT:-1}}"
BOOT_TIMEOUT="${BOOT_TIMEOUT:-60}"
RUN_TIMEOUT="${RUN_TIMEOUT:-150}"
SERVER_DUMMY_PLUG_CONFIG="${SERVER_DUMMY_PLUG_CONFIG:-$HERE/dummy_plug_net_webrtc_server_for_web_client.json}"

webrtc_link_maps "$MAP" "$BUNDLE_DIR"

SERVER_TMP="$OUT_DIR/server_tmp"
mkdir -p "$SERVER_TMP"
SERVER_LOG="$OUT_DIR/server.log"
rm -f "$SERVER_LOG"

echo "Bundle     : $BUNDLE_DIR"
echo "Out dir    : $OUT_DIR"
echo "Gateway    : (remote, from WEBRTC_GATEWAY_SERVER)"
echo "Server     : udp 0.0.0.0:$SERVER_UDP_PORT (not forwarded), serverId=$SERVER_ID"
# Informational: on a NAT'd runner these are RFC1918 addresses, which is
# exactly why the gateway cannot reach us without a punch.
echo "Local addrs: $(hostname -I 2>/dev/null || echo unknown)"

webrtc_install_playwright "$HERE/../web"

cleanup() {
    [ -n "${WEBRTC_SERVER_PID:-}" ] && kill "$WEBRTC_SERVER_PID" 2>/dev/null
}
trap cleanup EXIT

echo "Starting native server (registering with the remote gateway as $SERVER_ID)..."
webrtc_server_command "$TARGET" \
    "$RESOURCE_DIR" "$MAP" \
    --listen "0.0.0.0:$SERVER_UDP_PORT" \
    --gateway-register "$GATEWAY_URL" \
    --gateway-server-id "$SERVER_ID"
webrtc_start_server "$SERVER_LOG" "$SERVER_TMP" "$SERVER_DUMMY_PLUG_CONFIG" "$RUN_TIMEOUT"

if ! webrtc_wait_for_registration "$SERVER_LOG" "$SERVER_ID" "$BOOT_TIMEOUT" "$WEBRTC_SERVER_PID"; then
    # Distinguish "never reached the gateway at all" from "reached it but
    # the UDP punch didn't land": the first is the gateway being down or
    # its URL being wrong, and no amount of punching fixes it.
    if grep -q "websocket error" "$SERVER_LOG" 2>/dev/null; then
        echo "  (the signaling websocket never connected -- the remote gateway in"
        echo "   WEBRTC_GATEWAY_SERVER is down or unreachable, so this ran before"
        echo "   any punch was attempted)"
    else
        echo "  (the remote gateway never got our punch, or its reply never came back --"
        echo "   check that its UDP punch port is reachable and that it is actually up)"
    fi
    exit 1
fi

echo "Starting wasm client (Playwright, remote gateway #$SERVER_ID)..."
webrtc_run_wasm_client "$BUNDLE_DIR" "$OUT_DIR/client" \
    "${GATEWAY_URL}#${SERVER_ID}" "$HERE/../web/webrtc_client_smoke.mjs"
CLIENT_EXIT=$?

# The NAT-specific assertion, from our side of the NAT: the gateway asked
# us to punch a per-client relay port, and we did. (The gateway's own
# "authentic punch received from <addr>" line lives on the remote host and
# isn't readable from here -- but the client markers below can't pass
# without it having arrived.)
#
# Match only the stable prefix: what follows it is session identification
# whose formatting has already changed once (session ID -> tracking IDs),
# which silently turned this assertion into a guaranteed failure.
NAT_PUNCH_MARKER="relay punch started for"
NAT_OK=1
if grep -q "$NAT_PUNCH_MARKER" "$SERVER_LOG" 2>/dev/null; then
    echo "NAT check OK: per-client relay punch(es) sent:"
    grep "$NAT_PUNCH_MARKER" "$SERVER_LOG" | sed 's/^/  /'
else
    echo "FAIL: server never punched a per-client relay port (no client-relay from the gateway?)"
    NAT_OK=0
fi

# Logs are echoed only on failure; they are in OUT_DIR either way, which
# CI uploads as an artifact.
if [ "$CLIENT_EXIT" != "0" ] || [ "$NAT_OK" != "1" ]; then
    echo
    webrtc_dump_server_lines "$SERVER_LOG"
    webrtc_dump "client (browser console)" "$WEBRTC_CLIENT_LOG"
fi

if [ "$CLIENT_EXIT" != "0" ]; then
    echo "FAIL: wasm client did not complete the connect/join/roster-sync sequence"
    exit 1
fi
if [ "$NAT_OK" != "1" ]; then
    exit 1
fi
echo "PASS  (logs in $OUT_DIR)"
