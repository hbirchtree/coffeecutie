#!/bin/bash
# Orchestrator for the synthetic-pose arm retargeting test.
#
# Usage:
#   run_pose_demo_arms.sh [BUNDLE_DIR] [OUT_DIR]
#
# BUNDLE_DIR  directory containing BlamPoseDemo.html (the emscripten bundle).
#             Default: first BlamPoseDemo.bundle found under multi_build/web-*.
#   OUT_DIR   where to write <case>.png + output.log.
#             Default: /tmp/pose_demo_arms
#
# Env passthrough: SETTLE_SECONDS, CASE_SECONDS, BOOT_TIMEOUT_MS, VIEW_W,
#   VIEW_H, CAMERA_POS (JSON triple, e.g. "[1.5,0,0.6]").
#
# Runs under Xvfb with a real GL backend on purpose: SwiftShader loses the
# WebGL context on this bundle, and everything after that is fallout rather
# than a result.

set -euo pipefail
HERE="$(cd "$(dirname "$0")" && pwd)"
SRCDIR="$(cd "$HERE/../../.." && pwd)"

BUNDLE_DIR="${1:-}"
OUT_DIR="${2:-/tmp/pose_demo_arms}"

if [ -z "$BUNDLE_DIR" ]; then
    BUNDLE_DIR="$(find "$SRCDIR/multi_build" -type d -name BlamPoseDemo.bundle 2>/dev/null \
        | grep -E 'web-.*emscripten' | head -n 1)"
fi

if [ -z "$BUNDLE_DIR" ] || [ ! -f "$BUNDLE_DIR/BlamPoseDemo.html" ]; then
    echo "ERROR: could not find a BlamPoseDemo.bundle. Pass BUNDLE_DIR explicitly." >&2
    exit 2
fi

BUNDLE_DIR="$(realpath "$BUNDLE_DIR")"
mkdir -p "$OUT_DIR"
OUT_DIR="$(realpath "$OUT_DIR")"

echo "Bundle : $BUNDLE_DIR"
echo "Output : $OUT_DIR"

cd "$HERE"
if [ ! -d node_modules/playwright ]; then
    npm install --no-audit --no-fund
fi
if [ "${CI:-}" = "true" ]; then
    npx playwright install --with-deps chromium
else
    npx playwright install chromium
fi

export BUNDLE_DIR OUT_DIR
exec xvfb-run -a -s "-screen 0 1280x1024x24" node "$HERE/pose_demo_arms.mjs"
