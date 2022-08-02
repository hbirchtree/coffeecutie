#pragma once

#include <peripherals/identify/system.h>

#include <thread>

#include <platforms/base/device_variant.h>

#if defined(COFFEE_LINUX) || defined(COFFEE_ANDROID) || defined(COFFEE_APPLE)
#define USE_POSIX_PLATFORM_INFO
#include "posix/sysinfo.h"
#endif

#if defined(__GLIBC__) && C_HAS_INCLUDE(<gnu / libc - version.h>)
#include "glibc/sysinfo.h"
#endif

#if defined(COFFEE_LINUX) || defined(COFFEE_ANDROID)
#include "linux/sysinfo.h"
#elif defined(COFFEE_EMSCRIPTEN)
#include "emscripten/sysinfo.h"
#elif defined(COFFEE_MACOS) || defined(COFFEE_IOS)
#include "osx/sysinfo.h"
#else
#include <thread>
#endif

#if defined(COFFEE_ANDROID)
#include "android/sysinfo.h"
#endif

namespace platform::info {

namespace proc {

#if defined(COFFEE_LINUX) || defined(COFFEE_ANDROID)

using linux_::core_count;
using linux_::cpu_count;
using linux_::node_count;
using linux_::thread_count;

#elif defined(COFFEE_EMSCRIPTEN)
using emscripten::core_count;
using emscripten::cpu_count;
using emscripten::node_count;
using emscripten::thread_count;

#else

inline libc_types::u32 core_count(libc_types::u32 = 0)
{
    return 1;
}
inline libc_types::u32 cpu_count(libc_types::u32 = 0)
{
    return 1;
}
inline libc_types::u32 node_count()
{
    return 1;
}
inline libc_types::u32 thread_count(libc_types::u32 = 0)
{
    return std::thread::hardware_concurrency();
}
#endif

#if defined(COFFEE_LINUX) || defined(COFFEE_ANDROID)
constexpr bool has_cpuinfo = true;

using linux_::frequency;
using linux_::model;
#else
constexpr bool has_cpuinfo = false;

inline u32 frequency(u32 = 0, u32 = 0)
{
    return 0;
}
inline stl_types::Optional<
    stl_types::Pair<stl_types::String, stl_types::String>>
    model(u32 = 0, u32 = 0)
{
    return {};
}
#endif

inline bool is_hyperthreaded()
{
    return core_count() != thread_count();
}

} // namespace proc

namespace memory {

// using linux_::stats;
// using linux_::virtual_stats;

}

namespace os {

#if defined(USE_POSIX_PLATFORM_INFO)
using posix::architecture;
using posix::kernel;
using posix::kernel_version;

#elif defined(COFFEE_EMSCRIPTEN)
using emscripten::architecture;
using emscripten::kernel;
using emscripten::kernel_version;

#elif defined(COFFEE_WINDOWS)
using win32::architecture;
using win32::kernel;
using win32::kernel_version;

#else
inline stl_types::String kernel()
{
    return {};
}
inline stl_types::String kernel_version()
{
    return {};
}
inline stl_types::String architecture()
{
    return {};
}
#endif

#if defined(COFFEE_ANDROID)
using android::name;
using android::version;

#elif defined(COFFEE_LINUX)
using linux_::name;
using linux_::version;

#elif defined(COFFEE_EMSCRIPTEN)
using emscripten::name;
using emscripten::version;

#elif defined(COFFEE_WINDOWS)
using win32::name;
using win32::version;

using win32::is_wow64;
using win32::wine_version;

#else

inline stl_types::Optional<stl_types::String> name()
{
    return {};
}
inline stl_types::Optional<stl_types::String> version()
{
    return {};
}
#endif

#if defined(__GLIBC__) && C_HAS_INCLUDE(<gnu / libc - version.h>)
constexpr bool has_libc_info = true;

using glibc::libc_version;

#else
constexpr bool has_libc_info = false;

inline std::string_view libc_version()
{
    return {};
}

#endif

inline Platform variant()
{
    //#if defined(COFFEE_ANDROID)
    if constexpr(compile_info::platform::is_android)
        return PlatformAndroid;
    else if constexpr(compile_info::platform::is_raspberrypi)
        return PlatformLinuxRaspberry;
    else if constexpr(compile_info::platform::is_maemo)
        return PlatformLinuxMaemo;
    else if constexpr(compile_info::platform::is_linux)
        return PlatformLinux;
    else if constexpr(compile_info::platform::is_emscripten)
        return PlatformEmscripten;
    else if constexpr(compile_info::platform::is_mingw)
        return PlatformMinGW;
    else if constexpr(compile_info::platform::is_windows_uwp)
        return PlatformUWP;
    else if constexpr(compile_info::platform::is_windows)
        return PlatformWindows;
    else if constexpr(compile_info::platform::is_ios)
        return PlatformMacIOS;
    else if constexpr(compile_info::platform::is_macos)
        return PlatformMac;
    else if constexpr(compile_info::platform::is_unix)
        return PlatformUnix;
    if constexpr(compile_info::platform::is_gekko)
        return PlatformPOSIX;
    else
        return PlatformUnknown;
}

} // namespace os

namespace device {

#if defined(COFFEE_ANDROID)
using android::device;
#elif defined(COFFEE_EMSCRIPTEN)
using emscripten::device;
#elif defined(COFFEE_LINUX)
using linux_::device;
#elif defined(COFFEE_WINDOWS)
using win32::device;
#else
inline stl_types::Optional<
    stl_types::Pair<stl_types::String, stl_types::String>>
device()
{
    return {};
}
#endif

#if defined(COFFEE_ANDROID)
using android::motherboard;
#elif defined(COFFEE_LINUX)
using linux_::motherboard;
#elif defined(COFFEE_WINDOWS)
using win32::motherboard;
#else
inline stl_types::Optional<
    stl_types::Pair<stl_types::String, stl_types::String>>
motherboard()
{
    return {};
}
#endif

#if defined(COFFEE_ANDROID)
using android::chassis;
#elif defined(COFFEE_LINUX)
using linux_::chassis;
#elif defined(COFFEE_WINDOWS)
using win32::chassis;
#else
inline stl_types::Optional<
    stl_types::Pair<stl_types::String, stl_types::String>>
chassis()
{
    return {};
}
#endif

#if defined(COFFEE_ANDROID)
inline DeviceType variant()
{
  return DevicePhone;
}

#elif defined(COFFEE_LINUX)
using linux_::variant;

#elif defined(COFFEE_APPLE)
using apple::variant;

#else
inline DeviceType variant()
{
    if constexpr(
        compile_info::platform::is_android || compile_info::platform::is_ios ||
        compile_info::platform::is_maemo)
        return DevicePhone;

    /* TODO: Add better identification for Emscripten */
    if constexpr(
        compile_info::platform::is_iot || compile_info::platform::is_emscripten)
        return DeviceIOT;

    if constexpr(compile_info::platform::is_gekko)
        return DeviceConsole;

    if constexpr(compile_info::platform::is_windows)
        return DeviceDesktop;

    return DeviceUnknown;
}
#endif


namespace display {

#if defined(COFFEE_EMSCRIPTEN)
//using emscripten::dpi;
#elif defined(COFFEE_IOS) || defined(COFFEE_MACOS)
using info::display::apple::dpi;
#else

#endif

}

} // namespace device

namespace traits {

constexpr bool virtualfs =
#if defined(COFFEE_ANDROID) || defined(COFFEE_WINDOWS)
    true;
#else
    false;
#endif

} // namespace traits

} // namespace platform::info
