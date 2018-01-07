#include <coffee/android/android_main.h>
#include <coffee/core/CDebug>
#include <coffee/core/CMD>

#include <coffee/core/coffee.h>
#include <coffee/core/string_casting.h>
#include <coffee/core/profiler/profiling-export.h>

#include <sys/sysinfo.h>

#include <android_native_app_glue.h>
#include <android/native_activity.h>
#include <android/looper.h>
#include <android/window.h>
#include <gestureDetector.h>

#include <coffee/core/base/renderer/eventapplication_wrapper.h>

static Coffee::Map<CString, CString> intentVariables;
static Coffee::CString Android_cacheDir;
static Coffee::CString Android_abis;
static int Android_DPI;

extern "C" {

JNIEXPORT __attribute__((used)) void
Java_me_birchtrees_CoffeeNativeActivity_smuggleVariable(
        JNIEnv* env, jobject, jint id, jstring data
        )
{
    auto string_data = env->GetStringUTFChars(data, 0);

    if(!string_data)
        return;

//    Coffee::cDebug("Data {0}: {1}", id, CString(string_data));

    switch(id)
    {
    case 10:
    {
        /* Cache directory */
        Android_cacheDir = string_data;

        break;
    }
    case 11:
    {
        /* DPI report */
        Android_DPI = Coffee::cast_string<Coffee::i32>(string_data);

        break;
    }
    case 12:
    {
        /* ABI strings */
        Android_abis = string_data;

        break;
    }

    case 13:
    {
        /* Importing Intent extras as environment variables */

        Coffee::CString sdata = string_data;
        auto split = sdata.find('=');
        if(split == Coffee::CString::npos)
            break;
        auto keyName = sdata.substr(0, split);
        auto varName = sdata.substr(split + 1, sdata.size() - split - 1);

        intentVariables[keyName] = varName;
        setenv(keyName.c_str(), varName.c_str(), 1);

        break;
    }
    default:
        break;
    }

    env->ReleaseStringUTFChars(data, string_data);
}

}

using namespace Coffee;

static const constexpr u8 INPUT_VERB = 11;

enum AndroidAppState
{
    AndroidApp_Hidden,
    AndroidApp_Visible,
};

struct AndroidSensorData
{
    ASensorManager* manager;
    const ASensor* accelerometer;
    const ASensor* gyroscope;
    ASensorEventQueue* eventQueue;
};

struct CoffeeAndroidUserData
{
    AndroidSensorData sensors;
};

struct InputDetectors
{
    ndk_helper::TapDetector tap;
    ndk_helper::DoubletapDetector double_tap;
    ndk_helper::PinchDetector pinch;
    ndk_helper::DragDetector drag;
};

struct AndroidInternalState
{
    InputDetectors input;
    AndroidAppState currentState;
    CString cachePath;
};

static AndroidInternalState* app_internal_state = nullptr;

struct android_app* coffee_app = nullptr;
static JNIEnv* coffee_jni_env;

extern CoffeeMainWithArgs android_entry_point;

void* coffee_event_handling_data;

void(*CoffeeEventHandle)(void*, int);
void(*CoffeeEventHandleNA)(void*, int, void*, void*, void*);

void(*CoffeeForeignSignalHandle)(int);
void(*CoffeeForeignSignalHandleNA)(int, void*, void*, void*);

extern "C" int deref_main_c(int(*mainfun)(int, char**), int argc, char** argv);

javavar Coffee_JavaGetStaticMember(cstring clss, cstring var, cstring type)
{
    javavar _var;
    _var.env = coffee_app->activity->env;
    if(!_var.env)
        return {};

    _var.ass = _var.env->FindClass(clss);
    _var.field = _var.env->GetStaticFieldID(_var.ass,var,type);

    return _var;
}

JNIEnv* JNIPP::GetJNI()
{
    return coffee_jni_env;
}

static void AndroidHandleAppCmd(struct android_app* app, int32_t event);

static int32_t AndroidHandleInputCmd(struct android_app* app,
                                     struct AInputEvent* event);

static void AndroidForeignSignalHandle(int evtype);

static void AndroidForeignSignalHandleNA(int evtype, void* p1, void* p2,
                                         void* p3);

static char AndroidWindowType[] = "android.view.Window";
static char AndroidViewType[] = "android.view.View";
static char javaioFile[] = "java.io.File";

