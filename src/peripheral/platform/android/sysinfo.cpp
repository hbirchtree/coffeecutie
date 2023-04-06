#include <platforms/android/sysinfo.h>

#include <coffee/android/android_main.h>
#include <coffee/jni/jnipp.h>

using re = ::jnipp::return_type;
using namespace ::jnipp_operators;

namespace platform::info::os::android {

std::optional<std::string> name()
{
    return "Android";
}

std::optional<std::string> version()
{
    jnipp::java::object fieldValue
        = *"android.os.Build$VERSION"_jclass["RELEASE"_jfield.as(
            "java.lang.String")];
    return jnipp::java::type_unwrapper<std::string>(fieldValue);
}

} // namespace platform::info::os::android

namespace platform::info::device::android {

std::optional<std::pair<std::string, std::string>> device()
{
    jnipp::java::object brand
        = *"android.os.Build"_jclass["BRAND"_jfield.as(
            "java.lang.String")];
    jnipp::java::object model
        = *"android.os.Build"_jclass["MODEL"_jfield.as(
            "java.lang.String")];
    return std::pair<std::string, std::string>(
        jnipp::java::type_unwrapper<std::string>(brand),
        jnipp::java::type_unwrapper<std::string>(model));
}

std::optional<std::pair<std::string, std::string>> motherboard()
{
    if(::android::app_info().sdk_version() >= 31)
    {
        jnipp::java::object manufacturer
            = *"android.os.Build"_jclass["SOC_MANUFACTURER"_jfield.as(
                "java.lang.String")];
        jnipp::java::object board
            = *"android.os.Build"_jclass["SOC_MODEL"_jfield.as(
                "java.lang.String")];
        return std::pair<std::string, std::string>(
            jnipp::java::type_unwrapper<std::string>(manufacturer),
            jnipp::java::type_unwrapper<std::string>(board));
    } else
    {

        jnipp::java::object manufacturer
            = *"android.os.Build"_jclass["MANUFACTURER"_jfield.as(
                "java.lang.String")];
        jnipp::java::object board
            = *"android.os.Build"_jclass["BOARD"_jfield.as(
                "java.lang.String")];
        return std::pair<std::string, std::string>(
            jnipp::java::type_unwrapper<std::string>(manufacturer),
            jnipp::java::type_unwrapper<std::string>(board));
    }
}

std::optional<std::pair<std::string, std::string>> chassis()
{
    return std::nullopt;
}

} // namespace platform::info::device::android

