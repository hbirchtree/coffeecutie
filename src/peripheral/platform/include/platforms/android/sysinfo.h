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

#if defined(COFFEE_USE_SDL2)
struct PowerInfo : _SDLPowerInfo
{
};
#else
struct PowerInfo : PowerInfoDef
{
};
#endif

} // namespace android
} // namespace env

using PowerInfo = env::android::PowerInfo;
using SysInfo   = env::android::SysInfo;

} // namespace platform

#endif
