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

    static bigscalar ProcessorFrequency();

    static CoreCnt CpuCount();
    static CoreCnt CoreCount();

    static MemUnit MemTotal();
    static MemUnit MemAvailable();

    static bool HasFPU();

    static bool HasHyperThreading();
};

} // namespace mac
} // namespace env

using SysInfo = env::mac::SysInfo;
using PowerInfo = env::PowerInfoDef;

} // namespace platform
#endif
