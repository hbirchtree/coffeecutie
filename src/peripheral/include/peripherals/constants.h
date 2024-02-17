#pragma once

#include <string_view>

#include <peripherals/identify/architecture.h>
#include <peripherals/identify/compiler.h>
#include <peripherals/identify/quirks.h>
#include <peripherals/identify/system.h>
#include <peripherals/libc/types.h>

#if defined(COFFEE_WINDOWS)
#include <sdkddkver.h>
#endif

#if defined(COFFEE_ANDROID)
#include <android/ndk-version.h>
#endif

#if C_HAS_INCLUDE(<__config>)
#include <__config>
#endif

namespace compile_info {

using libc_types::u32;
using std::string_view;

struct compiler_version_t
{
    u32 major, minor, rev;
};

constexpr string_view target       = C_SYSTEM_STRING;
constexpr string_view architecture = COFFEE_ARCH;

namespace android {

constexpr u32 api =
#if defined(COFFEE_ANDROID)
    __ANDROID_API__
#else
    0
#endif
    ;

constexpr u32 ndk_ver =
#if defined(__NDK_MAJOR__)
    __NDK_MAJOR__ * 100 + __NDK_MINOR__
#else
    0
#endif
    ;

} // namespace android

namespace apple {

/* TODO: Fix this up on macOS/iOS */

namespace macos {
constexpr u32 target =
#if defined(__MAC_OS_X_VERSION_MAX_ALLOWED)
    __MAC_OS_X_VERSION_MAX_ALLOWED
#else
    0
#endif
    ;
constexpr u32 min_target =
#if defined(__MAC_OS_X_VERSION_MIN_REQUIRED)
    __MAC_OS_X_VERSION_MIN_REQUIRED
#else
    0
#endif
    ;
} // namespace macos

namespace ios {
constexpr u32 target =
#if defined(__IPHONE_OS_VERSION_MAX_ALLOWED)
    __IPHONE_OS_VERSION_MAX_ALLOWED
#else
    0
#endif
    ;
constexpr u32 min_target =
#if defined(__IPHONE_OS_VERSION_MIN_REQUIRED)
    __IPHONE_OS_VERSION_MIN_REQUIRED
#else
    0
#endif
    ;
} // namespace ios
} // namespace apple

namespace windows {

constexpr u32 target =
#if defined(COFFEE_WINDOWS) && !defined(COFFEE_MINGW64)
    WINVER
#else
    0
#endif
    ;
constexpr u32 wdk =
#if defined(COFFEE_WINDOWS) && !defined(COFFEE_MINGW64)
    WDK_NTDDI_VERSION
#else
    0
#endif
    ;

} // namespace windows

namespace linux_ {

constexpr string_view libc_runtime =
#if defined(_LIBCPP_VERSION)
    "libc++"
#elif defined(__GLIBC__)
    "glibc"
#else
    "<unknown>"
#endif
    ;

namespace glibc {

constexpr u32 major =
#if defined(__GLIBC__)
    __GLIBC__
#else
    0
#endif
    ;
constexpr u32 minor =
#if defined(__GLIBC_MINOR__)
    __GLIBC_MINOR__
#else
    0
#endif
    ;

} // namespace glibc

namespace libcpp {

constexpr u32 version =
#if defined(_LIBCPP_VERSION)
    _LIBCPP_VERSION
#else
    0
#endif
    ;

} // namespace libcpp

} // namespace linux_

namespace compiler {

constexpr string_view name        = C_COMPILER_NAME;
constexpr string_view version_str = C_STR(C_COMPILER_VER_MAJ) "." C_STR(
    C_COMPILER_VER_MIN) "." C_STR(C_COMPILER_VER_REV);

constexpr compiler_version_t version = {
#if defined(COFFEE_WINDOWS)
    C_COMPILER_VER_MAJ / 10000000,
    C_COMPILER_VER_MAJ / 100000 - (C_COMPILER_VER_MAJ / 10000000) * 100,
    C_COMPILER_VER_MAJ / 1000 - (C_COMPILER_VER_MAJ / 100000) * 100
#else
    C_COMPILER_VER_MAJ, C_COMPILER_VER_MIN, C_COMPILER_VER_REV
#endif
};

} // namespace compiler

namespace module {

constexpr string_view build_mode =
#if MODE_DEBUG
    "Debug"
#elif MODE_LOWFAT
    "Low-fat"
#elif MODE_RELEASE
    "Release"
#else
#error Unknown build mode!
#endif
    ;

#if defined(MODULE_VERSION)
constexpr cstring version = MODULE_VERSION;
#endif

constexpr string_view name = COFFEE_COMPONENT_NAME;

} // namespace module

/*
 * Build identifiers
 *
 */

enum class build_type_t
{
    debug,
    release,
    reldeb,
};

constexpr build_type_t build_type =
#if MODE_DEBUG
    build_type_t::debug
#elif MODE_RELEASE
    build_type_t::release
#else
    build_type_t::debug
#endif
    ;

constexpr bool debug_mode =
    build_type == build_type_t::debug || build_type == build_type_t::reldeb;
constexpr bool release_mode = build_type == build_type_t::release;

constexpr bool lowfat_mode =
#if MODE_LOWFAT
    true
#else
    false
#endif
    ;

constexpr bool internal_build =
#if defined(COFFEE_INTERNAL_BUILD)
    true
#else
    false
#endif
    ;

/*
 * Platform identifiers
 *
 */

namespace platform {

constexpr bool is_32bit =
#if defined(COFFEE_ARCH_X86) || defined(COFFEE_ARCH_ARM32)
    true
#else
    false
#endif
    ;

constexpr bool custom_exit =
#if defined(COFFEE_CUSTOM_EXIT_HANDLING)
    true
#else
    false
#endif
    ;

constexpr bool is_unix =
#if defined(COFFEE_UNIXPLAT) || defined(COFFEE_ANDROID)
    true
#else
    false
#endif
    ;

constexpr bool is_mingw =
#if defined(COFFEE_MINGW64)
    true
#else
    false
#endif
    ;

constexpr bool is_windows =
#if defined(COFFEE_WINDOWS)
    true
#else
    false
#endif
    ;

constexpr bool is_windows_uwp =
#if defined(COFFEE_WINDOWS_UWP)
    true
#else
    false
#endif
    ;

constexpr bool is_linux =
#if defined(COFFEE_LINUX)
    true
#else
    false
#endif
    ;

constexpr bool is_android =
#if defined(COFFEE_ANDROID)
    true
#else
    false
#endif
    ;

constexpr bool is_maemo =
#if defined(COFFEE_MAEMO)
    true
#else
    false
#endif
    ;

constexpr bool is_wasm =
#if defined(COFFEE_WASM)
    true;
#else
    false;
#endif

constexpr bool is_emscripten =
#if defined(COFFEE_EMSCRIPTEN)
    true
#else
    false
#endif
    ;

constexpr bool is_gekko =
#if defined(COFFEE_GEKKO)
    true
#else
    false
#endif
    ;

constexpr bool is_macos =
#if defined(COFFEE_APPLE_DESKTOP)
    true
#else
    false
#endif
    ;

constexpr bool is_ios =
#if defined(COFFEE_IOS)
    true
#else
    false
#endif
    ;

constexpr bool is_raspberrypi =
#if defined(COFFEE_RASPBERRY)
    true
#else
    false
#endif
    ;

constexpr bool is_iot = is_raspberrypi;

constexpr bool is_mobile = is_android || is_ios || is_maemo;

enum class endianness
{
    big_,
    little_,
};

constexpr endianness endian =
#if defined(__BYTE_ORDER__)
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    endianness::little_;
#else
    endianness::big_;
#endif
#else
#error Could not identify endianness
#endif

} // namespace platform

namespace printing {

constexpr bool is_syslogged = platform::is_android || platform::is_emscripten;
constexpr bool is_simple    = platform::is_gekko;

} // namespace printing

namespace profiler {

constexpr bool enabled =
// #if defined(COFFEE_GEKKO) || MODE_RELEASE
// #define PERIPHERAL_PROFILER_ENABLED 0
//     false
// #else
#define PERIPHERAL_PROFILER_ENABLED 1
    true
    // #endif
    ;

namespace gpu {

constexpr bool enabled = true;

}

} // namespace profiler
} // namespace compile_info
