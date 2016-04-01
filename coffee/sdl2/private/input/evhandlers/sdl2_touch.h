#pragma once

#include <coffee/core/CInput>
#include "sdl2eventhandlers.h"

#include <coffee/core/CDebug>

namespace Coffee{
namespace CSDL2Types{

FORCEDINLINE void EventHandleTap(SDL2EventHandler* ctxt, const SDL_TouchFingerEvent& ev)
{
    CIEvent e;
    CITouchTapEvent t;

    e.type = CIEvent::TouchTap;
    e.ts = ev.timestamp;

    t.pressed = ev.type == SDL_FINGERDOWN;
    t.pos.x = ev.x;
    t.pos.y = ev.y;
    t.finger = ev.fingerId;
    t.pressure = (hscalar)(ev.pressure*UInt16_Max);

    EventPack(ctxt,&e,&t);
}

FORCEDINLINE void EventHandleTMotion(SDL2EventHandler* ctxt, const SDL_TouchFingerEvent& ev)
{
    CIEvent e;
    CITouchMotionEvent t;

    e.type = CIEvent::TouchMotion;
    e.ts = ev.timestamp;

    t.origin.x = ev.x;
    t.origin.y = ev.y;
    t.finger = ev.fingerId;
    t.pressure = (hscalar)(ev.pressure*UInt16_Max);

    t.delta.x = ev.dx;
    t.delta.y = ev.dy;

    EventPack(ctxt,&e,&t);
}

FORCEDINLINE void EventHandleMultiTouch(SDL2EventHandler* ctxt, const SDL_MultiGestureEvent& ev)
{
    CIEvent e;
    CIMTouchMotionEvent t;

    e.type = CIEvent::MultiTouch;
    e.ts = ev.timestamp;

    t.origin.x = ev.x;
    t.origin.y = ev.y;
    t.fingers = ev.numFingers;
    t.dist = ev.dDist*Int16_Max;
    t.angle = ev.dTheta*Int16_Max;

    EventPack(ctxt,&e,&t);
}

}
}
