#ifndef CGLFWRENDERER_EVENTHANDLERS_H
#define CGLFWRENDERER_EVENTHANDLERS_H

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"

#include "cglfwrenderer.h"
#include "coffee/cinput.h"

using namespace Coffee::CInput;
using namespace Coffee::CDisplay;

inline static CGLFWRenderer* getPtr(GLFWwindow* w){
    return static_cast<CGLFWRenderer*>(glfwGetWindowUserPointer(w));
}

inline static void _createEvent(CIEvent::EventType t, void** outData, uint32_t* size, const void* data, uint32_t isize){
    CIEvent ev;
    ev.type = t;

    *outData = realloc(*outData,sizeof(ev)+isize);
    char* bytes = reinterpret_cast<char*>(*outData);

    memcpy(bytes,&ev,sizeof(ev));
    memcpy(bytes+sizeof(ev),data,isize);
    *size = sizeof(ev)+isize;
}

inline static void _createWEvent(CDEvent::EventType t, void** outData, uint32_t* size, const void* data, uint32_t isize){
    CDEvent ev;
    ev.type = t;

    *outData = realloc(*outData,sizeof(ev)+isize);
    char* bytes = reinterpret_cast<char*>(*outData);

    memcpy(bytes,&ev,sizeof(ev));
    memcpy(bytes+sizeof(ev),data,isize);
    *size = sizeof(ev)+isize;
}

inline static void _inputEventHandle(CIEvent::EventType type, GLFWwindow* win,
                              const void* idata, uint32_t isize)
{
    CGLFWRenderer* ptr = getPtr(win);

    void* data = nullptr;
    uint32_t size = 0;

    _createEvent(type,&data,&size,idata,isize);

    //Do something with ptr here
    free(data);
}

inline static void _windowEventHandle(CDEvent::EventType type, GLFWwindow* win,
                              const void* idata, uint32_t isize)
{
    CGLFWRenderer* ptr = getPtr(win);

    void* data = nullptr;
    uint32_t size = 0;

    _createWEvent(type,&data,&size,idata,isize);

    //Do something with ptr here
    free(data);
}

//These generate input events
static void glfw_input_mouseBtn         (GLFWwindow* win, int btn, int act, int mods){
    CIMouseEvent m;
    m.type = (act==GLFW_PRESS) ? CIMouseEvent::Press : CIMouseEvent::Release;

    if(btn==GLFW_MOUSE_BUTTON_MIDDLE)
        m.keyCode = CIMouseEvent::MiddleButton;
    else if(btn==GLFW_MOUSE_BUTTON_RIGHT)
        m.keyCode = CIMouseEvent::RightButton;
    else
        m.keyCode = btn+1;

    m.modifier = mods;

    _inputEventHandle(CIEvent::Mouse, win, &m, sizeof(m));
}

static void glfw_input_mouseMove        (GLFWwindow* win, double x, double y)
{
    CIMouseEvent m;
    m.type = CIMouseEvent::Move;

    m.x = x;
    m.y = y;

    _inputEventHandle(CIEvent::Mouse, win, &m, sizeof(m));
}

static void glfw_input_mouseenter       (GLFWwindow* win, int val)
{
    CIMouseEvent m;
    m.type = (val) ? CIMouseEvent::Enter : CIMouseEvent::Leave;

    _inputEventHandle(CIEvent::Mouse, win, &m, sizeof(m));
}

static void glfw_input_scroll           (GLFWwindow* win, double xoffset, double yoffset)
{
    CIScrollEvent m;
    m.deltaX = xoffset;
    m.deltaY = yoffset;

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
static void glfw_input_kbdKey           (GLFWwindow* win, int key, int scancode, int action, int mods)
{
    CIKeyEvent m;
    m.type = (action==GLFW_PRESS) ? CIKeyEvent::Press :(action==GLFW_RELEASE) ? CIKeyEvent::Release : CIKeyEvent::Repeated;

    m.scanCode = scancode;
    m.keyCode = key;
    m.modifier = mods;

    _inputEventHandle(CIEvent::Keyboard, win, &m, sizeof(m));
}
static void glfw_input_charwrite        (GLFWwindow *win, unsigned int character)
{
    CIWriteEvent m;
    m.character = character;

    _inputEventHandle(CIEvent::Input, win, &m, sizeof(m));
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
    cDebug(1,"GLFW error code %i: %s",stat,message);
}

#endif // CGLFWRENDERER_EVENTHANDLERS_H
