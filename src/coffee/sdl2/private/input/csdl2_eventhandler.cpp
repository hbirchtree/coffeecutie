#include <coffee/sdl2/input/csdl2_eventhandler.h>

#if defined(COFFEE_USE_SDL_EVENT)
#include <coffee/core/CFiles>
#include <coffee/core/CProfiling>

#include "evhandlers/sdl2eventhandlers.h"
#include "sdl2inputfun.h"

#if defined(COFFEE_USE_MAEMO_X11)
#include <coffee/windowing/windowing/x11/x11_window.h>
#endif

namespace Coffee {
namespace Display {

SDL2EventHandler::SDL2EventHandler()
{
    setSDL2Context(nullptr);
}

bool SDL2EventHandler::inputPreInit(CString*)
{
    DProfContext a("Setting up SDL input");

    /* Load input systems */

#if defined(COFFEE_LINUX)
    struct sigaction action;
    sigaction(SIGINT, nullptr, &action);
#endif

    if(SDL_InitSubSystem(SDL_INIT_EVENTS | SDL_INIT_GAMECONTROLLER) < 0)
    {
        cTag(SDLEV_API, "Failed to init: {0}", SDL_GetError());

        setSDL2Context(nullptr);
        return false;
    }

    if(SDL_InitSubSystem(SDL_INIT_HAPTIC) < 0)
    {
        cTag(SDLEV_API, "Failed to init haptics: {0}", SDL_GetError());
    }

#if defined(COFFEE_LINUX)
    sigaction(SIGINT, &action, nullptr);
#endif

    return true;
}

bool SDL2EventHandler::inputInit(CString*)
{
    DProfContext a(SDLEV_API "Loading SDL gamecontrollerdb");

    /* If found, load game controller mappings from file */
    auto mapping = "gamecontrollerdb.txt"_rsc;

    if(FileExists(mapping))
    {
        cTag("SDL2", "Found game controller mappings");
        auto map = C_OCAST<Bytes>(mapping);
        SDL_RWops* fsrc = SDL_RWFromConstMem(map.data, C_FCAST<int>(map.size));
        SDL_GameControllerAddMappingsFromRW(fsrc, 0);
        SDL_FreeRW(fsrc);
    } else
        Profiler::DeepProfile(SDLEV_API "Failed to locate gamecontrollerdb");
    return true;
}

bool SDL2EventHandler::inputPostInit(CString*)
{
    DProfContext a(SDLEV_API "Cleaning up joystick and haptic devices");

    /* WORKAROUND: Sometimes, haptic and joystick devices are not closed
     * properly. This part ensures that they are indeed closed before starting.
     *  Everything else should be fine. */
    for(int i = 0; i < SDL_NumHaptics(); i++)
    {
        SDL_Haptic* h = SDL_HapticOpen(i);
        SDL_HapticClose(h);
    }
    for(int i = 0; i < SDL_NumJoysticks(); i++)
    {
        SDL_Joystick* h = SDL_JoystickOpen(i);
        SDL_JoystickClose(h);
    }
    return true;
}

void SDL2EventHandler::inputTerminate()
{
    /* Close haptic devices and game controllers */
    if(getSDL2Context())
        for(Pair<uint8, SDL_GameController*> con :
            getSDL2Context()->controllers)
        {
            SDL_HapticClose(getSDL2Context()->haptics[con.first]);
            SDL_GameControllerClose(getSDL2Context()->controllers[con.first]);
        }
    SDL_QuitSubSystem(
        SDL_INIT_EVENTS | SDL_INIT_GAMECONTROLLER | SDL_INIT_HAPTIC);
}

void SDL2EventHandler::eventHandleD(const CDEvent& e, c_cptr d)
{
    internalProcessEvent(e, d);
}

/*!
 * \brief A default event handler created to intercept controller connect and
 * disconnect. This is optional, and the user may choose to implement their own
 * methods based on this. \param event
 */
void SDL2EventHandler::eventHandleI(const CIEvent& event, c_cptr data)
{
    if(!getSDL2Context())
        return;
    if(event.type == CIEvent::ControllerUpdate)
    {
        const CIControllerAtomicUpdateEvent* jev =
            (const CIControllerAtomicUpdateEvent*)data;
        _sdl2_controllers_handle(getSDL2Context(), this, jev);
    }
    internalProcessEvent(event, data);
}

void SDL2EventHandler::hapticInsert(
    const CIHapticEvent& e, C_UNUSED(c_cptr data))
{
    if(getSDL2Context())
        SDL_HapticRumblePlay(
            getSDL2Context()->haptics[e.rumble_input.index],
            e.rumble_input.strength,
            e.rumble_input.duration);
}

void SDL2EventHandler::setTextArea(const CRect& area)
{
    SDL_Rect r;

    r.x = area.x;
    r.y = area.y;
    r.w = area.w;
    r.h = area.h;

    SDL_SetTextInputRect(&r);
}

void SDL2EventHandler::eventHandle(const CIHapticEvent& haptic, c_cptr data)
{
    this->hapticInsert(haptic, data);
}

void SDL2EventHandler::eventHandle(const CIEvent& event, c_cptr data)
{
    this->eventHandleI(event, data);
}

void SDL2EventHandler::eventHandle(const CDEvent& event, c_cptr data)
{
    this->eventHandleD(event, data);
}

void SDL2EventHandler::setTextInputMode(bool m)
{
    if(m)
        SDL_StartTextInput();
    else
        SDL_StopTextInput();
}

bool SDL2EventHandler::textInputMode() const
{
    return SDL_IsTextInputActive();
}

void SDL2EventHandler::setKeyboardRepeat(C_UNUSED(bool m))
{
}

bool SDL2EventHandler::isMouseGrabbed() const
{
    if(getSDL2Context())
        return (SDL_GetWindowGrab(getSDL2Context()->window) == SDL_TRUE);
    return false;
}

void SDL2EventHandler::setMouseGrabbing(bool grab)
{
    if(getSDL2Context())
        SDL_SetWindowGrab(
            getSDL2Context()->window, (grab) ? SDL_TRUE : SDL_FALSE);
}

CPoint SDL2EventHandler::mousePosition() const
{
    CPoint p;
    SDL_GetMouseState(&p.x, &p.y);
    return p;
}

void SDL2EventHandler::setMousePosition(const CPoint& pos)
{
    if(getSDL2Context())
        SDL_WarpMouseInWindow(getSDL2Context()->window, pos.x, pos.y);
}

bool SDL2EventHandler::relativeMouse() const
{
    return SDL_GetRelativeMouseMode();
}

void SDL2EventHandler::setRelativeMouse(bool enable)
{
    SDL_SetRelativeMouseMode((enable) ? SDL_TRUE : SDL_FALSE);
}

CIControllerState SDL2EventHandler::getControllerState(uint16 index)
{
    if(!getSDL2Context())
        return {};

    SDL_GameController* gc = getSDL2Context()->controllers[index];
    if(!gc)
        return {};

    CIControllerState state;

    state.axes.e.l_x = SDL_GameControllerGetAxis(gc, SDL_CONTROLLER_AXIS_LEFTX);
    state.axes.e.l_y = SDL_GameControllerGetAxis(gc, SDL_CONTROLLER_AXIS_LEFTY);

    state.axes.e.r_x =
        SDL_GameControllerGetAxis(gc, SDL_CONTROLLER_AXIS_RIGHTX);
    state.axes.e.r_y =
        SDL_GameControllerGetAxis(gc, SDL_CONTROLLER_AXIS_RIGHTY);

    state.axes.e.t_l =
        SDL_GameControllerGetAxis(gc, SDL_CONTROLLER_AXIS_TRIGGERLEFT);
    state.axes.e.t_r =
        SDL_GameControllerGetAxis(gc, SDL_CONTROLLER_AXIS_TRIGGERRIGHT);

    state.buttons.e.a =
        SDL_GameControllerGetButton(gc, SDL_CONTROLLER_BUTTON_A);
    state.buttons.e.b =
        SDL_GameControllerGetButton(gc, SDL_CONTROLLER_BUTTON_B);
    state.buttons.e.x =
        SDL_GameControllerGetButton(gc, SDL_CONTROLLER_BUTTON_X);
    state.buttons.e.y =
        SDL_GameControllerGetButton(gc, SDL_CONTROLLER_BUTTON_Y);

    state.buttons.e.back =
        SDL_GameControllerGetButton(gc, SDL_CONTROLLER_BUTTON_BACK);
    state.buttons.e.guide =
        SDL_GameControllerGetButton(gc, SDL_CONTROLLER_BUTTON_GUIDE);
    state.buttons.e.start =
        SDL_GameControllerGetButton(gc, SDL_CONTROLLER_BUTTON_START);

    state.buttons.e.s_l =
        SDL_GameControllerGetButton(gc, SDL_CONTROLLER_BUTTON_LEFTSTICK);
    state.buttons.e.s_r =
        SDL_GameControllerGetButton(gc, SDL_CONTROLLER_BUTTON_RIGHTSTICK);

    state.buttons.e.b_l =
        SDL_GameControllerGetButton(gc, SDL_CONTROLLER_BUTTON_LEFTSHOULDER);
    state.buttons.e.b_r =
        SDL_GameControllerGetButton(gc, SDL_CONTROLLER_BUTTON_RIGHTSHOULDER);

    state.buttons.e.p_up =
        SDL_GameControllerGetButton(gc, SDL_CONTROLLER_BUTTON_DPAD_UP);
    state.buttons.e.p_down =
        SDL_GameControllerGetButton(gc, SDL_CONTROLLER_BUTTON_DPAD_DOWN);
    state.buttons.e.p_left =
        SDL_GameControllerGetButton(gc, SDL_CONTROLLER_BUTTON_DPAD_LEFT);
    state.buttons.e.p_right =
        SDL_GameControllerGetButton(gc, SDL_CONTROLLER_BUTTON_DPAD_RIGHT);

    return state;
}

void SDL2EventHandler::pollEvents()
{
    EventApplication::pollEvents();
    SDL_Event ev;
    while(SDL_PollEvent(&ev))
    {
        SDL2::EventHandleAll(this, &ev);
        for(EventHandlerSDL const& eh : m_eventhandlers_sdl)
            eh.func(eh.user_ptr, &ev, nullptr);
    }

#if defined(COFFEE_USE_MAEMO_X11)
    X11Window* xapp;

    xapp = C_DCAST<X11Window>((SDL2EventHandler*)this);

    if(xapp)
        xapp->processX11Events(this);
#endif
}

// bigscalar SDL2EventHandler::contextTime() const
//{
//    return static_cast<bigscalar>(SDL_GetTicks())/1000.0;
//}

bool SDL2EventHandler::closeFlag() const
{
    if(getSDL2Context())
        return (getSDL2Context()->contextFlags & 0x1) ||
               (EventApplication::closeFlag());
    else
        return EventApplication::closeFlag();
}

void SDL2EventHandler::internalProcessEvent(const CDEvent& e, c_cptr d)
{
    for(EventHandlerD& eh : m_eventhandlers_windw)
        eh.func(eh.user_ptr, e, d);
}

void SDL2EventHandler::internalProcessEvent(const CIEvent& e, c_cptr d)
{
    for(EventHandlerI& eh : m_eventhandlers_input)
        eh.func(eh.user_ptr, e, d);
}

bool SDL2EventHandler::installEventHandler(EventHandlerI e)
{
    m_eventhandlers_input.push_back(e);
    return true;
}

bool SDL2EventHandler::installEventHandler(EventHandlerD e)
{
    m_eventhandlers_windw.push_back(e);
    return true;
}

bool SDL2EventHandler::installEventHandler(EventHandlerSDL e)
{
    m_eventhandlers_sdl.push_back(e);
    return true;
}

Vector<EventApplication::EventHandlerI>* SDL2EventHandler::getEventHandlersI()
{
    return &m_eventhandlers_input;
}

Vector<EventApplication::EventHandlerD>* SDL2EventHandler::getEventHandlersD()
{
    return &m_eventhandlers_windw;
}

void SDL2EventHandler::injectEvent(const CIEvent& e, c_cptr d)
{
    eventHandleI(e, d);
}

void SDL2EventHandler::injectEvent(const CDEvent& e, c_cptr d)
{
    eventHandleD(e, d);
}

void SDL2EventHandler::injectEvent(SDL_Event* e)
{
    SDL_PushEvent(e);
}

void SDL2EventHandler::registerEventLoop(void* eventloop)
{
    m_eventloop = eventloop;
}

} // namespace Display
} // namespace Coffee
#endif
