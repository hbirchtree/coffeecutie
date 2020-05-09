#include <platforms/android/sysinfo.h>

#include <coffee/android/android_main.h>
#include <coffee/core/stl_types.h>
#include <jni.h>
#include <peripherals/stl/string_casting.h>
#include <platforms/file.h>

extern "C" {
#include <cpu-features.h>
}

//#include <coffee/android/android_main.h>

using namespace Coffee;

namespace platform {
namespace env {
namespace Linux {

extern CString get_kern_arch();

}
namespace android {

using LFileFun = file::Linux::FileFun;

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

    cmd.type = Android_QueryReleaseName;
    CoffeeForeignSignalHandleNA(
        CoffeeForeign_RequestPlatformData, &cmd, nullptr, nullptr);

    return cmd.store_string;
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

    cmd.type = Android_QueryReleaseName;
    CoffeeForeignSignalHandleNA(
        CoffeeForeign_RequestPlatformData, &cmd, nullptr, nullptr);

    auto version = cmd.store_string;

    return HWDeviceInfo(brand, product, version);
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

u16 PowerInfo::BatteryPercentage()
{
    using namespace ::jnipp_operators;

    auto BatteryManager    = "android.os.BatteryManager"_jclass;
    auto getProperty = "getIntProperty"_jmethod.arg<jint>()
                                     .ret<jint>();

    auto battery = *::android::app_info().get_service("batterymanager");

    auto values = BatteryManager(battery)[getProperty](4);

    return values.i;
}

PowerInfo::Temp PowerInfo::CpuTemperature()
{
    using namespace ::jnipp_operators;

    /* Note: CPU temps are not available anymore, get the battery temp I guess */

    auto BatteryManager    = "android.os.BatteryManager"_jclass;
    auto getProperty = "getIntProperty"_jmethod.arg<jint>()
                                     .arg<jint>()
                                     .ret<jfloatArray>();

    auto battery = *::android::app_info().get_service("batterymanager");

//    auto values = BatteryManager(battery)[getProperty](0);

    return {0.f, 0.f};
}

PowerInfo::Temp PowerInfo::GpuTemperature()
{
//    auto hw = *::android::app_info().get_service("hardware_properties");

    return {0.f, 0.f};
}

} // namespace android
} // namespace env
} // namespace platform
