#pragma once

#include "windowing/csdl2_window.h"

#if defined(COFFEE_USE_SDL_WINDOW)
#include "input/csdl2_eventhandler.h"
#include <coffee/core/CDRendererBase>

namespace Coffee {
namespace Display {

class SDL2WindowHost : public SDL2Window,
                       public SDL2EventHandler,
                       public CDRendererBase
{
  public:
    SDL2WindowHost(CObject* parent = nullptr) : CDRendererBase(parent)
    {
    }

    bool hostPreInit(const Properties& props, CString* err)
    {
        return windowPreInit(props, err) && inputPreInit(err);
    }
    bool hostInit(const Properties& props, CString* err)
    {
        return windowInit(props, err) && inputInit(err);
    }
    bool hostPostInit(const Properties& props, CString* err)
    {
        return windowPostInit(props, err) && inputPostInit(err);
    }

    bool init(const Properties& props, CString* err)
    {
        return hostPreInit(props, err) && hostInit(props, err) &&
               hostPostInit(props, err);
    }
    void run()
    {
    }
    void cleanup()
    {
        inputTerminate();
        windowTerminate();
    }

    void eventHandleD(const Event& e, c_cptr d)
    {
        SDL2EventHandler::eventHandleD(e, d);
    }

    void eventHandleI(const Input::CIEvent& e, c_cptr data)
    {
        SDL2EventHandler::eventHandleI(e, data);
    }
};

} // namespace Display
} // namespace Coffee
#endif
