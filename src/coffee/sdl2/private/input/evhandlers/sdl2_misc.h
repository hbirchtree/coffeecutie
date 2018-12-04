#pragma once

#include "../../types/sdl2datatypes.h"
#include "sdl2eventhandlers.h"
#include <coffee/core/CInput>
#include <peripherals/libc/string_ops.h>

namespace Coffee {
namespace SDL2 {

using namespace Input;

FORCEDINLINE void EventHandleDrop(
    SDL2EventHandler* ctxt, const SDL_DropEvent& drop)
{
    CIEvent e;
    e.type = CIEvent::Drop;
    e.ts   = drop.timestamp;

    CIDropEvent d;
    d.size           = libc::str::len(drop.file) + 1;
    d.type           = CIDropEvent::File;
    d.text_data.text = drop.file;

    EventPack(ctxt, &e, &d);
}

FORCEDINLINE void EventHandleInput(
    SDL2EventHandler* ctxt, const SDL_TextInputEvent& input)
{
    CIEvent e;
    e.type = CIEvent::TextInput;
    e.ts   = input.timestamp;

    CIWriteEvent w;
    w.text = input.text;

    EventPack(ctxt, &e, &w);
}

FORCEDINLINE void EventHandleInputEdit( // Note: Yet to be tested
    SDL2EventHandler*           ctxt,
    const SDL_TextEditingEvent& edit)
{
    CIEvent e;
    e.type = CIEvent::TextEdit;
    e.ts   = edit.timestamp;

    CIWEditEvent w;
    w.text   = edit.text;
    w.cursor = edit.start;
    w.len    = edit.length;

    EventPack(ctxt, &e, &w);
}

} // namespace SDL2
} // namespace Coffee
