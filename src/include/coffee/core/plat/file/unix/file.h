#pragma once

#include "../../plat_primary_identify.h"
#include "../cfile.h"

#if defined(COFFEE_APPLE)
// ???
#include <mach/vm_statistics.h>
#endif

#include <dirent.h>   // opendir(), readdir()
#include <errno.h>    // errno
#include <fcntl.h>    // open(), close(), read(), write(), creat()
#include <libgen.h>   // basename()
#include <sys/mman.h> // mmap(), mmap64(), msync(), mlock(), munlock()
#include <sys/stat.h> // lstat()
#include <sys/types.h>
#include <unistd.h> // ???

namespace Coffee {
namespace CResources {
namespace Posix {

struct posix_fd
{
    int fd;

    FORCEDINLINE posix_fd(int fd) : fd(fd)
    {
    }

    FORCEDINLINE posix_fd(posix_fd&& fd) : fd(fd.fd)
    {
        fd.clear();
    }

    /*!
     * \brief Transfer ownership of fd
     */
    FORCEDINLINE void clear()
    {
        fd = 0;
    }

    FORCEDINLINE posix_fd& operator=(posix_fd&& fd)
    {
        this->fd = fd.fd;
        fd.clear();

        return *this;
    }

    /*!
     * \brief If still contained, close the fd
     */
    FORCEDINLINE ~posix_fd()
    {
        if(fd > 0)
            close(fd);
    }

    FORCEDINLINE operator int() const
    {
        return fd;
    }
};

struct posix_error_category : error_category
{
    virtual const char* name() const noexcept;
    virtual std::string message(int error_code) const;
};

using posix_error_code = domain_error_code<int, posix_error_category>;

struct PosixApi
{
    struct FileHandle
    {
        FileHandle() : fd(0)
        {
        }

        posix_fd fd;
    };

    using FileMapping = FileFunDef<>::FileMapping;
};

using PosixCommonFileFun = CommonFileFun<posix_error_code>;

struct PosixFileMod_def : PosixCommonFileFun
{
    static bool ErrnoCheck(file_error& ec, cstring ref = nullptr, int fd = -1);

    static NodeType Stat(Url const& fn, file_error&);

    static bool Touch(NodeType t, Url const& fn, file_error& ec);

    static bool Rm(Url const& fn, file_error& ec);

    static CString DereferenceLink(Url const& fn, file_error& ec);

    static CString CanonicalName(Url const& fn, file_error& ec);

    static bool Ln(Url const& src, Url const& target, file_error& ec);

    static szptr Size(Url const& fn, file_error& ec);

    static bool Exists(Url const& fn, file_error&);

    static void Truncate(Url const& fn, szptr size, file_error& ec);

  protected:
    static uint32 PageSize();

    static int MappingFlags(RSCA acc);
    static int ProtFlags(RSCA acc);
    static int PosixRscFlags(RSCA acc);
};

template<
    typename FH,
    typename FM,
    typename ScratchBuf,
    typename implements<PosixApi::FileHandle, FH>::type*  = nullptr,
    typename implements<PosixApi::FileMapping, FM>::type* = nullptr,
    typename implements<PosixCommonFileFun::ScratchBuf, ScratchBuf>::type* =
        nullptr>
struct PosixFileFun_def : PosixFileMod_def
{
    using PosixFileMod_def::Exists;
    using PosixFileMod_def::Size;

    STATICINLINE FH Open(Url const& fn, RSCA ac, file_error& ec)
    {
        errno = 0;

        auto url = *fn;
        auto fd =
            posix_fd(open(url.c_str(), PosixRscFlags(ac), S_IRWXU | S_IRGRP));

        if(fd == -1)
        {
            ErrnoCheck(ec, url.c_str());
            return {};
        }

        FH fh;
        fh.fd = std::move(fd);

        fd.clear();

        return fh;
    }
    STATICINLINE bool Valid(FH const& fh, file_error&)
    {
        return C_OCAST<int>(fh.fd) != 0;
    }
    STATICINLINE bool Close(FH&&, file_error& ec)
    {
        return true;
    }

