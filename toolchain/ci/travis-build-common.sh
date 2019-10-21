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

function build_standalone()
{
    mkdir -p "$SOURCE_DIR" "$COFFEE_DIR" "$BUILD_DIR"
    [ ! -z ${TRAVIS} ] && sudo chmod -R 777 "$SOURCE_DIR" "$COFFEE_DIR" "$BUILD_DIR"

    if [ ! -z ${TRAVIS} ]; then
        echo "#####################################################"
        echo "#### Program versions ###############################"
        echo "#####################################################"

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
    
    EXIT_STAT=$?

    # We want to exit if the Make process fails horribly
    # Should also signify to Travis/CI that something went wrong
    # We will also submit status to Firebase
    BUILD_DATA="$(OUTPUT_TYPE=json $CI_DIR/../ci-identify.sh)"
    function get_j()
    {
        echo "$BUILD_DATA" | jq $1 | cut -d'"' -f 2
    }
    [[ ! "$EXIT_STAT" = 0 ]] && ${CI_DIR}/../notify/firebase/firebase-submit-report.sh "$EXIT_STAT"

    if [[ "$EXIT_STAT" = 0 ]]; then
        export MQTT_TITLE="Success on $(get_j .build_id)"
    else
        export MQTT_TITLE="Failure on $(get_j .build_id)"
    fi
    export MQTT_BODY="Build: $(get_j .build_id), branch: $(get_j .branch), repo: $(get_j .repo), service: $(get_j .service), exited with $EXIT_STAT"
    export MQTT_ICON="https://cdn.travis-ci.org/favicon-b4e438ec85b9ae88d26b49538bc4e5ce.png"
    export MQTT_URL="$(get_j .build_url)"
    export MQTT_TOPIC="builds/$(get_j .repo)/$(get_j .build_id)"
    ${CI_DIR}/../notify/mqtt/mqtt-notify.sh

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
    local _BASEDIR=${TAR_BASE:-$1/build/$BUILDVARIANT}
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

