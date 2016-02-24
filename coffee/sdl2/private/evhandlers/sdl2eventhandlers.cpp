#include "sdl2eventhandlers.h"

#include "sdl2_inputmap.h"

namespace Coffee{
namespace CSDL2Types{


inline static void coffee_sdl2_eventhandle_mouse_wheel(
        SDL2EventHandler* ctxt,
        const SDL_MouseWheelEvent& wheel)
{
    if(wheel.which==SDL_TOUCH_MOUSEID)
        return;

    CIEvent e;
    e.ts = wheel.timestamp;
    e.type = CIEvent::Scroll;

    CIScrollEvent s;

    s.delta.x = wheel.x;
    s.delta.y = wheel.y;

    EventPack(ctxt,&e,&s);
}

inline static void coffee_sdl2_eventhandle_mouse_motion(
        SDL2EventHandler* ctxt,
        const SDL_MouseMotionEvent& motion)
{
    if(motion.which==SDL_TOUCH_MOUSEID)
        return;

    CIEvent e;
    e.ts = motion.timestamp;
    e.type = CIEvent::MouseMove;

    CIMouseMoveEvent m;

    m.pos.x = motion.x;
    m.pos.y = motion.y;
    m.rel.x = motion.xrel;
    m.rel.y = motion.yrel;
    m.btn = coffee_sdl2_translate_mouse_btnmask(motion.state);

    EventPack(ctxt,&e,&m);
}

inline static void coffee_sdl2_eventhandle_mouse_btn(
        SDL2EventHandler* ctxt,
        const SDL_MouseButtonEvent& btn)
{
    if(btn.which==SDL_TOUCH_MOUSEID)
        return;

    CIEvent e;
    e.ts = btn.timestamp;
    e.type = CIEvent::MouseButton;

    CIMouseButtonEvent m;

    if(btn.state==SDL_PRESSED)
        m.mod = m.mod|CIMouseButtonEvent::Pressed;
    if(btn.clicks==2)
        m.mod = m.mod|CIMouseButtonEvent::DoubleClick;
    m.pos.x = btn.x;
    m.pos.y = btn.y;
    m.btn = coffee_sdl2_translate_mouse_btn(btn.button);

    EventPack(ctxt,&e,&m);
}

inline static CIKeyEvent::KeyModifiers coffee_sdl2_interpret_key_modifier(
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

inline static uint32 coffee_sdl2_interpret_symbol(
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

inline static void coffee_sdl2_eventhandle_keys(
        SDL2EventHandler* ctxt,
        Uint32 type,
        const SDL_KeyboardEvent& key)
{
    CIEvent e;
    CIKeyEvent k;

    e.type = CIEvent::Keyboard;
    e.ts = key.timestamp;

    if(key.repeat)
        k.mod = k.mod|CIKeyEvent::RepeatedModifier;
    if(key.type==SDL_KEYDOWN)
        k.mod = k.mod|CIKeyEvent::PressedModifier;
    k.mod = k.mod|coffee_sdl2_interpret_key_modifier(key.keysym.mod);

    if(key.keysym.sym<256)
        k.key = key.keysym.sym; //SDL uses Latin-1 (mostly, except for keypad)
    else
        k.key = coffee_sdl2_interpret_symbol(key.keysym.sym);

    k.scan = key.keysym.scancode;

    EventPack(ctxt,&e,&k);
}

inline static void coffee_sdl2_eventhandle_drop(
        SDL2EventHandler* ctxt,
        const SDL_DropEvent& drop)
{
    CIEvent e;
    e.type = CIEvent::Drop;
    e.ts = drop.timestamp;

    CIDropEvent d;
    d.size = CStrLen(drop.file)+1;
    d.type = CIDropEvent::File;
    d.text_data.text = drop.file;

    EventPack(ctxt,&e,&d);
}

inline static void coffee_sdl2_eventhandle_input(
        SDL2EventHandler* ctxt,
        const SDL_TextInputEvent& input)
{
    CIEvent e;
    e.type = CIEvent::TextInput;
    e.ts = input.timestamp;

    CIWriteEvent w;
    w.text = input.text;

    EventPack(ctxt,&e,&w);
}

inline static void coffee_sdl2_eventhandle_inputedit( //Note: Yet to be tested
        SDL2EventHandler* ctxt,
        const SDL_TextEditingEvent& edit)
{
    CIEvent e;
    e.type = CIEvent::TextEdit;
    e.ts = edit.timestamp;

    CIWEditEvent w;
    w.text = edit.text;
    w.cursor = edit.start;
    w.len = edit.length;

    EventPack(ctxt,&e,&w);
}

inline static void coffee_sdl2_eventhandle_controller_input(
        SDL2EventHandler* ctxt,
        Uint32 type,
        const SDL_ControllerAxisEvent& axis,
        const SDL_ControllerButtonEvent& btn)
{
    CIEvent e;
    e.type = CIEvent::Controller;

    CIControllerAtomicEvent c;

    e.ts = axis.timestamp;
    c.controller = axis.which+1;
    if(type==SDL_CONTROLLERAXISMOTION){
        c.axis = true;
        c.index = axis.axis;
        c.value = axis.value;
    }else{
        c.axis = false;
        c.index = btn.button;
        c.button_state = btn.state;
    }

    EventPack(ctxt,&e,&c);
}

inline static void coffee_sdl2_eventhandle_controller_device(
        SDL2EventHandler* ctxt,
        const SDL_ControllerDeviceEvent& dev)
{
    CIEvent e;
    e.type = CIEvent::ControllerUpdate;
    e.ts = dev.timestamp;

    cstring name = SDL_GameControllerNameForIndex(dev.which);

    CIControllerAtomicUpdateEvent c;

    c.name = name;
    c.connected = false;
    c.remapped = false;

    switch(dev.type){
    case SDL_CONTROLLERDEVICEREMAPPED:
        c.remapped = true;
    case SDL_CONTROLLERDEVICEADDED:{
        c.connected = true;
        break;
        }
    }

    c.controller = dev.which;

    EventPack(ctxt,&e,&c);
}

inline static void coffee_sdl2_eventhandle_window_state(
        SDL2EventHandler* ctxt,
        Uint32 ts,
        CDStateEvent::StateChange state)
{
    CDEvent e;
    e.type = CDEvent::State;
    e.ts = ts;
    CDStateEvent s;
    s.type = state;

    EventPack(ctxt,&e,&s);
}

inline static void coffee_sdl2_eventhandle_window_focus(
        SDL2EventHandler* ctxt,
        Uint32 ts,
        Uint8 type)
{
    CDEvent e;
    e.type = CDEvent::Focus;
    e.ts = ts;

    CDFocusEvent s;
    s.mod = CDFocusEvent::FocusMask();

    switch(type){
    case SDL_WINDOWEVENT_ENTER: s.mod |= CDFocusEvent::Mouse | CDFocusEvent::Enter; break;
    case SDL_WINDOWEVENT_LEAVE: s.mod |= CDFocusEvent::Mouse; break;
    case SDL_WINDOWEVENT_EXPOSED: s.mod |= CDFocusEvent::Exposed; break;
    case SDL_WINDOWEVENT_FOCUS_GAINED: s.mod |= CDFocusEvent::Enter; break;
    }

    EventPack(ctxt,&e,&s);
}

inline static void coffee_sdl2_eventhandle_window_resize(
        SDL2EventHandler* ctxt,
        Uint32 ts,
        int32 w,
        int32 h)
{
    CDEvent e;
    e.type = CDEvent::Resize;
    e.ts = ts;

    CDResizeEvent s;
    s.w = w;
    s.h = h;

    EventPack(ctxt,&e,&s);
}

inline static void coffee_sdl2_eventhandle_window_move(
        SDL2EventHandler* ctxt,
        Uint32 ts,
        int32 x,
        int32 y)
{
    CDEvent e;
    e.type = CDEvent::Move;
    e.ts = ts;

    CDMoveEvent s;
    s.x = x;
    s.y = y;

    EventPack(ctxt,&e,&s);
}

inline static void coffee_sdl2_eventhandle_window(
        SDL2EventHandler* ctxt,
        const SDL_WindowEvent& win)
{
    switch(win.event){
    case SDL_WINDOWEVENT_ENTER:
    case SDL_WINDOWEVENT_LEAVE:
    case SDL_WINDOWEVENT_EXPOSED:
    case SDL_WINDOWEVENT_FOCUS_GAINED:{
        coffee_sdl2_eventhandle_window_focus(ctxt,win.timestamp,win.event);
        break;
    }
    case SDL_WINDOWEVENT_CLOSE:{
        coffee_sdl2_eventhandle_window_state(ctxt,win.timestamp,CDStateEvent::Closed);
        break;
    }
    case SDL_WINDOWEVENT_MINIMIZED:{
        coffee_sdl2_eventhandle_window_state(ctxt,win.timestamp,CDStateEvent::Minimized);
        break;
    }
    case SDL_WINDOWEVENT_MAXIMIZED:{
        coffee_sdl2_eventhandle_window_state(ctxt,win.timestamp,CDStateEvent::Maximized);
        break;
    }
    case SDL_WINDOWEVENT_RESTORED:{
        coffee_sdl2_eventhandle_window_state(ctxt,win.timestamp,CDStateEvent::Restored);
        break;
    }
    case SDL_WINDOWEVENT_HIDDEN:{
        coffee_sdl2_eventhandle_window_state(ctxt,win.timestamp,CDStateEvent::Hidden);
        break;
    }
    case SDL_WINDOWEVENT_SHOWN:{
        coffee_sdl2_eventhandle_window_state(ctxt,win.timestamp,CDStateEvent::Shown);
        break;
    }
    case SDL_WINDOWEVENT_RESIZED:
    case SDL_WINDOWEVENT_SIZE_CHANGED:{
        coffee_sdl2_eventhandle_window_resize(ctxt,win.timestamp,win.data1,win.data2);
        break;
    }
    case SDL_WINDOWEVENT_MOVED:{
        coffee_sdl2_eventhandle_window_move(ctxt,win.timestamp,win.data1,win.data2);
        break;
    }
    }
}

void EventHandleAll(SDL2EventHandler *ctxt, const SDL_Event *ev)
{
    switch(ev->type){

    case SDL_QUIT:{
        EventAtomic<CIEvent>(ctxt,CIEvent::QuitSign);
        break;
    }

    case SDL_WINDOWEVENT:{
        coffee_sdl2_eventhandle_window(ctxt,ev->window);
        break;
    }

    case SDL_KEYDOWN:
    case SDL_KEYUP:{
        coffee_sdl2_eventhandle_keys(ctxt,ev->type,ev->key);
        break;
    }

    case SDL_MOUSEBUTTONDOWN:
    case SDL_MOUSEBUTTONUP:{
        coffee_sdl2_eventhandle_mouse_btn(ctxt,ev->button);
        break;
    }
    case SDL_MOUSEMOTION:{
        coffee_sdl2_eventhandle_mouse_motion(ctxt,ev->motion);
        break;
    }
    case SDL_MOUSEWHEEL:{
        coffee_sdl2_eventhandle_mouse_wheel(ctxt,ev->wheel);
        break;
    }
    case SDL_JOYDEVICEADDED:
    case SDL_JOYDEVICEREMOVED:{
        SDL_ControllerDeviceEvent dev;
        dev.type = (ev->type==SDL_JOYDEVICEADDED)
                ? SDL_CONTROLLERDEVICEADDED
                : SDL_CONTROLLERDEVICEREMOVED;
        dev.which = ev->jdevice.which;
        dev.timestamp = ev->jdevice.timestamp;
        coffee_sdl2_eventhandle_controller_device(ctxt,dev);
        break;
    }

    case SDL_CONTROLLERAXISMOTION:
    case SDL_CONTROLLERBUTTONDOWN:
    case SDL_CONTROLLERBUTTONUP:{
        coffee_sdl2_eventhandle_controller_input(ctxt,ev->type,ev->caxis,ev->cbutton);
        break;
    }
    case SDL_CONTROLLERDEVICEADDED:
    case SDL_CONTROLLERDEVICEREMOVED:
    case SDL_CONTROLLERDEVICEREMAPPED:{
        coffee_sdl2_eventhandle_controller_device(ctxt,ev->cdevice);
        break;
    }
    case SDL_DROPFILE:{
        coffee_sdl2_eventhandle_drop(ctxt,ev->drop);
        break;
    }
    case SDL_TEXTEDITING:{
        coffee_sdl2_eventhandle_inputedit(ctxt,ev->edit);
        break;
    }
    case SDL_TEXTINPUT:{
        coffee_sdl2_eventhandle_input(ctxt,ev->text);
        break;
    }
    }

    return;


}

}
}
