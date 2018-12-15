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
struct AndroidPowerInfo : _SDLPowerInfo
{
};
#else
struct AndroidPowerInfo : PowerInfoDef
{
};
#endif

} // namespace android
} // namespace env

using PowerInfo = Environment::Android::AndroidPowerInfo;
using SysInfo   = Environment::Android::AndroidSysInfo;

} // namespace platform

#endif
