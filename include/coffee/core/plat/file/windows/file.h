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
    STATICINLINE bool MkDir(cstring, bool)
    {
        return false;
    }
    STATICINLINE bool RmDir(cstring)
    {
        return false;
    }
};

struct WinFileApi
{
    struct FileHandle : CResources::FILEApi::FileHandle
    {
        FileHandle() :
            file(nullptr)
        {
        }
        HANDLE file;
    };
    struct FileMapping : CResources::FileFunDef::FileMapping
    {
        HANDLE file;
        HANDLE mapping;
    };
    struct FileAccess
    {
        DWORD open;
        DWORD share;
        DWORD create;
        DWORD attr;
    };

    STATICINLINE FileAccess GetAccess(ResourceAccess acc)
    {
        FileAccess f;
        f.open = 0;
        f.share = 0;
        f.create = 0;
        f.attr = 0;

        if (feval(acc&ResourceAccess::ReadOnly))
            f.open |= GENERIC_READ;
        if (feval(acc&ResourceAccess::WriteOnly))
            f.open |= GENERIC_WRITE;
        if (feval(acc&ResourceAccess::Executable))
            f.open |= GENERIC_EXECUTE;

        if (!feval(acc&ResourceAccess::ExclusiveLocking))
            f.share |= FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE;

        if (feval(acc&ResourceAccess::Discard))
            f.create |= CREATE_ALWAYS;
        else if (feval(acc&ResourceAccess::NewFile))
            f.create |= CREATE_NEW;
        else
            f.create |= OPEN_EXISTING;

        return f;
    }

    STATICINLINE HANDLE GetFileHandle(cstring fn, ResourceAccess acc)
    {
        FileAccess f = GetAccess(acc);
        return CreateFile(fn,f.open,f.share,nullptr,f.create,f.attr,nullptr);
    }
};

struct WinFileFun : CResources::CFILEFun_def<WinFileApi::FileHandle>
{
    using FileHandle = WinFileApi::FileHandle;
    using FileMapping = WinFileApi::FileMapping;

    STATICINLINE FileHandle* Open(cstring fn, ResourceAccess acc)
    {
        HANDLE ff = WinFileApi::GetFileHandle(fn,acc);

        if (ff == INVALID_HANDLE_VALUE )
            return nullptr;

        FileHandle* fh = CFILEFun_def<FileHandle>::Open(fn, acc);
        if (!fh)
        {
            CloseHandle(ff);
            return nullptr;
        }
        fh->file = ff;

        return fh;
    }
    STATICINLINE bool Close(FileHandle* fh)
    {
        if (fh)
        {
            HANDLE file = fh->file;
            return CFILEFun_def<FileHandle>::Close(fh)&&CloseHandle(file);
        }
        else
            return false;
        return false;
    }

    STATICINLINE bool Exists(cstring fn)
    {
        HANDLE fh = CreateFile(fn,0,FILE_SHARE_READ|FILE_SHARE_WRITE,nullptr,OPEN_EXISTING,0,nullptr);
        if (fh)
        {
            CloseHandle(fh);
            return true;
        }else
            return false;
    }

	STATICINLINE bool Write(FileHandle* fh, CByteData const& d, bool)
	{
		DWORD size = 0;
		BOOL stat = WriteFile(fh->file,d.data,d.size,&size,nullptr);
		BOOL stat2 = FlushFileBuffers(fh->file);
		return stat && stat2 && size == d.size;
	}

    STATICINLINE szptr Size(FileHandle* fh)
    {
        DWORD highpart = 0;
        DWORD low = GetFileSize(fh->file,&highpart);
        return (highpart << 32)+low;
    }
    STATICINLINE szptr Size(cstring fn)
    {
        struct stat st;
        stat(fn, &st);
        return static_cast<szptr>(st.st_size);
    }

