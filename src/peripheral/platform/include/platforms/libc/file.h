#pragma once

#include <cstdio>

#include <peripherals/error/posix.h>
#include <platforms/base/file.h>

namespace platform {
namespace file {

template<typename NestedError = sentinel_error_code>
struct CommonFileFun : FileFunDef<NestedError>
{
};

using FILE_error_code = posix_error_code;

struct FILEApi
{
    struct FileHandle : non_copy
    {
        FileHandle() : handle(nullptr)
        {
        }
        FileHandle(FileHandle&& f) : handle(f.handle)
        {
            f.handle = nullptr;
        }
        ~FileHandle()
        {
            if(handle)
                fclose(handle);
        }

        FileHandle& operator=(FileHandle&& other)
        {
            this->handle = other.handle;
            other.handle = nullptr;
            return *this;
        }

        FILE* handle;
    };
};

template<
    typename NestedError,
    typename FH,
    typename implements<FILEApi::FileHandle, FH>::type* = nullptr>
struct CFILEFunBase_def : CommonFileFun<NestedError>
{
    using file_error = typename CommonFileFun<NestedError>::file_error;
    using FileHandle = FH;

    using FileMappingImpl = FileMappingDefault<
        CFILEFunBase_def<NestedError, FH>,
        typename CommonFileFun<NestedError>::file_error>;

    using FileMapping = typename FileMappingImpl::FileMapping;

    STATICINLINE FH Open(Url const& fn, RSCA ac, file_error& ec)
    {
        using namespace enum_helpers;

        auto url = *fn;
        FH   fh  = {};

        cstring mode = nullptr;

        /* Because of the way masks work, feval must be run on each individual
         * flag */
        if(feval(ac & RSCA::ReadWrite) && feval(ac & RSCA::Append))
            mode = "ab+";
        else if(feval(ac & RSCA::WriteOnly) && feval(ac & RSCA::Append))
            mode = "ab";
        else if(feval(ac & (RSCA::ReadOnly)))
            mode = "rb";
        else if(feval(ac & RSCA::WriteOnly) && feval(ac & RSCA::Discard))
            mode = "wb+";
        else if(feval(ac & (RSCA::WriteOnly)))
            mode = "wb";
        else if(feval(ac & (RSCA::ReadWrite)))
            mode = "rb+";

        fh.handle = fopen(url.c_str(), mode);

        if(!fh.handle)
        {
            ec = FileError::NotFound;
            posix::collect_error_to(ec);
            return {};
        }

        return fh;
    }
    STATICINLINE bool Valid(FH const& fh, file_error& ec)
    {
        if(!fh.handle)
        {
            ec = FileError::InvalidHandle;
            return false;
        }
        return true;
    }
    STATICINLINE bool Close(FH&& fh, file_error& ec)
    {
        if(!Valid(fh, ec))
            return false;

        auto stat = fclose(fh.handle);

        if(stat != 0)
        {
            ec = FileError::SystemError;
            posix::collect_error_to(ec);
        }

        return stat == 0;
    }

    STATICINLINE Bytes Read(FH const& fh, u64 size, file_error& ec)
    {
        if(!Valid(fh, ec))
            return {};

        Bytes data;
        data.elements = 0;
        szptr esize   = Size(fh, ec);
        if(size <= esize && size != -1)
            esize = size;

        if(esize == 0)
            return {};

        data.size = esize;
        data      = Bytes::Alloc(data.size);

        szptr rsize = fread(data.data, sizeof(byte_t), esize, fh.handle);

        if(rsize != esize)
        {
            ec = FileError::ReadFailed;
            posix::collect_error_to(ec);
            return {};
        }

        data.size = rsize;

        return data;
    }
    STATICINLINE bool Seek(FH const& fh, u64 off, file_error& ec)
    {
        if(!Valid(fh, ec))
            return false;
        auto stat = fseek(fh.handle, off, SEEK_SET);

        if(stat != 0)
        {
            ec = FileError::SystemError;
            posix::collect_error_to(ec);
        }

        return stat == 0;
    }
    STATICINLINE bool Write(FH const& fh, Bytes const& d, file_error& ec)
    {
        if(!Valid(fh, ec))
            return false;

        szptr wsize = fwrite(d.data, sizeof(byte_t), d.size, fh.handle);

        if(wsize != d.size)
        {
            ec = FileError::WriteFailed;
            posix::collect_error_to(ec);
            return false;
        }

        return true;
    }

    STATICINLINE szptr Size(Url const& fn, file_error& ec)
    {
        FH f = Open(fn, RSCA::ReadOnly, ec);

        if(f.handle)
        {
            szptr tmp = Size(f, ec);
            return tmp;
        } else
        {
            ec = FileError::NotFound;
            posix::collect_error_to(ec);
            return 0;
        }
    }
    STATICINLINE szptr Size(FH const& fh, file_error& ec)
    {
        if(!Valid(fh, ec))
            return 0;

        szptr offset = ftell(fh.handle);
        int   res    = fseek(fh.handle, 0, SEEK_END);

        if(res != 0)
        {
            ec = FileError::SystemError;
            posix::collect_error_to(ec);
            return 0;
        }

        szptr fsize = ftell(fh.handle);
        res         = fseek(fh.handle, offset, SEEK_SET);

        if(res != 0)
        {
            ec = FileError::SystemError;
            posix::collect_error_to(ec);
            return 0;
        }

        return fsize;
    }

    STATICINLINE bool Exists(Url const& fn, file_error& ec)
    {
        FH f = Open(fn, RSCA::ReadOnly, ec);
        return Valid(f, ec);
    }

    template<typename... Args>
    STATICINLINE FileMapping Map(Args&&... args)
    {
        return FileMappingImpl::Map(std::forward<Args>(args)...);
    }
    template<typename... Args>
    STATICINLINE bool Unmap(Args&&... args)
    {
        return FileMappingImpl::Unmap(std::forward<Args>(args)...);
    }
};

template<typename FH>
using CFILEFun_def = CFILEFunBase_def<FILE_error_code, FH>;

using CFILEFun = CFILEFun_def<FILEApi::FileHandle>;

} // namespace file
} // namespace platform
