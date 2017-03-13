#pragma once

#include <coffee/core/CInput>

#include "../../types/sdl2datatypes.h"

#include "sdl2_inputmap.h"
#include "sdl2eventhandlers.h"

namespace Coffee{
namespace SDL2{

using namespace CInput;

FORCEDINLINE void EventHandleKeys(
        SDL2EventHandler* ctxt,
        Uint32 type,
        const SDL_KeyboardEvent& key)
{
    CIEvent e;
    CIKeyEvent k;

    e.type = CIEvent::Keyboard;
    e.ts = key.timestamp;

    if(key.repeat)
        k.mod = k.mod|CIKeyEvent::RepeatedModifier;
    if(key.type==SDL_KEYDOWN)
        k.mod = k.mod|CIKeyEvent::PressedModifier;
    k.mod = k.mod|InterpretKeyModifier(key.keysym.mod);

    if(key.keysym.sym<256)
        k.key = key.keysym.sym; //SDL uses Latin-1 (mostly, except for keypad)
    else
        k.key = InterpretKeySymbol(key.keysym.sym);

    k.scan = key.keysym.scancode;

    EventPack(ctxt,&e,&k);
}

}
}