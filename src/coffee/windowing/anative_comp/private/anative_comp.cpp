#include <coffee/anative/anative_comp.h>

#include <coffee/android/android_main.h>
#include <coffee/foreign/foreign.h>

#include <coffee/strings/libc_types.h>

#include <coffee/core/CDebug>

#include <android/input.h>
#include <android/keycodes.h>

namespace anative {

void Windowing::load(entity_container& e, comp_app::app_error& ec)
{
    using namespace Coffee;

    AndroidForeignCommand cmd;
    cmd.type = Android_QueryNativeWindow;

    CoffeeForeignSignalHandleNA(
        CoffeeForeign_RequestPlatformData, &cmd, nullptr, nullptr);

    //    if(!cmd.data.ptr)
    //    {
    //        ec = "ANativeWindow not available";
    //        ec = comp_app::AppError::NoWindow;
    //        return;
    //    }

    auto windowInfo = e.service<comp_app::PtrNativeWindowInfo>();
    if(!windowInfo)
    {
        ec = "Missing comp_app::PtrNativeWindowInfo service";
        ec = comp_app::AppError::ServiceMissing;
        return;
    }

    windowInfo->window = cmd.data.ptr;
}

comp_app::size_2d_t Windowing::size() const
{
    using namespace Coffee;

    int winSize[2] = {};

    CoffeeForeignSignalHandleNA(
        CoffeeForeign_GetWinSize, winSize, nullptr, nullptr);

    return {winSize[0], winSize[1]};
}

comp_app::detail::WindowState Windowing::state() const
{
    return comp_app::detail::WindowState::Normal;
}

void Windowing::setState(comp_app::detail::WindowState state)
{
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

void AndroidEventBus::load(entity_container& e, comp_app::app_error&)
{
    m_container = &e;
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
    case AINPUT_EVENT_TYPE_KEY:
    {
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
    case AINPUT_EVENT_TYPE_MOTION:
    {
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

        buttons.p_down = AMotionEvent_getAxisValue(
                             event, AMOTION_EVENT_AXIS_HAT_Y, 0) < -.5f;
        buttons.p_up =
            AMotionEvent_getAxisValue(event, AMOTION_EVENT_AXIS_HAT_Y, 0) > .5f;
        buttons.p_left = AMotionEvent_getAxisValue(
                             event, AMOTION_EVENT_AXIS_HAT_X, 0) < -.5f;
        buttons.p_right =
            AMotionEvent_getAxisValue(event, AMOTION_EVENT_AXIS_HAT_X, 0) > .5f;

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
    } else if(source & AINPUT_SOURCE_KEYBOARD)
    {
        i32 button = AKeyEvent_getKeyCode(event);
        i32 action = AKeyEvent_getAction(event);
        i32 flags  = AKeyEvent_getFlags(event);
        i32 meta   = AKeyEvent_getMetaState(event);
    }
}

void AndroidEventBus::handleMotionEvent(AInputEvent* event)
{
    using namespace libc_types;

    i32 source   = AInputEvent_getSource(event);
    i32 deviceId = AInputEvent_getDeviceId(event);

    switch(source)
    {
    case AINPUT_SOURCE_DPAD:
    {
        float x = AMotionEvent_getAxisValue(event, AMOTION_EVENT_AXIS_HAT_X, 0);
        float y = AMotionEvent_getAxisValue(event, AMOTION_EVENT_AXIS_HAT_Y, 0);
        break;
    }
    case AINPUT_SOURCE_GAMEPAD:
    {
        break;
    }
    case AINPUT_SOURCE_JOYSTICK:
    {
        float x = AMotionEvent_getAxisValue(event, AMOTION_EVENT_AXIS_X, 0);
        float y = AMotionEvent_getAxisValue(event, AMOTION_EVENT_AXIS_Y, 0);
        break;
    }
    case AINPUT_SOURCE_TOUCHSCREEN:
    {
        i32 edgeFlags = AMotionEvent_getEdgeFlags(event);
        break;
    }
    }

    Coffee::DebugFun::cDebug("Motion event: {0}", event);
}

void AndroidEventBus::handleInputEvent(AInputEvent* event)
{
    using namespace libc_types;

    i32 type   = AInputEvent_getType(event);
    i32 source = AInputEvent_getSource(event);

    if(source & (AINPUT_SOURCE_GAMEPAD | AINPUT_SOURCE_JOYSTICK))
    {
        handleGamepadEvent(event);
        return;
    }

    switch(type)
    {
    case AINPUT_EVENT_TYPE_KEY:
    {
        handleKeyEvent(event);
        break;
    }
    case AINPUT_EVENT_TYPE_MOTION:
    {
        handleMotionEvent(event);
        break;
    }
    }
}

void AndroidEventBus::handleWindowEvent(android_app* app, libc_types::i32 event)
{
    Coffee::DebugFun::cDebug("App event: {0}", event);
}

} // namespace anative
