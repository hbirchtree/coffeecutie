#!/bin/bash

function build_info()
{
    $(dirname $0)/buildinfo.py $@ 2>/dev/null
}

TRAVIS_OS_NAME=linux
BUILDVARIANT=$1
DEPENDENCIES="$(build_info dependencies)"
DEPENDENCIES="$(echo $DEPENDENCIES | sed -e 's/ /%/g')"
MAKEFILE_DIR=$(build_info makefile_location)
SCRIPT_DIR=

mkdir -p "$PWD/multi_build"

if [ ! -z $LOCALLIB ]; then
    echo " * Using local library build"
    NODEPS=1
    [ ! -d "$PWD/multi_build/coffee_lib" ] && ln -s $LOCALLIB "$PWD/multi_build/coffee_lib"
fi

[ -z $MAKEFILE_DIR ] && echo "No Makefile directory found" && exit 1

export TRAVIS_OS_NAME
export BUILDVARIANT
export DEPENDENCIES
export MAKEFILE_DIR
export NODEPS

$(build_info script_location)/travis-build.sh
