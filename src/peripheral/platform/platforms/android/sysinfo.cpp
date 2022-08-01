#include <platforms/android/sysinfo.h>

#include <coffee/android/android_main.h>
#include <coffee/jni/jnipp.h>

extern "C" {
#include <cpu-features.h>
}

using re = ::jnipp::return_type;
using namespace ::jnipp_operators;

namespace platform::info::os::android {

std::optional<std::string> name()
{
    return "Android";
}

std::optional<std::string> version()
{
    jnipp::java::object fieldValue =
        *"android.os.Build$VERSION"_jclass["RELEASE"_jfield.as(
            "java.lang.String")];
    return jnipp::java::type_unwrapper<std::string>(fieldValue);
}

} // namespace platform::info::os::android

namespace platform::info::device::android {

std::optional<std::pair<std::string, std::string>> device()
{
    return std::nullopt;
}

std::optional<std::pair<std::string, std::string>> motherboard()
{
    return std::nullopt;
}

std::optional<std::pair<std::string, std::string>> chassis()
{
    return std::nullopt;
}

} // namespace platform::info::device::android

// namespace platform {
// namespace env {
// namespace Linux {

// extern CString get_kern_arch();

//}
// namespace android {

// using LFileFun = file::Linux::FileFun;

// CString SysInfo::GetSystemVersion()
//{
//     AndroidForeignCommand cmd;

//    cmd.type = Android_QueryReleaseName;
//    CoffeeForeignSignalHandleNA(
//        CoffeeForeign_RequestPlatformData, &cmd, nullptr, nullptr);

//    return cmd.store_string;
//}

// HWDeviceInfo SysInfo::DeviceName()
//{
//     AndroidForeignCommand cmd;

//    cmd.type = Android_QueryDeviceBoardName;

//    cmd.type = Android_QueryDeviceBrand;
//    CoffeeForeignSignalHandleNA(
//        CoffeeForeign_RequestPlatformData, &cmd, nullptr, nullptr);

//    auto brand = cmd.store_string;

//    cmd.type = Android_QueryDeviceName;
//    CoffeeForeignSignalHandleNA(
//        CoffeeForeign_RequestPlatformData, &cmd, nullptr, nullptr);

//    auto product = cmd.store_string;

//    cmd.type = Android_QueryReleaseName;
//    CoffeeForeignSignalHandleNA(
//        CoffeeForeign_RequestPlatformData, &cmd, nullptr, nullptr);

//    auto version = cmd.store_string;

//    return HWDeviceInfo(brand, product, version);
//}

// HWDeviceInfo SysInfo::Motherboard()
//{
//     AndroidForeignCommand cmd;

//    cmd.type = Android_QueryDeviceBoardName;

//    CoffeeForeignSignalHandleNA(
//        CoffeeForeign_RequestPlatformData, &cmd, nullptr, nullptr);

//    auto board = cmd.store_string;

//    return HWDeviceInfo(board, "", "");
//}

// u32 SysInfo::CoreCount()
//{
//     return C_FCAST<u32>(android_getCpuCount());
// }

// ThrdCnt SysInfo::ThreadCount()
//{
//     return C_FCAST<u32>(android_getCpuCount());
// }

// u16 PowerInfo::BatteryPercentage()
//{
//     using namespace ::jnipp_operators;

//    auto BatteryManager = "android.os.BatteryManager"_jclass;
//    auto getProperty    =
//    "getIntProperty"_jmethod.arg<jint>().ret<re::int_>();

//    auto battery = *::android::app_info().get_service("batterymanager");

//    return BatteryManager(battery)[getProperty](4);
//}

// PowerInfo::Temp PowerInfo::CpuTemperature()
//{
//     using namespace ::jnipp_operators;

//    /* Note: CPU temps are not available anymore, get the battery temp I guess
//     */

//    auto BatteryManager = "android.os.BatteryManager"_jclass;
//    auto getProperty    = "getIntProperty"_jmethod.arg<jint>()
//                           .arg<jint>()
//                           .ret<re::int_>();

//    auto battery = *::android::app_info().get_service("batterymanager");

//    //    auto values = BatteryManager(battery)[getProperty](0);

//    return {0.f, 0.f};
//}

// PowerInfo::Temp PowerInfo::GpuTemperature()
//{
//     //    auto hw =
//     *::android::app_info().get_service("hardware_properties");

//    return {0.f, 0.f};
//}

//} // namespace android
//} // namespace env
//} // namespace platform
