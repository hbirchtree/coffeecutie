#include <coffee/anative/anative_comp.h>

#include <coffee/android/android_main.h>
#include <coffee/comp_app/app_events.h>
#include <peripherals/stl/magic_enum.hpp>

#include <coffee/strings/libc_types.h>

#include <coffee/core/CDebug>

#include <android/input.h>
#include <android/keycodes.h>
#include <android/native_activity.h>
#include <android/native_window.h>
#include <android/window.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-copy-with-user-provided-copy"
#include <gestureDetector.h>
#pragma GCC diagnostic pop

//#if 0
#include "/home/havard/.local/android-sdk-linux/ndk/25.1.8937393/sources/android/native_app_glue/android_native_app_glue.h"
#include "/home/havard/.local/android-sdk-linux/ndk/25.1.8937393/toolchains/llvm/prebuilt/linux-x86_64/sysroot/usr/include/android/input.h"
#include "/home/havard/.local/android-sdk-linux/ndk/25.1.8937393/toolchains/llvm/prebuilt/linux-x86_64/sysroot/usr/include/android/keycodes.h"
#include "/home/havard/.local/android-sdk-linux/ndk/25.1.8937393/toolchains/llvm/prebuilt/linux-x86_64/sysroot/usr/include/android/native_activity.h"
#include "/home/havard/.local/android-sdk-linux/ndk/25.1.8937393/toolchains/llvm/prebuilt/linux-x86_64/sysroot/usr/include/android/native_window.h"
#include "peripherals/stl/magic_enum.hpp"
//#endif

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
    auto window = android::activity_manager().window()->window;
    return comp_app::size_2d_t{
        .w = ANativeWindow_getWidth(window),
        .h = ANativeWindow_getHeight(window),
    };
}

comp_app::window_flags_t Windowing::state() const
{
    return comp_app::window_flags_t::fullscreen;
}

void Windowing::setState(comp_app::window_flags_t /*state*/)
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
comp_app::text_type_t ControllerInput::name(libc_types::u32 /*idx*/) const
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
            (*window).activity, ANATIVEACTIVITY_HIDE_SOFT_INPUT_IMPLICIT_ONLY);
#endif
}

bool MouseInput::mouseGrabbed() const
{
    return false;
}

void MouseInput::setMouseGrab(bool /*enabled*/)
{
}

comp_app::position_t MouseInput::position() const
{
    return m_position;
}

void MouseInput::warp(const comp_app::position_t& /*newPos*/)
{
}

