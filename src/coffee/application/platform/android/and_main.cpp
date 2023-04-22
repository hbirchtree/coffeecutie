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
#include <peripherals/stl/string_casting.h>
#include <peripherals/stl/types.h>
#include <platforms/environment.h>

#include <coffee/strings/info.h>
#include <coffee/strings/libc_types.h>
#include <coffee/strings/vector_types.h>

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
using namespace jnipp_operators;
using namespace platform::url;
using re = jnipp::return_type;

intent::intent() : m_intent({{}, {}})
{
    auto activity
        = "android.app.NativeActivity"_jclass(coffee_app->activity->clazz);
    auto Intent = "android.content.Intent"_jclass;

    auto getIntent = "getIntent"_jmethod.ret("android.content.Intent");

    m_intent = Intent(activity[getIntent]());
}

std::string intent::action()
{
    if(!java::objects::not_null(m_intent))
        return {};

    auto getAction = "getAction"_jmethod.ret("java.lang.String");

    auto out = m_intent[getAction]();

    if(!java::objects::not_null(out))
        return {};

    return jnipp::java::type_unwrapper<std::string>(out);
}

std::string intent::data()
{
    if(!java::objects::not_null(m_intent))
        return {};

    auto Uri = "android.net.Uri"_jclass;

    auto getData  = "getData"_jmethod.ret("android.net.Uri");
    auto toString = "toString"_jmethod.ret("java.lang.String");

    auto intentData = m_intent[getData]();

    if(!intentData)
        return {};

    auto intentUri = Uri(intentData);

    return jnipp::java::type_unwrapper<std::string>(intentUri[toString]());
}

std::set<std::string> intent::categories()
{
    if(!java::objects::not_null(m_intent))
        return {};

    auto Set = "java.util.Set"_jclass;

    auto getCategories = "getCategories"_jmethod.ret("java.util.Set");
    auto toArray = "toArray"_jmethod.ret<re::object_array_>("java.lang.Object");

    auto categories = m_intent[getCategories]();

    if(!categories)
        return {};

    auto categorySet   = Set(categories);
    auto categoryArray = jnipp::java::array_type_unwrapper<re::object_>(
        categorySet[toArray]());

    std::set<std::string> outCategories;

    for(auto category : *categoryArray)
        outCategories.insert(
            jnipp::java::type_unwrapper<std::string>(category));

    return outCategories;
}

std::map<std::string, std::string> intent::extras()
{
    if(!java::objects::not_null(m_intent))
        return {};

    std::map<std::string, std::string> out;

    auto Bundle = "android.os.Bundle"_jclass;
    auto Set    = "java.util.Set"_jclass;

    auto getExtras      = "getExtras"_jmethod.ret("android.os.Bundle");
    auto getStringExtra = "getStringExtra"_jmethod.ret("java.lang.String")
                              .arg<std::string>("java.lang.String");
    auto keySet = "keySet"_jmethod.ret("java.util.Set");
    auto setArray
        = "toArray"_jmethod.ret<re::object_array_>("java.lang.Object");

    auto extrasRef = m_intent[getExtras]();

    if(jnipp::java::objects::not_null(extrasRef))
    {
        auto extras       = Bundle(extrasRef);
        auto extrasKeySet = Set(extras[keySet]());

        auto extraKeys = jnipp::java::array_type_unwrapper<re::object_>(
            extrasKeySet[setArray]());

        for(auto key : *extraKeys)
        {
            std::string key_s = jnipp::java::type_unwrapper<std::string>(key);

            auto extraVal = m_intent[getStringExtra](key_s);

            if(!jnipp::java::objects::not_null(extraVal))
            {
                cDebug("Extra value not included: {0}", key_s);
                continue;
            }

            std::string value
                = jnipp::java::type_unwrapper<std::string>(extraVal);
            out[key_s] = value;
        }
    }

    return out;
}

std::optional<std::string> intent::extra(const std::string &key)
{
    auto extras_ = extras();
    auto value_it = extras_.find(key);
    if(value_it == extras_.end())
        return std::nullopt;
    return value_it->second;
}

int intent::flags()
{
    if(!java::objects::not_null(m_intent))
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
    auto File            = "java.io.File"_jclass;
    auto getCacheDir     = "getCacheDir"_jmethod.ret("java.io.File");
    auto getAbsolutePath = "getAbsolutePath"_jmethod.ret("java.lang.String");

    auto context  = Context(coffee_app->activity->clazz);
    auto cacheDir = File(context[getCacheDir]());

    return constructors::MkSysUrl(
        java::type_unwrapper<std::string>(cacheDir[getAbsolutePath]()));
}

