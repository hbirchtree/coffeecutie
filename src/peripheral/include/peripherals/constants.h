#pragma once

#include <peripherals/identify/architecture.h>
#include <peripherals/identify/compiler.h>
#include <peripherals/identify/system.h>
#include <peripherals/libc/types.h>
#include <peripherals/stl/types.h>

namespace compile_info {

using libc_types::cstring;
using libc_types::u32;

struct compiler_version_t
{
    u32 major, minor, rev;
};

constexpr cstring target       = C_SYSTEM_STRING;
constexpr cstring architecture = COFFEE_ARCH;

namespace android {

constexpr u32 api =
#if defined(COFFEE_ANDROID)
    __ANDROID_API__
#else
    0
#endif
    ;

} // namespace android

namespace apple {

/* TODO: Fix this up on macOS/iOS */

namespace macos {
constexpr u32 target = 1014;
}
namespace ios {
constexpr u32 target = 100;
}
} // namespace apple

namespace compiler {

constexpr cstring name        = C_COMPILER_NAME;
constexpr cstring version_str = C_STR(C_COMPILER_VER_MAJ) "." C_STR(
    C_COMPILER_VER_MIN) "." C_STR(C_COMPILER_VER_REV);
constexpr compiler_version_t version = {
    C_COMPILER_VER_MAJ, C_COMPILER_VER_MIN, C_COMPILER_VER_REV};

} // namespace compiler

namespace module {

constexpr cstring build_mode =
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

constexpr bool is_unix =
#if defined(COFFEE_UNIXPLAT) || defined(COFFEE_ANDROID)
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
#if defined(COFFEE_APPLE_MOBILE)
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

} // namespace platform

namespace printing {

constexpr bool is_syslogged = platform::is_android || platform::is_emscripten;
constexpr bool is_simple    = platform::is_gekko;

} // namespace printing

namespace profiler {

constexpr bool enabled =
#if defined(COFFEE_GEKKO) || defined(COFFEE_EMSCRIPTEN) || MODE_RELEASE
    false
#else
    true
#endif
    ;

namespace gpu {

constexpr bool enabled = true;

}

} // namespace profiler
} // namespace compile_info
