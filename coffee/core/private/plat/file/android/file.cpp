#include <coffee/core/plat/file/android/file.h>
#ifdef COFFEE_ANDROID

#include <SDL2/SDL_system.h>

namespace Coffee{
namespace CResources{

CString AndroidFileFun::NativePath(cstring fn)
{
    CString temp = fn;
    return temp;
}

}
}
#endif