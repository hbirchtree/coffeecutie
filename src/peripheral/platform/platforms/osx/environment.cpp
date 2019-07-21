#include <platforms/osx/environment.h>

#include <platforms/pimpl_state.h>

namespace platform{
namespace env{
namespace mac{

Url EnvironmentF::GetUserData(cstring orgname, cstring appname)
{
    auto appData = state->GetAppData();

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
