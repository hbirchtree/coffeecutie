#!/bin/bash

set -e

case $(uname) in
Linux)
    HOST_TOOLCHAIN_TRIPLET=-linux-native
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

export PATH=$PATH:${BASE_DIR}/multi_build/host-${HOST_TOOLCHAIN_TRIPLET}/vcpkg_installed/x64-linux/tools/glslang:${BASE_DIR}/multi_build/host-${HOST_TOOLCHAIN_TRIPLET}/vcpkg_installed/x64-linux/tools/spirv-tools/
export GLSLANG_PROGRAM=$(which glslangValidator)
export NINJA=$(which ninja)

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
    [[ "${ARCHITECTURE}" = *"linux"* ]] && IS_LINUX=1 || IS_LINUX=0
    [[ "${ARCHITECTURE}" = *"windows"* ]] && IS_WINDOWS=1 || IS_WINDOWS=0
    [[ "${ARCHITECTURE}" = *"osx"* ]] && IS_MACOS=1 || IS_MACOS=0
    if [[ "${ARCHITECTURE}" = *"linux"* ]] || [[ "${ARCHITECTURE}" = *"powerpc"* ]] || [[ "${ARCHITECTURE}" = *"mingw32"* ]]; then
        IS_DOWNLOADABLE=1
    else
        IS_DOWNLOADABLE=0
    fi
    if [[ ${BUILD_TYPE:-} = "rel" ]]; then
        BUILD_TYPE=Release
    else
        BUILD_TYPE=Debug
    fi
}

function cmake_debug()
{
    echo "About to execute:"
    echo cmake $@
    cmake $@
}

function host_tools_build()
{
    echo "::group::Building host tools"

    pushd ${BASE_DIR}

    export NINJA=$(which ninja)
    export VCPKG_ROOT=$(dirname $(readlink -f $(which vcpkg)))
    cmake_debug --preset host-${HOST_TOOLCHAIN_TRIPLET}
    cmake_debug --build --preset host-${HOST_TOOLCHAIN_TRIPLET}-rel

    popd

    if [ $(uname) = "Darwin" ]; then
        TOOLS_DIR=${BASE_DIR}/multi_build/x64-osx/vcpkg_installed/x64-osx/tools
    elif [ $(uname) = "Linux" ]; then
        TOOLS_DIR=${BASE_DIR}/multi_build/x64-linux-native/vcpkg_installed/x64-linux/tools
    fi
    PROTOC_PROGRAM=${TOOLS_DIR}/protobuf/protoc
    GLSLANG_PROGRAM=${TOOLS_DIR}/glslang/glslangValidator

    echo "::endgroup::"

    if [[ ${GITHUB_ACTIONS:-false} = "true" ]]; then
        return
    fi
    echo "::group::Downloading code tools"

    mkdir -p ${BASE_DIR}/multi_build/compilers/bin
    pushd ${BASE_DIR}/multi_build/compilers/bin

    TOOL_SUFFIX=
    CLANG_TOOLS_VER=17
    case ${HOST_TOOLCHAIN_TRIPLET} in
    "x64-linux-native")
        TOOL_SUFFIX=linux-amd64
        ;;
    "x64-osx")
        TOOL_SUFFIX=macosx-amd64
        ;;
    esac
    for TOOL in format tidy; do
        [[ -f clang-${TOOL} ]] && continue
        wget \
            "https://github.com/muttleyxd/clang-tools-static-binaries/releases/download/master-f7f02c1d/clang-${TOOL}-${CLANG_TOOLS_VER}_${TOOL_SUFFIX}" \
            -O clang-${TOOL}
        chmod +x clang-${TOOL}
    done

    popd

    echo "::endgroup::"
}

function toolchain_registry()
{
    mkdir -p $BASE_DIR/multi_build/compilers/meta
    VERSION=$($SELF build-info toolchain version)
    if [[ ! -f "$BASE_DIR/multi_build/compilers/meta/$VERSION.json" ]]; then
        gh release download \
            -R "$($SELF build-info toolchain source)" \
            -D "$BASE_DIR/multi_build/compilers/meta" \
            "$VERSION" \
            -p registry.json
        mv $BASE_DIR/multi_build/compilers/meta/registry.json \
            "$BASE_DIR/multi_build/compilers/meta/$VERSION.json"
    fi
    cat "$BASE_DIR/multi_build/compilers/meta/$VERSION.json"
}

