#pragma once

#include <coffee/core/types/tdef/integertypes.h>
#include <coffee/core/types/tdef/stltypes.h>

#include <coffee/core/base/renderer/eventapplication_wrapper.h>
#include <coffee/jni/jnipp.h>

#include <asset_manager.h>
#include <sensor.h>
#include <jni.h>

struct android_app;
struct AAsset;
struct AAssetManager;

namespace android {

struct jni_error_category : Coffee::error_category
{
    virtual const char* name() const noexcept;
    virtual std::string message(int error_code) const;
};

using jni_error_code = Coffee::domain_error_code<jint, jni_error_category>;

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
//        javaVm->DetachCurrentThread();
    }

    JNIEnv* env()
    {
        return jniEnvironment;
    }
};

} // namespace android

namespace jnipp {

extern android::ScopedJNI* SwapJNI(android::ScopedJNI* jniScope);

extern JavaVM* GetVM();

} // namespace JNIPP

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
