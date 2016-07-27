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
        return DirName(ExecutableName());
    }
    STATICINLINE CString GetUserData(cstring orgname, cstring appname)
    {
        CString homedir = GetUserHome();
        homedir = ConcatPath(homedir.c_str(),".local/share");
        homedir = ConcatPath(homedir.c_str(),orgname);
        return ConcatPath(homedir.c_str(),appname);
    }
};

}
}

using ColorMap_ = Environment::Posix::PosixTerminalColorCodes;
using Env_ = Environment::Linux::LinuxEnv;

}

#endif
