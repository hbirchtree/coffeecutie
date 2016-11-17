#include <coffee/core/plat/environment/osx/sysinfo.h>

#include <sys/types.h>
#include <sys/sysctl.h>

namespace Coffee{
namespace Environment{
namespace Mac{

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
    static const cstring rel_string = "kern.osrelease";
    static const cstring typ_string = "kern.ostype";

    CString target;
    CString kern;
    CString osrel;

    size_t len = 0;
    sysctlbyname(mod_string, nullptr, &len, nullptr, 0);

    if(len > 0)
    {
        target.resize(len+1);
        sysctlbyname(mod_string, &target[0], &len, nullptr, 0);
        target.resize(Mem::Search::ChrFind(&target[0], '\0') - &target[0]);
    }

    len = 0;
    sysctlbyname(rel_string, nullptr, &len, nullptr, 0);

    if(len)
    {
        osrel.resize(len+1);
        sysctlbyname(rel_string, &osrel[0], &len, nullptr, 0);
        osrel.resize(Mem::Search::ChrFind(&osrel[0], '\0') - &osrel[0]);
    }

    len = 0;
    sysctlbyname(rel_string, nullptr, &len, nullptr, 0);

    if(len)
    {
        kern.resize(len+1);
        sysctlbyname(rel_string, &kern[0], &len, nullptr, 0);
        kern.resize(Mem::Search::ChrFind(&kern[0], '\0') - &kern[0]);
    }

    return HWDeviceInfo("Apple", target, kern + " " + osrel);
}

}
}
}
