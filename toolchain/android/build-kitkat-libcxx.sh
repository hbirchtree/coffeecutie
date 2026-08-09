#!/usr/bin/env bash
#
# Build a modern libc++ (static only) for Android KitKat / API 19, armeabi-v7a.
#
# Why this exists
# ---------------
# NDK r26+ dropped API < 21, so the newest NDK we can target KitKat with is
# r25 (clang 14). But the *compiler* and the *platform* are separable: clang
# only needs a sysroot to target. So we run the newest NDK's clang against the
# r25 sysroot, and build our own libc++ from upstream LLVM sources on top.
#
#   compiler + binutils + compiler-rt + libunwind : NDK 29 (clang 21)
#   sysroot (bionic headers, API 19 stubs, libandroid_support) : NDK 25
#   libc++ / libc++abi : built here from upstream LLVM, static, merged
#
# Outputs (under $PREFIX_ROOT, inside the gitignored multi_build tree):
#
#   armeabi-v7a/include/c++/v1/     modern libc++ headers
#   armeabi-v7a/lib/libc++.a        libc++ + libc++abi + libunwind, one archive
#   android-kitkat.toolchain.cmake  ready-to-use CMake toolchain file
#
# Usage:
#   toolchain/android/build-kitkat-libcxx.sh              # build everything
#   toolchain/android/build-kitkat-libcxx.sh --clean      # wipe build tree first
#   toolchain/android/build-kitkat-libcxx.sh --verify     # only re-run smoke tests
#
# Everything below is overridable from the environment.

set -euo pipefail

REPO_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"

: "${SDK_ROOT:=${REPO_ROOT}/multi_build/compilers/android/latest}"
: "${NDK_SYSROOT_VER:=25.2.9519653}"   # last NDK shipping API 19 + libandroid_support
: "${NDK_CLANG_VER:=29.0.14206865}"    # newest NDK; only its clang/binutils are used
: "${API:=19}"
: "${ABI:=armeabi-v7a}"                # ARM only, by design
: "${LLVM_TAG:=llvmorg-21.1.8}"        # must be <= the NDK clang major (21)
: "${PREFIX_ROOT:=${REPO_ROOT}/multi_build/compilers/android/kitkat-clang21}"
: "${ABI_NAMESPACE:=__kk1}"            # deliberately NOT __ndk1; see note below
: "${HERMETIC:=ON}"
: "${JOBS:=$(nproc)}"
: "${BUILD_TYPE:=Release}"

DO_CLEAN=0
DO_BUILD=1
for arg in "$@"; do
    case "$arg" in
        --clean)   DO_CLEAN=1 ;;
        --verify)  DO_BUILD=0 ;;
        -h|--help) sed -n '2,40p' "${BASH_SOURCE[0]}"; exit 0 ;;
        *) echo "unknown argument: $arg" >&2; exit 2 ;;
    esac
done

NDK_SYSROOT="${SDK_ROOT}/ndk/${NDK_SYSROOT_VER}"
NDK_CLANG="${SDK_ROOT}/ndk/${NDK_CLANG_VER}"
HOST_TAG=linux-x86_64
T_SYSROOT="${NDK_SYSROOT}/toolchains/llvm/prebuilt/${HOST_TAG}"
T_CLANG="${NDK_CLANG}/toolchains/llvm/prebuilt/${HOST_TAG}"
SYSROOT="${T_SYSROOT}/sysroot"
BIN="${T_CLANG}/bin"

TRIPLE=arm-linux-androideabi
LLVM_TRIPLE="armv7a-linux-androideabi${API}"
API_LIBDIR="${SYSROOT}/usr/lib/${TRIPLE}/${API}"

