#include <coffee/core/platform_data.h>
#include <coffee/core/plat/plat_environment.h>


namespace Coffee{

#if defined(COFFEE_LINUX)
namespace Environment{
namespace Linux{
extern CString get_kern_name();
}
}
#endif

CString PlatformData::SystemDisplayString()
{
    const constexpr cstring _fmt = "%s %s %u-bit (%s ";
    CString sys_ver = SysInfo::GetSystemVersion();
    CString sys_name = C_SYSTEM_STRING;
#if defined(COFFEE_LINUX)
    sys_name = Environment::Linux::get_kern_name();
#endif
    int len = snprintf(nullptr,0,_fmt,
                       sys_name.c_str(),
                       sys_ver.c_str(),
                       C_SYSTEM_BITNESS,
                       COFFEE_ARCH);
    CString base;
    base.resize(len);
    snprintf(&base[0],base.size(),_fmt,
            sys_name.c_str(),
            sys_ver.c_str(),
            C_SYSTEM_BITNESS,
            COFFEE_ARCH);
    base.resize(base.find('\0'));
    /* What the fuck. Where does the rest of the string go? */
    base.append(")");
    return base;
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
