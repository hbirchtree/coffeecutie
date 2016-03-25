#pragma once

#include "../cfile.h"

#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>

namespace Coffee{
namespace CResources{

struct PosixApi
{
    struct FileHandle
    {
        FileHandle():
            handle(nullptr),
            fd(0)
        {
        }

        FILE* handle;
        int fd;
    };
    using FileMapping = FileFunDef::FileMapping;
};

struct PosixFileFun : CFILEFun_def<PosixApi::FileHandle>
{
    STATICINLINE void ErrnoCheck()
    {
        if(errno!=0)
            fprintf(stderr,"ERROR: %s\n",strerror(errno));
    }

    using FileHandle = PosixApi::FileHandle;
    using FileMapping = PosixApi::FileMapping;

    STATICINLINE FileHandle* Open(cstring fn, ResourceAccess ac)
    {
        FileHandle* fh = CFILEFun_def<FileHandle>::Open(fn,ac);
        if(!fh)
            return nullptr;

        fh->fd = open(fn,PosixRscFlags(ac));

        return fh;
    }
    STATICINLINE bool Close(FileHandle* fh)
    {
        close(fh->fd);
        CFILEFun_def<FileHandle>::Close(fh);
        return true;
    }

    STATICINLINE szptr Size(cstring fn)
    {
        struct stat st = {};
        if(stat(fn,&st)!=0)
            ErrnoCheck();
        errno = 0;
        return st.st_size;
    }

    STATICINLINE bool Exists(cstring fn)
    {
        struct stat st;
        bool status = stat(fn,&st)==0;
        errno = 0;
        if(status)
            return true;
        else
            return errno!=ENOENT||errno==ENOTDIR;
    }

    STATICINLINE szptr Size(FileHandle* fh)
    {
        struct stat st;
        if(fh->fd>0)
        {
            if(fstat(fh->fd,&st)!=0)
                ErrnoCheck();
            errno = 0;
            return st.st_size;
        }else{
            /* We somehow want a filename here. */
            return 0;
        }
    }
    STATICINLINE bool Exists(FileHandle* fn)
    {
        struct stat st;
        bool status = fstat(fn->fd,&st)==0;
        errno = 0;
        if(status)
            return true;
        else
            return errno==ENOENT|| errno==ENOTDIR;
    }

protected:
    STATICINLINE int MappingFlags(ResourceAccess acc)
    {
        int mapping = 0;

        if(feval(acc&(ResourceAccess::Persistent)))
            mapping = MAP_SHARED;
        else
            mapping = MAP_PRIVATE;

        if(feval(acc&ResourceAccess::HugeFile))
            mapping |= MAP_HUGETLB;

        if(feval(acc&ResourceAccess::ExclusiveLocking))
            mapping |= MAP_LOCKED;

        if(feval(acc&(ResourceAccess::Streaming)))
            mapping |= MAP_POPULATE;
        if(feval(acc&ResourceAccess::GreedyCache))
            mapping |= MAP_POPULATE|MAP_LOCKED;
        if(feval(acc&ResourceAccess::NoCache))
            mapping |= MAP_NONBLOCK;

        return mapping;
    }
    STATICINLINE int ProtFlags(ResourceAccess acc)
    {
        int prot = PROT_NONE;

        if(feval(acc&(ResourceAccess::ReadOnly)))
            prot |= PROT_READ;
        if(feval(acc&ResourceAccess::WriteOnly))
            prot |= PROT_WRITE;
        if(feval(acc&ResourceAccess::Executable))
            prot |= PROT_EXEC;

        return prot;
    }
    STATICINLINE int PosixRscFlags(ResourceAccess acc)
    {
        int oflags = 0;

        if(feval(acc&(ResourceAccess::ReadWrite)))
        {
            oflags = O_RDWR;

            if(feval(acc&ResourceAccess::Discard))
                oflags |= O_TRUNC;
        }
        else if(feval(acc&ResourceAccess::Executable))
            oflags = O_RDONLY;
        else if(feval(acc&(ResourceAccess::ReadOnly)))
            oflags = O_RDONLY;
        else if(feval(acc&(ResourceAccess::WriteOnly))||feval(acc&ResourceAccess::Append))
        {
            if(feval(acc&ResourceAccess::Append))
                oflags = O_APPEND;
            else
                oflags = O_WRONLY;

            if(feval(acc&ResourceAccess::Discard))
                oflags |= O_TRUNC;
        }

        return oflags;
    }
};

}
}