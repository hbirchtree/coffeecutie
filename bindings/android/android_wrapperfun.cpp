#include <coffee/android/android_main.h>
#include <android_native_app_glue.h>

using namespace Coffee;

int32 Coffee_AndroidGetApiVersion()
{
    return coffee_app->activity->sdkVersion;
}

CString Coffee_AndroidGetRelease()
{
    JNIEnv* env = coffee_app->activity->env;
    jclass buildcl = env->FindClass("/android/os/Build/VERSION");

    return "";
}

CString Coffee_AndroidGetBoard()
{
    javavar brd = Coffee_JavaGetStaticMember("android/os/Build","BOARD",
                                             "Ljava/lang/String;");
    if(!brd.env)
        return "!";
    jstring str = (jstring)brd.env->GetStaticObjectField(brd.ass,brd.field);
    return brd.env->GetStringUTFChars(str,0);
}
