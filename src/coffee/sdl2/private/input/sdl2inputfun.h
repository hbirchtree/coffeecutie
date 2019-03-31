#pragma once

#include "../types/sdl2datatypes.h"
#include <coffee/core/CDebug>
#include <coffee/core/CInput>
#include <coffee/core/base/renderer/hapticapplication.h>

namespace Coffee {
namespace Display {

using namespace Input;

/*!
 * \brief Opens and acquires a haptic device for a joystick
 * \param controller Controller for which we acquire a haptic device
 * \param index Index of controller used for associating it with a controller
 * \param dev Reference to a pointer to the haptic device
 * \return A valid pointer to a CIHapticEvent
 */
C_DEPRECATED
CIEvent* sdl2_controller_get_haptic(
    SDL_GameController* controller, u8 index, SDL_Haptic*& dev)
{
    dev = nullptr;

    if(!controller)
        return nullptr;

    SDL_Joystick* js = SDL_GameControllerGetJoystick(controller);
    dev              = SDL_HapticOpenFromJoystick(js);
    if(dev)
    {
        int      idx   = SDL_HapticIndex(dev);
        cstring  hname = SDL_HapticName(idx);
        CIEvent* ev    = (CIEvent*)calloc(
            1,
            sizeof(CIEvent) + sizeof(CIHapticEvent) + libc::str::len(hname) -
                6);
        ev->type         = CIEvent::HapticDev;
        CIHapticEvent* h = (CIHapticEvent*)&ev[1];
        memcpy(
            (byte_t*)&h->rumble_device.name, hname, libc::str::len(hname) + 1);
        h->rumble_device.index = index;

        if(SDL_HapticRumbleInit(dev) == 0)
            return ev;
        ::free(ev);
        SDL_HapticClose(dev);
    }
    return nullptr;
}

C_DEPRECATED
void _sdl2_controllers_handle(
    SDL2::SDL2Context*                   m_context,
    HapticApplication*                   haptictarget,
    const CIControllerAtomicUpdateEvent* ev)
{
    if(ev->connected)
    {
        if(m_context->controllers[ev->controller])
            return;

        if(ev->remapped)
        {
            cMsg("SDL2", "Controller remapped: {0}", ev->controller);
            SDL_GameControllerClose(m_context->controllers.at(ev->controller));
        }
        SDL_GameController* gc = SDL_GameControllerOpen(ev->controller);
        if(!gc)
            m_context->joysticks[ev->controller] =
                SDL_JoystickOpen(ev->controller);
        else
            m_context->controllers[ev->controller] = gc;

        SDL_Haptic* hdev;
        CIEvent*    hev = sdl2_controller_get_haptic(gc, ev->controller, hdev);
        if(hev)
        {
            cMsg(
                "SDL2",
                "Controller {0} with rumble connected: {1}",
                ev->controller,
                ev->name);

            m_context->haptics[ev->controller] = hdev;
            haptictarget->hapticInsert((const CIHapticEvent&)hev[1], nullptr);
            ::free(hev);
        } else
            cMsg(
                "SDL2",
                "Controller {0} connected: {1}",
                ev->controller,
                ev->name);
    } else
    {
        if(!m_context->controllers[ev->controller])
            return;

        SDL_GameControllerClose(m_context->controllers[ev->controller]);
        m_context->controllers.erase(ev->controller);
        SDL_HapticClose(m_context->haptics[ev->controller]);
        m_context->haptics.erase(ev->controller);
        cMsg("SDL2", "Controller {0} disconnected", ev->controller);
    }
}

} // namespace Display
} // namespace Coffee
