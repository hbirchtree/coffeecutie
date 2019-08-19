#include <coffee/windowing/windowing/glkit/glk_window.h>

#if defined(COFFEE_USE_APPLE_GLKIT)

#include <CEAGL/eagl.h>
#include <coffee/foreign/foreign.h>
#include <platforms/windowmanager.h>

#include <coffee/strings/libc_types.h>

#include <coffee/core/CDebug>

namespace Coffee {
namespace Display {

struct EGL_Data
{
    EGLDisplay display;
    EGLSurface surface;
    EGLConfig  config;
    EGLContext context;
};

static void Msg_Complete_Handle(void* ptr)
{
    cDebug("We're back! {0}", str::print::pointerify(ptr));
}

GLKWindow::GLKWindow()
{
}

GLKWindow::~GLKWindow()
{
}

Monitor GLKWindow::monitor()
{
    EGLDisplay display = nullptr;

    EGLRenderer* egl_side = C_DCAST<EGLRenderer>(this);

    if(egl_side)
        display = egl_side->m_eglData->display;

#define EGL_GET_PROP(var, prop)                       \
    eglGetConfigAttrib(display, nullptr, prop, &val); \
    var = val;

    Monitor mon;

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

    disp.user_ptr   = this;
    disp.completion = Msg_Complete_Handle;
    disp.actions    = nullptr;

    cDebug("Hello from Coffee!");

    CoffeeForeignSignalHandleNA(
        CoffeeForeign_DisplayMessage, (void*)t, (void*)m, &disp);
}

Window* GLKWindow::window()
{
    return new Window();
}

uint32 GLKWindow::windowState() const
{
    return 0;
}

void GLKWindow::setWindowState(Properties::State const&)
{
}

Size GLKWindow::windowSize() const
{
    int winSize[2] = {};

    CoffeeForeignSignalHandleNA(
        CoffeeForeign_GetWinSize, winSize, nullptr, nullptr);

    return size_2d<i32>{winSize[0], winSize[1]}.convert<u32>();
}

void GLKWindow::setWindowSize(Size const& s)
{
}

Point GLKWindow::windowPosition() const
{
    return {};
}

void GLKWindow::setWindowPosition(Point const&)
{
}

CString GLKWindow::windowTitle() const
{
    return {};
}

void GLKWindow::setWindowTitle(CString const& s)
{
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

bool GLKWindow::windowPreInit(Properties const& p, CString*)
{
    return true;
}
bool GLKWindow::windowInit(Properties const& p, CString*)
{
    return true;
}
bool GLKWindow::windowPostInit(Properties const& p, CString*)
{
    return true;
}
void GLKWindow::windowTerminate()
{
}

void GLKWindow::injectEvent(Input::CIEvent const& e, c_cptr data)
{
    eventHandle(e, data);
}

void GLKWindow::injectEvent(Event const& e, c_cptr data)
{
    eventHandle(e, data);
}

void GLKWindow::eventHandleD(const Event& e, c_cptr d)
{
    eventHandle(e, d);
}
void GLKWindow::eventHandleI(const Input::CIEvent& e, c_cptr d)
{
    eventHandle(e, d);
}

void GLKWindow::eventHandle(const Input::CIHapticEvent&, c_cptr)
{
}
void GLKWindow::eventHandle(const Event& e, c_cptr d)
{
    for(EventHandlerD const& eh : m_eventhandlers_windw)
    {
        eh.func(e, d);
    }
}
void GLKWindow::eventHandle(const Input::CIEvent& e, c_cptr d)
{
    for(EventHandlerI const& eh : m_eventhandlers_input)
    {
        eh.func(e, d);
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

} // namespace Display
} // namespace Coffee

#endif
