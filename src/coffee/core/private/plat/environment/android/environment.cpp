#include <coffee/core/plat/environment/android/environment.h>

#ifdef COFFEE_ANDROID

#include <coffee/android/android_main.h>

namespace Coffee{
namespace Environment{
namespace Android{

Url AndroidEnv::GetUserData(cstring,cstring)
{
    AndroidForeignCommand cmd;

    cmd.type = Android_QueryDataPath;

    CoffeeForeignSignalHandleNA(CoffeeForeign_RequestPlatformData,
                                &cmd, nullptr, nullptr);

    return MkUrl(cmd.store_string, RSCA::SystemFile);
}

}
}
}

#endif
