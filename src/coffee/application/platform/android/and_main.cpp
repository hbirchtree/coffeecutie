#include <coffee/android/android_main.h>

#include <coffee/anative/anative_comp.h>
#include <coffee/comp_app/bundle.h>
#include <coffee/core/coffee.h>
#include <coffee/core/profiler/profiling-export.h>
#include <coffee/core/types/point.h>
#include <coffee/core/types/size.h>

#include <peripherals/libc/signals.h>
#include <peripherals/stl/any_of.h>
#include <peripherals/stl/magic_enum.hpp>
#include <peripherals/stl/string/hex.h>
#include <peripherals/stl/string_casting.h>
#include <peripherals/stl/types.h>
#include <platforms/environment.h>

#include <fmt_extensions/info.h>
#include <fmt_extensions/vector_types.h>

#include <coffee/core/CDebug>
#include <coffee/strings/format.h>

#include <android/asset_manager.h>
#include <android/looper.h>
#include <android/native_activity.h>
#include <android/window.h>
#include <android_native_app_glue.h>
#include <sys/sysinfo.h>

namespace libc::signal {
extern std::vector<exit_handler> global_exit_handlers;
}

using namespace jnipp;

namespace Coffee {
struct android_app* coffee_app = nullptr;
} // namespace Coffee

