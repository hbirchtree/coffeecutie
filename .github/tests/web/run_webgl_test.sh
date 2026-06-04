#!/bin/bash
# Orchestrator for the headless software-rendered WebGL smoke test.
#
# Usage:
#   run_webgl_test.sh [BUNDLE_DIR] [OUT_DIR]
#
# BUNDLE_DIR  directory containing BlamGraphics.html (the emscripten bundle).
#             Default: auto-detect under multi_build/web-*/install/bin or
#             multi_build/web-*/bin (first BlamGraphics.bundle found).
#   OUT_DIR   where to write <name>.jpg + output.log.
#             Default: /tmp/webgl_test/<bundle-variant>
#
# Env passthrough (see webgl_smoke.mjs): RUN_SECONDS, MIN_FRAMES, BOOT_TIMEOUT_MS,
#   SCREENSHOT_NAME, SCREENSHOT_QUALITY, DUMMY_PLUG.

set -euo pipefail
HERE="$(cd "$(dirname "$0")" && pwd)"
SRCDIR="$(cd "$HERE/../../.." && pwd)"

BUNDLE_DIR="${1:-}"
OUT_DIR="${2:-}"

if [ -z "$BUNDLE_DIR" ]; then
    BUNDLE_DIR="$(find "$SRCDIR/multi_build" -type d -name BlamGraphics.bundle 2>/dev/null \
        | grep -E 'web-.*emscripten' | head -n 1)"
fi

if [ -z "$BUNDLE_DIR" ] || [ ! -f "$BUNDLE_DIR/BlamGraphics.html" ]; then
    echo "ERROR: could not find a BlamGraphics.bundle. Pass BUNDLE_DIR explicitly." >&2
    exit 2
fi

if [ -z "$OUT_DIR" ]; then
    VARIANT="$(echo "$BUNDLE_DIR" | grep -oE 'web-[a-z0-9-]+' | head -n 1)"
    OUT_DIR="/tmp/webgl_test/${VARIANT:-webgl}"
fi

echo "Bundle : $BUNDLE_DIR"
echo "Output : $OUT_DIR"

cd "$HERE"
if [ ! -d node_modules/playwright ]; then
    echo "Installing playwright..."
    npm install --no-audit --no-fund
fi
# Install the Chromium browser binary. --with-deps needs root (CI); fall back to plain.
if [ "${CI:-}" = "true" ]; then
    npx playwright install --with-deps chromium
else
    npx playwright install chromium
fi

export BUNDLE_DIR OUT_DIR
exec node webgl_smoke.mjs
