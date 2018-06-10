#pragma once

#include "../../plat_primary_identify.h"

#if defined(COFFEE_APPLE)

#include "../unix/environment.h"

#include "../../memory/stlstring_ops.h"

namespace Coffee{
namespace Environment{
namespace Mac{

struct MacEnv : Posix::PosixEnvironmentFun
{
    static CString ExecutableName(cstring_w = nullptr);

    STATICINLINE CString ApplicationDir()
    {
        CString execname = ExecutableName();
        return DirName(execname.c_str());
    }
    static Url GetUserData(cstring orgname, cstring appname);
};

#if defined(COFFEE_APPLE_DESKTOP)
struct MacColorMap : Posix::PosixTerminalColorCodes
{
};
#endif

}
}

using Env_ = Environment::Mac::MacEnv;
#if defined(COFFEE_APPLE_DESKTOP)
using ColorMap_ = Environment::Mac::MacColorMap;
#else
using ColorMap_ = Environment::EnvColorCodes;
#endif

}
#endif
