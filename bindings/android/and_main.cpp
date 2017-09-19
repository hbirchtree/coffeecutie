#include <coffee/android/android_main.h>
#include <coffee/core/CDebug>

#include <coffee/core/coffee.h>

#include <android_native_app_glue.h>
#include <android/native_activity.h>
#include <android/looper.h>
#include <android/window.h>

using namespace Coffee;

enum AndroidAppState
{
    AndroidApp_Hidden,
    AndroidApp_Visible,
};

struct AndroidInternalState
{
    AndroidAppState currentState;
};

AndroidInternalState* app_internal_state = nullptr;

struct android_app* coffee_app = nullptr;

extern CoffeeMainWithArgs android_entry_point;

void* coffee_event_handling_data;

void(*CoffeeEventHandle)(void*, int);
void(*CoffeeEventHandleNA)(void*, int, void*, void*, void*);

void(*CoffeeForeignSignalHandle)(int);
void(*CoffeeForeignSignalHandleNA)(int, void*, void*, void*);

extern "C" int deref_main_c(int(*mainfun)(int, char**), int argc, char** argv);

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

static void AndroidHandleAppCmd(struct android_app* app, int32_t event);

static int32_t AndroidHandleInputCmd(struct android_app* app,
                                     struct AInputEvent* event);

static void AndroidForeignSignalHandle(int evtype);

static void AndroidForeignSignalHandleNA(int evtype, void* p1, void* p2,
                                         void* p3);

void android_main(struct android_app* state)
{
    app_dummy();

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

//    cDebug("Board name: {0}", CJ_GetStatic<CString>("/android/os/Build", "BOARD"));

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
    case APP_CMD_RESUME:
    case APP_CMD_PAUSE:
    case APP_CMD_STOP:
    case APP_CMD_DESTROY:
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
    cDebug("Input: {0}", event);
    return 0;
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

static void AndroidForeignSignalHandleNA(int evtype, void* p1, void* p2,
                                         void* p3)
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
        case Android_QueryObbPath:
            out->store_string = coffee_app->activity->obbPath;
            break;

        case Android_QueryNativeWindow:
            out->data.ptr = coffee_app->window;
            break;
        case Android_QueryAssetManager:
            out->data.ptr = coffee_app->activity->assetManager;
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
