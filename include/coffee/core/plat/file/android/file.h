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
    using FileHandle = Posix::PosixApi::FileHandle;
};

struct AndroidFileFun : Posix::PosixFileFun
{
    using FileHandle = AndroidFileApi::FileHandle;
    struct FileMapping : FileFunDef::FileMapping
    {
        FileHandle* handle;
        void* ptr_backing;
    };

    static CString NativePath(cstring fn);

    STATICINLINE FileMapping Map(cstring fn,ResourceAccess acc,szptr offset,szptr size,int*)
    {
        FileMapping map = {};

        map.handle = Open(fn,acc);
        CByteData dat = Read(map.handle,size,false);

        if(offset+size > dat.size)
        {
            CFree(dat.data);
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