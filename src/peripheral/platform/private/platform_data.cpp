#include <coffee/core/platform_data.h>

#include <peripherals/base.h>
#include <platforms/environment.h>
#include <platforms/sysinfo.h>

#if defined(COFFEE_ANDROID)
#include <coffee/android/android_main.h>
#endif

#if defined(COFFEE_APPLE_MOBILE)
#include <coffee/foreign/foreign.h>
#endif

#if defined(COFFEE_WINDOWS)
#include <lm.h>
#include <peripherals/stl/string_casting.h>
#endif

#if defined(__GLIBC__)
#include <gnu/libc-version.h>
#endif

using namespace ::stl_types;

namespace platform {

using namespace Coffee;

#if defined(COFFEE_LINUX) || defined(COFFEE_ANDROID)
namespace env {
namespace Linux {
extern CString          get_kern_name();
extern CString          get_kern_arch();
extern CString          get_kern_ver();
extern lsb_data         get_lsb_release();
extern info::DeviceType get_device_variant();
} // namespace Linux
} // namespace env
#endif

#if defined(COFFEE_WINDOWS)
namespace env {
namespace win32 {

extern stl_types::Optional<CString> GetWineVersion();
stl_types::Optional<CString>        GetRegistryString(
           HKEY                key,
           libc_types::cstring subKey,
           libc_types::cstring valueKey,
           CString::size_type  size);

}
} // namespace env
#endif

namespace info {

CString system_name()
{
#ifndef COFFEE_LOWFAT
    //    const constexpr cstring _fmt = "%s %s %u-bit (%s ";
    CString sys_ver   = SysInfo::GetSystemVersion();
    CString sys_name  = C_SYSTEM_STRING;
    CString curr_arch = COFFEE_ARCH;
#if defined(COFFEE_ANDROID)
    sys_name = "Android"; // Override this for clarity
#elif defined(COFFEE_LINUX)
    sys_name = env::Linux::get_lsb_release().distribution;

    if(sys_name.empty())
        sys_name = env::Linux::get_kern_name();
#endif
#if defined(COFFEE_LINUX) || defined(COFFEE_ANDROID)
    curr_arch = env::Linux::get_kern_arch();
#endif

    CString base = sys_name + " " + sys_ver + " (" + curr_arch + ")";

#if defined(COFFEE_WINDOWS) && !defined(COFFEE_WINDOWS_UWP)
    typedef BOOL(WINAPI * LPFN_ISWOW64PROCESS)(HANDLE, PBOOL);
    static LPFN_ISWOW64PROCESS fnIsWow64Process;

    do
    {
        if(!fnIsWow64Process)
        {
            fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(
                GetModuleHandle(TEXT("kernel32")), "IsWow64Process");
        }

        if(!fnIsWow64Process)
            break;

        BOOL is_wow64 = FALSE;

        if(fnIsWow64Process(GetCurrentProcess(), &is_wow64) && is_wow64 == TRUE)
            base.append(" (WoW64)");
    } while(false);
#endif

    return base;
#else
    return {};
#endif
}

namespace device {

namespace display {
extern scalar dpi()
{
/* We add this to allow better indication on traces */
/* TODO: Re-enable this */
//    DProfContext _("Fetching device DPI");

/* TODO: Add DPI fetch for Android and iOS */
/* Also, add DPI fetching for OS X and Linux */
/* DPI in Windows is a lie */
#if defined(COFFEE_ANDROID)

    AndroidForeignCommand fcmd;
    fcmd.type = Android_QueryDeviceDPI;

    CoffeeForeignSignalHandleNA(
        CoffeeForeign_RequestPlatformData, &fcmd, nullptr, nullptr);

    /* A very careful ballpark set of constants
     *  based on how it looks on a 320 DPI screen
     *  and a 420 DPI screen */
    return (scalar(fcmd.data.scalarI64) / 160.f);
#elif defined(COFFEE_APPLE)
    stl_types::Vector<f32> dpis;
    ::platform::mac::get_display_dpi(dpis);

    if(dpis.empty())
        return 1.f;

    f32 maxDpi = 0.f;
    for(auto const& dpi : dpis)
        maxDpi = math::max(dpi, maxDpi);

    return maxDpi;
#else
    return 1.f;
#endif
}
extern SafeArea safe_area()
{
    SafeArea area = {};
#if defined(COFFEE_APPLE_MOBILE)
    CoffeeForeignSignalHandleNA(
        CoffeeForeign_GetSafeMargins, &area, nullptr, nullptr);
#endif
    return area;
}
} // namespace display

DeviceType variant()
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

