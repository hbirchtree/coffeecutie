#!/bin/bash

set -euo pipefail

DOLPHIN_DIR=$(realpath $(dirname $0)/../../../multi_build/runtime/dolphin)
mkdir -p ${DOLPHIN_DIR}/bin ${DOLPHIN_DIR}/config

if [ ! -f ${DOLPHIN_DIR}/bin/dolphin-emu-nogui ]; then
    mkdir -p ${DOLPHIN_DIR}/build
    pushd ${DOLPHIN_DIR}/build
    if [ ! -d ${DOLPHIN_DIR}/src ]; then
        git clone --recurse-submodules https://github.com/dolphin-emu/dolphin.git ${DOLPHIN_DIR}/src
    fi
    cmake ../src \
        -GNinja \
        \
        -DENABLE_HEADLESS=ON \
        \
        -DENABLE_ALSA=OFF \
        -DENABLE_ANALYTICS=OFF \
        -DENABLE_AUTOUPDATE=OFF \
        -DENABLE_BLUEZ=OFF \
        -DENABLE_EVDEV=OFF \
        -DENABLE_PULSEAUDIO=OFF \
        -DENABLE_QT=OFF \
        -DENABLE_TESTS=OFF \
        -DENABLE_VTUNE=OFF \
        -DENABLE_X11=OFF
    ninja
    cp Binaries/dolphin-emu-nogui ../bin
fi
pushd ${DOLPHIN_DIR}/config
mkdir -p User/Config
echo "[Settings]
UseFFV1 = False
DumpCodec = utvideo
" > User/Config/GFX.ini

