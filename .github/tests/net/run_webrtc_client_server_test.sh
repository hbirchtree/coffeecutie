#!/bin/bash
# WebRTC-transport client/server replication integration test.
#
# Same coverage as the plain-UDP client/server test (two BlamGraphics
# instances, journal comparison) but routes the client's connection
# through a webrtc-gateway (tools/webrtc-gateway) instead of a direct UDP
# dial (see examples/blam/cblam-testing/WEBRTC_TRANSPORT.md).
#
# One gateway, one topology: the server binds an ordinary GNS UDP listen
# socket and registers it with the gateway's fleet registry
# (--gateway-register/--gateway-server-id); the client bootstraps a
# DataChannel to the gateway and runs GNS's ordinary direct-UDP protocol
# over it (--server ws://...#<serverId>). The gateway relays
# DataChannel <-> UDP, reaching the server through the NAT mapping the
# server's own relay punch opened.
#
# Topology:
#   client --ws(/signal?server=<id>)--> gateway --UDP--> server (--listen)
#                                       gateway <--ws(/server-signal)-- server (register, heartbeat, punches)
#
# Usage:
#   run_webrtc_client_server_test.sh [OUT_DIR]
#
# Env overrides:
#   TARGET        cb build/run target        (default: desktop:x86_64-buildroot-linux-gnu:multi/BlamGraphics)
#   RESOURCE_DIR  asset dir passed to both    (default: multi_build/desktop-x86_64-buildroot-linux-gnu-multi/examples/blam/cblam-testing/assets/)
#   MAP           map file passed to both     (default: /mnt/blam/pc/bloodgulch.map)
#   SERVER_TRANSPORT   how the server is reachable (default: udp)
#                        udp    -- real GNS listen socket, gateway relays
#                                  DataChannel <-> UDP through a NAT punch
#                        webrtc -- WebRTC-hosted server: no UDP socket at
#                                  all, the gateway bridges the client's
#                                  DataChannel to one the server opens per
#                                  session. The client picks its GNS mode
#                                  from what the gateway reports, so its
#                                  command line is identical either way.
#   GATEWAY_HTTP_PORT  gateway WebSocket signaling port (default: 8098)
#   SERVER_UDP_PORT    real GNS UDP listen port the server binds (default: 19601)
#   SERVER_ID          fleet registry serverId to register/connect under (default: test)
#   BOOT_TIMEOUT  seconds to wait for each boot signal (gateway, server registration) (default: 30)
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
# shellcheck source=.github/tests/net/webrtc_test_lib.sh
. "$HERE/webrtc_test_lib.sh"
cd "$SRCDIR"

OUT_DIR="${1:-/tmp/webrtc_net_test}"
TARGET="${TARGET:-desktop:x86_64-buildroot-linux-gnu:multi/BlamGraphics}"
RESOURCE_DIR="${RESOURCE_DIR:-multi_build/desktop-x86_64-buildroot-linux-gnu-multi/examples/blam/cblam-testing/assets/}"
MAP="${MAP:-/mnt/blam/pc/bloodgulch.map}"
GATEWAY_HTTP_PORT="${GATEWAY_HTTP_PORT:-8098}"
SERVER_UDP_PORT="${SERVER_UDP_PORT:-19601}"
SERVER_ID="${SERVER_ID:-test}"
SERVER_TRANSPORT="${SERVER_TRANSPORT:-udp}"
if [ "$SERVER_TRANSPORT" != "udp" ] && [ "$SERVER_TRANSPORT" != "webrtc" ]; then
    echo "ERROR: SERVER_TRANSPORT must be udp or webrtc, got '$SERVER_TRANSPORT'" >&2
    exit 2
fi
BOOT_TIMEOUT="${BOOT_TIMEOUT:-30}"
RUN_TIMEOUT="${RUN_TIMEOUT:-60}"

SERVER_DUMMY_PLUG_CONFIG="${SERVER_DUMMY_PLUG_CONFIG:-$HERE/dummy_plug_net_webrtc_server.json}"
CLIENT_DUMMY_PLUG_CONFIG="${CLIENT_DUMMY_PLUG_CONFIG:-$HERE/dummy_plug_net_webrtc_client.json}"

GATEWAY_BIN="$GATEWAY_DIR/gateway"

webrtc_prepare_out_dir "$OUT_DIR"

