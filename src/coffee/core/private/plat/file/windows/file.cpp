#include <coffee/core/plat/file/windows/file.h>
#include <coffee/core/plat/plat_environment.h>

#include "../file_abstraction.h"

#include <fileapi.h>

namespace Coffee {
namespace Windows {
const constexpr cstring coffee_rsc_tag = "CF_RES";

WinFileApi::FileAccess WinFileApi::GetAccess(ResourceAccess acc)
{
    FileAccess f;
    f.open   = 0;
    f.share  = 0;
    f.create = 0;
    f.attr   = 0;

    if(feval(acc & ResourceAccess::ReadOnly))
        f.open |= GENERIC_READ;
    if(feval(acc & ResourceAccess::WriteOnly))
        f.open |= GENERIC_WRITE;
    if(feval(acc & ResourceAccess::Executable))
        f.open |= GENERIC_EXECUTE;

    if(!feval(acc & ResourceAccess::ExclusiveLocking))
        f.share |= FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE;

    if(feval(acc & ResourceAccess::Discard))
        f.create |= CREATE_ALWAYS;
    else if(feval(acc & ResourceAccess::NewFile))
        f.create |= CREATE_NEW;
    else
        f.create |= OPEN_EXISTING;

    return f;
}
HANDLE WinFileApi::GetFileHandle(Url const& fn, ResourceAccess acc)
{
    auto       url = *fn;
    FileAccess f   = GetAccess(acc);
#ifdef COFFEE_WINDOWS_UWP
    CWString fn_w = StrUtil::convertformat<wbyte_t>(url);
    return CreateFile2(&fn_w[0], f.open, f.share, f.create, nullptr);
    return INVALID_HANDLE_VALUE;
#else
    return CreateFile(
        url.c_str(), f.open, f.share, nullptr, f.create, f.attr, nullptr);
#endif
}
DWORD WinFileApi::GetMappingFlags(ResourceAccess acc)
{
    DWORD profl = 0;

    if(feval(acc & ResourceAccess::ReadOnly) &&
       feval(acc & ResourceAccess::WriteOnly) &&
       feval(acc & ResourceAccess::Executable) &&
       feval(acc & ResourceAccess::Persistent))
        profl = PAGE_EXECUTE_READWRITE;
    else if(
        feval(acc & ResourceAccess::ReadOnly) &&
        feval(acc & ResourceAccess::WriteOnly) &&
        feval(acc & ResourceAccess::Persistent))
        profl = PAGE_READWRITE;
    if(feval(acc & ResourceAccess::ReadOnly) &&
       feval(acc & ResourceAccess::WriteOnly) &&
       feval(acc & ResourceAccess::Executable))
        profl = PAGE_EXECUTE_WRITECOPY;
    else if(
        feval(acc & ResourceAccess::ReadOnly) &&
        feval(acc & ResourceAccess::WriteOnly))
        profl = PAGE_WRITECOPY;
    else if(feval(acc & ResourceAccess::ReadOnly))
        profl = PAGE_READONLY;

    if(feval(acc & ResourceAccess::NoCache))
        profl |= SEC_NOCACHE;
    if(feval(acc & ResourceAccess::HugeFile))
        profl |= SEC_LARGE_PAGES;
    if(feval(acc & ResourceAccess::GreedyCache))
        profl |= SEC_COMMIT;
    if(feval(acc & ResourceAccess::Virtual))
        profl |= SEC_RESERVE;

    return profl;
}

DWORD WinFileApi::GetMappingViewFlags(ResourceAccess acc)
{
    DWORD view_fl = 0;

    if(feval(acc & ResourceAccess::ReadOnly) &&
       feval(acc & ResourceAccess::WriteOnly) &&
       feval(acc & ResourceAccess::Persistent))
        view_fl = FILE_MAP_WRITE;
    if(feval(acc & ResourceAccess::ReadOnly) &&
       feval(acc & ResourceAccess::WriteOnly))
        view_fl |= FILE_MAP_COPY;
    else if(feval(acc & ResourceAccess::ReadOnly))
        view_fl = FILE_MAP_READ;

    return view_fl;
}

// CString WinFileFun::NativePath(cstring fn)
//{
//    if (fn[0] == ':')
//    {
//        //auto path =
//        ::Windows::ApplicationModel::Package::Current->InstalledLocation->Path->Data();
//        //CString appdir = StrUtil::convertformat<char, wchar_t>(path);
//        //CString appdir = Env::ApplicationDir();
//        cstring asset_path = AssetApi::GetAsset(fn);
//        //CString conc_path = Env::ConcatPath(appdir.c_str(), asset_path);
//        CString conc_path = asset_path;
//        conc_path = Mem::CStrReplace(conc_path, "/", "\\");
//#if !defined(COFFEE_WINDOWS_UWP)
//        conc_path = ":/" + conc_path;
//#endif
//        return conc_path;
//    }
//    return fn;
//}

// CString WinFileFun::NativePath(cstring fn, ResourceAccess storage)
//{
//    if(feval(storage,ResourceAccess::TemporaryFile))
//    {
//        CString temp_dir = Env::GetVar("TEMP");
//        return Env::ConcatPath(temp_dir.c_str(),fn);
//    }
//    else
//        return NativePath(fn);
//}

CString create_rsc_name(cstring fn)
{
    if(!fn)
        return {};

    /* Transform the filename to correspond with  */
    CString wrap = fn;
    wrap         = '"' + wrap + '"';
    wrap         = CStrReplace(wrap, "_", "___");
    wrap         = CStrReplace(wrap, "/", "_");
    wrap         = CStrReplace(wrap, "\\", "_");
    return wrap;
}

HRSRC open_rsc(cstring fn)
{
    CString wrap = create_rsc_name(fn);
#if !defined(COFFEE_WINDOWS_UWP)
    return FindResourceEx(nullptr, coffee_rsc_tag, wrap.c_str(), 1033);
#else
    return 0;
#endif
}

// bool WinFileFun::VerifyAsset(cstring fn)
//{
//#if defined(COFFEE_WINDOWS_UWP)
//    return true;
//#else
//    return open_rsc(fn);
//#endif
//}

WinFileFun::FileHandle WinFileFun::Open(Url const& fn, ResourceAccess acc)
{
    auto url = *fn;
    if(feval(fn.flags & ResourceAccess::AssetFile) &&
       !feval(acc, ResourceAccess::WriteOnly))
    {
        HRSRC rsc = open_rsc(url.c_str());

        if(rsc)
        {
            FileHandle h = {};
            h.type       = FileHandle::RC;
            h.rsrc       = rsc;
            return h;
        } else
            return {};
    }

    HANDLE ff = WinFileApi::GetFileHandle(fn, acc);

    if(ff == INVALID_HANDLE_VALUE)
        return {};

    FileHandle fh = CFILEFun_def<FileHandle>::Open(fn, acc);
    if(!fh.handle)
    {
        CloseHandle(ff);
        return {};
    }
    fh.file = ff;

    return fh;
}

bool WinFileFun::Close(WinFileFun::FileHandle&& fh)
{
    if(fh.type == FileHandle::FS)
    {
        HANDLE file = fh.file;
        return CFILEFun_def<FileHandle>::Close(std::move(fh)) &&
               CloseHandle(file);
    } else
        return true;
    return false;
}

Bytes WinFileFun::Read(FileHandle const& h, uint64 size, bool)
{
    if(h.type == FileHandle::FS)
    {
        Bytes d;

        d.size = Size(h);
        d.size = (size < d.size) ? size : d.size;

        d = Bytes::Alloc(d.size);

        DWORD size = 0;
        szptr i    = 0;
        DWORD chnk = 0;

        while(i < d.size)
        {
            chnk      = ((d.size - i) < UInt32_Max) ? (d.size - i) : UInt32_Max;
            BOOL stat = ReadFile(h.file, &d.data[i], chnk, &size, nullptr);
            SetFilePointer(h.file, chnk, nullptr, FILE_CURRENT);
            if(stat && size == chnk)
                i += chnk;
            else
            {
                d = Bytes();
                break;
            }
        }

        SetFilePointer(h.file, 0, nullptr, FILE_BEGIN);

        return d;
    } else
    {
        Bytes d = {};

#ifndef COFFEE_WINDOWS_UWP
        HGLOBAL lsrc = LoadResource(nullptr, h.rsrc);
        d.size       = SizeofResource(nullptr, h.rsrc);
        d.size       = (size < d.size) ? size : d.size;
        d.data       = C_RCAST<byte_t*>(LockResource(lsrc));
#endif

        return d;
    }
}

bool WinFileFun::Write(FileHandle const& fh, Bytes const& d, bool)
{
    if(fh.type == FileHandle::FS)
    {
        DWORD size = 0;
        szptr i    = 0;
        DWORD chnk = 0;
        while(i < d.size)
        {
            chnk      = ((d.size - i) < UInt32_Max) ? (d.size - i) : UInt32_Max;
            BOOL stat = WriteFile(fh.file, &d.data[i], chnk, &size, nullptr);
            SetFilePointer(fh.file, chnk, nullptr, FILE_CURRENT);
            if(stat && chnk == size)
                i += chnk;
            else
                break;
        }

        BOOL stat2 = FlushFileBuffers(fh.file);

        SetFilePointer(fh.file, 0, nullptr, FILE_BEGIN);

        return stat2 && i == d.size;
    } else
        return false;
}

bool WinFileFun::Exists(Url const& fn)
{
    /* If it's secretly a resource, return now with true */
    if(feval(fn.flags & ResourceAccess::AssetFile))
        return true;

    auto url = *fn;

    HANDLE fh = INVALID_HANDLE_VALUE;
#ifndef COFFEE_WINDOWS_UWP
    fh = CreateFile(url.c_str(), 0, 0, nullptr, OPEN_EXISTING, 0, nullptr);
#endif
    if(fh != INVALID_HANDLE_VALUE)
    {
        CloseHandle(fh);
        return true;
    } else
    {
        GetLastError();
        return false;
    }
}

szptr WinFileFun::Size(WinFileFun::FileHandle const& fh)
{
    if(fh.type == FileHandle::FS)
    {
        LARGE_INTEGER e;
        e.QuadPart = 0;
        if(GetFileSizeEx(fh.file, &e) == FALSE)
            return 0;
        return e.QuadPart;
    }
#ifndef COFFEE_WINDOWS_UWP
    return SizeofResource(nullptr, fh.rsrc);
#else
    return 0;
#endif
}
szptr WinFileFun::Size(Url const& fn)
{
    auto url = *fn;
#ifndef COFFEE_WINDOWS_UWP
    if(feval(fn.flags & ResourceAccess::AssetFile))
    {
        HRSRC rsc_h = open_rsc(url.c_str());
        DWORD sz    = 0;
        sz          = SizeofResource(nullptr, rsc_h);
        return sz;
    } else
#endif
    {
        LARGE_INTEGER e;
        HANDLE        f = INVALID_HANDLE_VALUE;
#ifndef COFFEE_WINDOWS_UWP
        f = CreateFile(
            url.c_str(), GENERIC_READ, 0, nullptr, OPEN_ALWAYS, 0, nullptr);
#else
        CWString wname =
            Mem::StrUtil::convertformat<wchar_t, char>(url.c_str());
        f = CreateFile2(wname.c_str(), GENERIC_READ, 0, OPEN_ALWAYS, nullptr);
        CString err = win_strerror(GetLastError());
#endif
        if(f != INVALID_HANDLE_VALUE)
        {
            BOOL s = GetFileSizeEx(f, &e);
            CloseHandle(f);
            return e.QuadPart;
        } else
            return 0;
    }
}

bool WinFileFun::Touch(NodeType t, Url const& n)
{
    auto url = *n;

    switch(t)
    {
    case NodeType::File:
    {
        HANDLE f = INVALID_HANDLE_VALUE;
#ifndef COFFEE_WINDOWS_UWP
        f = CreateFile(
            url.c_str(),
            0,
            0,
            nullptr,
            OPEN_ALWAYS,
            FILE_ATTRIBUTE_NORMAL,
            nullptr);
#endif
        if(f != INVALID_HANDLE_VALUE)
        {
            GetLastError(); // to clear ERROR_ALREADY_EXISTS if it occurs
            return CloseHandle(f) == TRUE;
        } else
        {
            return false;
        }
    }
    case NodeType::Directory:
        return DirFun::MkDir(n, false);
    default:
        return false;
    }
}
bool WinFileFun::Rm(Url const& fn)
{
    auto url = *fn;

#ifdef COFFEE_WINDOWS_UWP
    CWString fn_w = StrUtil::convertformat<wbyte_t>(url);
    // return DeleteFile(&fn_w[0]);
    return false;
#else
    return DeleteFile(url.c_str());
#endif
}
WinFileFun::NodeType WinFileFun::Stat(Url const& f)
{
    auto url = *f;

    DWORD d = GetFileAttributesA(url.c_str());

    if(d != INVALID_FILE_ATTRIBUTES)
    {
        if(d & FILE_ATTRIBUTE_DIRECTORY)
        {
            return NodeType::Directory;
        } else if(d != 0)
        {
            return NodeType::File;
        }
    }
    return NodeType::None;
}

WinFileFun::FileMapping WinFileFun::Map(
    Url const& fn, ResourceAccess acc, szptr off, szptr size, int* err)
{
    auto url = *fn;

    if(feval(fn.flags & ResourceAccess::AssetFile) &&
       !feval(acc, ResourceAccess::WriteOnly))
    {
        FileMapping fm = {};

        HRSRC rsc = open_rsc(url.c_str());

#ifndef COFFEE_WINDOWS_UWP
        if(rsc && off + size <= SizeofResource(nullptr, rsc))
        {
            HGLOBAL lsrc = LoadResource(nullptr, rsc);
            fm.size      = SizeofResource(nullptr, rsc);
            fm.data      = &(C_RCAST<byte_t*>(LockResource(lsrc)))[off];
            fm.assignAccess(acc);
        }
#endif

        return fm;
    }

    if(off + size > Size(fn))
        return {};

    HANDLE fh = WinFileApi::GetFileHandle(fn, acc);

    if(fh == INVALID_HANDLE_VALUE)
    {
        *err = GetLastError();
        return {};
    }

    DWORD profl = WinFileApi::GetMappingFlags(acc);

    szptr         offsize = off + size;
    LARGE_INTEGER offsize_;
    offsize_.QuadPart = offsize;

    HANDLE mh = nullptr;
#ifndef COFFEE_WINDOWS_UWP
    mh = CreateFileMapping(
        fh, nullptr, profl, offsize_.HighPart, offsize_.LowPart, nullptr);
#else
    // mh = CreateFIleMappingFromApp(fh,nullptr,profl,offsize,nullptr);
#endif

    if(!mh)
    {
        *err = GetLastError();
        CloseHandle(fh);
        return {};
    }

    DWORD view_fl = WinFileApi::GetMappingViewFlags(acc);

    LARGE_INTEGER off_;
    off_.QuadPart = off;

    void* ptr = nullptr;
#ifndef COFFEE_WINDOWS_UWP
    ptr = MapViewOfFile(mh, view_fl, off_.HighPart, off_.LowPart, size);
#else
    // ptr = MapViewOfFileFromApp(mh,view_fl,off_.QuadPart,size);
#endif

    if(!ptr)
    {
        *err = GetLastError();
        CloseHandle(fh);
        CloseHandle(mh);
        return {};
    }

    FileMapping fm;
    fm.file    = fh;
    fm.mapping = mh;
    fm.size    = size;
    fm.data    = C_RCAST<byte_t*>(ptr);
    fm.assignAccess(acc);

    return fm;
}
bool WinFileFun::Unmap(WinFileFun::FileMapping&& fp)
{
    if(!fp.mapping)
        return true;
    if(!UnmapViewOfFile(fp.data))
        return false;
    if(!CloseHandle(fp.mapping))
        return false;
    if(!CloseHandle(fp.file))
        return false;

    return true;
}
WinFileFun::ScratchBuf WinFileFun::ScratchBuffer(szptr size, ResourceAccess acc)
{
    ScratchBuf b = {};
    b.assignAccess(acc);

    DWORD         fl1 = WinFileApi::GetMappingFlags(acc);
    LARGE_INTEGER s;
    s.QuadPart = size;
#ifndef COFFEE_WINDOWS_UWP
    b.mapping = CreateFileMapping(
        INVALID_HANDLE_VALUE, nullptr, fl1, s.HighPart, s.LowPart, nullptr);
#else
    b.mapping = nullptr;
#endif

    if(!b.mapping)
    {
        CString err = win_strerror(GetLastError());
        return {};
    }

    DWORD fl2 = WinFileApi::GetMappingViewFlags(acc);

#ifndef COFFEE_WINDOWS_UWP
    b.data = C_RCAST<byte_t*>(MapViewOfFile(b.mapping, fl2, 0, 0, size));
#else
    b.data    = nullptr;
#endif

    if(!b.data)
    {
        CString err = win_strerror(GetLastError());
        CloseHandle(b.mapping);
        return {};
    }

    b.size = size;

    return b;
}
void WinFileFun::ScratchUnmap(WinFileFun::ScratchBuf&& buf)
{
    UnmapViewOfFile(buf.data);
    CloseHandle(buf.mapping);
}

bool WinDirFun::MkDir(Url const& dname, bool parent)
{
    auto url = *dname;

    if(!parent)
    {
        BOOL stat = CreateDirectoryA(url.c_str(), nullptr);

        if(stat == FALSE && GetLastError() == ERROR_ALREADY_EXISTS)
            return true;
        else
            return stat == TRUE;
    }

    char   tmp[255];
    char*  p = NULL;
    size_t len;

    snprintf(tmp, sizeof(tmp), "%s", url.c_str());
    len = strlen(tmp);
    if(tmp[len - 1] == '/')
        tmp[len - 1] = 0;
    for(p = tmp + 1; *p; p++)
        if(*p == '/')
        {
            *p = 0;
            CreateDirectoryA(tmp, nullptr);
            *p = '/';
        }
    BOOL stat = CreateDirectoryA(tmp, nullptr);

    if(stat == FALSE && GetLastError() == ERROR_ALREADY_EXISTS)
        return true;
    else
        return stat == TRUE;
}

bool WinDirFun::RmDir(Url const& fn)
{
    CString deref = *fn;

    return RemoveDirectoryA(deref.c_str()) == TRUE;
}

bool WinDirFun::Ls(Url const& fn, DirList& outl)
{
    CString deref = *fn;

    WIN32_FIND_DATAA ffd;

    auto hnd = FindFirstFileA(deref.c_str(), &ffd);

    if(hnd == INVALID_HANDLE_VALUE)
        return false;

    do
    {
        outl.push_back({ffd.cFileName,
                        (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
                            ? DirItem_t::Type::Directory
                            : DirItem_t::Type::File});

    } while(FindNextFileA(hnd, &ffd) != FALSE);

    if(GetLastError() != ERROR_NO_MORE_FILES)
        return false;

    FindClose(hnd);

    return true;
}

} // namespace Windows
} // namespace Coffee