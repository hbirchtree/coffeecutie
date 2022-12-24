#pragma once

#include <coffee/core/libc_types.h>
#include <coffee/core/stl_types.h>
#include <url/url.h>

#include <coffee/foreign/foreign.h>
#include <coffee/jni/jnipp.h>

#include <jni.h>

struct android_app;
struct AAsset;
struct AAssetManager;
struct ANativeWindow;
struct ANativeActivity;
struct AConfiguration;

using app_cmd_t = decltype(APP_CMD_START);

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
    int sdk_version();

    platform::url::Url data_path();
    platform::url::Url cache_path();
    platform::url::Url external_data_path();
    std::optional<platform::url::Url> obb_path();

    std::optional<::jnipp::java::object> get_service(
        std::string const& service);

    ANativeActivity* activity() const;
    AConfiguration* configuration() const;
    AInputQueue* input_queue() const;
    ALooper* looper() const;
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

    std::optional<result_t> query(network_class net = network_class_mobile);
};

struct activity_manager
{
    struct memory_info
    {
        libc_types::u64 available{0}, threshold{0}, low_mem{0}, total{0};
    };

    struct config_info
    {
        struct
        {
            libc_types::i32 gles_version{0};
            libc_types::i32 input_features{0};
            libc_types::i32 keyboard_type{0};
            libc_types::i32 touchscreen{0};
        } req;
    };

    struct window_info
    {
        ANativeActivity* activity{nullptr};
        ANativeWindow* window{nullptr};
        ::jobject activity_object{};
    };

    std::optional<memory_info> get_mem_info();
    std::optional<config_info> get_config_info();

    std::optional<window_info> window();

    AAssetManager* asset_manager();

    bool clear_app_data();

    bool is_low_ram_device();
    bool is_test_harness();
    bool is_user_harness();
    bool is_monkey();
};

extern std::vector<std::string> cpu_abis();

extern int app_dpi();

} // namespace android

namespace jnipp {

extern android::ScopedJNI* SwapJNI(android::ScopedJNI* jniScope);

extern JavaVM* GetVM();

} // namespace jnipp
