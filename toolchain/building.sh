#!/bin/bash

function die()
{
    echo " * $@"
    exit 1
}

[[ $_ != $0 ]] && true || die "Script is meant to be sourced"

function identify_host()
{
    HOST_ARCHITECTURE="$(uname -m)"
    HOST_PLATFORM=$(uname | tr '[:upper:]' '[:lower:]')
    HOST_TRIPLET="desktop-${HOST_ARCHITECTURE}-${HOST_PLATFORM}"
}

function identify_target()
{
    PLATFORM="$(echo $1 | cut -d: -f1)"
    ARCHITECTURE="$(echo $1 | cut -d: -f2)"
    SYSROOT="$(echo $1 | cut -d: -f3)"
    TARGET="$(echo $1 | cut -d: -f4)"
}

function install_dependencies()
{
    echo "::group::Installing dependencies"
    vcpkg install \
        --x-manifest-root=${BASE_DIR} \
        --feature-flags=versions \
        --overlay-ports=${BASE_DIR}/toolchain/vcpkg/ports \
        --overlay-triplets=${BASE_DIR}/toolchain/vcpkg/triplets \
        --triplet=${TOOLCHAIN_PREFIX}
    echo "::endgroup::"
}

function cmake_debug()
{
    echo "About to execute:"
    echo cmake $@
    cmake $@
}

