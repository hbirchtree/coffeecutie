#include <coffee/anative/anative_comp.h>

#include <coffee/android/android_main.h>
#include <coffee/comp_app/app_events.h>
#include <coffee/comp_app/subsystems.h>
#include <peripherals/stl/magic_enum.hpp>

#include <fmt_extensions/vector_types.h>

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

namespace anative {

using Coffee::Input::CIEvent;
using Coffee::Input::CIKeyEvent;
using libc_types::i32;

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
    return comp_app::window_flags_t::fullscreen
           | comp_app::window_flags_t::focused;
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

    m_touchConfig = &m_container->service<comp_app::AppLoader>()
                         ->config<comp_app::TouchConfig>();

    m_inputBus = m_container->service<comp_app::BasicEventBus<CIEvent>>();
    m_appBus
        = m_container->service<comp_app::BasicEventBus<comp_app::AppEvent>>();
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
                {AMOTION_EVENT_BUTTON_PRIMARY, MouseButton::LeftButton},
                {AMOTION_EVENT_BUTTON_SECONDARY, MouseButton::RightButton},
                {AMOTION_EVENT_BUTTON_TERTIARY, MouseButton::MiddleButton},
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

static std::pair<i16, i16> trigger_values(AInputEvent* event)
{
    /* Trigger values are fucked up.
     * Stolen logic from:
     * https://github.com/moonlight-stream/moonlight-android/blob/master/app/src/main/java/com/limelight/binding/input/ControllerHandler.java
     */

    using libc_types::convert_f32;

    std::pair<i16, i16> out;

    out.first = convert_f32<i16>(
        AMotionEvent_getAxisValue(event, AMOTION_EVENT_AXIS_LTRIGGER, 0));
    out.second = convert_f32<i16>(
        AMotionEvent_getAxisValue(event, AMOTION_EVENT_AXIS_RTRIGGER, 0));
    out.first = std::max(
        out.first,
        convert_f32<i16>(
            AMotionEvent_getAxisValue(event, AMOTION_EVENT_AXIS_BRAKE, 0)));
    out.second = std::max(
        out.second,
        convert_f32<i16>(
            AMotionEvent_getAxisValue(event, AMOTION_EVENT_AXIS_GAS, 0)));
    out.second = std::max(
        out.second,
        convert_f32<i16>(
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

inline CIKeyEvent::KeyModifiers meta_to_key_modifier(i32 meta)
{
    CIKeyEvent::KeyModifiers mod = CIKeyEvent::NoneModifier;
    if(meta & AMETA_ALT_LEFT_ON)
        mod |= CIKeyEvent::LAltModifier;
    if(meta & AMETA_ALT_RIGHT_ON)
        mod |= CIKeyEvent::RAltModifier;
    if(meta & AMETA_SHIFT_LEFT_ON)
        mod |= CIKeyEvent::LShiftModifier;
    if(meta & AMETA_SHIFT_RIGHT_ON)
        mod |= CIKeyEvent::RShiftModifier;
    if(meta & AMETA_CTRL_LEFT_ON)
        mod |= CIKeyEvent::LCtrlModifier;
    if(meta & AMETA_CTRL_RIGHT_ON)
        mod |= CIKeyEvent::RCtrlModifier;
    if(meta & AMETA_SYM_ON)
        mod |= CIKeyEvent::SuperModifier;
    if(meta & AMETA_CAPS_LOCK_ON)
        mod |= CIKeyEvent::CapsLockModifier;
    if(meta & AMETA_NUM_LOCK_ON)
        mod |= CIKeyEvent::NumLockModifier;
    return mod;
}

void AndroidEventBus::handleKeyEvent(AInputEvent* event)
{
    using namespace libc_types;
    using namespace Coffee::Input;

    i32 source   = AInputEvent_getSource(event);
    i32 deviceId = AInputEvent_getDeviceId(event);

    i32                  button = AKeyEvent_getKeyCode(event);
    i32                  flags  = AKeyEvent_getFlags(event);
    [[maybe_unused]] i32 action = AKeyEvent_getAction(event);
    [[maybe_unused]] i32 meta   = AKeyEvent_getMetaState(event);

    if(flags & AKEY_EVENT_FLAG_WOKE_HERE)
        return;

    if(flags & AKEY_EVENT_FLAG_KEEP_TOUCH_MODE)
        ;
    else if(m_appBus)
    {
        comp_app::AppEvent       event{comp_app::AppEvent::InputModeSwitch};
        comp_app::InputModeEvent switch_{comp_app::InputModeEvent::Desktop};
        m_appBus->inject(event, &switch_);
    }

    const auto keyEvent = [this, flags, action, meta](u16 button) {
        CIEvent event;
        event.type = CIEvent::Keyboard;
        CIKeyEvent key;
        key.key = button;
        key.mod = action == AKEY_EVENT_ACTION_DOWN ? CIKeyEvent::PressedModifier
                  : action == AKEY_EVENT_ACTION_MULTIPLE
                      ? CIKeyEvent::RepeatedModifier
                      : CIKeyEvent::NoneModifier;
        key.mod |= meta_to_key_modifier(meta);
        m_inputBus->inject(event, &key);
    };

    if(button >= AKEYCODE_0 && button <= AKEYCODE_9)
    {
        keyEvent(CK_0 + (button - AKEYCODE_0));
        return;
    }
    if(button >= AKEYCODE_A && button <= AKEYCODE_Z)
    {
        keyEvent(CK_a + (button - AKEYCODE_A));
        return;
    }
    if(button >= AKEYCODE_F1 && button <= AKEYCODE_F12)
    {
        keyEvent(CK_F1 + (button - AKEYCODE_F1));
        return;
    }

    const auto navEvent = [this](comp_app::NavigationEvent::Type key) {
        if(!m_appBus)
            return;
        comp_app::AppEvent        event{comp_app::AppEvent::NavigationEvent};
        comp_app::NavigationEvent nav{key};
        m_appBus->inject(event, &nav);
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
        // clang-format off
    case AKEYCODE_SHIFT_LEFT: keyEvent(CK_LShift); break;
    case AKEYCODE_SHIFT_RIGHT: keyEvent(CK_RShift); break;
    case AKEYCODE_CTRL_LEFT: keyEvent(CK_LCtrl); break;
    case AKEYCODE_CTRL_RIGHT: keyEvent(CK_RCtrl); break;
    case AKEYCODE_SPACE: keyEvent(CK_Space); break;
    case AKEYCODE_ENTER: keyEvent(CK_EnterNL); break;
        // clang-format on
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

    const bool mouseMapping
        = m_touchConfig->options & comp_app::TouchConfig::TouchToMouse;

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
    } else if(feval<i32>(source, AINPUT_SOURCE_MOUSE))
    {
        handleMouseEvent(event);
    }
}

void AndroidEventBus::handleInputEvent(AInputEvent* event)
{
    using namespace enum_helpers;
    using namespace libc_types;

    i32 type   = AInputEvent_getType(event);
    i32 source = AInputEvent_getSource(event);

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

    CIEvent out;

    const bool mouseMapping
        = m_touchConfig->options & comp_app::TouchConfig::TouchToMouse;

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
        Vec2  pos_;
        Vecf2 pos;
        m_dragDetector->GetPointer(pos_);
        pos_.Value(pos.x, pos.y);
        if(mouseMapping)
        {
            out.type = CIMouseMoveEvent::event_type;
            CIMouseMoveEvent move;
            move.origin.x = AMotionEvent_getHistoricalX(event, 0, 0);
            move.origin.y = AMotionEvent_getHistoricalY(event, 0, 0);
            move.delta    = pos - move.origin;
            move.btn      = CIMouseButtonEvent::LeftButton;
            if(state == GESTURE_STATE_END)
                move.btn = 0;
            m_inputBus->inject(out, &move);
        }
    }
    if(m_doubleDetector->Detect(event) != GESTURE_STATE_NONE)
    {
        CITouchTapEvent tap;
        tap.pos = {AMotionEvent_getX(event, 0), AMotionEvent_getY(event, 0)};
        tap.pressed = true;
        out.type    = CITouchTapEvent::event_type;
        m_inputBus->inject(out, &tap);
    }
    if(auto state = m_tapDetector->Detect(event); state != GESTURE_STATE_NONE)
    {
        CITouchTapEvent tap;
        tap.pos = {AMotionEvent_getX(event, 0), AMotionEvent_getY(event, 0)};
        tap.pressed = state != GESTURE_STATE_END;
        out.type    = CITouchTapEvent::event_type;
        m_inputBus->inject(out, &tap);
        if(mouseMapping)
        {
            out.type = CIMouseButtonEvent::event_type;
            CIMouseButtonEvent push;
            push.pos = tap.pos;
            push.mod = CIMouseButtonEvent::Pressed;
            push.btn = CIMouseButtonEvent::LeftButton;
            m_inputBus->inject(out, &push);
            push.mod         = CIMouseButtonEvent::NoneModifier;
            m_tapButtonEvent = push;
        }
    } else if(m_tapButtonEvent.has_value())
    {
        out.type = CIMouseButtonEvent::event_type;
        m_inputBus->inject(out, &m_tapButtonEvent.value());
        m_tapButtonEvent = std::nullopt;
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
