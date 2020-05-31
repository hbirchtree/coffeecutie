#pragma once

#include <peripherals/base.h>

#if defined(COFFEE_WINDOWS)


#include <sys/stat.h>

#include <platforms/libc/file.h>
#include <peripherals/platform/windows.h>
#include <peripherals/error/windows.h>

namespace platform {
namespace file {
namespace win32 {

using file_error =
    nested_domain_error_code<u32, platform::win32::error_category, FILE_error_code>;

struct WinFileApi
{
    struct FileHandle : FILEApi::FileHandle
    {
        FileHandle() : file(platform::win32::invalid_handle_value)
        {
        }
        enum FH_Type
        {
            FS,
            RC
        };
        win_handle file;
        HRSRC      rsrc;
        FH_Type    type = FS;
        RSCA       access;
    };
    struct FileMapping : FileFunDef<>::FileMapping
    {
        win_handle file;
        win_handle mapping;
    };
    struct ScratchBuf : CommonFileFun<>::ScratchBuf
    {
        win_handle mapping;
        win_handle view;
    };
    struct FileAccess
    {
        DWORD open;
        DWORD share;
        DWORD create;
        DWORD attr;
    };

    static FileAccess GetAccess(RSCA acc);

    static win_handle GetFileHandle(Url const& fn, RSCA acc);
    static DWORD      GetMappingFlags(RSCA acc);
    static DWORD      GetMappingViewFlags(RSCA acc);
};

using Win32FILEFun = CFILEFunBase_def<file_error, WinFileApi::FileHandle>;

struct WinFileFun : Win32FILEFun
{
    using FileHandle  = WinFileApi::FileHandle;
    using FileMapping = WinFileApi::FileMapping;
    using ScratchBuf  = WinFileApi::ScratchBuf;

    using Parent = Win32FILEFun;

    static FileHandle Open(Url const& fn, RSCA acc, file_error& ec);
    static bool       Close(FileHandle&& fh, file_error& ec);

    static bool Exists(Url const& fn, file_error& ec);

    static Bytes Read(FileHandle const& h, u64 size, file_error& ec);
    static bool  Write(FileHandle const& fh, Bytes const& d, file_error& ec);

    static szptr Size(FileHandle const& fh, file_error& ec);
    static szptr Size(Url const& fn, file_error& ec);

    static bool     Touch(NodeType, Url const&, file_error& ec);
    static bool     Rm(Url const& fn, file_error& ec);
    static NodeType Stat(Url const& fn, file_error& ec);

    static FileMapping Map(
        Url const& fn, RSCA acc, szptr off, szptr size, file_error& ec);
    static bool       Unmap(FileMapping&& fp, file_error& ec);
    static ScratchBuf ScratchBuffer(szptr size, RSCA acc, file_error& ec);
    static void       ScratchUnmap(ScratchBuf&& buf, file_error& ec);
};

struct WinDirFun : DirFunDef<file_error>
{
    using file_error = WinFileFun::file_error;

    /*TODO: Implement Windows directory functions*/
    static bool MkDir(Url const& dname, bool parent, file_error& ec);

    static bool RmDir(Url const&, file_error& ec);

    static bool Ls(Url const&, DirList&, file_error& ec);

    static Url Dirname(CString const& fn, file_error& ec);

    static Url Basename(CString const& fn, file_error& ec);
};

} // namespace win32

using DirFun  = win32::WinDirFun;
using FileFun = win32::WinFileFun;

} // namespace env
} // namespace platform

#endif
