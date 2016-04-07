#pragma once

#include "windowing/csdl2_window.h"
#include "input/csdl2_eventhandler.h"
#include <coffee/core/CDRendererBase>

namespace Coffee{
namespace Display{

class SDL2WindowHost :
        public SDL2Window,
        public SDL2EventHandler,
        public CDRendererBase
{
public:
    SDL2WindowHost(CObject* parent = nullptr):
        CDRendererBase(parent)
    {
    }

    bool hostPreInit(const CDProperties &props,CString*err)
    {
        return windowPreInit(props,err) && inputPreInit(err);
    }
    bool hostInit(const CDProperties &props,CString* err)
    {
        return windowInit(props,err) && inputInit(err);
    }
    bool hostPostInit(const CDProperties &props,CString* err)
    {
        return windowPostInit(props,err) && inputPostInit(err);
    }

    bool init(const CDProperties &props,CString* err)
    {
        return hostPreInit(props,err) && hostInit(props,err) && hostPostInit(props,err);
    }
    void run()
    {
    }
    void cleanup()
    {
        inputTerminate();
        windowTerminate();
    }

    void eventHandleD(const CDEvent &e, c_cptr d)
    {
        SDL2EventHandler::eventHandleD(e,d);
    }

    void eventHandleI(const CIEvent& e, c_cptr data)
    {
        SDL2EventHandler::eventHandleI(e,data);
        if(e.type==CIEvent::Keyboard)
        {
            CIKeyEvent const& ev = *(CIKeyEvent const*)data;
            if(ev.key == CK_Android_Back)
                exit(0);
        }
    }
};

}
}
