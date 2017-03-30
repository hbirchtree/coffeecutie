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
    CDMonitor mon = {};

    {
        ::Window win;
        i32 x, y;
        u32 w, h;
        u32 bw, d;
        XGetGeometry(m_xData->display, DefaultRootWindow(m_xData->display),
                     &win, &x, &y, &w, &h, &bw, &d);
        mon.resolution = {C_CAST<i32>(w), C_CAST<i32>(h)};
    }

    CSize w_size = windowSize();
    CPoint w_pos = windowPosition();

    mon.screenArea = {w_pos.x, w_pos.y, w_size.w, w_size.h};

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
    {
        bool stat = XUnmapWindow(m_xData->display, m_xData->window) == 0;
        EventApplication* eventapp = C_DCAST<EventApplication>(this);
        if(eventapp) eventapp->applyCloseFlag();
    }
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

    if(!m_xData->display)
    {
        *err = "Failed to open X11 display connection";
        return false;
    }

    ::Window rootWindow = DefaultRootWindow(m_xData->display);

    XSetWindowAttributes swa;
    swa.event_mask =
            ExposureMask
            | PointerMotionMask | ButtonPressMask | ButtonReleaseMask
            | KeyPressMask | KeyReleaseMask
            | FocusChangeMask | VisibilityChangeMask | ResizeRedirectMask
            | ButtonMotionMask
            | EnterWindowMask | LeaveWindowMask
            | ExposureMask
            ;

    m_xData->window = XCreateWindow(m_xData->display, rootWindow,
                                    0, 0, props.size.w, props.size.h, 0,
                                    CopyFromParent, InputOutput,
                                    CopyFromParent, CWEventMask,
                                    &swa);

    if(!m_xData->window)
    {
        *err = "Failed to create X11 window";
        return false;
    }

    Atom delete_window = XInternAtom(m_xData->display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(m_xData->display, m_xData->window, &delete_window, 1);

    XSetWindowAttributes xattr;

    xattr.override_redirect = False;
    XChangeWindowAttributes(m_xData->display, m_xData->window, CWOverrideRedirect, &xattr);

#if defined(COFFEE_X11_HILDON)
    int one = 1;
    XChangeProperty(m_xData->display, m_xData->window,
                    XInternAtom(m_xData->display, "_HILDON_NON_COMPOSITED_WINDOW", True),
                    XA_INTEGER, 32, PropModeReplace,
                    (u8*)&one, 1);
#endif

    XWMHints hints;
    hints.input = True;
    hints.flags = InputHint;
    XSetWMHints(m_xData->display, m_xData->window, &hints);

    showWindow();
    setWindowTitle(props.title);

    setWindowState(props.flags);

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

static constexpr const int _NET_WM_STATE_REMOVE = 0;
static constexpr const int _NET_WM_STATE_ADD = 1;
static constexpr const int _NET_WM_STATE_TOGGLE = 2;

static void X_Apply_State(::Display* xd, ::Window w, cstring msg, int mode, Vector<cstring>const& atoms)
{
    if(atoms.size() > 4)
    {
        cWarning("Cannot set this many atoms on one window, please split it ({0})", atoms.size());
        return;
    }

    ::Window rootWindow = DefaultRootWindow(xd);

    /* Send event to window */
    {
        Vector<Atom> atoms_;
        for(size_t i=0;i<atoms.size();i++)
        {
            atoms_.push_back(XInternAtom(xd, atoms[i], True));
        }
        XChangeProperty(xd, w,
                        XInternAtom(xd, msg, True),
                        XA_ATOM, 32, PropModeReplace,
                        (u8*)atoms_.data(), atoms_.size());
    }

    /* Send event to root window */
    {
        Atom wm_state = XInternAtom(xd, msg, False);

        Vector<Atom> atoms_;
        for(size_t i=0;i<atoms.size();i++)
        {
            atoms_.push_back(XInternAtom(xd, atoms[i], False));
        }

        XEvent xev;
        memset(&xev, 0, sizeof(xev));

        xev.type = ClientMessage;
        xev.xclient.window = w;
        xev.xclient.message_type = wm_state;
        xev.xclient.format = 32;
        xev.xclient.data.l[0] = mode;
        int num = 0;
        for(Atom state : atoms_)
        {
            xev.xclient.data.l[++num] = state;
        }

        XSendEvent(xd,
                   rootWindow,
                   False,
                   SubstructureNotifyMask,
                   &xev);
    }
}

static void X_Set_Decorated(::Display* disp, ::Window target, bool enable)
{
    typedef struct {
        unsigned long   flags;
        unsigned long   functions;
        unsigned long   decorations;
        long            inputMode;
        unsigned long   status;
    } Hints;

    Hints w_hints = {};
    w_hints.flags = 2;
    w_hints.decorations = enable;

    XChangeProperty(disp,target,
                    XInternAtom(disp,"_MOTIF_WM_HINTS",True),
                    XInternAtom(disp,"_MOTIF_WM_HINTS",True),
                    32,PropModeReplace,
                    (unsigned char*)&w_hints,5);
}

void X11Window::setWindowState(const CDProperties::State &s)
{
    if(s & CDProperties::FullScreen)
    {
        X_Apply_State(m_xData->display, m_xData->window,
                      "_NET_WM_STATE", _NET_WM_STATE_ADD,
        {"_NET_WM_STATE_FULLSCREEN"});
    }else if(s & CDProperties::Windowed)
    {
        X_Apply_State(m_xData->display, m_xData->window,
                      "_NET_WM_STATE", _NET_WM_STATE_REMOVE,
        {"_NET_WM_STATE_FULLSCREEN"});
        X_Set_Decorated(m_xData->display, m_xData->window, true);
    }else if(s & CDProperties::WindowedFullScreen)
    {
        X_Apply_State(m_xData->display, m_xData->window,
                      "_NET_WM_STATE", _NET_WM_STATE_REMOVE,
        {"_NET_WM_STATE_FULLSCREEN"});
        X_Set_Decorated(m_xData->display, m_xData->window, false);
    }

    if(s & CDProperties::Maximized)
    {
        X_Apply_State(m_xData->display, m_xData->window,
                      "_NET_WM_STATE", _NET_WM_STATE_ADD,
        {"_NET_WM_STATE_MAXIMIZED_VERT", "_NET_WM_STATE_MAXIMIZED_HORZ"});
    }else if(s & CDProperties::Normal)
    {
        X_Apply_State(m_xData->display, m_xData->window,
                      "_NET_WM_STATE", _NET_WM_STATE_REMOVE,
        {"_NET_WM_STATE_MAXIMIZED_VERT", "_NET_WM_STATE_MAXIMIZED_HORZ"});
    }else if(s & CDProperties::Minimized)
    {
        hideWindow();
    }
}

CSize X11Window::windowSize() const
{
    ::Window win;
    i32 x, y;
    u32 w, h;
    u32 bw, d;
    XGetGeometry(m_xData->display, m_xData->window,
                 &win, &x, &y, &w, &h, &bw, &d);
    return {C_CAST<i32>(w), C_CAST<i32>(h)};
}

void X11Window::setWindowSize(const CSize &s)
{
    auto pos = windowPosition();
    XMoveResizeWindow(m_xData->display, m_xData->window, pos.x, pos.y,
                      C_CAST<u32>(s.w), C_CAST<u32>(s.h));
}

CPoint X11Window::windowPosition() const
{
    ::Window win;
    i32 x, y;
    u32 w, h;
    u32 bw, d;
    XGetGeometry(m_xData->display, m_xData->window, &win,
                 &x, &y, &w, &h, &bw, &d);
    return {C_CAST<i32>(x), C_CAST<i32>(y)};
}

void X11Window::setWindowPosition(const CPoint &p)
{
    XMoveWindow(m_xData->display, m_xData->window, p.x, p.y);
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

void X11Window::processX11Events(InputApplication *eh)
{
    XEvent xev;
    CIEvent base_i;
    CDEvent base_d;
    while(XPending(m_xData->display))
    {
        XNextEvent(m_xData->display, &xev);

        switch(xev.type)
        {
        case KeyPress:
        case KeyRelease:
        {
            base_i.type = CIEvent::Keyboard;
            CIKeyEvent k;
            XKeyEvent& xk = xev.xkey;
            if(xk.keycode == XK_VoidSymbol)
                continue;
            if((xk.keycode > XK_space && xk.keycode < XK_asciitilde)
                    || (xk.keycode > XK_nobreakspace && xk.keycode < XK_ydiaeresis))
                /* ASCII letters */
                k.key = xk.keycode;
            else if(xk.keycode > XK_F1 && xk.keycode < XK_F12)
                k.key = xk.keycode - XK_F1 + CK_F1;

            if(xk.type == KeyPress)
                k.mod |= CIKeyEvent::PressedModifier;
            eh->eventHandle(base_i, &k);
            continue;
        }
//        case EnterNotify:
//        case LeaveNotify:
        case FocusIn:
        case FocusOut:
        {
            base_d.type = CDEvent::Focus;
            CDFocusEvent f;
            /* Right now, we smash these two together and hope for the best */
            f.mod = (xev.type == FocusIn || xev.type == EnterNotify)
                    ? CDFocusEvent::Enter : CDFocusEvent::Leave;
            eh->eventHandle(base_d, &f);
            break;
        }
        case ButtonRelease:
        case ButtonPress:
        {
            XButtonEvent& xb = xev.xbutton;
            base_i.type = CIEvent::MouseButton;
            CIMouseButtonEvent b;

            /* We remap this from:
             * Button1 = 1, Button2 = 2, Button3 = 3
             * To:
             * Button1 = 1, Button2 = 2, Button3 = 4
             */
            b.btn = C_CAST<CIMouseButtonEvent::MouseButton>(1 << xb.button);
            b.mod = (xev.type == ButtonPress)
                    ? CIMouseButtonEvent::Pressed
                    : CIMouseButtonEvent::NoneModifier;
            eh->eventHandle(base_i, &b);
            break;
        }
        case MotionNotify:
        {
            XMotionEvent& xm = xev.xmotion;
            base_i.type = CIEvent::MouseMove;
            CIMouseMoveEvent m;
            m.btn = CIMouseButtonEvent::NoneBtn;
            m.origin = CPoint(xm.x, xm.y).convert<scalar>();
            eh->eventHandle(base_i, &m);
            break;
        }
        case ResizeRequest:
        {
            XResizeRequestEvent& xr = xev.xresizerequest;
            CDResizeEvent r;
            setWindowSize({xr.width, xr.height});
            base_d.type = CDEvent::Resize;
            r.w = xr.width;
            r.h = xr.height;
            eh->eventHandle(base_d, &r);
            break;
        }
        case ClientMessage:
        {
            XClientMessageEvent& xcm = xev.xclient;
            base_i.type = CIEvent::QuitSign;
            eh->eventHandle(base_i, nullptr);
            break;
        }
        default:
            cVerbose(5, "X11:Unhandled event: {0}", xev.type);
            continue;
        }
    }
}

}
}
#endif
