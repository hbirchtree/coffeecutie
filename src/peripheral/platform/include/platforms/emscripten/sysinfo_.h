#pragma once

#include <peripherals/base.h>

#if defined(COFFEE_EMSCRIPTEN)

#include <coffee/core/types/hardware_info.h>
#include <malloc.h>
#include <platforms/base/sysinfo.h>

extern "C" char* get_user_agent();

namespace platform {
namespace env {
namespace emscripten {

struct SysInfo : SysInfoDef
{
    static CString GetSystemVersion();

    STATICINLINE info::HardwareDevice DeviceName()
    {
//        cstring_w userAgentRaw = get_user_agent();
//        if(!userAgentRaw)
//            return info::HardwareDevice("Generic", "Browser", "1.0");

//        CString userAgent = userAgentRaw;
//        free(userAgentRaw);
//        return info::HardwareDevice(userAgent, "", "1.0");
        return {};
    }
};

} // namespace emscripten
} // namespace env

using SysInfo = env::emscripten::SysInfo;

} // namespace platform
#endif
