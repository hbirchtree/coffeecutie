#include <coffee/core/coffee.h>

#include <coffee/core/plat/environment/linux/environment.h>
#include <coffee/core/plat/plat_file.h>

namespace Coffee {
namespace Environment {
namespace Linux {

CString LinuxEnv::ExecutableName(cstring_w)
{
    cstring_w p = realpath("/proc/self/exe", nullptr);
    CString   v = p;
    Mem::CFree(p);
    return v;
}

Url LinuxEnv::ApplicationDir()
{
    file_error ec;

    CString execname = ExecutableName();
    return DirFun::Dirname(execname.c_str(), ec);
}

Url LinuxEnv::GetUserData(cstring orgname, cstring appname)
{
    const constexpr cstring var_snappy = "SNAP_USER_COMMON";

    if(LinuxEnv::ExistsVar(var_snappy))
    {
        return MkUrl(LinuxEnv::GetVar(var_snappy), RSCA::SystemFile);
    } else
    {
        if(!orgname && !appname)
        {
            orgname = GetCurrentApp().organization_name.c_str();
            appname = GetCurrentApp().application_name.c_str();
        }

        Path homedir =
            ((Path(GetUserHome()) + ".local/share") + orgname) + appname;
        return MkUrl(homedir, RSCA::SystemFile);
    }
}

} // namespace Linux

} // namespace Environment
} // namespace Coffee
