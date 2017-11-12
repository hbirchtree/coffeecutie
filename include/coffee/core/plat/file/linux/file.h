#pragma once

#include "../../platform_detect.h"

#if defined(COFFEE_LINUX) || defined(COFFEE_ANDROID)
#include "../unix/file.h"

namespace Coffee{
namespace CResources{
namespace Linux{
struct LinuxFileFun : Posix::PosixFileFun
{

    static CString sys_read(cstring fn);
#if !defined(COFFEE_ANDROID)
    static CString NativePath(cstring fn);
    static CString NativePath(cstring fn,ResourceAccess storage);

    static bool VerifyAsset(cstring fn);


    STATICINLINE bool SuperCache()
    {
        return mlockall(MCL_CURRENT)==0;
    }
    STATICINLINE bool SuperUncache()
    {
        return munlockall()==0;
    }
#endif
};

struct LinuxDirFun : Posix::PosixDirFun
{

};

}

#if !defined(COFFEE_ANDROID)
using FileFun = Linux::LinuxFileFun;
using DirFun = Linux::LinuxDirFun;
#endif

}
}

#endif
