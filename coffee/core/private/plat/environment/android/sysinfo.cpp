#include <coffee/core/plat/environment/android/sysinfo.h>

#include <coffee/core/CDebug>

#include <jni.h>

#if !defined(COFFEE_USE_SDL2)
#include <coffee/android/android_main.h>
#else
#include <SDL_system.h>
#endif

namespace Coffee{
namespace Environment{
namespace Linux{
extern CString get_kern_arch();
}
namespace Android{


JNIEnv* jni_getEnv()
{
#if defined(COFFEE_USE_SDL2)
    cVerbose(7, "SDL_AndroidGetJNIEnv: {0}", c_cptr(SDL_AndroidGetJNIEnv));
    return C_CAST<JNIEnv*>(SDL_AndroidGetJNIEnv());
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

        jstring stringVal = C_CAST<jstring>(env->GetStaticObjectField(baseClass,stringField));
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
#if defined(COFFEE_USE_SDL2)
    JNIEnv* env = jni_getEnv();

    cVerbose(7, "JNIEnv: {0}", c_cptr(env));

    do {
        /*
         * Reference:
         * http://stackoverflow.com/questions/10196361/how-to-check-the-device-running-api-level-using-c-code-via-ndk
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
#else
    AndroidForeignCommand cmd;

    cmd.type = Android_QueryAPI;

    CoffeeForeignSignalHandleNA(CoffeeForeign_RequestPlatformData,
                                &cmd, nullptr,  nullptr);

    return "Android API " + cast_pod(cmd.data.scalarI64);
#endif
}

//HWDeviceInfo AndroidSysInfo::Processor()
//{
//    CString model = Mem::StrUtil::propercase(jni_getString("android/os/Build","BRAND")) + " ";
//    model += Mem::StrUtil::propercase(jni_getString("android/os/Build","HARDWARE"));
//    return HWDeviceInfo(model,
//                        "0x0");
//}

HWDeviceInfo AndroidSysInfo::DeviceName()
{
#if defined(COFFEE_USE_SDL2)
    return HWDeviceInfo(
                Mem::StrUtil::propercase(jni_getString("android/os/Build","BRAND")),
                jni_getString("android/os/Build","MODEL"),
                jni_getString("android/os/Build","BOOTLOADER"),
                jni_getString("android/os/Build","SERIAL"));
#else
    AndroidForeignCommand cmd;

    cmd.type = Android_QueryDeviceBoardName;

    cmd.type = Android_QueryDeviceBrand;
    CoffeeForeignSignalHandleNA(CoffeeForeign_RequestPlatformData,
                                &cmd, nullptr,  nullptr);

    auto brand = cmd.store_string;

    cmd.type = Android_QueryDeviceName;
    CoffeeForeignSignalHandleNA(CoffeeForeign_RequestPlatformData,
                                &cmd, nullptr,  nullptr);

    auto product = cmd.store_string;

    return HWDeviceInfo(brand, product, "0x0");
#endif
}

HWDeviceInfo AndroidSysInfo::Motherboard()
{
#if defined(COFFEE_USE_SDL2)
    return HWDeviceInfo("", "", "")
#else
    AndroidForeignCommand cmd;

    cmd.type = Android_QueryDeviceBoardName;

    CoffeeForeignSignalHandleNA(CoffeeForeign_RequestPlatformData,
                                &cmd, nullptr,  nullptr);

    auto board = cmd.store_string;

    return HWDeviceInfo(board, "", "");
#endif
}

}
}
}
