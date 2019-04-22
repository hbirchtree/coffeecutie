#!/bin/bash

source $(dirname ${BASH_SOURCE})/travis-common.sh

env

#######################################
# Create build directory and go to it
#######################################
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

function github_api()
{
    case "${TRAVIS_OS_NAME}" in
    *)
        # Python client
        python3 "$GITHUBPY" --api-token "$GITHUB_TOKEN" $@
    ;;
    "linux")
        # Qt/C++ client
        docker run --rm -v "$PWD:/data" ${QTHUB_DOCKER} --api-token "$GITHUB_TOKEN" $@
    ;;
    esac
}

#
# $1 : configuration list
# $2 : dependency
# $3 : version
#
function download_libraries()
{
    local slug=$(echo $2 | cut -d':' -f1)

    notify "Downloading library ${slug}:${3}"
    local CONFIG_RELEASE="$(${INFOPY} --source-dir "$SOURCE_DIR" ${1} ${2})"
    notify "Target version: $CONFIG_RELEASE"

    local ALL_RELEASES=$(github_api list release ${slug} | cut -d'|' -f 3)

    local LATEST_RELEASE=$(${HELPER} get-closest-release "$CONFIG_RELEASE" ${ALL_RELEASES})

    notify "Using release $LATEST_RELEASE for $slug"
    local CURRENT_ASSET="$(github_api list asset ${slug}:${LATEST_RELEASE} | grep "libraries_${3}.tar.gz" | head -1)"
    #echo Asset $CURRENT_ASSET

    [[ -z ${CURRENT_ASSET} ]] && echo "Failed to find ${slug}:$3" && return 1

    notify "Found assets: $CURRENT_ASSET (from $LATEST_RELEASE)"
    local ASSET_ID="$(echo ${CURRENT_ASSET} | cut -d'|' -f 3)"
    local ASSET_FN="$PWD/$(echo ${CURRENT_ASSET} | cut -d'|' -f 5)"

    github_api pull asset $2 ${ASSET_ID}

    [ ! -f ${ASSET_FN} ] && echo "Failed to pull asset" && return 1

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
        local CONTAINER_DATA=`grep "^$2:" "$CI_DIR/$MAKEFILE" -A 30 | grep 'DOCKER_CONTAINER\|DOCKER_CONFIG'`

        if [ -z `echo ${CONTAINER_DATA} | grep DOCKER_CONFIG` ]; then
            make -s -f ${CI_DIR}/Makefile.multi custom -e CUSTOM_COMMAND="$1" -e DOCKER_CONFIG=`echo ${CONTAINER_DATA} | cut -d '"' -f 2`
        else
            make -s -f ${CI_DIR}/Makefile.multi custom -e CUSTOM_COMMAND="$1" -e DOCKER_CONTAINER=`echo ${CONTAINER_DATA} | cut -d '"' -f 2`
        fi

    ;;
    "osx")
        $@
    ;;
    esac
}

function download_dependencies()
{
    #####################################################
    # First get the dependencies, which we will download
    #####################################################
    OLD_IFS=$IFS
    IFS='%'
    for dep in $1; do
        [ "${NODEPS}" != "1" ] && IFS=${OLD_IFS} download_libraries dependencies "$dep" "$BUILDVARIANT" || return 1
    done
}

function build_standalone()
{
    mkdir -p "$SOURCE_DIR" "$COFFEE_DIR" "$BUILD_DIR"
    [ ! -z ${TRAVIS} ] && sudo chmod -R 777 "$SOURCE_DIR" "$COFFEE_DIR" "$BUILD_DIR"

    if [ ! -z ${TRAVIS} ]; then
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

    [ ! -z $CMAKE_TARGET ] && _CMAKE_TARGET_OPT=-e CMAKE_TARGET="$CMAKE_TARGET"

    #####################################################
    # This invokes the build
    # Docker configuration and similar is auto-generated
    #####################################################
    make -f "$CI_DIR/$MAKEFILE" \
        -e SOURCE_DIR="$SOURCE_DIR" \
        -e BUILD_TYPE="$CONFIGURATION" \
        -e COFFEE_DIR="$COFFEE_DIR" \
        ${_CMAKE_TARGET_OPT:-} \
        -e GENERATE_PROGRAMS="$GENERATE_PROGRAMS" \
        $@

    # We want to exit if the Make process fails horribly
    # Should also signify to Travis/CI that something went wrong
    # We will also submit status to Firebase
    EXIT_STAT=$?
    [[ ! "$EXIT_STAT" = 0 ]] && [[ ! -z ${FIREBASE_ENDPOINT} ]] && ${CI_DIR}/../firebase-submit-report.sh "$EXIT_STAT"
    [[ ! "$EXIT_STAT" = 0 ]] && die "Make process failed"

    echo
    echo
}

function build_target()
{
    umask 000

    [[ -z ${TRAVIS_BUILD_DIR} ]] && TRAVIS_BUILD_DIR=${SOURCE_DIR}

    local BUILDVARIANT="$1"

    export DEPLOYED_BUILDVARIANT=${BUILDVARIANT}

    [ -z ${BUILDVARIANT} ] && die "No BUILDVARIANT specified"

    case "${TRAVIS_OS_NAME}" in
    "linux")
        requires make docker tar python3
    ;;
    "osx")
        requires make tar python3
        if [[ "$BUILDVARIANT" = "ios"* ]] || [[ "$BUILDVARIANT" = "osx"* ]]; then
            MAKEFILE="Makefile.mac"
        fi
    ;;
    *)
        exit 1
    ;;
    esac

    build_standalone "${BUILDVARIANT}"

    [ ! -z ${TRAVIS} ] && sudo chown -R $(whoami) ${BUILD_DIR}

    return 0
}

function package_libraries()
{
    local _BASEDIR=${TAR_BASE:-build/$BUILDVARIANT}
    local _OUTFILE=${PWD}/$2

    pushd $_BASEDIR

    tar -zcf "$_OUTFILE" \
            --exclude=${_BASEDIR}/*/bin \
            --exclude=${_BASEDIR}/*/packaged \
            .

    popd
}

function package_binaries()
{
    local _BASEDIR=${TAR_BASE:-$1/build/$BUILDVARIANT}
    local _OUTFILE=${PWD}/$2

    pushd $_BASEDIR

    tar -zcf "$_OUTFILE" \
            --exclude=${_BASEDIR}/*/include \
            --exclude=${_BASEDIR}/*/lib \
            --exclude=${_BASEDIR}/*/share \
            .

    popd
}

function main()
{
    [ ! -z ${NODEPLOY} ] && exit 0
}

#notify "Building $BUILDVARIANT"
#
#build_target "${BUILDVARIANT}"
#
#if [ -z ${NODEPLOY} ]; then
#    notify "Creating artifacts"
#    package_binaries ${BUILD_DIR}
#    package_libraries ${BUILD_DIR}
#fi
