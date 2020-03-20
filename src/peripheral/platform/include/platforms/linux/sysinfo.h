#pragma once

#include <peripherals/base.h>

#if defined(COFFEE_LINUX) || defined(COFFEE_ANDROID)

#include <platforms/base/sysinfo.h>

#if !defined(COFFEE_ANDROID)
#include <platforms/base/power.h>
#endif

#include <sys/sysinfo.h>
#include <sys/utsname.h>
#include <unistd.h>

namespace platform {
namespace env {
namespace Linux {

struct lsb_data
{
    CString distribution;
    CString release;
};

struct SysInfo : SysInfoDef
{
    static CString CPUInfoString(bool force = false);

    static Set<CString> CPUFlags();

    static NetStatusFlags NetStatus();

    static u32 CpuCount();

    static u32 CoreCount();

    STATICINLINE
    /*!
     * \brief Return CPU usage into provided vector, as percentage
     * One value designates the usage of one CPU core/thread
     * It is acceptable to return one CPU-wide value.
     * Vector shall be empty if no value can be found.
     */
    void GetCpuUsage(Vector<u8>&)
    {
        return;
    }

    static u64 CachedMemory();

    STATICINLINE bool MemVirtualAvailable()
    {
        return true;
    }

    STATICINLINE u64 MemTotal()
    {
        struct sysinfo inf;
        sysinfo(&inf);
        return inf.totalram * inf.mem_unit;
    }

    STATICINLINE u64 MemAvailable()
    {
        struct sysinfo inf;
        sysinfo(&inf);

        /* This is only here for easier debugging */
        u64 free = inf.freeram;
        free += inf.bufferram;
        free += CachedMemory();

        return free * inf.mem_unit;
    }

    STATICINLINE u64 SwapTotal()
    {
        struct sysinfo inf;
        sysinfo(&inf);
        return inf.totalswap * inf.mem_unit;
    }

    STATICINLINE u64 SwapAvailable()
    {
        struct sysinfo inf;
        sysinfo(&inf);
        return inf.freeswap * inf.mem_unit;
    }

    static info::HardwareDevice Processor();

    static Vector<bigscalar> ProcessorFrequencies();

    static bigscalar ProcessorFrequency();

    static bool HasFPU();

    static bool HasFPUExceptions();

    static u64 ProcessorCacheSize();

    static bool HasHyperThreading();

    static CString GetSystemVersion();

    static info::HardwareDevice DeviceName();

    static info::HardwareDevice Motherboard();

    static info::HardwareDevice Chassis();

    static info::HardwareDevice BIOS();

  private:
    static void    FreeCPUInfoString();
    static CString cached_cpuinfo_string;
};

struct PowerInfo :
#if !defined(COFFEE_ANDROID)
    _SDLPowerInfo
#else
    PowerInfoDef
#endif
{
    static Temp CpuTemperature();
};

} // namespace Linux
} // namespace env

#if !defined(COFFEE_ANDROID)
using PowerInfo = env::Linux::PowerInfo;
#if !defined(COFFEE_RASPBERRYPI)
using SysInfo = env::Linux::SysInfo;
#endif
#endif

} // namespace platform

#endif