namespace android {

/*
 *
 * Android structures
 *
 */

const char* jni_error_category::name() const noexcept
{
    return "jni_error_category";
}

std::string jni_error_category::message(int error_code) const
{
    switch(error_code)
    {
    case JNI_EVERSION:
        return "Invalid JNI version specified";
    case JNI_EDETACHED:
        return "JNI thread detached";
    case JNI_OK:
        return "No error";
    default:
        break;
    }

    throw implementation_error("error message not implemented");
}

using namespace Coffee;
using namespace jnipp::literals;
using namespace platform::url;
using re = jnipp::return_type;

namespace {

auto get_display_metrics()
{
    auto DisplayMetrics = "android.util.DisplayMetrics"_jclass;

    auto getResources =
        "getResources"_jmethod.ret("android.content.res.Resources");
    auto getDisplayMetrics =
        "getDisplayMetrics"_jmethod.ret("android.util.DisplayMetrics");

    auto resourceObject = app_objects::activity()[getResources]();
    return resourceObject[getDisplayMetrics]();
}

} // namespace

jnipp::wrapping::jobject app_objects::display()
{
    auto getDisplay = "getDisplay"_jmethod.ret("android.view.Display");
    return activity()[getDisplay]();
}

jnipp::wrapping::jobject app_objects::window()
{
    auto getWindow = "getWindow"_jmethod.ret("android.view.Window");
    return activity()[getWindow]();
}

jnipp::wrapping::jobject app_objects::activity()
{
    return "android.app.NativeActivity"_jclass(coffee_app->activity->clazz);
}

jnipp::wrapping::jobject app_objects::decor_view()
{
    auto getDecorView = "getDecorView"_jmethod.ret("android.view.View");
    return window()[getDecorView]();
}

intent::intent()
    : m_intent({{}, {}})
{
    auto getIntent = "getIntent"_jmethod.ret("android.content.Intent");
    m_intent       = app_objects::activity()[getIntent]();
}

std::string intent::action()
{
    if(!m_intent)
        return {};

    auto getAction = "getAction"_jmethod.ret("java.lang.String");

    auto out = m_intent[getAction]();

    if(!out)
        return {};

    return jnipp::java::type_unwrapper<std::string>(out);
}

std::string intent::data()
{
    if(!m_intent)
        return {};

    auto getData  = "getData"_jmethod.ret("android.net.Uri");
    auto toString = "toString"_jmethod.ret("java.lang.String");

    auto intentData = m_intent[getData]();

    if(!intentData)
        return {};

    return jnipp::java::type_unwrapper<std::string>(intentData[toString]());
}

std::set<std::string> intent::categories()
{
    if(!m_intent)
        return {};

    auto Set = "java.util.Set"_jclass;

    auto getCategories = "getCategories"_jmethod.ret("java.util.Set");
    auto toArray = "toArray"_jmethod.ret<re::object_array_>("java.lang.Object");

    auto categories = m_intent[getCategories]();

    if(!categories)
        return {};

    auto categoryArray = categories[toArray]();

    std::set<std::string> outCategories;

    for(auto category : *categoryArray)
        outCategories.insert(
            jnipp::java::type_unwrapper<std::string>(category));

    return outCategories;
}

std::map<std::string, std::string> intent::extras()
{
    if(!m_intent)
        return {};

    std::map<std::string, std::string> out;

    auto getExtras      = "getExtras"_jmethod.ret("android.os.Bundle");
    auto getStringExtra = "getStringExtra"_jmethod.ret("java.lang.String")
                              .arg<std::string>("java.lang.String");
    auto keySet = "keySet"_jmethod.ret("java.util.Set");
    auto setArray =
        "toArray"_jmethod.ret<re::object_array_>("java.lang.Object");

    auto extras = m_intent[getExtras]();

    if(!extras)
    {
        auto extrasKeySet = extras[keySet]();
        auto extraKeys    = extrasKeySet[setArray]();

        for(auto key : *extraKeys)
        {
            std::string key_s = jnipp::java::type_unwrapper<std::string>(key);

            auto extraVal = m_intent[getStringExtra](key_s);

            if(!extraVal)
            {
                cDebug("Extra value not included: {0}", key_s);
                continue;
            }

            std::string value =
                jnipp::java::type_unwrapper<std::string>(extraVal);
            out[key_s] = value;
        }
    }

    return out;
}

std::optional<std::string> intent::extra(const std::string& key)
{
    auto extras_  = extras();
    auto value_it = extras_.find(key);
    if(value_it == extras_.end())
        return std::nullopt;
    return value_it->second;
}

int intent::flags()
{
    if(!m_intent)
        return {};

    auto getFlags = "getFlags"_jmethod.ret<re::int_>();

    return m_intent[getFlags]();
}

std::string app_info::package_name()
{
    auto Context        = "android.content.Context"_jclass;
    auto getPackageName = "getPackageName"_jmethod.ret("java.lang.String");

    auto context = Context(coffee_app->activity->clazz);

    return jnipp::java::type_unwrapper<std::string>(context[getPackageName]());
}

int app_info::sdk_version()
{
    return coffee_app->activity->sdkVersion;
}

Url app_info::data_path()
{
    return constructors::MkSysUrl(coffee_app->activity->internalDataPath);
}

Url app_info::cache_path()
{
    auto Context         = "android.content.Context"_jclass;
    auto getCacheDir     = "getCacheDir"_jmethod.ret("java.io.File");
    auto getAbsolutePath = "getAbsolutePath"_jmethod.ret("java.lang.String");

    auto context  = Context(coffee_app->activity->clazz);
    auto cacheDir = context[getCacheDir]();

    return constructors::MkSysUrl(
        java::type_unwrapper<std::string>(cacheDir[getAbsolutePath]()));
}

Url app_info::external_data_path()
{
    auto def_path =
        Path{coffee_app->activity->externalDataPath}.url(RSCA::SystemFile);

    if(sdk_version() >= 21)
        return def_path;
    else
    {
        auto pkg_name = Path{package_name().data()};
        auto base     = Path("/storage/emulated/legacy") / Path{"Android/data"};
        return (base / pkg_name / "files").url(RSCA::SystemFile);
    }
}

std::vector<std::string> app_info::abis()
{
    auto Build = "android.os.Build"_jclass;

    std::vector<std::string> out;

    try
    {
        auto SUPPORTED_ABIS = "SUPPORTED_ABIS"_jfield.as("java.lang.String");

        auto abis = jnipp::java::array_type_unwrapper<re::object_>(
            *Build[SUPPORTED_ABIS]);

        for(auto abi : *abis)
            out.push_back(jnipp::java::type_unwrapper<std::string>(abi));
    } catch(jnipp::java_exception const&)
    {
        auto CPU_ABI  = "CPU_ABI"_jfield.as("java.lang.String");
        auto CPU_ABI2 = "CPU_ABI2"_jfield.as("java.lang.String");

        out.push_back(
            jnipp::java::type_unwrapper<std::string>(*Build[CPU_ABI]));
        out.push_back(
            jnipp::java::type_unwrapper<std::string>(*Build[CPU_ABI2]));
    }

    return out;
}

std::optional<::jnipp::wrapping::jobject> app_info::get_service(
    std::string const& service, std::optional<std::string> service_type)
{
    auto Context          = "android.content.Context"_jclass;
    auto getSystemService = "getSystemService"_jmethod.arg("java.lang.String")
                                .ret("java.lang.Object");

    auto instance = Context(coffee_app->activity->clazz)[getSystemService](
        jnipp::java::type_wrapper(service));

    auto class_type = jnipp::get_class_name(instance);

    if(service_type.has_value())
        instance = instance.cast({*service_type});
    return instance;
}

std::optional<jnipp::wrapping::jobject> app_info::input_method_service()
{
    return get_service(
        "input_method", "android.view.inputmethod.InputMethodManager");
}

std::optional<jnipp::wrapping::jobject> app_info::input_service()
{
    return get_service("input", "android.hardware.input.InputManager");
}

ANativeActivity* app_info::activity() const
{
    return coffee_app->activity;
}

AConfiguration* app_info::configuration() const
{
    return coffee_app->config;
}

AInputQueue* app_info::input_queue() const
{
    return coffee_app->inputQueue;
}

ALooper* app_info::looper() const
{
    return coffee_app->looper;
}

app_info::device_type_t app_info::device_type() const
{
    switch(AConfiguration_getUiModeType(configuration()))
    {
    case ACONFIGURATION_UI_MODE_TYPE_NORMAL:
        return device_type_t::phone;
    case ACONFIGURATION_UI_MODE_TYPE_TELEVISION:
        return device_type_t::tv;
    case ACONFIGURATION_UI_MODE_TYPE_VR_HEADSET:
        return device_type_t::vr_headset;
    default:
        return device_type_t::unknown;
    }
}

std::vector<std::string> app_info::system_features() const
{
    using jnipp::java::array_type_unwrapper;
    using jnipp::java::type_unwrapper;

    // There's something in this function incompatible with <21
    if(coffee_app->activity->sdkVersion < 21)
        return {};

    std::vector<std::string> features;

    auto Context     = "android.content.Context"_jclass;
    auto FeatureInfo = "android.content.pm.FeatureInfo"_jclass;

    auto getPackageManager =
        "getPackageManager"_jmethod.ret("android.content.pm.PackageManager");
    auto getSystemAvailableFeatures =
        "getSystemAvailableFeatures"_jmethod
            .ret<jnipp::return_type::object_array_>(
                "android.content.pm.FeatureInfo");
    auto name = "name"_jfield.as("java.lang.String");

    auto packageManager =
        Context(coffee_app->activity->clazz)[getPackageManager]();
    auto systemFeatures = packageManager[getSystemAvailableFeatures]();

    for(auto feature : *systemFeatures)
    {
        auto name_ = *feature[name];
        if(!name_)
            continue;
        features.push_back(type_unwrapper<std::string>(name_));
    }
    std::sort(features.begin(), features.end());

    return features;
}

void input_method_manager::show_soft_input()
{
    auto view          = app_objects::decor_view();
    auto showSoftInput = "showSoftInput"_jmethod.arg("android.view.View")
                             .arg<jint>()
                             .ret<jnipp::return_type::bool_>();
    auto input_method_manager = *app_info().input_method_service();
    input_method_manager[showSoftInput](view, 0);
}

void input_method_manager::hide_soft_input()
{
    auto view           = app_objects::decor_view();
    auto getWindowToken = "getWindowToken"_jmethod.ret("android.os.IBinder");
    auto token          = view[getWindowToken]();

    auto hideSoftInputFromWindow =
        "hideSoftInputFromWindow"_jmethod.arg("android.os.IBinder")
            .arg<jint>()
            .ret<jnipp::return_type::bool_>();
    auto input_method_manager = *app_info().input_method_service();
    input_method_manager[hideSoftInputFromWindow](token, 0);
}

std::optional<network_stats::result_t> network_stats::query(network_class net)
{
    if(coffee_app->activity->sdkVersion < 23)
        return std::nullopt;

    auto System            = "java.lang.System"_jclass;
    auto currentTimeMillis = "currentTimeMillis"_jmethod.ret<re::long_>();

    auto querySummary = "querySummary"_jmethod.arg<jint>()
                            .arg("java.lang.String")
                            .arg<jlong>()
                            .arg<jlong>()
                            .ret("android.app.usage.NetworkStats");

    auto getNextBucket =
        "getNextBucket"_jmethod.arg("android.app.usage.NetworkStats$Bucket")
            .ret<re::bool_>();
    auto hasNextBucket = "hasNextBucket"_jmethod.ret<re::bool_>();

    auto Bucket          = "android.app.usage.NetworkStats$Bucket"_jclass;
    auto bucketConstruct = "<init>"_jmethod;

    auto getRxBytes = "getRxBytes"_jmethod.ret<re::long_>();
    auto getTxBytes = "getTxBytes"_jmethod.ret<re::long_>();

    auto net_stats = *app_info().get_service(
        "netstats", "android.app.usage.NetworkStatsManager");

    java::value sub_id = ::jvalue();
    sub_id->l          = 0;

    auto now = System[currentTimeMillis]();

    auto stats = net_stats[querySummary](net, *sub_id, 0, now);

    auto bucket = Bucket.construct(bucketConstruct);

    result_t out;

    while(stats[hasNextBucket]())
    {
        if(!stats[getNextBucket](bucket))
            break;

        auto rx = bucket[getRxBytes]();
        auto tx = bucket[getTxBytes]();

        out.rx += rx;
        out.tx += tx;
    }

    return out;
}

std::optional<activity_manager::memory_info> activity_manager::get_mem_info()
{
    auto MemoryInfo          = "android.app.ActivityManager$MemoryInfo"_jclass;
    auto memoryInfoConstruct = "<init>"_jmethod;
    auto mem_info            = MemoryInfo.construct(memoryInfoConstruct);

    auto activity_manager =
        *app_info().get_service("activity", "android.app.ActivityManager");

    if(!C_OCAST<::jvalue>(mem_info).z || !C_OCAST<::jvalue>(activity_manager).z)
        return {};

    auto avail      = "availMem"_jfield.as<re::long_>();
    auto threshold  = "threshold"_jfield.as<re::long_>();
    auto total      = "totalMem"_jfield.as<re::long_>();
    auto is_low_mem = "lowMemory"_jfield.as<re::bool_>();

    auto getMemoryInfo = "getMemoryInfo"_jmethod.arg(MemoryInfo);
    activity_manager[getMemoryInfo](mem_info);

    return memory_info{
        .available = C_FCAST<libc_types::u64>(*mem_info[avail]),
        .total     = C_FCAST<libc_types::u64>(*mem_info[total]),
    };
}

std::optional<activity_manager::config_info> activity_manager::get_config_info()
{
    auto Activity   = "android.app.ActivityManager"_jclass;
    auto ConfigInfo = "android.app.ActivityManager$ConfigurationInfo"_jclass;

    auto getDeviceConfigurationInfo =
        "getDeviceConfigurationInfo"_jmethod.arg(ConfigInfo);

    return {};
}

std::optional<activity_manager::window_info> activity_manager::window()
{
    return window_info{
        .activity        = Coffee::coffee_app->activity,
        .window          = Coffee::coffee_app->window,
        .activity_object = Coffee::coffee_app->activity->clazz,
    };
}

AAssetManager* activity_manager::asset_manager()
{
    return Coffee::coffee_app->activity->assetManager;
}

display_info::hdr_mode_t display_info::hdr_modes()
{
    if(coffee_app->activity->sdkVersion < 30)
        return hdr_mode_t::none;

    auto display = app_objects::display();

    auto getHdrCapabilities = "getHdrCapabilities"_jmethod.ret(
        "android.view.Display$HdrCapabilities");
    auto isHdr = "isHdr"_jmethod.ret<re::bool_>();

    if(!display[isHdr]())
        return hdr_mode_t::none;

    enum android_types
    {
        dolby_vision = 1,
        hdr10,
        hlg,
        hdr10_plus,
    };

    auto getSupportedHdrTypes =
        "getSupportedHdrTypes"_jmethod.ret<re::int_array_>();

    auto hdrCapabilities = display[getHdrCapabilities]();
    auto hdrTypes        = hdrCapabilities[getSupportedHdrTypes]();

    hdr_mode_t out = none;
    for(i32 type : *hdrTypes)
    {
        if(type == dolby_vision)
            out |= hdr_mode_t::dolby_vision;
        else if(type == hdr10)
            out |= hdr_mode_t::hdr10;
        else if(type == hlg)
            out |= hdr_mode_t::hlg;
        else if(type == hdr10_plus)
            out |= hdr_mode_t::hdr10_plus;
    }

    return out;
}

bool display_info::is_low_latency()
{
    if(coffee_app->activity->sdkVersion < 30)
        return false;

    auto display = app_objects::display();
    auto isMinimalPostProcessingSupported =
        "isMinimalPostProcessingSupported"_jmethod.ret<re::bool_>();

    return display[isMinimalPostProcessingSupported]();
}

bool display_info::is_wide_gamut()
{
    if(coffee_app->activity->sdkVersion < 30)
        return false;

    auto display          = app_objects::display();
    auto isWideColorGamut = "isWideColorGamut"_jmethod.ret<re::bool_>();
    return display[isWideColorGamut]();
}

std::optional<display_info::insets_t> display_info::safe_insets()
{
    using libc_types::u32;

    if(coffee_app->activity->sdkVersion < 30)
        return std::nullopt;

    static std::optional<display_info::insets_t> cached_insets;

    if(cached_insets.has_value())
        return *cached_insets;

    auto display = app_objects::display();

    auto getCutout = "getCutout"_jmethod.ret("android.view.DisplayCutout");
    auto cutout    = display[getCutout]();
    if(!cutout)
        return std::nullopt;

    auto getSafeInsetBottom = "getSafeInsetBottom"_jmethod.ret<re::int_>();
    auto getSafeInsetLeft   = "getSafeInsetLeft"_jmethod.ret<re::int_>();
    auto getSafeInsetRight  = "getSafeInsetRight"_jmethod.ret<re::int_>();
    auto getSafeInsetTop    = "getSafeInsetTop"_jmethod.ret<re::int_>();

    cached_insets = insets_t{
        .top    = static_cast<f32>(cutout[getSafeInsetTop]()),
        .bottom = static_cast<f32>(cutout[getSafeInsetBottom]()),
        .left   = static_cast<f32>(cutout[getSafeInsetLeft]()),
        .right  = static_cast<f32>(cutout[getSafeInsetRight]()),
    };
    return *cached_insets;
}

display_info::rotation_t display_info::rotation()
{
    if(coffee_app->activity->sdkVersion < 30)
        return display_info::rotation_t::portrait_0;
    auto display          = app_objects::display();
    auto getRotation      = "getRotation"_jmethod.ret<re::int_>();
    auto current_rotation = display[getRotation]();
    return static_cast<rotation_t>(current_rotation);
}

typing::vector_types::Vecf2 display_info::physical_size()
{
    static std::optional<typing::vector_types::Vecf2> cached_size;

    if(cached_size.has_value())
        return *cached_size;

    auto displayMetrics = get_display_metrics();
    auto xdpi           = *displayMetrics["xdpi"_jfield.as<re::float_>()];
    auto ydpi           = *displayMetrics["ydpi"_jfield.as<re::float_>()];
    auto heightPixels   = *displayMetrics["heightPixels"_jfield.as<re::int_>()];
    auto widthPixels    = *displayMetrics["widthPixels"_jfield.as<re::int_>()];

    cached_size = typing::vector_types::Vecf2{
        widthPixels / xdpi,
        heightPixels / ydpi,
    };
    return *cached_size;
}

f32 display_info::dpi()
{
    static std::optional<f32> cached_dpi{};

    if(cached_dpi.has_value())
        return *cached_dpi;

    auto displayMetrics = get_display_metrics();

    cached_dpi = *displayMetrics["densityDpi"_jfield.as<re::int_>()] / 160.f;
    return *cached_dpi;
}

f32 display_info::refresh_rate()
{
    if(coffee_app->activity->sdkVersion < 30)
        return 60.f;
    auto display        = app_objects::display();
    auto getRefreshRate = "getRefreshRate"_jmethod.ret<re::float_>();

    return display[getRefreshRate]();
}

} // namespace android

