#include <coffee/core/platform_data.h>

#include <peripherals/base.h>
#include <platforms/environment.h>
#include <platforms/sysinfo.h>

#if defined(COFFEE_ANDROID)
#include <coffee/android/android_main.h>
#endif

#if defined(COFFEE_APPLE)
extern "C" void OSX_GetDisplayDPI(float* dpis, size_t* num_dpis);
#endif

#if defined(COFFEE_APPLE_MOBILE)
#include <coffee/foreign/foreign.h>
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
    size_t numDpis = 0;
    OSX_GetDisplayDPI(nullptr, &numDpis);
    if(numDpis == 0)
        return 1.f;

    Vector<float> dpis;
    dpis.resize(numDpis);
    OSX_GetDisplayDPI(dpis.data(), nullptr);

    float maxDpi = 0.f;
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

namespace is {
const bool mobile =
#if defined(COFFEE_ANDROID) || defined(COFFEE_IOS)
    true;
#else
    false;
#endif
} // namespace is

DeviceType variant()
{
#if defined(COFFEE_ANDROID) || defined(COFFEE_APPLE_MOBILE) || \
    defined(COFFEE_MAEMO)

    /* TODO: Add difference between tablet and phone */

    return DevicePhone;
#elif defined(COFFEE_LINUX)
    return env::Linux::get_device_variant();
#elif defined(COFFEE_RASPBERRY)
    return DeviceIOT;
#elif defined(COFFEE_EMSCRIPTEN)
    return DeviceIOT;
#elif defined(COFFEE_GEKKO)
    return DeviceConsole;
#else
    return DeviceUnknown;
#endif
}

CString system::runtime_kernel()
{
#if defined(COFFEE_LINUX) || defined(COFFEE_ANDROID)
    return env::Linux::get_kern_name();
#elif defined(COFFEE_APPLE)
    return env::mac::get_kern_name();
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
#if defined(COFFEE_LINUX) || defined(COFFEE_ANDROID)
    return env::Linux::get_kern_ver();
#elif defined(COFFEE_APPLE)
    return env::mac::get_kern_ver();
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
#else
    return {};
#endif
}

CString system::runtime_distro_version()
{
#if defined(COFFEE_LINUX)
    return env::Linux::get_lsb_release().release;
#else
    return {};
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
