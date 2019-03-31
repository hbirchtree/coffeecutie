#pragma once

#include <peripherals/base.h>

#if defined(COFFEE_RASPBERRYPI)
#include <coffee/core/types/hardware_info.h>
#include <platforms/linux/sysinfo.h>

namespace platform {
namespace env {
namespace raspberry {

struct SysInfo : Linux::SysInfo
{
    static info::HardwareDevice DeviceName();

    static bigscalar ProcessorFrequency();

    STATICINLINE bool HasFPU()
    {
        return true;
    }
};

} // namespace raspberry
} // namespace env

using SysInfo = env::raspberry::SysInfo;

} // namespace platform
#endif
