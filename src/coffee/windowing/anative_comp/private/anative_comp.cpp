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
#include <coffee/anative/gestureDetector.h>
#pragma GCC diagnostic pop

namespace anative {

using Coffee::cDebug;
using Coffee::Input::CIEvent;
using Coffee::Input::CIKeyEvent;
using libc_types::i16;
using libc_types::i32;

namespace {

CIKeyEvent::KeyModifiers meta_to_key_modifier(i32 meta)
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

std::pair<i16, i16> trigger_values(AInputEvent* event)
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

} // namespace

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

    using ws_t = comp_app::interfaces::PtrNativeWindowInfo::window_system_t;
    windowInfo->window        = window_info->window;
    windowInfo->window_system = ws_t::android;
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
    return comp_app::window_flags_t::fullscreen |
           comp_app::window_flags_t::focused;
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

void DisplayInfo::load(entity_container& e, comp_app::app_error&)
{
    m_windowing = e.service<Windowing>();
}

comp_app::size_2d_t DisplayInfo::virtualSize() const
{
    return m_windowing->size();
}

libc_types::u32 DisplayInfo::count() const
{
    return 1;
}

libc_types::u32 DisplayInfo::currentDisplay() const
{
    return 0;
}

comp_app::size_2d_t DisplayInfo::size(libc_types::u32 idx) const
{
    return m_windowing->size();
}

comp_app::size_2d_t DisplayInfo::physicalSize(libc_types::u32 idx) const
{
    auto size = android::display_info().physical_size();
    return comp_app::size_2d_t{
        static_cast<i32>(size.x),
        static_cast<i32>(size.y),
    };
}

libc_types::f32 DisplayInfo::dpi(libc_types::u32 idx) const
{
    return android::display_info().dpi();
}

libc_types::f32 DisplayInfo::diagonal(libc_types::u32 idx) const
{
    auto size = android::display_info().physical_size();
    return std::sqrt(size.x * size.x + size.y * size.y);
}

libc_types::u32 ControllerInput::count() const
{
    return m_cache.size();
}

ControllerInput::controller_map ControllerInput::state(
    libc_types::u32 idx) const
{
    if(idx < m_cache.size())
        return m_cache.at(idx);
    else
        return {};
}

comp_app::text_type_t ControllerInput::name(libc_types::u32 /*idx*/) const
{
    return "Generic Controller";
}

void KeyboardInput::startWriting() const
{
    // #if ANDROID_API_LEVEL >= 23
    //     if(auto window = android::activity_manager().window())
    //         ANativeActivity_showSoftInput(
    //             (*window).activity, ANATIVEACTIVITY_SHOW_SOFT_INPUT_FORCED);
    // #endif
    android::input_method_manager::show_soft_input();
}

void KeyboardInput::stopWriting() const
{
    // #if ANDROID_API_LEVEL >= 23
    //     if(auto window = android::activity_manager().window())
    //         ANativeActivity_hideSoftInput(
    //             (*window).activity,
    //             ANATIVEACTIVITY_HIDE_SOFT_INPUT_IMPLICIT_ONLY);
    // #endif
    android::input_method_manager::hide_soft_input();
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
    m_appBus =
        m_container->service<comp_app::BasicEventBus<comp_app::AppEvent>>();
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
    // AMotionEvent_getMetaState

    constexpr std::array<MouseButton, 3> supported_buttons = {{
        MouseButton::LeftButton,
        MouseButton::MiddleButton,
        MouseButton::RightButton,
    }};

    switch(type)
    {
    case AINPUT_EVENT_TYPE_MOTION: {
        auto action         = AMotionEvent_getAction(event);
        auto x              = AMotionEvent_getX(event, 0);
        auto y              = AMotionEvent_getY(event, 0);
        auto currentButtons = [event, mouse]() {
            constexpr std::array<std::pair<int, MouseButton>, 3> mapping = {{
                {AMOTION_EVENT_BUTTON_PRIMARY, MouseButton::LeftButton},
                {AMOTION_EVENT_BUTTON_SECONDARY, MouseButton::RightButton},
                {AMOTION_EVENT_BUTTON_TERTIARY, MouseButton::MiddleButton},
            }};
            auto         android_buttons = AMotionEvent_getButtonState(event);
            MouseButton& buttons         = mouse->m_buttons;
            MouseButton  changed         = MouseButton::NoneBtn;
            for(auto [native, mapped] : mapping)
            {
                const bool current  = android_buttons & native;
                const bool previous = buttons & mapped;
                if(current == previous)
                    continue;

                if(current)
                    buttons |= mapped;
                else
                    buttons = buttons & (MouseButton::AllButtons ^ mapped);
                changed |= mapped;
            }
            return changed;
        }();

        CIEvent ievent;

        switch(action)
        {
        case AMOTION_EVENT_ACTION_HOVER_ENTER:
        case AMOTION_EVENT_ACTION_HOVER_MOVE: {
            ievent.type = CIEvent::MouseMove;
            CIMouseMoveEvent hover;
            hover.origin = Vecf2{x, y};
            hover.delta  = {};
            inputBus->inject(ievent, &hover);
            break;
        }
        case AMOTION_EVENT_ACTION_BUTTON_PRESS:
        case AMOTION_EVENT_ACTION_BUTTON_RELEASE: {
            for(auto button : supported_buttons)
            {
                if((button & currentButtons) == MouseButton::NoneBtn)
                    continue;
                const bool current = mouse->buttons() & button;
                ievent.type        = CIEvent::MouseButton;
                CIMouseButtonEvent click;
                click.btn = button;
                click.mod = current ? CIMouseButtonEvent::Pressed
                                    : CIMouseButtonEvent::NoneModifier;
                click.pos = Vecf2(x, y);
                inputBus->inject(ievent, &click);
                cDebug(
                    "Click: {} = {}:{}",
                    click.pos,
                    static_cast<int>(click.btn),
                    static_cast<int>(click.mod));
            }

            break;
        }
        case AMOTION_EVENT_ACTION_UP:
        case AMOTION_EVENT_ACTION_DOWN: {
            ievent.type = CIEvent::MouseButton;
            CIMouseButtonEvent click;

            auto buttons = AMotionEvent_getButtonState(event);
            click.btn    = buttons & AMOTION_EVENT_BUTTON_STYLUS_PRIMARY
                               ? MouseButton::RightButton
                               : MouseButton::LeftButton;

            click.mod = action == AMOTION_EVENT_ACTION_DOWN
                            ? CIMouseButtonEvent::Pressed
                            : CIMouseButtonEvent::NoneModifier;
            click.pos = Vecf2(x, y);
            inputBus->inject(ievent, &click);
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
        controllers->m_cache.push_back({});
        it = controllers->m_mapping
                 .insert({deviceId, controllers->m_cache.size() - 1})
                 .first;
        cDebug(
            "Creating controller mapping: {}({}, {}) -> {}",
            it->first,
            type,
            source,
            it->second);
    }

    auto& c = controllers->m_cache.at(it->second);

    auto& axes    = c.axes.e;
    auto& buttons = c.buttons.e;

    const auto apply_deadzone = [](i16 x, i16 y) -> std::pair<i16, i16> {
        return std::abs(std::sqrt(x * x + y * y)) < 4000
                   ? std::pair<i16, i16>{0, 0}
                   : std::make_pair(x, y);
    };

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

        std::tie(axes.l_x, axes.l_y) = apply_deadzone(axes.l_x, axes.l_y);

        axes.r_x = convert_f32<i16>(
            AMotionEvent_getAxisValue(event, AMOTION_EVENT_AXIS_Z, 0));
        axes.r_y = convert_f32<i16>(
            AMotionEvent_getAxisValue(event, AMOTION_EVENT_AXIS_RZ, 0));

        std::tie(axes.r_x, axes.r_y) = apply_deadzone(axes.r_x, axes.r_y);

        std::tie(axes.t_l, axes.t_r) = trigger_values(event);

        buttons.p_down =
            AMotionEvent_getAxisValue(event, AMOTION_EVENT_AXIS_HAT_Y, 0) > .5f;
        buttons.p_up = AMotionEvent_getAxisValue(
                           event, AMOTION_EVENT_AXIS_HAT_Y, 0) < -.5f;
        buttons.p_left = AMotionEvent_getAxisValue(
                             event, AMOTION_EVENT_AXIS_HAT_X, 0) < -.5f;
        buttons.p_right =
            AMotionEvent_getAxisValue(event, AMOTION_EVENT_AXIS_HAT_X, 0) > .5f;
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
    case AKEYCODE_DPAD_CENTER:
        navEvent(comp_app::NavigationEvent::Confirm);
        break;
        // clang-format off
    case AKEYCODE_SHIFT_LEFT: keyEvent(CK_LShift); break;
    case AKEYCODE_SHIFT_RIGHT: keyEvent(CK_RShift); break;
    case AKEYCODE_CTRL_LEFT: keyEvent(CK_LCtrl); break;
    case AKEYCODE_CTRL_RIGHT: keyEvent(CK_RCtrl); break;
    case AKEYCODE_SPACE: keyEvent(CK_Space); break;
    case AKEYCODE_ENTER: keyEvent(CK_EnterNL); break;
    case AKEYCODE_DEL: keyEvent(CK_Delete); break;
    case AKEYCODE_FORWARD_DEL: keyEvent(CK_BackSpace); break;
        // clang-format on
    default:
        cDebug("Keycode: {0} {1} {2}", deviceId, source, button);
        break;
    }
}

void AndroidEventBus::handleMotionEvent(AInputEvent* event)
{
    using namespace libc_types;
    using enum_helpers::feval;

    i32                  source   = AInputEvent_getSource(event);
    [[maybe_unused]] i32 deviceId = AInputEvent_getDeviceId(event);

    const bool mouseMapping =
        m_touchConfig->options & comp_app::TouchConfig::TouchToMouse;

    if(feval<i32>(source, AINPUT_SOURCE_DPAD))
    {
        [[maybe_unused]] float x =
            AMotionEvent_getAxisValue(event, AMOTION_EVENT_AXIS_HAT_X, 0);
        [[maybe_unused]] float y =
            AMotionEvent_getAxisValue(event, AMOTION_EVENT_AXIS_HAT_Y, 0);
    } else if(feval<i32>(source, AINPUT_SOURCE_JOYSTICK))
    {
        float x = AMotionEvent_getAxisValue(event, AMOTION_EVENT_AXIS_X, 0);
        float y = AMotionEvent_getAxisValue(event, AMOTION_EVENT_AXIS_Y, 0);
        cDebug("Joystick event: {0},{1}", x, y);
    } else if(
        feval<i32>(source, AINPUT_SOURCE_TOUCHSCREEN) &&
        !feval<i32>(source, AINPUT_SOURCE_STYLUS))
        handleTouchEvent(event);
    else if(
        feval<i32>(source, AINPUT_SOURCE_MOUSE) ||
        feval<i32>(source, AINPUT_SOURCE_STYLUS))
        handleMouseEvent(event);
}

bool AndroidEventBus::handleTouchEvent(AInputEvent* event)
{
    using namespace ndk_helper;
    using namespace Coffee::Input;
    using namespace typing::vector_types;
    using Coffee::cDebug;

    CIEvent out;

    const bool mouseMapping =
        m_touchConfig->options & comp_app::TouchConfig::TouchToMouse;

    const auto pointer_idx = (AMotionEvent_getAction(event) &
                              AMOTION_EVENT_ACTION_POINTER_INDEX_MASK) >>
                             AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;
    const auto pointer_id = AMotionEvent_getPointerId(event, pointer_idx);

    if(auto state = m_pinchDetector->Detect(event); state != GESTURE_STATE_NONE)
    {
        Vec2 v1_, v2_;
        m_pinchDetector->GetPointers(v1_, v2_);
        Vecf2 v1, v2;
        v1_.Value(v1.x, v1.y);
        v2_.Value(v2.x, v2.y);
        if(state == GESTURE_STATE_START)
        {
            m_pinchOrigin   = (v1 + v2) / 2.f;
            m_pinchDistance = glm::length(v1 - v2);
        } else if(state == GESTURE_STATE_MOVE)
        {
            out.type = CIEvent::TouchPinch;
            CITouchPinchEvent pinch;
            pinch.origin = *m_pinchOrigin;
            pinch.factor = glm::length(v1 - v2) / *m_pinchDistance;
            m_inputBus->inject(out, &pinch);
        } else if(state == GESTURE_STATE_END)
        {
            out.type = CIEvent::TouchPinch;
            CITouchPinchEvent pinch;
            pinch.origin = *m_pinchOrigin;
            pinch.factor = 1.f;
            m_inputBus->inject(out, &pinch);

            m_pinchOrigin   = std::nullopt;
            m_pinchDistance = std::nullopt;
        }
    }
    {
        /* Drag detection; it's pretty simple on paper */
        const auto action = AMotionEvent_getAction(event);
        const auto flags  = action & AMOTION_EVENT_ACTION_MASK;

        Vecf2 pos = {
            AMotionEvent_getX(event, pointer_idx),
            AMotionEvent_getY(event, pointer_idx),
        };

        enum
        {
            NONE,
            MOVED,
            STOPPED,
        } state{NONE};

        switch(flags)
        {
        case AMOTION_EVENT_ACTION_POINTER_DOWN:
        case AMOTION_EVENT_ACTION_DOWN: {
            m_dragData.emplace(
                pointer_id,
                drag_data_t{
                    .origin = pos,
                });
            break;
        }
        case AMOTION_EVENT_ACTION_MOVE: {
            state = MOVED;
            break;
        }
        case AMOTION_EVENT_ACTION_POINTER_UP:
        case AMOTION_EVENT_ACTION_UP: {
            state = STOPPED;
            break;
        }
        }

        if(state != NONE)
        {
            out.type = CITouchMotionEvent::event_type;
            CITouchMotionEvent move{};
            for(auto i : stl_types::range(AMotionEvent_getPointerCount(event)))
            {
                const auto id = AMotionEvent_getPointerId(event, i);
                move.finger   = id;
                move.origin   = m_dragData[id].origin;
                move.previous = {
                    AMotionEvent_getHistoricalX(event, i, 0),
                    AMotionEvent_getHistoricalY(event, i, 0),
                };
                move.current = {
                    AMotionEvent_getX(event, i),
                    AMotionEvent_getY(event, i),
                };
                move.end      = state == STOPPED;
                move.pressure = AMotionEvent_getPressure(event, i);
                m_inputBus->inject(out, &move);
            }
        }
        if(state == STOPPED)
            m_dragData.erase(pointer_id);
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

void AndroidEventBus::handleInputEvent(AInputEvent* event)
{
    using namespace enum_helpers;
    using namespace libc_types;

    i32 type   = AInputEvent_getType(event);
    i32 source = AInputEvent_getSource(event);

    if(source & AINPUT_SOURCE_GAMEPAD)
    {
        if(handleGamepadEvent(event))
            return;
    }

    if((source & AINPUT_SOURCE_KEYBOARD) == AINPUT_SOURCE_KEYBOARD)
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
    case AINPUT_EVENT_TYPE_CAPTURE:
        cDebug("AINPUT_CAPTURE");
        break;
    case AINPUT_EVENT_TYPE_DRAG:
        cDebug("AINPUT_DRAG");
        break;
    case AINPUT_EVENT_TYPE_TOUCH_MODE:
        cDebug("AINPUT_TOUCH_MODE");
        break;
    case AINPUT_EVENT_TYPE_FOCUS:
        cDebug("AINPUT_FOCUS");
        break;
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
        Coffee::Display::ResizeEvent resize = {
            ANativeWindow_getWidth(app->window),
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
