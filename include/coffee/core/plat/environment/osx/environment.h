#pragma once

#include "../../plat_primary_identify.h"

#if defined(COFFEE_APPLE) || 1

#include "../unix/environment.h"

#include <libproc.h>
#include <errno.h>

#include "../../memory/stlstring_ops.h"

namespace Coffee{
namespace Environment{
namespace Mac{

struct MacEnv : Posix::PosixEnvironmentFun
{
    static CString ExecutableName(cstring_w = nullptr);

    STATICINLINE CString GetUserData(cstring orgname, cstring appname)
    {
        CString dir = GetVar("HOME");
        dir = ConcatPath(dir.c_str(),"Library/Application Support");
        dir = ConcatPath(dir.c_str(),orgname);
        dir = ConcatPath(dir.c_str(),appname);
        return dir;
    }
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
