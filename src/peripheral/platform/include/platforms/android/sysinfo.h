#pragma once

#include <peripherals/base.h>

#ifdef COFFEE_ANDROID

#include "../linux/sysinfo.h"

#if defined(COFFEE_USE_SDL2)
#include "../sdlpowerinfo.h"
#endif

namespace platform {
namespace env {
namespace android {
struct SysInfo : Linux::SysInfo
{
    static CString      GetSystemVersion();
    static info::HardwareDevice DeviceName();
    static info::HardwareDevice Motherboard();

    static u32     CoreCount();
    static ThrdCnt ThreadCount();
};

struct PowerInfo : PowerInfoDef
{
    static u16 BatteryPercentage();

    static Temp CpuTemperature();
    static Temp GpuTemperature();
};

} // namespace android
} // namespace env

using PowerInfo = env::android::PowerInfo;
using SysInfo   = env::android::SysInfo;

} // namespace platform

#endif
