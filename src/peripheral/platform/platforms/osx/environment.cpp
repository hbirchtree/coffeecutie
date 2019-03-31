#include <platforms/osx/environment.h>

#include <coffee/core/base_state.h>

namespace platform{
namespace env{
namespace mac{

using namespace ::Coffee::State;

Url EnvironmentF::GetUserData(cstring orgname, cstring appname)
{
    auto appData = GetAppData();

    if(!orgname && !appname && appData)
    {
        orgname = appData->organization_name.c_str();
        appname = appData->application_name.c_str();
    }

    CString dir = GetVar("HOME");
    dir = ConcatPath(dir.c_str(),"Library/Application Support");
    dir = ConcatPath(dir.c_str(),orgname);
    dir = ConcatPath(dir.c_str(),appname);
    return constructors::MkUrl(dir, RSCA::SystemFile);
}

}
}
}
