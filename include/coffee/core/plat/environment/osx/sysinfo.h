#pragma once

#include "../../plat_primary_identify.h"

#if defined(COFFEE_APPLE)

#include "../sysinfo_def.h"
#include "../sdlpowerinfo.h"

#include <stdio.h>

namespace Coffee{
namespace Environment{
namespace Mac{

struct MacSysInfo : SysInfoDef
{
    STATICINLINE CString GetSystemVersion()
    {
        FILE* out = popen("sw_vers -productVersion","r");
        if(out)
        {
            char buf[16];
            char* ptr = fgets(buf, sizeof(buf),out);
            pclose(out);
            CString output = ptr;
            output.resize(Search::ChrFind((cstring)ptr,'\n')-ptr);
            return output;
        }else
            return "0.0";
    }
    static
    HWDeviceInfo DeviceName();
};

using MacPowerInfo = _SDLPowerInfo;

}
}

using PowerInfo = Environment::Mac::MacPowerInfo;
using SysInfo = Environment::Mac::MacSysInfo;

}
#endif
