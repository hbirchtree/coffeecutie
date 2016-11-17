#include <coffee/core/plat/environment/osx/sysinfo.h>

#include <sys/types.h>
#include <sys/sysctl.h>

namespace Coffee{
namespace Environment{
namespace Mac{

HWDeviceInfo MacSysInfo::DeviceName()
{
    static const cstring mod_string = "hw.model";

    CString target;

    size_t len = 0;
    sysctlbyname(mod_string, nullptr, &len, nullptr, 0);

    if(len > 0)
    {
        target.resize(len+1);
        sysctlbyname(mod_string, &target[0], &len, nullptr, 0);
    }

    return HWDeviceInfo("Apple", target, GetSystemVersion());
}

}
}
}
