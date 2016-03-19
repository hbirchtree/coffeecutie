#pragma once

#include "windowing/csdl2_window.h"
#include "input/csdl2_eventhandler.h"
#include <coffee/core/CDRendererBase>

namespace Coffee{
namespace CDisplay{

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

    void hostPreInit(const CDProperties &props)
    {
        windowPreInit(props);
        inputPreInit();
    }
    void hostInit(const CDProperties &props)
    {
        windowInit(props);
        inputInit();
    }
    void hostPostInit(const CDProperties &props)
    {
        windowPostInit(props);
        inputPostInit();
    }

    void init(const CDProperties &props)
    {
        hostPreInit(props);
        hostInit(props);
        hostPostInit(props);
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
    }
};

}
}