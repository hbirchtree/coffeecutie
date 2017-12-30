#pragma once

#include "../../platform_detect.h"

#if defined(COFFEE_LINUX) || defined(COFFEE_ANDROID)

#include "../../../coffee_message_macros.h"
#include "../sysinfo_def.h"

#if !defined(COFFEE_ANDROID)
#include "../sdlpowerinfo.h"
#endif

#include <sys/sysinfo.h>
#include <unistd.h>
#include <sys/utsname.h>

namespace Coffee{
namespace Environment{
namespace Linux{
struct LinuxSysInfo : SysInfoDef
{
    static CString CPUInfoString(bool force = false);

    static Set<CString> CPUFlags();

    static NetStatusFlags NetStatus();

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

    static Vector<bigscalar> ProcessorFrequencies();
    static bigscalar ProcessorFrequency();

    static bool HasFPU();

    static bool HasFPUExceptions();

    static uint64 ProcessorCacheSize();

    static bool HasHyperThreading();

    static CString GetSystemVersion();

    static HWDeviceInfo DeviceName();

    static HWDeviceInfo Motherboard();

    static HWDeviceInfo Chassis();

    static HWDeviceInfo BIOS():

private:
    static void FreeCPUInfoString();
    static CString cached_cpuinfo_string;
};

struct LinuxPowerInfo :
        #if !defined(COFFEE_ANDROID)
        _SDLPowerInfo
        #else
        PowerInfoDef
        #endif
{
    static Temp CpuTemperature();
};

}
}

#if !defined(COFFEE_ANDROID)
using PowerInfo = Environment::Linux::LinuxPowerInfo;
#if !defined(COFFEE_RASPBERRYPI)
using SysInfo = Environment::Linux::LinuxSysInfo;
#endif
#endif

}

#endif
