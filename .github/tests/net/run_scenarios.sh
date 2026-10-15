#!/bin/bash
# Runs client/server scenarios from scenarios.json, each through
# run_client_server_test.sh, and sums up which passed.
#
# Usage:
#   run_scenarios.sh [OUT_DIR] [SCENARIO[@NETWORK]...]
#
# Without names, every scenario with a server and a client plug runs, once
# under each network profile it lists (lan if none); a bare name runs all
# of its profiles. Each run writes to OUT_DIR/<name>@<network>, its output
# to OUT_DIR/<name>@<network>.log (default
# OUT_DIR: /tmp/net_scenarios). The same
# environment overrides as run_client_server_test.sh apply to all of them;
# BUILD=1 builds once, before the first.

set -uo pipefail
HERE="$(cd "$(dirname "$0")" && pwd)"

OUT_DIR="${1:-/tmp/net_scenarios}"
shift || true
mapfile -t runs < <(python3 - "$HERE/scenarios.json" "$@" <<'PYEOF'
import json, sys
scenarios = json.load(open(sys.argv[1]))["scenarios"]
wanted = sys.argv[2:] or [name for name, s in scenarios.items()
                          if "server" in s and "client" in s]
for want in wanted:
    name, _, network = want.partition("@")
    for net in [network] if network else scenarios.get(name, {}).get("networks", ["lan"]):
        print(f"{name}@{net}")
PYEOF
)

mkdir -p "$OUT_DIR"
failed=()
for name in "${runs[@]}"; do
    echo "=== $name"
    if SCENARIO="${name%@*}" NETWORK="${name#*@}" \
        "$HERE/run_client_server_test.sh" "$OUT_DIR/$name" \
        > "$OUT_DIR/$name.log" 2>&1; then
        echo "PASS $name"
    else
        echo "FAIL $name (log: $OUT_DIR/$name.log)"
        grep -E "^FAIL" "$OUT_DIR/$name.log" | head -5
        failed+=("$name")
    fi
    export BUILD=0
done

echo
if [ "${#failed[@]}" -eq 0 ]; then
    echo "All ${#runs[@]} runs passed"
    exit 0
fi
echo "${#failed[@]} of ${#runs[@]} failed: ${failed[*]}"
exit 1
