#pragma once

#ifdef COFFEE_ANDROID

#include "../sysinfo_def.h"

#if !defined(ANDROID_DONT_USE_SDL2)
#include "../sdlpowerinfo.h"
#else
#include <coffee/android/android_main.h>
#endif

namespace Coffee{
namespace Environment{
namespace Android{
struct AndroidSysInfo : SysInfoDef
{
    STATICINLINE
    CString GetSystemVersion()
    {
#if !defined(ANDROID_DONT_USE_SDL2)
        return "[0]";
#else
        return Convert::inttostring(Coffee_AndroidGetApiVersion());
#endif
    }
};


#if !defined(ANDROID_DONT_USE_SDL2)
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
