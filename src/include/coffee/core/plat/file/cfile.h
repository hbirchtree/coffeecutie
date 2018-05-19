#ifndef COFFEE_CORE_PLAT_CFILE_H
#define COFFEE_CORE_PLAT_CFILE_H

#include <cstdio>

#include "../../coffee_strings.h"
#include "file_def.h"

namespace Coffee {
namespace CResources {

struct CommonFileFun : FileFunDef
{
};

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
            FileFunDef::FileMapping&& data, FileHandle&& handle)
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
            this->handle = std::move(other.handle);
            this->data = other.data;
            this->size = other.size;

            other.fm_handle = {};
            other.handle    = {};
            other.data      = nullptr;
            other.size      = 0;

            return *this;
        }

        FileFunDef::FileMapping fm_handle;
        FileHandle              handle;
        void*                   data;
        szptr                   size;
    };
};

template<
    typename FH,
    typename implements<FILEApi::FileHandle, FH>::type* = nullptr>
struct CFILEFun_def : CommonFileFun
{
    using FileMapping = FILEApi::FileMapping;

    STATICINLINE FH Open(Url const& fn, ResourceAccess ac)
    {
        auto url = *fn;
        FH   fh  = {};

        cstring mode = nullptr;

        /* Because of the way masks work, feval must be run on each individual
         * flag */
        if(feval(ac & ResourceAccess::ReadWrite) &&
           feval(ac & ResourceAccess::Append))
            mode = "ab+";
        else if(
            feval(ac & ResourceAccess::WriteOnly) &&
            feval(ac & ResourceAccess::Append))
            mode = "ab";
        else if(feval(ac & (ResourceAccess::ReadOnly)))
            mode = "rb";
        else if(
            feval(ac & ResourceAccess::WriteOnly) &&
            feval(ac & ResourceAccess::Discard))
            mode = "wb+";
        else if(feval(ac & (ResourceAccess::WriteOnly)))
            mode = "wb";
        else if(feval(ac & (ResourceAccess::ReadWrite)))
            mode = "rb+";

        fh.handle = fopen(url.c_str(), mode);

        if(!fh.handle)
            return {};

        return fh;
    }
    STATICINLINE bool Valid(FH const& fh)
    {
        return fh.handle != nullptr;
    }
    STATICINLINE bool Close(FH&& fh)
    {
        fclose(fh.handle);
        return true;
    }

    STATICINLINE Bytes Read(FH const& fh, uint64 size, bool nterminate)
    {
        Bytes data;
        data.elements = 0;
        szptr esize   = Size(fh);
        if(size <= esize && size != -1)
            esize = size;

        if(esize == 0)
            return {};

        if(nterminate)
            data.size = esize + 1;
        else
            data.size = esize;

        data        = Bytes::Alloc(data.size);
        szptr rsize = fread(data.data, sizeof(byte_t), esize, fh.handle);
        data.size   = rsize;
        if(nterminate)
            data.data[esize - 1] = 0;
        //        if(rsize<esize)
        //            cLog(CFStrings::Plat_File_Native_SizeErr,esize,rsize);
        return data;
    }
    STATICINLINE bool Seek(FH const& fh, uint64 off)
    {
        return fseek(fh.handle, off, SEEK_SET) == 0;
    }
    STATICINLINE bool Write(FH const& fh, Bytes const& d, bool)
    {
        szptr wsize = fwrite(d.data, sizeof(byte_t), d.size, fh.handle);
        return wsize == d.size;
    }

    STATICINLINE szptr Size(Url const& fn)
    {
        FH f = Open(fn, ResourceAccess::ReadOnly);
        if(f.handle)
        {
            szptr tmp = Size(f);
            return tmp;
        } else
            return 0;
    }
    STATICINLINE szptr Size(FH const& fh)
    {
        szptr offset = ftell(fh.handle);
        fseek(fh.handle, 0, SEEK_END);
        szptr fsize = ftell(fh.handle);
        fseek(fh.handle, offset, SEEK_SET);
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
                 Map(Url const& fname, ResourceAccess access, szptr offset, szptr size, int*)
    {
        auto  handle = Open(fname, access);
        szptr r_size = Size(handle);
        if(size + offset > r_size)
        {
            return {};
        }

        auto data = Read(handle, offset + size, false);
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
    STATICINLINE bool Unmap(FileMapping&& map)
    {
        C_USED(map);

        return true;
    }
    STATICINLINE bool Exists(Url const& fn)
    {
        FH f = Open(fn, ResourceAccess::ReadOnly);
        if(f.handle)
            return true;
        return false;
    }
};

using CFILEFun = CFILEFun_def<FILEApi::FileHandle>;

} // namespace CResources
} // namespace Coffee

#endif