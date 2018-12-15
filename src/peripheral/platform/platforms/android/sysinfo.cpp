#include <platforms/android/sysinfo.h>

#include <jni.h>

extern "C" {
#include <cpu-features.h>
}

//#include <coffee/android/android_main.h>

namespace platform {
namespace env {
namespace Linux {
extern CString get_kern_arch();
}
namespace android {

JNIEnv* jni_getEnv()
{
    /* TODO: Add native_app_glue implementation */
    return nullptr;
}

CString jni_getString(cstring cname, cstring field)
{
    JNIEnv* env = jni_getEnv();

    do
    {
        if(!env)
            break;

        jclass baseClass = env->FindClass(cname);
        if(!baseClass)
            break;

        jfieldID stringField =
            env->GetStaticFieldID(baseClass, field, "Ljava/lang/String;");
        if(!stringField)
            break;

        jstring stringVal =
            C_CAST<jstring>(env->GetStaticObjectField(baseClass, stringField));
        if(!stringVal)
            break;

        cstring outStr = env->GetStringUTFChars(stringVal, nullptr);

        CString output(outStr);

        env->ReleaseStringUTFChars(stringVal, outStr);

        env->ExceptionCheck();

        return output;
    } while(false);

    return "";
}

CString SysInfo::GetSystemVersion()
{
    AndroidForeignCommand cmd;

    cmd.type = Android_QueryAPI;

    CoffeeForeignSignalHandleNA(
        CoffeeForeign_RequestPlatformData, &cmd, nullptr, nullptr);

    return "Android API " + cast_pod(cmd.data.scalarI64);
}

HWDeviceInfo SysInfo::DeviceName()
{
    AndroidForeignCommand cmd;

    cmd.type = Android_QueryDeviceBoardName;

    cmd.type = Android_QueryDeviceBrand;
    CoffeeForeignSignalHandleNA(
        CoffeeForeign_RequestPlatformData, &cmd, nullptr, nullptr);

    auto brand = cmd.store_string;

    cmd.type = Android_QueryDeviceName;
    CoffeeForeignSignalHandleNA(
        CoffeeForeign_RequestPlatformData, &cmd, nullptr, nullptr);

    auto product = cmd.store_string;

    return HWDeviceInfo(brand, product, "0x0");
}

HWDeviceInfo SysInfo::Motherboard()
{
    AndroidForeignCommand cmd;

    cmd.type = Android_QueryDeviceBoardName;

    CoffeeForeignSignalHandleNA(
        CoffeeForeign_RequestPlatformData, &cmd, nullptr, nullptr);

    auto board = cmd.store_string;

    return HWDeviceInfo(board, "", "");
}

u32 SysInfo::CoreCount()
{
    return C_FCAST<u32>(android_getCpuCount());
}

ThrdCnt SysInfo::ThreadCount()
{
    return C_FCAST<u32>(android_getCpuCount());
}

} // namespace android
} // namespace env
} // namespace platform