    STATICINLINE bool Touch(NodeType, cstring)
    {
        return false;
    }
    STATICINLINE bool Rm(cstring fn)
    {
        return DeleteFile(fn);
    }
    STATICINLINE NodeType Stat(cstring)
    {
        return NodeType::File;
    }

    STATICINLINE FileMapping Map(cstring fn, ResourceAccess acc, szptr off, szptr size, int* err)
    {
        if (off + size > Size(fn))
            return {};

        HANDLE fh = WinFileApi::GetFileHandle(fn, acc);

        if (fh==INVALID_HANDLE_VALUE)
        {
            *err = GetLastError();
            return {};
        }

        DWORD profl = 0;

        if (feval(acc&ResourceAccess::ReadOnly)
                && feval(acc&ResourceAccess::WriteOnly)
                && feval(acc&ResourceAccess::Executable)
                && feval(acc&ResourceAccess::Persistent))
            profl = PAGE_EXECUTE_READWRITE;
        else if (feval(acc&ResourceAccess::ReadOnly)
                 && feval(acc&ResourceAccess::WriteOnly)
                 && feval(acc&ResourceAccess::Persistent))
            profl = PAGE_READWRITE;
        if (feval(acc&ResourceAccess::ReadOnly)
                && feval(acc&ResourceAccess::WriteOnly)
                && feval(acc&ResourceAccess::Executable))
            profl = PAGE_EXECUTE_WRITECOPY;
        else if (feval(acc&ResourceAccess::ReadOnly)
                 && feval(acc&ResourceAccess::WriteOnly))
            profl = PAGE_WRITECOPY;
        else if (feval(acc&ResourceAccess::ReadOnly))
            profl = PAGE_READONLY;

        if (feval(acc&ResourceAccess::NoCache))
            profl |= SEC_NOCACHE;
        if (feval(acc&ResourceAccess::HugeFile))
            profl |= SEC_LARGE_PAGES;
        if (feval(acc&ResourceAccess::GreedyCache))
            profl |= SEC_COMMIT;
        if (feval(acc&ResourceAccess::Virtual))
            profl |= SEC_RESERVE;

        DWORD size_high = 0;
        DWORD size_low = off+size;

        HANDLE mh = CreateFileMapping(fh, nullptr, profl, size_high, size_low, nullptr);

        if (!mh)
        {
            *err = GetLastError();
            CloseHandle(fh);
            return {};
        }

        DWORD view_fl = 0;

        if (feval(acc&ResourceAccess::ReadOnly)
                && feval(acc&ResourceAccess::WriteOnly)
                && feval(acc&ResourceAccess::Persistent))
            view_fl = FILE_MAP_WRITE;
        if (feval(acc&ResourceAccess::ReadOnly)
                && feval(acc&ResourceAccess::WriteOnly))
            view_fl |= FILE_MAP_COPY;
        else if (feval(acc&ResourceAccess::ReadOnly))
            view_fl = FILE_MAP_READ;

        DWORD off_high = 0;
        DWORD off_low = off;

        void* ptr = MapViewOfFile(mh, view_fl, off_high, off_low, size);

        if (!ptr)
        {
            *err = GetLastError();
            CloseHandle(fh);
            CloseHandle(mh);
            return {};
        }

        FileMapping fm;
        fm.acc = acc;
        fm.file = fh;
        fm.mapping = mh;
        fm.size = size;
        fm.ptr = ptr;

        return fm;
    }
    STATICINLINE bool Unmap(FileMapping* fp)
    {
        if (!UnmapViewOfFile(fp->ptr))
            return false;
        fp->ptr = nullptr;
        if (!CloseHandle(fp->mapping))
            return false;
        fp->mapping = nullptr;
        if (!CloseHandle(fp->file))
            return false;
        fp->file = nullptr;

        return true;
    }
};
}

namespace CResources {
using DirFun = Windows::WinDirFun;
using FileFun = Windows::WinFileFun;
}
}

#endif