MouseInput::MouseButton MouseInput::buttons() const
{
    return m_buttons;
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

void AndroidEventBus::handleMouseEvent(AInputEvent* event)
{
    using namespace libc_types;
    using namespace Coffee::Input;
    using MouseButton = MouseInput::MouseButton;
    using IBus        = comp_app::BasicEventBus<CIEvent>;

    IBus*                inputBus = m_container->service<IBus>();
    anative::MouseInput* mouse    = m_container->service<anative::MouseInput>();

    i32 type = AInputEvent_getType(event);

    switch(type)
    {
    case AINPUT_EVENT_TYPE_MOTION: {
        auto action = AMotionEvent_getAction(event);
        auto x      = AMotionEvent_getX(event, 0);
        auto y      = AMotionEvent_getY(event, 0);

        switch(action)
        {
        case AMOTION_EVENT_ACTION_BUTTON_PRESS:
        case AMOTION_EVENT_ACTION_BUTTON_RELEASE: {
            constexpr std::array<std::pair<int, MouseButton>, 3> mapping = {{
                {0x1, MouseButton::LeftButton},
                {0x2, MouseButton::RightButton},
                {0x4, MouseButton::MiddleButton},
            }};

            auto currentButtons = AMotionEvent_getButtonState(event);
            auto prevButtons    = mouse->m_buttons;
            for(auto const& map : mapping)
            {
                auto current  = static_cast<bool>(currentButtons & map.first);
                auto previous = static_cast<bool>(prevButtons & map.second);

                if(current == previous)
                    continue;

                if(current)
                    mouse->m_buttons |= map.second;
                else
                    mouse->m_buttons = mouse->m_buttons
                                       & (MouseButton::AllButtons ^ map.second);

                CIEvent event;
                event.type = CIEvent::MouseButton;
                CIMouseButtonEvent click;
                click.btn = map.second;
                click.mod = current ? CIMouseButtonEvent::Pressed
                                    : CIMouseButtonEvent::NoneModifier;
                click.pos = Vecf2(x, y);
                inputBus->inject(event, &click);
            }

            break;
        }
        default:
            break;
        }

        mouse->m_position = comp_app::position_t(x, y);

        break;
    }
    default:
        Coffee::cDebug("Unrecognized mouse event: {0}", type);
        break;
    }
}

using libc_types::i16;

static std::pair<i16, i16> trigger_values(AInputEvent *event)
{
    /* Trigger values are fucked up.
     * Stolen logic from: https://github.com/moonlight-stream/moonlight-android/blob/master/app/src/main/java/com/limelight/binding/input/ControllerHandler.java
     */

    using libc_types::convert_f32;

    std::pair<i16, i16> out;

    out.first = convert_f32<i16>(
        AMotionEvent_getAxisValue(event, AMOTION_EVENT_AXIS_LTRIGGER, 0));
    out.second = convert_f32<i16>(
        AMotionEvent_getAxisValue(event, AMOTION_EVENT_AXIS_RTRIGGER, 0));
    out.first = std::max(out.first, convert_f32<i16>(
        AMotionEvent_getAxisValue(event, AMOTION_EVENT_AXIS_BRAKE, 0)));
    out.second = std::max(out.second, convert_f32<i16>(
        AMotionEvent_getAxisValue(event, AMOTION_EVENT_AXIS_GAS, 0)));
    out.second = std::max(out.second, convert_f32<i16>(
        AMotionEvent_getAxisValue(event, AMOTION_EVENT_AXIS_THROTTLE, 0)));

    if(out.first > 0 || out.second > 0)
        return out;

    return out;
}

bool AndroidEventBus::handleGamepadEvent(AInputEvent* event)
{
    using namespace libc_types;

    i32                  type     = AInputEvent_getType(event);
    [[maybe_unused]] i32 source   = AInputEvent_getSource(event);
    i32                  deviceId = AInputEvent_getDeviceId(event);

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
        i32                  button = AKeyEvent_getKeyCode(event);
        i32                  action = AKeyEvent_getAction(event);
        [[maybe_unused]] i32 flags  = AKeyEvent_getFlags(event);
        [[maybe_unused]] i32 meta   = AKeyEvent_getMetaState(event);

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
            buttons.b_l = pressed;
            break;
        case AKEYCODE_BUTTON_R1:
            buttons.b_r = pressed;
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
            return false;
        }
        return true;
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

        std::tie(axes.t_l, axes.t_r) = trigger_values(event);

        buttons.p_down
            = AMotionEvent_getAxisValue(event, AMOTION_EVENT_AXIS_HAT_Y, 0)
              > .5f;
        buttons.p_up
            = AMotionEvent_getAxisValue(event, AMOTION_EVENT_AXIS_HAT_Y, 0)
              < -.5f;
        buttons.p_left
            = AMotionEvent_getAxisValue(event, AMOTION_EVENT_AXIS_HAT_X, 0)
              < -.5f;
        buttons.p_right
            = AMotionEvent_getAxisValue(event, AMOTION_EVENT_AXIS_HAT_X, 0)
              > .5f;
        return true;
    }
    default:
        break;
    }
    return false;
}

void AndroidEventBus::handleKeyEvent(AInputEvent* event)
{
    using namespace libc_types;

    i32 source   = AInputEvent_getSource(event);
    i32 deviceId = AInputEvent_getDeviceId(event);

    Coffee::cDebug(
        "Key event: {0} {1}",
        source,
        magic_enum::enum_name(
            static_cast<decltype(AKEYCODE_A)>(AKeyEvent_getKeyCode(event))));

    i32                  button = AKeyEvent_getKeyCode(event);
    i32                  flags  = AKeyEvent_getFlags(event);
    [[maybe_unused]] i32 action = AKeyEvent_getAction(event);
    [[maybe_unused]] i32 meta   = AKeyEvent_getMetaState(event);

    if(flags & AKEY_EVENT_FLAG_WOKE_HERE)
        return;

    const auto navEvent = [this](comp_app::NavigationEvent::Type key) {
        using AppBus    = comp_app::BasicEventBus<comp_app::AppEvent>;
        AppBus* app_bus = m_container->service<AppBus>();
        if(!app_bus)
            return;
        comp_app::AppEvent        event{comp_app::AppEvent::NavigationEvent};
        comp_app::NavigationEvent nav{key};
        app_bus->inject(event, &nav);
    };

    switch(button)
    {
    case AKEYCODE_BACK:
        navEvent(comp_app::NavigationEvent::Back);
        break;
    case AKEYCODE_DPAD_LEFT:
        navEvent(comp_app::NavigationEvent::Left);
        break;
    case AKEYCODE_DPAD_RIGHT:
        navEvent(comp_app::NavigationEvent::Right);
        break;
    case AKEYCODE_DPAD_UP:
        navEvent(comp_app::NavigationEvent::Up);
        break;
    case AKEYCODE_DPAD_DOWN:
        navEvent(comp_app::NavigationEvent::Down);
        break;
    default:
        Coffee::cDebug("Keycode: {0} {1} {2}", deviceId, source, button);
        break;
    }
}

