#include <coffee/android/android_main.h>

#include <coffee/anative/anative_comp.h>
#include <coffee/comp_app/bundle.h>
#include <coffee/core/coffee.h>
#include <coffee/core/profiler/profiling-export.h>
#include <coffee/core/types/point.h>
#include <coffee/core/types/size.h>
#include <coffee/core/types/vector_types.h>
#include <coffee/foreign/foreign.h>
#include <peripherals/libc/signals.h>
#include <peripherals/stl/string_casting.h>
#include <peripherals/stl/types.h>
#include <platforms/environment.h>
#include <platforms/sensor.h>

#include <coffee/strings/info.h>
#include <coffee/strings/libc_types.h>
#include <coffee/strings/vector_types.h>

#include <coffee/core/CDebug>
#include <coffee/strings/format.h>

#include <android/looper.h>
#include <android/native_activity.h>
#include <android/window.h>
#include <android_native_app_glue.h>
#include <gestureDetector.h>
#include <sys/sysinfo.h>

namespace libc {
namespace signal {

extern stl_types::Vector<exit_handler> global_exit_handlers;

}
} // namespace libc

using namespace jnipp;

namespace android {

/*
 *
 * Android structures
 *
 */

enum AndroidAppState
{
    AndroidApp_Hidden      = 0x1,
    AndroidApp_Visible     = 0x2,
    AndroidApp_Initialized = 0x4,
};

C_FLAGS(AndroidAppState, libc_types::u32);

struct AndroidSensorData
{
    ASensorManager*    manager;
    const ASensor*     accelerometer;
    const ASensor*     gyroscope;
    ASensorEventQueue* eventQueue;
};

struct InputDetectors
{
    ndk_helper::TapDetector       tap;
    ndk_helper::DoubletapDetector double_tap;
    ndk_helper::PinchDetector     pinch;
    ndk_helper::DragDetector      drag;
};

struct AndroidInternalState
{
    InputDetectors  input;
    AndroidAppState currentState;
    std::string     cachePath;
};

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

} // namespace android

extern void* coffee_event_handling_data;

namespace Coffee {

int MainSetup(::MainWithArgs mainfun, int argc, char** argv, u32 flags);
int MainSetup(::MainNoArgs mainfun, int argc, char** argv, u32 flags);

using namespace android;

/*
 *
 * Android storage
 *
 */
static const constexpr u8    INPUT_VERB         = 11;
static AndroidInternalState* app_internal_state = nullptr;
struct android_app*          coffee_app         = nullptr;
static JNIEnv*               coffee_jni_env;

void (*CoffeeEventHandle_Platform)(void*, int);
void (*CoffeeEventHandleNA_Platform)(void*, int, void*, void*, void*);

void (*CoffeeForeignSignalHandle_Platform)(int);
void (*CoffeeForeignSignalHandleNA_Platform)(int, void*, void*, void*);

/*
 *
 * Android event handling
 *
 */

inline void AndroidForwardAppEvent(android_app* app, libc_types::i32 event)
{
    auto& entities    = comp_app::createContainer();
    auto  android_bus = entities.service<anative::AndroidEventBus>();

    if(!android_bus)
        return;

    android_bus->handleWindowEvent(app, event);
}

void AndroidHandleAppCmd(struct android_app* app, int32_t event)
{
    AndroidForwardAppEvent(app, event);

    switch(event)
    {
    case APP_CMD_START: {
        break;
    }
    case APP_CMD_RESUME:
    case APP_CMD_PAUSE:
        //    case APP_CMD_DESTROY:
    case APP_CMD_STOP: {
        cDebug("Lifecycle event triggered: {0}", event);
        break;
    }

        /* Lifecycle events we care about */
    case APP_CMD_INIT_WINDOW: {
        if(!(app_internal_state->currentState & AndroidApp_Initialized))
        {
            auto& entrypoints = Coffee::main_functions;

            if(entrypoints.is_no_args)
                MainSetup(entrypoints.no_args, 0, nullptr);
            else
                MainSetup(entrypoints.with_args, 0, nullptr);

            auto extras = android::intent().extras();
            if(auto it = extras.find("COFFEE_VERBOSITY"); it != extras.end())
                Coffee::SetPrintingVerbosity(cast_string<u8>(it->second));

            app_internal_state->currentState |= AndroidApp_Initialized;
        }

        CoffeeEventHandleCall(CoffeeHandle_Setup);

        ANativeActivity_setWindowFlags(
            app->activity,
            AWINDOW_FLAG_FULLSCREEN | AWINDOW_FLAG_KEEP_SCREEN_ON,
            AWINDOW_FLAG_FULLSCREEN | AWINDOW_FLAG_KEEP_SCREEN_ON);

        cDebug("Window: {0}", coffee_app->window);

        /* Intentional fallthrough, we need to push a resize event */
    }
    case APP_CMD_WINDOW_RESIZED: {
        struct CfGeneralEvent gev;
        gev.type = CfResizeEvent;

        struct CfResizeEventData rev;
        rev.w = C_FCAST<u32>(ANativeWindow_getWidth(app->window));
        rev.h = C_FCAST<u32>(ANativeWindow_getHeight(app->window));

        CoffeeEventHandleNACall(CoffeeHandle_GeneralEvent, &gev, &rev, nullptr);

        break;
    }

    case APP_CMD_GAINED_FOCUS: {
        CoffeeEventHandleCall(CoffeeHandle_IsForeground);

        app_internal_state->currentState =
            AndroidApp_Visible | AndroidApp_Initialized;
        break;
    }
    case APP_CMD_LOST_FOCUS: {
        CoffeeEventHandleCall(CoffeeHandle_IsBackground);

        app_internal_state->currentState =
            AndroidApp_Hidden | AndroidApp_Initialized;
        break;
    }
    case APP_CMD_TERM_WINDOW: {
        CoffeeEventHandleCall(CoffeeHandle_Cleanup);

        Profiling::ExitRoutine();

        auto rev_handlers = libc::signal::global_exit_handlers;
        std::reverse(rev_handlers.begin(), rev_handlers.end());

        for(auto const& hnd : rev_handlers)
            hnd();

        libc::signal::exit(libc::signal::sig::abort);

        break;
    }

        /* Special events */
    case APP_CMD_LOW_MEMORY: {
        CoffeeEventHandleCall(CoffeeHandle_LowMem);
        break;
    }

    case APP_CMD_DESTROY: {
        break;
    }

    default: {
        cWarning("Unhandled native event: {0}", event);
        break;
    }
    }
}

int32_t AndroidHandleInputCmd(
    struct android_app* app, struct AInputEvent* event)
{
    pthread_mutex_lock(&app->mutex);

