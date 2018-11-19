#pragma once

#include "../../platform_detect.h"

#ifdef COFFEE_WINDOWS

#include "../environment_details.h"
#include "../../file/windows/file.h"

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
    STATICINLINE Url GetUserHome()
    {
        return MkUrl(GetVar("USERPROFILE"), RSCA::SystemFile);
    }

    static Url GetUserData(cstring org, cstring app);

    STATICINLINE Url ApplicationDir()
    {
        CResources::Windows::WinFileFun::file_error ec;
        CString fn = ExecutableName();
        return DirFun::Dirname(fn.c_str(), ec);
    }

    static Url CurrentDir();
};
}

}

using ColorMap_ = Environment::EnvColorCodes;
using Env_ = Environment::Windows::WindowsEnvFun;

}

#endif
