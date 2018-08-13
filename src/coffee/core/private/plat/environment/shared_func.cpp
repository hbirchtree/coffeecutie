#include <coffee/core/plat/environment/sysinfo_def.h>

#if defined(COFFEE_UNIXPLAT)
#include <unistd.h>
#elif defined(COFFEE_WINDOWS)
#include <coffee/core/plat/plat_windows.h>
#endif

namespace Coffee{

CString SysInfoDef::GetSystemString()
{
    /* Please don't ask about the leading space :) */
    const constexpr cstring _fmt = "%.3s%u_%s ";
    int len = snprintf(nullptr,0,_fmt,C_SYSTEM_STRING,BitNess(),COFFEE_ARCH);
    CString base;
    base.resize(len);
    snprintf(&base[0],base.size(),_fmt,C_SYSTEM_STRING,BitNess(),COFFEE_ARCH);
    base.resize(base.find('\0'));
    base = str::fmt::lower(base);
    return base;
}

CString SysInfoDef::HostName()
{
#if !defined(NDEBUG)

#if defined(COFFEE_UNIXPLAT) || defined(COFFEE_WINDOWS)
    /* For now, we assume this works. We might implement a better one where it retries upon failure. */
    CString _m;
    _m.resize(60);
    gethostname(&_m[0],_m.size());
    _m.resize(_m.find('\0'));
    return _m;
#else
    return "localhost";
#endif

#else
    return "";
#endif
}

ThrdCnt SysInfoDef::SmartParallelism(u64 worksize, u64 weight)
{
    if(worksize*weight <= ThreadCount())
    {
        return 1;
    }else if(worksize*weight <= CMath::pow(Parallelism(),3))
    {
        return ThreadCount();
    }else
        return Parallelism();
}

#if !defined(COFFEE_LINUX) || !defined(COFFEE_WINDOWS)
HWDeviceInfo SysInfoDef::Processor(u32 i)
{
    C_UNUSED(i);
    return HWDeviceInfo("Generic Processor","0x0");
}
#endif

#if !defined(COFFEE_LINUX) || !defined(COFFEE_ANDROID)
HWDeviceInfo SysInfoDef::DeviceName()
{
    return HWDeviceInfo("Generic","Device","0x0");
}

HWDeviceInfo SysInfoDef::Motherboard()
{
    return HWDeviceInfo("Generic", "Motherboard", "0x0");
}

HWDeviceInfo SysInfoDef::Chassis()
{
    return HWDeviceInfo("Generic", "Chassis", "0x0");
}

HWDeviceInfo SysInfoDef::BIOS()
{
    return HWDeviceInfo("Generic", "BIOS", "0x0");
}
#endif

}
