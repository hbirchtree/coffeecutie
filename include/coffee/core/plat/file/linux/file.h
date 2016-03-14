#include "../../platform_detect.h"

#ifdef COFFEE_LINUX
#ifndef COFFEE_CORE_PLAT_FILE_H
#define COFFEE_CORE_PLAT_FILE_H

#include "../unix/file.h"

namespace Coffee{
namespace CResources{

struct LinuxFileFun : PosixFileFun
{
    STATICINLINE uint32 PageSize()
    {
        return sysconf(_SC_PAGE_SIZE)-1;
    }

    STATICINLINE void* Map(cstring filename, ResourceAccess acc,
                           szptr offset, szptr size, int* error)
    {
        szptr pa_offset = offset & ~(PageSize());

        /*Translate access flags*/
        int oflags = PosixRscFlags(acc);
        int prot = ProtFlags(acc);
        int mapping = MappingFlags(acc);

        /*... and then actually open it*/
        int fd = open(filename,oflags);
        if(fd < 0)
        {
            *error = errno;
            return nullptr;
        }
        byte_t* addr = (byte_t*)mmap64(
                    NULL,
                    offset+size-pa_offset,
                    prot,mapping,
                    fd,pa_offset);
        if(addr == MAP_FAILED)
        {
            *error = errno;
            return nullptr;
        }
        return addr;
    }
    STATICINLINE bool  Unmap(void* ptr, szptr size)
    {
        return munmap(ptr,size);
    }
    STATICINLINE bool MapCache(void* t_ptr, szptr t_size, szptr r_off, szptr r_size)
    {
        if(r_off+r_size>t_size)
            return false;
        byte_t* base = (byte_t*)t_ptr;
        base += r_off;
        return mlock(base,r_size)==0;
    }
    STATICINLINE bool MapUncache(void* t_ptr, szptr t_size, szptr r_off, szptr r_size)
    {
        if(r_off+r_size>t_size)
            return false;
        byte_t* base = (byte_t*)t_ptr;
        base += r_off;
        return munlock(base,r_size)==0;
    }
    STATICINLINE bool MapSync(void* ptr, szptr size)
    {
        return msync(ptr,size,MS_SYNC|MS_INVALIDATE);
    }

    STATICINLINE bool SuperCache()
    {
        return mlockall(MCL_CURRENT)==0;
    }
    STATICINLINE bool SuperUncache()
    {
        return munlockall()==0;
    }

    STATICINLINE void* ScratchBuffer(szptr size, ResourceAccess access)
    {
        int proto = ProtFlags(access);
        int mapflags = MappingFlags(access)|MAP_ANONYMOUS;

        mapflags |= MAP_ANONYMOUS;

        return mmap64(nullptr,size,proto,mapflags,-1,0);
    }
    STATICINLINE void ScratchUnmap(void* ptr, szptr size)
    {
        munmap(ptr,size);
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

using FileFun = LinuxFileFun;
using DirFun = LinuxDirFun;

}
}

#endif
#endif