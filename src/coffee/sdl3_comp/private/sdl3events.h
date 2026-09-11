#pragma once

#include <SDL3/SDL.h>
#include <coffee/core/types/display/event.h>
#include <coffee/core/types/input/event_types.h>

namespace sdl3::translate {
using namespace Coffee::Input;
using namespace Coffee::Display;
using typing::vector_types::Vecf2;

template<typename T, typename std::enable_if<false, T>::type* = nullptr>
T event(SDL_Event&)
{
    return {};
}

#define EVENT_TRANSLATE(type_)        \
    template<typename T>              \
    requires std::is_same_v<T, type_> \
    type_ event([[maybe_unused]] SDL_Event& ev)

EVENT_TRANSLATE(FocusEvent)
{
    switch(ev.window.type)
    {
    case SDL_EVENT_WINDOW_MOUSE_ENTER:
        return {FocusEvent::Enter};
    case SDL_EVENT_WINDOW_EXPOSED:
        return {FocusEvent::Exposed};
    case SDL_EVENT_WINDOW_MOUSE_LEAVE:
        return {FocusEvent::Leave};
    case SDL_EVENT_WINDOW_FOCUS_LOST:
        return {FocusEvent::Leave};
    case SDL_EVENT_WINDOW_FOCUS_GAINED:
        return {FocusEvent::Enter};
    }
    return {};
}

EVENT_TRANSLATE(ResizeEvent)
{
    using libc_types::i32;
    return ResizeEvent(ev.window.data1, ev.window.data2);
}

EVENT_TRANSLATE(MoveEvent)
{
    return {ev.window.data1, ev.window.data2};
}

EVENT_TRANSLATE(CIKeyEvent)
{
    CIKeyEvent key;

    {
        /* SDL3 dropped the keysym indirection and renamed sym to key */
        auto  sym = ev.key.key;
        auto& out = key.key;

#define MAP_KEY(from, to) \
    case from:            \
        out = to;         \
        break;

        if(sym < 256)
            out = C_FCAST<libc_types::u32>(sym);
        else
        {
            switch(sym)
            {
                MAP_KEY(SDLK_LSHIFT, CK_LShift);
                MAP_KEY(SDLK_RSHIFT, CK_RShift);
                MAP_KEY(SDLK_LALT, CK_LAlt);
                MAP_KEY(SDLK_RALT, CK_AltGr);
                MAP_KEY(SDLK_LGUI, CK_LSuper);
                MAP_KEY(SDLK_RGUI, CK_RSuper);
                MAP_KEY(SDLK_LCTRL, CK_LCtrl);
                MAP_KEY(SDLK_RCTRL, CK_RCtrl);

                MAP_KEY(SDLK_LEFT, CK_Left);
                MAP_KEY(SDLK_RIGHT, CK_Right);
                MAP_KEY(SDLK_UP, CK_Up);
                MAP_KEY(SDLK_DOWN, CK_Down);
            default:
                break;
            }

            if(sym >= SDLK_F1 && sym <= SDLK_F12)
                out = CK_F1 + (sym - SDLK_F1);
            if(sym >= SDLK_KP_1 && sym <= SDLK_KP_9)
                out = CK_KP_1 + (sym - SDLK_KP_1);

#undef MAP_KEY
        }
    }

    key.mod |= ev.key.down ? CIKeyEvent::PressedModifier
                           : CIKeyEvent::NoneModifier;

    key.mod |=
        ev.key.repeat ? CIKeyEvent::RepeatedModifier : CIKeyEvent::NoneModifier;

    return key;
}

EVENT_TRANSLATE(CIMouseButtonEvent)
{
    using BTN = CIMouseButtonEvent;

    CIMouseButtonEvent btn;

    btn.pos = Vecf2(ev.button.x, ev.button.y);
    btn.mod = ev.button.down ? BTN::Pressed : BTN::NoneModifier;

    switch(ev.button.button)
    {
    case SDL_BUTTON_LEFT:
        btn.btn = BTN::LeftButton;
        break;
    case SDL_BUTTON_RIGHT:
        btn.btn = BTN::RightButton;
        break;
    case SDL_BUTTON_MIDDLE:
        btn.btn = BTN::MiddleButton;
        break;
    case SDL_BUTTON_X1:
        btn.btn = BTN::X1Button;
        break;
    case SDL_BUTTON_X2:
        btn.btn = BTN::X2Button;
        break;
    }

    return btn;
}

EVENT_TRANSLATE(CIMouseMoveEvent)
{
    CIMouseMoveEvent out = {
        Vecf2(ev.motion.x, ev.motion.y), Vecf2(ev.motion.xrel, ev.motion.yrel)};

    auto buttons = SDL_GetMouseState(nullptr, nullptr);

    using BTN = CIMouseButtonEvent;

    out.btn = 0;
    out.btn |= (buttons & SDL_BUTTON_LMASK) ? BTN::LeftButton : 0;
    out.btn |= (buttons & SDL_BUTTON_MMASK) ? BTN::MiddleButton : 0;
    out.btn |= (buttons & SDL_BUTTON_RMASK) ? BTN::RightButton : 0;
    out.btn |= (buttons & SDL_BUTTON_X1MASK) ? BTN::X1Button : 0;
    out.btn |= (buttons & SDL_BUTTON_X2MASK) ? BTN::X2Button : 0;

    return out;
}

EVENT_TRANSLATE(CIScrollEvent)
{
    return {Vecf2(ev.wheel.x, ev.wheel.y), 0};
}

EVENT_TRANSLATE(CIControllerAtomicUpdateEvent)
{
    CIControllerAtomicUpdateEvent out;

    if(ev.type == SDL_EVENT_GAMEPAD_ADDED ||
       ev.type == SDL_EVENT_GAMEPAD_REMOVED)
    {
        auto& device = ev.gdevice;

        out.controller = device.which;
        out.connected  = device.type != SDL_EVENT_GAMEPAD_REMOVED;
    } else if(ev.type == SDL_EVENT_JOYSTICK_REMOVED)
    {
        auto& joydev = ev.jdevice;

        out.controller = joydev.which;
        out.connected  = joydev.type != SDL_EVENT_JOYSTICK_REMOVED;
    }

    return out;
}

EVENT_TRANSLATE(CIQuit)
{
    return {};
}

#undef EVENT_TRANSLATE

} // namespace sdl3::translate
