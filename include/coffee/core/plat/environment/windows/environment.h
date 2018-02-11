#include "../../platform_detect.h"

#ifdef COFFEE_WINDOWS
#ifndef COFFEE_CORE_PLAT_ENVIRONMENT_DETAILS_WINDOWS_H
#define COFFEE_CORE_PLAT_ENVIRONMENT_DETAILS_WINDOWS_H

#include "../environment_details.h"

namespace Coffee{
namespace Environment{
namespace Windows {

struct WindowsEnvFun : EnvInterface
{
    static CString ExecutableName(cstring_w = nullptr);

    static Variables Environment();

	static bool ExistsVar(cstring v);

    static CString GetVar(cstring v);

    STATICINLINE bool SetVar(cstring, cstring)
    {
        return false;
    }
    STATICINLINE bool UnsetVar(cstring)
    {
        return false;
    }
    STATICINLINE bool ClearEnv()
    {
        return false;
    }

    STATICINLINE CString GetPathSep()
    {
        return "\\";
    }
    STATICINLINE CString ConcatPath(cstring f, cstring fv)
    {
        CString out;
        out += f;
        out += GetPathSep();
        out += fv;
        return out;
    }
    STATICINLINE CString GetUserHome()
    {
        return GetVar("USERPROFILE");
    }

    static CString GetUserData(cstring org, cstring app);

    STATICINLINE CString ApplicationDir()
    {
        CString fn = ExecutableName();
        return DirName(fn.c_str());
    }
    static CString CurrentDir();
    static CString DirName(cstring fn);
    static CString BaseName(cstring fn);
};
}

}

using ColorMap_ = Environment::EnvColorCodes;
using Env_ = Environment::Windows::WindowsEnvFun;

}

#endif
#endif