    auto& entities    = comp_app::createContainer();
    auto  android_bus = entities.service<anative::AndroidEventBus>();

    if(!android_bus)
    {
        pthread_mutex_unlock(&app->mutex);
        return 0;
    }

    android_bus->handleInputEvent(event);

    pthread_mutex_unlock(&app->mutex);
    return 1;
}

static void AndroidForeignSignalHandle(int evtype)
{
    switch(evtype)
    {
    case CoffeeForeign_ActivateMotion: {
        /* TODO: Fix sensor code */
        //        Sensor::Android::Android_InitSensors();
        //        libc::signal::register_atexit(Sensor::Android::Android_DestroySensors);

        break;
    }

    default:
        break;
    }
}

template<jnipp::return_type T>
static std::string GetBuildField(wrapping::jfield<T>&& field)
{
    using namespace ::jnipp_operators;

    java::object fieldValue =
        *"android.os.Build"_jclass[field.as("java.lang.String")];

    return java::type_unwrapper<std::string>(fieldValue);
}

template<jnipp::return_type T>
static std::string GetBuildVersionField(wrapping::jfield<T>&& field)
{
    using namespace ::jnipp_operators;

    java::object fieldValue =
        *"android.os.Build$VERSION"_jclass[field.as("java.lang.String")];

    return java::type_unwrapper<std::string>(fieldValue);
}

static void AndroidForeignSignalHandleNA(int evtype, void* p1, void*, void*)
{
    using namespace ::jnipp_operators;

    /*
    switch(evtype)
    {
    case CoffeeForeign_RequestPlatformData: {
        auto out = C_FCAST<AndroidForeignCommand*>(p1);
        switch(out->type)
        {
        case Android_QueryAPI:
            out->data.scalarI64 = coffee_app->activity->sdkVersion;
            break;

        case Android_QueryDataPath:
            out->store_string = coffee_app->activity->internalDataPath;
            break;
        case Android_QueryExternalDataPath:
            out->store_string = coffee_app->activity->externalDataPath;
            break;
        case Android_QueryCachePath: {
            auto Context     = "android.content.Context"_jclass;
            auto File        = "java.io.File"_jclass;
            auto getCacheDir = "getCacheDir"_jmethod.ret("java.io.File");
            auto getAbsolutePath =
                "getAbsolutePath"_jmethod.ret("java.lang.String");

            auto context  = Context(coffee_app->activity->clazz);
            auto cacheDir = File(context[getCacheDir]());

            out->store_string =
                java::type_unwrapper<std::string>(cacheDir[getAbsolutePath]());
            break;
        }
#if ANDROID_API_LEVEL >= 13
        case Android_QueryObbPath:
            out->store_string = coffee_app->activity->obbPath;
            break;
#endif

        case Android_QueryPlatformABIs:
            out->store_string = {};

            for(auto abi : android::cpu_abis())
                out->store_string += abi + " ";

            out->store_string = str::trim::both(out->store_string);

            break;

        case Android_QueryReleaseName:
            out->store_string = Strings::fmt(
                "{0} ({1})",
                GetBuildVersionField("RELEASE"_jfield),
                (coffee_app->activity->sdkVersion >= 23)
                    ? GetBuildVersionField("SECURITY_PATCH"_jfield)
                    : CString());
            break;
        case Android_QueryDeviceBoardName:
            out->store_string = GetBuildField("BOARD"_jfield);
            break;
        case Android_QueryDeviceBrand:
            out->store_string = GetBuildField("BRAND"_jfield);
            break;
        case Android_QueryDeviceName:
            out->store_string = GetBuildField("MODEL"_jfield);
            break;
        case Android_QueryDeviceManufacturer:
            out->store_string = GetBuildField("MANUFACTURER"_jfield);
            break;
        case Android_QueryDeviceProduct:
            out->store_string = GetBuildField("PRODUCT"_jfield);
            break;

        case Android_QueryMaxMemory:
            struct sysinfo inf;
            sysinfo(&inf);
            out->data.scalarI64 = C_FCAST<i64>(inf.totalram * inf.mem_unit);
            break;

        case Android_QueryDeviceDPI:
            out->data.scalarI64 = android::app_dpi();
            break;

        case Android_QueryNativeWindow:
            out->data.ptr = coffee_app->window;
            cDebug("Window ptr: {0}", coffee_app->window);
            break;
        case Android_QueryActivity:
            out->data.ptr = coffee_app->activity->clazz;
            break;
        case Android_QueryAssetManager:
            out->data.ptr = coffee_app->activity->assetManager;
            break;
        case Android_QueryApp:
            out->data.ptr = coffee_app;
            break;

        case Android_QueryStartActivity:
            out->store_string = jnipp::java::objects::get_class(
                {{}, coffee_app->activity->clazz});

        default:
            break;
        }

        break;
    }
    case CoffeeForeign_GetWinSize: {
        int* winSize = C_FCAST<int*>(p1);

        winSize[0] = ANativeWindow_getWidth(coffee_app->window);
        winSize[1] = ANativeWindow_getHeight(coffee_app->window);

        break;
    }
    }
    */
}

STATICINLINE void GetExtras()
{
    using namespace jnipp_operators;

    jobject native_app = coffee_app->activity->clazz;

    /* CLasses */
    auto NativeActivity = "android.app.NativeActivity"_jclass;

    /* Methods */

    auto activityObject = NativeActivity(native_app);

    {
        /* Get display DPI */

        cDebug("Display DPI: {0}", android::app_dpi());
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

STATICINLINE void InitializeState(struct android_app* state)
{
    using namespace jnipp_operators;

    coffee_app = state;

    ScopedJNI jni(coffee_app->activity->vm);
    jnipp::SwapJNI(&jni);

    app_internal_state = new AndroidInternalState;

    app_internal_state->currentState = AndroidApp_Hidden;

    CoffeeForeignSignalHandle   = AndroidForeignSignalHandle;
    CoffeeForeignSignalHandleNA = AndroidForeignSignalHandleNA;

    state->onAppCmd     = AndroidHandleAppCmd;
    state->onInputEvent = AndroidHandleInputCmd;

    Coffee::SetPrintingVerbosity(15);

    auto activityName = jnipp::java::objects::get_class(
        jnipp::java::object({}, state->activity->clazz));

    cDebug("State:       {0}", str::print::pointerify(state));
    cDebug("Activity:    {0}", activityName);
    cDebug("Android API: {0}", state->activity->sdkVersion);

    auto memory = *android::activity_manager().get_mem_info();
    cDebug("System memory: {0}", memory.total);

    auto stats = android::network_stats().query();

    GetExtras();

    jnipp::SwapJNI(nullptr);
}

STATICINLINE void HandleEvents()
{
    int ident;
    int events;

    struct android_poll_source* source;

    while((ident = ALooper_pollAll(0, nullptr, &events, (void**)&source)) >= 0)
    {
        if(source != nullptr)
            source->process(coffee_app, source);

        if(ident == LOOPER_ID_USER)
        {
            cDebug("User event");
        }

        if(coffee_app->destroyRequested)
        {
            CoffeeEventHandleCall(CoffeeHandle_Cleanup);
            return;
        }
    }
}

STATICINLINE void StartEventProcessing()
{
    ScopedJNI jni(coffee_app->activity->vm);
    jnipp::SwapJNI(&jni);

    while(1)
    {
        int timeout = -1;

        if(app_internal_state->currentState == AndroidApp_Visible)
            timeout = 0;

        HandleEvents();

        if(app_internal_state->currentState & AndroidApp_Visible)
        {
            CoffeeEventHandleCall(CoffeeHandle_Loop);
        }
    }

    jnipp::SwapJNI(nullptr);
}

} // namespace Coffee

/*
 *
 * JNI functions
 *
 */

namespace android {

using namespace Coffee;
using namespace jnipp_operators;
using namespace platform::url;
using re = jnipp::return_type;

intent::intent() : m_intent({{}, {}})
{
    auto activity =
        "android.app.NativeActivity"_jclass(coffee_app->activity->clazz);
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

    auto categorySet = Set(categories);
    auto categoryArray =
        jnipp::java::array_type_unwrapper<re::object_>(categorySet[toArray]());

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
    auto setArray =
        "toArray"_jmethod.ret<re::object_array_>("java.lang.Object");

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

            std::string value =
                jnipp::java::type_unwrapper<std::string>(extraVal);
            out[key_s] = value;
        }
    }

    return out;
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
    return constructors::MkSysUrl(coffee_app->activity->externalDataPath);
}

stl_types::Optional<::jnipp::java::object> app_info::get_service(
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

stl_types::Optional<network_stats::result_t> network_stats::query(
    network_class net)
{
    auto System            = "java.lang.System"_jclass;
    auto currentTimeMillis = "currentTimeMillis"_jmethod.ret<re::long_>();

    auto NetStats     = "android.app.usage.NetworkStatsManager"_jclass;
    auto querySummary = "querySummary"_jmethod.arg<jint>()
                            .arg("java.lang.String")
                            .arg<jlong>()
                            .arg<jlong>()
                            .ret("android.app.usage.NetworkStats");

    auto NetworkStats = "android.app.usage.NetworkStats"_jclass;
    auto getNextBucket =
        "getNextBucket"_jmethod.arg("android.app.usage.NetworkStats$Bucket")
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

    auto stats = NetworkStats(
        net_stats[querySummary](net, *sub_id, 0, now));

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

stl_types::Optional<activity_manager::memory_info> activity_manager::
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
        .total = C_FCAST<libc_types::u64>(*mem_info[total]),
    };
}

stl_types::Optional<activity_manager::config_info> activity_manager::
    get_config_info()
{
    auto Activity   = "android.app.ActivityManager"_jclass;
    auto ConfigInfo = "android.app.ActivityManager$ConfigurationInfo"_jclass;

    auto getDeviceConfigurationInfo =
        "getDeviceConfigurationInfo"_jmethod.arg(ConfigInfo);

    return {};
}

stl_types::Optional<activity_manager::window_info> activity_manager::window()
{
    return window_info{
        .activity = Coffee::coffee_app->activity,
        .window = Coffee::coffee_app->window,
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

int app_dpi()
{
    auto activityObject =
        "android.app.NativeActivity"_jclass(coffee_app->activity->clazz);

    auto Resources      = "android.content.res.Resources"_jclass;
    auto DisplayMetrics = "android.util.DisplayMetrics"_jclass;

    auto getResources =
        "getResources"_jmethod.ret("android.content.res.Resources");
    auto getDisplayMetrics =
        "getDisplayMetrics"_jmethod.ret("android.util.DisplayMetrics");

    auto resourceObject = Resources(activityObject[getResources]());
    auto displayMetrics = DisplayMetrics(resourceObject[getDisplayMetrics]());

    auto displayDpi = *displayMetrics["densityDpi"_jfield.as<re::int_>()];

    return displayDpi;
}

} // namespace android

void android_main(struct android_app* state)
{
    Coffee::InitializeState(state);
    Coffee::SetPrintingVerbosity(15);
    Coffee::StartEventProcessing();
}
