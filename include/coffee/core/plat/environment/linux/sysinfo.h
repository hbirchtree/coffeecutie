#pragma once

#include "../../platform_detect.h"

#if defined(COFFEE_LINUX)

#include "../../../coffee_message_macros.h"
#include "../sysinfo_def.h"
#include "../sdlpowerinfo.h"

#include <sys/sysinfo.h>
#include <unistd.h>
#include <sys/utsname.h>

namespace Coffee{
namespace Environment{
namespace Linux{
struct LinuxSysInfo : SysInfoDef
{
    static CString CPUInfoString(bool force = false);

    static Vector<CString> CPUFlags();

    static uint32 CpuCount();
    static uint32 CoreCount();
    STATICINLINE bool MemVirtualAvailable()
    {
        return true;
    }
    STATICINLINE uint64 MemTotal()
    {
        struct sysinfo inf;
        sysinfo(&inf);
        return inf.totalram*inf.mem_unit;
    }
    STATICINLINE uint64 MemAvailable()
    {
        struct sysinfo inf;
        sysinfo(&inf);
        return inf.freeram*inf.mem_unit;
    }
    STATICINLINE uint64 SwapTotal()
    {
        struct sysinfo inf;
        sysinfo(&inf);
        return inf.totalswap*inf.mem_unit;
    }
    STATICINLINE uint64 SwapAvailable()
    {
        struct sysinfo inf;
        sysinfo(&inf);
        return inf.freeswap*inf.mem_unit;
    }

    static HWDeviceInfo Processor();

    static bigscalar ProcessorFrequency();

    static bool HasFPU();

    static bool HasFPUExceptions();

    static uint64 ProcessorCacheSize();

    static bool HasHyperThreading();

    STATICINLINE NetStatusFlags NetStatus()
    {
        return NetStatLocalOnly;
    }

    STATICINLINE CString GetSystemVersion()
    {
	utsname d;
	if(uname(&d)!=0)
	    return "?";
	else
	    return CString(d.release);
    }

private:
    thread_local static CString cached_cpuinfo_string;
};

using LinuxPowerInfo = _SDLPowerInfo;

}
}

using PowerInfo = Environment::Linux::LinuxPowerInfo;
using SysInfo = Environment::Linux::LinuxSysInfo;

}

#endif
