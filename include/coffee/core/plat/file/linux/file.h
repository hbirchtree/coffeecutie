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
    STATICINLINE bool SuperCache()
    {
        return mlockall(MCL_CURRENT)==0;
    }
    STATICINLINE bool SuperUncache()
    {
        return munlockall()==0;
    }
};

struct LinuxDirFun : DirFunDef
{
    STATICINLINE bool MkDir(cstring dname, bool createParent)
    {
        if(!createParent)
            return mkdir(dname,S_IRWXU|S_IRWXG)==0;

        char tmp[256];
        char *p = NULL;
        size_t len;

        snprintf(tmp,sizeof(tmp),"%s",dname);
        len = strlen(tmp);
        if(tmp[len-1] == '/')
            tmp[len-1] = 0;
        for(p = tmp+1; *p;p++)
            if(*p == '/')
            {
                *p = 0;
                mkdir(tmp,S_IRWXU);
                *p = '/';
            }
        return mkdir(tmp,S_IRWXU)==0;
    }
    STATICINLINE bool RmDir(cstring)
    {
        /*TODO: Implement RmDir function*/
        return false;
    }
};
}

using FileFun = Linux::LinuxFileFun;
using DirFun = Linux::LinuxDirFun;

}
}

#endif
#endif
