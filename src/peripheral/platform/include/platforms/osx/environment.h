#pragma once

#include <peripherals/base.h>

#if defined(COFFEE_APPLE)

#include <platforms/posix/environment.h>
#include <platforms/posix/file.h>

#include "../../memory/stlstring_ops.h"

namespace platform {
namespace env {
namespace mac {

struct MacEnv : Posix::PosixEnvironmentFun
{
    static CString ExecutableName(cstring_w = nullptr);

    STATICINLINE Url ApplicationDir()
    {
        CResources::Posix::PosixDirFun::file_error ec;
        CString                                    execname = ExecutableName();
        return CResources::Posix::PosixDirFun::Dirname(execname.c_str(), ec);
    }
    static Url GetUserData(cstring orgname, cstring appname);
};

#if defined(COFFEE_APPLE_DESKTOP)
struct MacColorMap : Posix::PosixTerminalColorCodes
{
};
#endif

} // namespace mac
} // namespace env

using Env_ = Environment::Mac::MacEnv;
#if defined(COFFEE_APPLE_DESKTOP)
using ColorMap_ = Environment::Mac::MacColorMap;
#else
using ColorMap_ = Environment::EnvColorCodes;
#endif

} // namespace platform
#endif
