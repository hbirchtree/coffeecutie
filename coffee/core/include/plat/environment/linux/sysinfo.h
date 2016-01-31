#ifndef COFFEE_CORE_PLAT_ENV_SYSINFO_H
#define COFFEE_CORE_PLAT_ENV_SYSINFO_H

#include "../sysinfo_def.h"
#include <sys/sysinfo.h>
#include <unistd.h>

namespace Coffee{

struct PlatSysInfo : SysInfoDef
{
    static uint32 CpuCount()
    {
        return sysconf(_SC_NPROCESSORS_ONLN);
    }
    static uint32 CpuCoreCount()
    {
        return sysconf(_SC_NPROCESSORS_ONLN);
    }
    static bool MemVirtualAvailable()
    {
        return true;
    }
    static int64 MemTotal()
    {
        struct sysinfo inf;
        sysinfo(&inf);
        return inf.totalram*inf.mem_unit;
    }
    static int64 MemAvailable()
    {
        struct sysinfo inf;
        sysinfo(&inf);
        return inf.freeram*inf.mem_unit;
    }
    static int64 SwapTotal()
    {
        struct sysinfo inf;
        sysinfo(&inf);
        return inf.totalswap*inf.mem_unit;
    }
    static int64 SwapAvailable()
    {
        struct sysinfo inf;
        sysinfo(&inf);
        return inf.freeswap*inf.mem_unit;
    }
    static NetStatusFlags NetStatus()
    {
        return NetStatLocalOnly;
    }
};

using SysInfo = PlatSysInfo;

}

#endif