function toolchain_download()
{
    FILE="$(toolchain_registry | jq -r ".[] | select(.name==\""${1%_*}"\") | $2")"
    if [[ "$FILE" = "" ]]; then
        echo "Found no toolchain for $1, aborting"
        return
    fi
    gh release download -R "$($SELF build-info toolchain source)" "$($SELF build-info toolchain version)" -p "$FILE"
    case "$2" in
    ".compiler")
        mv "$FILE" "compiler.tar.xz"
        ;;
    ".manifest")
        mv "$FILE" "compiler.manifest"
        ;;
    esac
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

    DEFAULT_ROOT="${BASE_DIR}/multi_build/compilers/${PLATFORM}-${ARCHITECTURE}/${TOOLCHAIN_VER}"
    export TOOLCHAIN_PREFIX="${ARCHITECTURE}"

    if [[ "${PLATFORM}-${ARCHITECTURE}" = "console-powerpc-eabi"* ]]; then
        TOOLCHAIN_DOWNLOAD="gamecube-powerpc-eabi"
        DEFAULT_ROOT="${BASE_DIR}/multi_build/compilers/${TOOLCHAIN_DOWNLOAD}/${TOOLCHAIN_VER}"
        export TOOLCHAIN_PREFIX="powerpc-eabi"
    fi

    export TOOLCHAIN_ROOT="${TOOLCHAIN_ROOT:-$DEFAULT_ROOT}"

    echo " * Selected platform ${PLATFORM}:${ARCHITECTURE}:${SYSROOT}"

    echo "${DEFAULT_ROOT} ${TOOLCHAIN_ROOT} ${IS_DOWNLOADABLE}"
    if [[ "$DEFAULT_ROOT" = "$TOOLCHAIN_ROOT" ]] && [[ ! -d "${TOOLCHAIN_ROOT}" ]] && [[ $IS_DOWNLOADABLE = "1" ]]; then
        echo "::group::Getting compiler"
        TOOLCHAIN_REPO=$($SELF build-info toolchain source)
        mkdir -p ${TOOLCHAIN_ROOT}
        pushd ${TOOLCHAIN_ROOT}

        rm ${TOOLCHAIN_DOWNLOAD}.manifest ${TOOLCHAIN_DOWNLOAD}.tar.xz || true
        toolchain_download "${TOOLCHAIN_DOWNLOAD}" .compiler
        toolchain_download "${TOOLCHAIN_DOWNLOAD}" .manifest && cat compiler.manifest

        umask 022
        tar x \
            --exclude=*/sysroot/dev \
            --no-same-owner \
            --no-same-permissions \
            --file=compiler.tar.xz
        chmod -R u+w $(realpath .)
        popd

        echo "::endgroup::"
    fi

    export PATH=$PATH:$TOOLCHAIN_ROOT/bin

    export VCPKG_ROOT=$(dirname $(readlink -f $(which vcpkg)))

    TARGET_SPEC=""
    if [ -n "${TARGET}" ]; then
        TARGET_SPEC="--target ${TARGET}"
    fi

    TOOLCHAIN_SYSROOT="${TOOLCHAIN_ROOT}/${ARCHITECTURE}/sysroot"

    echo "::group::Configuring project"
    echo "::info::Set up for ${TOOLCHAIN_PREFIX} (${TOOLCHAIN_ROOT})"

    export NINJA=$(which ninja)
    export VCPKG_ROOT=$(dirname $(readlink -f $(which vcpkg)))

    pushd $BASE_DIR

    if [[ $IS_LINUX = "1" ]]; then
        cmake --preset ${PLATFORM}-${ARCHITECTURE}-${SYSROOT}

        echo "::info::Installing stdc++ libs into lib/ directory"
        mkdir -p $BASE_DIR/multi_build/${PLATFORM}-${ARCHITECTURE}-${SYSROOT}
        pushd $BASE_DIR/multi_build/${PLATFORM}-${ARCHITECTURE}-${SYSROOT}
        mkdir -p lib/
        for f in ${TOOLCHAIN_SYSROOT}/lib/libstdc++.so.6 ${TOOLCHAIN_SYSROOT}/lib/libssp.so.0 ${TOOLCHAIN_SYSROOT}/usr/lib/libbacktrace.so.0; do
            rm -f "lib/$(basename $f)"
            ln -s "$f" lib/
        done
        popd
    elif [[ $IS_WINDOWS = "1" ]]; then
        cmake --preset ${PLATFORM}-${ARCHITECTURE}-${SYSROOT}
    elif [[ $IS_MACOS = "1" ]]; then
        echo "TODO"
