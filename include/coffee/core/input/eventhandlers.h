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
        auto rev = C_CAST<const CDResizeEvent*>(data);
        CRect64 view(0,0,rev->w,rev->h);
        GLM::DefaultFramebuffer().resize(0,view);
    }
}

template<typename T>
FORCEDINLINE void ExitOnQuitSignal(T* r, const CIEvent& e, c_cptr)
{
    if(e.type == CIEvent::QuitSign)
        r->closeWindow();
}

template<class GL>
FORCEDINLINE void ResizeWindow(const CDEvent& e, c_cptr data)
{
    if(e.type==CDEvent::Resize)
    {
        auto rev = C_CAST<const CDResizeEvent*>(data);
        CRect64 view(0,0,rev->w,rev->h);
        GL::ViewportSet(view);
    }
}

template<typename T>
FORCEDINLINE void EscapeCloseWindow(T *r, const CIEvent& e, c_cptr data)
{
    if(e.type==CIEvent::Keyboard)
    {
        auto kev = C_CAST<const CIKeyEvent*>(data);
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
        const CDStateEvent* sev = C_CAST<const CDStateEvent*>(data);
        if(sev->type==CDStateEvent::Closed)
            r->closeWindow();
    }
}

template<typename T>
FORCEDINLINE void WindowManagerFullscreen(T* r, CIEvent const& e, c_cptr data)
{
#if !defined(COFFEE_ANDROID) || !defined(COFFEE_APPLE_MOBILE)
    if(e.type==CIEvent::Keyboard)
    {
        auto kev = C_CAST<CIKeyEvent const*>(data);

        if(kev->mod & CIKeyEvent::RepeatedModifier || kev->mod & CIKeyEvent::PressedModifier)
            return;

        switch(kev->key)
        {
        case CK_EnterCR:
        case CK_EnterNL:
            if(!(kev->mod & CIKeyEvent::LAltModifier))
                break;
        case CK_F11:
            if(r->windowState() & CDProperties::Windowed)
                r->setWindowState(CDProperties::WindowedFullScreen);
            else
                r->setWindowState(CDProperties::Windowed);
            break;
        default:
            break;
        }
    }
#endif
}

FORCEDINLINE void RotateView(CQuat& q, const CIEvent& e, c_cptr data)
{
    if(e.type==CIEvent::Controller)
    {
        auto ev = C_CAST<const CIControllerAtomicEvent*>(data);
        if(ev->axis && (   ev->index+CK_AXIS_LEFT_X == CK_AXIS_RIGHT_X
                           || ev->index+CK_AXIS_LEFT_X == CK_AXIS_RIGHT_Y))
            ControllerRotate(q,ev);
    }else if(e.type==CIEvent::MouseMove)
    {
        auto ev = C_CAST<const CIMouseMoveEvent*>(data);
        MouseRotate(q,ev);
    }
}

/* These are pluggable event handlers */

template<typename WindowHandler>
void EscapeCloseWindow(void* r, CIEvent const& e, c_cptr data)
{
    EscapeCloseWindow<WindowHandler>(C_CAST<WindowHandler*>(r),e,data);
}
template<typename WindowHandler>
void WindowManagerCloseWindow(void* r, CDEvent const& e, c_cptr data)
{
    WindowManagerCloseWindow<WindowHandler>(C_CAST<WindowHandler*>(r),e,data);
}
template<typename WindowHandler>
void WindowManagerFullscreen(void* r, CIEvent const& e, c_cptr data)
{
    WindowManagerFullscreen(C_CAST<WindowHandler*>(r),e,data);
}
template<typename GraphicsHandler>
void ResizeWindowUniversal(void*, CDEvent const& e, c_cptr data)
{
    ResizeWindowUniversal<GraphicsHandler>(e,data);
}
template<typename Camera>
void StandardCamera(void* r, const CIEvent& e, c_cptr data)
{
    Camera& camera = *C_CAST<Camera*>(r);
    switch(e.type)
    {
    case CIEvent::Keyboard:
    {
        auto kev = C_CAST<CIKeyEvent const*>(data);
        scalar acceleration = 1.0;

        if(kev->mod & CIKeyEvent::LShiftModifier)
            acceleration = 15;

        switch(kev->key)
        {
        case CK_w:
            camera.position.z() += 0.05f * acceleration;
            break;
        case CK_s:
            camera.position.z() -= 0.05f * acceleration;
            break;
        case CK_a:
            camera.position.x() += 0.05f * acceleration;
            break;
        case CK_d:
            camera.position.x() -= 0.05f * acceleration;
            break;
        case CK_q:
            camera.position.y() += 0.05f * acceleration;
            break;
        case CK_e:
            camera.position.y() -= 0.05f * acceleration;
            break;
        default:
            break;
        }
        break;
    }
    case CIEvent::MouseMove:
    {
        auto mev = C_CAST<CIMouseMoveEvent const*>(data);

        camera.rotation.x() += mev->delta.y * 0.05f;
        camera.rotation.y() += mev->delta.x * 0.05f;

        camera.rotation = normalize_quat(camera.rotation);

        break;
    }
    default:
        break;
    }
}

}
}
}
