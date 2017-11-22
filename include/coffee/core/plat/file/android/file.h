#pragma once

#include "../../plat_primary_identify.h"

#ifdef COFFEE_ANDROID

#include "../unix/file.h"

#include "../../../coffee_message_macros.h"

struct AAsset;

namespace Coffee{
namespace CResources{
namespace Android{

struct AndroidFileApi
{
    struct FileHandle : Posix::PosixApi::FileHandle
    {
        AAsset *fp;
    };
};

struct AndroidFileFun :
        Posix::PosixFileFun_def<

        AndroidFileApi::FileHandle,
        Posix::PosixApi::FileMapping,
        CommonFileFun::ScratchBuf

        >
{
    using FileHandle = AndroidFileApi::FileHandle;

    struct FileMapping : FileFunDef::FileMapping
    {
        FileHandle* handle;
    };

    using Ancestor = Posix::PosixFileFun_def<
    AndroidFileApi::FileHandle,
    Posix::PosixApi::FileMapping,
    CommonFileFun::ScratchBuf>;

    static CString NativePath(cstring fn);
    static CString NativePath(cstring fn,ResourceAccess storage);

    static FileHandle* Open(Url const& fn, ResourceAccess ac);

    static bool Close(FileHandle *fh);

    static CByteData Read(FileHandle *fh, uint64 size, bool nterminate);

    static bool Write(FileHandle *fh, const CByteData &d, bool);

    static szptr Size(FileHandle *fh);
    static szptr Size(Url const& fn);

    static FileMapping Map(Url const& fn,ResourceAccess acc,
                           szptr offset,szptr size,int*);
    static bool Unmap(FileMapping* mp);
};

}

using FileFun = Android::AndroidFileFun;
using DirFun = DirFunDef;

}
}
#endif
