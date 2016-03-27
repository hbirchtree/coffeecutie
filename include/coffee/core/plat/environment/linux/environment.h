#pragma once

#include "../../plat_primary_identify.h"

#ifdef COFFEE_LINUX

#include "../posix/environment.h"

namespace Coffee{
namespace Environment{
namespace Linux{

struct LinuxEnvironmentFun : Posix::PosixEnvironmentFun
{
    STATICINLINE CString ExecutableName(cstring_w = nullptr)
    {
        cstring_w p = realpath("/proc/self/exe",nullptr);
        CString v = p;
        CFree(p);
        return v;
    }
};

}
}

using ColorMap = Environment::Posix::PosixTerminalColorCodes;
using Env = Environment::Linux::LinuxEnvironmentFun;

}

#endif