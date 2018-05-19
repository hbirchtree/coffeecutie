#!/bin/bash

WDIR=$(dirname $(dirname $(realpath $0)))

CONFIGURATION=Release
NODEPLOY=1

export CONFIGURATION
export NODEPLOY

function git_update()
{
   local SUBJECT_SRC="$WDIR/multi_build/build-depdendencies/$1/source"
   [ -d $SUBJECT_SRC ] && git -C $SUBJECT_SRC pull
}

function qb()
{
  $WDIR/quick-build.sh $@
}

function build_dep()
{
  git_update $1
  qb $1.ubuntu.amd64
}

# Update and build dependencies for tools
build_dep shaderc
build_dep squish
build_dep spirv-cross
build_dep assimp

# Strip generated libraries
qb strip.ubuntu.amd64

# Install libraries locally first
$WDIR/extract-native-libs.sh $WDIR/multi_build/dependencies/native-libs

# Now build the hopefully up-to-date tools
BUILD_MODE=bare qb ubuntu.amd64.tools
