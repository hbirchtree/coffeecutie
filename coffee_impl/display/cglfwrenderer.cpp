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
            cm.colorBits.red = vmode->redBits;
            cm.colorBits.green = vmode->greenBits;
            cm.colorBits.blue = vmode->blueBits;
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
        flags |= CDWindowProperties::Focused;
    if(glfwGetWindowAttrib(m_ctxt->window,GLFW_VISIBLE))
        flags |= CDWindowProperties::Visible;

    if(glfwGetWindowAttrib(m_ctxt->window,GLFW_ICONIFIED))
        flags |= CDWindowProperties::Minimized;
    else
        flags |= CDWindowProperties::Maximized;

    if(glfwGetWindowAttrib(m_ctxt->window,GLFW_DECORATED))
        flags |= CDWindowProperties::Decorated;
    if(glfwGetWindowAttrib(m_ctxt->window,GLFW_FLOATING))
        flags |= CDWindowProperties::Floating;

    if(glfwGetWindowMonitor(m_ctxt->window))
        flags |= CDWindowProperties::FullScreen;
    else
        flags |= CDWindowProperties::Windowed;

    return flags;
}

void CGLFWRenderer::setWindowState(uint32_t newstate)
{
    if(newstate&CDWindowProperties::Minimized){
        glfwIconifyWindow(m_ctxt->window);
    }else if(newstate&CDWindowProperties::Maximized){
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

CPoint CGLFWRenderer::windowPosition() const
{
    CPoint pos;
    glfwGetWindowPos(m_ctxt->window,&pos.x,&pos.y);
    return pos;
}

void CGLFWRenderer::setWindowPosition(CPoint pos)
{
    glfwSetWindowPos(m_ctxt->window,pos.x,pos.y);
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

void CGLFWRenderer::init(const CDWindowProperties &props)
{
    m_properties = props;
    try{
        _glfw_init(props);
    }catch(CStdFault err){
        m_initMutex.unlock();
        throw err;
    }
}

void CGLFWRenderer::_glfw_init(const CDWindowProperties& props)
{
    m_initMutex.lock();
    m_contextThread = std::this_thread::get_id();

    if(!glfwInit())
        cWarning("Failed to initialize GLFW");
    cMsg("GLFW","Initialized");

    bindingPreInit();

    glfwDefaultWindowHints();

    //Core profile
    if(props.contextProperties.flags&CGLContextProperties::GLCoreProfile)
        glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
    //Debugging
    if(props.contextProperties.flags&CGLContextProperties::GLDebug)
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT,true);
    //OpenGL version
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,
                   props.contextProperties.version.major);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,
                   props.contextProperties.version.minor);
    //Window flags
    glfwWindowHint(GLFW_VISIBLE,
                   (bool)(props.flags&CDWindowProperties::Visible));
    glfwWindowHint(GLFW_RESIZABLE,
                   (bool)(props.flags&CDWindowProperties::Resizable));
    glfwWindowHint(GLFW_DECORATED,
                   (bool)(props.flags&CDWindowProperties::Decorated));
    glfwWindowHint(GLFW_FLOATING,
                   (bool)(props.flags&CDWindowProperties::Floating));

    //Allocate our context object, is deleted when context is gone
    m_ctxt = reinterpret_cast<CGLFWContext*>(malloc(sizeof(CGLFWContext)));

    glfwWindowHint(GLFW_RED_BITS,   props.contextProperties.bits.red);
    glfwWindowHint(GLFW_GREEN_BITS, props.contextProperties.bits.green);
    glfwWindowHint(GLFW_BLUE_BITS,  props.contextProperties.bits.blue);
    glfwWindowHint(GLFW_ALPHA_BITS, props.contextProperties.bits.alpha);
    glfwWindowHint(GLFW_STENCIL_BITS,props.contextProperties.bits.stencil);
    glfwWindowHint(GLFW_DEPTH_BITS, props.contextProperties.bits.depth);

    switch(props.flags&(CDWindowProperties::FullScreen
                  |CDWindowProperties::WindowedFullScreen
                  |CDWindowProperties::Windowed))
    {
    case CDWindowProperties::FullScreen:
    case CDWindowProperties::WindowedFullScreen:{
        int count = 0;
        GLFWmonitor** monitors = glfwGetMonitors(&count);
        if(props.monitor>=count)
            return;

        GLFWmonitor* mon = monitors[props.monitor];
        if(props.flags&CDWindowProperties::WindowedFullScreen){
            const GLFWvidmode* current = glfwGetVideoMode(mon);

            glfwWindowHint(GLFW_RED_BITS,   current->redBits);
            glfwWindowHint(GLFW_GREEN_BITS, current->greenBits);
            glfwWindowHint(GLFW_BLUE_BITS,  current->blueBits);
            glfwWindowHint(GLFW_REFRESH_RATE,current->refreshRate);

            m_properties.size.w = current->width;
            m_properties.size.h = current->height;
        }
        m_ctxt->window = glfwCreateWindow(props.size.w,props.size.h,
                                          "",
                                          mon,NULL);
        break;
    }
    case CDWindowProperties::Windowed:
        m_ctxt->window = glfwCreateWindow(props.size.w,props.size.h,
                                          "",
                                          NULL,NULL);
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

    if(props.contextProperties.flags&CGLContextProperties::GLVSync)
        glfwSwapInterval(1);
    else
        glfwSwapInterval(0);

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

//    CGLFWNativeFuncs::glfwSetDecorations(m_ctxt->window,false);

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