void android_main(struct android_app* state)
{
    Env::SetVar("COFFEE_REPORT_URL",
                "https://coffee.birchtrees.me/reports");

    coffee_app = state;

    app_internal_state = new AndroidInternalState;

    app_internal_state->currentState = AndroidApp_Hidden;

    CoffeeForeignSignalHandle = AndroidForeignSignalHandle;
    CoffeeForeignSignalHandleNA = AndroidForeignSignalHandleNA;

    state->onAppCmd = AndroidHandleAppCmd;
    state->onInputEvent = AndroidHandleInputCmd;

    Coffee::SetPrintingVerbosity(10);

    cDebug("State: {0}", StrUtil::pointerify(state));

    cDebug("Running under API {0}", state->activity->sdkVersion);

    deref_main_c(android_entry_point, 0, nullptr);

    state->activity->vm->AttachCurrentThread(
            &coffee_jni_env, NULL);

    auto build = "android.os.Build"_jclass;

    auto var = build[FieldAs<JavaString>("BOARD"_jfield)].value();
    cDebug("Board name: {0}", var);

    var = build[FieldAs<JavaString>("BRAND"_jfield)].value();
    cDebug("Brand: {0}", var);

    var = build[FieldAs<JavaString>("PRODUCT"_jfield)].value();
    cDebug("Product: {0}", var);

    auto myClass = "android.app.NativeActivity"_jclass;

    using NativeActivity = JavaClass<javaioFile, void, void>;

    auto myInstance = myClass(state->activity->clazz);

    cDebug("JCall Start: {0}", Chrono::high_resolution_clock::now().time_since_epoch().count());
    auto cacheDirFile = myInstance["getCacheDir"_jmethod.returns<NativeActivity>()]({});
    cDebug("JCall End: {0}", Chrono::high_resolution_clock::now().time_since_epoch().count());

    cDebug("Thread: {0}", ThreadId().hash());

    while(1)
    {
        int ident;
        int events;

        struct android_poll_source* source;

        int timeout = -1;

        if(app_internal_state->currentState == AndroidApp_Visible)
            timeout = 0;

        while((ident=ALooper_pollAll(0, nullptr,
                                     &events, (void**)&source)) >= 0)
        {
            if(source != nullptr)
                source->process(state, source);

            if(ident == LOOPER_ID_USER)
            {
                cDebug("User event");
            }

            if(state->destroyRequested != 0)
            {
                CoffeeEventHandleCall(CoffeeHandle_Cleanup);
                return;
            }
        }

        if(app_internal_state->currentState == AndroidApp_Visible)
            CoffeeEventHandleCall(CoffeeHandle_Loop);
    }
}

void AndroidHandleAppCmd(struct android_app* app, int32_t event)
{
    switch(event)
    {
    case APP_CMD_START:
    {
        cDebug("App start", event);
        break;
    }
    case APP_CMD_RESUME:
    case APP_CMD_PAUSE:
    case APP_CMD_DESTROY:
    case APP_CMD_STOP:
    {
        cDebug("Lifecycle event triggered: {0}", event);
        break;
    }

        /* Lifecycle events we care about */
    case APP_CMD_INIT_WINDOW:
    {
        CoffeeEventHandleCall(CoffeeHandle_Setup);

        ANativeActivity_setWindowFlags(app->activity,
                                       AWINDOW_FLAG_FULLSCREEN|
                                       AWINDOW_FLAG_KEEP_SCREEN_ON,
                                       AWINDOW_FLAG_FULLSCREEN|
                                       AWINDOW_FLAG_KEEP_SCREEN_ON);

        /* Intentional fallthrough, we need to push a resize event */
    }
    case APP_CMD_WINDOW_RESIZED:
    {
        struct CfGeneralEvent gev;
        gev.type = CfResizeEvent;

        struct CfResizeEventData rev;
        rev.w = C_FCAST<u32>(ANativeWindow_getWidth(app->window));
        rev.h = C_FCAST<u32>(ANativeWindow_getHeight(app->window));

        CoffeeEventHandleNACall(CoffeeHandle_GeneralEvent,
                                &gev, &rev, nullptr);

        break;
    }

    case APP_CMD_GAINED_FOCUS:
    {
        CoffeeEventHandleCall(CoffeeHandle_IsForeground);

        app_internal_state->currentState = AndroidApp_Visible;
        break;
    }
    case APP_CMD_LOST_FOCUS:
    {
        CoffeeEventHandleCall(CoffeeHandle_IsBackground);

        app_internal_state->currentState = AndroidApp_Hidden;
        break;
    }
    case APP_CMD_TERM_WINDOW:
    {
        CoffeeEventHandleCall(CoffeeHandle_Cleanup);
        break;
    }

        /* Special events */
    case APP_CMD_LOW_MEMORY:
    {
        CoffeeEventHandleCall(CoffeeHandle_LowMem);
        break;
    }

    default:
    {
        cWarning("Unhandled native event: {0}", event);
        break;
    }
    }
}

int32_t AndroidHandleInputCmd(struct android_app* app,
                              struct AInputEvent* event)
{
    pthread_mutex_lock(&app->mutex);

