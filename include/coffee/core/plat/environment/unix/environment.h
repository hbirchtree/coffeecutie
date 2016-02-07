#include "../../platform_detect.h"

#ifdef COFFEE_UNIXPLAT
#ifndef COFFEE_CORE_PLAT_ENVIRONMENT_DETAILS_LINUX_H
#define COFFEE_CORE_PLAT_ENVIRONMENT_DETAILS_LINUX_H

#include "../environment_details.h"

#include <libgen.h>
#include <unistd.h>
#include <stdlib.h>
#include <limits.h>

namespace Coffee{
namespace Environment{

struct PosixEnvironmentFun : EnvInterface
{
    STATICINLINE CString ExecutableName(cstring_w = nullptr)
    {
	return realpath("/proc/self/exe",nullptr);
    }

    STATICINLINE CString GetVar(cstring var)
    {
	return getenv(var);
    }
    STATICINLINE bool SetVar(cstring var, cstring val)
    {
	return setenv(var,val,1)==0;
    }
    STATICINLINE bool UnsetVar(cstring var)
    {
	return unsetenv(var)==0;
    }
    STATICINLINE bool ClearEnv()
    {
	return clearenv()==0;
    }

    STATICINLINE CString GetPathSep()
    {
	return CString("/");
    }
    STATICINLINE CString ConcatPath(cstring v1, cstring v2)
    {
	return v1+GetPathSep()+v2;
    }

    STATICINLINE CString GetUserHome()
    {
	return GetVar("HOME");
    }
    STATICINLINE CString GetUserData(cstring orgname, cstring appname)
    {
	CString homedir = GetUserHome();
	homedir = ConcatPath(homedir.c_str(),".local/share");
	homedir = ConcatPath(homedir.c_str(),orgname);
	return ConcatPath(homedir.c_str(),appname);
    }
    STATICINLINE CString ApplicationDir()
    {
	CString tmp = ExecutableName();
	CString stmp = dirname(&tmp[0]);
	return stmp;
    }
    STATICINLINE CString CurrentDir()
    {
	CString dir;
	dir.resize(PATH_MAX);
	getcwd(&dir[0],PATH_MAX);
	return dir;
    }
};

}

using Env = Environment::PosixEnvironmentFun;

}

#endif
#endif