LLVM_SRC="${PREFIX_ROOT}/llvm-project"
BUILD_DIR="${PREFIX_ROOT}/build-${ABI}"
PREFIX="${PREFIX_ROOT}/${ABI}"
BUILD_TC="${PREFIX_ROOT}/build-${ABI}.toolchain.cmake"
OUT_TC="${PREFIX_ROOT}/android-kitkat.toolchain.cmake"
OVERLAY="${PREFIX_ROOT}/kitkat-libcxx.cmake"

COMPAT_SRC_DIR="${REPO_ROOT}/toolchain/android/kitkat-compat"
COMPAT_HEADER="${PREFIX}/include/kitkat_compat.h"

# libcxx pulls shared numeric helpers out of llvm-libc, so libc/ has to be in
# the sparse checkout even though we build none of it.
SPARSE_PATHS=(cmake runtimes libcxx libcxxabi libunwind libc llvm/cmake third-party)

log()  { printf '\033[1;36m==>\033[0m %s\n' "$*"; }
warn() { printf '\033[1;33m!!\033[0m %s\n' "$*" >&2; }
die()  { printf '\033[1;31mxx\033[0m %s\n' "$*" >&2; exit 1; }

# ---------------------------------------------------------------- preflight

log "Checking prerequisites"
[[ -d "${SYSROOT}" ]]     || die "NDK ${NDK_SYSROOT_VER} sysroot not found: ${SYSROOT}"
[[ -d "${API_LIBDIR}" ]]  || die "No API ${API} libs in ${NDK_SYSROOT_VER}: ${API_LIBDIR}"
[[ -x "${BIN}/clang++" ]] || die "NDK ${NDK_CLANG_VER} clang not found: ${BIN}/clang++"
[[ -f "${SYSROOT}/usr/lib/${TRIPLE}/libandroid_support.a" ]] \
    || die "libandroid_support.a missing; API<21 libc++ needs it"
command -v cmake >/dev/null || die "cmake not on PATH"
command -v ninja >/dev/null || die "ninja not on PATH"
command -v git   >/dev/null || die "git not on PATH"

CLANG_MAJOR="$("${BIN}/clang" --version | sed -n 's/.*clang version \([0-9]*\).*/\1/p' | head -1)"
log "clang ${CLANG_MAJOR} from NDK ${NDK_CLANG_VER}, sysroot from NDK ${NDK_SYSROOT_VER}, target ${LLVM_TRIPLE}"

# ------------------------------------------------------------- llvm sources

fetch_llvm() {
    local stamp="${LLVM_SRC}/.stamp-${LLVM_TAG}"
    # A stamp with no revision in it predates this script recording one.
    [[ -s "${stamp}" ]] || rm -f "${stamp}"
    if [[ ! -f "${stamp}" ]] && [[ -d "${LLVM_SRC}/.git" ]] \
        && git -C "${LLVM_SRC}" rev-parse --verify -q FETCH_HEAD > "${stamp}.tmp" 2>/dev/null; then
        mv "${stamp}.tmp" "${stamp}"
        log "Recovered revision for existing ${LLVM_TAG} checkout"
    fi
    rm -f "${stamp}.tmp"
    if [[ ! -f "${stamp}" ]]; then
        log "Fetching LLVM sources at ${LLVM_TAG} (sparse, blobless)"
        rm -rf "${LLVM_SRC}"
        mkdir -p "${LLVM_SRC}"
        git -C "${LLVM_SRC}" init -q
        git -C "${LLVM_SRC}" remote add origin https://github.com/llvm/llvm-project.git
        git -C "${LLVM_SRC}" sparse-checkout init --cone
        git -C "${LLVM_SRC}" fetch --depth 1 --filter=blob:none origin "tag" "${LLVM_TAG}"
        git -C "${LLVM_SRC}" rev-parse FETCH_HEAD > "${stamp}"
    else
        log "LLVM sources already at ${LLVM_TAG}"
    fi
    # Re-applied every run: the set of paths we need grows as the script does,
    # and a blobless clone backfills whatever the new paths require on checkout.
    git -C "${LLVM_SRC}" sparse-checkout set "${SPARSE_PATHS[@]}"
    git -C "${LLVM_SRC}" checkout -q "$(cat "${stamp}")"
}

