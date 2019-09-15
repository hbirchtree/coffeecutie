#include <stdint.h>
#include <stdio.h>

#include <coffee/core/base.h>
#include <coffee/foreign/foreign.h>

#include <coffee/core/base/renderer/eventapplication.h>
#include <coffee/core/types/input/event_types.h>

namespace Coffee {
namespace CfAdaptors {

void CfResizeHandler(
    EventApplication* app, int /*event*/, void* /*p1*/, void* p2, void* /*p3*/)
{
    CfResizeEventData* evdata = C_FCAST<CfResizeEventData*>(p2);

    Event dEv = {};
    dEv.type  = Event::Resize;

    ResizeEvent rEv = {evdata->w, evdata->h};

    app->injectEvent(dEv, &rEv);
}

void CfTouchHandler(
    EventApplication* app, int /*event*/, void* /*p1*/, void* p2, void* /*p3*/)
{
    CfTouchEventData* evdata = C_FCAST<CfTouchEventData*>(p2);

    Input::CIEvent iEv = {};

    Input::CITouchTapEvent     tap;
    Input::CIMTouchMotionEvent pan;
    Input::CITouchPinchEvent   pinch;
    Input::CITouchRotateEvent  rotate;

    c_cptr data = nullptr;

    switch(evdata->type)
    {
    case CfTouchTap:
    {
        iEv.type = Input::CIEvent::TouchTap;
        data     = &tap;

        tap.pos.x = evdata->event.tap.x;
        tap.pos.y = evdata->event.tap.y;

        break;
    }
    case CfTouchPan:
    {
        iEv.type = Input::CIEvent::TouchPan;
        data     = &pan;

        pan.origin.x = evdata->event.pan.ox;
        pan.origin.y = evdata->event.pan.oy;

        pan.translation.x = evdata->event.pan.dx;
        pan.translation.y = evdata->event.pan.dy;

        pan.velocity.x = evdata->event.pan.vx;
        pan.velocity.y = evdata->event.pan.vy;

        pan.fingers = evdata->event.pan.fingerCount;
        break;
    }
    case CfTouchPinch:
    {
        iEv.type = Input::CIEvent::TouchPinch;
        data     = &pinch;

        pinch.origin.x = evdata->event.pinch.x;
        pinch.origin.y = evdata->event.pinch.y;

        pinch.factor = evdata->event.pinch.factor;
        break;
    }
    case CfTouchRotate:
    {
        iEv.type = Input::CIEvent::TouchRotate;
        data     = &rotate;

        rotate.origin.x = evdata->event.rotate.x;
        rotate.origin.y = evdata->event.rotate.y;

        rotate.radians = evdata->event.rotate.radians;
        break;
    }
    default:
        return;
    }

    app->injectEvent(iEv, data);
}

} // namespace CfAdaptors
} // namespace Coffee
