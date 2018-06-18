#pragma once

#include "../../types/sdl2datatypes.h"
#include "sdl2eventhandlers.h"
#include "sdl2_inputmap.h"
#include <coffee/core/CInput>

namespace Coffee{
namespace SDL2{

FORCEDINLINE void EventHandleControllerInput(
        SDL2EventHandler* ctxt,
        Uint32 type,
        const SDL_ControllerAxisEvent& axis,
        const SDL_ControllerButtonEvent& btn)
{
    CIEvent e;
    e.type = CIEvent::Controller;

    CIControllerAtomicEvent c;

    e.ts = axis.timestamp;
    c.controller = axis.which+1;
    if(type==SDL_CONTROLLERAXISMOTION){
        c.axis = true;
        c.index = axis.axis;
        c.value = axis.value;
    }else{
        c.axis = false;
        c.index = btn.button;
        c.button_state = btn.state;
    }

    EventPack(ctxt,&e,&c);
}

FORCEDINLINE void EventHandleControllerUpdate(
        SDL2EventHandler* ctxt,
        const SDL_ControllerDeviceEvent& dev)
{
    CIEvent e;
    e.type = CIEvent::ControllerUpdate;
    e.ts = dev.timestamp;

    cstring name = nullptr;

    if (dev.type != SDL_CONTROLLERDEVICEREMOVED)
        name = SDL_GameControllerNameForIndex(dev.which);
    if(!name)
    {
        /* This is the case for SDL_JOYDEVICEADDED */
        name = SDL_JoystickNameForIndex(dev.which);
    }

    CIControllerAtomicUpdateEvent c;

    c.name = name;
    c.connected = false;
    c.remapped = false;

    switch(dev.type){
    case SDL_CONTROLLERDEVICEREMAPPED:
        c.remapped = true;
    case SDL_CONTROLLERDEVICEADDED:{
        c.connected = true;
        break;
        }
    }

    c.controller = dev.which;

    EventPack(ctxt,&e,&c);
}

}
}
