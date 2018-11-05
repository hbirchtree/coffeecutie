#pragma once

#include <coffee/core/base.h>

#ifdef COFFEE_ANDROID

#include "../linux/sysinfo.h"

#if defined(COFFEE_USE_SDL2)
#include "../sdlpowerinfo.h"
#endif

namespace Coffee {
namespace Environment {
namespace Android {
struct AndroidSysInfo : Linux::LinuxSysInfo
{
    static CString      GetSystemVersion();
    static HWDeviceInfo DeviceName();
    static HWDeviceInfo Motherboard();

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

} // namespace Android
} // namespace Environment

using PowerInfo = Environment::Android::AndroidPowerInfo;
using SysInfo   = Environment::Android::AndroidSysInfo;

} // namespace Coffee

#endif
