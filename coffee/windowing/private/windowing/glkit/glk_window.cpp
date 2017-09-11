#include <coffee/windowing/windowing/glkit/glk_window.h>

#if defined(COFFEE_USE_APPLE_GLKIT)

#include <coffee/core/CDebug>

#include <coffee/core/base/renderer/eventapplication_wrapper.h>
#include <CEAGL/eagl.h>

#include <coffee/core/plat/windowmanager/plat_windowtype.h>

#include <CEAGL/eagl.h>

namespace Coffee{
namespace Display{

struct EGL_Data
{
    EGLDisplay display;
    EGLSurface surface;
    EGLConfig config;
    EGLContext context;
};

static void Msg_Complete_Handle(void* ptr)
{
    cDebug("We're back! {0}", StrUtil::pointerify(ptr));
}

GLKWindow::GLKWindow()
{
}

GLKWindow::~GLKWindow()
{
}

CDMonitor GLKWindow::monitor()
{
    EGLDisplay display = nullptr;
    
    EGLRenderer* egl_side = C_DCAST<EGLRenderer>(this);
    
    if(egl_side)
        display = egl_side->m_eglData->display;
    
#define EGL_GET_PROP(var, prop) \
        eglGetConfigAttrib(display, nullptr, prop, &val); \
        var = val;
    
    CDMonitor mon;
    
    EGLint val = 0;
    EGL_GET_PROP(mon.colorBits.red, EGL_RED_SIZE);
    EGL_GET_PROP(mon.colorBits.green, EGL_GREEN_SIZE);
    EGL_GET_PROP(mon.colorBits.blue, EGL_BLUE_SIZE);
    
    EGL_GET_PROP(mon.colorBits.extra, EGL_ALPHA_SIZE);
    
    mon.resolution = this->windowSize();

    return mon;
}

bool GLKWindow::showWindow()
{
    return false;
}

bool GLKWindow::hideWindow()
{
    return false;
}

bool GLKWindow::closeWindow()
{
    return false;
}

void GLKWindow::popErrorMessage(Severity s, cstring t, cstring m)
{
    CfMessageDisplay disp;
    
    disp.user_ptr = this;
    disp.completion = Msg_Complete_Handle;
    disp.actions = nullptr;
    
    cDebug("Hello from Coffee!");

    CoffeeForeignSignalHandleNA(CoffeeForeign_DisplayMessage,
                                (void*)t, (void*)m, &disp);
}

CDWindow* GLKWindow::window()
{
    return new CDWindow();
}

uint32 GLKWindow::windowState() const
{
    return 0;
}

void GLKWindow::setWindowState(CDProperties::State const&)
{
}

CSize GLKWindow::windowSize() const
{
    int winSize[2] = {};
    
    CoffeeForeignSignalHandleNA(CoffeeForeign_GetWinSize,
                                winSize, nullptr, nullptr);
    
    return {winSize[0], winSize[1]};
}

void GLKWindow::setWindowSize(CSize const& s)
{
}

CPoint GLKWindow::windowPosition() const
{
    return {};
}

void GLKWindow::setWindowPosition(CPoint const&)
{
}

CString GLKWindow::windowTitle() const
{
    return {};
}

void GLKWindow::setWindowTitle(CString const& s)
{
}

bool GLKWindow::setWindowIcon(CBitmap& s)
{
    return false;
}

void GLKWindow::setScreensaverMode(bool)
{
}

bool GLKWindow::screensaverMode()
{
    return false;
}

CString GLKWindow::windowLibrary() const
{
    return "Apple GLKit";
}

bool GLKWindow::windowPreInit(CDProperties const& p, CString*)
{
    return true;
}
bool GLKWindow::windowInit(CDProperties const& p, CString*)
{
    return true;
}
bool GLKWindow::windowPostInit(CDProperties const& p, CString*)
{
    return true;
}
void GLKWindow::windowTerminate()
{
}

void GLKWindow::injectEvent(CIEvent const& e, c_cptr data)
{
    eventHandle(e, data);
}

void GLKWindow::injectEvent(CDEvent const& e, c_cptr data)
{
    eventHandle(e, data);
}

void GLKWindow::eventHandleD(const CDEvent& e, c_cptr d)
{
    eventHandle(e, d);
}
void GLKWindow::eventHandleI(const CIEvent& e, c_cptr d)
{
    eventHandle(e, d);
}
    
void GLKWindow::eventHandle(const CIHapticEvent&, c_cptr)
{
    
}
void GLKWindow::eventHandle(const CDEvent& e, c_cptr d)
{
    for(EventHandlerD const& eh : m_eventhandlers_windw)
    {
        eh.func(eh.user_ptr, e, d);
    }
}
void GLKWindow::eventHandle(const CIEvent& e, c_cptr d)
{
    for(EventHandlerI const& eh : m_eventhandlers_input)
    {
        eh.func(eh.user_ptr, e, d);
    }
}

bool GLKWindow::installEventHandler(EventHandlerD eh)
{
    m_eventhandlers_windw.push_back(eh);
    return true;
}
bool GLKWindow::installEventHandler(EventHandlerI eh)
{
    m_eventhandlers_input.push_back(eh);
    return true;
}

}
}

#endif
