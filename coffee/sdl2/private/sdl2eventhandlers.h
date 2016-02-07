#ifndef SDL2EVENTHANDLERS
#define SDL2EVENTHANDLERS

#include "sdl2datatypes.h"
#include <coffee/sdl2/csdl2renderer.h>

namespace Coffee{
namespace CSDL2Types{

template<typename T>
inline void coffee_sdl2_eventatomic(CSDL2Renderer* ctxt, typename T::EventType type)
{
    T e;
    e.type = type;
    ctxt->eventHandle(e,nullptr);
}

template<typename T1,typename T2>
inline void coffee_sdl2_eventpack(CSDL2Renderer* ctxt, const T1* p1, const T2* p2)
{
    ctxt->eventHandle(*p1,p2);
}

extern void coffee_sdl2_eventhandle_all(CSDL2Renderer* ctxt,const SDL_Event* ev);

}
}
#endif // SDL2EVENTHANDLERS


