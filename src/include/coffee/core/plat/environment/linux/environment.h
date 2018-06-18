#pragma once

#include "../../plat_primary_identify.h"

#ifdef COFFEE_LINUX

#include <coffee/core/types/tdef/integertypes.h>
#include <coffee/core/types/tdef/stltypes.h>
#include <coffee/core/plat/environment/unix/environment.h>

namespace Coffee{
namespace Environment{
namespace Linux{

struct LinuxEnv : Posix::PosixEnvironmentFun
{
    static CString ExecutableName(cstring_w = nullptr);
    static Url ApplicationDir();
    static Url GetUserData(cstring orgname, cstring appname);
};

}
}

using ColorMap_ = Environment::Posix::PosixTerminalColorCodes;
using Env_ = Environment::Linux::LinuxEnv;

}

#endif
