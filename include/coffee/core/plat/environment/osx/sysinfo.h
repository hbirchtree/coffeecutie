#pragma once

#include "../../plat_primary_identify.h"

//#if defined(COFFEE_APPLE)

#include "../sysinfo_def.h"
#include "../sdlpowerinfo.h"

#include <stdio.h>

namespace Coffee{
namespace Environment{
namespace Mac{

struct MacSysInfo : SysInfoDef
{
    static CString GetSystemVersion();
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

}
}

using PowerInfo = Environment::Mac::MacPowerInfo;
using SysInfo = Environment::Mac::MacSysInfo;

}
//#endif
