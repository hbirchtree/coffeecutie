#include <coffee/sdl2/windowing/x11_window.h>

#if defined(COFFEE_USE_MAEMO_X11)

#include <coffee/core/plat/windowmanager/plat_windowtype.h>

#include <coffee/core/CDebug>

#include <iostream>

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>

#include <EGL/egl.h>

static int x11_handler(Display*, XErrorEvent* ev)
{
    Coffee::cVerbose(8, "X11 error: {0}", ev->error_code);
    return 0;
}

namespace Coffee{
namespace Display{

struct X11_Data
{
    ::Display* display;
    ::Window window;
};

X11Window::X11Window()
{
}

X11Window::~X11Window()
{
}

CDMonitor X11Window::monitor()
{
    return {};
}

bool X11Window::showWindow()
{
    if(m_xData)
    {
        return XMapWindow(m_xData->display, m_xData->window);
    }
    return false;
}

bool X11Window::hideWindow()
{
    return false;
}

bool X11Window::closeWindow()
{
    if(m_xData)
        return XUnmapWindow(m_xData->display, m_xData->window) == 0;
    return false;
}

void X11Window::popErrorMessage(Severity s, cstring title, cstring msg)
{

}

bool X11Window::windowPreInit(const CDProperties &, CString *)
{
    return true;
}

bool X11Window::windowInit(const CDProperties &props, CString *err)
{
    XSetErrorHandler(x11_handler);

    /* Reference: https://wiki.maemo.org/SimpleGL_example */

    m_xData = UqPtr<X11_Data>(new X11_Data());

    m_xData->display = XOpenDisplay(nullptr);

    ::Window rootWindow = DefaultRootWindow(m_xData->display);

    XSetWindowAttributes swa;
    swa.event_mask = ExposureMask | PointerMotionMask | KeyPressMask | KeyReleaseMask;

    m_xData->window = XCreateWindow(m_xData->display, rootWindow,
                                    0, 0, props.size.w, props.size.h, 0,
                                    CopyFromParent, InputOutput,
                                    CopyFromParent, CWEventMask,
                                    &swa);

    XSetWindowAttributes xattr;

    xattr.override_redirect = False;
    XChangeWindowAttributes(m_xData->display, m_xData->window, CWOverrideRedirect, &xattr);

    Atom atom;
    atom = XInternAtom(m_xData->display, "_NET_WM_STATE_FULLSCREEN", True);
    XChangeProperty(m_xData->display, m_xData->window,
                    XInternAtom(m_xData->display, "_NET_WM_STATE", True),
                    XA_ATOM, 32, PropModeReplace,
                    (u8*)&atom, 1);

    int one = 1;
    XChangeProperty(m_xData->display, m_xData->window,
                    XInternAtom(m_xData->display, "_HILDON_NON_COMPOSITED_WINDOW", True),
                    XA_INTEGER, 32, PropModeReplace,
                    (u8*)&one, 1);

    XWMHints hints;
    hints.input = True;
    hints.flags = InputHint;
    XSetWMHints(m_xData->display, m_xData->window, &hints);

    showWindow();
    setWindowTitle(props.title);

    Atom wm_state = XInternAtom(m_xData->display, "_NET_WM_STATE", False);
    Atom fullscreen = XInternAtom(m_xData->display, "_NET_WM_STATE_FULLSCREEN", False);

    XEvent xev;
    memset(&xev, 0, sizeof(xev));

    xev.type = ClientMessage;
    xev.xclient.window = m_xData->window;
    xev.xclient.message_type = wm_state;
    xev.xclient.format = 32;
    xev.xclient.data.l[0] = 1;
    xev.xclient.data.l[1] = fullscreen;

    XSendEvent(m_xData->display,
               rootWindow,
               False,
               SubstructureNotifyMask,
               &xev);

    return true;
}

bool X11Window::windowPostInit(const CDProperties &, CString *)
{
    return true;
}

void X11Window::windowTerminate()
{
    if(!m_xData)
        return;

    XUnmapWindow(m_xData->display, m_xData->window);
    XDestroyWindow(m_xData->display, m_xData->window);
    XCloseDisplay(m_xData->display);
}

CDWindow *X11Window::window()
{
    if(!m_xData)
        return nullptr;

    CDWindow* win = new CDWindow();
    win->wininfo.x11.display = m_xData->display;
    win->wininfo.x11.window = m_xData->window;
    return win;
}

uint32 X11Window::windowState() const
{
    return 0;
}

void X11Window::setWindowState(const CDProperties::State &s)
{

}

CSize X11Window::windowSize() const
{
    ::Window win;
    i32 x, y;
    u32 w, h;
    u32 bw, d;
    XGetGeometry(m_xData->display, m_xData->window, &win,
                 &x, &y, &w, &h, &bw, &d);
    return {C_CAST<i32>(w), C_CAST<i32>(h)};
}

void X11Window::setWindowSize(const CSize &s)
{

}

CPoint X11Window::windowPosition() const
{
    return {};
}

void X11Window::setWindowPosition(const CPoint &p)
{

}

CString X11Window::windowTitle() const
{
    if(!m_xData)
        return {};

    char* wm_name = nullptr;
    if(XFetchName(m_xData->display, m_xData->window, &wm_name) != True || !wm_name)
        return {};
    CString name = wm_name;
    XFree(wm_name);
    return name;
}

void X11Window::setWindowTitle(const CString &tl)
{
    if(m_xData)
        XStoreName(m_xData->display, m_xData->window, tl.c_str());
}

bool X11Window::setWindowIcon(CBitmap &icon)
{
    return false;
}

bool X11Window::screensaverMode()
{
    return false;
}

void X11Window::setScreensaverMode(bool m)
{
    C_UNUSED(m);
}

CString X11Window::windowLibrary() const
{
    return "Xlib";
}

}
}
#endif
