#pragma once

#include <platforms/base/sysinfo.h>

namespace platform {
namespace env {

#if defined(FEATURE_ENABLE_SDLPowerInfo)
struct _SDLPowerInfo : PowerInfoDef
{
    static bool IsPowered();
    static bool IsCharging();
    static bool HasBattery();

    static u16 BatteryPercentage();
    static u32 BatteryLifetime();
};
#else
using _SDLPowerInfo = PowerInfoDef;
#endif
} // namespace env
} // namespace platform
