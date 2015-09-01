#include "cglfwrenderer.h"

#include "cglfwrenderer_eventhandlers.h"
#include <stdio.h>

namespace Coffee {
namespace CDisplay {

CGLFWRenderer::CGLFWRenderer(CObject *parent) : CDRendererBase(parent)
{
}

CGLFWRenderer::~CGLFWRenderer()
{
    cleanup();
}

CString CGLFWRenderer::windowTitle() const
{
    return CString(); //Not available with GLFW
}

void CGLFWRenderer::setWindowTitle(const CString &title)
{
    glfwSetWindowTitle(m_window,title.c_str());
}

CDMonitor CGLFWRenderer::monitor()
{
    GLFWmonitor* gm = glfwGetWindowMonitor(m_window);
    CDMonitor cm;

    if(!gm){
        //Not fullscreen
        cm.screenArea.h = 0;
        cm.screenArea.w = 0;
    }else{
        cm.name = glfwGetMonitorName(gm);
        glfwGetMonitorPhysicalSize(gm,&(cm.phySize.w),&(cm.phySize.h));
        glfwGetMonitorPos(gm,&(cm.screenArea.x),&(cm.screenArea.y));

        const GLFWvidmode* vmode = glfwGetVideoMode(gm);
        if(vmode){
            cm.colorBits.bits_blue = vmode->blueBits;
            cm.colorBits.bits_red = vmode->redBits;
            cm.colorBits.bits_green = vmode->greenBits;
            cm.refresh = vmode->refreshRate;

            cm.screenArea.w = vmode->width;
            cm.screenArea.h = vmode->height;
        }
    }
    return cm;
}

CDWindow CGLFWRenderer::window()
{
    CDWindow cw;
    cw.handle = CGLFWNativeFuncs::glfwGetNativeWindowHandle(m_window);
    glfwGetWindowSize(m_window,&(cw.screenArea.w),&(cw.screenArea.h));
    glfwGetWindowPos(m_window,&(cw.screenArea.x),&(cw.screenArea.y));

    return cw;
}

CDContextBits CGLFWRenderer::context()
{
    CDContextBits bits;

    bits.red = glfwGetWindowAttrib(m_window,GLFW_RED_BITS);
    bits.green = glfwGetWindowAttrib(m_window,GLFW_GREEN_BITS);
    bits.blue = glfwGetWindowAttrib(m_window,GLFW_BLUE_BITS);
    bits.alpha = glfwGetWindowAttrib(m_window,GLFW_ALPHA_BITS);

    bits.depth = glfwGetWindowAttrib(m_window,GLFW_DEPTH_BITS);
    bits.stencil = glfwGetWindowAttrib(m_window,GLFW_STENCIL_BITS);

    return bits;
}

uint32_t CGLFWRenderer::windowState() const
{
    uint32_t flags = 0;

    if(glfwGetWindowAttrib(m_window,GLFW_FOCUSED))
        flags |= Focused;
    if(glfwGetWindowAttrib(m_window,GLFW_VISIBLE))
        flags |= Visible;

    if(glfwGetWindowAttrib(m_window,GLFW_ICONIFIED))
        flags |= Minimized;
    else
        flags |= Maximized;

    if(glfwGetWindowAttrib(m_window,GLFW_DECORATED))
        flags |= Decorated;
    if(glfwGetWindowAttrib(m_window,GLFW_FLOATING))
        flags |= Floating;

    if(glfwGetWindowMonitor(m_window))
        flags |= FullScreen;
    else
        flags |= Windowed;

    return flags;
}

void CGLFWRenderer::setWindowState(uint32_t newstate)
{
    if(newstate&Minimized){
        glfwIconifyWindow(m_window);
    }else if(newstate&Maximized){
        glfwRestoreWindow(m_window);
    }
}

bool CGLFWRenderer::showWindow()
{
    glfwShowWindow(m_window);
    return true;
}

bool CGLFWRenderer::hideWindow()
{
    glfwHideWindow(m_window);
    return true;
}

bool CGLFWRenderer::closeWindow()
{
    glfwSetWindowShouldClose(m_window,1);
    return true;
}

int CGLFWRenderer::swapInterval()
{
    return -1; //Not available with GLFW
}

void CGLFWRenderer::setSwapInterval(int interval)
{
    glfwSwapInterval(interval);
}

CSize CGLFWRenderer::framebufferSize() const
{
    CSize f;
    glfwGetFramebufferSize(m_window,&(f.w),&(f.h));
    return f;
}

CSize CGLFWRenderer::windowSize() const
{
    CSize w;
    glfwGetWindowSize(m_window,&(w.w),&(w.h));
    return w;
}

void CGLFWRenderer::setWindowSize(const CSize &size)
{
    glfwSetWindowSize(m_window,size.w,size.h);
}

bool CGLFWRenderer::isMouseGrabbed() const
{
    return glfwGetInputMode(m_window,GLFW_CURSOR) == GLFW_CURSOR_DISABLED;
}

void CGLFWRenderer::setMouseGrabbing(bool grab)
{
    int val;

    if(grab){
        val = GLFW_CURSOR_DISABLED;
    }else{
        val = GLFW_CURSOR_NORMAL;
    }

    glfwSetInputMode(m_window,GLFW_CURSOR,val);
}

bool CGLFWRenderer::closeFlag()
{
    return glfwWindowShouldClose(m_window);
}

void CGLFWRenderer::swapBuffers()
{
    glfwSwapBuffers(m_window);
}

void CGLFWRenderer::pollEvents()
{
    glfwPollEvents();
    updateJoysticks();
}

void CGLFWRenderer::init(WindowState startState, CSize startSize, int monitorIndex)
{
    if(!glfwInit())
        cDebug(2,"Failed to initialize GLFW");
    cMsg("GLFW","Initialized");

    glfwDefaultWindowHints();

    glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT,true);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
    glfwWindowHint(GLFW_VISIBLE,false);
    glfwWindowHint(GLFW_RESIZABLE,true);

