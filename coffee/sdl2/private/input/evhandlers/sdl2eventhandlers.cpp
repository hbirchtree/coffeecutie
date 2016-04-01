#include "sdl2eventhandlers.h"

#include <coffee/core/CDebug>

#include "sdl2_controller.h"
#include "sdl2_keyboard.h"
#include "sdl2_misc.h"
#include "sdl2_mouse.h"
#include "sdl2_touch.h"
#include "sdl2_window.h"

namespace Coffee{
namespace CSDL2Types{

void EventHandleAll(SDL2EventHandler *ctxt, const SDL_Event *ev)
{
    switch(ev->type){
    case SDL_APP_TERMINATING:
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

    case SDL_FINGERDOWN:
    case SDL_FINGERUP:
        EventHandleTap(ctxt,ev->tfinger);
        break;
    case SDL_FINGERMOTION:
        EventHandleTMotion(ctxt,ev->tfinger);
        break;

    case SDL_MULTIGESTURE:
        EventHandleMultiTouch(ctxt,ev->mgesture);
        break;

    case SDL_DOLLARGESTURE:
        break;
    case SDL_DOLLARRECORD:
        break;

    case SDL_APP_WILLENTERBACKGROUND:
        break;
    case SDL_APP_DIDENTERBACKGROUND:
        break;

    case SDL_APP_WILLENTERFOREGROUND:
        break;
    case SDL_APP_DIDENTERFOREGROUND:
        break;

    default:
    {
        cDebug("Unhandled SDL event: type={0}",ev->type);
        break;
    }
    }

    return;
}

}
}
