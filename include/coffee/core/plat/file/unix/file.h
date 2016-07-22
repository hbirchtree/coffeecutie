#pragma once

#include "../cfile.h"
#include "../../plat_primary_identify.h"

#if defined(COFFEE_APPLE)
// ???
#include <mach/vm_statistics.h>
#endif

// errno
#include <errno.h>
// ???
#include <unistd.h>
// open(), close(), read(), write(), creat()
#include <fcntl.h>
// opendir(), readdir()
#include <dirent.h>
#include <sys/types.h>
// lstat()
#include <sys/stat.h>
// mmap(), mmap64(), msync(), mlock(), munlock()
#include <sys/mman.h>

// basename()
#include <libgen.h>

namespace Coffee{
namespace CResources{
namespace Posix{

struct PosixApi
{
    struct FileHandle
    {
	FileHandle():
            fd(0)
        {
        }

        int fd;
    };
    using FileMapping = FileFunDef::FileMapping;
};

struct PosixFileMod_def : CommonFileFun
{
    STATICINLINE bool ErrnoCheck(cstring ref = nullptr)
    {
	if(errno!=0)
        {
            fprintf(stderr,"ERROR:%s: %s\n",ref,strerror(errno));
            errno = 0;
            return true;
        }
        return false;
    }

    static NodeType Stat(cstring fn);

    static bool Touch(NodeType t, cstring fn);

    STATICINLINE bool Rm(cstring fn)
    {
	bool stat = unlink(fn) == 0;
        ErrnoCheck(fn);
	return stat;
    }

    static szptr Size(cstring fn);

    static bool Exists(cstring fn);

protected:
    STATICINLINE uint32 PageSize()
    {
#if defined(COFFEE_LINUX) || defined(COFFEE_APPLE)
        return sysconf(_SC_PAGESIZE)-1;
#else
        return 8;
#endif
    }

    static int MappingFlags(ResourceAccess acc);
    static int ProtFlags(ResourceAccess acc);
    static int PosixRscFlags(ResourceAccess acc);
};

template<typename FH, typename FM,typename ScratchBuf>
struct PosixFileFun_def : PosixFileMod_def
{
    using PosixFileMod_def::Exists;
    using PosixFileMod_def::Size;

    STATICINLINE FH* Open(cstring fn, ResourceAccess ac)
    {
        int fd = open(fn,PosixRscFlags(ac),S_IRWXU|S_IRGRP);

        if(fd == -1)
            return nullptr;

        FH* fh = new FH();
        fh->fd = fd;

        return fh;
    }
    STATICINLINE bool Close(FH* fh)
    {
        close(fh->fd);
        delete fh;
        return true;
    }

    STATICINLINE CByteData Read(FH* f_h, uint64 f_size, bool)
    {
        CByteData data;

        szptr sz = Size(f_h);

        if(f_size <= sz && f_size != -1)
            sz = f_size;

        data.data = (byte_t*)Alloc(sz);
        data.size = sz;

        szptr i = 0;
        szptr chnk = 0;
        while(i<sz)
        {
            chnk = ((sz-i) < Int32_Max) ? (sz-i) : Int32_Max;
            i += read(f_h->fd,
                      &(((byte_t*)(data.data))[i]),
                      chnk);
            if(ErrnoCheck())
                break;
        }

        if(i < sz)
        {
            CFree(data.data);
            return {};
        }

	return data;
    }

    STATICINLINE bool Seek(FH*, uint64)
    {
	/* We won't use this, it's tedious */
	return false;
    }

    STATICINLINE bool Write(FH* f_h, CByteData const& d, bool)
    {
        szptr i = 0;
        szptr it = 0;
        szptr chnk = 0;
        while(i<d.size)
        {
            chnk = ((d.size-i) < Int32_Max) ? (d.size-i) : Int32_Max;
            i += write(f_h->fd,
                       &(((byte_t*)(d.data))[i]),
                       chnk);
            if(ErrnoCheck() && it != 0)
                break;
            it++;
        }
        return i == d.size;
    }

    STATICINLINE FM Map(cstring filename, ResourceAccess acc,
                    szptr offset, szptr size, int* error)
    {
        if(error)
            *error = 0;
        szptr pa_offset = offset & ~(PageSize());

        if(pa_offset!=offset)
            return {};

        /*Translate access flags*/
        int oflags = PosixRscFlags(acc);
        int prot = ProtFlags(acc);
        int mapping = MappingFlags(acc);

        /*... and then actually open it*/
        int fd = open(filename,oflags);
        if(fd < 0)
        {
            if(error)
                *error = errno;
            return {};
        }

        byte_t* addr = nullptr;

#if defined(COFFEE_LINUX)
        addr = (byte_t*)mmap64(
                    NULL,
                    offset+size-pa_offset,
                    prot,mapping,
                    fd,pa_offset);
#else
        addr = (byte_t*)mmap(
                    NULL,
                    offset+size-pa_offset,
                    prot,mapping,
                    fd,pa_offset);
#endif

        if(addr == MAP_FAILED)
        {
            *error = errno;
            return {};
        }

        FileMapping map;
        map.ptr = addr;
        map.size = size;
        map.acc = acc;

        return map;
    }

    STATICINLINE bool Unmap(FM* mapp)
    {
        void* ptr = mapp->ptr;
        szptr size = mapp->size;
        if(munmap(ptr,size)==0)
        {
            return true;
        }else
            return false;
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

    STATICINLINE ScratchBuf ScratchBuffer(szptr size, ResourceAccess access)
    {
        int proto = ProtFlags(access);
        int mapflags = MappingFlags(access);

#if defined(COFFEE_LINUX)
        mapflags |= MAP_ANONYMOUS;
#elif defined(COFFEE_APPLE)
        mapflags |= MAP_ANON;
#endif
        
        
        ScratchBuf buf;
#if defined(COFFEE_LINUX)
        // mmap64() is a Linux-specific feature it seems
        buf.ptr = mmap64(nullptr,size,proto,mapflags,-1,0);
#elif defined(COFFEE_APPLE)
        buf.ptr = mmap(nullptr,size,proto,mapflags,-1,0);
#endif
        buf.acc = access;
        buf.size = size;

        if(!buf.ptr)
            return {};

        return buf;
    }
    STATICINLINE void ScratchUnmap(ScratchBuf* buf)
    {
        munmap(buf->ptr,buf->size);
    }

    STATICINLINE szptr Size(FH* fh)
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
    STATICINLINE bool Exists(FH* fn)
    {
        struct stat st;
        bool status = fstat(fn->fd,&st)==0;
        errno = 0;
        if(status)
            return true;
        else
            return errno==ENOENT|| errno==ENOTDIR;
    }
};

struct PosixFileFun : PosixFileFun_def<
        PosixApi::FileHandle,
        PosixApi::FileMapping,
        CommonFileFun::ScratchBuf>
{
    using FileHandle = PosixApi::FileHandle;
    using FileMapping = PosixApi::FileMapping;
    using ScratchBuf = CommonFileFun::ScratchBuf;
};

struct PosixDirFun : DirFunDef
{
    static bool ChDir(cstring dir);

    static bool MkDir(cstring dname, bool createParent);

    STATICINLINE bool RmDir(cstring dname)
    {
        return rmdir(dname) == 0 || (errno = 0);
    }

    using Type = FileFunDef::NodeType;

    static bool Ls(cstring dname, DirList& entries);
};

}
}
}
