#!/usr/bin/env bash
# Smoke test: boot an iOS simulator, launch BlamGraphics and wait for frames.
#
# Usage: run_simulator_smoke.sh <BlamGraphics.app> <output dir> [map path]
#
# Passes once a frame is logged, after the map has loaded when one is given.
# Fails if the app exits first or RUN_TIMEOUT (seconds) runs out.
# The console log and a screenshot land in the output dir either way.
set -euo pipefail

APP="$1"
OUT="$2"
MAP="${3:-}"
TIMEOUT="${RUN_TIMEOUT:-600}"

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

# With a map, only frames after it finished loading count
rendered() {
    if [ -n "$MAP" ]; then
        awk '/Starting MapFinished handler/ { loaded = 1 }
             loaded && /ComponentApp: FPS: [0-9]/ { found = 1 }
             END { exit !found }' "$LOG"
    else
        grep -q "ComponentApp: FPS: [0-9]" "$LOG"
    fi
}

STATUS=1
for ((i = 0; i < TIMEOUT; i++)); do
    if rendered; then
        STATUS=0
        break
    fi
    if ! kill -0 "$LAUNCHER" 2>/dev/null; then
        echo "::error::BlamGraphics exited before rendering"
        break
    fi
    sleep 1
done
if [ "$STATUS" -ne 0 ] && [ "$i" -ge "$TIMEOUT" ]; then
    echo "::error::No frames within ${TIMEOUT}s"
fi

xcrun simctl io "$DEVICE" screenshot "$OUT/screenshot.png" || true
xcrun simctl terminate "$DEVICE" "$BUNDLE_ID" >/dev/null 2>&1 || true
wait "$LAUNCHER" || true

grep -E "GL version|FPS:|FTAL|ERRO" "$LOG" | tail -n 20 || true
exit "$STATUS"
