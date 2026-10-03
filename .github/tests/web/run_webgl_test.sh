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
# Env passthrough to the smoke driver: RUN_SECONDS, MIN_FRAMES, BOOT_TIMEOUT_MS,
#   SCREENSHOT_NAME, SCREENSHOT_QUALITY, DUMMY_PLUG.
#
# MAPS: space-separated map paths fetched from the map server into BUNDLE_DIR
#   (default: the PC beavercreek set). Skipped when MAP_ACCESS_TOKEN is unset.

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

# Resolve to absolute paths before we cd into the harness dir, otherwise relative
# args (e.g. "artifacts/bin/BlamGraphics.bundle") would break after the cd.
BUNDLE_DIR="$(realpath "$BUNDLE_DIR")"
mkdir -p "$OUT_DIR"
OUT_DIR="$(realpath "$OUT_DIR")"

echo "Bundle : $BUNDLE_DIR"
echo "Output : $OUT_DIR"

echo "::group::Installing NPM/Playwright dependencies"
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
echo "::endgroup::"

MAPS="${MAPS:-pc/beavercreek.map pc/bitmaps.map pc/sounds.map}"
if [ -n "${MAP_ACCESS_TOKEN:-}" ]; then
    echo "::group::Downloading test assets"
    for map in $MAPS; do
        mkdir -p "$BUNDLE_DIR/$(dirname "$map")"
        wget -q -O "$BUNDLE_DIR/$map" --header="Authorization: $MAP_ACCESS_TOKEN" "https://maps.speen.dev/$map"
    done
    echo "::endgroup::"
else
    echo "MAP_ACCESS_TOKEN not set, running without maps"
fi

export BUNDLE_DIR OUT_DIR
exec node webgl_smoke.mjs
