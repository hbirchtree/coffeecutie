#ifndef COFFEE_CORE_PLAT_CFILE_H
#define COFFEE_CORE_PLAT_CFILE_H

#include <cstdio>

#include "file_def.h"
#include "../../coffee_strings.h"

//#ifdef COFFEE_ANDROID_FILE_ASSET_API
//#include <SDL2/SDL_system.h>
//#endif

namespace Coffee{
namespace CResources{

struct CommonFileFun : FileFunDef
{
    STATICINLINE CString NativePath(cstring fn)
    {
        CString temp = fn;
#if defined(COFFEE_WINDOWS)
        temp = CStrReplace(fn,"/","\\");
#endif
        return temp;
    }
};

struct FILEApi
{
    struct FileHandle
    {
        FileHandle():
            handle(nullptr)
        {
        }
        FILE* handle;
    };
};

template<typename FH>
struct CFILEFun_def : CommonFileFun
{
    using FileHandle = FILEApi::FileHandle;

    STATICINLINE FH* Open(cstring fn, ResourceAccess ac)
    {
        FH* fh = new FH;

        cstring mode = "";

        /* Because of the way masks work, feval must be run on each individual flag */
        if(feval(ac&ResourceAccess::ReadWrite) &&
                feval(ac&ResourceAccess::Append))
            mode = "a+";
        else if(feval(ac&ResourceAccess::WriteOnly) &&
                feval(ac&ResourceAccess::Append))
            mode = "a";
        else if(feval(ac&(ResourceAccess::ReadOnly)))
            mode = "r";
	else if(feval(ac&ResourceAccess::WriteOnly)&&
		feval(ac&ResourceAccess::Discard))
	    mode = "w+";
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
    STATICINLINE bool Close(FH* fh)
    {
        fclose(fh->handle);
        delete fh;
        return true;
    }

    STATICINLINE CByteData Read(FH* fh, uint64 size,bool nterminate)
    {
        CByteData data;
        data.elements = 0;
        szptr esize = Size(fh);
        if(size <= esize && size != -1)
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
    STATICINLINE bool Seek(FH* fh,uint64 off)
    {
        return fseek(fh->handle,off,SEEK_SET)==0;
    }
    STATICINLINE bool Write(FH* fh,CByteData const& d,bool)
    {
        szptr wsize = fwrite(d.data,sizeof(byte_t),d.size,fh->handle);
        return wsize==d.size;
    }

    STATICINLINE void* Map(cstring, ResourceAccess, szptr, szptr, int*)
    {
        return nullptr;
    }
    STATICINLINE bool Unmap(void*, szptr)
    {
        return false;
    }

    STATICINLINE szptr Size(FH* fh)
    {
        szptr offset = ftell(fh->handle);
        fseek(fh->handle,0,SEEK_END);
        szptr fsize = ftell(fh->handle);
        fseek(fh->handle,offset,SEEK_SET);
        return fsize;
    }
    STATICINLINE bool Rm(cstring)
    {
        return false;
    }
};

using CFILEFun = CFILEFun_def<FILEApi::FileHandle>;

}
}

#endif
