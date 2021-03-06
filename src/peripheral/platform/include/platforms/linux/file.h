#pragma once

#include <peripherals/base.h>

#if defined(COFFEE_LINUX) || defined(COFFEE_ANDROID)
#include <platforms/posix/file.h>

namespace platform {
namespace file {
namespace Linux {

struct FileFun : posix::FileFun
{
    STATICINLINE CString sys_read(cstring fn)
    {
        file_error ec;
        return sys_read(fn, ec);
    }

    static CString sys_read(cstring fn, file_error& ec);

#if !defined(COFFEE_ANDROID)
    STATICINLINE bool SuperCache()
    {
        return mlockall(MCL_CURRENT) == 0;
    }
    STATICINLINE bool SuperUncache()
    {
        return munlockall() == 0;
    }
#endif
};

} // namespace Linux

#if !defined(COFFEE_ANDROID)
using FileFun = Linux::FileFun;
using DirFun  = posix::DirFun;
#endif

} // namespace file
} // namespace platform

#endif
