#pragma once

#include <peripherals/base.h>

#if defined(COFFEE_APPLE)

#include <platforms/base/sysinfo.h>

#include <stdio.h>

namespace platform {
namespace env {
namespace mac {

struct SysInfo : SysInfoDef
{
    static CString      GetSystemVersion();
    static info::HardwareDevice DeviceName();

    static info::HardwareDevice Processor();

    static bigscalar ProcessorFrequency();

    static CoreCnt CpuCount();
    static CoreCnt CoreCount();

    static MemUnit MemTotal();
    static MemUnit MemAvailable();

    static bool HasFPU();

    static bool HasHyperThreading();
};

} // namespace Mac
} // namespace Environment

using SysInfo   = env::mac::SysInfo;

} // namespace Coffee
#endif
