#include <coffee/core/plat/environment/android/sysinfo.h>

#if defined(ANDROID_DONT_USE_SDL2)
#include <coffee/android/android_main.h>
#else
#include <jni.h>
#include <SDL_system.h>
#endif

namespace Coffee{
namespace Environment{
namespace Android{

JNIEnv* jni_getEnv()
{
#if !defined(ANDROID_DONT_USE_SDL2)
    return (JNIEnv*)SDL_AndroidGetJNIEnv();
#else
    /* TODO: Add native_app_glue implementation */
    return nullptr;
#endif
}

CString jni_getString(cstring cname, cstring field)
{
    JNIEnv* env = jni_getEnv();

    do{
        if(!env)
            break;

        jclass baseClass = env->FindClass(cname);
        if(!baseClass)
            break;

        jfieldID stringField = env->GetStaticFieldID(
                    baseClass,field,"Ljava/lang/String;");
        if(!stringField)
            break;

        jstring stringVal = (jstring)env->GetStaticObjectField(baseClass,stringField);
        if(!stringVal)
            break;

        cstring outStr = env->GetStringUTFChars(stringVal,nullptr);

        CString output(outStr);

        env->ReleaseStringUTFChars(stringVal,outStr);

        env->ExceptionCheck();

        return output;
    }while(false);
    return "";
}

CString AndroidSysInfo::GetSystemVersion()
{
    JNIEnv* env = jni_getEnv();

    do {
        /*
         * Reference: http://stackoverflow.com/questions/10196361/how-to-check-the-device-running-api-level-using-c-code-via-ndk
         */
        if(!env)
            break;

        if(env->ExceptionCheck())
            break;

        jclass versionClass = env->FindClass("android/os/Build$VERSION");
        if(!versionClass)
            break;

        jfieldID sdkIntFieldId = env->GetStaticFieldID(versionClass,"SDK_INT","I");
        if(!sdkIntFieldId)
        {
            break;
        }

        jint sdkVer = env->GetStaticIntField(versionClass,sdkIntFieldId);

        return jni_getString("android/os/Build$VERSION","RELEASE")
                + " (" + Mem::Convert::inttostring(sdkVer) + ")";

    }while(false);

    return "[0]";
}

HWDeviceInfo AndroidSysInfo::Processor()
{
    CString model = Mem::StrUtil::propercase(jni_getString("android/os/Build","BRAND")) + " ";
    model += Mem::StrUtil::propercase(jni_getString("android/os/Build","HARDWARE"));
    return HWDeviceInfo(model,
                        "0x0");
}

HWDeviceInfo AndroidSysInfo::DeviceName()
{
    return HWDeviceInfo(
                Mem::StrUtil::propercase(jni_getString("android/os/Build","BRAND")),
                jni_getString("android/os/Build","MODEL"),
                jni_getString("android/os/Build","BOOTLOADER"),
                jni_getString("android/os/Build","SERIAL"));
}


}
}
}
