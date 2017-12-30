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
    static bool ErrnoCheck(cstring ref = nullptr, int fd = -1);

    static NodeType Stat(Url const& fn);

    static bool Touch(NodeType t, Url const& fn);

    static bool Rm(Url const& fn);

    static CString DereferenceLink(Url const& fn);

    static CString CanonicalName(Url const& fn);

    static bool Ln(Url const& src, Url const& target);

    static szptr Size(Url const& fn);

    static bool Exists(Url const& fn);

protected:
    static uint32 PageSize();

    static int MappingFlags(ResourceAccess acc);
    static int ProtFlags(ResourceAccess acc);
    static int PosixRscFlags(ResourceAccess acc);
};

template<typename FH, typename FM,typename ScratchBuf>
struct PosixFileFun_def : PosixFileMod_def
{
    using PosixFileMod_def::Exists;
    using PosixFileMod_def::Size;

    STATICINLINE FH* Open(Url const& fn, ResourceAccess ac)
    {
        auto url = *fn;
        int fd = open(url.c_str(),PosixRscFlags(ac),S_IRWXU|S_IRGRP);

        if(fd == -1)
            return nullptr;

        FH* fh = new FH();
        fh->fd = fd;

        return fh;
    }
    STATICINLINE bool Close(FH* fh)
    {
        if(!fh)
            return false;
        close(fh->fd);
        delete fh;
        return true;
    }

    STATICINLINE CByteData Read(FH* f_h, int64 f_size, bool nullterm)
    {
        CByteData data;

        int64 sz = Size(f_h);

        if(f_size <= sz && f_size != -1)
            sz = f_size;

        szptr szp = C_CAST<szptr>(sz) + ((nullterm) ? 1 : 0);

        data.data = C_CAST<byte_t*>(Calloc(1,szp));
        data.size = szp + nullterm;

        if(nullterm)
            szp --;

        if(!data.data)
            return {};

        szptr i = 0;
        szptr chnk = 0;
        while(i < szp)
        {
            chnk = ((szp-i) < Int32_Max) ? (szp-i) : Int32_Max;
            i += read(f_h->fd,
                      &((C_CAST<byte_t*>(data.data))[i]),
                      chnk);
            if(ErrnoCheck())
                break;
        }

        if(i < szp)
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
        if(!f_h)
            return false;

        szptr i = 0;
        szptr it = 0;
        szptr chnk = 0;
        while(i<d.size)
        {
            chnk = ((d.size-i) < Int32_Max) ? (d.size-i) : Int32_Max;
            i += write(f_h->fd,
                       &(C_CAST<byte_t*>(d.data)[i]),
                       chnk);
            if(ErrnoCheck(nullptr, f_h->fd) && it != 0)
                break;
            it++;
        }
        return i == d.size;
    }

    STATICINLINE FM Map(Url const& filename, ResourceAccess acc,
                    szptr offset, szptr size, int* error)
    {
        auto url = *filename;
        if(error)
            *error = 0;
        uint64 pa_offset = offset & ~(PageSize());

        if(pa_offset!=offset)
            return {};

        /*Translate access flags*/
        int oflags = PosixRscFlags(acc);
        int prot = ProtFlags(acc);
        int mapping = MappingFlags(acc);

        /*... and then actually open it*/
        int fd = open(url.c_str(),oflags);
        if(fd < 0)
        {
            if(error)
                *error = errno;
            return {};
        }

        byte_t* addr = nullptr;

#if defined(COFFEE_LINUX)
        addr = static_cast<byte_t*>(mmap64(
                    NULL,
                    offset+size-pa_offset,
                    prot,mapping,
                    fd,pa_offset));
#else
        addr = static_cast<byte_t*>(mmap(
                    NULL,
                    offset+size-pa_offset,
                    prot,mapping,
                    fd,pa_offset));
#endif

        if(addr == MAP_FAILED)
        {
            *error = errno;
            return {};
        }

        if(::close(fd) != 0)
            ErrnoCheck();

        return {addr,size,acc};
    }

    STATICINLINE bool Unmap(FM* mapp)
    {
        void* ptr = mapp->ptr;
        szptr size = mapp->size;
        if(munmap(ptr,size)==0)
        {
            return true;
        }else
        {
            ErrnoCheck("munmap()");
            return false;
        }
    }
    STATICINLINE bool MapCache(void* t_ptr, szptr t_size, szptr r_off, szptr r_size)
    {
        if(r_off+r_size>t_size)
            return false;
        byte_t* base = static_cast<byte_t*>(t_ptr);
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

        if(!buf.ptr || buf.ptr == (void*)0xFFFFFFFF)
            return {};

        return buf;
    }
    STATICINLINE void ScratchUnmap(ScratchBuf* buf)
    {
        munmap(buf->ptr,buf->size);
    }

    STATICINLINE szptr Size(FH* fh)
    {
        if(!fh)
            return 0;

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
        if(status)
            return true;
        else
            return errno==ENOENT|| errno==ENOTDIR || (errno = 0);
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
    static bool ChDir(Url const& dir);

    static bool MkDir(Url const& dname, bool createParent);

    static bool RmDir(Url const& dname);

    using Type = FileFunDef::NodeType;

    static bool Ls(Url const& dname, DirList& entries);
};

}
}
}
