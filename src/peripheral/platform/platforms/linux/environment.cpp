#include <coffee/core/plat/environment/linux/environment.h>

#include <coffee/core/plat/file.h>
#include <peripherals/semantic/ptr_wrap.h>

namespace Coffee {
namespace Environment {
namespace Linux {

using namespace semantic;

CString LinuxEnv::ExecutableName(cstring_w)
{
    Ptr<char, ptr_opts::managed_ptr> p = realpath("/proc/self/exe", nullptr);

    CString v = p.get();
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
