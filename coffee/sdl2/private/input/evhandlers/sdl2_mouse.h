#pragma once

#include <coffee/core/CInput>

#include "../../types/sdl2datatypes.h"
#include "sdl2eventhandlers.h"

namespace Coffee{
namespace CSDL2Types{

using namespace CInput;

FORCEDINLINE void EventHandleMouseWheel(
        SDL2EventHandler* ctxt,
        const SDL_MouseWheelEvent& wheel)
{
    if(wheel.which==SDL_TOUCH_MOUSEID)
        return;

    CIEvent e;
    e.ts = wheel.timestamp;
    e.type = CIEvent::Scroll;

    CIScrollEvent s;

    s.delta.x = wheel.x;
    s.delta.y = wheel.y;

    EventPack(ctxt,&e,&s);
}

FORCEDINLINE void EventHandleMouseMove(
        SDL2EventHandler* ctxt,
        const SDL_MouseMotionEvent& motion)
{
    if(motion.which==SDL_TOUCH_MOUSEID)
        return;

    CIEvent e;
    e.ts = motion.timestamp;
    e.type = CIEvent::MouseMove;

    CIMouseMoveEvent m;

    m.pos.x = motion.x;
    m.pos.y = motion.y;
    m.rel.x = motion.xrel;
    m.rel.y = motion.yrel;
    m.btn = coffee_sdl2_translate_mouse_btnmask(motion.state);

    EventPack(ctxt,&e,&m);
}

FORCEDINLINE void EventHandleMouseBtn(
        SDL2EventHandler* ctxt,
        const SDL_MouseButtonEvent& btn)
{
    if(btn.which==SDL_TOUCH_MOUSEID)
        return;

    CIEvent e;
    e.ts = btn.timestamp;
    e.type = CIEvent::MouseButton;

    CIMouseButtonEvent m;

    if(btn.state==SDL_PRESSED)
        m.mod = m.mod|CIMouseButtonEvent::Pressed;
    if(btn.clicks==2)
        m.mod = m.mod|CIMouseButtonEvent::DoubleClick;
    m.pos.x = btn.x;
    m.pos.y = btn.y;
    m.btn = coffee_sdl2_translate_mouse_btn(btn.button);

    EventPack(ctxt,&e,&m);
}

}
}