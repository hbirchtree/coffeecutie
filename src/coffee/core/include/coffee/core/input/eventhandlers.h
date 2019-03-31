#pragma once

#include <coffee/core/types/display/event.h>
#include <coffee/core/types/input/event_types.h>
#include <coffee/core/types/rect.h>

#include "../base/renderer/windowapplication.h"

namespace Coffee {
namespace Display {
namespace EventHandlers {

using namespace Input;

template<class GLM>
FORCEDINLINE void ResizeWindowUniversal(const Event& e, c_cptr data)
{
    if(e.type == Event::Resize)
    {
        auto   rev = C_CAST<const ResizeEvent*>(data);
        Rect64 view(0, 0, rev->w, rev->h);
        GLM::DefaultFramebuffer().resize(0, view);
    }
}

template<typename T>
FORCEDINLINE void ExitOnQuitSignal(void* rp, const CIEvent& e, c_cptr)
{
    T* r = C_FCAST<T*>(rp);
    if(e.type == CIEvent::QuitSign)
        r->closeWindow();
}

template<class GL>
FORCEDINLINE void ResizeWindow(const Event& e, c_cptr data)
{
    if(e.type == Event::Resize)
    {
        auto    rev = C_CAST<const ResizeEvent*>(data);
        Rect64 view(0, 0, rev->w, rev->h);
        GL::ViewportSet(view);
    }
}

template<typename T>
FORCEDINLINE void EscapeCloseWindow(T* r, const CIEvent& e, c_cptr data)
{
    if(e.type == CIEvent::Keyboard)
    {
        auto kev = C_CAST<const CIKeyEvent*>(data);
        if(kev->key == CK_Escape)
            r->closeWindow();
    }
}

template<typename T>
FORCEDINLINE void WindowManagerCloseWindow(
    T* r, const Event& event, c_cptr data)
{
    if(event.type == Event::State)
    {
        const StateEvent* sev = C_CAST<const StateEvent*>(data);
        if(sev->type == StateEvent::Closed)
            r->closeWindow();
    }
}

template<typename T>
FORCEDINLINE void WindowManagerFullscreen(T* r, CIEvent const& e, c_cptr data)
{
#if !defined(COFFEE_ANDROID) || !defined(COFFEE_APPLE_MOBILE)
    if(e.type == CIEvent::Keyboard)
    {
        auto kev = C_CAST<CIKeyEvent const*>(data);

        if(kev->mod & CIKeyEvent::RepeatedModifier ||
           kev->mod & CIKeyEvent::PressedModifier)
            return;

        switch(kev->key)
        {
        case CK_EnterCR:
        case CK_EnterNL:
            if(!(kev->mod & CIKeyEvent::LAltModifier))
                break;
        case CK_F11:
            if(r->windowState() & Properties::Windowed)
                r->setWindowState(Properties::WindowedFullScreen);
            else
                r->setWindowState(Properties::Windowed);
            break;
        default:
            break;
        }
    }
#endif
}

/* These are pluggable event handlers */

template<typename WindowHandler>
void EscapeCloseWindow(void* r, CIEvent const& e, c_cptr data)
{
    EscapeCloseWindow<WindowHandler>(C_CAST<WindowHandler*>(r), e, data);
}
template<typename WindowHandler>
void WindowManagerCloseWindow(void* r, Event const& e, c_cptr data)
{
    WindowManagerCloseWindow<WindowHandler>(C_CAST<WindowHandler*>(r), e, data);
}
template<typename WindowHandler>
void WindowManagerFullscreen(void* r, CIEvent const& e, c_cptr data)
{
    WindowManagerFullscreen(C_CAST<WindowHandler*>(r), e, data);
}
template<typename GraphicsHandler>
void ResizeWindowUniversal(void*, Event const& e, c_cptr data)
{
    ResizeWindowUniversal<GraphicsHandler>(e, data);
}

} // namespace EventHandlers
} // namespace Display
} // namespace Coffee
