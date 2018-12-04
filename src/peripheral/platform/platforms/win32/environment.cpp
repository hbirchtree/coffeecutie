#include <coffee/core/plat/environment/windows/environment.h>

#include <coffee/core/coffee.h>
#include <coffee/core/plat/plat_memory.h>
#include <coffee/core/plat/plat_windows.h>
#include <stdlib.h>
#include <wchar.h>

namespace Coffee {
namespace Environment {
namespace Windows {
CString WindowsEnvFun::ExecutableName(cstring_w)
{
    CWString excname;
    excname.resize(MAX_PATH);

    GetModuleFileNameW(nullptr, &excname[0], excname.size());
    excname.resize(excname.find(L'\0'));

    excname = str::replace::str(excname, L"\\", L"/");

    return str::encode::to<char>(excname);
}

CString WindowsEnvFun::GetVar(cstring v)
{
    CString out;

#ifndef COFFEE_WINDOWS_UWP
    cstring_w var = getenv(v);

    if(var)
        out = var;
#endif

    return out;
}

WindowsEnvFun::Variables WindowsEnvFun::Environment()
{
#ifdef COFFEE_WINDOWS_UWP
    return {};
#else
    TCHAR* env = GetEnvironmentStrings();
    if(!env)
        return {};
    Variables var;

    while(env[0] != TCHAR(0))
    {
        CString p_r;

        p_r = env;

        szptr key_end = p_r.find('=');

        var[p_r.substr(0, key_end)] = p_r.substr(key_end);

        env = (TCHAR*)str::find(env, TCHAR(0));
        env += 1; // Skipping the expected \0
    }

    return var;
#endif
}

bool WindowsEnvFun::ExistsVar(cstring v)
{
#ifdef COFFEE_WINDOWS_UWP
    return false;
#else
    return getenv(v) != nullptr;
#endif
}

Url WindowsEnvFun::CurrentDir()
{
    CWString out;
    out.resize(GetCurrentDirectoryW(0, nullptr));
    GetCurrentDirectoryW(out.size(), &out[0]);
    out.resize(out.size() - 1);

    out = str::replace::str(out, L"\\", L"/");
    return MkUrl(str::encode::to<char>(out), RSCA::SystemFile);
}

Url WindowsEnvFun::GetUserData(cstring org, cstring app)
{
    if(!org && !app)
    {
        org = ApplicationData().organization_name.c_str();
        app = ApplicationData().application_name.c_str();
    }

    CString out;
    out = GetUserHome().internUrl;
    out = ConcatPath(out.c_str(), "AppData");
    out = ConcatPath(out.c_str(), "Local");
    out = ConcatPath(out.c_str(), org);
    out = ConcatPath(out.c_str(), app);
    out = str::replace::str(out, "\\", "/");
    return MkUrl(out, RSCA::SystemFile);
}
} // namespace Windows
} // namespace Environment
} // namespace Coffee