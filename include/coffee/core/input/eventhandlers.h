#pragma once

#include "../base/cdisplay.h"
#include "../base/cinput.h"
#include "cinputfunctions.h"

namespace Coffee{
namespace CDisplay{
namespace EventHandlers
{

using namespace CInput;

template<class GL>
FORCEDINLINE void ResizeWindow(const CDEvent& e, c_cptr data)
{
    if(e.type==CDEvent::Resize)
    {
        auto rev = (const CDResizeEvent*)data;
        CRect64 view(0,0,rev->w,rev->h);
        GL::ViewportSet(&view);
    }
}

FORCEDINLINE void CloseWindow(CDRendererBase *r,
                              const CIEvent& e, c_cptr data)
{
    if(e.type==CIEvent::Keyboard)
    {
        auto kev = (const CIKeyEvent*)data;
        if(kev->key==CK_Escape)
            r->closeWindow();
    }
}

FORCEDINLINE void RotateView(CQuat& q, const CIEvent& e, c_cptr data)
{
    if(e.type==CIEvent::Controller)
    {
        auto ev = (const CIControllerAtomicEvent*)data;
        if(ev->axis && (ev->index == CK_AXIS_RIGHT_X || ev->index == CK_AXIS_RIGHT_Y))
            ControllerRotate(q,ev);
    }else if(e.type==CIEvent::MouseMove)
    {
        auto ev = (const CIMouseMoveEvent*)data;
        MouseRotate(q,ev);
    }
}

}
}
}