void AndroidEventBus::handleMotionEvent(AInputEvent* event)
{
    using namespace libc_types;
    using enum_helpers::feval;

    i32                  source   = AInputEvent_getSource(event);
    [[maybe_unused]] i32 deviceId = AInputEvent_getDeviceId(event);

    if(feval<i32>(source, AINPUT_SOURCE_DPAD))
    {
        [[maybe_unused]] float x
            = AMotionEvent_getAxisValue(event, AMOTION_EVENT_AXIS_HAT_X, 0);
        [[maybe_unused]] float y
            = AMotionEvent_getAxisValue(event, AMOTION_EVENT_AXIS_HAT_Y, 0);
    } else if(feval<i32>(source, AINPUT_SOURCE_JOYSTICK))
    {
        [[maybe_unused]] float x
            = AMotionEvent_getAxisValue(event, AMOTION_EVENT_AXIS_X, 0);
        [[maybe_unused]] float y
            = AMotionEvent_getAxisValue(event, AMOTION_EVENT_AXIS_Y, 0);
    } else if(feval<i32>(source, AINPUT_SOURCE_TOUCHSCREEN))
    {
        [[maybe_unused]] i32 edgeFlags = AMotionEvent_getEdgeFlags(event);
        filterTouchEvent(event);
    }
}

void AndroidEventBus::handleInputEvent(AInputEvent* event)
{
    using namespace enum_helpers;
    using namespace libc_types;

    i32 type   = AInputEvent_getType(event);
    i32 source = AInputEvent_getSource(event);

    if(source & AINPUT_SOURCE_MOUSE)
    {
        handleMouseEvent(event);
        return;
    }

    if(source & AINPUT_SOURCE_GAMEPAD || source & AINPUT_SOURCE_JOYSTICK)
    {
        if(handleGamepadEvent(event))
            return;
    }

    if(source & AINPUT_SOURCE_KEYBOARD)
    {
        handleKeyEvent(event);
        return;
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
    using namespace typing::vector_types;

    comp_app::TouchConfig& touchConfig
        = m_container->service<comp_app::AppLoader>()
              ->config<comp_app::TouchConfig>();

    using IBus       = comp_app::BasicEventBus<CIEvent>;
    IBus*   inputBus = m_container->service<IBus>();
    CIEvent out;

    const bool mouseMapping
        = touchConfig.options & comp_app::TouchConfig::TouchToMouse;

    if(m_pinchDetector->Detect(event) != GESTURE_STATE_NONE)
    {
        // TODO
        //        ndk_helper::Vec2 v1, v2;
        //        m_pinchDetector->GetPointers(v1, v2);

        //        out.type = CIEvent::TouchPinch;
        //        CITouchPinchEvent pinch;
        //        inputBus->inject(out, &pinch);
        Coffee::cDebug("Pinch");
    }
    if(auto state = m_dragDetector->Detect(event); state != GESTURE_STATE_NONE)
    {
        Vec2 pos_;
        Vecf2 pos;
        m_dragDetector->GetPointer(pos_);
        pos_.Value(pos.x(), pos.y());
        if(mouseMapping)
        {
            out.type = CIMouseMoveEvent::event_type;
            CIMouseMoveEvent move;
            move.origin = pos;
            move.btn    = CIMouseButtonEvent::LeftButton;
        }
        Coffee::cDebug("Drag");
    }
    if(m_doubleDetector->Detect(event) != GESTURE_STATE_NONE)
    {
        CITouchTapEvent tap;
        tap.pos = {AMotionEvent_getX(event, 0), AMotionEvent_getY(event, 0)};
        tap.pressed = true;
        out.type = CITouchTapEvent::event_type;
        inputBus->inject(out, &tap);
        Coffee::cDebug("Double tap");
    }
    if(m_tapDetector->Detect(event) != GESTURE_STATE_NONE)
    {
        CITouchTapEvent tap;
        tap.pos = {AMotionEvent_getX(event, 0), AMotionEvent_getY(event, 0)};
        tap.pressed = true;
        out.type = CITouchTapEvent::event_type;
        inputBus->inject(out, &tap);
        /* TODO: Emit release event */
        Coffee::cDebug("Tap");
    }

    return false;
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
