#pragma once

#include <coffee/core/plat/plat_primary_identify.h>

#if defined(COFFEE_STUBBED_SYSINFO)

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
