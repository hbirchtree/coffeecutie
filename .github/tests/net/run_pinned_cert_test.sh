#!/bin/bash
# Native-to-native pinned-certificate test.
#
# A server issues itself a GNS certificate signed by its Ed25519 key
# (--gateway-auth-key, which despite the name is just the server's keypair and
# needs no gateway), and a client pins the matching public key with
# --server-key. GNS then accepts only a certificate that key signed.
#
# Both directions are checked, because only the pair is meaningful: a pin that
# accepts the right key proves nothing on its own if it would accept any key.
#   1. correct key  -> the connection completes
#   2. wrong key    -> the connection is refused
#
# No gateway and no browser anywhere in this test: the point is that the
# authentication is a property of the GNS session, not of the DataChannel
# transport it was originally written for. For plain UDP it is the only thing
# authenticating the peer, there being no second encrypted hop.
#
# Usage: run_pinned_cert_test.sh [OUT_DIR]
#
# Env overrides: TARGET, RESOURCE_DIR, MAP, PORT, BOOT_TIMEOUT, RUN_TIMEOUT

set -uo pipefail
HERE="$(cd "$(dirname "$0")" && pwd)"
SRCDIR="$(cd "$HERE/../../.." && pwd)"
# shellcheck source=.github/tests/net/webrtc_test_lib.sh
. "$HERE/webrtc_test_lib.sh"
cd "$SRCDIR"

OUT_DIR="${1:-/tmp/pinned_cert_test}"
mkdir -p "$OUT_DIR"
OUT_DIR="$(realpath "$OUT_DIR")"

TARGET="${TARGET:-desktop:x86_64-buildroot-linux-gnu:multi/BlamGraphics}"
RESOURCE_DIR="${RESOURCE_DIR:-multi_build/desktop-x86_64-buildroot-linux-gnu-multi/examples/blam/cblam-testing/assets/}"
MAP="${MAP:-/mnt/blam/pc/bloodgulch.map}"
PORT="${PORT:-19701}"
BOOT_TIMEOUT="${BOOT_TIMEOUT:-60}"
RUN_TIMEOUT="${RUN_TIMEOUT:-90}"
SERVER_DUMMY_PLUG_CONFIG="${SERVER_DUMMY_PLUG_CONFIG:-$HERE/dummy_plug_net_server.json}"
CLIENT_DUMMY_PLUG_CONFIG="${CLIENT_DUMMY_PLUG_CONFIG:-$HERE/dummy_plug_net_client.json}"

KEY="$OUT_DIR/server_identity.pem"
WRONG_KEY="$OUT_DIR/other_identity.pem"
rm -f "$KEY" "$WRONG_KEY"
openssl genpkey -algorithm ed25519 -out "$KEY" 2>/dev/null || {
    echo "FAIL: could not generate an Ed25519 key (need openssl)"; exit 2; }
openssl genpkey -algorithm ed25519 -out "$WRONG_KEY" 2>/dev/null || exit 2

# An Ed25519 SubjectPublicKeyInfo is 44 DER bytes with the raw 32-byte key at
# the end, which is the form the pin takes.
pubkey_b64() {
    openssl pkey -in "$1" -pubout -outform DER 2>/dev/null | tail -c 32 | base64 -w0
}
GOOD_KEY_B64="$(pubkey_b64 "$KEY")"
WRONG_KEY_B64="$(pubkey_b64 "$WRONG_KEY")"
if [ -z "$GOOD_KEY_B64" ] || [ -z "$WRONG_KEY_B64" ]; then
    echo "FAIL: could not extract public keys"; exit 2
fi
echo "Server key : $GOOD_KEY_B64"
echo "Wrong key  : $WRONG_KEY_B64"

