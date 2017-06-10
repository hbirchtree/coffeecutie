#!/bin/bash

SOURCE_DIR="$PWD"
BUILD_DIR="$SOURCE_DIR/multi_build"

CI_DIR="$SOURCE_DIR/ci"

COFFEE_DIR="$BUILD_DIR/coffee_lib"

mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

QTHUB_DOCKER="hbirch/coffeecutie:qthub-client"
COFFEE_SLUG="hbirchtree/coffeecutie"

function die()
{
    echo " * " $@
    exit 1
}

function notify()
{
    echo " * " $@
}

function github_api()
{
    docker run --rm -v $PWD:/data $QTHUB_DOCKER --api-token "$GITHUB_TOKEN" $@
}

function download_libraries()
{
    notify "Downloading library ${1}:${BUILDVARIANT}"
    local LATEST_RELEASE="$(github_api list release ${1} | head -1 | cut -d'|' -f 3)"
    local CURRENT_ASSET="$(github_api list asset ${1}:${LATEST_RELEASE} | grep $BUILDVARIANT)"

    [[ -z $CURRENT_ASSET ]] && die "Failed to find library release for $BUILDVARIANT"

    notify "Found assets: $CURRENT_ASSET (from $LATEST_RELEASE)"
    local ASSET_ID="$(echo $CURRENT_ASSET | cut -d'|' -f 3)"
    local ASSET_FN="$(echo $CURRENT_ASSET | cut -d'|' -f 5)"

    github_api pull asset $1 $ASSET_ID

    tar -xvf "$ASSET_FN"
    mv build $COFFEE_DIR
}

function build_standalone()
{
    download_libraries $COFFEE_SLUG

    make -f "$CI_DIR/Makefile.standalone" \
        -e SOURCE_DIR="$SOURCE_DIR" \
        -e COFFEE_DIR="$COFFEE_DIR" $@

    # We want to exit if the Make process fails horribly
    # Should also signify to Travis/CI that something went wrong
    EXIT_STAT=$?
    [[ ! "$EXIT_STAT" = 0 ]] && die "Make process failed"
}

function build_mac()
{
    download_libraries_mac $COFFEE_SLUG

    make -f "$CI_DIR/Makefile.mac" \
        -e SOURCE_DIR="$SOURCE_DIR" \
        -e COFFEE_DIR="$COFFEE_DIR" $@

    EXIT_STAT=$?
    [[ ! "$EXIT_STAT" = 0 ]] && die "Make process failed"
}

case "${TRAVIS_OS_NAME}" in
"linux")
    build_standalone "$BUILDVARIANT"

    tar -zcvf "$TRAVIS_BUILD_DIR/libraries_$BUILDVARIANT.tar.gz" -C ${BUILD_DIR} build/
;;
"osx")
    build_mac "$BUILDVARIANT"

    tar -zcvf "$TRAVIS_BUILD_DIR/libraries_$BUILDVARIANT.tar.gz" -C ${BUILD_DIR} build/
;;
esac