    STATICINLINE Bytes Read(FH const& f_h, int64 f_size, file_error& ec)
    {
        Bytes data = {};
        errno      = 0;

        int64 sz = Size(f_h, ec);

        if(f_size <= sz && f_size != -1)
            sz = f_size;

        if(sz == 0)
            return data;

        szptr szp = C_CAST<szptr>(sz);

        data = Bytes::Alloc(szp);

        if(!data.data)
            return {};

        szptr i    = 0;
        szptr chnk = 0;
        while(i < szp)
        {
            chnk = ((szp - i) < Int32_Max) ? (szp - i) : Int32_Max;
            i += read(f_h.fd, &((C_CAST<byte_t*>(data.data))[i]), chnk);
            if(ErrnoCheck(ec))
                break;
        }

        if(i < szp)
        {
            return {};
        }

        return data;
    }

    STATICINLINE bool Write(FH const& f_h, Bytes const& d, file_error& ec)
    {
        szptr i    = 0;
        szptr it   = 0;
        szptr chnk = 0;
        errno      = 0;
        while(i < d.size)
        {
            chnk = ((d.size - i) < Int32_Max) ? (d.size - i) : Int32_Max;
            i += write(f_h.fd, &(C_CAST<byte_t*>(d.data)[i]), chnk);
            if(ErrnoCheck(ec, nullptr, f_h.fd) && it != 0)
                break;
            it++;
        }
        return i == d.size;
    }

    STATICINLINE
    FM Map(
        Url const& filename, RSCA acc, szptr offset, szptr size, file_error& ec)
    {
        auto   url       = *filename;
        uint64 pa_offset = offset & ~(PageSize());
        errno            = 0;

        if(pa_offset != offset)
            return {};

        /*Translate access flags*/
        int oflags  = PosixRscFlags(acc);
        int prot    = ProtFlags(acc);
        int mapping = MappingFlags(acc);

        /*... and then actually open it*/
        int fd = open(url.c_str(), oflags);
        if(fd < 0)
        {
            ErrnoCheck(ec);
            return {};
        }

        byte_t* addr = nullptr;

#if defined(COFFEE_LINUX) && !defined(COFFEE_NO_MMAP64)
        addr = static_cast<byte_t*>(mmap64(
            nullptr,
            offset + size - pa_offset,
            prot,
            mapping,
            fd,
            C_FCAST<i64>(pa_offset)));
#else
        addr = static_cast<byte_t*>(mmap(
            NULL, offset + size - pa_offset, prot, mapping, fd, pa_offset));
#endif

        if(!addr || addr == MAP_FAILED)
        {
            ErrnoCheck(ec, url.c_str(), fd);
            return {};
        }

        if(::close(fd) != 0)
            ErrnoCheck(ec);

        FM out(addr, size, size);
        out.assignAccess(acc);

        /* Set up some semantics to automatically unmap it
         *  when out of scope */
        FM::SetDestr(out, [](FM& f) {
            file_error ec;
            Unmap(std::move(f), ec);
        });

        return out;
    }

    STATICINLINE bool Unmap(FM&& mapp, file_error& ec)
    {
        void* ptr  = mapp.data;
        szptr size = mapp.size;
        errno      = 0;
        if(munmap(ptr, size) == 0)
        {
            return true;
        } else
        {
            ErrnoCheck(ec, "munmap()");
            return false;
        }
    }

    STATICINLINE bool MapCache(
        void* t_ptr, szptr t_size, szptr r_off, szptr r_size, file_error& ec)
    {
        if(r_off + r_size > t_size)
            return false;
        byte_t* base = static_cast<byte_t*>(t_ptr);
        errno        = 0;
        base += r_off;
        return mlock(base, r_size) == 0;
    }

