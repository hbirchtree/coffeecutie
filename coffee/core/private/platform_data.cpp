#include <coffee/core/platform_data.h>
#include <coffee/core/plat/plat_environment.h>


namespace Coffee{

CString PlatformData::SystemDisplayString()
{
    const constexpr cstring _fmt = "%s %s %u-bit (%s ";
    CString sys_ver = SysInfo::GetSystemVersion();
    int len = snprintf(nullptr,0,_fmt,
                       C_SYSTEM_STRING,
                       sys_ver.c_str(),
                       C_SYSTEM_BITNESS,
                       COFFEE_ARCH);
    CString base;
    base.resize(len);
    snprintf(&base[0],base.size(),_fmt,
            C_SYSTEM_STRING,
            sys_ver.c_str(),
            C_SYSTEM_BITNESS,
            COFFEE_ARCH);
    base.resize(base.find('\0'));
    base.append(")");
    return base;
}

}
