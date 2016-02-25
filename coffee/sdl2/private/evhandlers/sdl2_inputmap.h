#pragma once

#include "../types/sdl2datatypes.h"
#include <coffee/core/CInput>

namespace Coffee{
namespace CSDL2Types{

using namespace CInput;

FORCEDINLINE CIMouseButtonEvent::MouseButton coffee_sdl2_translate_mouse_btn(
        Uint8 code)
{
    switch(code){
    case SDL_BUTTON_LEFT: return CIMouseButtonEvent::LeftButton;
    case SDL_BUTTON_MIDDLE: return CIMouseButtonEvent::MiddleButton;
    case SDL_BUTTON_RIGHT: return CIMouseButtonEvent::RightButton;
    case SDL_BUTTON_X1: return CIMouseButtonEvent::X1Button;
    case SDL_BUTTON_X2: return CIMouseButtonEvent::X2Button;

    //Any remaining buttons will map to the range 17-255
    default: return (CIMouseButtonEvent::MouseButton)(code+11);
    }
}

FORCEDINLINE uint8 coffee_sdl2_translate_mouse_btnmask(
        Uint32 code)
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

FORCEDINLINE CIKeyEvent::KeyModifiers coffee_sdl2_interpret_key_modifier(
        Uint16 mod)
{
    CIKeyEvent::KeyModifiers res = CIKeyEvent::NoneModifier;
    if(mod&KMOD_LSHIFT)
        res = res|CIKeyEvent::LShiftModifier;
    if(mod&KMOD_LCTRL)
        res = res|CIKeyEvent::LCtrlModifier;
    if(mod&KMOD_LALT)
        res = res|CIKeyEvent::LAltModifier;

    if(mod&KMOD_RSHIFT)
        res = res|CIKeyEvent::RShiftModifier;
    if(mod&KMOD_RCTRL)
        res = res|CIKeyEvent::RCtrlModifier;
    if(mod&KMOD_RALT)
        res = res|CIKeyEvent::RAltModifier;

    if(mod&KMOD_GUI)
        res = res|CIKeyEvent::SuperModifier;

    if(mod&KMOD_MODE)
        res = res|CIKeyEvent::AltGrModifier;

    return res;
}

FORCEDINLINE uint32 coffee_sdl2_interpret_symbol(
        SDL_Keycode key)
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

}
}