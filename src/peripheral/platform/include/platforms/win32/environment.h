#pragma once

#include <peripherals/base.h>

#ifdef COFFEE_WINDOWS

#include "../../file/windows/file.h"
#include "../environment_details.h"

namespace platform {
namespace env {
namespace win32 {

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
    STATICINLINE Url GetUserHome()
    {
        return MkUrl(GetVar("USERPROFILE"), RSCA::SystemFile);
    }

    static Url GetUserData(cstring org, cstring app);

    STATICINLINE Url ApplicationDir()
    {
        CResources::Windows::WinFileFun::file_error ec;
        CString                                     fn = ExecutableName();
        return DirFun::Dirname(fn.c_str(), ec);
    }

    static Url CurrentDir();
};
} // namespace win32
} // namespace env

using ColorMap_ = env::EnvColorCodes;
using Env_      = env::win32::WindowsEnvFun;

} // namespace platform

#endif
