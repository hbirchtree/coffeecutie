#!/bin/bash

CURRENT_DIR=$(dirname $(dirname $(realpath $0)))

NATIVE_LIB_DIR="$PWD"

if [ ! -z "$1" ]; then
  NATIVE_LIB_DIR="$1"
fi

TOOLCHAIN="ubuntu.amd64"

SRC_PREFIX=$CURRENT_DIR/multi_build/$TOOLCHAIN.tools/

ALL_BIN_SRCS=$SRC_PREFIX/bin/*
ALL_LIB_SRCS=$SRC_PREFIX/lib/*.so

BIN_PATH=$NATIVE_LIB_DIR/Tools/$TOOLCHAIN/bin
LIB_PATH=$NATIVE_LIB_DIR/Tools/$TOOLCHAIN/lib

cp $ALL_BIN_SRCS -t $BIN_PATH
cp $ALL_LIB_SRCS -t $LIB_PATH
