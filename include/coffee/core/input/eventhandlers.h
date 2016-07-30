#pragma once

#include "../base/types/cdisplay.h"
#include "../base/input/cinput.h"
#include "cinputfunctions.h"

#include "../base/renderer/windowapplication.h"

namespace Coffee{
namespace Display{
namespace EventHandlers{

using namespace CInput;

template<class GLM>
FORCEDINLINE void ResizeWindowUniversal(const CDEvent& e, c_cptr data)
{
    if(e.type==CDEvent::Resize)
    {
        auto rev = (const CDResizeEvent*)data;
        CRect64 view(0,0,rev->w,rev->h);
        GLM::DefaultFramebuffer.resize(0,view);
    }
}

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

template<typename T>
FORCEDINLINE void EscapeCloseWindow(T *r,
                                    const CIEvent& e, c_cptr data)
{
    if(e.type==CIEvent::Keyboard)
    {
        auto kev = (const CIKeyEvent*)data;
        if(kev->key==CK_Escape)
            r->closeWindow();
    }
}

template<typename T>
FORCEDINLINE void WindowManagerCloseWindow(T* r,
                                           const CDEvent& event, c_cptr data)
{
    if(event.type==CDEvent::State)
    {
        const CDStateEvent* sev = (const CDStateEvent*)data;
        if(sev->type==CDStateEvent::Closed)
            r->closeWindow();
    }
}

FORCEDINLINE void RotateView(CQuat& q, const CIEvent& e, c_cptr data)
{
    if(e.type==CIEvent::Controller)
    {
        auto ev = (const CIControllerAtomicEvent*)data;
        if(ev->axis && (   ev->index+CK_AXIS_LEFT_X == CK_AXIS_RIGHT_X
                        || ev->index+CK_AXIS_LEFT_X == CK_AXIS_RIGHT_Y))
            ControllerRotate(q,ev);
    }else if(e.type==CIEvent::MouseMove)
    {
        auto ev = (const CIMouseMoveEvent*)data;
        MouseRotate(q,ev);
    }
}

#if defined(COFFEE_ENABLE_PLUGGABLE_EVENTHANDLERS)
FORCEDINLINE void EscapeCloseWindow(void* r, CIEvent const& e, c_cptr data)
{
    EscapeCloseWindow((WindowHandler*)r,e,data);
}

FORCEDINLINE void WindowManagerCloseWindow(void* r, CDEvent const& e, c_cptr data)
{
    WindowManagerCloseWindow((WindowHandler*)r,e,data);
}
#endif

}
}
}
