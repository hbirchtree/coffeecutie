#!/bin/bash

#######################################
# For potential issues with create directories
#######################################
umask 000

#######################################
# General variables
#######################################
SOURCE_DIR="$PWD"
BUILD_DIR="$SOURCE_DIR/multi_build"

CI_DIR="$SOURCE_DIR/$MAKEFILE_DIR"

COFFEE_DIR="$BUILD_DIR/coffee_lib"

#######################################
# Create build directory and go to it
#######################################
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

#######################################
# Export for scripts
#######################################
export BUILDVARIANT
export MANUAL_DEPLOY
export MANUAL_CONTEXT

QTHUB_DOCKER="hbirch/coffeecutie:qthub-client"
MAKEFILE="Makefile.linux"

INFOPY="$SOURCE_DIR/toolchain/buildinfo.py"

#######################################
# Retrieve script location relative to source dir
#######################################
SCRIPT_DIR="$SOURCE_DIR/$($INFOPY --source-dir $SOURCE_DIR script_location)"

HELPER="$SCRIPT_DIR/travis-helper.py"
GITHUBPY="$SCRIPT_DIR/github_api.py"

function die()
{
    echo " * " $@
    exit 1
}

function notify()
{
    echo " * " $@
}

function debug()
{
    echo $@ > /dev/stderr
}

function requires()
{
    for prog in $@; do
        local mute=$(which $prog)
        if [[ ! "$?" = "0" ]]; then
            die "Could not find program: $prog"
        fi
    done
}

function github_api()
{
    case "${TRAVIS_OS_NAME}" in
    *)
        # Python client
        python3 "$GITHUBPY" --api-token "$GITHUB_TOKEN" $@
    ;;
    "linux")
        # Qt/C++ client
        docker run --rm -v "$PWD:/data" $QTHUB_DOCKER --api-token "$GITHUB_TOKEN" $@
    ;;
    esac
}

function download_libraries()
{
    local slug=$(echo $1 | cut -d':' -f1)

    notify "Downloading library ${slug}:${2}"
    local CONFIG_RELEASE="$($INFOPY --source-dir "$SOURCE_DIR" dependencies ${1})"
    notify "Target version: $CONFIG_RELEASE"

    local ALL_RELEASES=$(github_api list release ${slug} | cut -d'|' -f 3)

    local LATEST_RELEASE=$($HELPER get-closest-release "$CONFIG_RELEASE" $ALL_RELEASES)

    notify "Using release $LATEST_RELEASE for $slug"
    local CURRENT_ASSET="$(github_api list asset ${slug}:${LATEST_RELEASE} | grep "libraries_${2}.tar.gz" | head -1)"
    #echo Asset $CURRENT_ASSET

    [[ -z $CURRENT_ASSET ]] && die "Failed to find ${slug} for $2"

    notify "Found assets: $CURRENT_ASSET (from $LATEST_RELEASE)"
    local ASSET_ID="$(echo $CURRENT_ASSET | cut -d'|' -f 3)"
    local ASSET_FN="$PWD/$(echo $CURRENT_ASSET | cut -d'|' -f 5)"

    github_api pull asset $1 $ASSET_ID

    local PREV_WD="$PWD"

    mkdir -p "$COFFEE_DIR"
    cd "$COFFEE_DIR"

    tar -xvf "$ASSET_FN" --strip-components=1
    rm -f "$ASSET_FN"

    cd "$PREV_WD"
}

function container_run()
{
    case "${TRAVIS_OS_NAME}" in
    "linux")
        local CONTAINER_DATA=`grep "^$2:" "$CI_DIR/$MAKEFILE" -A 5 | grep 'DOCKER_CONTAINER\|DOCKER_CONFIG'`

        if [ -z `echo $CONTAINER_DATA | grep DOCKER_CONTFIG` ]; then
            make -s -f $CI_DIR/Makefile.multi custom -e CUSTOM_COMMAND="$1" -e DOCKER_CONFIG=`echo $CONTAINER_DATA | cut -d '"' -f 2`
        else
            make -s -f $CI_DIR/Makefile.multi custom -e CUSTOM_COMMAND="$1" -e DOCKER_CONTAINER=`echo $CONTAINER_DATA | cut -d '"' -f 2`
        fi

    ;;
    "osx")
        $@
    ;;
    esac
}

