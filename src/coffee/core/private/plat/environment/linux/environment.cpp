#include <coffee/core/plat/environment/linux/environment.h>
#include <coffee/core/coffee.h>

namespace Coffee{
namespace Environment{
namespace Linux{

CString LinuxEnv::GetUserData(cstring orgname, cstring appname)
{
    const constexpr cstring var_snappy = "SNAP_USER_COMMON";

    if(LinuxEnv::ExistsVar(var_snappy))
    {
        return LinuxEnv::GetVar(var_snappy);
    }else{
        if(!orgname && !appname)
        {
            orgname = GetCurrentApp().organization_name.c_str();
            appname = GetCurrentApp().application_name.c_str();
        }

        CString homedir = GetUserHome();
        homedir = ConcatPath(homedir.c_str(),".local/share");
        homedir = ConcatPath(homedir.c_str(),orgname);
        return ConcatPath(homedir.c_str(),appname);
    }
}

}

}
}
