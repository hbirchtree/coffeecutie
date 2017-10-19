#include <coffee/core/platform_data.h>
#include <coffee/core/plat/plat_environment.h>

#include <coffee/core/plat/plat_primary_identify.h>
#include <coffee/core/coffee_mem_macros.h>

namespace Coffee{

#if defined(COFFEE_LINUX)
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
#if defined(COFFEE_ANDROID) || defined(COFFEE_APPLE_MOBILE)
    /* TODO: Add difference between tablet and phone */
    return DevicePhone;
#elif defined(COFFEE_LINUX)
    return DeviceUnknown;
#elif defined(COFFEE_RASPBERRY)
    return DeviceIOT;
#elif defined(COFFEE_MAEMO)
    return DevicePhone;
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
#elif defined(COFFEE_LINUX)
    return PlatformLinux;
#elif defined(COFFEE_WINDOWS)
    return PlatformWindows;
#elif defined(COFFEE_APPLE_MOBILE)
    return PlatformMacIOS;
#elif defined(COFFEE_APPLE)
    return PlatformMac;
#endif
}

scalar PlatformData::DeviceDPI()
{
    /* TODO: Add DPI fetch for Android and iOS */
    /* Also, add DPI fetching for OS X and Linux */
    /* DPI in Windows is a lie */
    return 1.f;
}

CString PlatformData::SystemDisplayString()
{
    const constexpr cstring _fmt = "%s %s %u-bit (%s ";
    CString sys_ver = SysInfo::GetSystemVersion();
    CString sys_name = C_SYSTEM_STRING;
    CString curr_arch = COFFEE_ARCH;
#if defined(COFFEE_LINUX)
    sys_name = Environment::Linux::get_kern_name();
    curr_arch = Environment::Linux::get_kern_arch();
#endif
    int len = snprintf(nullptr,0,_fmt,
                       sys_name.c_str(),
                       sys_ver.c_str(),
                       C_SYSTEM_BITNESS,
                       curr_arch.c_str());
    CString base;
    base.resize(len);
    snprintf(&base[0],base.size(),_fmt,
            sys_name.c_str(),
            sys_ver.c_str(),
            C_SYSTEM_BITNESS,
            curr_arch.c_str());
    base.resize(base.find('\0'));
    /* What the fuck. Where does the rest of the string go? */
    base.append(")");
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
