#pragma once

#include "../../platform_detect.h"

#ifdef COFFEE_WINDOWS

#include "../cfile.h"

#include <sys/stat.h>

#include "../../plat_windows.h"

namespace Coffee {
namespace CResources {
namespace Windows {

struct win32_error_category : error_category
{
    virtual const char* name() const noexcept
    {
        return "win32_error_code";
    }
    virtual std::string message(int error_code) const
    {
        return "";
    }
};

using win32_error_code =
    nested_domain_error_code<DWORD, win32_error_category, FILE_error_code>;

struct WinFileApi
{
    struct FileHandle : FILEApi::FileHandle
    {
        FileHandle() : file(nullptr)
        {
        }
        enum FH_Type
        {
            FS,
            RC
        };
        HANDLE  file;
        HRSRC   rsrc;
        FH_Type type = FS;
    };
    struct FileMapping : FileFunDef<>::FileMapping
    {
        HANDLE file;
        HANDLE mapping;
    };
    struct ScratchBuf : CommonFileFun<>::ScratchBuf
    {
        HANDLE mapping;
        HANDLE view;
    };
    struct FileAccess
    {
        DWORD open;
        DWORD share;
        DWORD create;
        DWORD attr;
    };

    static FileAccess GetAccess(RSCA acc);

    static HANDLE GetFileHandle(Url const& fn, RSCA acc);
    static DWORD  GetMappingFlags(RSCA acc);
    static DWORD  GetMappingViewFlags(RSCA acc);
};

using Win32FILEFun = CFILEFunBase_def<win32_error_code, WinFileApi::FileHandle>;

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

struct WinDirFun : DirFunDef<win32_error_code>
{
    using file_error = WinFileFun::file_error;

    /*TODO: Implement Windows directory functions*/
    static bool MkDir(Url const& dname, bool parent, file_error& ec);

    static bool RmDir(Url const&, file_error& ec);

    static bool Ls(Url const&, DirList&, file_error& ec);

    static Url Dirname(CString const& fn, file_error& ec);

    static Url Basename(CString const& fn, file_error& ec);
};

} // namespace Windows

using DirFun  = Windows::WinDirFun;
using FileFun = Windows::WinFileFun;

} // namespace CResources
} // namespace Coffee

#endif