echo "Out dir  : $OUT_DIR"
echo "Target   : $TARGET"
echo "Gateway  : :$GATEWAY_HTTP_PORT"
if [ "$SERVER_TRANSPORT" = "webrtc" ]; then
    echo "Server   : webrtc-hosted (no UDP socket), serverId=$SERVER_ID"
else
    echo "Server   : udp 127.0.0.1:$SERVER_UDP_PORT, serverId=$SERVER_ID"
fi

webrtc_build_gateway "$GATEWAY_DIR" "$(webrtc_find_go)" || exit 2
webrtc_build_target "$TARGET" || exit 2

trap webrtc_cleanup_pids EXIT

webrtc_start_gateway "$GATEWAY_BIN" "$GATEWAY_HTTP_PORT" "$WEBRTC_GATEWAY_LOG" "$BOOT_TIMEOUT" || exit 1

GATEWAY_URL="ws://127.0.0.1:$GATEWAY_HTTP_PORT"

if [ "$SERVER_TRANSPORT" = "webrtc" ]; then
    # A ws:// --listen makes the server WebRTC-hosted: it registers over
    # /server-signal and serves each client a DataChannel of its own,
    # which the gateway bridges. No listen socket, so nothing to punch.
    echo "Starting server (--listen $GATEWAY_URL#$SERVER_ID, webrtc-hosted)..."
    webrtc_server_command "$TARGET" \
        "$RESOURCE_DIR" "$MAP" \
        --listen "${GATEWAY_URL}#${SERVER_ID}"
else
    echo "Starting server (--listen 127.0.0.1:$SERVER_UDP_PORT --gateway-register $GATEWAY_URL --gateway-server-id $SERVER_ID)..."
    webrtc_server_command "$TARGET" \
        "$RESOURCE_DIR" "$MAP" \
        --listen "127.0.0.1:$SERVER_UDP_PORT" \
        --gateway-register "$GATEWAY_URL" \
        --gateway-server-id "$SERVER_ID"
fi
webrtc_start_server "$WEBRTC_SERVER_LOG" "$WEBRTC_SERVER_TMP" "$SERVER_DUMMY_PLUG_CONFIG" "$RUN_TIMEOUT"

webrtc_wait_for_registration "$WEBRTC_SERVER_LOG" "$SERVER_ID" "$BOOT_TIMEOUT" "$WEBRTC_SERVER_PID" || {
    webrtc_dump "gateway.log" "$WEBRTC_GATEWAY_LOG"
    exit 1
}

echo "Starting client (--server $GATEWAY_URL#$SERVER_ID)..."
webrtc_start_native_client "$TARGET" "$RESOURCE_DIR" "$MAP" \
    "$WEBRTC_CLIENT_LOG" "$WEBRTC_CLIENT_TMP" "$CLIENT_DUMMY_PLUG_CONFIG" "$RUN_TIMEOUT" \
    --server "${GATEWAY_URL}#${SERVER_ID}"

echo "Waiting for both processes to finish (dummy_plug end_time closes them)..."
wait "$WEBRTC_CLIENT_PID"
CLIENT_EXIT=$?
wait "$WEBRTC_SERVER_PID"
SERVER_EXIT=$?

echo "server exit=$SERVER_EXIT  client exit=$CLIENT_EXIT"
if [ "$SERVER_EXIT" = "124" ] || [ "$CLIENT_EXIT" = "124" ]; then
    echo "FAIL: a process hit the ${RUN_TIMEOUT}s timeout (hung -- likely connection never established)"
fi

webrtc_compare_journals "$WEBRTC_SERVER_JOURNAL" "$WEBRTC_CLIENT_JOURNAL" "$HERE/compare_journals.py"
RESULT=$?

# Process logs are echoed only when something went wrong; they are in
# OUT_DIR either way, which CI uploads as an artifact.
if [ "$RESULT" != "0" ] || [ "$SERVER_EXIT" != "0" ] || [ "$CLIENT_EXIT" != "0" ]; then
    echo
    webrtc_dump "gateway.log" "$WEBRTC_GATEWAY_LOG"
    webrtc_dump_matching "server.log (connection + state-dump lines)" "$WEBRTC_SERVER_LOG" \
        "State dumped|registration active|gateway_fleet_registration|Connection info|Player joined|Controller|Error|error"
    webrtc_dump_matching "client.log (connection + state-dump lines)" "$WEBRTC_CLIENT_LOG" \
        "State dumped|join confirmation|roster received|WebRTC|Error|error"
fi
[ "$RESULT" = "0" ] && echo "PASS  (logs in $OUT_DIR)"
exit $RESULT
