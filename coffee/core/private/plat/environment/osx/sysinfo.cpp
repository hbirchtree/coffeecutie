#include <coffee/core/plat/environment/osx/sysinfo.h>

#include <sys/types.h>
#include <sys/sysctl.h>

namespace Coffee{
namespace Environment{
namespace Mac{

static CString _GetSysctlString(const cstring mod_string)
{
    CString target;

    size_t len = 0;
    sysctlbyname(mod_string, nullptr, &len, nullptr, 0);

    if(len > 0)
    {
        target.resize(len+1);
        sysctlbyname(mod_string, &target[0], &len, nullptr, 0);
        target.resize(Mem::Search::ChrFind(&target[0], '\0') - &target[0]);
    }

    return target;
}

CString MacSysInfo::GetSystemVersion()
{
    FILE* out = popen("sw_vers -productVersion","r");
    if(out)
    {
        char buf[16];
        char* ptr = fgets(buf, sizeof(buf),out);
        pclose(out);
        CString output = ptr;
        output.resize(Mem::Search::ChrFind((cstring)ptr,'\n')-ptr);
        return output;
    }else
        return "0.0";
}

HWDeviceInfo MacSysInfo::DeviceName()
{
    static const cstring mod_string = "hw.model";
    static const cstring typ_string = "kern.ostype";
    static const cstring rel_string = "kern.osrelease";

    CString target = _GetSysctlString(mod_string);
    CString kern = _GetSysctlString(typ_string);;
    CString osrel = _GetSysctlString(rel_string);;

    return HWDeviceInfo("Apple", target, kern + " " + osrel);
}

}
}
}
