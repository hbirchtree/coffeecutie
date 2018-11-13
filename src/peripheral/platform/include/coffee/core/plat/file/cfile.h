#pragma once

#include <cstdio>

#include "file_def.h"

namespace Coffee {
namespace CResources {

template<typename NestedError = sentinel_error_code>
struct CommonFileFun : FileFunDef<NestedError>
{
};

struct FILE_error_category : error_category
{
    virtual const char* name() const noexcept;
    virtual std::string message(int error_code) const;
};

using FILE_error_code = domain_error_code<int, FILE_error_category>;

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
    struct FileMapping : non_copy
    {
        static FileMapping Wrap(
            FileFunDef<>::FileMapping&& data, FileHandle&& handle)
        {
            FileMapping out;

            out.fm_handle = std::move(data);
            out.handle    = std::move(handle);
            out.data      = out.fm_handle.data;
            out.size      = out.fm_handle.size;

            return out;
        }

        FileMapping() : fm_handle(), handle(), data(nullptr), size(0)
        {
        }

        FileMapping(FileMapping&& other) :
            fm_handle(std::move(other.fm_handle)),
            handle(std::move(other.handle)), data(other.data), size(other.size)
        {
            other.fm_handle = {};
            other.handle    = {};
            other.data      = nullptr;
            other.size      = 0;
        }

        FileMapping& operator=(FileMapping&& other)
        {
            this->fm_handle = std::move(other.fm_handle);
            this->handle    = std::move(other.handle);
            this->data      = other.data;
            this->size      = other.size;

            other.fm_handle = {};
            other.handle    = {};
            other.data      = nullptr;
            other.size      = 0;

            return *this;
        }

        FileFunDef<>::FileMapping fm_handle;
        FileHandle                handle;
        void*                     data;
        szptr                     size;
    };

    static void CheckError(FILE* fd, FILE_error_code& ec);
};

template<
    typename NestedError,
    typename FH,
    typename implements<FILEApi::FileHandle, FH>::type* = nullptr>
struct CFILEFunBase_def : CommonFileFun<NestedError>
{
    using file_error  = typename CommonFileFun<NestedError>::file_error;
    using FileMapping = FILEApi::FileMapping;

    STATICINLINE FH Open(Url const& fn, RSCA ac, file_error& ec)
    {
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
            FILEApi::CheckError(fh.handle, ec.template as<FILE_error_code>());
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
            FILEApi::CheckError(fh.handle, ec.template as<FILE_error_code>());
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
            FILEApi::CheckError(fh.handle, ec.template as<FILE_error_code>());
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
            FILEApi::CheckError(fh.handle, ec.template as<FILE_error_code>());
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
            FILEApi::CheckError(fh.handle, ec.template as<FILE_error_code>());
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
            FILEApi::CheckError(f.handle, ec.template as<FILE_error_code>());
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
            FILEApi::CheckError(fh.handle, ec.template as<FILE_error_code>());
            return 0;
        }

        szptr fsize = ftell(fh.handle);
        res         = fseek(fh.handle, offset, SEEK_SET);

        if(res != 0)
        {
            ec = FileError::SystemError;
            FILEApi::CheckError(fh.handle, ec.template as<FILE_error_code>());
            return 0;
        }

        return fsize;
    }
    /*!
     * \brief Wrapped function for platforms without support for file mapping.
     * \param fname
     * \param access
     * \param size
     * \param offset
     * \param err
     * \return
     */
    STATICINLINE FileMapping
                 Map(Url const& fname, RSCA access, szptr offset, szptr size, file_error& ec)
    {
        auto  handle = Open(fname, access, ec);
        szptr r_size = Size(handle, ec);
        if(size + offset > r_size)
        {
            return {};
        }

        auto data = Read(handle, offset + size, ec);
        data.assignAccess(access);

        auto f = FileMapping::Wrap(std::move(data), std::move(handle));

        data   = {};
        handle = {};

        return f;
    }
    /*!
     * \brief Unmap the fake mapping.
     *  Works by moving the mapping into this scope, deleting it on return.
     * \param map
     * \return
     */
    STATICINLINE bool Unmap(C_UNUSED(FileMapping&& map), file_error&)
    {
        return true;
    }
    STATICINLINE bool Exists(Url const& fn, file_error& ec)
    {
        FH f = Open(fn, RSCA::ReadOnly, ec);
        return Valid(f, ec);
    }
};

template<typename FH>
using CFILEFun_def = CFILEFunBase_def<FILE_error_code, FH>;

using CFILEFun = CFILEFun_def<FILEApi::FileHandle>;

} // namespace CResources
} // namespace Coffee
