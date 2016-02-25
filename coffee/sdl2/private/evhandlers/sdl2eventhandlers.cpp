#include "sdl2eventhandlers.h"

#include "sdl2_controller.h"
#include "sdl2_keyboard.h"
#include "sdl2_misc.h"
#include "sdl2_mouse.h"
#include "sdl2_window.h"

namespace Coffee{
namespace CSDL2Types{

void EventHandleAll(SDL2EventHandler *ctxt, const SDL_Event *ev)
{
    switch(ev->type){

    case SDL_QUIT:{
        EventAtomic<CIEvent>(ctxt,CIEvent::QuitSign);
        break;
    }

    case SDL_WINDOWEVENT:{
        EventHandleWindow(ctxt,ev->window);
        break;
    }

    case SDL_KEYDOWN:
    case SDL_KEYUP:{
        EventHandleKeys(ctxt,ev->type,ev->key);
        break;
    }

    case SDL_MOUSEBUTTONDOWN:
    case SDL_MOUSEBUTTONUP:{
        EventHandleMouseBtn(ctxt,ev->button);
        break;
    }
    case SDL_MOUSEMOTION:{
        EventHandleMouseMove(ctxt,ev->motion);
        break;
    }
    case SDL_MOUSEWHEEL:{
        EventHandleMouseWheel(ctxt,ev->wheel);
        break;
    }
    case SDL_JOYDEVICEADDED:
    case SDL_JOYDEVICEREMOVED:{
        SDL_ControllerDeviceEvent dev;
        dev.type = (ev->type==SDL_JOYDEVICEADDED)
                ? SDL_CONTROLLERDEVICEADDED
                : SDL_CONTROLLERDEVICEREMOVED;
        dev.which = ev->jdevice.which;
        dev.timestamp = ev->jdevice.timestamp;
        EventHandleControllerUpdate(ctxt,dev);
        break;
    }

    case SDL_CONTROLLERAXISMOTION:
    case SDL_CONTROLLERBUTTONDOWN:
    case SDL_CONTROLLERBUTTONUP:{
        EventHandleControllerInput(ctxt,ev->type,ev->caxis,ev->cbutton);
        break;
    }
    case SDL_CONTROLLERDEVICEADDED:
    case SDL_CONTROLLERDEVICEREMOVED:
    case SDL_CONTROLLERDEVICEREMAPPED:{
        EventHandleControllerUpdate(ctxt,ev->cdevice);
        break;
    }
    case SDL_DROPFILE:{
        EventHandleDrop(ctxt,ev->drop);
        break;
    }
    case SDL_TEXTEDITING:{
        EventHandleInputEdit(ctxt,ev->edit);
        break;
    }
    case SDL_TEXTINPUT:{
        EventHandleInput(ctxt,ev->text);
        break;
    }
    }

    return;
}

}
}
