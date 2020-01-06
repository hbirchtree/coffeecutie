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

using namespace jnipp;

namespace android {

} // namespace android

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

C_FLAGS(AndroidAppState, Coffee::u32);

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

extern CoffeeMainWithArgs android_entry_point;
extern void*              coffee_event_handling_data;
extern "C" int deref_main_c(int (*mainfun)(int, char**), int argc, char** argv);

namespace Coffee {

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
    case APP_CMD_START:
    {
        break;
    }
    case APP_CMD_RESUME:
    case APP_CMD_PAUSE:
        //    case APP_CMD_DESTROY:
    case APP_CMD_STOP:
    {
        cDebug("Lifecycle event triggered: {0}", event);
        break;
    }

        /* Lifecycle events we care about */
    case APP_CMD_INIT_WINDOW:
    {
        if(!(app_internal_state->currentState & AndroidApp_Initialized))
        {
            deref_main_c(android_entry_point, 0, nullptr);
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
    case APP_CMD_WINDOW_RESIZED:
    {
        struct CfGeneralEvent gev;
        gev.type = CfResizeEvent;

        struct CfResizeEventData rev;
        rev.w = C_FCAST<u32>(ANativeWindow_getWidth(app->window));
        rev.h = C_FCAST<u32>(ANativeWindow_getHeight(app->window));

        CoffeeEventHandleNACall(CoffeeHandle_GeneralEvent, &gev, &rev, nullptr);

        break;
    }

    case APP_CMD_GAINED_FOCUS:
    {
        CoffeeEventHandleCall(CoffeeHandle_IsForeground);

        app_internal_state->currentState =
            AndroidApp_Visible | AndroidApp_Initialized;
        break;
    }
    case APP_CMD_LOST_FOCUS:
    {
        CoffeeEventHandleCall(CoffeeHandle_IsBackground);

        app_internal_state->currentState =
            AndroidApp_Hidden | AndroidApp_Initialized;
        break;
    }
    case APP_CMD_TERM_WINDOW:
    {
        CoffeeEventHandleCall(CoffeeHandle_Cleanup);

        libc::signal::exit(libc::signal::sig::abort);

        //        auto exit_func = CmdInterface::BasicTerm::GetAtExit();

        //        for(auto func : exit_func)
        //            func();

        break;
    }

        /* Special events */
    case APP_CMD_LOW_MEMORY:
    {
        CoffeeEventHandleCall(CoffeeHandle_LowMem);
        break;
    }

    case APP_CMD_DESTROY:
    {
        break;
    }

    default:
    {
        cWarning("Unhandled native event: {0}", event);
        break;
    }
    }
}

inline void AndroidForwardInputEvent(AInputEvent* event)
{
    auto& entities    = comp_app::createContainer();
    auto  android_bus = entities.service<anative::AndroidEventBus>();

    if(!android_bus)
        return;

    android_bus->handleInputEvent(event);
}

int32_t AndroidHandleInputCmd(
    struct android_app* app, struct AInputEvent* event)
{
    pthread_mutex_lock(&app->mutex);

    AndroidForwardInputEvent(event);

