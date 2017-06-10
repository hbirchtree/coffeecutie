#!/bin/bash

SOURCE_DIR="$PWD"
BUILD_DIR="$SOURCE_DIR/multi_build"

CI_DIR="$SOURCE_DIR/tools/makers"

mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

function die()
{
    echo " * " $@
    exit 1
}

function notify()
{
    echo " * " $@
}

function build_standalone()
{
    make -f "$CI_DIR/Makefile.standalone" \
        -e SOURCE_DIR="$SOURCE_DIR" $@

    # We want to exit if the Make process fails horribly
    # Should also signify to Travis/CI that something went wrong
    EXIT_STAT=$?
    [[ ! "$EXIT_STAT" = 0 ]] && die "Make process failed"
}

function build_mac()
{
    download_libraries_mac $COFFEE_SLUG

    make -f "$CI_DIR/Makefile.mac" \
        -e SOURCE_DIR="$SOURCE_DIR" $@

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