# Runs one server/client pair with the given pin ("" for none). Sets
# RUN_CONNECTED (yes/no/error), RUN_TRUST (authenticated/unauthenticated/none,
# from the client's connect line) and RUN_REFUSED (yes when GNS rejected the
# server's cert against the pin).
run_pair() {
    local label="$1" pin="$2" out="$OUT_DIR/$1"
    mkdir -p "$out"
    webrtc_prepare_out_dir "$out"

    webrtc_server_command "$TARGET" "$RESOURCE_DIR" "$MAP" \
        --listen "127.0.0.1:$PORT" --gateway-auth-key "$KEY"
    webrtc_start_server "$WEBRTC_SERVER_LOG" "$WEBRTC_SERVER_TMP" \
        "$SERVER_DUMMY_PLUG_CONFIG" "$RUN_TIMEOUT"

    local booted=0 _
    for _ in $(seq 1 "$BOOT_TIMEOUT"); do
        grep -q "Started server on" "$WEBRTC_SERVER_LOG" 2>/dev/null && { booted=1; break; }
        kill -0 "$WEBRTC_SERVER_PID" 2>/dev/null || break
        sleep 1
    done
    if [ "$booted" != "1" ]; then
        echo "FAIL[$label]: server never started listening"
        webrtc_dump "server.log (tail)" "$WEBRTC_SERVER_LOG" 40
        webrtc_kill_tree "$WEBRTC_SERVER_PID"
        RUN_CONNECTED=error
        return
    fi

    if [ -n "$pin" ]; then
        webrtc_start_native_client "$TARGET" "$RESOURCE_DIR" "$MAP" \
            "$WEBRTC_CLIENT_LOG" "$WEBRTC_CLIENT_TMP" "$CLIENT_DUMMY_PLUG_CONFIG" \
            "$RUN_TIMEOUT" --server "127.0.0.1:$PORT" --server-key "$pin"
    else
        webrtc_start_native_client "$TARGET" "$RESOURCE_DIR" "$MAP" \
            "$WEBRTC_CLIENT_LOG" "$WEBRTC_CLIENT_TMP" "$CLIENT_DUMMY_PLUG_CONFIG" \
            "$RUN_TIMEOUT" --server "127.0.0.1:$PORT"
    fi

    wait "$WEBRTC_CLIENT_PID" 2>/dev/null
    webrtc_kill_tree "$WEBRTC_SERVER_PID"
    wait "$WEBRTC_SERVER_PID" 2>/dev/null

    RUN_CONNECTED=no
    RUN_TRUST=none
    RUN_REFUSED=no
    if grep -q "Connection to server/peer established" "$WEBRTC_CLIENT_LOG" 2>/dev/null; then
        RUN_CONNECTED=yes
        # The trust state GNS reports for the session, logged at connect:
        # "authenticated" only when a pinned key verified the server's cert
        if grep -q "established (.*, authenticated)" "$WEBRTC_CLIENT_LOG"; then
            RUN_TRUST=authenticated
        elif grep -q "established (.*, unauthenticated)" "$WEBRTC_CLIENT_LOG"; then
            RUN_TRUST=unauthenticated
        fi
    fi
    # GNS's reason for refusing a cert the pinned key did not sign. Without
    # this, a client that crashed or never ran would pass case 2 as well.
    if grep -q "does not chain to the pinned root" "$WEBRTC_CLIENT_LOG" 2>/dev/null; then
        RUN_REFUSED=yes
    fi
}

echo
echo "=== 1. correct key: the connection should complete ==="
run_pair good "$GOOD_KEY_B64"
GOOD_RESULT="$RUN_CONNECTED"
GOOD_TRUST="$RUN_TRUST"
echo "connected=$GOOD_RESULT trust=$GOOD_TRUST"

echo
echo "=== 2. wrong key: the connection should be refused ==="
run_pair wrong "$WRONG_KEY_B64"
WRONG_RESULT="$RUN_CONNECTED"
WRONG_REFUSED="$RUN_REFUSED"
echo "connected=$WRONG_RESULT refused=$WRONG_REFUSED"

echo
echo "=== 3. no key: should still connect, just unauthenticated ==="
# Opting into a certificate must not lock out clients that were never given
# the key -- they simply cannot tell who they are talking to.
run_pair nokey ""
NOKEY_RESULT="$RUN_CONNECTED"
NOKEY_TRUST="$RUN_TRUST"
echo "connected=$NOKEY_RESULT trust=$NOKEY_TRUST"

GOOD_OK=0; WRONG_OK=0; NOKEY_OK=0
[ "$GOOD_RESULT" = "yes" ] && [ "$GOOD_TRUST" = "authenticated" ] && GOOD_OK=1
[ "$WRONG_RESULT" = "no" ] && [ "$WRONG_REFUSED" = "yes" ] && WRONG_OK=1
[ "$NOKEY_RESULT" = "yes" ] && [ "$NOKEY_TRUST" = "unauthenticated" ] && NOKEY_OK=1

echo
echo "================ pinned certificate result ================"
[ "$GOOD_OK" = "1" ]  && echo "  PASS  correct key connects, authenticated" \
                      || echo "  FAIL  correct key: connected=$GOOD_RESULT trust=$GOOD_TRUST"
[ "$WRONG_OK" = "1" ] && echo "  PASS  wrong key is refused for its cert" \
                      || echo "  FAIL  wrong key: connected=$WRONG_RESULT refused=$WRONG_REFUSED"
[ "$NOKEY_OK" = "1" ] && echo "  PASS  keyless client connects, unauthenticated" \
                      || echo "  FAIL  keyless client: connected=$NOKEY_RESULT trust=$NOKEY_TRUST"
echo "==========================================================="

if [ "$GOOD_OK" = "1" ] && [ "$WRONG_OK" = "1" ] && [ "$NOKEY_OK" = "1" ]; then
    echo "PASS  (logs in $OUT_DIR)"
    exit 0
fi
webrtc_dump "good/client.log (tail)" "$OUT_DIR/good/client.log" 40
webrtc_dump "wrong/client.log (tail)" "$OUT_DIR/wrong/client.log" 40
exit 1
