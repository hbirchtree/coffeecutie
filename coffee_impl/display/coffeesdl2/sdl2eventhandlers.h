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
        e.type = CIEvent::MouseMove;

        m.pos.x = motion.x;
        m.pos.y = motion.y;
        m.rel.x = motion.xrel;
        m.rel.y = motion.yrel;
        m.btn = coffee_sdl2_translate_mouse_btnmask(motion.state);

        coffee_sdl2_send_full_ievent(ctxt,&e,sizeof(e),&m,sizeof(CIMouseMoveEvent));
    }else if((type==SDL_MOUSEBUTTONDOWN||
              type==SDL_MOUSEBUTTONUP)&&
             btn.which!=SDL_TOUCH_MOUSEID){

        CIMouseButtonEvent m;
        e.ts = btn.timestamp;
        e.type = CIEvent::MouseButton;

        if(btn.state==SDL_PRESSED)
            m.modifier|=CIMouseButtonEvent::Pressed;
        if(btn.clicks==2)
            m.modifier|=CIMouseButtonEvent::DoubleClick;
        m.pos.x = btn.x;
        m.pos.y = btn.y;
        m.btn = coffee_sdl2_translate_mouse_btn(btn.button);

        coffee_sdl2_send_full_ievent(ctxt,&e,sizeof(e),&m,sizeof(CIMouseButtonEvent));
    }
}

inline static int32 coffee_sdl2_interpret_key_modifier(Uint16 mod)
{
    int32 res;
    if(mod&KMOD_LSHIFT)
        res|=CIKeyEvent::LShiftModifier;
    if(mod&KMOD_LCTRL)
        res|=CIKeyEvent::LCtrlModifier;
    if(mod&KMOD_LALT)
        res|=CIKeyEvent::LAltModifier;

    if(mod&KMOD_RSHIFT)
        res|=CIKeyEvent::RShiftModifier;
    if(mod&KMOD_RCTRL)
        res|=CIKeyEvent::RCtrlModifier;
    if(mod&KMOD_RALT)
        res|=CIKeyEvent::RAltModifier;

    if(mod&KMOD_GUI)
        res|=CIKeyEvent::SuperModifier;

    if(mod&KMOD_MODE)
        res|=CIKeyEvent::AltGrModifier;

        return res;
}

inline static uint32 coffee_sdl2_interpret_symbol(SDL_Keycode key)
{
    switch(key){

    case SDLK_KP_ENTER: return CK_KP_Enter;
    case SDLK_KP_MULTIPLY: return CK_KP_Mul;
    case SDLK_KP_DIVIDE: return CK_KP_Div;
    case SDLK_KP_PLUS: return CK_KP_Add;
    case SDLK_KP_MINUS: return CK_KP_Sub;
    case SDLK_KP_COMMA: return CK_KP_Comma;
    case SDLK_KP_0: return CK_KP_0;

    case SDLK_INSERT: return CK_Insert;
    case SDLK_HOME: return CK_Home;
    case SDLK_END: return CK_End;
    case SDLK_PAGEUP: return CK_PgUp;
    case SDLK_PAGEDOWN: return CK_PgDn;

    case SDLK_PRINTSCREEN: return CK_PrntScrn;
    case SDLK_PAUSE: return CK_Pause;

    case SDLK_LSHIFT: return CK_LShift;
    case SDLK_RSHIFT: return CK_RShift;
    case SDLK_LCTRL: return CK_LCtrl;
    case SDLK_RCTRL: return CK_RCtrl;
    case SDLK_LALT: return CK_LAlt;
    case SDLK_RALT: return CK_AltGr;
    case SDLK_LGUI: return CK_LSuper;
    case SDLK_RGUI: return CK_RSuper;

    case SDLK_CAPSLOCK: return CK_CapsLock;
    case SDLK_NUMLOCKCLEAR: return CK_NumLock;

    case SDLK_LEFT: return CK_Left;
    case SDLK_RIGHT: return CK_Right;
    case SDLK_UP: return CK_Up;
    case SDLK_DOWN: return CK_Down;

    default:break;
    }
    if(key>=SDLK_F1&&key<=SDLK_F12)
        return CK_F1+(key-SDLK_F1);
    if(key>=SDLK_KP_1&&key<=SDLK_KP_9)
        return CK_KP_1+(key-SDLK_KP_1);

    return 0;
}

inline static void coffee_sdl2_eventhandle_keys(
        CSDL2Renderer* ctxt,
        Uint32 type,
        const SDL_KeyboardEvent& key)
{
    CIEvent e;
    CIKeyEvent k;

    e.type = CIEvent::Keyboard;
    e.ts = key.timestamp;

    if(key.repeat)
        k.mod|=CIKeyEvent::RepeatedModifier;
    if(key.type==SDL_KEYDOWN)
        k.mod|=CIKeyEvent::PressedModifier;
    k.mod|=coffee_sdl2_interpret_key_modifier(key.keysym.mod);

    if(key.keysym.sym<256)
        k.key = key.keysym.sym; //SDL uses Latin-1 (mostly, except for keypad)
    else
        k.key = coffee_sdl2_interpret_symbol(key.keysym.sym);

    k.scan = key.keysym.scancode;

    coffee_sdl2_send_full_ievent(ctxt,&e,sizeof(e),&k,sizeof(CIKeyEvent));
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
        coffee_sdl2_eventhandle_keys(ctxt,ev->type,ev->key);
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


