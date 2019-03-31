#pragma once

#include <peripherals/base.h>

#ifdef COFFEE_ANDROID

#include <platforms/posix/file.h>

struct AAsset;

namespace platform {
namespace file {
namespace android {

struct FileApi
{
    struct FileHandle : posix::PosixApi::FileHandle
    {
        AAsset* fp;
    };
};

struct FileFun : posix::PosixFileFun_def<

                     FileApi::FileHandle,
                     FileFunDef<>::FileMapping,
                     FileFunDef<>::ScratchBuf

                     >
{
    using FileHandle = FileApi::FileHandle;

    struct FileMapping : FileFunDef<>::FileMapping
    {
        FileHandle handle;
    };

    using Ancestor = posix::PosixFileFun_def<
        FileApi::FileHandle,
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

} // namespace android

using FileFun = android::FileFun;
using DirFun  = posix::DirFun;

} // namespace file
} // namespace platform
#endif
