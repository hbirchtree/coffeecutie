#pragma once

#include "../../plat_primary_identify.h"

#ifdef COFFEE_ANDROID

#include "../sysinfo_def.h"

#if defined(COFFEE_USE_SDL2)
#include "../sdlpowerinfo.h"
#endif

namespace Coffee{
namespace Environment{
namespace Android{
struct AndroidSysInfo : SysInfoDef
{
    static CString GetSystemVersion();
    static HWDeviceInfo Processor();
    static HWDeviceInfo DeviceName();
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

}
}

using PowerInfo = Environment::Android::AndroidPowerInfo;
using SysInfo = Environment::Android::AndroidSysInfo;

}

#endif
