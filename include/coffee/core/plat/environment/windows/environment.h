#include "../../platform_detect.h"

#ifdef COFFEE_WINDOWS
#ifndef COFFEE_CORE_PLAT_ENVIRONMENT_DETAILS_WINDOWS_H
#define COFFEE_CORE_PLAT_ENVIRONMENT_DETAILS_WINDOWS_H

#include "../environment_details.h"
#include <windows.h>
#include <cstdlib>
#include <Pathcch.h>

namespace Coffee{
namespace Environment{

struct WindowsEnvFun : EnvInterface
{
    STATICINLINE CString ExecutableName(cstring_w = nullptr)
    {
        CString excname;

        excname.resize(MAX_PATH);
        GetModuleFileName(nullptr,&excname[0],excname.size());

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
        return GetVar("USERPROFILE");
    }

    STATICINLINE CString GetUserData(cstring org, cstring app)
    {
		CString out;
		out = GetUserHome();
		out += GetPathSep();
		out += org;
		out += GetPathSep();
		out += app;
        return out;
    }

    STATICINLINE CString ApplicationDir()
    {
		return DirName(ExecutableName());
    }
    STATICINLINE CString CurrentDir()
    {
		CString out;

		out.resize(GetCurrentDirectory(0,nullptr));
		GetCurrentDirectory(out.size(),&out[0]);

        return out;
    }
	STATICINLINE CString DirName(CString fn)
	{
		std::wstring fn_w(fn.begin(), fn.end());
		PathCchRemoveFileSpec(&fn_w[0], fn_w.size());
		CString out(fn_w.begin(),fn_w.end());
		out.resize(StrLen(out.c_str()));
		return out;
	}
	STATICINLINE CString BaseName(CString fn)
	{
		CString sep = GetPathSep();
		cstring out_f = Search::ChrFindR(fn.c_str(), sep[0]);
		CString out;
		if (!out_f)
			out = fn;
		else
			out = out_f+1;
		return out;
	}
};

}

using ColorMap = EnvColorCodes;
using Env = Environment::WindowsEnvFun;

}

#endif
#endif
