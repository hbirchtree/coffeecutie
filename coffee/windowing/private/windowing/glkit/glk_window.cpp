#include <coffee/windowing/windowing/glkit/glk_window.h>

#if defined(COFFEE_USE_APPLE_GLKIT)

#include <coffee/core/CDebug>

#include <coffee/core/base/renderer/eventapplication_wrapper.h>
#include <CEAGL/eagl.h>

#include <coffee/core/plat/windowmanager/plat_windowtype.h>

#include <CEAGL/eagl.h>

namespace Coffee{
namespace Display{

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
    EGLDisplay* display = nullptr;
    CoffeeForeignSignalHandleNA(CoffeeForeing_GetEGLDisplay,
                                &display, nullptr, nullptr);
    
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

}
}

#endif
