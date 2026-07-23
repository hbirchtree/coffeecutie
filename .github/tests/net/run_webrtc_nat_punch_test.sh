#!/bin/bash
# NAT punch-through integration test for the webrtc-gateway fleet path.
#
# Same client/server/gateway roles as run_webrtc_gateway_fleet_test.sh
# (single gateway, fleet registration, wasm browser client via
# /signal?server=<id>, direct-UDP-over-DataChannel -- see that script and
# examples/blam/cblam-testing/WEBRTC_TRANSPORT.md's Phase 6 section), but
# the native server runs inside a docker container behind a real
# MASQUERADE NAT (nat-punch/docker-compose.yml) with its private network
# marked internal -- the gateway physically cannot reach it except
# through mappings the server's own punches open. Linux conntrack is
# endpoint-restricted (symmetric-ish), the strictest NAT type the punch
# design claims to survive, so a pass here proves real traversal, which
# the loopback fleet test cannot.
#
# Extra pass criteria on top of the usual four client markers:
#   - the gateway's "punch received from <addr>" lines must NOT show the
#     server's private address (172.28.100.3) -- i.e. the punch really
#     was NAT'd, we didn't accidentally have a direct route.
#   - registration reached active (that alone proves return-routability:
#     the challenge nonce came back in through a punched mapping).
#
# Topology:
#   client(wasm,browser,host) --ws/ICE--> gateway(host) --UDP--> [conntrack NAT] --> server(container)
#                                          gateway(host) <--UDP punches-- server(container)
#
# Usage:
#   run_webrtc_nat_punch_test.sh [BUNDLE_DIR] [OUT_DIR]
#
# Requires docker + docker compose. Env overrides:
#   SERVER_BINARY  path to BlamGraphics binary   (REQUIRED -- runs inside the container)
#   SERVER_LD      dynamic loader to run it through (strongly recommended: the
#                  container image is bare alpine, the binary runs entirely
#                  against the mounted sysroot; without this the binary must be
#                  self-sufficient against SERVER_IMAGE's own libc)
#   SERVER_LIB_PATH  library path for SERVER_LD --library-path
#   RESOURCE_DIR   asset dir (default: multi_build/desktop-x86_64-buildroot-linux-gnu-multi/examples/blam/cblam-testing/assets/)
#   MAP            map file (default: /mnt/blam/pc/bloodgulch.map)
#   MOUNT_ROOT     directory bind-mounted (ro, same path) into the server
#                  container; must contain SERVER_BINARY, RESOURCE_DIR, the
#                  sysroot, and the repo checkout (default: GITHUB_WORKSPACE,
#                  falling back to the repo root)
#   SERVER_IMAGE   container image for the server (default: alpine:3.20)
#   GATEWAY_HTTP_PORT (default 8098), SERVER_UDP_PORT (default 19601), SERVER_ID (default nat-test)
#   BOOT_TIMEOUT   seconds to wait for gateway/registration (default 60 -- container pulls + apk add take time on first run)
#   RUN_TIMEOUT    cap on the containerized server process (default 150)
#   BOOT_TIMEOUT_MS / CONNECT_TIMEOUT_MS  passed to webrtc_client_smoke.mjs
#   BUILD_GATEWAY  1 = go build the gateway first (default 0)
#   GO             go binary for BUILD_GATEWAY
#   DUMMY_PLUG_CONFIG  server dummy_plug config (default: dummy_plug_net_webrtc_server_for_web_client.json)
#   Mesa/EGL env (LIBGL_*, GALLIUM_DRIVER, EGL_PLATFORM, MESA_LOADER_DRIVER_PATH)
#   is forwarded into the container verbatim if set.

set -uo pipefail
HERE="$(cd "$(dirname "$0")" && pwd)"
SRCDIR="$(cd "$HERE/../../.." && pwd)"
GATEWAY_DIR="$SRCDIR/tools/webrtc-gateway"
COMPOSE_FILE="$HERE/nat-punch/docker-compose.yml"
cd "$SRCDIR"

if ! docker compose version >/dev/null 2>&1; then
    echo "ERROR: docker compose not available -- this test needs docker" >&2
    exit 2
fi

BUNDLE_DIR="${1:-}"
OUT_DIR="${2:-/tmp/webrtc_nat_punch_test}"

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

SERVER_BINARY="${SERVER_BINARY:-}"
if [ -z "$SERVER_BINARY" ] || [ ! -f "$SERVER_BINARY" ]; then
    echo "ERROR: SERVER_BINARY must point at a BlamGraphics binary (it runs inside the container)" >&2
    exit 2
