#!/bin/bash
# Shared harness for the webrtc-gateway integration tests. Source it:
#
#   . "$(dirname "$0")/webrtc_test_lib.sh"
#
# The three tests that use it (native<->native, wasm client, NAT
# punch-through against a remote gateway) all drive the same topology --
# one gateway, one fleet-registered native server, one client -- and
# differ only in where the gateway lives and what plays the client. Only
# those differences belong in the individual scripts.
#
# Functions set these globals rather than echoing (arrays don't survive
# command substitution): WEBRTC_GW_PID, WEBRTC_SERVER_PID,
# WEBRTC_SERVER_CMD, WEBRTC_SERVER_ENV.

# Locates a built emscripten bundle, or returns 1 with a message.
webrtc_find_bundle() {
    local srcdir="$1" bundle="${2:-}"
    if [ -z "$bundle" ]; then
        bundle="$(find "$srcdir/multi_build" -type d -name BlamGraphics.bundle 2>/dev/null \
            | grep -E 'web-.*emscripten' | grep -v '\.web_scratch' | head -n 1)"
    fi
    if [ -z "$bundle" ] || [ ! -f "$bundle/BlamGraphics.html" ]; then
        echo "ERROR: could not find a BlamGraphics.bundle. Pass BUNDLE_DIR explicitly." >&2
        return 1
    fi
    realpath "$bundle"
}

