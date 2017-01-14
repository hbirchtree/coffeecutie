#pragma once

#ifdef __EMSCRIPTEN__

#include "../sysinfo_def.h"

namespace Coffee {
namespace Environment{
namespace Emscripten {

struct EmSysInfo : SysInfoDef
{
};
struct EmPowerInfo : PowerInfoDef
{
};

}
}

using PowerInfo = Environment::Emscripten::EmPowerInfo;
using SysInfo = Environment::Emscripten::EmSysInfo;

}
#endif