# ------------------------------------------------- toolchain used *to build*

# We reuse the NDK's own toolchain file so ABI flags, --sysroot and the API
# level stay exactly as the NDK intends them, then swap the compiler and
# binutils for the newer ones. Binutils must move with the compiler: llvm-ar
# and friends have to understand whatever bitcode/object format clang 21 emits.
write_build_toolchain() {
    mkdir -p "${PREFIX_ROOT}"
    cat > "${BUILD_TC}" <<EOF
# Generated by toolchain/android/build-kitkat-libcxx.sh -- do not edit.
# Toolchain used to compile libc++ itself. No STL is available here yet.
set(ANDROID_ABI ${ABI})
set(ANDROID_ARM_NEON ON)
set(ANDROID_PLATFORM android-${API})
set(ANDROID_STL none)
include("${NDK_SYSROOT}/build/cmake/android.toolchain.cmake")

set(_kk_bin "${BIN}")
set(CMAKE_C_COMPILER   "\${_kk_bin}/clang")
set(CMAKE_CXX_COMPILER "\${_kk_bin}/clang++")
set(CMAKE_ASM_COMPILER "\${_kk_bin}/clang")
set(CMAKE_AR      "\${_kk_bin}/llvm-ar"      CACHE FILEPATH "" FORCE)
set(CMAKE_RANLIB  "\${_kk_bin}/llvm-ranlib"  CACHE FILEPATH "" FORCE)
set(CMAKE_STRIP   "\${_kk_bin}/llvm-strip"   CACHE FILEPATH "" FORCE)
set(CMAKE_NM      "\${_kk_bin}/llvm-nm"      CACHE FILEPATH "" FORCE)
set(CMAKE_OBJCOPY "\${_kk_bin}/llvm-objcopy" CACHE FILEPATH "" FORCE)
set(CMAKE_OBJDUMP "\${_kk_bin}/llvm-objdump" CACHE FILEPATH "" FORCE)
set(CMAKE_READELF "\${_kk_bin}/llvm-readelf" CACHE FILEPATH "" FORCE)

# Declares the *_l locale functions bionic hides before API 21. libc++ routes
# Android through its glibc locale backend and calls them unconditionally.
string(APPEND CMAKE_CXX_FLAGS " -include \\"${COMPAT_SRC_DIR}/kitkat_compat.h\\"")

# strtod_l/strtof_l are declared __INTRODUCED_IN(26) but we supply our own
# definitions in kitkat_compat.c, so the availability diagnostic is wrong here.
# Scoped to the libc++ build only; consumers get the headers via -isystem and
# keep the check active for their own code.
string(APPEND CMAKE_C_FLAGS   " -Wno-unguarded-availability-new")
string(APPEND CMAKE_CXX_FLAGS " -Wno-unguarded-availability-new -Wno-error=unguarded-availability-new")

# libc++'s configure-time link checks need the pre-21 compat shim, otherwise
# they misdetect wide-char and locale support as unavailable. It has to go
# through *_STANDARD_LIBRARIES_INIT rather than the linker flags: those land
# ahead of the object files, where an archive resolves nothing.
string(APPEND CMAKE_C_STANDARD_LIBRARIES_INIT " -landroid_support")
string(APPEND CMAKE_CXX_STANDARD_LIBRARIES_INIT " -landroid_support")
EOF
}

# ---------------------------------------------------------- compat objects

