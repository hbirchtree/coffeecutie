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

inline static void _inputEventHandle(CIEvent::EventType type, GLFWwindow* win,
                              const void* idata, uint32_t isize)
{
    CGLFWRenderer* ptr = getPtr(win);

    void* data = nullptr;
    uint32_t size = 0;

    _createEvent(type,&data,&size,idata,isize);

    qDebug() << size << data << ptr;

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

}

static void glfw_input_scroll           (GLFWwindow* win, double xoffset, double yoffset)
{

}
static void glfw_input_dropevent        (GLFWwindow* win, int numfiles, const char** paths)
{

}
static void glfw_input_kbdKey           (GLFWwindow* win, int key, int scancode, int action, int mods)
{

}
static void glfw_input_charwrite        (GLFWwindow *window, unsigned int character)
{

}

//These generate window events
static void glfw_win_resize             (GLFWwindow* win, int width, int height)
{

}
static void glfw_win_fbresize           (GLFWwindow* win, int width, int height)
{

}
static void glfw_win_focus              (GLFWwindow* win, int val)
{

}
static void glfw_win_pos                (GLFWwindow* win, int x, int y)
{

}
static void glfw_win_refresh            (GLFWwindow* win)
{

}
static void glfw_win_close              (GLFWwindow* win)
{

}
static void glfw_win_state              (GLFWwindow* win, int val)
{

}

//This generates errors. All the time.
static void glfw_error_function         (int stat, const char* message)
{

}

#endif // CGLFWRENDERER_EVENTHANDLERS_H
