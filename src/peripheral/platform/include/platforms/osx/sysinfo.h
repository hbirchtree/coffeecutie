#pragma once

#include <peripherals/base.h>

#if defined(COFFEE_APPLE)

#include <platforms/base/sysinfo.h>

#include <stdio.h>

namespace platform {
namespace env {
namespace mac {

extern CString get_kern_name();
extern CString get_kern_ver();

struct SysInfo : SysInfoDef
{
    static CString              GetSystemVersion();
    static info::HardwareDevice DeviceName();

    static info::HardwareDevice Processor();

    static Vector<bigscalar> ProcessorFrequencies(u32 = 0);

    static CoreCnt CpuCount();
    static CoreCnt CoreCount();

    static MemUnit MemTotal();
    static MemUnit MemAvailable();

    static MemUnit MemResident();

    static bool HasFPU();

    static bool HasHyperThreading();
};

struct PowerInfo : PowerInfoDef
{
    static Temp CpuTemperature();

    static Temp GpuTemperature();
    
    static bool IsPowered();
    static bool IsCharging();
    static bool HasBattery();
    static u16 BatteryPercentage();
};

} // namespace mac
} // namespace env

using SysInfo   = env::mac::SysInfo;
using PowerInfo = env::mac::PowerInfo;

namespace mac {

extern void get_display_dpi(stl_types::Vector<libc_types::f32>& dpis);

}
} // namespace platform
#endif
