#pragma once

#ifdef COFFEE_ANDROID

#include "../sysinfo_def.h"
#include "../sdlpowerinfo.h"

namespace Coffee{
namespace Environment{
namespace Android{
struct AndroidSysInfo : SysInfoDef
{
};

struct AndroidPowerInfo : SDLPowerInfo
{
};

}
}

using PowerInfo = Environment::Android::PowerInfo;
using SysInfo = Environment::Android::AndroidSysInfo;

}

#endif
