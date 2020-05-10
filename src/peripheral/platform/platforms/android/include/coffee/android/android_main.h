#pragma once

#include <coffee/core/libc_types.h>
#include <coffee/core/stl_types.h>

#include <coffee/foreign/foreign.h>
#include <coffee/jni/jnipp.h>

#include <android/asset_manager.h>
#include <android/sensor.h>
#include <jni.h>

struct android_app;
struct AAsset;
struct AAssetManager;

namespace android {

struct jni_error_category : stl_types::error_category
{
    virtual const char* name() const noexcept;
    virtual std::string message(int error_code) const;
};

using jni_error_code = stl_types::domain_error_code<jint, jni_error_category>;

struct ScopedJNI
{
    JNIEnv* jniEnvironment;
    JavaVM* javaVm;

    ScopedJNI(JavaVM* jvm) : javaVm(jvm)
    {
        jni_error_code ec;

        auto envcode =
            javaVm->GetEnv(C_RCAST<void**>(&jniEnvironment), JNI_VERSION_1_6);

        switch(envcode)
        {
        case JNI_EDETACHED:
            break;
        case JNI_OK:
            return;
        default:
        {
            ec = envcode;
            C_ERROR_CHECK(ec);
            return;
        }
        }

        auto code = javaVm->AttachCurrentThread(&jniEnvironment, nullptr);

        if(code != JNI_OK)
        {
            ec = code;
            C_ERROR_CHECK(ec);
        }
    }

    ~ScopedJNI()
    {
    }

    JNIEnv* env()
    {
        return jniEnvironment;
    }
};

struct intent
{
    intent();

    std::string action();

    std::string data();

    std::set<std::string> categories();

    std::map<std::string, std::string> extras();

    int flags();

  private:
    jnipp::wrapping::jobject m_intent;
};

struct app_info
{
    std::string package_name();

    stl_types::Optional<::jobject> get_service(std::string const& service);
};

struct network_stats
{
    enum network_class
    {
        network_class_mobile = 0,
        network_class_wifi   = 1,
    };

    struct result_t
    {
        result_t(libc_types::u64 rx = 0, libc_types::u64 tx = 0) :
            rx(rx), tx(tx)
        {
        }

        libc_types::u64 rx, tx;
    };

    stl_types::Optional<result_t> query();
};

extern std::vector<std::string> cpu_abis();

extern int app_dpi();

} // namespace android

namespace jnipp {

extern android::ScopedJNI* SwapJNI(android::ScopedJNI* jniScope);

extern JavaVM* GetVM();

} // namespace jnipp

namespace Coffee {

extern struct android_app* coffee_app;

enum FCmdType
{
    Android_QueryNull,

    /* System information queries */
    Android_QueryAPI,
    Android_QueryReleaseName,
    Android_QueryDeviceBoardName,
    Android_QueryDeviceBrand,
    Android_QueryDeviceName,
    Android_QueryDeviceManufacturer,
    Android_QueryDeviceProduct,

    /* Low-level information */
    Android_QueryPlatformABIs,
    Android_QueryBuildType,

    /* Hardware information */
    Android_QueryMaxMemory,
    Android_QueryDeviceDPI,

    /* System-level functionality */
    Android_QueryNativeWindow,
    Android_QueryAssetManager,
    Android_QueryActivity,
    Android_QueryApp,

    /* Filesystem queries */
    Android_QueryDataPath,
    Android_QueryExternalDataPath,
    Android_QueryObbPath,
    Android_QueryCachePath,

    Android_QueryStartActivity,
};

struct AndroidForeignCommand
{
    FCmdType type;

    union
    {
        u64       scalarU64;
        i64       scalarI64;
        scalar    scalarF32;
        bigscalar scalarF64;
        cstring   ptr_string;
        void*     ptr;
    } data;

    CString store_string;
};

} // namespace Coffee
