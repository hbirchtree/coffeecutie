#ifndef COFFEE_CORE_PLAT_CFILE_H
#define COFFEE_CORE_PLAT_CFILE_H

#include "file_def.h"
#include <cstdio>
#include <coffee_strings.h>

#ifdef COFFEE_ANDROID_FILE_ASSET_API
#include <SDL2/SDL_system.h>
#endif

namespace Coffee{
namespace CResources{

struct PlatFileFun : FileFunDef
{
    struct FileHandle
    {
        FILE* handle;
    };

    static CString NativePath(cstring fn)
    {
        CString temp = fn;
#if defined(COFFEE_WINDOWS)
        temp = c_str_replace(fn,"/","\\");
#elif defined(COFFEE_ANDROID_FILE_ASSET_API)
        return "";
#endif
        return temp;
    }

    static FileHandle* Open(cstring fn, ResourceAccess ac)
    {
        FileHandle* fh = new FileHandle;

        cstring mode = "";

        if(feval(ac&(ResourceAccess::ReadWrite|ResourceAccess::Append)))
            mode = "a+";
        else if(feval(ac&(ResourceAccess::WriteOnly|ResourceAccess::Append)))
            mode = "a";
        else if(feval(ac&(ResourceAccess::ReadOnly)))
            mode = "r";
        else if(feval(ac&(ResourceAccess::WriteOnly)))
            mode = "w";
        else if(feval(ac&(ResourceAccess::ReadWrite)))
            mode = "r+";

        CString fn_native = NativePath(fn);
        fh->handle = fopen(fn_native.c_str(),mode);

        if(!fh->handle)
        {
            delete fh;
            return nullptr;
        }

        return fh;
    }
    static bool Close(FileHandle* fh)
    {
        int status = fclose(fh->handle);
        if(status!=0)
            return false;
        delete fh;
        return true;
    }

    static CByteData Read(FileHandle* fh, uint64 size,bool nterminate)
    {
        CByteData data;
        data.elements = 0;
        szptr esize = Size(fh);
        if(size < esize && size != -1)
            esize = size;

        if(nterminate)
            data.size = esize+1;
        else
            data.size = esize;
        data.data = (byte_t*)Alloc(data.size);
        szptr rsize = fread(data.data,sizeof(byte_t),esize-1,fh->handle);
        if(nterminate)
            data.data[esize-1] = 0;
//        if(rsize<esize)
//            cLog(CFStrings::Plat_File_Native_SizeErr,esize,rsize);
        return data;
    }
    static bool Seek(FileHandle* fh,uint64 off)
    {
        return fseek(fh->handle,off,SEEK_END)==0;
    }
    static bool Write(FileHandle* fh,CByteData const& d,bool)
    {
        szptr wsize = fwrite(d.data,sizeof(byte_t),d.size,fh->handle);
        return wsize==d.size;
    }

    static void* Map(cstring, ResourceAccess, szptr, szptr, int*)
    {
        return nullptr;
    }
    static bool Unmap(void*, szptr)
    {
        return false;
    }

    static szptr Size(FileHandle* fh)
    {
        szptr offset = ftell(fh->handle);
        fseek(fh->handle,0,SEEK_END);
        szptr fsize = ftell(fh->handle);
        fseek(fh->handle,offset,SEEK_END);
        return fsize;
    }
    static bool Rm(cstring)
    {
        return false;
    }
};

}
}

#endif