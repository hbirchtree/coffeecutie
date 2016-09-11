#include "../../platform_detect.h"

#ifdef COFFEE_LINUX
#ifndef COFFEE_CORE_PLAT_FILE_H
#define COFFEE_CORE_PLAT_FILE_H

#include "../unix/file.h"

namespace Coffee{
namespace CResources{
namespace Linux{
struct LinuxFileFun : Posix::PosixFileFun
{
    static CString NativePath(cstring fn);
    static CString NativePath(cstring fn,ResourceAccess storage);

    static bool VerifyAsset(cstring fn);

    static CString sys_read(cstring fn);

    STATICINLINE bool SuperCache()
    {
        return mlockall(MCL_CURRENT)==0;
    }
    STATICINLINE bool SuperUncache()
    {
        return munlockall()==0;
    }
};

struct LinuxDirFun : Posix::PosixDirFun
{

};
}

using FileFun = Linux::LinuxFileFun;
using DirFun = Linux::LinuxDirFun;

}
}

#endif
#endif
