#pragma once

#if defined(__EMSCRIPTEN__) || defined(COFFEE_NACL)

#include "../sysinfo_def.h"

namespace Coffee {
namespace Environment{
namespace Emscripten {

struct EmSysInfo : SysInfoDef
{
    static CString GetSystemVersion();
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
