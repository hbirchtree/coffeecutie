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
    $PWD/full-sysroot/usr/lib/libOSMesa* \
    $PWD/full-sysroot/usr/lib/libbacktrace* \
    sysroot/lib/

export LD_LIBRARY_PATH=$PWD/sysroot/lib

BUILDDIR=$SRCDIR/multi_build/desktop-x86_64-buildroot-linux-gnu-multi

$BUILDDIR/bin/BlamGraphics $BUILDDIR/examples/blam/cblam-testing/assets $MAPDIR/pc/bloodgulch.map

pushd /tmp/Blam\ Graphics/
tar Jcvf $SRCDIR/../blam_test_results.tar.xz .
popd

popd
