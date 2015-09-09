#include "cglfwrenderer.h"

#include "cglfwnativefuncs.h"
#include "cglfwrenderer_eventhandlers.h"

namespace Coffee {
namespace CDisplay {

CGLFWRenderer::CGLFWRenderer(CObject *parent) : CDQueueRendererBase(parent)
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
    glfwSetWindowTitle(m_ctxt->window,title.c_str());
}

CDMonitor CGLFWRenderer::monitor()
{
    GLFWmonitor* gm = glfwGetWindowMonitor(m_ctxt->window);
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
    cw.handle = CGLFWNativeFuncs::glfwGetNativeWindowHandle(m_ctxt->window);
    glfwGetWindowSize(m_ctxt->window,&(cw.screenArea.w),&(cw.screenArea.h));
    glfwGetWindowPos(m_ctxt->window,&(cw.screenArea.x),&(cw.screenArea.y));

    return cw;
}

CDContextBits CGLFWRenderer::context()
{
    CDContextBits bits;

    bits.red = glfwGetWindowAttrib(m_ctxt->window,GLFW_RED_BITS);
    bits.green = glfwGetWindowAttrib(m_ctxt->window,GLFW_GREEN_BITS);
    bits.blue = glfwGetWindowAttrib(m_ctxt->window,GLFW_BLUE_BITS);
    bits.alpha = glfwGetWindowAttrib(m_ctxt->window,GLFW_ALPHA_BITS);

    bits.depth = glfwGetWindowAttrib(m_ctxt->window,GLFW_DEPTH_BITS);
    bits.stencil = glfwGetWindowAttrib(m_ctxt->window,GLFW_STENCIL_BITS);

    return bits;
}

uint32_t CGLFWRenderer::windowState() const
{
    uint32_t flags = 0;

    if(glfwGetWindowAttrib(m_ctxt->window,GLFW_FOCUSED))
        flags |= Focused;
    if(glfwGetWindowAttrib(m_ctxt->window,GLFW_VISIBLE))
        flags |= Visible;

    if(glfwGetWindowAttrib(m_ctxt->window,GLFW_ICONIFIED))
        flags |= Minimized;
    else
        flags |= Maximized;

    if(glfwGetWindowAttrib(m_ctxt->window,GLFW_DECORATED))
        flags |= Decorated;
    if(glfwGetWindowAttrib(m_ctxt->window,GLFW_FLOATING))
        flags |= Floating;

    if(glfwGetWindowMonitor(m_ctxt->window))
        flags |= FullScreen;
    else
        flags |= Windowed;

    return flags;
}

void CGLFWRenderer::setWindowState(uint32_t newstate)
{
    if(newstate&Minimized){
        glfwIconifyWindow(m_ctxt->window);
    }else if(newstate&Maximized){
        glfwRestoreWindow(m_ctxt->window);
    }
}

bool CGLFWRenderer::showWindow()
{
    glfwShowWindow(m_ctxt->window);
    return true;
}

bool CGLFWRenderer::hideWindow()
{
    glfwHideWindow(m_ctxt->window);
    return true;
}

bool CGLFWRenderer::closeWindow()
{
    glfwSetWindowShouldClose(m_ctxt->window,1);
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
    glfwGetFramebufferSize(m_ctxt->window,&(f.w),&(f.h));
    return f;
}

CSize CGLFWRenderer::windowSize() const
{
    CSize w;
    glfwGetWindowSize(m_ctxt->window,&(w.w),&(w.h));
    return w;
}

void CGLFWRenderer::setWindowSize(const CSize &size)
{
    glfwSetWindowSize(m_ctxt->window,size.w,size.h);
}

double CGLFWRenderer::contextTime() const
{
    return glfwGetTime();
}

bool CGLFWRenderer::isMouseGrabbed() const
{
    return glfwGetInputMode(m_ctxt->window,GLFW_CURSOR) == GLFW_CURSOR_DISABLED;
}

void CGLFWRenderer::setMouseGrabbing(bool grab)
{
    int val;

    if(grab){
        val = GLFW_CURSOR_DISABLED;
    }else{
        val = GLFW_CURSOR_NORMAL;
    }

    glfwSetInputMode(m_ctxt->window,GLFW_CURSOR,val);
}

bool CGLFWRenderer::closeFlag()
{
    return glfwWindowShouldClose(m_ctxt->window);
}

bool CGLFWRenderer::requestGLExtension(cstring extension)
{
    return glfwExtensionSupported(extension);
}

void CGLFWRenderer::swapBuffers()
{
    glfwSwapBuffers(m_ctxt->window);
}

void CGLFWRenderer::pollEvents()
{
    glfwPollEvents();
    updateJoysticks();
}

void CGLFWRenderer::init(WindowState startState, CSize startSize, int monitorIndex)
{
    m_initMutex.lock();
    m_contextThread = std::this_thread::get_id();

    if(!glfwInit())
        cWarning("Failed to initialize GLFW");
    cMsg("GLFW","Initialized");

    bindingPreInit();

    glfwDefaultWindowHints();

    glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT,true);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,1);
    glfwWindowHint(GLFW_VISIBLE,false);
    glfwWindowHint(GLFW_RESIZABLE,true);

    //Allocate our context object, is deleted when context is gone
    m_ctxt = reinterpret_cast<CGLFWContext*>(malloc(sizeof(CGLFWContext)));

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
        m_ctxt->window = glfwCreateWindow(startSize.w,startSize.h,"",mon,NULL);
        break;
    }
    case Windowed:
        m_ctxt->window = glfwCreateWindow(startSize.w,startSize.h,"",NULL,NULL);
        break;
    default:
        return;
    }

    if(!m_ctxt->window){
        cFatal("GLFW: No window could be created!");
    }

    m_ctxt->makeCurrent();
    glfwSetWindowUserPointer(m_ctxt->window,this);
    cMsg("GLFW","User-pointer set");

    //Input callbacks
    glfwSetMouseButtonCallback      (m_ctxt->window,glfw_input_mouseBtn);
    glfwSetKeyCallback              (m_ctxt->window,glfw_input_kbdKey);
    glfwSetCursorPosCallback        (m_ctxt->window,glfw_input_mouseMove);
    glfwSetCursorEnterCallback      (m_ctxt->window,glfw_input_mouseenter);
    glfwSetDropCallback             (m_ctxt->window,glfw_input_dropevent);
    glfwSetScrollCallback           (m_ctxt->window,glfw_input_scroll);
    glfwSetCharCallback             (m_ctxt->window,glfw_input_charwrite);

    //Window callbacks
    glfwSetWindowSizeCallback       (m_ctxt->window,glfw_win_resize);
    glfwSetWindowCloseCallback      (m_ctxt->window,glfw_win_close);
    glfwSetWindowFocusCallback      (m_ctxt->window,glfw_win_focus);
    glfwSetWindowIconifyCallback    (m_ctxt->window,glfw_win_state);
    glfwSetWindowPosCallback        (m_ctxt->window,glfw_win_pos);
    glfwSetFramebufferSizeCallback  (m_ctxt->window,glfw_win_fbresize);

    glfwSetErrorCallback            (glfw_error_function);

    cMsg("GLFW","Callbacks were set");

    {
        int maj,min,rev;
        glfwGetVersion(&maj,&min,&rev);
        char* cstring = new char[100];
        sprintf(cstring,"GLFW %i.%i rev. %i",maj,min,rev);
        m_contextString = cstring;
        delete[] cstring;
    }

    bindingPostInit();

    m_initMutex.unlock();
}

void CGLFWRenderer::cleanup()
{
    m_initMutex.lock();
    if(!m_ctxt){
        cMsg("GLFW","Context already terminated");
        return;
    }

    if(m_contextThread!=std::this_thread::get_id())
        cFatal("GLFW context cannot be terminated on this thread!");
    if(m_ctxt->window){
        glfwDestroyWindow(m_ctxt->window);
        m_ctxt->window = nullptr;
    }
    glfwTerminate();
    cMsg("GLFW","Terminated");
    free(m_ctxt);
    m_ctxt = nullptr;
    m_initMutex.unlock();
}

void CGLFWRenderer::updateJoysticks()
{
}

} // namespace CDisplay
} // namespace Coffee

