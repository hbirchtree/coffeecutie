#pragma once

#include <peripherals/base.h>

#if defined(COFFEE_APPLE)

#include <platforms/posix/environment.h>
#include <platforms/posix/file.h>

#include <peripherals/stl/stlstring_ops.h>

namespace platform {
namespace env {
namespace mac {

struct EnvironmentF : posix::EnvironmentF
{
    static CString ExecutableName(cstring_w = nullptr);

    STATICINLINE Url ApplicationDir()
    {
        file::posix::FileFun::file_error ec;

        CString execname = ExecutableName();
        return file::posix::DirFun::Dirname(execname.c_str(), ec);
    }
    static Url GetUserData(cstring orgname, cstring appname);
};

} // namespace mac
} // namespace env

using Env_      = env::mac::EnvironmentF;
using ColorMap_ = env::EnvColorCodes;

} // namespace platform
#endif