    switch(AInputEvent_getType(event))
    {
    case AINPUT_EVENT_TYPE_KEY:
    {
        auto action = AKeyEvent_getAction(event);
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
                (actionPointerIndex &
                 AMOTION_EVENT_ACTION_POINTER_INDEX_MASK) >> 8;

        auto& input_data = app_internal_state->input;

        auto& tap = input_data.tap;
        auto& dubtap = input_data.double_tap;
        auto& drag = input_data.drag;
        auto& pinch = input_data.pinch;

        auto flag = ndk_helper::GESTURE_STATE_START;
        u32 dflg;

        CfGeneralEvent gev = {};
        gev.type = CfTouchEvent;

        CfTouchEventData tev = {};
        tev.type = CfTouch_None;

        auto tapCoord = CPointF{
            AMotionEvent_getX(event, pointerIdx),
            AMotionEvent_getY(event, pointerIdx)
        }.convert<u32>();

        if(dubtap.Detect(event) & flag)
        {
            cVerbose(INPUT_VERB, "Double tap: {0}",
                   tapCoord
                   );

            tev.type = CfTouchType::CfTouchTap;

            tev.event.tap.x = tapCoord.x;
            tev.event.tap.y = tapCoord.y;
            tev.event.tap.doubleTap = 1;
        }else
        if(tap.Detect(event) & flag)
        {
            cVerbose(INPUT_VERB, "Tap: {0},{1}",
                   AMotionEvent_getX(event, pointerIdx),
                   AMotionEvent_getY(event, pointerIdx));

            tev.type = CfTouchType::CfTouchTap;

            tev.event.tap.x = tapCoord.x;
            tev.event.tap.y = tapCoord.y;
            tev.event.tap.doubleTap = 0;
        }else
        if((dflg = (pinch.Detect(event)
                & (flag|ndk_helper::GESTURE_STATE_MOVE))))
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

            cVerbose(INPUT_VERB, "Pinch: {0} ({1})", points,
                   AMotionEvent_getHistorySize(event));

            CPointF pinch_point = {
                (points.x() + points.z()) / 2.f,
                (points.y() + points.w()) / 2.f
            };

            tev.type = CfTouchType::CfTouchPinch;
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

                tev.event.pinch.factor = { dist1 / dist2 };

                /* TODO: Allow this process to emit rotation gestures */
                /* TODO: Allow this to fall over into a drag gesture? */
            }

        }else
        if((dflg = (drag.Detect(event)
                    & (flag|ndk_helper::GESTURE_STATE_MOVE))))
        {
            ndk_helper::Vec2 p;
            Vecf2 point;

            static Vecf2 initial_point;

            drag.GetPointer(p);
            p.Value(point.x(), point.y());

            if(dflg == ndk_helper::GESTURE_STATE_START)
                initial_point = {};

            cVerbose(INPUT_VERB, "Drag: {0} -> {1}", initial_point, point);

            tev.type = CfTouchType::CfTouchPan;
            tev.event.pan.ox = C_CAST<u32>(point.x());
            tev.event.pan.oy = C_CAST<u32>(point.y());
            tev.event.pan.dx = C_CAST<i32>(initial_point.x() - point.x());
            tev.event.pan.dy = C_CAST<i32>(initial_point.y() - point.y());
            tev.event.pan.fingerCount = 1;

            initial_point = point;
        }

        if(tev.type != CfTouch_None)
        {
            CoffeeEventHandleNACall(CoffeeHandle_GeneralEvent,
                                    &gev, &tev, nullptr);
        }

//        cDebug("Motion event: {0}:{1} @ {2},{3}", pointerIdx, action, 0, 0);
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
        /* TODO: Activate motion sensors here */
        break;
    }

    default:
        break;
    }
}

static CString AndroidGetBuildField(JNIPP::JObjectField<void>&& field)
{
    auto build = "android.os.Build"_jclass;
    auto var = build[FieldAs<JavaString>(std::move(field))].value();
    return var;
}

static void AndroidForeignSignalHandleNA(int evtype, void* p1,
                                         void* p2, void* p3)
{
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
            out->store_string = Android_cacheDir;
            break;
#if ANDROID_API_LEVEL >= 13
        case Android_QueryObbPath:
            out->store_string = coffee_app->activity->obbPath;
            break;
#endif

        case Android_QueryPlatformABIs:
            out->store_string = Android_abis;
            break;

        case Android_QueryReleaseName:
            out->store_string = AndroidGetBuildField("BOARD"_jfield);
            break;
        case Android_QueryDeviceBoardName:
            out->store_string = AndroidGetBuildField("BOARD"_jfield);
            break;
        case Android_QueryDeviceBrand:
            out->store_string = AndroidGetBuildField("BRAND"_jfield);
            break;
        case Android_QueryDeviceName:
            out->store_string = AndroidGetBuildField("MODEL"_jfield);
            break;
        case Android_QueryDeviceManufacturer:
            out->store_string = AndroidGetBuildField("MANUFACTURER"_jfield);
            break;
        case Android_QueryDeviceProduct:
            out->store_string = AndroidGetBuildField("PRODUCT"_jfield);
            break;

        case Android_QueryMaxMemory:
            struct sysinfo inf;
            sysinfo(&inf);
            out->data.scalarI64 = C_FCAST<i64>(inf.totalram*inf.mem_unit);
            break;

        case Android_QueryDeviceDPI:
            out->data.scalarI64 = Android_DPI;
            break;

        case Android_QueryNativeWindow:
            out->data.ptr = coffee_app->window;
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
