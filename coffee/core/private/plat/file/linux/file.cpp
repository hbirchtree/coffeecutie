#include <coffee/core/plat/file/linux/file.h>
#include <coffee/core/plat/plat_environment.h>

#include "../file_abstraction.h"
#include <coffee/core/coffee_version.h>
#include <coffee/core/coffee.h>

#include <stdio.h>

namespace Coffee{
namespace CResources{
namespace Linux{

CString LinuxFileFun::sys_read(cstring fn, bool quiet)
{
#ifndef COFFEE_LOWFAT
    CString out;
    FILE* fh = fopen(fn,"r");
    char* arg = 0;
    size_t size = 0;

    if(!fh && !quiet)
    {
        ErrnoCheck(fn, -1);
        return out;
    }else
        errno = 0;

#if !defined(COFFEE_ANDROID)
    while(getdelim(&arg,&size,0,fh) != -1)
    {
        out.append(arg);
    }

    free(arg);
    if(out.size() > 0)
        out.resize(out.size()-1);
#else
    /* Source:
     * https://stackoverflow.com/questions/12237712/how-can-i-show-the-size-of-files-in-proc-it-should-not-be-size-zero */
    do {
        char linedata[256];
        memset(linedata, 0, sizeof(linedata));
        if(fgets(linedata, sizeof(linedata), fh) == nullptr)
            break;
        out.append(linedata);
    } while(!feof(fh));
#endif
    fclose(fh);
    return out;
#else
    return {};
#endif
}

}

}
}
