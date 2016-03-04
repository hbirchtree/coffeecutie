#pragma once

#include <coffee/core/CDebug>
#include <coffee/core/base/renderer/hapticapplication.h>
#include <coffee/core/CInput>
#include "../types/sdl2datatypes.h"

namespace Coffee{
namespace CDisplay{

using namespace CInput;

/*!
 * \brief Opens and acquires a haptic device for a joystick
 * \param controller Controller for which we acquire a haptic device
 * \param index Index of controller used for associating it with a controller
 * \param dev Reference to a pointer to the haptic device
 * \return A valid pointer to a CIHapticEvent
 */
CIEvent* sdl2_controller_get_haptic(
        SDL_GameController* controller,
        uint8 index,
        SDL_Haptic*& dev)
{
    dev = nullptr;

    if(!controller)
        return nullptr;

    SDL_Joystick* js = SDL_GameControllerGetJoystick(controller);
    dev = SDL_HapticOpenFromJoystick(js);
    if(dev)
    {
        int idx = SDL_HapticIndex(dev);
        cstring hname = SDL_HapticName(idx);
        CIEvent* ev = (CIEvent*)Calloc(1,sizeof(CIEvent)
                                       +sizeof(CIHapticEvent)
                                       +StrLen(hname)-6);
        ev->type = CIEvent::HapticDev;
        CIHapticEvent *h = (CIHapticEvent*)&ev[1];
        MemCpy((byte_t*)&h->rumble_device.name,
               hname,
               StrLen(hname)+1);
        h->rumble_device.index = index;

        if(SDL_HapticRumbleInit(dev)==0)
            return ev;
        CFree(ev);
        SDL_HapticClose(dev);
    }
    return nullptr;
}

void _sdl2_controllers_handle(CSDL2Types::CSDL2Context* m_context,
                              HapticApplication* haptictarget,
                              const CIControllerAtomicUpdateEvent *ev)
{
    if(ev->connected){
        if(m_context->controllers[ev->controller])
            return;

        if(ev->remapped){
            cMsg("SDL2","Controller remapped: {0}",ev->controller);
            SDL_GameControllerClose(m_context->controllers.at(ev->controller));
        }
        SDL_GameController* gc = SDL_GameControllerOpen(ev->controller);
        m_context->controllers[ev->controller] = gc;

        SDL_Haptic* hdev;
        CIEvent* hev = sdl2_controller_get_haptic(gc,ev->controller,hdev);
        if(hev)
        {
            cMsg("SDL2","Controller {0} with rumble connected: {1}",
                 ev->controller,
                 ev->name);

            m_context->haptics[ev->controller] = hdev;
            haptictarget->hapticInsert((const CIHapticEvent&)hev[1],nullptr);
            CFree(hev);
        }else
            cMsg("SDL2","Controller {0} connected: {1}",ev->controller,ev->name);
    }else{
        if(!m_context->controllers[ev->controller])
            return;

        SDL_GameControllerClose(m_context->controllers[ev->controller]);
        m_context->controllers.erase(ev->controller);
        SDL_HapticClose(m_context->haptics[ev->controller]);
        m_context->haptics.erase(ev->controller);
        cMsg("SDL2","Controller {0} disconnected",ev->controller);
    }
}

}
}