#include <coffee/core/platform_data.h>
#include <coffee/core/plat/plat_environment.h>
#include <coffee/core/CProfiling>

#include <coffee/core/plat/plat_primary_identify.h>
#include <coffee/core/coffee_mem_macros.h>

#if defined(COFFEE_ANDROID)
#include <coffee/android/android_main.h>
#endif

#if defined(COFFEE_APPLE)
extern "C" void OSX_GetDisplayDPI(float* dpis, size_t* num_dpis);
#endif

#if defined(COFFEE_APPLE_MOBILE)
#include <coffee/core/base/renderer/eventapplication_wrapper.h>
#endif

namespace Coffee{

#if defined(COFFEE_LINUX) || defined(COFFEE_ANDROID)
namespace Environment{
namespace Linux{
extern CString get_kern_name();
extern CString get_kern_arch();
extern PlatformData::DeviceType get_device_variant();
}
}
#endif

PlatformData::DeviceType PlatformData::DeviceVariant()
{
#if defined(COFFEE_ANDROID) \
    || defined(COFFEE_APPLE_MOBILE) \
    || defined(COFFEE_MAEMO)

    /* TODO: Add difference between tablet and phone */

    return DevicePhone;
#elif defined(COFFEE_LINUX)
    return Environment::Linux::get_device_variant();
#elif defined(COFFEE_RASPBERRY)
    return DeviceIOT;
#elif defined(COFFEE_EMSCRIPTEN)
    return DeviceIOT;
#else
    return DeviceUnknown;
#endif
}

PlatformData::Platform PlatformData::PlatformVariant()
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
#endif
}

scalar PlatformData::DeviceDPI()
{
    /* We add this to allow better indication on traces */
    DProfContext _("Fetching device DPI");

    /* TODO: Add DPI fetch for Android and iOS */
    /* Also, add DPI fetching for OS X and Linux */
    /* DPI in Windows is a lie */
#if defined(COFFEE_ANDROID)

    AndroidForeignCommand fcmd;
    fcmd.type = Android_QueryDeviceDPI;

    CoffeeForeignSignalHandleNA(CoffeeForeign_RequestPlatformData,
                                &fcmd, nullptr, nullptr);

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
        maxDpi = CMath::max(dpi, maxDpi);
    
    return maxDpi;
#else
    return 1.f;
#endif
}

bool PlatformData::DeviceSafeArea(SafeArea& area)
{
#if defined(COFFEE_APPLE_MOBILE)
    CoffeeForeignSignalHandleNA(CoffeeForeign_GetSafeMargins,
            &area, nullptr, nullptr);

    return true;
#else
    return false;
#endif
}

CString PlatformData::SystemDisplayString()
{
//    const constexpr cstring _fmt = "%s %s %u-bit (%s ";
    const constexpr cstring _fmt = "%s %s (%s ";
    CString sys_ver = SysInfo::GetSystemVersion();
    CString sys_name = C_SYSTEM_STRING;
    CString curr_arch = COFFEE_ARCH;
#if defined(COFFEE_LINUX) || defined(COFFEE_ANDROID)
    sys_name = Environment::Linux::get_kern_name();
    curr_arch = Environment::Linux::get_kern_arch();
#endif
    int len = snprintf(nullptr,0,_fmt,
                       sys_name.c_str(),
                       sys_ver.c_str(),
//                       C_SYSTEM_BITNESS,
                       curr_arch.c_str());
    CString base;
    base.resize(len);
    snprintf(&base[0],base.size(),_fmt,
            sys_name.c_str(),
            sys_ver.c_str(),
//            C_SYSTEM_BITNESS,
            curr_arch.c_str());
    base.resize(base.find('\0'));
    /* What the fuck. Where does the rest of the string go? */
    base.append(")");

#if defined(COFFEE_WINDOWS) && !defined(COFFEE_WINDOWS_UWP)
	typedef BOOL(WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);
	static LPFN_ISWOW64PROCESS fnIsWow64Process;

	do {
		if (!fnIsWow64Process)
		{
			fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(
				GetModuleHandle(TEXT("kernel32")), "IsWow64Process");
		}

		if (!fnIsWow64Process)
			break;

		BOOL is_wow64 = FALSE;

		if (fnIsWow64Process(GetCurrentProcess(), &is_wow64)
			&& is_wow64 == TRUE)
			base.append(" (WoW64)");
	} while (false);
#endif

    return base;
}

bool PlatformData::IsMobile()
{
#if defined(COFFEE_ANDROID) || defined(COFFEE_IOS)
    return true;
#else
    return false;
#endif
}

bool PlatformData::UseVirtualFS()
{
#if defined(COFFEE_ANDROID) || defined(COFFEE_WINDOWS)
    return true;
#else
    return false;
#endif
}

bool PlatformData::IsDebug()
{
#ifndef NDEBUG
    return true;
#else
    return false;
#endif
}

}