function native_build()
{
    identify_target $1
    TOOLCHAIN_DOWNLOAD="${PLATFORM}-${ARCHITECTURE}_${SYSROOT}"

    mkdir -p $BASE_DIR/multi_build/$TOOLCHAIN_DOWNLOAD
    pushd $BASE_DIR/multi_build/$TOOLCHAIN_DOWNLOAD

    TOOLCHAIN_VER=$($SELF build-info toolchain version)
    if [[ -z "$TOOLCHAIN_VER" ]]; then
        echo \
    "No compiler version found in .build.yml, add one with:
    toolchain:
    source: <repo>
    version: <version>

    Example:
    toolchain:
    source: hbirchtree/coffeecutie-automation-tools
    version: v1.0.18"
        exit 1
    fi

    DEFAULT_ROOT="$PWD/compiler-$TOOLCHAIN_VER"
    export TOOLCHAIN_ROOT="${TOOLCHAIN_ROOT:-$DEFAULT_ROOT}"
    export TOOLCHAIN_PREFIX="${ARCHITECTURE}"

    echo " * Selected platform ${PLATFORM}:${ARCHITECTURE}:${SYSROOT}"

    if [[ "$DEFAULT_ROOT" = "$TOOLCHAIN_ROOT" ]] && [[ "$(cat compiler-ver)" != "$TOOLCHAIN_VER" ]]; then
        echo "::group::Getting compiler"
        TOOLCHAIN_REPO=$($SELF build-info toolchain source)

        rm $TOOLCHAIN_DOWNLOAD.tar.xz ${PLATFORM}-${ARCHITECTURE}.manifest || true
        gh release download -R "$TOOLCHAIN_REPO" "$TOOLCHAIN_VER" -p "${TOOLCHAIN_DOWNLOAD}.*"
        gh release download -R "$TOOLCHAIN_REPO" "$TOOLCHAIN_VER" -p "${PLATFORM}-${ARCHITECTURE}.manifest"
        cat ${PLATFORM}-${ARCHITECTURE}.manifest

        mkdir -p compiler-$TOOLCHAIN_VER
        pushd compiler-$TOOLCHAIN_VER
        umask 022
        tar xf ../$TOOLCHAIN_DOWNLOAD.tar.xz --no-same-owner --no-same-permissions
        chmod -R u+w $(realpath .)
        popd

        echo $TOOLCHAIN_VER > compiler-ver

        echo "::endgroup::"
    fi

    export GENERATE_PROGRAMS=${GENERATE_PROGRAMS:-ON}
    export CONFIGURATION=${CONFIGURATION:-Debug}
    export CMAKE_SOURCE_DIR=${BASE_DIR}
    export CMAKE_INSTALL_DIR=${INSTALL_DIR:-$PWD/install}

    export PATH=$PATH:$TOOLCHAIN_ROOT/bin

    export VCPKG_ROOT=$(dirname $(readlink $(which vcpkg)))
    export VCPKG_CHAINLOAD_TOOLCHAIN_FILE=${BASE_DIR}/toolchain/cmake/Toolchains/${TOOLCHAIN_PREFIX}.toolchain.cmake

    TARGET_SPEC=""
    if [ -n "${TARGET}" ]; then
        TARGET_SPEC="--target ${TARGET}"
    fi

    install_dependencies

    echo "::group::Configuring project"
    echo "::info::Set up for ${TOOLCHAIN_PREFIX} (${TOOLCHAIN_ROOT})"
    cmake \
        -GNinja \
        -C${BASE_DIR}/.github/cmake/${PLATFORM}-${ARCHITECTURE}.preload.cmake \
        -DCMAKE_C_COMPILER=${TOOLCHAIN_ROOT}/bin/${ARCHITECTURE}-gcc \
        -DCMAKE_CXX_COMPILER=${TOOLCHAIN_ROOT}/bin/${ARCHITECTURE}-g++ \
        -DCMAKE_INSTALL_PREFIX=$PWD/install \
        -DCMAKE_MAKE_PROGRAM=$(which ninja) \
        -DCMAKE_TOOLCHAIN_FILE=${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake \
        -DTOOLCHAIN_ROOT="${TOOLCHAIN_ROOT}" \
        -DTOOLCHAIN_PREFIX=${TOOLCHAIN_PREFIX} \
        -DVCPKG_CHAINLOAD_TOOLCHAIN_FILE=${VCPKG_CHAINLOAD_TOOLCHAIN_FILE} \
        -DVCPKG_OVERLAY_PORTS=${BASE_DIR}/toolchain/vcpkg/ports \
        -DVCPKG_OVERLAY_TRIPLETS=${BASE_DIR}/toolchain/vcpkg/triplets \
        -DVCPKG_TARGET_TRIPLET=${TOOLCHAIN_PREFIX} \
        ${BASE_DIR} ${@:2}
    echo "::endgroup::"

    echo "::group::Building project"
    cmake --build . ${TARGET_SPEC}
    echo "::endgroup::"

    popd
}

function emscripten_build()
{
    identify_target $1

    echo " * Selected platform ${PLATFORM}:${ARCHITECTURE}:${SYSROOT}"

    mkdir -p $BASE_DIR/multi_build/${PLATFORM}-${ARCHITECTURE}_${SYSROOT}
    pushd $BASE_DIR/multi_build/${PLATFORM}-${ARCHITECTURE}_${SYSROOT}

    DEFAULT_ROOT=$PWD/emsdk
    TOOLCHAIN_ROOT=${TOOLCHAIN_ROOT:-$DEFAULT_ROOT}
    if [ "${DEFAULT_ROOT}" = "${TOOLCHAIN_ROOT}" ] && [ ! -d emsdk ]; then
        echo "::group::Getting compiler"
        CONFIG_VERSION=$($SELF build-info toolchain emsdk version)
        SELECTED_VERSION=${CONFIG_VERSION:-latest}
        git clone https://github.com/emscripten-core/emsdk.git
        emsdk/emsdk install $SELECTED_VERSION
        emsdk/emsdk activate $SELECTED_VERSION
        echo "::endgroup::"
    fi

    source $TOOLCHAIN_ROOT/emsdk_env.sh
    echo "::info::Set up for ${PLATFORM}:${ARCHITECTURE}:${SYSROOT} (${TOOLCHAIN_ROOT})"
    export TOOLCHAIN_ROOT=$TOOLCHAIN_ROOT
    export TOOLCHAIN_PREFIX=${ARCHITECTURE}

    export VCPKG_ROOT=$(dirname $(readlink $(which vcpkg)))
    export VCPKG_CHAINLOAD_TOOLCHAIN_FILE=${EMSDK}/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake

    TARGET_SPEC=""
    if [ -n "${TARGET}" ]; then
        TARGET_SPEC="--target ${TARGET}"
    fi

    install_dependencies

    echo "::group::Configuring project"
    cmake \
        -GNinja \
        -C${BASE_DIR}/.github/cmake/${ARCHITECTURE}-${SYSROOT}.preload.cmake \
        -DCMAKE_INSTALL_PREFIX=$PWD/install \
        -DCMAKE_MAKE_PROGRAM=$(which ninja) \
        -DCMAKE_TOOLCHAIN_FILE=${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake \
        -DVCPKG_CHAINLOAD_TOOLCHAIN_FILE=${VCPKG_CHAINLOAD_TOOLCHAIN_FILE} \
        -DVCPKG_OVERLAY_PORTS=${BASE_DIR}/toolchain/vcpkg/ports \
        -DVCPKG_OVERLAY_TRIPLETS=${BASE_DIR}/toolchain/vcpkg/triplets \
        -DVCPKG_TARGET_TRIPLET=${TOOLCHAIN_PREFIX} \
        ${BASE_DIR} ${@:2}
    echo "::endgroup::"

    echo "::group::Building project"
    cmake --build . ${TARGET_SPEC}
    echo "::endgroup::"

    popd
}

function xcode_build()
{
    identify_target $1

    echo " * Selected platform ${PLATFORM}:${ARCHITECTURE}"

    mkdir -p $BASE_DIR/multi_build/${PLATFORM}-${ARCHITECTURE}-${SYSROOT:-}
    pushd $BASE_DIR/multi_build/${PLATFORM}-${ARCHITECTURE}-${SYSROOT:-}

    echo "::info::Set up for ${PLATFORM}:${ARCHITECTURE}:${SYSROOT}"
    export TOOLCHAIN_PREFIX=$ARCHITECTURE

    export VCPKG_ROOT=$(dirname $(readlink $(which vcpkg)))

    TARGET_SPEC=""
    if [ -n "${TARGET}" ]; then
        TARGET_SPEC="--target ${TARGET}"
    fi

    install_dependencies

    echo "::group::Configuring project"
    if [[ "${ARCHITECTURE}" = *ios ]]; then
        echo "::info::Doing iOS build"
        cmake \
            -G${GENERATOR:-Xcode} \
            -C${BASE_DIR}/.github/cmake/${ARCHITECTURE}.preload.cmake \
            -DCMAKE_INSTALL_PREFIX=$PWD/install \
            -DCMAKE_TOOLCHAIN_FILE=${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake \
            -DVCPKG_CHAINLOAD_TOOLCHAIN_FILE=${BASE_DIR}/toolchain/cmake/Toolchains/all-ios.toolchain.cmake \
            -DVCPKG_OVERLAY_PORTS=${BASE_DIR}/toolchain/vcpkg/ports \
            -DVCPKG_OVERLAY_TRIPLETS=${BASE_DIR}/toolchain/vcpkg/triplets \
            -DVCPKG_TARGET_TRIPLET=${TOOLCHAIN_PREFIX} \
            -DIOS_PLATFORM=${PLATFORM} \
            ${BASE_DIR} ${@:2}
    else
        echo "::info::Doing macOS build"
        cmake \
            -G${GENERATOR:-Ninja} \
            -C${BASE_DIR}/.github/cmake/${ARCHITECTURE}.preload.cmake \
            -DCMAKE_INSTALL_PREFIX=$PWD/install \
            -DCMAKE_TOOLCHAIN_FILE=${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake \
            -DVCPKG_OVERLAY_PORTS=${BASE_DIR}/toolchain/vcpkg/ports \
            -DVCPKG_OVERLAY_TRIPLETS=${BASE_DIR}/toolchain/vcpkg/triplets \
            -DVCPKG_TARGET_TRIPLET=${TOOLCHAIN_PREFIX} \
            ${BASE_DIR} ${@:2}
    fi
    echo "::endgroup::"

    echo "::group::Building project"
    cmake --build . ${TARGET_SPEC}
    echo "::endgroup::"

    popd
}

function android_build()
{
    identify_target $1

    BUILD_NAME="${PLATFORM}-${ARCHITECTURE}_${SYSROOT}"

    mkdir -p $BASE_DIR/multi_build/$BUILD_NAME
    pushd $BASE_DIR/multi_build/$BUILD_NAME

    export TOOLCHAIN_ROOT="${TOOLCHAIN_ROOT:-$ANDROID_NDK}"
    export TOOLCHAIN_PREFIX="${ARCHITECTURE}-android"
    export ANDROID_NDK_HOME=$ANDROID_NDK
    ANDROID_API_LEVEL=android-${SYSROOT:-28}

    echo " * Selected platform ${PLATFORM}:${ARCHITECTURE}:${SYSROOT}"
    echo " * Selected Android NDK ${ANDROID_NDK} ($(cat ${ANDROID_NDK}/source.properties | grep Pkg.Revision | cut -d= -f2))"
    echo " * Selected Android SDK ${ANDROID_SDK}"

    export CONFIGURATION=${CONFIGURATION:-Debug}
    export CMAKE_SOURCE_DIR=${BASE_DIR}
    export CMAKE_INSTALL_DIR=${INSTALL_DIR:-$PWD/install}

    export VCPKG_ROOT=$(dirname $(readlink $(which vcpkg)))
    export VCPKG_CHAINLOAD_TOOLCHAIN_FILE=${ANDROID_NDK}/build/cmake/android.toolchain.cmake

    TARGET_SPEC=""
    if [ -n "${TARGET}" ]; then
        TARGET_SPEC="--target ${TARGET}"
    fi

    #install_dependencies

    echo "::group::Configuring project"
    echo "::info::Set up for ${TOOLCHAIN_PREFIX} (${TOOLCHAIN_ROOT})"
    cmake_debug \
        -GNinja \
        -C${BASE_DIR}/.github/cmake/${PLATFORM}-${ARCHITECTURE}.preload.cmake \
        -DANDROID_SDK=${ANDROID_SDK} \
        -DANDROID_NDK=${ANDROID_NDK} \
        -DCMAKE_INSTALL_PREFIX=$PWD/install \
        -DVCPKG_OVERLAY_PORTS=${BASE_DIR}/toolchain/vcpkg/ports \
        -DVCPKG_OVERLAY_TRIPLETS=${BASE_DIR}/toolchain/vcpkg/triplets \
        -DVCPKG_TARGET_TRIPLET=${TOOLCHAIN_PREFIX} \
        -DANDROID_PLATFORM=${ANDROID_API_LEVEL} \
        -DVCPKG_CHAINLOAD_TOOLCHAIN_FILE=${VCPKG_ROOT}/scripts/toolchains/android.cmake \
        -DCMAKE_TOOLCHAIN_FILE=${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake \
        ${BASE_DIR} ${@:2}
    echo "::endgroup::"
        #-DVCPKG_CHAINLOAD_TOOLCHAIN_FILE=${VCPKG_CHAINLOAD_TOOLCHAIN_FILE} \
        #-DANDROID_ABI=${ARCHITECTURE} \

    echo "::group::Building project"
    cmake --build . ${TARGET_SPEC}
    echo "::endgroup::"

    popd
}