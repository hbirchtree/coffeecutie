#include <plat/platform_detect.h>

#if defined(COFFEE_ANDROID)

#include <jni.h>
#include <android/log.h>

namespace Coffee{

cstring GetInternalStorageDir()
{
    return nullptr;
}

cstring GetExternalStorageDir()
{
    return nullptr;
}

cstring GetUserDataDir()
{
    return nullptr;
}

cstring GetAppName()
{
    return nullptr;
}

}

#endif
