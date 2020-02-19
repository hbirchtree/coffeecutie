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

    static uint16 BatteryPercentage();
    static uint32 BatteryLifetime();
};
#else
using _SDLPowerInfo = PowerInfoDef;
#endif
}
} // namespace platform
