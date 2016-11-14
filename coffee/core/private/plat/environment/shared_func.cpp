#include <coffee/core/plat/environment/sysinfo_def.h>

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
    base = Mem::StrUtil::lower(base);
    return base;
}

CString SysInfoDef::HostName()
{
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
}

ThrdCnt SysInfoDef::SmartParallelism(uint64 worksize, uint64 weight)
{
    if(worksize*weight <= ThreadCount())
    {
        return 1;
    }else if(worksize*weight <= CMath::pow<uint64>(Parallelism(),3))
    {
        return ThreadCount();
    }else
        return Parallelism();
}

HWDeviceInfo SysInfoDef::Processor()
{
    return HWDeviceInfo("Generic Processor","0x0");
}

HWDeviceInfo SysInfoDef::DeviceName()
{
    return HWDeviceInfo("Generic","Device","0x0");
}

}