    switch(startState){
    case FullScreen:
    case WindowedFullScreen:{
        int count = 0;
        GLFWmonitor** monitors = glfwGetMonitors(&count);
        if(monitorIndex>=count)
            return;

        GLFWmonitor* mon = monitors[monitorIndex];
        if(startState==WindowedFullScreen){
            const GLFWvidmode* current = glfwGetVideoMode(mon);

            glfwWindowHint(GLFW_RED_BITS,current->redBits);
            glfwWindowHint(GLFW_GREEN_BITS,current->greenBits);
            glfwWindowHint(GLFW_BLUE_BITS,current->blueBits);
            glfwWindowHint(GLFW_REFRESH_RATE,current->refreshRate);

            startSize.w = current->width;
            startSize.h = current->height;
        }
        m_window = glfwCreateWindow(startSize.w,startSize.h,"",mon,NULL);
        break;
    }
    case Windowed:
        m_window = glfwCreateWindow(startSize.w,startSize.h,"",NULL,NULL);
        break;
    default:
        return;
    }

    glfwMakeContextCurrent(m_window);
    glfwSetWindowUserPointer(m_window,this);

    //Input callbacks
    glfwSetMouseButtonCallback(m_window,glfw_input_mouseBtn);
    glfwSetKeyCallback(m_window,glfw_input_kbdKey);
    glfwSetCursorPosCallback(m_window,glfw_input_mouseMove);
    glfwSetCursorEnterCallback(m_window,glfw_input_mouseenter);
    glfwSetDropCallback(m_window,glfw_input_dropevent);
    glfwSetScrollCallback(m_window,glfw_input_scroll);
    glfwSetCharCallback(m_window,glfw_input_charwrite);

    //Window callbacks
    glfwSetWindowSizeCallback       (m_window,glfw_win_resize);
    glfwSetWindowCloseCallback      (m_window,glfw_win_close);
    glfwSetWindowFocusCallback      (m_window,glfw_win_focus);
    glfwSetWindowIconifyCallback    (m_window,glfw_win_state);
    glfwSetWindowPosCallback        (m_window,glfw_win_pos);
    glfwSetFramebufferSizeCallback  (m_window,glfw_win_fbresize);

    glfwSetErrorCallback            (glfw_error_function);

    {
        int maj,min,rev;
        glfwGetVersion(&maj,&min,&rev);
        char* cstring = new char[100];
        sprintf(cstring,"GLFW %i.%i rev. %i",maj,min,rev);
        m_contextString = cstring;
        delete[] cstring;
    }

    //GLBINDING BEGINS
    glbinding::Binding::initialize(true);

    m_rendererString = glbinding::ContextInfo::renderer();
    m_vendorString = glbinding::ContextInfo::vendor();
    m_versionString = glbinding::ContextInfo::version().toString();

    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(glbindingCallbackDirect,this);

    //GLBINDING ENDS
}

void CGLFWRenderer::cleanup()
{
    glfwDestroyWindow(m_window);
    glfwTerminate();
}

void CGLFWRenderer::glbindingCallbackDirect(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *msg, const void *userPtr)
{
    const CGLFWRenderer* renderer = static_cast<const CGLFWRenderer*>(userPtr);
    renderer->glbindingCallbackInternal(source,type,id,severity,length,msg);
}

void CGLFWRenderer::updateJoysticks()
{
}

} // namespace CDisplay
} // namespace Coffee

