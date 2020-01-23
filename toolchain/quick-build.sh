#!/bin/bash
#
# This script should be run in the source directory. It will automatically 
#  create a multi_build directory in there.
#
# Usage:
#  ../quick-build.sh (target name)
#
# Common targets (Linux + Docker):
# - android.armv7a
# - android.armv7a.kitkat
# - android.armv8a
# - emscripten.asmjs
# - emscripten.wasm
# - fedora.amd64
# - steam.amd64
# - ubuntu.amd64
#
# Common targets (Mac OS X):
# - osx
# - ios.x86_64
# - ios (universal build with armv7 and arm64)
#
# CONFIGURATION FLAGS (environment variables)
#
# NODEPLOY=1        -- Do not create tarballs for libraries.
#
# NODEPS=1          -- Do not download dependencies.
#
# LOCALLIB=...      -- Use a local library directory for build should be a
#                   --  product of another quick-build.sh run.
#
# CONFIGURATION=... -- Set build configuration, typically Debug or Release.
#                   -- Some target can only perform Release mode.
#
# CMAKE_TARGET=...  -- Override CMake build target. Does not work for all
#                   --  builds, such as Android and Emscripten.
#
# GITHUB_TOKEN=...  -- Use OAuth2 token to access Github when downloading
#                   --  dependencies. Without this, you are limited to
#                   --  60 requests.
#
# BUILD_MODE=...    -- Specify 'containerized' to build in Docker,
#                   --  or 'bare' to build without containerization.
#                   -- Applies only to Linux.
#

set -x
set -euf -o pipefail

function build_info()
{
    "$(dirname "$0")"/buildinfo.py $@ 2>/dev/null
}

function get_travis_platform()
{
    osname=$(uname)
    case "$osname" in
    Linux)
        echo linux
    ;;
    Darwin)
        echo osx
    ;;
    esac
}

export TRAVIS_OS_NAME=$(get_travis_platform)
export BUILDVARIANT=$1
export MAKEFILE_DIR=$(build_info makefile_location)
export SCRIPT_DIR=
export NODEPS=${NODEPS:-1}
export LOCALLIB=${LOCALLIB:-}
export MULTI_DIR="$PWD/../multi_build"

mkdir -p "$MULTI_DIR"

[ -z "${MAKEFILE_DIR}" ] && echo "No Makefile directory found" && exit 1

source "$(build_info script_location)"/travis-build-common.sh

build_target "${BUILDVARIANT}"