fi
SERVER_BINARY="$(realpath "$SERVER_BINARY")"
RESOURCE_DIR="${RESOURCE_DIR:-$SRCDIR/multi_build/desktop-x86_64-buildroot-linux-gnu-multi/examples/blam/cblam-testing/assets/}"
RESOURCE_DIR="$(realpath "$RESOURCE_DIR")/"
MAP="${MAP:-/mnt/blam/pc/bloodgulch.map}"
MAP="$(realpath "$MAP")"
MAP_DIR="$(dirname "$MAP")"
MOUNT_ROOT="${MOUNT_ROOT:-${GITHUB_WORKSPACE:-$SRCDIR}}"
MOUNT_ROOT="$(realpath "$MOUNT_ROOT")"

# Same reasoning as run_webrtc_web_client_test.sh: the smoke script's
# static server only serves BUNDLE_DIR, and the wasm client requests map
# files by bare relative path.
for mapfile in "$MAP_DIR"/*.map; do
    [ -f "$mapfile" ] && ln -sf "$mapfile" "$BUNDLE_DIR/$(basename "$mapfile")"
done

GATEWAY_HTTP_PORT="${GATEWAY_HTTP_PORT:-8098}"
SERVER_UDP_PORT="${SERVER_UDP_PORT:-19601}"
SERVER_ID="${SERVER_ID:-nat-test}"
BOOT_TIMEOUT="${BOOT_TIMEOUT:-60}"
RUN_TIMEOUT="${RUN_TIMEOUT:-150}"
DUMMY_PLUG_CONFIG="${DUMMY_PLUG_CONFIG:-$HERE/dummy_plug_net_webrtc_server_for_web_client.json}"
DUMMY_PLUG_CONFIG="$(realpath "$DUMMY_PLUG_CONFIG")"

GO="${GO:-go}"
if ! command -v "$GO" >/dev/null 2>&1; then
    if [ -x "$HOME/local/go/bin/go" ]; then
        GO="$HOME/local/go/bin/go"
    fi
fi
GATEWAY_BIN="$GATEWAY_DIR/gateway"
if [ "${BUILD_GATEWAY:-0}" != "0" ]; then
    (cd "$GATEWAY_DIR" && "$GO" build -o gateway .) || { echo "FAIL: gateway build failed"; exit 2; }
fi

GATEWAY_LOG="$OUT_DIR/gateway.log"
SERVER_LOG="$OUT_DIR/server.log"
mkdir -p "$OUT_DIR/server_tmp"
rm -f "$GATEWAY_LOG" "$SERVER_LOG"

echo "Bundle    : $BUNDLE_DIR"
echo "Out dir   : $OUT_DIR"
echo "Binary    : $SERVER_BINARY"
echo "Mount root: $MOUNT_ROOT"
echo "Gateway   : :$GATEWAY_HTTP_PORT"
echo "Server    : containerized behind NAT, udp :$SERVER_UDP_PORT, serverId=$SERVER_ID"

echo "::group::Installing NPM/Playwright dependencies"
(
    cd "$HERE/../web"
    if [ ! -d node_modules/playwright ]; then
        npm install --no-audit --no-fund
    fi
    if [ "${CI:-}" = "true" ]; then
        npx playwright install --with-deps chromium
    else
        npx playwright install chromium
    fi
)
echo "::endgroup::"

# Everything the containerized server needs, in one generated script --
# keeps the compose file static. Mesa/EGL env is forwarded verbatim so CI
# can point at the sysroot's own llvmpipe (same approach as
# run_webrtc_web_client_test.sh's host-side server).
RUN_SERVER="$OUT_DIR/run_server.sh"
{
    echo '#!/bin/sh'
    for var in LIBGL_ALWAYS_SOFTWARE GALLIUM_DRIVER LIBGL_DRIVERS_PATH \
               MESA_LOADER_DRIVER_PATH LIBGL_DRI3_DISABLE EGL_PLATFORM; do
        eval "val=\${$var:-}"
        [ -n "$val" ] && echo "export $var='$val'"
    done
    echo "export TMPDIR='$OUT_DIR/server_tmp'"
    echo "export DUMMY_PLUG_CONFIG='$DUMMY_PLUG_CONFIG'"
    echo "export COFFEE_DISABLE_PROFILER=1"
    # Bare seconds, no "s" suffix -- busybox timeout (the alpine default
    # image) rejects the suffix GNU timeout accepts.
    if [ -n "${SERVER_LD:-}" ]; then
        echo "exec timeout ${RUN_TIMEOUT} '$SERVER_LD' --library-path '${SERVER_LIB_PATH:-}' '$SERVER_BINARY' \\"
    else
        echo "exec timeout ${RUN_TIMEOUT} '$SERVER_BINARY' \\"
    fi
    echo "  '$RESOURCE_DIR' '$MAP' \\"
    echo "  --listen 0.0.0.0:$SERVER_UDP_PORT \\"
    echo "  --gateway-register ws://gateway-host:$GATEWAY_HTTP_PORT \\"
    echo "  --gateway-server-id '$SERVER_ID' \\"
    echo "  > '$SERVER_LOG' 2>&1"
} > "$RUN_SERVER"
chmod +x "$RUN_SERVER"

export MOUNT_ROOT MAP_DIR OUT_DIR
export SERVER_IMAGE="${SERVER_IMAGE:-alpine:3.20}"
COMPOSE=(docker compose -p webrtc-nat-punch -f "$COMPOSE_FILE")

GW_PID=""
cleanup() {
    "${COMPOSE[@]}" down -v --remove-orphans >/dev/null 2>&1
    [ -n "$GW_PID" ] && kill "$GW_PID" 2>/dev/null
}
trap cleanup EXIT

echo "Starting gateway on the host (no -dest -- pure fleet-registry mode)..."
"$GATEWAY_BIN" -listen ":$GATEWAY_HTTP_PORT" > "$GATEWAY_LOG" 2>&1 &
GW_PID=$!
for _ in $(seq 1 15); do
    if (exec 3<>"/dev/tcp/127.0.0.1/$GATEWAY_HTTP_PORT") 2>/dev/null; then
        exec 3>&- 3<&-
        break
    fi
    kill -0 "$GW_PID" 2>/dev/null || { echo "FAIL: gateway exited early"; tail -n 20 "$GATEWAY_LOG"; exit 1; }
    sleep 1
done
echo "Gateway is listening."

echo "Starting NAT router + containerized server..."
"${COMPOSE[@]}" up -d || { echo "FAIL: docker compose up failed"; exit 1; }

echo "Waiting for the registration challenge to pass through the NAT (up to ${BOOT_TIMEOUT}s)..."
booted=0
for _ in $(seq 1 "$BOOT_TIMEOUT"); do
    if grep -q "registration active for serverId=$SERVER_ID" "$SERVER_LOG" 2>/dev/null &&
       grep -q "registration active (challenge passed)" "$GATEWAY_LOG" 2>/dev/null; then
        booted=1
        break
    fi
    if ! "${COMPOSE[@]}" ps --status running --format '{{.Service}}' 2>/dev/null | grep -q '^server$'; then
        echo "FAIL: server container exited before registering"
        break
    fi
    sleep 1
done
if [ "$booted" != "1" ]; then
    echo "FAIL: registration did not become active within ${BOOT_TIMEOUT}s"
    echo "--- compose ps ---";      "${COMPOSE[@]}" ps || true
    echo "--- natrouter logs ---";  "${COMPOSE[@]}" logs natrouter 2>&1 | tail -n 20
    echo "--- server (container) logs ---"; "${COMPOSE[@]}" logs server 2>&1 | tail -n 20
    echo "--- server.log tail ---"; tail -n 40 "$SERVER_LOG" 2>/dev/null
    echo "--- gateway.log ---";     cat "$GATEWAY_LOG"
    exit 1
fi
echo "Server registration is active (challenge crossed the NAT both ways)."

echo "Starting wasm client (Playwright, ws://127.0.0.1:$GATEWAY_HTTP_PORT#$SERVER_ID)..."
BUNDLE_DIR="$BUNDLE_DIR" \
OUT_DIR="$OUT_DIR/client" \
SERVER_URL="ws://127.0.0.1:${GATEWAY_HTTP_PORT}#${SERVER_ID}" \
BOOT_TIMEOUT_MS="${BOOT_TIMEOUT_MS:-45000}" \
CONNECT_TIMEOUT_MS="${CONNECT_TIMEOUT_MS:-60000}" \
node "$HERE/../web/webrtc_client_smoke.mjs"
CLIENT_EXIT=$?

echo
echo "::group::gateway.log"
cat "$GATEWAY_LOG" || true
echo "::endgroup::"
echo "::group::server.log (registration + connection lines)"
grep -E "gateway_fleet_registration|Connection info|Player joined|state change|Error|error" "$SERVER_LOG" 2>/dev/null || true
echo "::endgroup::"
echo "::group::client (browser console) output.log"
cat "$OUT_DIR/client/output.log" 2>/dev/null || true
echo "::endgroup::"

# The NAT-specific assertions -- what makes this more than a reachability
# rerun of the fleet test.
NAT_OK=1
if ! grep -q "punch received from" "$GATEWAY_LOG"; then
    echo "FAIL: gateway never logged a per-client relay punch"
    NAT_OK=0
elif grep "punch received from" "$GATEWAY_LOG" | grep -q "172\.28\.100\.3"; then
    echo "FAIL: relay punch arrived from the server's PRIVATE address -- traffic bypassed the NAT, test topology is broken"
    NAT_OK=0
else
    echo "NAT check OK: relay punches arrived from a masqueraded address:"
    grep "punch received from" "$GATEWAY_LOG" | sed 's/^/  /'
fi

if [ "$CLIENT_EXIT" != "0" ]; then
    echo "FAIL: wasm client did not complete the connect/join/roster-sync sequence"
    exit 1
fi
if [ "$NAT_OK" != "1" ]; then
    exit 1
fi
echo "PASS"
exit 0
