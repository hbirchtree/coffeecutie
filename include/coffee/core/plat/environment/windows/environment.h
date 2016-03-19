#include "../../platform_detect.h"

#ifdef COFFEE_WINDOWS
#ifndef COFFEE_CORE_PLAT_ENVIRONMENT_DETAILS_WINDOWS_H
#define COFFEE_CORE_PLAT_ENVIRONMENT_DETAILS_WINDOWS_H

#include "../environment_details.h"
#include <windows.h>
#include <cstdlib>

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
	STATICINLINE CString GetVar(cstring v)
	{
		CString out;

		sbyte_t* var = std::getenv(v);

		out = var;

		return out;
	}
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
        return GetVar("HOME");
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
