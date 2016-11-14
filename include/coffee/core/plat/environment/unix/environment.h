#pragma once

#include "../../plat_primary_identify.h"

#if defined(COFFEE_UNIXPLAT) && !defined(COFFEE_WINDOWS)

#include "../environment_details.h"
#include "../../memory/cmemory.h"

// getenv, setenv, unsetenv, clearenv
#include <stdlib.h>

extern "C" char** environ;

namespace Coffee{
namespace Environment{
namespace Posix{

struct PosixEnvironmentFun : EnvInterface
{
    static CString BaseName(cstring n);

    static CString DirName(cstring fname);

    STATICINLINE bool ExistsVar(cstring var)
    {
        return getenv(var);
    }
    STATICINLINE CString GetVar(cstring var)
    {
        cstring val = getenv(var);
        return val ? CString(val) : CString();
    }
    STATICINLINE bool SetVar(cstring var, cstring val)
    {
        return setenv(var,val,1)==0;
    }

    static bool PrependVar(cstring var, cstring val);
    static bool AppendVar(cstring var, cstring val);

    STATICINLINE bool UnsetVar(cstring var)
    {
        return unsetenv(var)==0;
    }
    STATICINLINE bool ClearEnv()
    {
#if !defined(COFFEE_APPLE)
        return clearenv()==0;
#endif
        return false;
    }

    STATICINLINE CString GetPathSep()
    {
	return CString("/");
    }
    static CString ConcatPath(cstring v1, cstring v2);

    STATICINLINE CString GetUserHome()
    {
	return GetVar("HOME");
    }

    static CString CurrentDir();
    static Variables Environment();
};

struct PosixTerminalColorCodes : EnvColorCodes
{
    // Because Android doesn't have a proper terminal

    static CString& ColorText(CString& text, CmdFormat fmt);


};

}
}

}

#endif