Url app_info::external_data_path()
{
    auto def_path
        = Path{coffee_app->activity->externalDataPath}.url(RSCA::SystemFile);

    if(sdk_version() >= 21)
        return def_path;
    else
    {
        auto pkg_name = Path{package_name().data()};
        auto base     = Path("/storage/emulated/legacy") / Path{"Android/data"};
        return (base / pkg_name / "files").url(RSCA::SystemFile);
    }
}

std::optional<::jnipp::java::object> app_info::get_service(
    std::string const& service)
{
    auto Context          = "android.content.Context"_jclass;
    auto getSystemService = "getSystemService"_jmethod.arg("java.lang.String")
                                .ret("java.lang.Object");

    auto instance = Context(coffee_app->activity->clazz)[getSystemService](
        jnipp::java::type_wrapper(service));

    auto class_type = jnipp::java::objects::get_class(instance);

    return jnipp::java::object{Context.clazz, instance};
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

std::optional<network_stats::result_t> network_stats::query(network_class net)
{
    if(coffee_app->activity->sdkVersion < 23)
        return std::nullopt;

    auto System            = "java.lang.System"_jclass;
    auto currentTimeMillis = "currentTimeMillis"_jmethod.ret<re::long_>();

    auto NetStats     = "android.app.usage.NetworkStatsManager"_jclass;
    auto querySummary = "querySummary"_jmethod.arg<jint>()
                            .arg("java.lang.String")
                            .arg<jlong>()
                            .arg<jlong>()
                            .ret("android.app.usage.NetworkStats");

    auto NetworkStats = "android.app.usage.NetworkStats"_jclass;
    auto getNextBucket
        = "getNextBucket"_jmethod.arg("android.app.usage.NetworkStats$Bucket")
              .ret<re::bool_>();
    auto hasNextBucket = "hasNextBucket"_jmethod.ret<re::bool_>();

    auto Bucket          = "android.app.usage.NetworkStats$Bucket"_jclass;
    auto bucketConstruct = "<init>"_jmethod;

    auto getRxBytes = "getRxBytes"_jmethod.ret<re::long_>();
    auto getTxBytes = "getTxBytes"_jmethod.ret<re::long_>();

    auto net_stats = NetStats(*app_info().get_service("netstats"));

    java::value sub_id = ::jvalue();
    sub_id->l          = 0;

    auto now = System[currentTimeMillis]();

    auto stats = NetworkStats(net_stats[querySummary](net, *sub_id, 0, now));

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

std::optional<activity_manager::memory_info> activity_manager::
    get_mem_info()
{
    auto Activity      = "android.app.ActivityManager"_jclass;
    auto MemoryInfo    = "android.app.ActivityManager$MemoryInfo"_jclass;
    auto getMemoryInfo = "getMemoryInfo"_jmethod.arg(MemoryInfo);

    auto memoryInfoConstruct = "<init>"_jmethod;

    auto mem_info         = MemoryInfo.construct(memoryInfoConstruct);
    auto activity_manager = Activity(*app_info().get_service("activity"));

    if(!C_OCAST<::jvalue>(mem_info).z || !C_OCAST<::jvalue>(activity_manager).z)
        return {};

    auto avail      = "availMem"_jfield.as<re::long_>();
    auto threshold  = "threshold"_jfield.as<re::long_>();
    auto total      = "totalMem"_jfield.as<re::long_>();
    auto is_low_mem = "lowMemory"_jfield.as<re::bool_>();

    activity_manager[getMemoryInfo](mem_info);

    return memory_info{
        .available = C_FCAST<libc_types::u64>(*mem_info[avail]),
        .total     = C_FCAST<libc_types::u64>(*mem_info[total]),
    };
}

std::optional<activity_manager::config_info> activity_manager::
    get_config_info()
{
    auto Activity   = "android.app.ActivityManager"_jclass;
    auto ConfigInfo = "android.app.ActivityManager$ConfigurationInfo"_jclass;

    auto getDeviceConfigurationInfo
        = "getDeviceConfigurationInfo"_jmethod.arg(ConfigInfo);

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

std::vector<std::string> cpu_abis()
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

static auto getDisplay()
{
    auto activityObject
        = "android.app.NativeActivity"_jclass(coffee_app->activity->clazz);
    auto getDisplay = "getDisplay"_jmethod.ret("android.view.Display");

    return "android.view.Display"_jclass(activityObject[getDisplay]());
}

display_info::hdr_mode_t display_info::hdr_modes()
{
    if(coffee_app->activity->sdkVersion < 30)
        return hdr_mode_t::none;

    auto display = getDisplay();

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

    auto HdrCapabilities = "android.view.Display$HdrCapabilities"_jclass;
    auto getSupportedHdrTypes
        = "getSupportedHdrTypes"_jmethod.ret<re::int_array_>();

    auto hdrCapabilities = HdrCapabilities(display[getHdrCapabilities]());
    auto hdrTypes        = hdrCapabilities[getSupportedHdrTypes]();

    hdr_mode_t out = none;
    for(i32 type : jnipp::java::array_extractors::container<re::int_>(hdrTypes))
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

    auto display = getDisplay();
    auto isMinimalPostProcessingSupported
        = "isMinimalPostProcessingSupported"_jmethod.ret<re::bool_>();

    return display[isMinimalPostProcessingSupported]();
}

bool display_info::is_wide_gamut()
{
    if(coffee_app->activity->sdkVersion < 30)
        return false;

    auto display          = getDisplay();
    auto isWideColorGamut = "isWideColorGamut"_jmethod.ret<re::bool_>();
    return display[isWideColorGamut]();
}

std::optional<display_info::insets_t> display_info::safe_insets()
{
    using libc_types::u32;

    if(coffee_app->activity->sdkVersion < 30)
        return std::nullopt;

    auto display   = getDisplay();
    auto getCutout = "getCutout"_jmethod.ret("android.view.DisplayCutout");

    auto cutoutValue = display[getCutout]();
    if(!cutoutValue)
        return std::nullopt;
    auto DisplayCutout = "android.view.DisplayCutout"_jclass;
    auto cutout        = DisplayCutout(cutoutValue);

    auto getSafeInsetBottom = "getSafeInsetBottom"_jmethod.ret<re::int_>();
    auto getSafeInsetLeft   = "getSafeInsetLeft"_jmethod.ret<re::int_>();
    auto getSafeInsetRight  = "getSafeInsetRight"_jmethod.ret<re::int_>();
    auto getSafeInsetTop    = "getSafeInsetTop"_jmethod.ret<re::int_>();

    return insets_t{
        .top    = static_cast<f32>(cutout[getSafeInsetTop]()),
        .bottom = static_cast<f32>(cutout[getSafeInsetBottom]()),
        .left   = static_cast<f32>(cutout[getSafeInsetLeft]()),
        .right  = static_cast<f32>(cutout[getSafeInsetRight]()),
    };
}

display_info::rotation_t display_info::rotation()
{
    if(coffee_app->activity->sdkVersion < 30)
        return display_info::rotation_t::portrait_0;
    auto display          = getDisplay();
    auto getRotation      = "getRotation"_jmethod.ret<re::int_>();
    auto current_rotation = display[getRotation]();
    return static_cast<rotation_t>(current_rotation);
}

f32 display_info::dpi()
{
    auto activityObject
        = "android.app.NativeActivity"_jclass(coffee_app->activity->clazz);

    auto Resources      = "android.content.res.Resources"_jclass;
    auto DisplayMetrics = "android.util.DisplayMetrics"_jclass;

    auto getResources
        = "getResources"_jmethod.ret("android.content.res.Resources");
    auto getDisplayMetrics
        = "getDisplayMetrics"_jmethod.ret("android.util.DisplayMetrics");

    auto resourceObject = Resources(activityObject[getResources]());
    auto displayMetrics = DisplayMetrics(resourceObject[getDisplayMetrics]());

    auto displayDpi = *displayMetrics["densityDpi"_jfield.as<re::int_>()];

    return displayDpi;
}

f32 display_info::refresh_rate()
{
    if(coffee_app->activity->sdkVersion < 30)
        return 60.f;
    auto display        = getDisplay();
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
    using namespace jnipp_operators;

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
        for(auto const& abi : android::cpu_abis())
            cDebug("{0}", abi);
    }

    {
        /* Intent extras */

        intent appIntent;

        cDebug("Intent summary:");

        cDebug("App URI: {0}", appIntent.data());

        auto extras = appIntent.extras();

        for(auto e : extras)
        {
            if(e.first.substr(0, 7) != "COFFEE_")
                continue;

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

        cDebug("App flags: {0}", str::convert::hexify(appIntent.flags()));
    }
}

static bool                                  window_initialized  = false;
static bool                                  first_loop_complete = false;
static std::chrono::steady_clock::time_point launch_time;

STATICINLINE void InitializeState(struct android_app* state)
{
    using namespace jnipp_operators;

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
            auto& entrypoints = Coffee::main_functions;
            if(entrypoints.is_no_args)
                MainSetup(entrypoints.no_args, 0, nullptr);
            else
                MainSetup(entrypoints.with_args, 0, nullptr);

            auto extras = android::intent().extras();
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

    auto activityName = jnipp::java::objects::get_class(
        jnipp::java::object({}, state->activity->clazz));

    cDebug("State:       {0}", str::print::pointerify(state));
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

        while((ident = ALooper_pollAll(0, nullptr, &events, (void**)&source))
              >= 0)
        {
            if(source != nullptr)
                source->process(state, source);

            if(state->destroyRequested)
                break;
        }

        bool active = state->activityState == APP_CMD_START
                      || state->activityState == APP_CMD_RESUME;

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
