#pragma once

#include <coffee/core/CInput>

#include "../../types/sdl2datatypes.h"
#include "sdl2eventhandlers.h"
#include "sdl2_inputmap.h"

namespace Coffee{
namespace CSDL2Types{

FORCEDINLINE void coffee_sdl2_eventhandle_window_state(
        SDL2EventHandler* ctxt,
        Uint32 ts,
        CDStateEvent::StateChange state)
{
    CDEvent e;
    e.type = CDEvent::State;
    e.ts = ts;
    CDStateEvent s;
    s.type = state;

    EventPack(ctxt,&e,&s);
}

FORCEDINLINE void coffee_sdl2_eventhandle_window_focus(
        SDL2EventHandler* ctxt,
        Uint32 ts,
        Uint8 type)
{
    CDEvent e;
    e.type = CDEvent::Focus;
    e.ts = ts;

    CDFocusEvent s;
    s.mod = CDFocusEvent::FocusMask();

    switch(type){
    case SDL_WINDOWEVENT_ENTER: s.mod |= CDFocusEvent::Mouse | CDFocusEvent::Enter; break;
    case SDL_WINDOWEVENT_LEAVE: s.mod |= CDFocusEvent::Mouse; break;
    case SDL_WINDOWEVENT_EXPOSED: s.mod |= CDFocusEvent::Exposed; break;
    case SDL_WINDOWEVENT_FOCUS_GAINED: s.mod |= CDFocusEvent::Enter; break;
    case SDL_WINDOWEVENT_FOCUS_LOST: s.mod |= CDFocusEvent::Leave; break;
    }

    EventPack(ctxt,&e,&s);
}

FORCEDINLINE void coffee_sdl2_eventhandle_window_resize(
        SDL2EventHandler* ctxt,
        Uint32 ts,
        int32 w,
        int32 h)
{
    CDEvent e;
    e.type = CDEvent::Resize;
    e.ts = ts;

    CDResizeEvent s;
    s.w = w;
    s.h = h;

    EventPack(ctxt,&e,&s);
}

FORCEDINLINE void coffee_sdl2_eventhandle_window_move(
        SDL2EventHandler* ctxt,
        Uint32 ts,
        int32 x,
        int32 y)
{
    CDEvent e;
    e.type = CDEvent::Move;
    e.ts = ts;

    CDMoveEvent s;
    s.x = x;
    s.y = y;

    EventPack(ctxt,&e,&s);
}

FORCEDINLINE void EventHandleWindow(
        SDL2EventHandler* ctxt,
        const SDL_WindowEvent& win)
{
    switch(win.event){
    case SDL_WINDOWEVENT_ENTER:
    case SDL_WINDOWEVENT_LEAVE:
    case SDL_WINDOWEVENT_EXPOSED:
    case SDL_WINDOWEVENT_FOCUS_GAINED:{
        coffee_sdl2_eventhandle_window_focus(ctxt,win.timestamp,win.event);
        break;
    }
    case SDL_WINDOWEVENT_FOCUS_LOST:{
        coffee_sdl2_eventhandle_window_focus(ctxt,win.timestamp,win.event);
        break;
    }
    case SDL_WINDOWEVENT_CLOSE:{
        coffee_sdl2_eventhandle_window_state(ctxt,win.timestamp,CDStateEvent::Closed);
        break;
    }
    case SDL_WINDOWEVENT_MINIMIZED:{
        coffee_sdl2_eventhandle_window_state(ctxt,win.timestamp,CDStateEvent::Minimized);
        break;
    }
    case SDL_WINDOWEVENT_MAXIMIZED:{
        coffee_sdl2_eventhandle_window_state(ctxt,win.timestamp,CDStateEvent::Maximized);
        break;
    }
    case SDL_WINDOWEVENT_RESTORED:{
        coffee_sdl2_eventhandle_window_state(ctxt,win.timestamp,CDStateEvent::Restored);
        break;
    }
    case SDL_WINDOWEVENT_HIDDEN:{
        coffee_sdl2_eventhandle_window_state(ctxt,win.timestamp,CDStateEvent::Hidden);
        break;
    }
    case SDL_WINDOWEVENT_SHOWN:{
        coffee_sdl2_eventhandle_window_state(ctxt,win.timestamp,CDStateEvent::Shown);
        break;
    }
    case SDL_WINDOWEVENT_RESIZED:
    case SDL_WINDOWEVENT_SIZE_CHANGED:{
        coffee_sdl2_eventhandle_window_resize(ctxt,win.timestamp,win.data1,win.data2);
        break;
    }
    case SDL_WINDOWEVENT_MOVED:{
        coffee_sdl2_eventhandle_window_move(ctxt,win.timestamp,win.data1,win.data2);
        break;
    }
    }
}

}
}
