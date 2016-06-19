#include <coffee/core/plat/environment/android/environment.h>

#ifdef COFFEE_ANDROID

#ifndef ANDROID_DONT_USE_SDL2
#include <SDL2/SDL_system.h>
#else
#include <coffee/android/android_main.h>
#endif

namespace Coffee{
namespace Environment{
namespace Android{

CString AndroidEnv::GetUserData(cstring,cstring)
{
#ifndef ANDROID_DONT_USE_SDL2
    return SDL_AndroidGetInternalStoragePath();
#else
    return Coffee_GetDataPath();
#endif
}

}
}
}

#endif
