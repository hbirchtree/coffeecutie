#include <coffee/core/plat/file/android/file.h>

#include <coffee/core/CDebug>
#include "../file_abstraction.h"

#if !defined(ANDROID_DONT_USE_SDL2)
#include <SDL2/SDL_system.h>
#else
#include <coffee/android/android_main.h>
#endif

namespace Coffee{
namespace CResources{
namespace Android{

struct AndroidFileApi::SDLData
{
#if defined(ANDROID_DONT_USE_SDL2)
    AAsset* fp;
#endif
    cstring fn;
};

CString AndroidFileFun::NativePath(cstring fn)
{
    cVerbose(6,"Input filename: {0}",fn);

    if(!fn)
        return "";

    if(fn[0] == '/')
        return fn;

    if(AssetApi::GetAsset(fn))
    {
        cVerbose(6,"File {0} identified as asset",fn);
        return fn;
    }

    CString prefix;

    cVerbose(6,"ExternalStorageState() function pointer: {0}",(uint64)SDL_AndroidGetExternalStorageState);

#if !defined(ANDROID_DONT_USE_SDL2)
    if(SDL_AndroidGetExternalStorageState() == 0 ||
            !(SDL_AndroidGetExternalStorageState()&SDL_ANDROID_EXTERNAL_STORAGE_READ))
    {
        /* Failure! */
        cVerbose(6,"Android SDL external storage status failed");
        if(SDL_AndroidGetInternalStoragePath())
            prefix = SDL_AndroidGetInternalStoragePath();
    }else{
        if(SDL_AndroidGetExternalStoragePath())
            prefix = SDL_AndroidGetExternalStoragePath();
    }

    cVerbose(6,"Storage prefix: {0}",prefix);
#else
    prefix = Coffee_GetExternalDataPath();
#endif

    if(prefix.size()==0)
        return CString(fn);

    prefix.append("/");

    cVerbose(5,"Android native path: {0}",prefix+fn);

    return prefix + fn;
}

CString AndroidFileFun::NativePath(cstring fn, ResourceAccess storage)
{
    if(feval(storage,ResourceAccess::TemporaryFile))
        return Env::ConcatPath("/data/local/tmp",fn);
    else
        return NativePath(fn);
}

AndroidFileFun::FileHandle *AndroidFileFun::Open(cstring fn, ResourceAccess ac)
{
    FileHandle* fh = nullptr;
    if(AssetApi::GetAsset(fn))
        fh = new FileHandle;
    else
        fh = Ancestor::Open(fn,ac);

    if(!fh)
        return nullptr;

    fh->extra_data = new AndroidFileApi::SDLData;
    fh->extra_data->fn = fn;
#if defined(ANDROID_DONT_USE_SDL2)
    fh->extra_data->fp = Coffee_AssetGet(fn);
#endif

    return fh;
}

bool AndroidFileFun::Close(FileHandle *fh)
{
#if defined(ANDROID_DONT_USE_SDL2)
    Coffee_AssetClose(fh->extra_data->fp);
#endif

    if(fh->extra_data)
        delete fh->extra_data;
    delete fh;
}

CByteData AndroidFileFun::Read(FileHandle *fh, uint64 size, bool nterminate)
{
    if(!fh)
        return {};
    cstring check = AssetApi::GetAsset(fh->extra_data->fn);
    if(check)
    {
        /* In this case, the file exists as an asset */

        CByteData data;

#if !defined(ANDROID_DONT_USE_SDL2)
        SDL_RWops* rdev = SDL_RWFromFile(check,"rb");
        if(!rdev || SDL_RWsize(rdev)==0)
            return {};
        data.size = SDL_RWsize(rdev);

        if(data.size != size)
        {
            SDL_RWclose(rdev);
            return {};
        }

        data.data = (byte_t*)Alloc(data.size);

        if(SDL_RWread(rdev,data.data,1,data.size)!=data.size)
        {
            SDL_RWclose(rdev);
            CFree(data.data);
            return {};
        }

        SDL_RWclose(rdev);
#else
        data.size = Coffee_AssetGetSize(fh->extra_data->fp);
        /* NOTE: Be aware! You might fuck sh*t up real bad. */
        data.data = (byte_t*)Coffee_AssetGetPtr(fh->extra_data->fp);
#endif
        return data;
    }else
        return Ancestor::Read(fh,size,nterminate);
}

bool AndroidFileFun::Write(FileHandle *fh, const CByteData &d, bool)
{
    cstring check = AssetApi::GetAsset(fh->extra_data->fn);
    if(check)
    {
#if !defined(ANDROID_DONT_USE_SDL2)
        SDL_RWops* wdev = SDL_RWFromFile(check,"wb");
        if(!wdev)
            return false;

        szptr outsize = SDL_RWwrite(wdev,d.data,1,d.size);
        SDL_RWclose(wdev);
        if(outsize != d.size)
        {
            return false;
        }else{
            return true;
        }
#else
        /* AAsset does not support writing to assets. Bad. */
        return false;
#endif
    }else
        return Ancestor::Write(fh,d,false);
}

szptr AndroidFileFun::Size(AndroidFileFun::FileHandle *fh)
{
    cstring check = AssetApi::GetAsset(fh->extra_data->fn);
    if(check)
    {
#if !defined(ANDROID_DONT_USE_SDL2)
        return Size(fh->extra_data->fn);
#else
        return Coffee_AssetGetSize(fh->extra_data->fp);
#endif
    }else
        return Ancestor::Size(fh);
}

szptr AndroidFileFun::Size(cstring fn)
{
    cstring check = AssetApi::GetAsset(fn);
    if(check)
    {
#if !defined(ANDROID_DONT_USE_SDL2)
        SDL_RWops* sdev = SDL_RWFromFile(check,"rb");
        if(!sdev)
            return 0;

        szptr size = SDL_RWsize(sdev);

        SDL_RWclose(sdev);
        return size;
#else
        return Coffee_AssetGetSizeFn(fn);
#endif

    }else
        return Ancestor::Size(fn);
}

}
}
}