namespace Coffee {

using libc_types::u32;

int MainSetup(::MainWithArgs mainfun, int argc, char** argv, u32 flags = 0);
int MainSetup(::MainNoArgs mainfun, int argc, char** argv, u32 flags = 0);

using namespace android;

/*
 *
 * Android event handling
 *
 */

STATICINLINE void GetExtras()
{
    using namespace jnipp::literals;

    {
        /* Get display DPI */

        cDebug("Display DPI: {0}", android::display_info().dpi());
        cDebug(
            "Display HDR mode: {0}",
            static_cast<int>(android::display_info().hdr_modes()));
        cDebug(
            "Display low latency: {0}",
            android::display_info().is_low_latency());
        cDebug(
            "Display wide gamut: {0}", android::display_info().is_wide_gamut());
        cDebug(
            "Display refresh rate: {0}",
            android::display_info().refresh_rate());

        if(auto insets_ = android::display_info().safe_insets())
        {
            auto insets = insets_.value();
            cDebug(
                "Insets: top:{0} bottom:{1} left:{2} right:{3}",
                insets.top,
                insets.bottom,
                insets.left,
                insets.right);
        }
    }

    {
        /* Get system ABIs */
        for(auto const& abi : android::app_info().abis())
            cDebug("{0}", abi);
    }

    {
        /* Intent extras */
        intent appIntent;

        cDebug("Intent summary:");

        cDebug("App URI: {0}", appIntent.data());

        static std::vector<std::string> stringStorage;
        static auto                     extras = appIntent.extras();

        for(auto const& e : extras)
        {
            if(e.first.substr(0, 7) == "COFFEE_")
                platform::env::set_var(e.first, e.second);
            cDebug("{0} = {1}", e.first, e.second);
        }

        auto verbosity = extras.find("COFFEE_VERBOSITY");
        if(verbosity != extras.end())
            Coffee::SetPrintingVerbosity(
                stl_types::cast_string<u8>(verbosity->second));

        cDebug("App action: {0}", appIntent.action());

        for(auto cat : appIntent.categories())
            cDebug("{0}", cat);

        cDebug("App flags: {0}", str::fmt::hexify(appIntent.flags()));
    }
}

static bool                                  window_initialized  = false;
static bool                                  first_loop_complete = false;
static std::chrono::steady_clock::time_point launch_time;

STATICINLINE void InitializeState(struct android_app* state)
{
    using namespace jnipp::literals;

    coffee_app = state;

    ScopedJNI jni(coffee_app->activity->vm);
    jnipp::SwapJNI(&jni);

    state->onAppCmd = [](struct android_app* app, int32_t event) {
        static anative::AndroidEventBus* android_bus;

        cDebug(
            "Incoming event: {0}",
            magic_enum::enum_name(static_cast<app_cmd_t>(event)));

        if(event == APP_CMD_INIT_WINDOW)
        {
            static std::vector<std::string> stringStorage;
            std::vector<char*>              args;
            stringStorage.push_back("lib");

            auto extras = android::intent().extras();
            for(auto& e : extras)
                if(!std::isupper(e.first.at(0), std::locale()))
                {
                    stringStorage.push_back(fmt::format("--{}", e.first));
                    stringStorage.push_back(e.second);
                }
            for(auto& arg : stringStorage)
                args.push_back(arg.data());
            cDebug("Starting CoffeeMain with: {}", args);

            auto& entrypoints = Coffee::main_functions;
            if(entrypoints.is_no_args)
                MainSetup(entrypoints.no_args, args.size(), args.data());
            else
                MainSetup(entrypoints.with_args, args.size(), args.data());

            if(auto it = extras.find("COFFEE_VERBOSITY"); it != extras.end())
                Coffee::SetPrintingVerbosity(cast_string<u8>(it->second));
            else
                Coffee::SetPrintingVerbosity(compile_info::debug_mode ? 15 : 1);

            window_initialized = true;
            auto delta_launch  = std::chrono::steady_clock::now() - launch_time;
            cDebug(
                "Took {0} ms to create window",
                std::chrono::duration_cast<std::chrono::milliseconds>(
                    delta_launch)
                    .count());
        }

        if(!window_initialized)
        {
            android_bus = nullptr;
        }

        if(!android_bus)
        {
            auto& entities = comp_app::createContainer();
            android_bus    = entities.service<anative::AndroidEventBus>();
        }

        if(!android_bus)
            return;

        android_bus->handleWindowEvent(app, event);

        if(event == APP_CMD_TERM_WINDOW)
        {
            Profiling::ExitRoutine();

            auto rev_handlers = libc::signal::global_exit_handlers;
            std::reverse(rev_handlers.begin(), rev_handlers.end());

            for(auto const& hnd : rev_handlers)
                hnd();

            libc::signal::exit(libc::signal::sig::abort);
            window_initialized = false;
        }
    };
    state->onInputEvent = [](struct android_app*, struct AInputEvent* event) {
        static anative::AndroidEventBus* android_bus;

        if(!window_initialized)
        {
            android_bus = nullptr;
            return 0;
        }

        if(!android_bus)
        {
            auto& entities = comp_app::createContainer();
            android_bus    = entities.service<anative::AndroidEventBus>();
        }

        if(!android_bus)
            return 0;

        android_bus->handleInputEvent(event);

        return 1;
    };

    auto activityName =
        jnipp::get_class_name(jnipp::java::object({}, state->activity->clazz));

    cDebug("State:       {0}", str::fmt::pointerify(state));
    cDebug("Activity:    {0}", activityName);
    cDebug("Android API: {0}", state->activity->sdkVersion);

    auto memory = *android::activity_manager().get_mem_info();
    cDebug("System memory: {0}", memory.total);

    GetExtras();
    auto extras = android::intent().extras();
    if(auto it = extras.find("COFFEE_VERBOSITY"); it != extras.end())
        SetPrintingVerbosity(cast_string<u8>(it->second));

    jnipp::SwapJNI(nullptr);
}

STATICINLINE void StartEventProcessing(android_app* state)
{
    ScopedJNI jni(state->activity->vm);
    jnipp::SwapJNI(&jni);

    cDebug(
        "Starting in state: activity_state={0}, running={1}",
        magic_enum::enum_name(static_cast<app_cmd_t>(state->activityState)),
        static_cast<bool>(state->running));

    cDebug(
        "Took {0} ms to start event loop",
        std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::steady_clock::now() - launch_time))
            .count());

    while(1)
    {
        //        int timeout = -1;

        //        if(state->activityState == APP_CMD_START
        //           || state->activityState == APP_CMD_RESUME)
        //            timeout = 0;

        int ident;
        int events;

        struct android_poll_source* source = nullptr;

        while((ident = ALooper_pollAll(0, nullptr, &events, (void**)&source)) >=
              0)
        {
            if(source != nullptr)
                source->process(state, source);

            if(state->destroyRequested)
                break;
        }

        bool active = state->activityState == APP_CMD_START ||
                      state->activityState == APP_CMD_RESUME;

        if(window_initialized && active)
        {
            if(!first_loop_complete)
            {
                cDebug(
                    "Took {0} ms till first draw",
                    std::chrono::duration_cast<std::chrono::milliseconds>(
                        std::chrono::duration_cast<std::chrono::milliseconds>(
                            std::chrono::steady_clock::now() - launch_time))
                        .count());
                first_loop_complete = true;
            }
            comp_app::setup_and_loop_container(comp_app::createContainer());
        }
    }

    comp_app::cleanup_container(comp_app::createContainer());
    jnipp::SwapJNI(nullptr);
}

} // namespace Coffee

void android_main(struct android_app* state)
{
    Coffee::launch_time = std::chrono::steady_clock::now();
    Coffee::SetPrintingVerbosity(15);
    Coffee::InitializeState(state);
    Coffee::StartEventProcessing(state);
}
