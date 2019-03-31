#pragma once

#include <peripherals/base.h>

#if defined(COFFEE_UNIXPLAT) || defined(COFFEE_EMSCRIPTEN) || defined(COFFEE_ANDROID)

#include <platforms/base/environment.h>

// getenv, setenv, unsetenv, clearenv
#include <stdlib.h>

extern "C" char** environ;

namespace platform {
namespace env {
namespace posix {

struct EnvironmentF : EnvInterface
{
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
        return setenv(var, val, 1) == 0;
    }

    static bool PrependVar(cstring var, cstring val);
    static bool AppendVar(cstring var, cstring val);

    STATICINLINE bool UnsetVar(cstring var)
    {
        return unsetenv(var) == 0;
    }
    STATICINLINE bool ClearEnv()
    {
#if !defined(COFFEE_APPLE)
        return clearenv() == 0;
#endif
        return false;
    }

    STATICINLINE CString GetPathSep()
    {
        return CString("/");
    }

    static CString ConcatPath(cstring v1, cstring v2);

    STATICINLINE Url GetUserHome()
    {
        return constructors::MkUrl(GetVar("HOME"), RSCA::SystemFile);
    }

    static Url       CurrentDir();
    static Variables Environment();
};

struct TerminalColorCodes : EnvColorCodes
{
    static CString& ColorText(CString& text, CmdFormat fmt);
};

} // namespace posix
} // namespace env

#if defined(COFFEE_EMSCRIPTEN)
using ColorMap_ = env::posix::TerminalColorCodes;
using Env_      = env::posix::EnvironmentF;
#endif

} // namespace platform

#endif