function build_standalone()
{
    #####################################################
    # First get the dependencies, which we will download
    #####################################################
    OLD_IFS=$IFS
    IFS='%'
    for dep in $DEPENDENCIES; do
        [ -z $NODEPS ] && IFS=$OLD_IFS download_libraries "$dep" "$1"
    done

    #####################################################
    # Set some default targets
    #####################################################
    [ -z $CONFIGURATION ] && export CONFIGURATION=Debug
    [ -z $CMAKE_TARGET ] && export CMAKE_TARGET=install
    [ ! -z $TRAVIS ] && sudo chmod -R 777 "$SOURCE_DIR" "$COFFEE_DIR" "$BUILD_DIR"
    [ -z $GENERATE_PROGRAMS ] && export GENERATE_PROGRAMS=ON

    if [ ! -z $TRAVIS ]; then
        echo "#####################################################"
        echo "#### Program versions ###############################"
        echo "#####################################################"

        container_run "cmake --version" "$1"
        #container_run "clang --version"
        #container_run "clang++ --version"
        #container_run "gcc --version"
        #container_run "g++ --version"
        #container_run "ld --version"
        tar --version

        echo "#####################################################"
        echo "#####################################################"
        echo "#####################################################"

        echo
        echo
    fi

    #####################################################
    # This invokes the build
    # Docker configuration and similar is auto-generated
    #####################################################
    make -f "$CI_DIR/$MAKEFILE" \
        -e SOURCE_DIR="$SOURCE_DIR" \
        -e BUILD_TYPE="$CONFIGURATION" \
        -e COFFEE_DIR="$COFFEE_DIR" \
        -e CMAKE_TARGET="$CMAKE_TARGET" \
        -e GENERATE_PROGRAMS="$GENERATE_PROGRAMS" \
        $@

    # We want to exit if the Make process fails horribly
    # Should also signify to Travis/CI that something went wrong
    # We will also submit status to Firebase
    EXIT_STAT=$?
    [[ ! "$EXIT_STAT" = 0 ]] && [[ ! -z $FIREBASE_ENDPOINT ]] && $CI_DIR/../firebase-submit-report.sh "$EXIT_STAT"
    [[ ! "$EXIT_STAT" = 0 ]] && die "Make process failed"

    echo
    echo
}

function main()
{
    [[ -z $TRAVIS_BUILD_DIR ]] && TRAVIS_BUILD_DIR=$SOURCE_DIR

    local BUILDVARIANT="$1"
    local LIB_ARCHIVE="$TRAVIS_BUILD_DIR/libraries_$1.tar.gz"
    local BIN_ARCHIVE="$TRAVIS_BUILD_DIR/binaries_$1.tar.gz"

    [ -z $BUILDVARIANT ] && die "No BUILDVARIANT specified"

    case "${TRAVIS_OS_NAME}" in
    "linux")
        requires make docker tar python3
    ;;
    "osx")
        requires make tar python3
        MAKEFILE="Makefile.mac"
    ;;
    *)
        exit 1
    ;;
    esac

    build_standalone "$1"

    [ ! -z $NODEPLOY ] && exit 0
    [ ! -z $TRAVIS ] && sudo chown -R $(whoami) ${BUILD_DIR}

    #######################################
    # Package binaries and libraries separately
    #######################################
    tar -zcvf "$LIB_ARCHIVE" -C ${BUILD_DIR} \
            --exclude=build/*/bin \
            --exclude=build/*/packaged \
            build/
    tar -zcvf "$BIN_ARCHIVE" -C ${BUILD_DIR} \
            --exclude=build/*/include \
            --exclude=build/*/lib \
            --exclude=build/*/share \
            build/
}

notify "Building $BUILDVARIANT"
main "$BUILDVARIANT"