    switch(AInputEvent_getType(event))
    {
    case AINPUT_EVENT_TYPE_KEY:
    {
        auto action  = AKeyEvent_getAction(event);
        auto keyCode = AKeyEvent_getKeyCode(event);

        auto flags = AKeyEvent_getFlags(event);

        /* Just drop fake inputs, what use could they be? */
        if(!(flags & AKEY_EVENT_FLAG_FROM_SYSTEM))
            break;

        /* TODO: Handle soft keyboard inputs specifically */
        if(flags & AKEY_EVENT_FLAG_SOFT_KEYBOARD)
        {
            break;
        }

        if(flags & AKEY_EVENT_FLAG_VIRTUAL_HARD_KEY)
            cDebug("Virtual hard key");

        cDebug("Key event action: {0}:{1}:{2}", action, keyCode, flags);

        break;
    }
    case AINPUT_EVENT_TYPE_MOTION:
    {
        int32_t actionPointerIndex = AMotionEvent_getAction(event);

        uint8_t action = actionPointerIndex & AMOTION_EVENT_ACTION_MASK;
        uint8_t pointerIdx =
            (actionPointerIndex & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK) >> 8;

        auto& input_data = app_internal_state->input;

        auto& tap    = input_data.tap;
        auto& dubtap = input_data.double_tap;
        auto& drag   = input_data.drag;
        auto& pinch  = input_data.pinch;

        auto flag = ndk_helper::GESTURE_STATE_START;
        u32  dflg;

        CfGeneralEvent gev = {};
        gev.type           = CfTouchEvent;

        CfTouchEventData tev = {};
        tev.type             = CfTouch_None;

        auto tapCoord = PtF{AMotionEvent_getX(event, pointerIdx),
                            AMotionEvent_getY(event, pointerIdx)}
                            .convert<u32>();

        if(dubtap.Detect(event) & flag)
        {
            cVerbose(INPUT_VERB, "Double tap: {0}", tapCoord);

            tev.type = CfTouchType::CfTouchTap;

            tev.event.tap.x         = tapCoord.x;
            tev.event.tap.y         = tapCoord.y;
            tev.event.tap.doubleTap = 1;
        } else if(tap.Detect(event) & flag)
        {
            cVerbose(
                INPUT_VERB,
                "Tap: {0},{1}",
                AMotionEvent_getX(event, pointerIdx),
                AMotionEvent_getY(event, pointerIdx));

            tev.type = CfTouchType::CfTouchTap;

            tev.event.tap.x         = tapCoord.x;
            tev.event.tap.y         = tapCoord.y;
            tev.event.tap.doubleTap = 0;
        } else if((dflg =
                       (pinch.Detect(event) &
                        (flag | ndk_helper::GESTURE_STATE_MOVE))))
        {
            ndk_helper::Vec2 p1, p2;

            static Vecf4 initial_points;

            Vecf4 points;

            if(!pinch.GetPointers(p1, p2))
                break;

            p1.Value(points.x(), points.y());
            p2.Value(points.z(), points.w());

            if(dflg == ndk_helper::GESTURE_STATE_START)
                initial_points = points;

            cVerbose(
                INPUT_VERB,
                "Pinch: {0} ({1})",
                points,
                AMotionEvent_getHistorySize(event));

            PtF pinch_point = {(points.x() + points.z()) / 2.f,
                               (points.y() + points.w()) / 2.f};

            tev.type          = CfTouchType::CfTouchPinch;
            tev.event.pinch.x = C_CAST<u32>(pinch_point.x);
            tev.event.pinch.y = C_CAST<u32>(pinch_point.y);

            if(dflg == ndk_helper::GESTURE_STATE_START)
                tev.event.pinch.factor = 1.f;
            else
            {
                Vecf2 v1 = {points.x(), points.y()};
                Vecf2 v2 = {points.z(), points.w()};

                Vecf2 v1_i = {initial_points.x(), initial_points.y()};
                Vecf2 v2_i = {initial_points.z(), initial_points.w()};

                auto dist1 = length(v1 - v2);
                auto dist2 = length(v1_i - v2_i);

                if(dist2 == 0.f)
                    dist2 = 0.01f;

                tev.event.pinch.factor = {dist1 / dist2};

                /* TODO: Allow this process to emit rotation gestures */
                /* TODO: Allow this to fall over into a drag gesture? */
            }

        } else if((dflg =
                       (drag.Detect(event) &
                        (flag | ndk_helper::GESTURE_STATE_MOVE))))
        {
            ndk_helper::Vec2 p;
            Vecf2            point;

            static Vecf2 initial_point;

            drag.GetPointer(p);
            p.Value(point.x(), point.y());

            if(dflg == ndk_helper::GESTURE_STATE_START)
                initial_point = {};

            cVerbose(INPUT_VERB, "Drag: {0} -> {1}", initial_point, point);

            tev.type         = CfTouchType::CfTouchPan;
            tev.event.pan.ox = C_CAST<u32>(point.x());
            tev.event.pan.oy = C_CAST<u32>(point.y());
            tev.event.pan.dx = C_CAST<i32>(initial_point.x() - point.x());
            tev.event.pan.dy = C_CAST<i32>(initial_point.y() - point.y());
            tev.event.pan.fingerCount = 1;

            initial_point = point;
        } else
        {
            cVerbose(
                INPUT_VERB, "Motion event: {0},{1}", tapCoord.x, tapCoord.y);
        }

        if(tev.type != CfTouch_None)
        {
            CoffeeEventHandleNACall(
                CoffeeHandle_GeneralEvent, &gev, &tev, nullptr);
        }

        //        cDebug("Motion event: {0}:{1} @ {2},{3}", pointerIdx, action,
        //        0, 0);
        break;
    }
    default:
    {
        pthread_mutex_unlock(&app->mutex);
        return 0;
    }
    }

    pthread_mutex_unlock(&app->mutex);

