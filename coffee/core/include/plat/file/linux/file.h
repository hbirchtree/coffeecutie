#include <plat/platform_detect.h>

#ifdef COFFEE_LINUX
#ifndef COFFEE_CORE_PLAT_FILE_H
#define COFFEE_CORE_PLAT_FILE_H

#include "../cfile.h"
#include <coffee/core/CTypes>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>

namespace Coffee{
namespace CResources{

struct LinuxFileFun : PlatFileFun
{
    static void* Map(cstring filename, ResourceAccess acc, szptr offset, szptr size, int* error)
    {
        szptr pa_offset = offset & ~(sysconf(_SC_PAGE_SIZE)-1);

        int oflags = 0;

        if(feval(acc&(ResourceAccess::ReadWrite)))
            oflags = O_RDWR;
        else if(feval(acc&(ResourceAccess::ReadOnly)))
            oflags = O_RDONLY;
        else if(feval(acc&(ResourceAccess::WriteOnly)))
            oflags = O_WRONLY;

        int fd = open(filename,oflags);
        if(fd < 0)
        {
            *error = errno;
            return nullptr;
        }
        byte_t* addr = (byte_t*)mmap(
                    NULL,
                    offset+size-pa_offset,
                    PROT_READ,MAP_PRIVATE,
                    fd,pa_offset);
        if(addr == MAP_FAILED)
        {
            *error = errno;
            return nullptr;
        }
        return addr;
    }
    static bool  Unmap(void* ptr, szptr size)
    {
        return munmap(ptr,size);
    }
};

struct LinuxDirFun : DirFunDef
{
    static bool MkDir(cstring dname, bool createParent)
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
    static bool RmDir(cstring)
    {
        /*TODO: Implement RmDir function*/
        return false;
    }
};

using FileFun = LinuxFileFun;
using DirFun = LinuxDirFun;

}
}

#endif
#endif