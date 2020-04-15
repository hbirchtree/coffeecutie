#!/bin/bash

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
        local mute=$(which "${prog}")
        if [[ ! "$?" = "0" ]]; then
            die "Could not find program: $prog"
        fi
    done
}

#######################################
# Build configuration
#######################################
TRAVIS_OS_NAME=${TRAVIS_OS_NAME:-linux}
TRAVIS_BUILD_DIR=${TRAVIS_BUILD_DIR:-${PWD}}
TRAVIS_REPO_SLUG=${TRAVIS_REPO_SLUG:-}
TRAVIS_COMMIT=${TRAVIS_COMMIT:-0000}


#######################################
# Optional build options
#######################################
GENERATE_PROGRAMS=${GENERATE_PROGRAMS:-ON}
CONFIGURATION=${CONFIGURATION:-Debug}
CMAKE_TARGET=${CMAKE_TARGET:-}


#######################################
# For build reports
#######################################
FIREBASE_ENDPOINT=${FIREBASE_ENDPOINT:-}


#######################################
# Dependencies
#######################################
# For downloading dependencies
GITHUB_TOKEN=${GITHUB_TOKEN:-}


#######################################
# No deploy configuration
#######################################
NODEPLOY=${NODEPLOY:-}


#######################################
# Defined only for Travis CI builds, do not define locally!
#######################################
TRAVIS=${TRAVIS:-}


#######################################
# For potential issues with create directories
#######################################
#umask 000

#######################################
# General variables
#######################################
SOURCE_DIR="$PWD"
BUILD_DIR="$SOURCE_DIR/multi_build"


COFFEE_DIR="$BUILD_DIR/coffee_lib"

#######################################
# Export for scripts
#######################################
export BUILDVARIANT
export MANUAL_DEPLOY
export MANUAL_CONTEXT

QTHUB_DOCKER="hbirch/coffeecutie:qthub-client"
MAKEFILE="Makefile.linux"

INFOPY="$SOURCE_DIR/toolchain/buildinfo.py"

function buildinfo_() {
    "$INFOPY" $@
}

#######################################
# Retrieve script location relative to source dir
#######################################
SCRIPT_DIR="${SOURCE_DIR}/$(buildinfo_ --source-dir "${SOURCE_DIR}" script_location)"
MAKEFILE_DIR="${SOURCE_DIR}/$(buildinfo_ --source-dir "${SOURCE_DIR}" makefile_location)"

CI_DIR="$MAKEFILE_DIR"

HELPER="$SCRIPT_DIR/get_matching_release.py"
GITHUBPY="$SCRIPT_DIR/github_api.py"

function matching_release_() {
    "$HELPER" $@
}

function github_py_() {
    "$GITHUBPY" $@
}

