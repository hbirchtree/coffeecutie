#pragma once

#include <peripherals/base.h>

#if defined(COFFEE_UNIXPLAT) || defined(COFFEE_EMSCRIPTEN)

#include "../../memory/cmemory.h"
#include "../environment_details.h"

// getenv, setenv, unsetenv, clearenv
#include <stdlib.h>

extern "C" char** environ;

namespace Coffee {
namespace Environment {
namespace Posix {

struct PosixEnvironmentFun : EnvInterface
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
        return MkUrl(GetVar("HOME"), RSCA::SystemFile);
    }

    static Url       CurrentDir();
    static Variables Environment();
};

struct PosixTerminalColorCodes : EnvColorCodes
{
    // Because Android doesn't have a proper terminal

    static CString& ColorText(CString& text, CmdFormat fmt);
};

} // namespace Posix
} // namespace Environment

#if defined(COFFEE_EMSCRIPTEN)
using ColorMap_ = Environment::Posix::PosixTerminalColorCodes;
using Env_      = Environment::Posix::PosixEnvironmentFun;
#endif

} // namespace Coffee

#endif
