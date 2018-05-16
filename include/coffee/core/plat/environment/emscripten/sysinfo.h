#pragma once

#include <coffee/core/plat/plat_quirks_toggling.h>

#if defined(COFFEE_EMSCRIPTEN)

#include "../sysinfo_def.h"
#include <malloc.h>

extern "C" char* get_user_agent();

namespace Coffee {
namespace Environment{
namespace Emscripten {

struct EmSysInfo : SysInfoDef
{
    static CString GetSystemVersion();

    STATICINLINE HWDeviceInfo DeviceName()
    {
        cstring_w userAgentRaw = get_user_agent();
        if(!userAgentRaw)
            return HWDeviceInfo("Generic", "Browser", "1.0");

        CString userAgent = userAgentRaw;
        free(userAgentRaw);
        return HWDeviceInfo(userAgent, "", "1.0");
    }
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
