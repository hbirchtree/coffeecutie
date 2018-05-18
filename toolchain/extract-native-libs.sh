#!/bin/bash

CURRENT_DIR=$(dirname $(dirname $(realpath $0)))

NATIVE_LIB_DIR="$PWD"

if [ ! -z "$1" ]; then
  NATIVE_LIB_DIR="$1"
fi

TOOLCHAIN="ubuntu.amd64"

TARGET_TOOLCHAIN="Ubuntu"
TARGET_LIB_PATH="x86_64-linux-gnu"

LIB_PATH="$NATIVE_LIB_DIR/$TARGET_TOOLCHAIN/lib/$TARGET_LIB_PATH"
INC_PATH="$NATIVE_LIB_DIR/$TARGET_TOOLCHAIN/include"

SRC_PREFIX=$CURRENT_DIR/multi_build/build-dependencies/*/install/$TOOLCHAIN/

ALL_LIB_SRCS=$SRC_PREFIX/lib/*
ALL_INC_SRCS=$SRC_PREFIX/include/*

cp $ALL_LIB_SRCS -t $LIB_PATH
cp -r $ALL_INC_SRCS -t $INC_PATH
