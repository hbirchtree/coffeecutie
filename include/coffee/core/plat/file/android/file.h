#pragma once

#include "../../plat_primary_identify.h"

#ifdef COFFEE_ANDROID

#include "../unix/file.h"

#include "../../../coffee_message_macros.h"

namespace Coffee{
namespace CResources{
namespace Android{

struct AndroidFileApi
{
    struct SDLData;

    struct FileHandle : Posix::PosixApi::FileHandle
    {
        SDLData *extra_data;
    };
};

struct AndroidFileFun : Posix::PosixFileFun_def<AndroidFileApi::FileHandle>
{
    using FileHandle = AndroidFileApi::FileHandle;

    struct FileMapping : FileFunDef::FileMapping
    {
        FileHandle* handle;
        void* ptr_backing;
    };

    using Ancestor = Posix::PosixFileFun_def<AndroidFileApi::FileHandle>;

    static CString NativePath(cstring fn);

    static FileHandle* Open(cstring fn, ResourceAccess ac);

    static bool Close(FileHandle *fh);

    static CByteData Read(FileHandle *fh, uint64 size, bool nterminate);

    static bool Write(FileHandle *fh, const CByteData &d, bool);

    static szptr Size(FileHandle *fh);
    static szptr Size(cstring fn);

    STATICINLINE
    /*!
     * \brief If you are here about performance, you may be wondering why files
     *  are not being mapped into memory virtually.
     *  The truth is that Android supports this poorly on low-memory systems,
     *  and therefore we opt for reading the file into memory as a safer method.
     *  Yes, it is slower, but it's more versatile on Android.
     *  We'll still offset the pointer for you, though!
     * \param fn
     * \param acc
     * \param offset
     * \param size
     * \return
     */
    FileMapping Map(cstring fn,ResourceAccess acc,
                    szptr offset,szptr size,int*)
    {
        FileMapping map = {};

        map.ptr_backing = nullptr;

        map.handle = Open(fn,acc);
        if(!map.handle)
            return {};
        CByteData dat = Read(map.handle,size,false);

        if(offset+size > dat.size)
        {
            if(dat.data)
                CFree(dat.data);
            if(map.handle)
                Close(map.handle);
            return {};
        }

        byte_t* ptr = (byte_t*)dat.data;
        map.ptr = &ptr[offset];
        map.ptr_backing = dat.data;
        map.size = dat.size;
        map.acc = acc;

        return map;
    }
    STATICINLINE bool Unmap(FileMapping* mp)
    {
        if(mp->ptr_backing)
            CFree(mp->ptr_backing);
        return true;
    }
};

}

using FileFun = Android::AndroidFileFun;
using DirFun = DirFunDef;

}
}
#endif
