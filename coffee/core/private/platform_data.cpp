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
#if defined(COFFEE_LINUX)

#else
    return DeviceUnknown;
#endif
}

scalar PlatformData::DeviceDPI()
{
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
#if !defined(COFFEE_ANDROID) && !defined(COFFEE_IOS)
    return false;
#else
    return true;
#endif
}

bool PlatformData::IsGLES()
{
#ifdef COFFEE_GLEAM_DESKTOP
    return false;
#else
    return true;
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