#        cmake_debug \
#            -GNinja \
#            -C${PRELOAD_FILE} \
#            -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
#            -DCMAKE_INSTALL_PREFIX=$PWD/install \
#            -DCMAKE_MAKE_PROGRAM=$(which ninja) \
#            -DCMAKE_TOOLCHAIN_FILE=${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake \
#            -DVCPKG_DEP_INFO_OVERRIDE_VARS=${VCPKG_DEP_INFO_OVERRIDE_VARS} \
#            -DVCPKG_MANIFEST_FEATURES=${TARGET_FEATURES} \
#            -DVCPKG_TARGET_TRIPLET=${TOOLCHAIN_PREFIX} \
#            -DHOST_TOOLS_BINARY_DIR=$HOST_TOOLS_BINARY_DIR \
#            ${BASE_DIR} ${@:2}
    elif [[ $SYSROOT = "cube" ]] || [[ $SYSROOT = "wii" ]]; then
        echo "::info::Using PowerPC presets"
        cmake --preset ${PLATFORM}-${ARCHITECTURE}-${SYSROOT}
    fi
    echo "::endgroup::"

    echo "::group::Building project"
    cmake \
        --build \
        --preset ${PLATFORM}-${ARCHITECTURE}-${SYSROOT}-dbg \
        ${TARGET_SPEC}
    echo "::endgroup::"

    popd
}

function emscripten_build()
{
    # Prepare the host tools
    host_tools_build

    identify_target $1

    echo " * Selected platform ${PLATFORM}:${ARCHITECTURE}:${SYSROOT}"

    DEFAULT_ROOT=$BASE_DIR/multi_build/compilers/emsdk
    TOOLCHAIN_ROOT=${TOOLCHAIN_ROOT:-$DEFAULT_ROOT}
    if [ "${DEFAULT_ROOT}" = "${TOOLCHAIN_ROOT}" ] && [ ! -d ${TOOLCHAIN_ROOT} ]; then
        echo "::group::Getting compiler"
        CONFIG_VERSION=$($SELF build-info toolchain emsdk version)
        SELECTED_VERSION=${CONFIG_VERSION:-latest}
        git clone https://github.com/emscripten-core/emsdk.git ${TOOLCHAIN_ROOT}
        ${TOOLCHAIN_ROOT}/emsdk install $SELECTED_VERSION
        ${TOOLCHAIN_ROOT}/emsdk activate $SELECTED_VERSION
        echo "::endgroup::"
    else
        echo "::info::Using compiler on disk"
        echo "::endgroup::"
    fi

    echo "::group::Toolchain versions"
    echo "::info::emsdk version: $(${TOOLCHAIN_ROOT}/emsdk list | grep INSTALLED | head -1 | awk '{print $1}')"
    echo "::info::clang version: $(${TOOLCHAIN_ROOT}/upstream/bin/clang --version | head -1 | awk '{print $3}')"
    echo "::info::node version: $(${TOOLCHAIN_ROOT}/emsdk list | grep node- | grep INSTALLED | head -1 | awk '{print $2}' | cut -d'-' -f2)"
    echo "::endgroup::"

    echo "::group::Setting up emsdk environment"
    source ${TOOLCHAIN_ROOT}/emsdk_env.sh
    echo "::endgroup::"
    echo "::info::Set up for ${PLATFORM}:${ARCHITECTURE}:${SYSROOT} (${TOOLCHAIN_ROOT})"
    export TOOLCHAIN_ROOT=$TOOLCHAIN_ROOT
    export TOOLCHAIN_PREFIX=${ARCHITECTURE}

    TARGET_SPEC=""
    if [ -n "${TARGET}" ]; then
        TARGET_SPEC="--target ${TARGET}"
    fi

    pushd $BASE_DIR

    echo "::group::Configuring project"
    echo "::info::Set up for ${TOOLCHAIN_PREFIX} (${TOOLCHAIN_ROOT})"
    export NINJA=$(which ninja)
    export VCPKG_ROOT=$(dirname $(readlink -f $(which vcpkg)))
    cmake --preset ${PLATFORM}-${ARCHITECTURE}-${SYSROOT}
    echo "::endgroup::"

    echo "::group::Building project"
    cmake --build --preset ${PLATFORM}-${ARCHITECTURE}-${SYSROOT}-dbg ${TARGET_SPEC}
    echo "::endgroup::"

    popd
}

