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

inline static uint8 coffee_sdl2_translate_mouse_btn(Uint8 code)
{
    switch(code){
    case SDL_BUTTON_LEFT: return CIMouseButtonEvent::LeftButton;
    case SDL_BUTTON_MIDDLE: return CIMouseButtonEvent::MiddleButton;
    case SDL_BUTTON_RIGHT: return CIMouseButtonEvent::RightButton;
    case SDL_BUTTON_X1: return CIMouseButtonEvent::X1Button;
    case SDL_BUTTON_X2: return CIMouseButtonEvent::X2Button;

    //Any remaining buttons will map to the range 17-255
    default: return code+11;
    }
}

inline static uint8 coffee_sdl2_translate_mouse_btnmask(Uint32 code)
{
    uint8 res = 0;

    if(code&SDL_BUTTON_LMASK)
        res |= CIMouseButtonEvent::LeftButton;
    if(code&SDL_BUTTON_MMASK)
        res |= CIMouseButtonEvent::MiddleButton;
    if(code&SDL_BUTTON_RMASK)
        res |= CIMouseButtonEvent::RightButton;
    if(code&SDL_BUTTON_X1MASK)
        res |= CIMouseButtonEvent::X1Button;
    if(code&SDL_BUTTON_X2MASK)
        res |= CIMouseButtonEvent::X2Button;

    return res;
}

inline static void coffee_sdl2_eventhandle_mouse(
        CSDL2Renderer* ctxt,
        Uint32 type,
        const SDL_MouseButtonEvent& btn,
        const SDL_MouseMotionEvent& motion,
        const SDL_MouseWheelEvent& wheel)
{
    CIEvent e;

    if(type==SDL_MOUSEWHEEL
            &&wheel.which!=SDL_TOUCH_MOUSEID){
        CIScrollEvent s;
        e.ts = wheel.timestamp;
        e.type = CIEvent::Scroll;

        s.delta.x = wheel.x;
        s.delta.y = wheel.y;

        coffee_sdl2_send_full_ievent(ctxt,&e,sizeof(e),&s,sizeof(CIScrollEvent));
    }else if(type==SDL_MOUSEMOTION&&
             motion.which!=SDL_TOUCH_MOUSEID){
        CIMouseMoveEvent m;
        e.ts = motion.timestamp;

        m.pos.x = motion.x;
        m.pos.y = motion.y;
        m.rel.x = motion.xrel;
        m.rel.y = motion.yrel;
        m.btn = coffee_sdl2_translate_mouse_btnmask(motion.state);

    }else if((type==SDL_MOUSEBUTTONDOWN||
              type==SDL_MOUSEBUTTONUP)&&
             btn.which!=SDL_TOUCH_MOUSEID){
        CIMouseButtonEvent m;
        e.ts = btn.timestamp;

        if(btn.state==SDL_PRESSED)
            m.modifier|=CIMouseButtonEvent::Pressed;
        m.pos.x = btn.x;
        m.pos.y = btn.y;
        m.btn = coffee_sdl2_translate_mouse_btn(btn.button);
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

