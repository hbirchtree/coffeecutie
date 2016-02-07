#ifndef COFFEE_CORE_PLAT_ENV_SYSINFO_H
#define COFFEE_CORE_PLAT_ENV_SYSINFO_H

#include "../sysinfo_def.h"
#include <sys/sysinfo.h>
#include <unistd.h>

namespace Coffee{

struct PlatSysInfo : SysInfoDef
{
    STATICINLINE uint32 CpuCount()
    {
        return sysconf(_SC_NPROCESSORS_ONLN);
    }
    STATICINLINE uint32 CoreCount()
    {
        return sysconf(_SC_NPROCESSORS_ONLN);
    }
    STATICINLINE bool MemVirtualAvailable()
    {
        return true;
    }
    STATICINLINE int64 MemTotal()
    {
        struct sysinfo inf;
        sysinfo(&inf);
        return inf.totalram*inf.mem_unit;
    }
    STATICINLINE int64 MemAvailable()
    {
        struct sysinfo inf;
        sysinfo(&inf);
        return inf.freeram*inf.mem_unit;
    }
    STATICINLINE int64 SwapTotal()
    {
        struct sysinfo inf;
        sysinfo(&inf);
        return inf.totalswap*inf.mem_unit;
    }
    STATICINLINE int64 SwapAvailable()
    {
        struct sysinfo inf;
        sysinfo(&inf);
        return inf.freeswap*inf.mem_unit;
    }
    STATICINLINE NetStatusFlags NetStatus()
    {
        return NetStatLocalOnly;
    }
};

using SysInfo = PlatSysInfo;

}

#endif