#include <coffee/anative/anative_comp.h>

#include <coffee/android/android_main.h>
#include <coffee/comp_app/app_events.h>
#include <coffee/foreign/foreign.h>

#include <coffee/strings/libc_types.h>

#include <coffee/core/CDebug>

#include <android/input.h>
#include <android/keycodes.h>
#include <android/native_window.h>
#include <gestureDetector.h>

#include "/home/havard/.local/android-sdk-linux/ndk/25.1.8937393/sources/android/native_app_glue/android_native_app_glue.h"
#include "/home/havard/.local/android-sdk-linux/ndk/25.1.8937393/toolchains/llvm/prebuilt/linux-x86_64/sysroot/usr/include/android/input.h"
#include "/home/havard/.local/android-sdk-linux/ndk/25.1.8937393/toolchains/llvm/prebuilt/linux-x86_64/sysroot/usr/include/android/native_activity.h"
#include "/home/havard/.local/android-sdk-linux/ndk/25.1.8937393/toolchains/llvm/prebuilt/linux-x86_64/sysroot/usr/include/android/native_window.h"

namespace anative {

void Windowing::load(entity_container& e, comp_app::app_error& ec)
{
    auto window_info = android::activity_manager().window();

    if(!window_info || !window_info->window)
    {
        ec = "ANativeWindow not available";
        ec = comp_app::AppError::NoWindow;
        return;
    }

#if ANDROID_API_LEVEL >= 23
    if(window_info)
        ANativeActivity_setWindowFlags(
            (*window_info).activity,
            AWINDOW_FLAG_FULLSCREEN | AWINDOW_FLAG_KEEP_SCREEN_ON,
            AWINDOW_FLAG_FULLSCREEN | AWINDOW_FLAG_KEEP_SCREEN_ON);
#endif

    auto windowInfo = e.service<comp_app::PtrNativeWindowInfoService>();
    if(!windowInfo)
    {
        ec = "Missing comp_app::PtrNativeWindowInfo service";
        ec = comp_app::AppError::ServiceMissing;
        return;
    }

    windowInfo->window = window_info->window;
}

comp_app::size_2d_t Windowing::size() const
{
    return {0, 0};
}

comp_app::detail::WindowState Windowing::state() const
{
    return comp_app::detail::WindowState::Normal;
}

void Windowing::setState(comp_app::detail::WindowState state)
{
}

void Windowing::close()
{
#if ANDROID_API_LEVEL >= 23
    if(auto window = android::activity_manager().window())
        ANativeActivity_finish((*window).activity);
#endif
}

libc_types::u32 ControllerInput::count() const
{
    return m_cache.size();
}
ControllerInput::controller_map ControllerInput::state(
    libc_types::u32 idx) const
{
    return m_cache.at(idx);
}
comp_app::text_type_t ControllerInput::name(libc_types::u32 idx) const
{
    return "Generic Controller";
}

void KeyboardInput::openVirtual() const
{
#if ANDROID_API_LEVEL >= 23
    if(auto window = android::activity_manager().window())
        ANativeActivity_showSoftInput(
            (*window).activity, ANATIVEACTIVITY_SHOW_SOFT_INPUT_IMPLICIT);
#endif
}

void KeyboardInput::closeVirtual() const
{
#if ANDROID_API_LEVEL >= 23
    if(auto window = android::activity_manager().window())
        ANativeActivity_hideSoftInput(
            (*window).activity, ANATIVEACTIVITY_HIDE_SOFT_INPUT_IMPLICIT);
#endif
}

void AndroidEventBus::load(entity_container& e, comp_app::app_error&)
{
    m_container = &e;

    m_tapDetector    = new ndk_helper::TapDetector;
    m_doubleDetector = new ndk_helper::DoubletapDetector;
    m_pinchDetector  = new ndk_helper::PinchDetector;
    m_dragDetector   = new ndk_helper::DragDetector;

    m_tapDetector->SetConfiguration(android::app_info().configuration());
    m_doubleDetector->SetConfiguration(android::app_info().configuration());
}

void AndroidEventBus::handleGamepadEvent(AInputEvent* event)
{
    using namespace libc_types;

    i32 type     = AInputEvent_getType(event);
    i32 source   = AInputEvent_getSource(event);
    i32 deviceId = AInputEvent_getDeviceId(event);

    auto controllers = m_container->service<anative::ControllerInput>();
    auto it          = controllers->m_mapping.find(deviceId);

    if(it == controllers->m_mapping.end())
    {
        it = controllers->m_mapping
                 .insert({deviceId, controllers->m_cache.size() - 1})
                 .first;
        controllers->m_cache.push_back({});
    }

    auto& c = controllers->m_cache.at(it->second);

    auto& axes    = c.axes.e;
    auto& buttons = c.buttons.e;

    switch(type)
    {
    case AINPUT_EVENT_TYPE_KEY: {
        i32 button = AKeyEvent_getKeyCode(event);
        i32 action = AKeyEvent_getAction(event);
        i32 flags  = AKeyEvent_getFlags(event);
        i32 meta   = AKeyEvent_getMetaState(event);

        bool pressed = action == AKEY_EVENT_ACTION_DOWN;

        switch(button)
        {
        case AKEYCODE_BUTTON_X:
            buttons.x = pressed;
            break;
        case AKEYCODE_BUTTON_Y:
            buttons.y = pressed;
            break;
        case AKEYCODE_BUTTON_A:
            buttons.a = pressed;
            break;
        case AKEYCODE_BUTTON_B:
            buttons.b = pressed;
            break;

        case AKEYCODE_BUTTON_L1:
            buttons.s_l = pressed;
            break;
        case AKEYCODE_BUTTON_L2:
            axes.t_l = pressed ? convert_f32<i16>(1) : convert_f32<i16>(0);
            break;
        case AKEYCODE_BUTTON_R1:
            buttons.s_r = pressed;
            break;
        case AKEYCODE_BUTTON_R2:
            axes.t_r = pressed ? convert_f32<i16>(1) : convert_f32<i16>(0);
            break;

        case AKEYCODE_BUTTON_THUMBL:
            buttons.s_l = pressed;
            break;
        case AKEYCODE_BUTTON_THUMBR:
            buttons.s_r = pressed;
            break;

        case AKEYCODE_BUTTON_SELECT:
            buttons.back = pressed;
            break;
        case AKEYCODE_BUTTON_START:
            buttons.start = pressed;
            break;
        case AKEYCODE_BUTTON_MODE:
            buttons.guide = pressed;
            break;

        default:
            Throw(undefined_behavior("unhandled controller button"));
        }

        break;
    }
    case AINPUT_EVENT_TYPE_MOTION: {
        axes.l_x = convert_f32<i16>(
            AMotionEvent_getAxisValue(event, AMOTION_EVENT_AXIS_X, 0));
        axes.l_y = convert_f32<i16>(
            AMotionEvent_getAxisValue(event, AMOTION_EVENT_AXIS_Y, 0));

        axes.r_x = convert_f32<i16>(
            AMotionEvent_getAxisValue(event, AMOTION_EVENT_AXIS_Z, 0));
        axes.r_y = convert_f32<i16>(
            AMotionEvent_getAxisValue(event, AMOTION_EVENT_AXIS_RZ, 0));

        axes.t_l = convert_f32<i16>(
            AMotionEvent_getAxisValue(event, AMOTION_EVENT_AXIS_LTRIGGER, 0));
        axes.t_r = convert_f32<i16>(
            AMotionEvent_getAxisValue(event, AMOTION_EVENT_AXIS_RTRIGGER, 0));

        buttons.p_down
            = AMotionEvent_getAxisValue(event, AMOTION_EVENT_AXIS_HAT_Y, 0)
              < -.5f;
        buttons.p_up
            = AMotionEvent_getAxisValue(event, AMOTION_EVENT_AXIS_HAT_Y, 0)
              > .5f;
        buttons.p_left
            = AMotionEvent_getAxisValue(event, AMOTION_EVENT_AXIS_HAT_X, 0)
              < -.5f;
        buttons.p_right
            = AMotionEvent_getAxisValue(event, AMOTION_EVENT_AXIS_HAT_X, 0)
              > .5f;

        break;
    }
    }
}

void AndroidEventBus::handleKeyEvent(AInputEvent* event)
{
    using namespace libc_types;

    i32 source   = AInputEvent_getSource(event);
    i32 deviceId = AInputEvent_getDeviceId(event);

    if(source & AINPUT_SOURCE_TOUCH_NAVIGATION)
    {
        i32 button = AKeyEvent_getKeyCode(event);
        i32 action = AKeyEvent_getAction(event);
        i32 flags  = AKeyEvent_getFlags(event);
        i32 meta   = AKeyEvent_getMetaState(event);

        switch(button)
        {
        case AKEYCODE_BACK: {
            using AppBus    = comp_app::BasicEventBus<comp_app::AppEvent>;
            AppBus* app_bus = m_container->service<AppBus>();
            if(!app_bus)
                break;
            comp_app::AppEvent event{comp_app::AppEvent::NavigationEvent};
            comp_app::NavigationEvent nav{comp_app::NavigationEvent::Back};
            app_bus->inject(event, &nav);
            break;
        }
        default:
            break;
        }
    } else if(source & AINPUT_SOURCE_KEYBOARD)
    {
        i32 button  = AKeyEvent_getKeyCode(event);
        i32 action  = AKeyEvent_getAction(event);
        i32 flags   = AKeyEvent_getFlags(event);
        i32 meta    = AKeyEvent_getMetaState(event);
        i32 repeats = AKeyEvent_getRepeatCount(event);
    }
}

void AndroidEventBus::handleMotionEvent(AInputEvent* event)
{
    using namespace libc_types;

    i32 source   = AInputEvent_getSource(event);
    i32 deviceId = AInputEvent_getDeviceId(event);

    switch(source)
    {
    case AINPUT_SOURCE_DPAD: {
        float x = AMotionEvent_getAxisValue(event, AMOTION_EVENT_AXIS_HAT_X, 0);
        float y = AMotionEvent_getAxisValue(event, AMOTION_EVENT_AXIS_HAT_Y, 0);
        break;
    }
    case AINPUT_SOURCE_GAMEPAD: {
        break;
    }
    case AINPUT_SOURCE_JOYSTICK: {
        float x = AMotionEvent_getAxisValue(event, AMOTION_EVENT_AXIS_X, 0);
        float y = AMotionEvent_getAxisValue(event, AMOTION_EVENT_AXIS_Y, 0);
        break;
    }
    case AINPUT_SOURCE_TOUCHSCREEN: {
        i32 edgeFlags = AMotionEvent_getEdgeFlags(event);
        filterTouchEvent(event);
        break;
    }
    }

    Coffee::cDebug("Motion event: {0}", event);
}

void AndroidEventBus::handleInputEvent(AInputEvent* event)
{
    using namespace libc_types;

    i32 type   = AInputEvent_getType(event);
    i32 source = AInputEvent_getSource(event);

    switch(source)
    {
    case AINPUT_SOURCE_GAMEPAD:
    case AINPUT_SOURCE_JOYSTICK:
        handleGamepadEvent(event);
        return;
    default:
        break;
    }

    switch(type)
    {
    case AINPUT_EVENT_TYPE_KEY: {
        handleKeyEvent(event);
        break;
    }
    case AINPUT_EVENT_TYPE_MOTION: {
        handleMotionEvent(event);
        break;
    }
    }
}

bool AndroidEventBus::filterTouchEvent(AInputEvent* event)
{
    using namespace ndk_helper;

    using namespace Coffee::Input;

    using IBus     = comp_app::BasicEventBus<CIEvent>;
    IBus* inputBus = m_container->service<IBus>();
    CIEvent out;

    if(m_pinchDetector->Detect(event) != GESTURE_STATE_NONE)
    {
        // TODO
//        ndk_helper::Vec2 v1, v2;
//        m_pinchDetector->GetPointers(v1, v2);

//        out.type = CIEvent::TouchPinch;
//        CITouchPinchEvent pinch;
//        inputBus->inject(out, &pinch);
    }
    if(m_dragDetector->Detect(event) != GESTURE_STATE_NONE)
    {
    }
    if(m_doubleDetector->Detect(event) != GESTURE_STATE_NONE)
    {
        CITouchTapEvent tap;
        tap.pos = {AMotionEvent_getX(event, 0), AMotionEvent_getY(event, 0)};
        tap.pressed = true;
        inputBus->inject(out, &tap);
    }
    if(m_tapDetector->Detect(event) != GESTURE_STATE_NONE)
    {
        CITouchTapEvent tap;
        tap.pos = {AMotionEvent_getX(event, 0), AMotionEvent_getY(event, 0)};
        tap.pressed = true;
        inputBus->inject(out, &tap);
        /* TODO: Emit release event */
    }
}

void AndroidEventBus::handleWindowEvent(android_app* app, libc_types::i32 event)
{
    Coffee::cDebug("App event: {0}", event);

    switch(static_cast<decltype(APP_CMD_START)>(event))
    {
    case APP_CMD_START:
    case APP_CMD_RESUME:
        emitLifecycleEvent(
            comp_app::LifecycleEvent{comp_app::LifecycleEvent::Foreground});
        break;
    case APP_CMD_PAUSE:
    case APP_CMD_STOP:
        emitLifecycleEvent(
            comp_app::LifecycleEvent{comp_app::LifecycleEvent::Background});
        break;

    case APP_CMD_INIT_WINDOW:
        emitLifecycleEvent(
            comp_app::LifecycleEvent{comp_app::LifecycleEvent::Startup});
        [[fallthrough]];
    case APP_CMD_WINDOW_RESIZED: {
        using DEvent            = Coffee::Display::Event;
        using DisplayBus        = comp_app::BasicEventBus<DEvent>;
        DisplayBus* display_bus = m_container->service<DisplayBus>();
        Coffee::Display::ResizeEvent resize
            = {ANativeWindow_getWidth(app->window),
               ANativeWindow_getHeight(app->window)};
        auto event = DEvent::Create(0, DEvent::Resize);
        display_bus->inject(event, &resize);
        break;
    }
    case APP_CMD_TERM_WINDOW:
        emitLifecycleEvent(
            comp_app::LifecycleEvent{comp_app::LifecycleEvent::Terminate});
        break;
    case APP_CMD_DESTROY:
        emitLifecycleEvent(
            comp_app::LifecycleEvent{comp_app::LifecycleEvent::ResourcesLost});
        break;

    case APP_CMD_INPUT_CHANGED:
        // AInputQueue changed
        break;
    case APP_CMD_GAINED_FOCUS:
    case APP_CMD_LOST_FOCUS:
        // Gaining/losing input focus to other apps
        break;

    case APP_CMD_CONFIG_CHANGED:
        // Config change, eg. rotation
        break;

    case APP_CMD_LOW_MEMORY:
        emitLifecycleEvent(
            comp_app::LifecycleEvent{comp_app::LifecycleEvent::LowMemory});
        break;

    case APP_CMD_SAVE_STATE:
        // App should save temporary state
        break;
    case APP_CMD_CONTENT_RECT_CHANGED:
    case APP_CMD_WINDOW_REDRAW_NEEDED:
        break;
    }
}

void AndroidEventBus::emitLifecycleEvent(comp_app::LifecycleEvent event)
{
    using AppBus    = comp_app::BasicEventBus<comp_app::AppEvent>;
    AppBus* app_bus = m_container->service<AppBus>();

    if(!app_bus)
        return;

    comp_app::AppEvent app_event{comp_app::AppEvent::LifecycleEvent};
    app_bus->inject(app_event, &event);
}

} // namespace anative
