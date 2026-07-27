#!/bin/bash
# Shared harness for the webrtc-gateway integration tests. Source it:
#
#   . "$(dirname "$0")/webrtc_test_lib.sh"
#
# Its callers all drive the same topology -- one gateway, one
# fleet-registered server, one client -- and differ only in where the
# gateway lives and what plays each role. Only those differences belong in
# the individual scripts.
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

# The browser tests' static server only serves the bundle dir, and the
# wasm build asks for maps by bare relative path -- so every .map next to
# $1 gets symlinked in rather than teaching the server about a second
# directory.
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

# Opt-in engine build, gated on BUILD like the gateway's is on
# BUILD_GATEWAY. Returns 2 on failure so callers can exit straight through.
webrtc_build_target() {
    local target="$1"
    [ "${BUILD:-0}" = "0" ] && return 0
    echo "::group::Building $target"
    BUILD_HOST_TOOLS=0 COFFEE_DISABLE_PROFILER=1 ./cb build "$target" || {
        echo "FAIL: build failed"
        echo "::endgroup::"
        return 2
    }
    echo "::endgroup::"
    return 0
}

# Creates the per-run directory layout every test uses and exports the
# paths as globals: WEBRTC_SERVER_TMP/WEBRTC_CLIENT_TMP (each process needs
# its own TMPDIR -- the journal and state dump land there),
# WEBRTC_SERVER_LOG/WEBRTC_CLIENT_LOG/WEBRTC_GATEWAY_LOG and the two
# journal paths. Stale logs are removed so a re-run can't be read as fresh.
webrtc_prepare_out_dir() {
    local out_dir="$1"
    WEBRTC_SERVER_TMP="$out_dir/server_tmp"
    WEBRTC_CLIENT_TMP="$out_dir/client_tmp"
    mkdir -p "$WEBRTC_SERVER_TMP" "$WEBRTC_CLIENT_TMP"
    WEBRTC_SERVER_LOG="$out_dir/server.log"
    WEBRTC_CLIENT_LOG="$out_dir/client.log"
    WEBRTC_GATEWAY_LOG="$out_dir/gateway.log"
    WEBRTC_SERVER_JOURNAL="$WEBRTC_SERVER_TMP/journal.jsonl"
    WEBRTC_CLIENT_JOURNAL="$WEBRTC_CLIENT_TMP/journal.jsonl"
    rm -f "$WEBRTC_SERVER_LOG" "$WEBRTC_CLIENT_LOG" "$WEBRTC_GATEWAY_LOG" \
          "$WEBRTC_SERVER_JOURNAL" "$WEBRTC_CLIENT_JOURNAL"
    return 0
}

# Kills every PID this harness may have started. Callers trap it on EXIT.
webrtc_cleanup_pids() {
    local pid
    for pid in "${WEBRTC_CLIENT_PID:-}" "${WEBRTC_SERVER_PID:-}" "${WEBRTC_GW_PID:-}"; do
        [ -n "$pid" ] && kill "$pid" 2>/dev/null
    done
    return 0
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
# loader/sysroot, which is the only reliable way to get a working headless
# Mesa/EGL stack in CI.
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

# The native-client counterpart: same launcher, its own TMPDIR (journal and
# state dump land there) and its own connect args. Sets WEBRTC_CLIENT_PID.
webrtc_start_native_client() {
    local target="$1" resource_dir="$2" map="$3" log="$4" tmpdir="$5"
    local dummy_plug_config="$6" run_timeout="$7"
    shift 7
    # set -u is on in the callers, and the server command may not have been
    # built yet in a client-only run.
    local saved_cmd=("${WEBRTC_SERVER_CMD[@]:-}")
    webrtc_server_command "$target" "$resource_dir" "$map" "$@"
    TMPDIR="$tmpdir" \
    DUMMY_PLUG_CONFIG="$dummy_plug_config" \
    COFFEE_DISABLE_PROFILER=1 \
    timeout "${run_timeout}s" "${WEBRTC_SERVER_ENV[@]}" "${WEBRTC_SERVER_CMD[@]}" \
        > "$log" 2>&1 &
    WEBRTC_CLIENT_PID=$!
    # The server's own invocation is still needed by nothing after this
    # point, but restoring it keeps the global honest for later callers.
    WEBRTC_SERVER_CMD=("${saved_cmd[@]}")
    return 0
}

# Greps one log for the lines worth seeing, inside a collapsed group.
webrtc_dump_matching() {
    local label="$1" file="$2" pattern="$3"
    echo "::group::$label"
    grep -E "$pattern" "$file" 2>/dev/null || true
    echo "::endgroup::"
    return 0
}

# Both native tests end the same way: fail if either journal is missing,
# diff them, and on a mismatch print the merged timeline. Returns the
# comparison's exit code.
webrtc_compare_journals() {
    local server_journal="$1" client_journal="$2" compare_script="$3"
    local journal
    for journal in "$server_journal" "$client_journal"; do
        if [ ! -s "$journal" ]; then
            echo "FAIL: $journal was never written (journal disabled, or the process died at startup?)"
            return 1
        fi
    done

    echo
    echo "=== Journal comparison ==="
    echo "server: $server_journal"
    echo "client: $client_journal"
    echo "::group::journal comparison"
    python3 "$compare_script" "$server_journal" "$client_journal"
    local result=$?
    echo "::endgroup::"
    if [ "$result" != "0" ]; then
        echo
        echo "::group::merged journal timeline"
        python3 "$compare_script" --timeline "$server_journal" "$client_journal" || true
        echo "::endgroup::"
    fi
    return $result
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
# all connect/join/roster-sync markers were seen. Its output goes to a file
# (path in WEBRTC_CLIENT_LOG) rather than the terminal: every browser
# console line would otherwise stream past ungrouped, and callers dump it
# collapsed on failure anyway.
webrtc_run_wasm_client() {
    local bundle="$1" out_dir="$2" url="$3" smoke="$4"
    mkdir -p "$out_dir"
    WEBRTC_CLIENT_LOG="$out_dir/console.log"
    echo "Browser console -> $WEBRTC_CLIENT_LOG"
    BUNDLE_DIR="$bundle" \
    OUT_DIR="$out_dir" \
    SERVER_URL="$url" \
    BOOT_TIMEOUT_MS="${BOOT_TIMEOUT_MS:-45000}" \
    CONNECT_TIMEOUT_MS="${CONNECT_TIMEOUT_MS:-60000}" \
    node "$smoke" > "$WEBRTC_CLIENT_LOG" 2>&1
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
