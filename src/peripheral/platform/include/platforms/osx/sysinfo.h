#pragma once

#include <peripherals/base.h>

#if defined(COFFEE_APPLE)

#include "../sdlpowerinfo.h"
#include "../sysinfo_def.h"

#include <stdio.h>

namespace Coffee {
namespace Environment {
namespace Mac {

struct MacSysInfo : SysInfoDef
{
    static CString      GetSystemVersion();
    static HWDeviceInfo DeviceName();

    static HWDeviceInfo Processor();

    static bigscalar ProcessorFrequency();

    static CoreCnt CpuCount();
    static CoreCnt CoreCount();

    static MemUnit MemTotal();
    static MemUnit MemAvailable();

    static bool HasFPU();

    static bool HasHyperThreading();
};

using MacPowerInfo = _SDLPowerInfo;

} // namespace Mac
} // namespace Environment

using PowerInfo = Environment::Mac::MacPowerInfo;
using SysInfo   = Environment::Mac::MacSysInfo;

} // namespace Coffee
#endif
