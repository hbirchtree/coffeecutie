#include <coffee/core/plat/environment/osx/environment.h>
#include <coffee/core/coffee.h>

namespace Coffee{
namespace Environment{
namespace Mac{

Url MacEnv::GetUserData(cstring orgname, cstring appname)
{
    if(!orgname && !appname)
    {
        orgname = ApplicationData().organization_name.c_str();
        appname = ApplicationData().application_name.c_str();
    }

    CString dir = GetVar("HOME");
    dir = ConcatPath(dir.c_str(),"Library/Application Support");
    dir = ConcatPath(dir.c_str(),orgname);
    dir = ConcatPath(dir.c_str(),appname);
    return MkUrl(dir, RSCA::SystemFile);
}

}
}
}
