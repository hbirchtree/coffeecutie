#!/bin/bash

case $(uname) in
Linux)
    HOST_TOOLCHAIN_TRIPLET=-linux
;;
Darwin)
    HOST_TOOLCHAIN_TRIPLET=-osx
;;
*)
    echo "::error::Host kernel could not be determined"
    return 1
;;
esac
case $(uname -m) in
x86_64)
    HOST_TOOLCHAIN_TRIPLET=x64${HOST_TOOLCHAIN_TRIPLET}
;;
aarch64)
    HOST_TOOLCHAIN_TRIPLET=arm64${HOST_TOOLCHAIN_TRIPLET}
;;
*)
    echo "::error::Host architecture could not be determined"
    return 1
;;
esac

HOST_TOOLS_BINARY_DIR=$BASE_DIR/multi_build/host_$HOST_TOOLCHAIN_TRIPLET/bin

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
    return
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
    echo $@ > cmake_params
    cmake $@
}

function host_tools_build()
{
    echo "::group::Building host tools"

    mkdir -p $BASE_DIR/multi_build/host_$HOST_TOOLCHAIN_TRIPLET
    pushd $BASE_DIR/multi_build/host_$HOST_TOOLCHAIN_TRIPLET

    export VCPKG_ROOT=$(dirname $(readlink $(which vcpkg)))

    PRELOAD_FILE=${BASE_DIR}/.github/cmake/host-tools.preload.cmake

    cmake_debug \
        -GNinja \
        -C${PRELOAD_FILE} \
        -DBUILD_WITH_WERROR=OFF \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
        -DCMAKE_INSTALL_PREFIX=${PWD}/install \
        -DCMAKE_MAKE_PROGRAM=$(which ninja) \
        -DCMAKE_TOOLCHAIN_FILE=${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake \
        -DVCPKG_TARGET_TRIPLET=$HOST_TOOLCHAIN_TRIPLET \
        -DVCPKG_MANIFEST_FEATURES=network\;pressure-cooker \
        -DVCPKG_MANIFEST_NO_DEFAULT_FEATURES=ON \
        ${BASE_DIR} ${@:2}

    cmake_debug --build .

    popd
    echo "::endgroup::"
}

function native_build()
{
    # Prepare the host tools
    BUILD_HOST_TOOLS=${BUILD_HOST_TOOLS:-1}
    if [ ${BUILD_HOST_TOOLS} = 1 ]; then
        host_tools_build
    fi

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
    export

    export PATH=$PATH:$TOOLCHAIN_ROOT/bin

    export VCPKG_ROOT=$(dirname $(readlink $(which vcpkg)))
    export VCPKG_CHAINLOAD_TOOLCHAIN_FILE=${BASE_DIR}/toolchain/cmake/Toolchains/${TOOLCHAIN_PREFIX}.toolchain.cmake
    export VCPKG_DEP_INFO_OVERRIDE_VARS="${PLATFORM};${ARCHITECTURE#-*};${SYSROOT}"

    TARGET_SPEC=""
    if [ -n "${TARGET}" ]; then
        TARGET_SPEC="--target ${TARGET}"
    fi

    install_dependencies

    TOOLCHAIN_SYSROOT="${TOOLCHAIN_ROOT}/${ARCHITECTURE}/sysroot"

    echo "::info::Installing stdc++ libs"
    mkdir -p lib/
    for f in ${TOOLCHAIN_SYSROOT}/lib/libstdc++.so.6 ${TOOLCHAIN_SYSROOT}/lib/libssp.so.0 ${TOOLCHAIN_SYSROOT}/usr/lib/libbacktrace.so.0; do
        rm "lib/$(basename $f)"
        ln -s "$f" lib/
    done

    if [ "${ENTER_ENV:-0}" = "1" ]; then
        exec $SHELL
        return
    fi

    echo "::group::Configuring project"
    echo "::info::Set up for ${TOOLCHAIN_PREFIX} (${TOOLCHAIN_ROOT})"
    PRELOAD_FILE=${BASE_DIR}/.github/cmake/${PLATFORM}-${ARCHITECTURE}-${SYSROOT}.preload.cmake
    APPIMAGE_EXTRAS="${TOOLCHAIN_SYSROOT}/lib/libstdc++.so.6;${TOOLCHAIN_SYSROOT}/lib/libssp.so.0;${TOOLCHAIN_ROOT}/usr/lib/libbacktrace.so.0"
    APPIMAGE_RUNTIME="${PWD}/vcpkg_installed/${PLATFORM}-${TOOLCHAIN_PREFIX}/bin/runtime"
    echo "::info::Using preload ${PRELOAD_FILE}"

    if [ ! -f $PRELOAD_FILE ]; then
        echo "::error::Preload file not found"
        return
    fi
    cmake_debug \
        -GNinja \
        -C${PRELOAD_FILE} \
        -DCMAKE_C_COMPILER=${TOOLCHAIN_ROOT}/bin/${ARCHITECTURE}-gcc \
        -DCMAKE_CXX_COMPILER=${TOOLCHAIN_ROOT}/bin/${ARCHITECTURE}-g++ \
        -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
        -DCMAKE_INSTALL_PREFIX=$PWD/install \
        -DCMAKE_MAKE_PROGRAM=$(which ninja) \
        -DCMAKE_TOOLCHAIN_FILE=${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake \
        -DTOOLCHAIN_ROOT="${TOOLCHAIN_ROOT}" \
        -DTOOLCHAIN_PREFIX=${TOOLCHAIN_PREFIX} \
        -DVCPKG_CHAINLOAD_TOOLCHAIN_FILE=${VCPKG_CHAINLOAD_TOOLCHAIN_FILE} \
        -DVCPKG_DEP_INFO_OVERRIDE_VARS=${VCPKG_DEP_INFO_OVERRIDE_VARS} \
        -DVCPKG_TARGET_TRIPLET=${PLATFORM}-${TOOLCHAIN_PREFIX} \
        -DAPPIMAGE_EXTRA_LIBRARIES="${APPIMAGE_EXTRAS}" \
        -DAPPIMAGE_RUNTIME_BINARY="${APPIMAGE_RUNTIME}" \
        -DHOST_TOOLS_BINARY_DIR=$HOST_TOOLS_BINARY_DIR \
        ${BASE_DIR} ${@:2}
    echo "::endgroup::"

    echo "::group::Building project"
    cmake --build . ${TARGET_SPEC}
    echo "::endgroup::"

    popd
}

function emscripten_build()
{
    # Prepare the host tools
    host_tools_build

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
    else
        echo "::info::Using compiler on disk"
                echo "::endgroup::"
    fi

    echo "::group::Toolchain versions"
    echo "::info::emsdk version: $(emsdk/emsdk list | grep INSTALLED | head -1 | awk '{print $1}')"
    echo "::info::clang version: $(emsdk/upstream/bin/clang --version | head -1 | awk '{print $3}')"
    echo "::info::node version: $(emsdk/emsdk list | grep node- | grep INSTALLED | head -1 | awk '{print $2}' | cut -d'-' -f2)"
    echo "::endgroup::"

    echo "::group::Setting up emsdk environment"
    source $TOOLCHAIN_ROOT/emsdk_env.sh
    echo "::endgroup::"
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

    if [ "${ENTER_ENV:-0}" = "1" ]; then
        exec $SHELL
        return
    fi

    echo "::group::Configuring project"
    echo "::info::Set up for ${TOOLCHAIN_PREFIX} (${TOOLCHAIN_ROOT})"
    PRELOAD_FILE=${BASE_DIR}/.github/cmake/${PLATFORM}-${ARCHITECTURE}-${SYSROOT}.preload.cmake
    echo "::info::Using preload ${PRELOAD_FILE}"

    if [ ! -f $PRELOAD_FILE ]; then
        echo "::error::Preload file not found"
        return
    fi
    cmake_debug \
        -GNinja \
        -C${BASE_DIR}/.github/cmake/${PLATFORM}-${ARCHITECTURE}-${SYSROOT}.preload.cmake \
        -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
        -DCMAKE_INSTALL_PREFIX=$PWD/install \
        -DCMAKE_MAKE_PROGRAM=$(which ninja) \
        -DCMAKE_TOOLCHAIN_FILE=${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake \
        -DVCPKG_CHAINLOAD_TOOLCHAIN_FILE=${VCPKG_CHAINLOAD_TOOLCHAIN_FILE} \
        -DVCPKG_TARGET_TRIPLET=${TOOLCHAIN_PREFIX} \
        -DHOST_TOOLS_BINARY_DIR=$HOST_TOOLS_BINARY_DIR \
        ${BASE_DIR} ${@:2}
    echo "::endgroup::"

    echo "::group::Building project"
    cmake --build . ${TARGET_SPEC}
    echo "::endgroup::"

    popd
}

function xcode_build()
{
    # Prepare the host tools
    host_tools_build

    identify_target $1

    echo " * Selected platform ${PLATFORM}:${ARCHITECTURE}"

    mkdir -p $BASE_DIR/multi_build/${PLATFORM}-${ARCHITECTURE}-${SYSROOT:-}
    pushd $BASE_DIR/multi_build/${PLATFORM}-${ARCHITECTURE}-${SYSROOT:-}

    echo "::info::Set up for ${PLATFORM}:${ARCHITECTURE}:${SYSROOT}"
    export TOOLCHAIN_PREFIX=$ARCHITECTURE
    PRELOAD_FILE=${BASE_DIR}/.github/cmake/${ARCHITECTURE}-${SYSROOT}.preload.cmake
    echo "::info::Using preload ${PRELOAD_FILE}"

    if [ ! -f $PRELOAD_FILE ]; then
        echo "::error::Preload file not found"
        return
    fi

    export VCPKG_ROOT=$(dirname $(readlink $(which vcpkg)))

    TARGET_SPEC=""
    if [ -n "${TARGET}" ]; then
        TARGET_SPEC="--target ${TARGET}"
    fi

    install_dependencies

    if [ "${ENTER_ENV:-0}" = "1" ]; then
        exec $SHELL
        return
    fi

    echo "::group::Configuring project"
    if [[ "${ARCHITECTURE}" = *ios ]]; then
        echo "::info::Doing iOS build"
        TRANSLATED_PLATFORM=""
        if [ $SYSROOT = "ios" ]; then
            TRANSLATED_PLATFORM=OS64COMBINED
        elif [ $SYSROOT = "tvos" ]; then
            TRANSLATED_PLATFORM=TVOSCOMBINED
        elif [ $SYSROOT = "watchos" ]; then
            TRANSLATED_PLATFORM=WATCHOSCOMBINED
        elif [ $SYSROOT = "catalyst" ]; then
            TRANSLATED_PLATFORM=MAC_CATALYST
        else
            echo "::error::Invalid platform specified: Needs to be one of:"
            echo "::error::ios, tvos, watchos"
            echo "::endgroup::"
            return
        fi
        cmake_debug \
            -G${GENERATOR:-Xcode} \
            -C${PRELOAD_FILE} \
            -DCMAKE_INSTALL_PREFIX=$PWD/install \
            -DCMAKE_TOOLCHAIN_FILE=${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake \
            -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
            -DVCPKG_CHAINLOAD_TOOLCHAIN_FILE=${BASE_DIR}/toolchain/cmake/Toolchains/all-ios.toolchain.cmake \
            -DVCPKG_TARGET_TRIPLET=${TOOLCHAIN_PREFIX} \
            -DPLATFORM=${TRANSLATED_PLATFORM} \
            -DHOST_TOOLS_BINARY_DIR=$HOST_TOOLS_BINARY_DIR \
            ${BASE_DIR} ${@:2}
    else
        echo "::info::Doing macOS build"
        TRANSLATED_PLATFORM=""
        if [ $SYSROOT = "universal" ]; then
            TRANSLATED_PLATFORM=MAC_UNIVERSAL
        elif [ $SYSROOT = "intel" ]; then
            TRANSLATED_PLATFORM=MAC
        elif [ $SYSROOT = "apple" ]; then
            TRANSLATED_PLATFORM=MAC
        elif [ $SYSROOT = "catalyst" ]; then
            TRANSLATED_PLATFORM=MAC_CATALYST
        else
            echo "::error::Invalid platform specified: Needs to be one of:"
            echo "::error::universal, intel, apple, catalyst"
            echo "::endgroup::"
            return
        fi
        cmake_debug \
            -G${GENERATOR:-Xcode} \
            -C${PRELOAD_FILE} \
            -DCMAKE_INSTALL_PREFIX=$PWD/install \
            -DCMAKE_TOOLCHAIN_FILE=${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake \
            -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
            -DVCPKG_CHAINLOAD_TOOLCHAIN_FILE=${BASE_DIR}/toolchain/cmake/Toolchains/all-ios.toolchain.cmake \
            -DVCPKG_TARGET_TRIPLET=${TOOLCHAIN_PREFIX} \
            -DHOST_TOOLS_BINARY_DIR=$HOST_TOOLS_BINARY_DIR \
            -DPLATFORM=${TRANSLATED_PLATFORM} \
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
    # Prepare the host tools
    host_tools_build

    identify_target $1

    BUILD_NAME="${PLATFORM}-${ARCHITECTURE}_${SYSROOT}"

    mkdir -p $BASE_DIR/multi_build/$BUILD_NAME
    pushd $BASE_DIR/multi_build/$BUILD_NAME

    export TOOLCHAIN_ROOT="${TOOLCHAIN_ROOT:-$ANDROID_NDK}"
    export TOOLCHAIN_PREFIX="${ARCHITECTURE}-android"
    export ANDROID_NDK_HOME=$ANDROID_NDK
    ANDROID_API_LEVEL=${SYSROOT%-*}
    ANDROID_API_LEVEL=android-${ANDROID_API_LEVEL}

    echo " * Selected platform ${PLATFORM}:${ARCHITECTURE}:${SYSROOT}"
    echo " * Selected vcpkg triplet ${TOOLCHAIN_PREFIX}"
    echo " * Selected Android NDK ${ANDROID_NDK} ($(cat ${ANDROID_NDK}/source.properties | grep Pkg.Revision | cut -d= -f2))"
    echo " * Selected Android SDK ${ANDROID_SDK}"

    export CONFIGURATION=${CONFIGURATION:-Debug}
    export CMAKE_SOURCE_DIR=${BASE_DIR}
    export CMAKE_INSTALL_DIR=${INSTALL_DIR:-$PWD/install}

    export VCPKG_ROOT=$(dirname $(readlink $(which vcpkg)))
    export VCPKG_CHAINLOAD_TOOLCHAIN_FILE=${ANDROID_NDK}/build/cmake/android.toolchain.cmake
    export VCPKG_TARGET_TRIPLET=${TOOLCHAIN_PREFIX}

    TARGET_SPEC=""
    if [ -n "${TARGET}" ]; then
        TARGET_SPEC="--target ${TARGET}"
    fi

    if [ "${ENTER_ENV:-0}" = "1" ]; then
        exec $SHELL
        return
    fi

    echo "::group::Configuring project"
    echo "::info::Set up for ${TOOLCHAIN_PREFIX} (${TOOLCHAIN_ROOT})"
    PRELOAD_FILE=${BASE_DIR}/.github/cmake/${PLATFORM}-${ARCHITECTURE}-${SYSROOT}.preload.cmake
    echo "::info::Using preload ${PRELOAD_FILE}"

    if [ ! -f $PRELOAD_FILE ]; then
        echo "::error::Preload file not found"
        return
    fi
    cmake_debug \
        -GNinja \
        -C${PRELOAD_FILE} \
        -DANDROID_SDK=${ANDROID_SDK} \
        -DANDROID_NDK=${ANDROID_NDK} \
        -DCMAKE_MAKE_PROGRAM=$(which ninja) \
        -DCMAKE_INSTALL_PREFIX=$PWD/install \
        -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
        -DVCPKG_TARGET_TRIPLET=${TOOLCHAIN_PREFIX} \
        -DANDROID_PLATFORM=${ANDROID_API_LEVEL} \
        -DVCPKG_CMAKE_SYSTEM_VERSION=${SYSROOT%-*} \
        -DVCPKG_CRT_LINKAGE=static \
        -DVCPKG_LIBRARY_LINKAGE=static \
        -DVCPKG_CHAINLOAD_TOOLCHAIN_FILE=${BASE_DIR}/toolchain/vcpkg/toolchains/android.cmake \
        -DCMAKE_TOOLCHAIN_FILE=${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake \
        -DHOST_TOOLS_BINARY_DIR=$HOST_TOOLS_BINARY_DIR \
        ${BASE_DIR} ${@:2}
    echo "::endgroup::"
        #-DVCPKG_CHAINLOAD_TOOLCHAIN_FILE=${VCPKG_CHAINLOAD_TOOLCHAIN_FILE} \

    echo "::group::Building project"
    cmake --build . ${TARGET_SPEC}
    echo "::endgroup::"

    popd
}

function mingw_build()
{
    # Prepare the host tools
    host_tools_build

    identify_target $1
    TOOLCHAIN_DOWNLOAD="${PLATFORM}-${ARCHITECTURE}_${SYSROOT}"

    mkdir -p $BASE_DIR/multi_build/$TOOLCHAIN_DOWNLOAD
    pushd $BASE_DIR/multi_build/$TOOLCHAIN_DOWNLOAD

    case $ARCHITECTURE in
    "x64-mingw")
        export ARCHITECTURE=x64-mingw-static
        export TOOLCHAIN_PREFIX=x86_64-w64-mingw32
    ;;
    "x64-windows")
        echo "Not yet :("
        return 1
    ;;
    *)
        return 1
    ;;
    esac

    echo " * Selected platform ${PLATFORM}:${ARCHITECTURE}:${SYSROOT}"

    export GENERATE_PROGRAMS=${GENERATE_PROGRAMS:-ON}
    export CONFIGURATION=${CONFIGURATION:-Debug}
    export CMAKE_SOURCE_DIR=${BASE_DIR}
    export CMAKE_INSTALL_DIR=${INSTALL_DIR:-$PWD/install}
    export

    export VCPKG_ROOT=$(dirname $(readlink $(which vcpkg)))
    export VCPKG_DEP_INFO_OVERRIDE_VARS="${PLATFORM};${ARCHITECTURE#-*}:$(echo $ARCHITECTURE | cut -d'-' -f2);${SYSROOT}"

    TARGET_SPEC=""
    if [ -n "${TARGET}" ]; then
        TARGET_SPEC="--target ${TARGET}"
    fi

    if [ "${ENTER_ENV:-0}" = "1" ]; then
        exec $SHELL
        return
    fi

    echo "::group::Configuring project"
    echo "::info::Set up for ${TOOLCHAIN_PREFIX} (system)"
    PRELOAD_FILE=${BASE_DIR}/.github/cmake/${PLATFORM}-${ARCHITECTURE}-${SYSROOT}.preload.cmake
    echo "::info::Using preload ${PRELOAD_FILE}"

    if [ ! -f $PRELOAD_FILE ]; then
        echo "::error::Preload file not found"
        return
    fi
    cmake_debug \
        -GNinja \
        -C${PRELOAD_FILE} \
        -DCMAKE_C_COMPILER=${TOOLCHAIN_PREFIX}-gcc \
        -DCMAKE_CXX_COMPILER=${TOOLCHAIN_PREFIX}-g++ \
        -DCMAKE_INSTALL_PREFIX=$PWD/install \
        -DCMAKE_MAKE_PROGRAM=$(which ninja) \
        -DCMAKE_TOOLCHAIN_FILE=${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake \
        -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
        -DVCPKG_DEP_INFO_OVERRIDE_VARS=${VCPKG_DEP_INFO_OVERRIDE_VARS} \
        -DVCPKG_TARGET_TRIPLET=x64-mingw-static \
        -DVCPKG_MANIFEST_NO_DEFAULT_FEATURES=ON \
        -DVCPKG_MANIFEST_FEATURES='blam;headful;openssl' \
        -DHOST_TOOLS_BINARY_DIR=$HOST_TOOLS_BINARY_DIR \
        ${BASE_DIR} ${@:2}
    echo "::endgroup::"

    echo "::group::Building project"
    cmake --build . ${TARGET_SPEC}
    echo "::endgroup::"

    popd

}