# webrtc_client_smoke.mjs's static server only serves the bundle dir, and
# the wasm client asks for maps by bare relative path -- so every .map
# next to $1 gets symlinked in rather than teaching the server about a
# second directory.
webrtc_link_maps() {
    local map="$1" bundle="$2" mapfile
    for mapfile in "$(dirname "$map")"/*.map; do
        [ -f "$mapfile" ] && ln -sf "$mapfile" "$bundle/$(basename "$mapfile")"
    done
    return 0
}

webrtc_install_playwright() {
    local web_dir="$1"
    echo "::group::Installing NPM/Playwright dependencies"
    (
        cd "$web_dir" || exit 1
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
}

# Polling a TCP connect is more reliable than grepping the gateway's
# "listening on" line, which it prints just before ListenAndServe rather
# than after the socket is bound.
webrtc_wait_for_port() {
    local port="$1" timeout="$2" pid="$3" label="$4"
    local _
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

# Dumps a file (or its tail, if $3 is a line count) inside a collapsed
# GitHub Actions group.
webrtc_dump() {
    local label="$1" file="$2" lines="${3:-}"
    echo "::group::$label"
    if [ -n "$lines" ]; then
        tail -n "$lines" "$file" 2>/dev/null
    else
        cat "$file" 2>/dev/null
    fi
    echo "::endgroup::"
    return 0
}

# Opt-in gateway build, same convention as the BUILD flag: the caller owns
# keeping the binary current (a stale one fails later with a confusing
# "flag provided but not defined" instead of here).
webrtc_build_gateway() {
    local gateway_dir="$1" go="${2:-go}"
    [ "${BUILD_GATEWAY:-0}" = "0" ] && return 0
    echo "::group::Building webrtc-gateway"
    if ! command -v "$go" >/dev/null 2>&1 && [ ! -x "$go" ]; then
        echo "FAIL: no go toolchain found (set GO=/path/to/go)"
        echo "::endgroup::"
        return 2
    fi
    (cd "$gateway_dir" && "$go" build -o gateway .) || {
        echo "FAIL: gateway build failed"
        echo "::endgroup::"
        return 2
    }
    echo "::endgroup::"
    return 0
}

webrtc_find_go() {
    local go="${GO:-go}"
    if ! command -v "$go" >/dev/null 2>&1 && [ -x "$HOME/local/go/bin/go" ]; then
        go="$HOME/local/go/bin/go"
    fi
    echo "$go"
}

# Starts a local gateway and waits for its port. Sets WEBRTC_GW_PID.
webrtc_start_gateway() {
    local bin="$1" port="$2" log="$3" timeout="${4:-30}"
    # Anything past the fixed four is passed to the gateway verbatim.
    shift $(( $# > 4 ? 4 : $# ))
    echo "Starting gateway..."
    # -admin-port :0 keeps the SSH admin listener off a fixed port, so a
    # stray gateway from an earlier run can't make this one fail to bind.
    # A caller-supplied -admin-port later in the args still wins.
    "$bin" -listen ":$port" -admin-port ":0" "$@" > "$log" 2>&1 &
    WEBRTC_GW_PID=$!
    if ! webrtc_wait_for_port "$port" "$timeout" "$WEBRTC_GW_PID" "gateway"; then
        webrtc_dump "gateway.log (tail)" "$log" 40
        return 1
    fi
    echo "Gateway is listening."
    return 0
}

# Builds the server invocation into WEBRTC_SERVER_CMD/WEBRTC_SERVER_ENV.
# SERVER_BINARY runs a prebuilt binary directly (CI has no local build
# tree for `cb run`), optionally through SERVER_LD -- a specific
# loader/sysroot, which .github/tests/test_blam_graphics.sh established as
# the only reliable way to get a working headless Mesa/EGL stack in CI.
webrtc_server_command() {
    local target="$1"
    shift
    if [ -n "${SERVER_BINARY:-}" ]; then
        if [ -n "${SERVER_LD:-}" ]; then
            WEBRTC_SERVER_CMD=("$SERVER_LD" --library-path "${SERVER_LIB_PATH:-}" "$SERVER_BINARY")
        else
            WEBRTC_SERVER_CMD=("$SERVER_BINARY")
        fi
        WEBRTC_SERVER_CMD+=("$@")
    else
        WEBRTC_SERVER_CMD=(./cb run "$target" -- "$@")
    fi
    WEBRTC_SERVER_ENV=()
    if [ -n "${SERVER_LIB_PATH:-}" ]; then
        WEBRTC_SERVER_ENV=(env "LD_LIBRARY_PATH=$SERVER_LIB_PATH")
    fi
    return 0
}

# Runs WEBRTC_SERVER_CMD in the background. Sets WEBRTC_SERVER_PID.
webrtc_start_server() {
    local log="$1" tmpdir="$2" dummy_plug_config="$3" run_timeout="$4"
    TMPDIR="$tmpdir" \
    DUMMY_PLUG_CONFIG="$dummy_plug_config" \
    COFFEE_DISABLE_PROFILER=1 \
    timeout "${run_timeout}s" "${WEBRTC_SERVER_ENV[@]}" "${WEBRTC_SERVER_CMD[@]}" \
        > "$log" 2>&1 &
    WEBRTC_SERVER_PID=$!
    return 0
}

# Waits for the fleet registration's return-routability challenge to pass.
# This is the one boot signal worth gating on: it means the gateway has
# accepted the server AND can reach it.
webrtc_wait_for_registration() {
    local log="$1" server_id="$2" timeout="$3" pid="$4"
    local _
    echo "Waiting for the registration challenge to pass (up to ${timeout}s)..."
    for _ in $(seq 1 "$timeout"); do
        if grep -q "registration active for serverId=$server_id" "$log" 2>/dev/null; then
            echo "Server registration is active."
            return 0
        fi
        if ! kill -0 "$pid" 2>/dev/null; then
            echo "FAIL: server process exited before its registration became active"
            webrtc_dump "server.log (tail)" "$log" 40
            return 1
        fi
        sleep 1
    done
    echo "FAIL: registration did not become active within ${timeout}s"
    webrtc_dump "server.log (tail)" "$log" 60
    return 1
}

# Drives the headless-Chromium wasm client. Returns its exit code: 0 means
# all four connect/join/roster-sync markers were seen.
webrtc_run_wasm_client() {
    local bundle="$1" out_dir="$2" url="$3" smoke="$4"
    BUNDLE_DIR="$bundle" \
    OUT_DIR="$out_dir" \
    SERVER_URL="$url" \
    BOOT_TIMEOUT_MS="${BOOT_TIMEOUT_MS:-45000}" \
    CONNECT_TIMEOUT_MS="${CONNECT_TIMEOUT_MS:-60000}" \
    node "$smoke"
}

# The server-side log lines worth surfacing in CI, collapsed.
webrtc_dump_server_lines() {
    local log="$1"
    echo "::group::server.log (registration + connection lines)"
    grep -E "gateway_fleet_registration|Connection info|Player joined|state change|Error|error" \
        "$log" 2>/dev/null || true
    echo "::endgroup::"
    return 0
}
