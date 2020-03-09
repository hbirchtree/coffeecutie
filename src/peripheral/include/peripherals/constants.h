#pragma once

#include <peripherals/identify/architecture.h>
#include <peripherals/identify/system.h>

namespace build_props {

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

} // namespace profiler
} // namespace build_props
