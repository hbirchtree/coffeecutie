#!/bin/sh

ARCH_STRING="none"

case "$SNAP_ARCH" in
@SNAPPY_ARCH_DATA@
esac

EXEC_NAME="$SNAP/bin/$ARCH_STRING/$(basename $0)"

echo "-- System information --"
uname -a
lsb_release -a
echo "------------------------"
echo "--- Snappy variables ---"
env | grep SNAP
echo "------------------------"
echo "Current directory: $(pwd)"
echo "------------------------"
echo "-- Dynamic libraries  --"
ldd "$EXEC_NAME"
echo "------------------------"

[ "$ARCH_STRING" = "none" ] && \
    echo "$SNAP_NAME: Unsupported architecture (SNAP_ARCH=$SNAP_ARCH)" && \
    exit 1

"$EXEC_NAME" "$@"

EXIT_CODE="$?"

[ "$EXIT_CODE" != "0" ] && \
    echo "------------------------" && \
    echo "Exited with status: $EXIT_CODE"

echo "------------------------"
echo "Current directory: $(pwd)"
echo "------------------------"
