#include "sdl2eventhandlers.h"


namespace Coffee{
namespace CSDL2Types{

inline static void coffee_sdl2_send_atomic_ievent(
        CSDL2Renderer* ctxt,
        uint8 type)
{
    CIEvent e;
    e.type = type;
    ctxt->eventInputHandle(&e);
}

inline static void coffee_sdl2_send_atomic_wevent(
        CSDL2Renderer* ctxt,
        uint8 type)
{
    CDEvent e;
    e.type = type;
    ctxt->eventWindowsHandle(&e);
}

inline static void coffee_sdl2_send_full_ievent(
        CSDL2Renderer* ctxt,
        const void* base,
        szptr baseSize,
        const void* data,
        szptr dataSize)
{
    void* payload = malloc(baseSize+dataSize);
    memcpy(payload,base,baseSize);
    memcpy(&reinterpret_cast<char*>(payload)[baseSize],data,dataSize);


    ctxt->eventInputHandle((CIEvent*)payload);
    free(payload);
}

inline static void coffee_sdl2_send_full_wevent(
        CSDL2Renderer* ctxt,
        const void* base,
        szptr baseSize,
        const void* data,
        szptr dataSize)
{
    void* payload = malloc(baseSize+dataSize);
    memcpy(payload,base,baseSize);
    memcpy(&reinterpret_cast<char*>(payload)[baseSize],data,dataSize);

    ctxt->eventWindowsHandle((CDEvent*)payload);
    free(payload);
}

inline static uint8 coffee_sdl2_translate_mouse_btn(
        Uint8 code)
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

inline static uint8 coffee_sdl2_translate_mouse_btnmask(
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

inline static void coffee_sdl2_eventhandle_mouse_wheel(
        CSDL2Renderer* ctxt,
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

    coffee_sdl2_send_full_ievent(ctxt,&e,sizeof(e),&s,sizeof(s));
}

inline static void coffee_sdl2_eventhandle_mouse_motion(
        CSDL2Renderer* ctxt,
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

    coffee_sdl2_send_full_ievent(ctxt,&e,sizeof(e),&m,sizeof(m));
}

inline static void coffee_sdl2_eventhandle_mouse_btn(
        CSDL2Renderer* ctxt,
        const SDL_MouseButtonEvent& btn)
{
    if(btn.which==SDL_TOUCH_MOUSEID)
        return;

    CIEvent e;
    e.ts = btn.timestamp;
    e.type = CIEvent::MouseButton;

    CIMouseButtonEvent m;

    if(btn.state==SDL_PRESSED)
        m.mod|=CIMouseButtonEvent::Pressed;
    if(btn.clicks==2)
        m.mod|=CIMouseButtonEvent::DoubleClick;
    m.pos.x = btn.x;
    m.pos.y = btn.y;
    m.btn = coffee_sdl2_translate_mouse_btn(btn.button);

    coffee_sdl2_send_full_ievent(ctxt,&e,sizeof(e),&m,sizeof(m));
}

inline static int32 coffee_sdl2_interpret_key_modifier(
        Uint16 mod)
{
    int32 res = 0;
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

    coffee_sdl2_send_full_ievent(ctxt,&e,sizeof(e),&k,sizeof(k));
}

inline static void coffee_sdl2_eventhandle_drop(
        CSDL2Renderer* ctxt,
        const SDL_DropEvent& drop)
{
    CIEvent e;
    e.type = CIEvent::Drop;
    e.ts = drop.timestamp;
    szptr textsz = strlen(drop.file)+1;
    CIDropEvent* d = (CIDropEvent*)
            malloc(sizeof(CIDropEvent)
                   +textsz);
    d->size = textsz;
    memcpy((byte*)&d->text_data.text,drop.file,d->size);
    d->type = CIDropEvent::File;

    coffee_sdl2_send_full_ievent(ctxt,&e,sizeof(e),d,sizeof(CIDropEvent)+textsz);
}

inline static void coffee_sdl2_eventhandle_input(
        CSDL2Renderer* ctxt,
        const SDL_TextInputEvent& input)
{
    CIEvent e;
    e.type = CIEvent::TextInput;
    e.ts = input.timestamp;
    CIWriteEvent w;
    memcpy(w.text,input.text,ci_max_text_edit_size);

    coffee_sdl2_send_full_ievent(ctxt,&e,sizeof(e),&w,sizeof(w));
}

inline static void coffee_sdl2_eventhandle_inputedit( //Note: Yet to be tested
        CSDL2Renderer* ctxt,
        const SDL_TextEditingEvent& edit)
{
    CIEvent e;
    e.type = CIEvent::TextEdit;
    e.ts = edit.timestamp;

    CIWEditEvent w;
    memcpy(w.text,edit.text,ci_max_text_edit_size);
    w.cursor = edit.start;
    w.len = edit.length;

    coffee_sdl2_send_full_ievent(ctxt,&e,sizeof(e),&w,sizeof(w));
}

inline static void coffee_sdl2_eventhandle_controller_input(
        CSDL2Renderer* ctxt,
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

    coffee_sdl2_send_full_ievent(ctxt,&e,sizeof(e),&c,sizeof(c));
}

inline static void coffee_sdl2_eventhandle_controller_device(
        CSDL2Renderer* ctxt,
        const SDL_ControllerDeviceEvent& dev)
{
    CIEvent e;
    e.type = CIEvent::ControllerUpdate;
    e.ts = dev.timestamp;

    cstring name = SDL_GameControllerNameForIndex(dev.which);

    szptr evsize = sizeof(CIControllerAtomicUpdateEvent);
    if(name)
        evsize += strlen(name)+1;

    CIControllerAtomicUpdateEvent *c = (CIControllerAtomicUpdateEvent*)calloc(1,evsize);

    if(name)
        memcpy((byte*)&c->name,name,strlen(name)+1);
    else
        memset((byte*)&c->name,0,1);

    c->connected = false;
    c->remapped = false;
    switch(dev.type){
    case SDL_CONTROLLERDEVICEREMAPPED:
        c->remapped = true;
    case SDL_CONTROLLERDEVICEADDED:{
        c->connected = true;
        break;
        }
    }

    c->controller = dev.which;

    coffee_sdl2_send_full_ievent(ctxt,&e,sizeof(e),c,evsize);
}

inline static void coffee_sdl2_eventhandle_window_state(
        CSDL2Renderer* ctxt,
        Uint32 ts,
        uint8 state)
{
    CDEvent e;
    CDStateEvent s;
    e.type = CDEvent::State;
    e.ts = ts;
    s.type = state;

    coffee_sdl2_send_full_wevent(ctxt,&e,sizeof(e),&s,sizeof(s));
}

inline static void coffee_sdl2_eventhandle_window_focus(
        CSDL2Renderer* ctxt,
        Uint32 ts,
        Uint8 type)
{
    CDEvent e;
    CDFocusEvent s;
    e.type = CDEvent::Focus;
    e.ts = ts;
    s.mod = 0;

    switch(type){
    case SDL_WINDOWEVENT_ENTER: s.mod |= CDFocusEvent::Mouse | CDFocusEvent::Enter; break;
    case SDL_WINDOWEVENT_LEAVE: s.mod |= CDFocusEvent::Mouse; break;
    case SDL_WINDOWEVENT_EXPOSED: s.mod |= CDFocusEvent::Exposed; break;
    case SDL_WINDOWEVENT_FOCUS_GAINED: s.mod |= CDFocusEvent::Enter; break;
    }

    coffee_sdl2_send_full_wevent(ctxt,&e,sizeof(e),&s,sizeof(s));
}

inline static void coffee_sdl2_eventhandle_window_resize(
        CSDL2Renderer* ctxt,
        Uint32 ts,
        int32 w,
        int32 h)
{
    CDEvent e;
    CDResizeEvent s;
    e.type = CDEvent::Resize;
    e.ts = ts;
    s.w = w;
    s.h = h;

    coffee_sdl2_send_full_wevent(ctxt,&e,sizeof(e),&s,sizeof(s));
}

inline static void coffee_sdl2_eventhandle_window_move(
        CSDL2Renderer* ctxt,
        Uint32 ts,
        int32 x,
        int32 y)
{
    CDEvent e;
    CDMoveEvent s;
    e.type = CDEvent::Move;
    e.ts = ts;
    s.x = x;
    s.y = y;

    coffee_sdl2_send_full_wevent(ctxt,&e,sizeof(e),&s,sizeof(s));
}

inline static void coffee_sdl2_eventhandle_window(
        CSDL2Renderer* ctxt,
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

void coffee_sdl2_eventhandle_all(CSDL2Renderer *ctxt, const SDL_Event *ev){

    switch(ev->type){

    case SDL_QUIT:{
        coffee_sdl2_send_atomic_ievent(ctxt,CIEvent::QuitSign);
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
