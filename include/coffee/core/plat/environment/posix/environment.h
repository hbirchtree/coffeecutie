#pragma once

#include "../../plat_primary_identify.h"

#if defined(COFFEE_UNIXPLAT) && !defined(COFFEE_WINDOWS) && !defined(COFFEE_ANDROID)

#include "../environment_details.h"
#include "../../memory/cmemory.h"

#include <libgen.h>
#include <unistd.h>
#include <stdio.h>
#include <cstdlib>
#include <limits.h>

extern char** environ;

namespace Coffee{
namespace Environment{
namespace Posix{

struct PosixEnvironmentFun : EnvInterface
{
    static CString BaseName(CString const& n);

    STATICINLINE bool ExistsVar(cstring var)
    {
        return std::getenv(var);
    }
    STATICINLINE CString GetVar(cstring var)
    {
        return std::getenv(var);
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
#if !defined(COFFEE_APPLE)
        return clearenv()==0;
#endif
        return false;
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

    static CString ApplicationDir();
    static CString CurrentDir();
    static Variables Environment();
};

struct PosixTerminalColorCodes : EnvColorCodes
{
    static constexpr cstring Reset = "\033[0m";

    /* Font color */

    /* Basics */
    static constexpr cstring FontBlack = "\033[1m\033[30m";
    static constexpr cstring FontWhite = "\033[1m\033[37m";

    /* Primaries */
    static constexpr cstring FontRed = "\033[0m\033[31m";
    static constexpr cstring FontBlue = "\033[1m\033[34m";
    static constexpr cstring FontGreen = "\033[0m\033[32m";
    static constexpr cstring FontYellow = "\033[1m\033[33m";

    /* Expanded */
    static constexpr cstring FontCyan = "\033[1m\033[36m";
    static constexpr cstring FontDeepViolet = "\033[1m\033[35m";
    static constexpr cstring FontBrown = "\033[0m\033[33m";
    static constexpr cstring FontGray = "\033[0m\033[30m";
    static constexpr cstring FontDarkGreen = "\033[1m\033[32m";
    static constexpr cstring FontLightGray = "\033[0m\033[37m";
    static constexpr cstring FontLightCyan = "\033[1m\033[36m";
    static constexpr cstring FontPink = "\033[0m\033[35m";

    /* Background color */

    /* Basics */
    static constexpr cstring BgBlack = "\033[1m\033[40m";
    static constexpr cstring BgWhite = "\033[1m\033[47m";

    /* Primaries */
    static constexpr cstring BgRed = "\033[0m\033[41m";
    static constexpr cstring BgBlue = "\033[1m\033[44m";
    static constexpr cstring BgGreen = "\033[0m\033[42m";
    static constexpr cstring BgDarkGreen = "\033[1m\033[42m";
    static constexpr cstring BgYellow = "\033[1m\033[43m";

    /* Expanded */
    static constexpr cstring BgCyan = "\033[1m\033[46m";
    static constexpr cstring BgDeepViolet = "\033[1m\033[45m";
    static constexpr cstring BgBrown = "\033[0m\033[43m";
    static constexpr cstring BgGray = "\033[0m\033[40m";
    static constexpr cstring BgLightGray = "\033[0m\033[47m";
    static constexpr cstring BgLightCyan = "\033[1m\033[46m";
    static constexpr cstring BgPink = "\033[1m\033[41m";
};

}
}

}

#endif
