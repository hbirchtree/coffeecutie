#pragma once

#include <peripherals/base.h>

#ifdef COFFEE_LINUX

#include <coffee/core/plat/environment/unix/environment.h>
#include <peripherals/libc/types.h>
#include <peripherals/stl/types.h>

namespace Coffee {
namespace Environment {
namespace Linux {

struct LinuxEnv : Posix::PosixEnvironmentFun
{
    static CString ExecutableName(cstring_w = nullptr);
    static Url     ApplicationDir();
    static Url     GetUserData(cstring orgname, cstring appname);
};

} // namespace Linux
} // namespace Environment

using ColorMap_ = Environment::Posix::PosixTerminalColorCodes;
using Env_      = Environment::Linux::LinuxEnv;

} // namespace Coffee

#endif
