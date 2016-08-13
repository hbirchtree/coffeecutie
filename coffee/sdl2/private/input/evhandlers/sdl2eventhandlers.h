#ifndef SDL2EVENTHANDLERS
#define SDL2EVENTHANDLERS

#include <coffee/graphics_apis/gleam/csdl2renderer.h>

#include "../../types/sdl2datatypes.h"

namespace Coffee{
namespace CSDL2Types{

template<typename T>
FORCEDINLINE void EventAtomic(SDL2EventHandler* ctxt, typename T::EventType type)
{
    T e;
    e.type = type;
    ctxt->eventHandle(e,nullptr);
}

template<typename T1,typename T2>
FORCEDINLINE void EventPack(SDL2EventHandler* ctxt, const T1* p1, const T2* p2)
{
    ctxt->eventHandle(*p1,p2);
}

extern void EventHandleAll(SDL2EventHandler *ctxt, const SDL_Event* ev);

}
}
#endif // SDL2EVENTHANDLERS


