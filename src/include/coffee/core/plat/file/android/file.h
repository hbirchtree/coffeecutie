#pragma once

#include "../../plat_primary_identify.h"

#ifdef COFFEE_ANDROID

#include "../../../coffee_message_macros.h"
#include "../unix/file.h"

struct AAsset;

namespace Coffee {
namespace CResources {
namespace Android {

struct AndroidFileApi
{
    struct FileHandle : Posix::PosixApi::FileHandle
    {
        AAsset* fp;
    };
};

struct AndroidFileFun : Posix::PosixFileFun_def<

                            AndroidFileApi::FileHandle,
                            FileFunDef<>::FileMapping,
                            FileFunDef<>::ScratchBuf

                            >
{
    using FileHandle = AndroidFileApi::FileHandle;

    struct FileMapping : FileFunDef<>::FileMapping
    {
        FileHandle handle;
    };

    using Ancestor = Posix::PosixFileFun_def<
        AndroidFileApi::FileHandle,
        FileFunDef<>::FileMapping,
        FileFunDef<>::ScratchBuf>;

    static FileHandle Open(Url const& fn, RSCA ac, file_error& ec);

    static bool Close(FileHandle&& fh, file_error& ec);

    static Bytes Read(FileHandle const& fh, szptr size, file_error& ec);

    static bool Write(FileHandle const& fh, const Bytes& d, file_error& ec);

    static szptr Size(FileHandle const& fh, file_error& ec);
    static szptr Size(Url const& fn, file_error& ec);

    static FileMapping Map(
        Url const& fn, RSCA acc, szptr offset, szptr size, file_error& ec);
    static bool Unmap(FileMapping&& mp, file_error& ec);
};

} // namespace Android

using FileFun = Android::AndroidFileFun;
using DirFun  = Posix::PosixDirFun;

} // namespace CResources
} // namespace Coffee
#endif
