#pragma once

#include <coffee/core/CInput>

#include "../../types/sdl2datatypes.h"
#include "sdl2_inputmap.h"
#include "sdl2eventhandlers.h"

namespace Coffee {
namespace SDL2 {

using namespace Display;

FORCEDINLINE void coffee_sdl2_eventhandle_window_state(
    SDL2EventHandler* ctxt, Uint32 ts, StateEvent::StateChange state)
{
    Event e;
    e.type = Event::State;
    e.ts   = ts;
    StateEvent s;
    s.type = state;

    EventPack(ctxt, &e, &s);
}

FORCEDINLINE void coffee_sdl2_eventhandle_window_focus(
    SDL2EventHandler* ctxt, Uint32 ts, Uint8 type)
{
    Event e;
    e.type = Event::Focus;
    e.ts   = ts;

    FocusEvent s;
    s.mod = FocusEvent::FocusMask();

    switch(type)
    {
    case SDL_WINDOWEVENT_ENTER:
        s.mod |= FocusEvent::Mouse | FocusEvent::Enter;
        break;
    case SDL_WINDOWEVENT_LEAVE:
        s.mod |= FocusEvent::Mouse;
        break;
    case SDL_WINDOWEVENT_EXPOSED:
        s.mod |= FocusEvent::Exposed;
        break;
    case SDL_WINDOWEVENT_FOCUS_GAINED:
        s.mod |= FocusEvent::Enter;
        break;
    case SDL_WINDOWEVENT_FOCUS_LOST:
        s.mod |= FocusEvent::Leave;
        break;
    }

    EventPack(ctxt, &e, &s);
}

FORCEDINLINE void coffee_sdl2_eventhandle_window_resize(
    SDL2EventHandler* ctxt, Uint32 ts, int32 w, int32 h)
{
    Event e;
    e.type = Event::Resize;
    e.ts   = ts;

    ResizeEvent s;
    s.w = w;
    s.h = h;

    EventPack(ctxt, &e, &s);
}

FORCEDINLINE void coffee_sdl2_eventhandle_window_move(
    SDL2EventHandler* ctxt, Uint32 ts, int32 x, int32 y)
{
    Event e;
    e.type = Event::Move;
    e.ts   = ts;

    MoveEvent s;
    s.x = x;
    s.y = y;

    EventPack(ctxt, &e, &s);
}

FORCEDINLINE void EventHandleWindow(
    SDL2EventHandler* ctxt, const SDL_WindowEvent& win)
{
    switch(win.event)
    {
    case SDL_WINDOWEVENT_ENTER:
    case SDL_WINDOWEVENT_LEAVE:
    case SDL_WINDOWEVENT_EXPOSED:
    case SDL_WINDOWEVENT_FOCUS_GAINED:
    {
        coffee_sdl2_eventhandle_window_focus(ctxt, win.timestamp, win.event);
        break;
    }
    case SDL_WINDOWEVENT_FOCUS_LOST:
    {
        coffee_sdl2_eventhandle_window_focus(ctxt, win.timestamp, win.event);
        break;
    }
    case SDL_WINDOWEVENT_CLOSE:
    {
        coffee_sdl2_eventhandle_window_state(
            ctxt, win.timestamp, StateEvent::Closed);
        break;
    }
    case SDL_WINDOWEVENT_MINIMIZED:
    {
        coffee_sdl2_eventhandle_window_state(
            ctxt, win.timestamp, StateEvent::Minimized);
        break;
    }
    case SDL_WINDOWEVENT_MAXIMIZED:
    {
        coffee_sdl2_eventhandle_window_state(
            ctxt, win.timestamp, StateEvent::Maximized);
        break;
    }
    case SDL_WINDOWEVENT_RESTORED:
    {
        coffee_sdl2_eventhandle_window_state(
            ctxt, win.timestamp, StateEvent::Restored);
        break;
    }
    case SDL_WINDOWEVENT_HIDDEN:
    {
        coffee_sdl2_eventhandle_window_state(
            ctxt, win.timestamp, StateEvent::Hidden);
        break;
    }
    case SDL_WINDOWEVENT_SHOWN:
    {
        coffee_sdl2_eventhandle_window_state(
            ctxt, win.timestamp, StateEvent::Shown);
        break;
    }
    case SDL_WINDOWEVENT_RESIZED:
    case SDL_WINDOWEVENT_SIZE_CHANGED:
    {
        coffee_sdl2_eventhandle_window_resize(
            ctxt, win.timestamp, win.data1, win.data2);
        break;
    }
    case SDL_WINDOWEVENT_MOVED:
    {
        coffee_sdl2_eventhandle_window_move(
            ctxt, win.timestamp, win.data1, win.data2);
        break;
    }
    }
}

} // namespace SDL2
} // namespace Coffee
