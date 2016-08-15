#pragma once

#include "../../plat_primary_identify.h"

#ifdef COFFEE_LINUX

#include "../unix/environment.h"

namespace Coffee{
namespace Environment{
namespace Linux{

struct LinuxEnv : Posix::PosixEnvironmentFun
{
    STATICINLINE CString ExecutableName(cstring_w = nullptr)
    {
        cstring_w p = realpath("/proc/self/exe",nullptr);
        CString v = p;
        CFree(p);
        return v;
    }
    STATICINLINE CString ApplicationDir()
    {
        CString execname = ExecutableName();
        return DirName(execname.c_str());
    }
    static CString GetUserData(cstring orgname, cstring appname);
};

}
}

using ColorMap_ = Environment::Posix::PosixTerminalColorCodes;
using Env_ = Environment::Linux::LinuxEnv;

}

#endif
