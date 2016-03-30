#include <coffee/core/plat/file/android/file.h>

#ifdef COFFEE_ANDROID

#include <SDL2/SDL_system.h>

namespace Coffee{
namespace CResources{
namespace Android{

CString AndroidFileFun::NativePath(cstring fn)
{
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

}

bool AndroidFileFun::Close(FileHandle *fh)
{

}

CByteData AndroidFileFun::Read(FileHandle *fh, uint64 size, bool nterminate)
{

}

CByteData AndroidFileFun::Write(FileHandle *fh, const CByteData &d, bool)
{

}

}
}
}
#endif
