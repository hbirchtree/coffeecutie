#pragma once

#include "../../platform_detect.h"

#ifdef COFFEE_WINDOWS

#include "../cfile.h"

#include <sys/stat.h>

#include "../../plat_windows.h"

namespace Coffee {
namespace Windows {
struct WinDirFun : CResources::DirFunDef
{
    /*TODO: Implement Windows directory functions*/
    static bool MkDir(Url const& dname, bool parent, file_error& ec);

    static bool RmDir(Url const&, file_error& ec);

    static bool Ls(Url const&, DirList&, file_error& ec);

    static Url Dirname(CString const& fn, file_error& ec);

    static Url Basename(CString const& fn, file_error& ec);
};

struct WinFileApi
{
    struct FileHandle : CResources::FILEApi::FileHandle
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
    struct FileMapping : CResources::FileFunDef::FileMapping
    {
        HANDLE file;
        HANDLE mapping;
    };
    struct ScratchBuf : CResources::CommonFileFun::ScratchBuf
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

struct WinFileFun : CResources::CFILEFun_def<WinFileApi::FileHandle>
{
    using FileHandle  = WinFileApi::FileHandle;
    using FileMapping = WinFileApi::FileMapping;
    using ScratchBuf  = WinFileApi::ScratchBuf;

    static FileHandle Open(Url const& fn, RSCA acc, file_error& ec);
    static bool       Close(FileHandle&& fh, file_error& ec);

    static bool Exists(Url const& fn, file_error& ec);

    static Bytes Read(FileHandle const& h, uint64 size, file_error& ec);
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
} // namespace Windows

namespace CResources {
using DirFun  = Windows::WinDirFun;
using FileFun = Windows::WinFileFun;
} // namespace CResources
} // namespace Coffee

#endif
