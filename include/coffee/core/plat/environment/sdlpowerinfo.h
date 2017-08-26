#pragma once

#include "sysinfo_def.h"

namespace Coffee{
namespace Environment{

#if defined(COFFEE_USE_SDL2)
struct _SDLPowerInfo : PowerInfoDef
{
    static bool IsPowered();
    static bool IsCharging();
    static bool HasBattery();

    static uint16 BatteryPercentage();
    static uint32 BatteryLifetime();
};
#else
using _SDLPowerInfo  = PowerInfoDef;
#endif

}
}
