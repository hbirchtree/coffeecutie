#!/bin/bash

SRCDIR=$(realpath $(dirname $0))/../..
TESTDIR=/tmp/BlamTest_$RANDOM

export LIBGL_ALWAYS_SOFTWARE=1
export GALLIUM_DRIVER=llvmpipe
export DUMMY_PLUG_CONFIG=$(realpath $(dirname $0))/${DUMMY_PLUG:-dummy_plug.json}

mkdir -p $TESTDIR
pushd $TESTDIR

# Acquire the sysroot for the desktop x86_64 platform
gh release -R hbirchtree/coffeecutie-automation-tools download v2.3 -p desktop_x86_64-buildroot-linux-gnu_*_target.tar.xz
mkdir -p sysroot/lib full-sysroot
cd full-sysroot
tar xf ../*.tar.xz
cd ..
rm *.tar.xz

# Diagnostic: check sysroot structure
echo "::group::Sysroot structure"
find full-sysroot -maxdepth 3
echo "::endgroup::"

# Set up a more complete lib directory
# We link all .so files from the sysroot to ensure nothing is missed
echo "::group::Linking sysroot libraries"
find $PWD/full-sysroot/usr/lib -maxdepth 1 -name "*.so*" -exec ln -sf {} $PWD/sysroot/lib/ \;
find $PWD/full-sysroot/lib -maxdepth 1 -name "*.so*" -exec ln -sf {} $PWD/sysroot/lib/ \;
echo "::endgroup::"

# Ensure we have the DRI path set correctly
DRI_PATH=$(find $PWD/full-sysroot -name dri -type d | head -n 1)
SYS_LD=$(find $PWD/full-sysroot -name "ld-linux-x86-64.so.2" | head -n 1)

export LIBGL_DRIVERS_PATH=$DRI_PATH
export MESA_LOADER_DRIVER_PATH=$DRI_PATH
# We don't export LD_LIBRARY_PATH globally because it breaks host tools
# like ls, grep, etc. which might be called by the script or xvfb-run
# and they would try to load the sysroot's libc.
# export LD_LIBRARY_PATH=$PWD/sysroot/lib 
export EGL_LOG_LEVEL=debug
export LIBGL_DEBUG=verbose
export MESA_DEBUG=1
export LIBGL_DRI3_DISABLE=1
export EGL_PLATFORM=surfaceless

echo "===================================="
echo "::group::EGL info"
# Try with host loader first
eglinfo -v
echo "::endgroup::"
echo "===================================="

BUILDDIR=$SRCDIR/multi_build/desktop-x86_64-buildroot-linux-gnu-multi

# Flatten artifacts if they were uploaded with full paths (common in upload-artifact@v4+)
if [ -d "$BUILDDIR/source" ]; then
    echo "Flattening deep artifact structure..."
    mv $BUILDDIR/source/multi_build/desktop-x86_64-buildroot-linux-gnu-multi/install/* $BUILDDIR/
fi

# Fix BUILDDIR if it contains an 'install' directory
if [ -d "$BUILDDIR/install" ]; then
    BUILDDIR=$BUILDDIR/install
fi

echo "===================================="
echo "::group::Artifact structure"
ls -R $BUILDDIR
echo "::endgroup::"

# Find the actual binary
BINARY=$(find $BUILDDIR -name BlamGraphics -type f | head -n 1)

if [ -f "$BINARY" ]; then
    echo "===================================="
    echo "::group::Library linkage"
    if [ -n "$SYS_LD" ]; then
        $SYS_LD --library-path $PWD/sysroot/lib --list $BINARY
    else
        ldd $BINARY
    fi
    echo "::endgroup::"
fi

APPDIR=$BUILDDIR/packaged/linux-appdir/blam_graphics.AppDir
if [ -d "$APPDIR" ]; then
    chmod +x \
        $APPDIR/AppRun \
        $APPDIR/BlamGraphics \
        $APPDIR/CrashRecovery
fi

echo "===================================="
echo "::group::Downloading test assets"
mkdir -p maps/pc/
wget -q -O maps/pc/beavercreek.map --header="Authorization: $MAP_ACCESS_TOKEN" https://maps.speen.dev/pc/beavercreek.map
wget -q -O maps/pc/bloodgulch.map  --header="Authorization: $MAP_ACCESS_TOKEN" https://maps.speen.dev/pc/bloodgulch.map
wget -q -O maps/pc/bitmaps.map     --header="Authorization: $MAP_ACCESS_TOKEN" https://maps.speen.dev/pc/bitmaps.map
wget -q -O maps/pc/sounds.map      --header="Authorization: $MAP_ACCESS_TOKEN" https://maps.speen.dev/pc/sounds.map
echo "::endgroup::"
echo "===================================="

# Find assets directory
ASSETS_DIR=$(find $BUILDDIR -name assets -type d | head -n 1)

# Create a temp dir for this test
# So we know what's new on reruns
export TMPDIR=$TESTDIR/tmp
mkdir -p $TMPDIR

BLAM_TEST_OUTPUT_DIR="$TMPDIR"
if [ -n "$GITHUB_ENV" ]; then
    echo "BLAM_TEST_OUTPUT_DIR=$BLAM_TEST_OUTPUT_DIR" >> "$GITHUB_ENV"
fi

# BOOT_MAP: which downloaded map to launch with (default beavercreek, the
# original single-process smoke-test map). The networking scenario boots
# bloodgulch and switches to beavercreek mid-session via its dummy plug
# config.
BOOT_MAP=${BOOT_MAP:-beavercreek.map}

if [[ $BOOT_MAP != /* ]]; then
    BOOT_MAP=$TESTDIR/maps/pc/$BOOT_MAP
fi

echo "::group::Runtime environment"
env
echo "Directory: $TESTDIR"
echo "Binary:    $BINARY"
echo "::endgroup::"

# Turn off profiler; it fills the disk with this kind of test
export COFFEE_DISABLE_PROFILER=1

if [ -n "$SYS_LD" ] && [ -f "$BINARY" ]; then
    echo "::group::Running binary directly with sysroot loader"
    LD_LIBRARY_PATH=$PWD/sysroot/lib $SYS_LD --library-path $PWD/sysroot/lib $BINARY $ASSETS_DIR $BOOT_MAP 2>&1 | tee "$TESTDIR/output.log"
    echo "Return code: $?"
    echo "::endgroup::"
elif [ -d "$APPDIR" ]; then
    echo "::group::Running via AppRun"
    $APPDIR/AppRun $BOOT_MAP 2>&1 | tee "$TESTDIR/output.log"
    echo "Return code: $?"
    echo "::endgroup::"
else
    echo "ERROR: Could not find a way to run the application"
    exit 1
fi

popd
