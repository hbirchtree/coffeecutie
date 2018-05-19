#pragma once
#include "../../plat_primary_identify.h"

#if defined(COFFEE_RASPBERRYPI)
#include "../linux/sysinfo.h"

namespace Coffee{
namespace Environment{
namespace Raspberry{

struct RaspberrySysInfo : Linux::LinuxSysInfo
{
    static HWDeviceInfo DeviceName();

    static bigscalar ProcessorFrequency();

    STATICINLINE bool HasFPU()
    {
        return true;
    }
};

}
}

using SysInfo = Environment::Raspberry::RaspberrySysInfo;

}
#endif
