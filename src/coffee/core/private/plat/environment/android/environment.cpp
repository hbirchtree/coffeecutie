#include <coffee/core/plat/environment/android/environment.h>

#ifdef COFFEE_ANDROID

#ifdef COFFEE_USE_SDL2
#include <SDL_system.h>
#else
#include <coffee/android/android_main.h>
#endif

namespace Coffee{
namespace Environment{
namespace Android{

Url AndroidEnv::GetUserData(cstring,cstring)
{
#ifdef COFFEE_USE_SDL2
    auto path = SDL_AndroidGetInternalStoragePath();
    if(path)
        return path;
    else
        return {};
#else
    AndroidForeignCommand cmd;

    cmd.type = Android_QueryDataPath;

    CoffeeForeignSignalHandleNA(CoffeeForeign_RequestPlatformData,
                                &cmd, nullptr, nullptr);

    return MkUrl(cmd.store_string, RSCA::SystemFile);
#endif
}

}
}
}

#endif
