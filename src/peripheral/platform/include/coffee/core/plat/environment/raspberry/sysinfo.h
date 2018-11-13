#pragma once

#include <coffee/core/base.h>

#if defined(COFFEE_RASPBERRYPI)
#include "../linux/sysinfo.h"

namespace Coffee {
namespace Environment {
namespace Raspberry {

struct RaspberrySysInfo : Linux::LinuxSysInfo
{
    static HWDeviceInfo DeviceName();

    static bigscalar ProcessorFrequency();

    STATICINLINE bool HasFPU()
    {
        return true;
    }
};

} // namespace Raspberry
} // namespace Environment

using SysInfo = Environment::Raspberry::RaspberrySysInfo;

} // namespace Coffee
#endif
