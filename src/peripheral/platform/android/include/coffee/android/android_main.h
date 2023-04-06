#pragma once

#include <peripherals/libc/types.h>
#include <peripherals/stl/types.h>
#include <url/url.h>

#include <coffee/jni/jnipp.h>

#include <android_native_app_glue.h>

#include <jni.h>

struct android_app;
struct AAsset;
struct AAssetManager;
struct AInputQueue;
struct ALooper;
struct ANativeWindow;
struct ANativeActivity;
struct AConfiguration;

using app_cmd_t = decltype(APP_CMD_START);

namespace android {

using libc_types::f32;
using libc_types::u32;

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

        auto envcode
            = javaVm->GetEnv(C_RCAST<void**>(&jniEnvironment), JNI_VERSION_1_6);

        switch(envcode)
        {
        case JNI_EDETACHED:
            break;
        case JNI_OK:
            return;
        default: {
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
    int         sdk_version();

    platform::url::Url                data_path();
    platform::url::Url                cache_path();
    platform::url::Url                external_data_path();
    std::optional<platform::url::Url> obb_path();

    std::optional<::jnipp::java::object> get_service(
        std::string const& service);

    ANativeActivity* activity() const;
    AConfiguration*  configuration() const;
    AInputQueue*     input_queue() const;
    ALooper*         looper() const;
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
        ANativeWindow*   window{nullptr};
        ::jobject        activity_object{};
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

struct display_info
{
    enum hdr_mode_t
    {
        none         = 0x0,
        dolby_vision = 0x1,
        hdr10        = 0x2,
        hlg          = 0x4,
        hdr10_plus   = 0x8,
    };
    enum rotation_t
    {
        portrait_0,
        landscape_90,
        portrait_180,
        landscape_270,
    };
    struct insets_t
    {
        f32 top, bottom, left, right;
    };

    hdr_mode_t hdr_modes();
    bool     is_low_latency();
    bool     is_wide_gamut();

    std::optional<insets_t> safe_insets();
    rotation_t rotation();

    f32 dpi();
    f32 refresh_rate();
};

C_FLAGS(display_info::hdr_mode_t, u32);

extern std::vector<std::string> cpu_abis();

} // namespace android

namespace jnipp {

extern android::ScopedJNI* SwapJNI(android::ScopedJNI* jniScope);

extern JavaVM* GetVM();

} // namespace jnipp
