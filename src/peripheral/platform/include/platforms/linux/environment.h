#pragma once

#include <peripherals/base.h>

#ifdef COFFEE_LINUX

#include <peripherals/libc/types.h>
#include <peripherals/stl/types.h>
#include <platforms/posix/environment.h>

namespace platform {
namespace env {
namespace Linux {

struct EnvironmentF : posix::EnvironmentF
{
    static CString ExecutableName(cstring_w = nullptr);
    static Url     ApplicationDir();
    static Url     GetUserData(cstring orgname, cstring appname);
};

} // namespace Linux
} // namespace env

using ColorMap_ = env::posix::TerminalColorCodes;
using Env_      = env::Linux::EnvironmentF;

} // namespace platform

#endif
