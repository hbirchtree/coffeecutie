#!/bin/bash

SRCDIR=$(realpath $(dirname $0))/../..
TESTDIR=/tmp/BlamTest_$RANDOM

export DUMMY_PLUG_CONFIG=$(realpath $(dirname $0))/dummy_plug.json

mkdir -p $TESTDIR
pushd $TESTDIR

# Acquire the sysroot for the desktop x86_64 platform
gh release -R hbirchtree/coffeecutie-automation-tools download v2.1.3 -p desktop_x86_64-buildroot-linux-gnu_*_target.tar.xz
mkdir -p sysroot/lib full-sysroot
cd full-sysroot
tar xf ../*.tar.xz
cd ..
rm *.tar.xz

# Now set up a minimal lib directory for running MESA
ln -s \
    $PWD/full-sysroot/usr/lib/libGL* \
    $PWD/full-sysroot/usr/lib/libEGL* \
    $PWD/full-sysroot/usr/lib/libLLVM* \
    $PWD/full-sysroot/usr/lib/libOSMesa* \
    $PWD/full-sysroot/usr/lib/libbacktrace* \
    $PWD/full-sysroot/usr/lib/libdrm* \
    $PWD/full-sysroot/usr/lib/libglapi* \
    $PWD/full-sysroot/lib/libstdc++* \
    sysroot/lib/

export LD_LIBRARY_PATH=$PWD/sysroot/lib

BUILDDIR=$SRCDIR/multi_build/desktop-x86_64-buildroot-linux-gnu-multi

echo "===================================="
echo "========= Library linkage =========="
ldd $BUILDDIR/bin/BlamGraphics
echo "===================================="
echo "===================================="
ldd $BUILDDIR/bin/BlamGraphics > linkage.txt

echo "===================================="
echo "===== Dowloading test assets ======="
mkdir -p maps/pc/
wget -O maps/pc/beavercreek.map --header="Authorization: $MAP_ACCESS_TOKEN" https://maps.speen.dev/pc/beavercreek.map
wget -O maps/pc/bitmaps.map     --header="Authorization: $MAP_ACCESS_TOKEN" https://maps.speen.dev/pc/bitmaps.map
wget -O maps/pc/sounds.map      --header="Authorization: $MAP_ACCESS_TOKEN" https://maps.speen.dev/pc/sounds.map
echo "===================================="
echo "===================================="

echo "-- " $BUILDDIR/bin/BlamGraphics $BUILDDIR/examples/blam/cblam-testing/assets $PWD/maps/pc/beavercreek.map
mkdir -p "/tmp/Blam Graphics"
$BUILDDIR/bin/BlamGraphics $BUILDDIR/examples/blam/cblam-testing/assets $PWD/maps/pc/beavercreek.map 2>&1 | tee "/tmp/Blam Graphics/output.log"

popd