# Definitions for symbols bionic declares but KitKat does not export. They are
# folded straight into libc++.a so consumers have exactly one thing to link.
build_and_merge_compat() {
    local obj="${BUILD_DIR}/kitkat_compat.o"
    log "Building pre-21 compat objects"
    "${BIN}/clang" \
        --target="${LLVM_TRIPLE}" \
        --sysroot="${SYSROOT}" \
        -march=armv7-a -mthumb -mfpu=neon \
        -O2 -fPIC -fvisibility=hidden \
        -Wall -Wextra -Wno-unguarded-availability-new \
        -c "${COMPAT_SRC_DIR}/kitkat_compat.c" -o "${obj}"

    log "Merging compat objects into libc++.a"
    "${BIN}/llvm-ar" r "${PREFIX}/lib/libc++.a" "${obj}"
    "${BIN}/llvm-ranlib" "${PREFIX}/lib/libc++.a"

    install -D -m 644 "${COMPAT_SRC_DIR}/kitkat_compat.h" "${COMPAT_HEADER}"
}

# ------------------------------------------------------------------- build

configure_and_build() {
    (( DO_CLEAN )) && { log "Cleaning ${BUILD_DIR} and ${PREFIX}"; rm -rf "${BUILD_DIR}" "${PREFIX}"; }

    log "Configuring libc++ / libc++abi / libunwind"
    cmake -G Ninja -S "${LLVM_SRC}/runtimes" -B "${BUILD_DIR}" \
        -DCMAKE_TOOLCHAIN_FILE="${BUILD_TC}" \
        -DCMAKE_BUILD_TYPE="${BUILD_TYPE}" \
        -DCMAKE_INSTALL_PREFIX="${PREFIX}" \
        -DCMAKE_POSITION_INDEPENDENT_CODE=ON \
        -DLLVM_ENABLE_RUNTIMES="libunwind;libcxxabi;libcxx" \
        -DLLVM_ENABLE_PER_TARGET_RUNTIME_DIR=OFF \
        -DLLVM_INCLUDE_TESTS=OFF \
        \
        -DLIBUNWIND_ENABLE_SHARED=OFF \
        -DLIBUNWIND_ENABLE_STATIC=ON \
        -DLIBUNWIND_USE_COMPILER_RT=ON \
        -DLIBUNWIND_HIDE_SYMBOLS=ON \
        -DLIBUNWIND_INCLUDE_TESTS=OFF \
        -DLIBUNWIND_INCLUDE_DOCS=OFF \
        \
        -DLIBCXXABI_ENABLE_SHARED=OFF \
        -DLIBCXXABI_ENABLE_STATIC=ON \
        -DLIBCXXABI_USE_LLVM_UNWINDER=ON \
        -DLIBCXXABI_ENABLE_STATIC_UNWINDER=ON \
        -DLIBCXXABI_STATICALLY_LINK_UNWINDER_IN_STATIC_LIBRARY=ON \
        -DLIBCXXABI_USE_COMPILER_RT=ON \
        -DLIBCXXABI_HERMETIC_STATIC_LIBRARY="${HERMETIC}" \
        -DLIBCXXABI_INCLUDE_TESTS=OFF \
        \
        -DLIBCXX_ENABLE_SHARED=OFF \
        -DLIBCXX_ENABLE_STATIC=ON \
        -DLIBCXX_CXX_ABI=libcxxabi \
        -DLIBCXX_ENABLE_STATIC_ABI_LIBRARY=ON \
        -DLIBCXX_STATICALLY_LINK_ABI_IN_STATIC_LIBRARY=ON \
        -DLIBCXX_USE_COMPILER_RT=ON \
        -DLIBCXX_HERMETIC_STATIC_LIBRARY="${HERMETIC}" \
        -DLIBCXX_ABI_NAMESPACE="${ABI_NAMESPACE}" \
        -DLIBCXX_ENABLE_EXCEPTIONS=ON \
        -DLIBCXX_ENABLE_RTTI=ON \
        -DLIBCXX_ENABLE_THREADS=ON \
        -DLIBCXX_HAS_PTHREAD_API=OFF \
        -DLIBCXX_HAS_MUSL_LIBC=OFF \
        -DLIBCXX_ENABLE_FILESYSTEM=ON \
        -DLIBCXX_ENABLE_WIDE_CHARACTERS=ON \
        -DLIBCXX_ENABLE_LOCALIZATION=ON \
        -DLIBCXX_ENABLE_TIME_ZONE_DATABASE=OFF \
        -DLIBCXX_INCLUDE_TESTS=OFF \
        -DLIBCXX_INCLUDE_BENCHMARKS=OFF \
        -DLIBCXX_INCLUDE_DOCS=OFF

    log "Building with ${JOBS} jobs"
    cmake --build "${BUILD_DIR}" --parallel "${JOBS}"
    log "Installing to ${PREFIX}"
    cmake --install "${BUILD_DIR}"

    [[ -f "${PREFIX}/lib/libc++.a" ]] || die "libc++.a was not produced"
    # Static-only means nothing shared should have escaped into the prefix.
    if compgen -G "${PREFIX}/lib/*.so" > /dev/null; then
        warn "shared libraries present in ${PREFIX}/lib; removing (static-only build)"
        rm -f "${PREFIX}"/lib/*.so*
    fi
}

# --------------------------------------------- toolchain emitted *for users*

# Emitted as two files. The overlay carries everything that is specific to this
# split toolchain and assumes an NDK toolchain file has already been included,
# so both the standalone toolchain below and the project's vcpkg chainload
# (toolchain/vcpkg/toolchains/android-kitkat.cmake) can share it. Neither
# duplicates the ABI/platform setup the NDK already does.
write_overlay() {
    log "Writing ${OVERLAY}"
    cat > "${OVERLAY}" <<EOF
# Generated by toolchain/android/build-kitkat-libcxx.sh -- do not edit.
#
# Include AFTER an NDK android.toolchain.cmake configured for API ${API} /
# ${ABI} with ANDROID_STL=none. Turns that into:
#
#   clang ${CLANG_MAJOR} from NDK ${NDK_CLANG_VER}
#   sysroot from NDK ${NDK_SYSROOT_VER}
#   libc++ ${LLVM_TAG}, static, built against that sysroot
#
# The libc++ ABI namespace is ${ABI_NAMESPACE}, not the NDK's __ndk1. That is
# deliberate: if a prebuilt NDK libc++ ever gets linked in alongside this one,
# you get loud undefined-symbol errors instead of a silent ODR violation.

if(_KITKAT_LIBCXX_APPLIED)
    return()
endif()
set(_KITKAT_LIBCXX_APPLIED 1)

set(_kk_bin "${BIN}")
set(CMAKE_C_COMPILER   "\${_kk_bin}/clang")
set(CMAKE_CXX_COMPILER "\${_kk_bin}/clang++")
set(CMAKE_ASM_COMPILER "\${_kk_bin}/clang")
set(CMAKE_AR      "\${_kk_bin}/llvm-ar"      CACHE FILEPATH "" FORCE)
set(CMAKE_RANLIB  "\${_kk_bin}/llvm-ranlib"  CACHE FILEPATH "" FORCE)
set(CMAKE_STRIP   "\${_kk_bin}/llvm-strip"   CACHE FILEPATH "" FORCE)
set(CMAKE_NM      "\${_kk_bin}/llvm-nm"      CACHE FILEPATH "" FORCE)
set(CMAKE_OBJCOPY "\${_kk_bin}/llvm-objcopy" CACHE FILEPATH "" FORCE)
set(CMAKE_OBJDUMP "\${_kk_bin}/llvm-objdump" CACHE FILEPATH "" FORCE)
set(CMAKE_READELF "\${_kk_bin}/llvm-readelf" CACHE FILEPATH "" FORCE)

set(KITKAT_LIBCXX_PREFIX "${PREFIX}")

# Our headers replace the NDK's LLVM 14 ones outright.
string(APPEND CMAKE_CXX_FLAGS " -nostdinc++ -isystem \\"\${KITKAT_LIBCXX_PREFIX}/include/c++/v1\\"")
string(APPEND CMAKE_CXX_FLAGS " -D_LIBCPP_DISABLE_AVAILABILITY")

# libc++'s locale backend calls the *_l functions bionic hides before API 21,
# so their declarations have to be in scope in every C++ translation unit.
string(APPEND CMAKE_CXX_FLAGS " -include \\"\${KITKAT_LIBCXX_PREFIX}/include/kitkat_compat.h\\"")

# -nostdlib++ stops the driver adding its own -lc++. libc++.a already contains
# libc++abi and libunwind; libandroid_support backfills the pre-21 bionic gaps
# and must come after it on the link line.
foreach(_v EXE SHARED MODULE)
    string(APPEND CMAKE_\${_v}_LINKER_FLAGS " -nostdlib++")
    string(APPEND CMAKE_\${_v}_LINKER_FLAGS " -Wl,--exclude-libs,libc++.a")
    string(APPEND CMAKE_\${_v}_LINKER_FLAGS " -Wl,--exclude-libs,libandroid_support.a")
    string(APPEND CMAKE_\${_v}_LINKER_FLAGS " -Wl,--exclude-libs,libunwind.a")
endforeach()
#
# This must go through CMAKE_CXX_STANDARD_LIBRARIES_INIT, not
# CMAKE_CXX_STANDARD_LIBRARIES. The NDK toolchain populates the _INIT variable,
# and project() then creates the cache entry from it -- which silently discards
# any plain variable of the non-_INIT name that a toolchain file had set. The
# NDK recomputes _INIT from scratch every time this file is read, so appending
# here stays idempotent across reconfigures.
string(APPEND CMAKE_CXX_STANDARD_LIBRARIES_INIT
    " \\"\${KITKAT_LIBCXX_PREFIX}/lib/libc++.a\\" -landroid_support")
EOF
}

# Self-contained entry point for plain CMake builds that are not going through
# the project's vcpkg presets.
write_standalone_toolchain() {
    log "Writing ${OUT_TC}"
    cat > "${OUT_TC}" <<EOF
# Generated by toolchain/android/build-kitkat-libcxx.sh -- do not edit.
#
# Standalone toolchain for Android KitKat (API ${API}, ${ABI}).
# Inside the project's own build, the vcpkg chainload
# toolchain/vcpkg/toolchains/android-kitkat.cmake is used instead; both end up
# including the same overlay.

set(ANDROID_ABI ${ABI})
set(ANDROID_ARM_NEON ON)
set(ANDROID_PLATFORM android-${API})
set(ANDROID_CPP_FEATURES "rtti exceptions")
set(ANDROID_STL none CACHE STRING "" FORCE)
include("${NDK_SYSROOT}/build/cmake/android.toolchain.cmake")
include("${OVERLAY}")
EOF
}

# ------------------------------------------------------------------ verify

# The two things that actually break on a KitKat device: a relocation its
# ancient linker cannot process, or an undefined symbol bionic never had.
# Both are cheap to check statically, so always check them.
verify() {
    local work; work="$(mktemp -d)"
    trap 'rm -rf "${work}"' RETURN
    log "Smoke-testing the emitted toolchain"

    cat > "${work}/CMakeLists.txt" <<'EOF'
cmake_minimum_required(VERSION 3.20)
project(kitkat_smoke CXX)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_library(smoke SHARED smoke.cpp)
EOF
    # Exercise the parts most likely to need symbols API 19 does not have:
    # locale, wide chars, filesystem, threads, exceptions, chrono.
    cat > "${work}/smoke.cpp" <<'EOF'
#include <algorithm>
#include <chrono>
#include <filesystem>
#include <format>
#include <iostream>
#include <locale>
#include <mutex>
#include <ranges>
#include <sstream>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>

static std::mutex g_m;

extern "C" int smoke() {
    std::vector<int> v{5, 3, 1, 4};
    std::ranges::sort(v);
    std::wstringstream ws;
    ws.imbue(std::locale::classic());
    ws << 3.5L << L' ' << 42;
    std::string s = std::format("{}-{}", v.front(), v.size());
    auto now = std::chrono::steady_clock::now();
    std::error_code ec;
    bool e = std::filesystem::exists("/system", ec);
    int r = 0;
    std::thread t([&] {
        std::scoped_lock lk(g_m);
        try { throw std::runtime_error(s); }
        catch (const std::exception& ex) { r = static_cast<int>(std::string(ex.what()).size()); }
    });
    t.join();
    std::ostringstream os;
    os << std::this_thread::get_id();
    (void)now; (void)e;
    return r + static_cast<int>(ws.str().size() + os.str().size());
}
EOF
    if ! cmake -G Ninja -S "${work}" -B "${work}/b" \
            -DCMAKE_TOOLCHAIN_FILE="${OUT_TC}" \
            -DCMAKE_BUILD_TYPE=Release > "${work}/cfg.log" 2>&1; then
        tail -40 "${work}/cfg.log" >&2; die "smoke test failed to configure"
    fi
    if ! cmake --build "${work}/b" > "${work}/build.log" 2>&1; then
        tail -60 "${work}/build.log" >&2; die "smoke test failed to build"
    fi

    local so="${work}/b/libsmoke.so"
    local fail=0

    # KitKat's linker predates IFUNC; an IRELATIVE reloc is an instant crash.
    local bad_relocs
    bad_relocs="$("${BIN}/llvm-readelf" -r "${so}" | grep -oE 'R_ARM_IRELATIVE' | sort -u || true)"
    if [[ -n "${bad_relocs}" ]]; then
        warn "unsupported relocations for the API ${API} linker: ${bad_relocs}"
        fail=1
    else
        log "relocations OK (no IRELATIVE)"
    fi

    # Every undefined symbol must exist in the API 19 platform libraries.
    local api_syms="${work}/api.txt" undef="${work}/undef.txt"
    for l in libc.so libm.so libdl.so liblog.so libstdc++.so libandroid.so libEGL.so libGLESv2.so libOpenSLES.so libz.so; do
        [[ -f "${API_LIBDIR}/${l}" ]] || continue
        "${BIN}/llvm-nm" -D --defined-only "${API_LIBDIR}/${l}" 2>/dev/null | sed 's/@.*//' | awk '{print $NF}'
    done | sort -u > "${api_syms}"
    "${BIN}/llvm-nm" -u -D "${so}" | sed 's/@.*//' | awk '{print $NF}' | sort -u > "${undef}"

    local missing
    missing="$(comm -23 "${undef}" "${api_syms}")"
    if [[ -n "${missing}" ]]; then
        warn "symbols not present on API ${API}:"
        printf '      %s\n' ${missing} >&2
        fail=1
    else
        log "all $(wc -l < "${undef}") undefined symbols resolve against API ${API}"
    fi

    (( fail )) && die "verification failed"
    log "verification passed"
}

# -------------------------------------------------------------------- main

if (( DO_BUILD )); then
    fetch_llvm
    write_build_toolchain
    configure_and_build
    build_and_merge_compat
fi
write_overlay
write_standalone_toolchain
verify

cat <<EOF

$(log "Done")

  libc++ (static, ${LLVM_TAG})  ${PREFIX}/lib/libc++.a
  headers                       ${PREFIX}/include/c++/v1
  toolchain file                ${OUT_TC}

To use it with the existing vcpkg setup, point the chainload at it:

  VCPKG_CHAINLOAD_TOOLCHAIN_FILE=${OUT_TC}

or, for a plain CMake build:

  cmake -DCMAKE_TOOLCHAIN_FILE=${OUT_TC} ...
EOF
