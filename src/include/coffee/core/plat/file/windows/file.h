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
    static bool MkDir(Url const& dname, bool parent);

	static bool RmDir(Url const&);

	static bool Ls(Url const&, DirList&);
};

struct WinFileApi
{
    struct FileHandle : CResources::FILEApi::FileHandle
    {
        FileHandle() :
            file(nullptr)
        {
        }
        enum FH_Type
        {
            FS,
            RC
        };
        HANDLE file;
        HRSRC rsrc;
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

    static FileAccess GetAccess(ResourceAccess acc);

    static HANDLE GetFileHandle(Url const& fn, ResourceAccess acc);
    static DWORD GetMappingFlags(ResourceAccess acc);
    static DWORD GetMappingViewFlags(ResourceAccess acc);
};

struct WinFileFun : CResources::CFILEFun_def<WinFileApi::FileHandle>
{
    using FileHandle = WinFileApi::FileHandle;
    using FileMapping = WinFileApi::FileMapping;
    using ScratchBuf = WinFileApi::ScratchBuf;

//    static CString NativePath(cstring fn);
//    static CString NativePath(cstring fn,ResourceAccess storage);
//    static bool VerifyAsset(cstring fn);

    static FileHandle Open(Url const& fn, ResourceAccess acc);
    static bool Close(FileHandle&& fh);

    static bool Exists(Url const& fn);

    static Bytes Read(FileHandle const& h, uint64 size, bool nterminate);
    static bool Write(FileHandle const& fh, Bytes const& d, bool);

    static szptr Size(FileHandle const& fh);
    static szptr Size(Url const& fn);

    static bool Touch(NodeType, Url const&);
    static bool Rm(Url const& fn);
    static NodeType Stat(Url const& fn);

    static FileMapping Map(Url const& fn, ResourceAccess acc,
                           szptr off, szptr size, int* err);
    static bool Unmap(FileMapping&& fp);
    static ScratchBuf ScratchBuffer(szptr size, ResourceAccess acc);
    static void ScratchUnmap(ScratchBuf&& buf);
};
}

namespace CResources {
using DirFun = Windows::WinDirFun;
using FileFun = Windows::WinFileFun;
}
}

#endif
