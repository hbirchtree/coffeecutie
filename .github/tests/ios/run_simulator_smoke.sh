#!/usr/bin/env bash
# Smoke test: boot an iOS simulator, launch BlamGraphics and wait for frames.
#
# Usage: run_simulator_smoke.sh <BlamGraphics.app> <output dir> [map path]
#
# Frames must show up within RUN_TIMEOUT seconds (after the map has loaded,
# when one is given), and the app must keep rendering for SETTLE_SECONDS.
# The console log, a screenshot and any crash reports land in the output dir.
set -euo pipefail

APP="$1"
OUT="$2"
MAP="${3:-}"
TIMEOUT="${RUN_TIMEOUT:-600}"
SETTLE="${SETTLE_SECONDS:-20}"

mkdir -p "$OUT"
LOG="$OUT/console.log"

DEVICE="${SIMULATOR_UDID:-}"
if [ -z "$DEVICE" ]; then
    DEVICE=$(xcrun simctl list devices available -j | python3 -c '
import json, sys
devices = json.load(sys.stdin)["devices"]
for runtime, entries in sorted(devices.items(), reverse=True):
    if "iOS" not in runtime:
        continue
    for d in entries:
        if d["name"].startswith("iPhone"):
            print(d["udid"])
            sys.exit(0)
')
fi
if [ -z "$DEVICE" ]; then
    echo "::error::No iPhone simulator available"
    exit 1
fi

BUNDLE_ID=$(/usr/libexec/PlistBuddy -c "Print :CFBundleIdentifier" "$APP/Info.plist")
echo "Device $DEVICE, bundle $BUNDLE_ID, map '${MAP:-none}'"

xcrun simctl bootstatus "$DEVICE" -b > "$OUT/boot.log"
trap 'xcrun simctl shutdown "$DEVICE" >/dev/null 2>&1 || true' EXIT
xcrun simctl install "$DEVICE" "$APP"

# --console-pty keeps the app's stdout line-buffered
xcrun simctl launch --console-pty --terminate-running-process \
    "$DEVICE" "$BUNDLE_ID" ${MAP:+"$MAP"} > "$LOG" 2>&1 &
LAUNCHER=$!

# Frame reports so far; with a map, only those after it finished loading
frames() {
    if [ -n "$MAP" ]; then
        awk '/Starting MapFinished handler/ { loaded = 1 }
             loaded && /ComponentApp: FPS: [0-9]/ { n++ }
             END { print n + 0 }' "$LOG"
    else
        grep -c "ComponentApp: FPS: [0-9]" "$LOG" || true
    fi
}

alive() {
    kill -0 "$LAUNCHER" 2>/dev/null
}

STATUS=1
FIRST=0
for ((i = 0; i < TIMEOUT; i++)); do
    if ! alive; then
        echo "::error::BlamGraphics exited before rendering"
        break
    fi
    FIRST=$(frames)
    if [ "$FIRST" -gt 0 ]; then
        STATUS=0
        break
    fi
    sleep 1
done
if [ "$STATUS" -ne 0 ] && [ "$i" -ge "$TIMEOUT" ]; then
    echo "::error::No frames within ${TIMEOUT}s"
fi

# The first frame report can come out of the loading frame, so keep watching
if [ "$STATUS" -eq 0 ]; then
    for ((i = 0; i < SETTLE; i++)); do
        sleep 1
        if ! alive; then
            echo "::error::BlamGraphics exited after its first frames"
            STATUS=1
            break
        fi
    done
    if [ "$STATUS" -eq 0 ] && [ "$(frames)" -le "$FIRST" ]; then
        echo "::error::BlamGraphics stopped rendering after its first frames"
        STATUS=1
    fi
fi

xcrun simctl io "$DEVICE" screenshot "$OUT/screenshot.png" || true
xcrun simctl terminate "$DEVICE" "$BUNDLE_ID" >/dev/null 2>&1 || true
wait "$LAUNCHER" || true

# Simulator app crashes are reported on the host, as are the Metal host's
for reports in "$HOME/Library/Logs/DiagnosticReports" /Library/Logs/DiagnosticReports; do
    find "$reports" -newer "$OUT/boot.log" -type f 2>/dev/null >> "$OUT/new_reports.txt" || true
    find "$reports" -newer "$OUT/boot.log" \
        \( -name "BlamGraphics*" -o -name "*Metal*" \) \
        -exec cp {} "$OUT/" \; 2>/dev/null || true
done
# SimMetalHost runs on the host, its side of the story is in the host log
log show --style compact --last 30m \
    --predicate 'process CONTAINS "SimMetalHost" OR eventMessage CONTAINS "SimMetalHost"' \
    > "$OUT/metalhost.log" 2>&1 || true
# The app's own stacktrace.json and friends
CONTAINER=$(xcrun simctl get_app_container "$DEVICE" "$BUNDLE_ID" data 2>/dev/null || true)
if [ -n "$CONTAINER" ]; then
    find "$CONTAINER/tmp" -maxdepth 1 -name "*.json" -exec cp {} "$OUT/" \; 2>/dev/null || true
fi
# Why the system ended the process, if it did
xcrun simctl spawn "$DEVICE" log show --style compact --last 30m \
    --predicate 'process == "BlamGraphics" OR eventMessage CONTAINS "BlamGraphics"' \
    > "$OUT/system.log" 2>&1 || true

if [ "$STATUS" -ne 0 ]; then
    for report in "$OUT"/*.ips; do
        [ -f "$report" ] || continue
        echo "== $(basename "$report")"
        python3 - "$report" <<'EOF' || true
import json, sys
text = open(sys.argv[1]).read()
report = json.loads(text.split("\n", 1)[1])
print("exception:", report.get("exception"), report.get("termination", {}).get("indicator", ""))
images = report.get("usedImages", [])
thread = next((t for t in report.get("threads", []) if t.get("triggered")), None)
for frame in (thread or {}).get("frames", [])[:30]:
    image = images[frame["imageIndex"]].get("name", "?") if "imageIndex" in frame else "?"
    print(f"  {image} {frame.get('symbol', hex(frame.get('imageOffset', 0)))} +{frame.get('symbolLocation', 0)}")
EOF
    done
    grep -iE "termin|crash|signal|jetsam|watchdog|exit" "$OUT/system.log" | tail -n 20 || true
    echo "== New crash reports"
    cat "$OUT/new_reports.txt" || true
    echo "== SimMetalHost"
    grep -E " (E|F) +SimMetalHost|exited due to|crashed: SimMetalHost" "$OUT/metalhost.log" \
        | head -n 20 || true
fi

grep -E "GL version|FPS:|FTAL|ERRO" "$LOG" | tail -n 20 || true
# Coffee's signal handler prints the stack before it kills the process
grep -A 40 "signal encountered" "$LOG" || true
exit "$STATUS"