function xcode_build()
{
    # Prepare the host tools
    host_tools_build

    identify_target $1

    echo " * Selected platform ${PLATFORM}:${ARCHITECTURE}:${SYSROOT}"
    echo "::info::Set up for ${PLATFORM}:${ARCHITECTURE}:${SYSROOT}"

    export VCPKG_ROOT=$(dirname $(readlink $(which vcpkg)))

    TARGET_SPEC=""
    if [ -n "${TARGET}" ]; then
        TARGET_SPEC="--target ${TARGET}"
    fi

    pushd $BASE_DIR

    export NINJA=$(which ninja)
    echo "::group::Configuring project"
    cmake --preset ${PLATFORM}-${ARCHITECTURE}-${SYSROOT}
    echo "::endgroup::"

    echo "::group::Building project"
    cmake --build --preset ${PLATFORM}-${ARCHITECTURE}-${SYSROOT}-dbg ${TARGET_SPEC}
    echo "::endgroup::"

    popd
}

function android_build()
{
    # Prepare the host tools
    host_tools_build

    identify_target $1

    export ANDROID_SDK=${ANDROID_SDK:-${BASE_DIR}/multi_build/compilers/android/latest}
    if [[ ! -d "${ANDROID_SDK}" ]]; then
        echo "::group::Fetching Android SDK"
        SDKTOOLS_URL="https://dl.google.com/android/repository/commandlinetools-linux-10406996_latest.zip"
        PTOOLS_URL="https://dl.google.com/android/repository/platform-tools-latest-linux.zip"
        mkdir -p $(dirname ${ANDROID_SDK})
        pushd $(dirname ${ANDROID_SDK})
        wget ${SDKTOOLS_URL} -O android-sdk-tools.zip
        wget ${PTOOLS_URL} -O android-platform-tools.zip
        unzip android-sdk-tools.zip
        unzip android-platform-tools.zip
        mv cmdline-tools latest
        mv platform-tools latest/
        popd
        pushd ${ANDROID_SDK}
        printf "y\ny\ny\ny\ny\ny\ny\ny\n" | bin/sdkmanager \
            --sdk_root=${ANDROID_SDK} \
            --install build-tools\;34.0.0 \
            ndk\;25.2.9519653 \
            ndk\;26.0.10792818 \
            platforms\;android-19 \
            platforms\;android-25 \
            platforms\;android-30 \
            platforms\;android-32 \
            platforms\;android-33
        popd
        echo "::endgroup::"
    else
        echo "::info::Using preinstalled Android SDK: ${ANDROID_SDK}"
    fi

    echo " * Selected platform ${PLATFORM}:${ARCHITECTURE}:${SYSROOT}"
    echo " * Selected vcpkg triplet ${ARCHITECTURE}-android"
#    echo " * Selected Android NDK ${ANDROID_NDK} ($(cat ${ANDROID_NDK}/source.properties | grep Pkg.Revision | cut -d= -f2))"
    echo " * Selected Android SDK ${ANDROID_SDK}"

    TARGET_SPEC=""
    if [ -n "${TARGET}" ]; then
        TARGET_SPEC="--target ${TARGET}"
    fi

    pushd $BASE_DIR

    echo "::group::Configuring project"
    echo "::info::Set up for ${ARCHITECTURE}-${SYSROOT} (${ANDROID_SDK})"

    export NINJA=$(which ninja)
    export ANDROID_SDK=${ANDROID_SDK}
    export VCPKG_ROOT=$(dirname $(readlink -f $(which vcpkg)))
    cmake --preset ${PLATFORM}-${ARCHITECTURE}-${SYSROOT}
    echo "::endgroup::"

    echo "::group::Building project"
    cmake --build --preset ${PLATFORM}-${ARCHITECTURE}-${SYSROOT}-dbg ${TARGET_SPEC}
    echo "::endgroup::"

    popd
}

