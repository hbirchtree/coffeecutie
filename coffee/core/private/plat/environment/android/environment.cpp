#pragma once

#include <coffee/core/plat/environment/android/environment.h>

#ifdef COFFEE_ANDROID

#include <SDL2/SDL_system.h>

namespace Coffee{
namespace Environment{
namespace Android{

CString AndroidEnv::GetUserData(cstring,cstring)
{
    return SDL_AndroidGetInternalStoragePath();
}

}
}
}

#endif