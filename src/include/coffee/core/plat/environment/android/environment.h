#pragma once

#include "../../plat_primary_identify.h"

#ifdef COFFEE_ANDROID
#include "../unix/environment.h"

namespace Coffee{
namespace Environment{
namespace Android{

struct AndroidEnv : Posix::PosixEnvironmentFun
{
    static Url GetUserData(cstring,cstring);

    STATICINLINE CString GetPathSep()
    {
        return "/";
    }
    STATICINLINE CString ConcatPath(cstring v1, cstring v2)
    {
        return v1+GetPathSep()+v2;
    }
};

}
}

using ColorMap_ = Environment::EnvColorCodes;
using Env_ = Environment::Android::AndroidEnv;

}
#endif