function mingw_build()
{
    # Prepare the host tools
    host_tools_build

    identify_target $1
    TOOLCHAIN_DOWNLOAD="${PLATFORM}-${ARCHITECTURE}_${SYSROOT}"

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

    DEFAULT_ROOT="${BASE_DIR}/multi_build/compilers/mingw/${ARCHITECTURE}/${TOOLCHAIN_VER}"
    export TOOLCHAIN_PREFIX="${ARCHITECTURE}"

    export TOOLCHAIN_ROOT="${TOOLCHAIN_ROOT:-$DEFAULT_ROOT}"

    echo " * Doing MinGW build"

    if [[ "$DEFAULT_ROOT" = "$TOOLCHAIN_ROOT" ]] && [[ ! -d "${TOOLCHAIN_ROOT}" ]] && [[ $IS_DOWNLOADABLE = "1" ]]; then
        echo "::group::Getting compiler"
        TOOLCHAIN_REPO=$($SELF build-info toolchain source)
        mkdir -p ${TOOLCHAIN_ROOT}
        pushd ${TOOLCHAIN_ROOT}

        toolchain_download "windows-${ARCHITECTURE}_posix" .compiler
        toolchain_download "windows-${ARCHITECTURE}_posix" .manifest && cat compiler.manifest

        umask 022
        tar x \
            --exclude=*/sysroot/dev \
            --no-same-owner \
            --no-same-permissions \
            --file=compiler.tar.xz
        chmod -R u+w $(realpath .)
        popd

        echo "::endgroup::"
    fi

    mkdir -p $BASE_DIR/multi_build/${PLATFORM}-${ARCHITECTURE}-${SYSROOT}

    echo " * Selected platform ${PLATFORM}:${ARCHITECTURE}:${SYSROOT}"

    export VCPKG_ROOT=$(dirname $(readlink -f $(which vcpkg)))

    echo "::group::Configuring project"
    echo "::info::Set up for ${TOOLCHAIN_PREFIX} (system)"

    pushd ${BASE_DIR}

    cmake --preset ${PLATFORM}-${ARCHITECTURE}-${SYSROOT}
#    cmake_debug \
#        -GNinja \
#        -C${PRELOAD_FILE} \
#        -DCMAKE_C_COMPILER=${TOOLCHAIN_PREFIX}-gcc \
#        -DCMAKE_CXX_COMPILER=${TOOLCHAIN_PREFIX}-g++ \
#        -DCMAKE_INSTALL_PREFIX=$PWD/install \
#        -DCMAKE_MAKE_PROGRAM=$(which ninja) \
#        -DCMAKE_TOOLCHAIN_FILE=${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake \
#        -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
#        -DVCPKG_DEP_INFO_OVERRIDE_VARS=${VCPKG_DEP_INFO_OVERRIDE_VARS} \
#        -DVCPKG_TARGET_TRIPLET=x64-mingw-static \
#        -DVCPKG_MANIFEST_NO_DEFAULT_FEATURES=ON \
#        -DVCPKG_MANIFEST_FEATURES='blam;headful;openssl' \
#        -DHOST_TOOLS_BINARY_DIR=$HOST_TOOLS_BINARY_DIR \
#        ${BASE_DIR} ${@:2}
    echo "::endgroup::"

    echo "::group::Building project"
    cmake --build --preset ${PLATFORM}-${ARCHITECTURE}-${SYSROOT}-dbg
    echo "::endgroup::"
    popd
}
