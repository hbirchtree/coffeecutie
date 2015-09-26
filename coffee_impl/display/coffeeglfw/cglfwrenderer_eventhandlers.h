#ifndef CGLFWRENDERER_EVENTHANDLERS_H
#define CGLFWRENDERER_EVENTHANDLERS_H

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"

#include "coffee_impl/display/cglfwrenderer.h"
#include "coffee/cinput.h"

#include "coffee_impl/input/cinput_eventhandling.h"

using namespace Coffee;
using namespace Coffee::CInput;
using namespace Coffee::CDisplay;

inline static CGLFWRenderer* getPtr(GLFWwindow* w){
    return static_cast<CGLFWRenderer*>(glfwGetWindowUserPointer(w));
}


inline static void _createEvent(
        CIEvent::EventType t,
        void** outData,
        uint32* size,
        const void* data,
        uint32 isize){
    CIEvent ev;
    ev.type = t;

    *outData = realloc(*outData,sizeof(ev)+isize);
    char* bytes = reinterpret_cast<char*>(*outData);

    memcpy(bytes,&ev,sizeof(ev));
    memcpy(bytes+sizeof(ev),data,isize);
    *size = sizeof(ev)+isize;
}

inline static void _createWEvent(
        CDEvent::EventType t,
        void** outData,
        uint32* size,
        const void* data,
        uint32 isize){
    CDEvent ev;
    ev.type = t;

    *outData = realloc(*outData,sizeof(ev)+isize);
    byte* bytes = reinterpret_cast<byte*>(*outData);

    memcpy(bytes,&ev,sizeof(ev));
    memcpy(bytes+sizeof(ev),data,isize);
    *size = sizeof(ev)+isize;
}

inline static void _windowEventHandle(
        CDEvent::EventType type,
        GLFWwindow* win,
        const void* idata,
        uint32 isize)
{
    CGLFWRenderer* ptr = getPtr(win);

    void* data = nullptr;
    uint32 size = 0;

    _createWEvent(type,&data,&size,idata,isize);

    CDEvent* ev = reinterpret_cast<CDEvent*>(data);
    ptr->eventWHandle(ev);

    free(data);
}

inline static void _inputEventHandle(
        CIEvent::EventType type,
        GLFWwindow* win,
        const void* idata,
        uint32 isize)
{
    CGLFWRenderer* ptr = getPtr(win);

    void* data = nullptr;
    uint32 size = 0;

    _createEvent(type,&data,&size,idata,isize);

    CIEvent* ev = reinterpret_cast<CIEvent*>(data);
    ptr->eventIHandle(ev);

    free(data);
}

//These generate input events
static void glfw_input_mouseBtn         (GLFWwindow* win, int btn, int act, int mods){
    CIMouseButtonEvent m;
    m.mod = (act==GLFW_PRESS) ? CIMouseButtonEvent::Pressed : 0;

    if(btn==GLFW_MOUSE_BUTTON_MIDDLE)
        m.btn = CIMouseButtonEvent::MiddleButton;
    else if(btn==GLFW_MOUSE_BUTTON_RIGHT)
        m.btn = CIMouseButtonEvent::RightButton;
    else
        m.btn = btn+1;

    m.mod = mods;

    _inputEventHandle(CIEvent::MouseButton, win, &m, sizeof(m));
}

static void glfw_input_mouseMove        (GLFWwindow* win, double x, double y)
{
    CIMouseMoveEvent m;

    m.pos.x = x;
    m.pos.y = y;

    _inputEventHandle(CIEvent::MouseMove, win, &m, sizeof(m));
}

static void glfw_input_mouseenter       (GLFWwindow* win, int val)
{
    CIFocusEvent m;
    m.mod = (val) ? CIFocusEvent::Enter : CIFocusEvent::Leave;

    _inputEventHandle(CIEvent::Focus, win, &m, sizeof(m));
}

static void glfw_input_scroll           (GLFWwindow* win, double xoffset, double yoffset)
{
    CIScrollEvent m;
    m.delta.x = xoffset;
    m.delta.y = yoffset;

    _inputEventHandle(CIEvent::Scroll, win, &m, sizeof(m));
}
static void glfw_input_dropevent        (GLFWwindow* win, int numfiles, const char** paths)
{
    CIDropEvent m;
    m.type = CIDropEvent::Link;
    m.size = numfiles;
    m.data = paths;

    _inputEventHandle(CIEvent::Drop, win, &m, sizeof(m));
}

inline static uint32 glfw_translate_kbdkey(int key)
{
    if(key>=GLFW_KEY_A&&key<=GLFW_KEY_Z)
        return CK_a+key-GLFW_KEY_A;
    if(key>0&&key<256)
        return key;
    switch(key){
    case GLFW_KEY_ESCAPE: return CK_Escape;
    default:break;
    }
    return 0;
}

static void glfw_input_kbdKey           (GLFWwindow* win, int key, int scancode, int action, int mods)
{
    CIKeyEvent m;
    m.mod |= (action==GLFW_PRESS) ? CIKeyEvent::PressedModifier :(action==GLFW_RELEASE) ? 0 : CIKeyEvent::RepeatedModifier|CIKeyEvent::PressedModifier;

    m.scan = scancode;
    m.key = glfw_translate_kbdkey(key);
    m.mod |= mods;

    _inputEventHandle(CIEvent::Keyboard, win, &m, sizeof(m));
}
static void glfw_input_charwrite        (GLFWwindow *win, unsigned int character)
{
    CIWriteEvent m;
    m.character = character;

    _inputEventHandle(CIEvent::TextInput, win, &m, sizeof(m));
}

//These generate window events
static void glfw_win_resize             (GLFWwindow* win, int width, int height)
{
    CDResizeEvent m;
    m.w = width;
    m.h = height;

    _windowEventHandle(CDEvent::WindowResized, win, &m, sizeof(m));
}
static void glfw_win_fbresize           (GLFWwindow* win, int width, int height)
{
    CDResizeEvent m;
    m.w = width;
    m.h = height;

    _windowEventHandle(CDEvent::FramebufferResized, win, &m, sizeof(m));
}
static void glfw_win_focus              (GLFWwindow* win, int val)
{
    CDStateEvent m;
    m.bits = (val) ? CDStateEvent::GainedFocus : CDStateEvent::LostFocus;

    _windowEventHandle(CDEvent::WindowStateChanged, win, &m, sizeof(m));
}
static void glfw_win_pos                (GLFWwindow* win, int x, int y)
{
    CDMoveEvent m;
    m.x = x;
    m.y = y;

    _windowEventHandle(CDEvent::WindowMoved, win, &m, sizeof(m));
}
static void glfw_win_close              (GLFWwindow* win)
{
    CDStateEvent m;
    m.bits = CDStateEvent::Closed;

    _windowEventHandle(CDEvent::WindowStateChanged, win, &m, sizeof(m));
}
static void glfw_win_state              (GLFWwindow* win, int val)
{
    CDStateEvent m;
    m.bits = (val) ? CDStateEvent::Minimized : CDStateEvent::Maximized;

    _windowEventHandle(CDEvent::WindowStateChanged, win, &m, sizeof(m));
}

//This generates errors. All the time.
static void glfw_error_function         (int stat, const char* message)
{
    cDebug("GLFW error code %i: %s",stat,message);
}

#endif // CGLFWRENDERER_EVENTHANDLERS_H