    STATICINLINE bool MapUncache(
        void* t_ptr, szptr t_size, szptr r_off, szptr r_size, file_error& ec)
    {
        if(r_off + r_size > t_size)
            return false;
        byte_t* base = C_RCAST<byte_t*>(t_ptr);
        errno        = 0;
        base += r_off;
        return munlock(base, r_size) == 0;
    }

    STATICINLINE bool MapSync(void* ptr, szptr size, file_error& ec)
    {
        errno    = 0;
        auto ret = msync(ptr, size, MS_SYNC | MS_INVALIDATE);

        ErrnoCheck(ec);

        return ret;
    }

    STATICINLINE ScratchBuf
                 ScratchBuffer(szptr size, RSCA access, file_error& ec)
    {
        int proto    = ProtFlags(access);
        int mapflags = MappingFlags(access);
        errno        = 0;

#if defined(COFFEE_LINUX)
        mapflags |= MAP_ANONYMOUS;
#elif defined(COFFEE_APPLE)
        mapflags |= MAP_ANON;
#endif

        ScratchBuf buf;
#if defined(COFFEE_LINUX) && !defined(COFFEE_NO_MMAP64)
        // mmap64() is a Linux-specific feature it seems
        buf.data =
            C_RCAST<byte_t*>(mmap64(nullptr, size, proto, mapflags, -1, 0));
#elif defined(COFFEE_APPLE)
        buf.data =
            C_RCAST<byte_t*>(mmap(nullptr, size, proto, mapflags, -1, 0));
#endif
        buf.assignAccess(access);
        buf.size = buf.elements = size;

        if(!buf.data || buf.data == MAP_FAILED)
        {
            ec = FileError::MappingFailed;
            return {};
        }

        return buf;
    }
    STATICINLINE void ScratchUnmap(ScratchBuf&& buf, file_error& ec)
    {
        errno = 0;
        munmap(buf.data, buf.size);

        ErrnoCheck(ec);
    }

    STATICINLINE szptr Size(FH const& fh, file_error& ec)
    {
        errno = 0;

        struct stat st;
        if(fh.fd)
        {
            if(fstat(fh.fd, &st) != 0)
                ErrnoCheck(ec);

            return C_FCAST<szptr>(st.st_size);
        } else
        {
            /* We somehow want a filename here. */
            return 0;
        }
    }

    STATICINLINE bool Exists(FH const& fn, file_error&)
    {
        errno = 0;

        struct stat st;
        bool        status = fstat(fn->fd, &st) == 0;

        if(status)
            return true;
        else
            return errno == ENOENT || errno == ENOTDIR || (errno = 0);
    }
};

struct PosixFileFun : PosixFileFun_def<
                          PosixApi::FileHandle,
                          PosixApi::FileMapping,
                          PosixCommonFileFun::ScratchBuf>
{
    using FileHandle  = PosixApi::FileHandle;
    using FileMapping = PosixApi::FileMapping;
    using ScratchBuf  = CommonFileFun::ScratchBuf;
};

struct PosixDirFun : DirFunDef<posix_error_code>
{
    using Type = FileFunDef<>::NodeType;

    static bool ChDir(Url const& dir, file_error& ec);

    static bool MkDir(Url const& dname, bool createParent, file_error& ec);

    static bool RmDir(Url const& dname, file_error& ec);

    static bool Ls(Url const& dname, DirList& entries, file_error& ec);

    static Url Basename(CString const& n, file_error& ec);

    static Url Dirname(CString const& fname, file_error& ec);

    STATICINLINE CString ConcatPath(cstring p1, cstring p2, file_error&)
    {
        if(p1)
            return CString(p1) + GetPathSep() + p2;
        else if(p2)
            return p2;
        else
            return ".";
    }
};

} // namespace Posix
} // namespace CResources
} // namespace Coffee