    if constexpr(compile_info::platform::is_macos)
    {
        auto system_type = SysInfo::DeviceName().model;

        if(system_type.find("MacBook") != CString::npos)
            return DeviceLaptop;

        if(system_type.find("iMac") != CString::npos)
            return DeviceAllInOne;

        if(system_type.find("MacPro") != CString::npos ||
           system_type.find("Macmini") != CString::npos)
            return DeviceDesktop;

        return DeviceUnknown;
    }

    if constexpr(compile_info::platform::is_windows)
        return DeviceDesktop;

#if defined(COFFEE_LINUX)
    return env::Linux::get_device_variant();
#endif

    return DeviceUnknown;
}

CString system::runtime_kernel()
{
#if defined(COFFEE_LINUX) || defined(COFFEE_ANDROID)
    return env::Linux::get_kern_name();
#elif defined(COFFEE_APPLE)
    return env::mac::get_kern_name();
#elif defined(COFFEE_WINDOWS)
    if(auto wineVer = env::win32::GetWineVersion(); wineVer)
        return "WINE";

    return "Windows NT";
#else
    return C_SYSTEM_STRING;
#endif
}

CString system::runtime_arch()
{
#if defined(COFFEE_LINUX) || defined(COFFEE_ANDROID)
    return env::Linux::get_kern_arch();
#else
    return COFFEE_ARCH;
#endif
}

CString system::runtime_kernel_version()
{
    using namespace stl_types;

#if defined(COFFEE_LINUX) || defined(COFFEE_ANDROID)
    return env::Linux::get_kern_ver();
#elif defined(COFFEE_APPLE)
    return env::mac::get_kern_ver();
#elif defined(COFFEE_WINDOWS)
    if(auto wineVer = env::win32::GetWineVersion(); wineVer)
        return *wineVer;

    if(auto version = env::win32::GetRegistryString(
           HKEY_LOCAL_MACHINE,
           "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion",
           "BuildLab",
           128);
       version)
        return *version;

    LPSERVER_INFO_101 ver = nullptr;
    if(auto err = NetServerGetInfo(nullptr, 101, C_RCAST<LPBYTE*>(&ver));
       err != NERR_Success)
        return env::win32::WindowsSysInfo::GetSystemVersion();
    else
    {
        auto base_ver = cast_pod(ver->sv101_version_major) + "." +
                        cast_pod(ver->sv101_version_minor);

        NetApiBufferFree(&ver);

        return base_ver;
    }

#else
    return {};
#endif
}

CString system::runtime_distro()
{
#if defined(COFFEE_LINUX)
    return env::Linux::get_lsb_release().distribution;
#elif defined(COFFEE_IOS)
    return "iOS";
#elif defined(COFFEE_APPLE)
    return "macOS";
#elif defined(COFFEE_WINDOWS)
    if(auto name = env::win32::GetRegistryString(
           HKEY_LOCAL_MACHINE,
           "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion",
           "ProductName",
           128);
       name)
        return *name;
    else
        return C_SYSTEM_STRING;
#else
    return C_SYSTEM_STRING;
#endif
}

CString system::runtime_distro_version()
{
#if defined(COFFEE_LINUX)
    return env::Linux::get_lsb_release().release;
#elif defined(COFFEE_WINDOWS)
    if(auto version = env::win32::GetRegistryString(
           HKEY_LOCAL_MACHINE,
           "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion",
           "ReleaseId",
           128);
       version)
        return *version;
    else
        return {};
#else
    return {};
#endif
}

CString system::runtime_libc_version()
{
#if defined(__GLIBC__)
    return gnu_get_libc_version();
#else
    return "0";
#endif
}

} // namespace device

namespace platform {

Platform variant()
{
#if defined(COFFEE_ANDROID)
    return PlatformAndroid;

#elif defined(COFFEE_RASPBERRYPI)
    return PlatformLinuxRaspberry;
#elif defined(COFFEE_MAEMO)
    return PlatformLinuxMaemo;
#elif defined(COFFEE_LINUX)
    return PlatformLinux;

#elif defined(COFFEE_EMSCRIPTEN)
    return PlatformEmscripten;

#elif defined(COFFEE_MINGW64)
    return PlatformMinGW;
#elif defined(COFFEE_WINDOWS_UWP)
    return PlatformUWP;
#elif defined(COFFEE_WINDOWS)
    return PlatformWindows;

#elif defined(COFFEE_APPLE_MOBILE)
    return PlatformMacIOS;
#elif defined(COFFEE_APPLE)
    return PlatformMac;

#elif defined(COFFEE_UNIXPLAT)
    return PlatformUnix;
#elif defined(COFFEE_GEKKO)
    return PlatformPOSIX;
#endif
}

} // namespace platform

} // namespace info
} // namespace platform
