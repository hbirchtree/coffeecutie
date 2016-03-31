#include <coffee/core/plat/file/android/file.h>

#ifdef COFFEE_ANDROID

#include <coffee/core/CDebug>
#include "../file_abstraction.h"

#include <SDL2/SDL_system.h>
#include <SDL2/SDL_rwops.h>

namespace Coffee{
namespace CResources{
namespace Android{

struct AndroidFileApi::SDLData
{
    cstring fn;
};

CString AndroidFileFun::NativePath(cstring fn)
{
    if(AssetApi::GetAsset(fn))
        return fn;

    CString prefix;
    if(!(SDL_AndroidGetExternalStorageState()&SDL_ANDROID_EXTERNAL_STORAGE_READ))
    {
        /* Failure! */
    }

    prefix = SDL_AndroidGetExternalStoragePath();

    if(prefix.size()==0)
        return CString(fn);

    prefix.append("/");
    return prefix + fn;
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

    return fh;
}

bool AndroidFileFun::Close(FileHandle *fh)
{
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
        SDL_RWops* rdev = SDL_RWFromFile(check,"rb");
        if(!rdev || SDL_RWsize(rdev)==0)
            return {};
        CByteData data;
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
        return data;
    }else
        return Ancestor::Read(fh,size,nterminate);
}

bool AndroidFileFun::Write(FileHandle *fh, const CByteData &d, bool)
{
    cstring check = AssetApi::GetAsset(fh->extra_data->fn);
    if(check)
    {
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
    }else
        return Ancestor::Write(fh,d,false);
}

szptr AndroidFileFun::Size(AndroidFileFun::FileHandle *fh)
{
    cstring check = AssetApi::GetAsset(fh->extra_data->fn);
    if(check)
    {
        return Size(fh->extra_data->fn);
    }else
        return Ancestor::Size(fh);
}

szptr AndroidFileFun::Size(cstring fn)
{
    cstring check = AssetApi::GetAsset(fn);
    if(check)
    {
        SDL_RWops* sdev = SDL_RWFromFile(check,"rb");
        if(!sdev)
            return 0;

        szptr size = SDL_RWsize(sdev);

        SDL_RWclose(sdev);

        return size;
    }else
        return Ancestor::Size(fn);
}

}
}
}
#endif
