#ifndef SDL2EVENTHANDLERS
#define SDL2EVENTHANDLERS

#include "sdl2datatypes.h"
#include "coffee_impl/display/csdl2renderer.h"
#include "coffee_impl/input/cinput_eventhandling.h"

namespace Coffee{
namespace CSDL2Types{

inline static void coffee_sdl2_send_atomic_ievent(CSDL2Renderer* ctxt, uint8 type)
{
    CIEvent e;
    e.type = type;
    ctxt->eventIHandle(&e);
}

inline static void coffee_sdl2_send_atomic_wevent(CSDL2Renderer* ctxt, uint8 type)
{
    CDEvent e;
    e.type = type;
    ctxt->eventWHandle(&e);
}

inline static void coffee_sdl2_send_full_ievent(CSDL2Renderer* ctxt,
                                                const void* base,
                                                szptr baseSize,
                                                const void* data,
                                                szptr dataSize)
{
    void* payload = malloc(baseSize+dataSize);
    memcpy(payload,base,baseSize);
    memcpy(&reinterpret_cast<char*>(payload)[baseSize],data,dataSize);

    ctxt->eventIHandle((CIEvent*)payload);
    free(payload);
}

inline static void coffee_sdl2_eventhandle_mouse(
        CSDL2Renderer* ctxt,
        Uint32 type,
        const SDL_MouseButtonEvent& btn,
        const SDL_MouseMotionEvent& motion,
        const SDL_MouseWheelEvent& wheel)
{
    CIEvent e;

    if(type==SDL_MOUSEWHEEL&&wheel.which!=SDL_TOUCH_MOUSEID){
        CIScrollEvent s;
        e.ts = wheel.timestamp;
        e.type = CIEvent::Scroll;
        s.deltaX = wheel.x;
        s.deltaY = wheel.y;

        coffee_sdl2_send_full_ievent(ctxt,&e,sizeof(e),&s,sizeof(CIScrollEvent));
    }
}

inline static void coffee_sdl2_eventhandle_all(CSDL2Renderer* ctxt,const SDL_Event* ev){
    switch(ev->type){
    case SDL_QUIT:{
        coffee_sdl2_send_atomic_ievent(ctxt,CIEvent::QuitSign);
        break;
    }
    case SDL_WINDOWEVENT:{
        break;
    }
    case SDL_KEYDOWN:
    case SDL_KEYUP:{
        break;
    }
    case SDL_MOUSEBUTTONDOWN:
    case SDL_MOUSEBUTTONUP:
    case SDL_MOUSEMOTION:
    case SDL_MOUSEWHEEL:{
        coffee_sdl2_eventhandle_mouse(ctxt,ev->type,ev->button,ev->motion,ev->wheel);
        break;
    }
    case SDL_CONTROLLERAXISMOTION:
    case SDL_CONTROLLERBUTTONDOWN:
    case SDL_CONTROLLERBUTTONUP:

    case SDL_CONTROLLERDEVICEADDED:
    case SDL_CONTROLLERDEVICEREMOVED:
    case SDL_CONTROLLERDEVICEREMAPPED:{
        break;
    }
    case SDL_DROPFILE:{
        break;
    }
    case SDL_TEXTEDITING:
    case SDL_TEXTINPUT:{
        break;
    }
    }
}

}
}
#endif // SDL2EVENTHANDLERS

