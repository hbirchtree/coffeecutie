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

# Runs one server/client pair with the given pin ("" for none), and reports
# whether the client reached a connected state. Sets RUN_CONNECTED.
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
        kill "$WEBRTC_SERVER_PID" 2>/dev/null
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
    kill "$WEBRTC_SERVER_PID" 2>/dev/null
    wait "$WEBRTC_SERVER_PID" 2>/dev/null

    if grep -q "Connection to server/peer established" "$WEBRTC_CLIENT_LOG" 2>/dev/null; then
        RUN_CONNECTED=yes
    else
        RUN_CONNECTED=no
    fi
}

echo
echo "=== 1. correct key: the connection should complete ==="
run_pair good "$GOOD_KEY_B64"
GOOD_RESULT="$RUN_CONNECTED"
echo "connected=$GOOD_RESULT"

echo
echo "=== 2. wrong key: the connection should be refused ==="
run_pair wrong "$WRONG_KEY_B64"
WRONG_RESULT="$RUN_CONNECTED"
echo "connected=$WRONG_RESULT"

echo
echo "=== 3. no key: should still connect, just unauthenticated ==="
# Opting into a certificate must not lock out clients that were never given
# the key -- they simply cannot tell who they are talking to.
run_pair nokey ""
NOKEY_RESULT="$RUN_CONNECTED"
echo "connected=$NOKEY_RESULT"

echo
echo "================ pinned certificate result ================"
[ "$GOOD_RESULT" = "yes" ] && echo "  PASS  correct key connects" \
                           || echo "  FAIL  correct key did not connect"
[ "$WRONG_RESULT" = "no" ]  && echo "  PASS  wrong key is refused" \
                           || echo "  FAIL  wrong key was NOT refused"
[ "$NOKEY_RESULT" = "yes" ] && echo "  PASS  keyless client still connects" \
                           || echo "  FAIL  keyless client was locked out"
echo "==========================================================="

if [ "$GOOD_RESULT" = "yes" ] && [ "$WRONG_RESULT" = "no" ] \
   && [ "$NOKEY_RESULT" = "yes" ]; then
    echo "PASS  (logs in $OUT_DIR)"
    exit 0
fi
webrtc_dump "good/client.log (tail)" "$OUT_DIR/good/client.log" 40
webrtc_dump "wrong/client.log (tail)" "$OUT_DIR/wrong/client.log" 40
exit 1