    return 1;
}

static void AndroidForeignSignalHandle(int evtype)
{
    switch(evtype)
    {
    case CoffeeForeign_ActivateMotion:
    {
        /* TODO: Fix sensor code */
        //        Sensor::Android::Android_InitSensors();
        //        libc::signal::register_atexit(Sensor::Android::Android_DestroySensors);

        break;
    }

    default:
        break;
    }
}

template<typename T>
static std::string GetBuildField(wrapping::jfield<T>&& field)
{
    using namespace ::jnipp_operators;

    ScopedJNI jni(coffee_app->activity->vm);
    jnipp::SwapJNI(&jni);

    jvalue fieldValue =
        *"android.os.Build"_jclass[field.as("java.lang.String")];

    return java::type_unwrapper<std::string>(fieldValue);
}

template<typename T>
static std::string GetBuildVersionField(wrapping::jfield<T>&& field)
{
    using namespace ::jnipp_operators;

    ScopedJNI jni(coffee_app->activity->vm);
    jnipp::SwapJNI(&jni);

    jvalue fieldValue =
        *"android.os.Build$VERSION"_jclass[field.as("java.lang.String")];

    return java::type_unwrapper<std::string>(fieldValue);
}

static void AndroidForeignSignalHandleNA(int evtype, void* p1, void*, void*)
{
    using namespace ::jnipp_operators;

    android::ScopedJNI _(coffee_app->activity->vm);
    jnipp::SwapJNI(&_);

    switch(evtype)
    {
    case CoffeeForeign_RequestPlatformData:
    {
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
        case Android_QueryCachePath:
            Throw(implementation_error("deprecated function"));
            break;
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
                "Android {0} ({1})",
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
            out->store_string =
                jnipp::java::objects::get_class(coffee_app->activity->clazz);

        default:
            break;
        }

        break;
    }
    case CoffeeForeign_GetWinSize:
    {
        int* winSize = C_FCAST<int*>(p1);

        winSize[0] = ANativeWindow_getWidth(coffee_app->window);
        winSize[1] = ANativeWindow_getHeight(coffee_app->window);

        break;
    }
    }
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

            platform::Env::SetVar(e.first.c_str(), e.second.c_str());
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

    auto activityName = jnipp::java::objects::get_class(state->activity->clazz);

    cDebug("State:       {0}", str::print::pointerify(state));
    cDebug("Activity:    {0}", activityName);
    cDebug("Android API: {0}", state->activity->sdkVersion);

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
    while(1)
    {
        int timeout = -1;

        if(app_internal_state->currentState == AndroidApp_Visible)
            timeout = 0;

        HandleEvents();

        if(app_internal_state->currentState & AndroidApp_Visible)
            CoffeeEventHandleCall(CoffeeHandle_Loop);
    }
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

intent::intent() : m_intent(nullptr, nullptr)
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

    return jnipp::java::type_unwrapper<std::string>(m_intent[getAction]());
}

std::string intent::data()
{
    if(!java::objects::not_null(m_intent))
        return {};

    auto Uri = "android.net.Uri"_jclass;

    auto getData  = "getData"_jmethod.ret("android.net.Uri");
    auto toString = "toString"_jmethod.ret("java.lang.String");

    auto intentData = m_intent[getData]();

    if(!java::objects::not_null(intentData))
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
    auto toArray = "toArray"_jmethod.ret<jobjectArray>("java.lang.Object");

    auto categories = m_intent[getCategories]();

    if(!java::objects::not_null(categories))
        return {};

    auto categorySet = Set(categories);
    auto categoryArray =
        jnipp::java::array_type_unwrapper<jobjectArray>(categorySet[toArray]());

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
    auto keySet   = "keySet"_jmethod.ret("java.util.Set");
    auto setArray = "toArray"_jmethod.ret<jobjectArray>("java.lang.Object");

    auto extrasRef = m_intent[getExtras]().l;

    if(jnipp::java::objects::not_null(extrasRef))
    {
        auto extras       = Bundle(extrasRef);
        auto extrasKeySet = Set(extras[keySet]().l);

        auto extraKeys = jnipp::java::array_type_unwrapper<jobjectArray>(
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

    auto getFlags = "getFlags"_jmethod.ret<jint>();

    return jnipp::java::type_unwrapper<jint>(m_intent[getFlags]());
}

std::vector<std::string> cpu_abis()
{
    auto Build = "android.os.Build"_jclass;

    std::vector<std::string> out;

    try
    {
        auto SUPPORTED_ABIS =
            "SUPPORTED_ABIS"_jfield.as<jobjectArray>("java.lang.String");

        auto abis = jnipp::java::array_type_unwrapper<jobjectArray>(
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

    auto resourceObject = Resources(activityObject[getResources]().l);
    auto displayMetrics = DisplayMetrics(resourceObject[getDisplayMetrics]().l);

    auto displayDpi = C_CAST<jint>(jnipp::java::type_unwrapper<jint>(
        *displayMetrics["densityDpi"_jfield.as<jint>()]));

    return displayDpi;
}

} // namespace android

void android_main(struct android_app* state)
{
    Coffee::InitializeState(state);
    Coffee::SetPrintingVerbosity(15);
    Coffee::StartEventProcessing();
}
