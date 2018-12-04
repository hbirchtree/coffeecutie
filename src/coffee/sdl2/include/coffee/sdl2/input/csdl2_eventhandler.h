#pragma once

#include <coffee/core/base/renderer/eventapplication.h>
#include <coffee/core/base/renderer/hapticapplication.h>

#if defined(COFFEE_USE_SDL_EVENT)
#include "../csdl2_context.h"

union SDL_Event;

namespace Coffee {
namespace Display {

class SDL2EventHandler : public virtual EventApplication,
                         public virtual HapticApplication,
                         public virtual SDL2ContextUser
{
  public:
    SDL2EventHandler();

  protected:
#if defined(COFFEE_USE_MAEMO_X11)
    CDWindow* m_window = nullptr;
#endif

    using EventHandlerSDL = EventHandler<SDL_Event*>;

    Vector<EventHandlerI>   m_eventhandlers_input;
    Vector<EventHandlerD>   m_eventhandlers_windw;
    Vector<EventHandlerSDL> m_eventhandlers_sdl;

    void* m_eventloop = nullptr;

    void internalProcessEvent(Event const& e, c_cptr d);
    void internalProcessEvent(Input::CIEvent const& e, c_cptr d);

    // InputApplication interface
  public:
    bool inputPreInit(CString*);
    bool inputInit(CString*);
    bool inputPostInit(CString*);
    void inputTerminate();

    void eventHandleD(const Event&, c_cptr);
    void eventHandleI(const Input::CIEvent& event, c_cptr data);

    Input::CIControllerState getControllerState(uint16 index);

    bool isMouseGrabbed() const;
    void setMouseGrabbing(bool m);

    Point mousePosition() const;
    void  setMousePosition(const Point& p);

    bool relativeMouse() const;
    void setRelativeMouse(bool enable);

    void setKeyboardRepeat(bool m);

    bool textInputMode() const;
    void setTextInputMode(bool m);

    void setTextArea(const Rect& area);

    void eventHandle(const Input::CIHapticEvent& haptic, c_cptr data);
    void eventHandle(const Input::CIEvent& event, c_cptr data);
    void eventHandle(const Event& event, c_cptr data);

    // HapticApplication interface
  public:
    void hapticInsert(const Input::CIHapticEvent& e, c_cptr data);

    // EventApplication interface
  public:
    void pollEvents();
    //    bigscalar contextTime() const;
    bool         closeFlag() const;
    virtual bool installEventHandler(EventHandlerI e);
    virtual bool installEventHandler(EventHandlerD e);
    virtual bool installEventHandler(EventHandlerSDL e);

    virtual Vector<EventHandlerI>* getEventHandlersI();
    virtual Vector<EventHandlerD>* getEventHandlersD();

    void injectEvent(const Input::CIEvent& e, c_cptr d);
    void injectEvent(const Event& e, c_cptr d);
    void injectEvent(SDL_Event* e);

    void registerEventLoop(void* eventloop);
};

} // namespace Display
} // namespace Coffee
#endif
