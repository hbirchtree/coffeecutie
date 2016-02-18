#include "../../platform_detect.h"

#ifdef COFFEE_WINDOWS
#ifndef COFFEE_CORE_PLAT_ENVIRONMENT_DETAILS_WINDOWS_H
#define COFFEE_CORE_PLAT_ENVIRONMENT_DETAILS_WINDOWS_H

#include "../environment_details.h"
#include <windows.h>

namespace Coffee{
namespace Environment{

struct WindowsEnvFun : EnvInterface
{
    STATICINLINE CString ExecutableName(cstring_w = nullptr)
    {
        CString excname;

        HMODULE self = GetModuleHandle(NULL);

        excname.resize(GetModuleFileName(self,NULL,0));
        GetModuleFileName(self,&excname[0],excname.size());

        return excname;
    }
    STATICINLINE CString GetVar(cstring);
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
    STATICINLINE CString ConcatPath(cstring, cstring)
    {
        return "C:\\Users\\";
    }
    STATICINLINE CString GetUserHome()
    {
        return "C:\\Users\\";
    }

    STATICINLINE CString GetUserData(cstring, cstring)
    {
        return "C:\\Users\\";
    }

    STATICINLINE CString ApplicationDir()
    {
        return "";
    }
    STATICINLINE CString CurrentDir()
    {
        return "";
    }
};

}

using ColorMap = EnvColorCodes;
using Env = Environment::WindowsEnvFun;

}

#endif
#endif
