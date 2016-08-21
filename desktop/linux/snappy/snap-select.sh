#!/bin/sh

ARCH_STRING="none"

case "$SNAP_ARCH" in
@SNAPPY_ARCH_DATA@
esac

echo "-- System information --"
uname -a
echo "------------------------"
echo "--- Snappy variables ---"
env | grep SNAP
echo "------------------------"
echo "Current directory: $(pwd)"
echo "------------------------"

[ "$ARCH_STRING" = "none" ] && \
    echo "$SNAP_NAME: Unsupported architecture (SNAP_ARCH=$SNAP_ARCH)" && \
    exit 1

"$SNAP/bin/$ARCH_STRING/@TARGET@"

echo "------------------------"
echo "Current directory: $(pwd)"
echo "------------------------"
