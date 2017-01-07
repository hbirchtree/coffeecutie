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
    static CString GetUserData(cstring orgname, cstring appname);
};

struct MacColorMap : Posix::PosixTerminalColorCodes
{
};

}
}

using Env_ = Environment::Mac::MacEnv;
using ColorMap_ = Environment::Mac::MacColorMap;

}
#endif
