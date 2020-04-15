#include <coffee/x11/x11_comp.h>

#include <coffee/comp_app/subsystems.h>

#include <X11/X.h>
#include <X11/XKBlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>

#include <GL/glx.h>
#include <GL/glxext.h>

namespace x11 {

using namespace comp_app;

void Windowing::load(entity_container& e, comp_app::app_error& ec)
{
    auto display = XOpenDisplay(nullptr);
    m_display    = display;

    if(!display)
    {
        ec = AppError::NoDisplay;
        return;
    }

    GLint attrs[] = {GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None};

    auto& windowConf = AppLoader::config<comp_app::WindowConfig>(e);
    auto& x11Conf    = AppLoader::config<X11Config>(e);

    ::Window       rootWindow = DefaultRootWindow(display);
    ::XVisualInfo* visualInfo = glXChooseVisual(display, 0, attrs);

    if(!visualInfo)
    {
        ec = AppError::FramebufferMismatch;
        return;
    }
    ::Visual*            visual     = visualInfo->visual;
    XSetWindowAttributes windowAttr = {};

    windowAttr.colormap =
        XCreateColormap(display, rootWindow, visual, AllocNone);

    windowAttr.background_pixmap = None;
    windowAttr.border_pixmap     = None;
    windowAttr.border_pixel      = 0;
    windowAttr.event_mask =
        PointerMotionMask | ButtonPressMask | ButtonReleaseMask | KeyPressMask |
        KeyReleaseMask | FocusChangeMask | VisibilityChangeMask |
        ResizeRedirectMask | ButtonMotionMask | EnterWindowMask |
        LeaveWindowMask | ExposureMask | StructureNotifyMask;

    m_window = C_RCAST<void*>(XCreateWindow(
        display,
        rootWindow,
        windowConf.position.x,
        windowConf.position.y,
        C_FCAST<libc_types::u32>(windowConf.size.w),
        C_FCAST<libc_types::u32>(windowConf.size.h),
        0,
        visualInfo->depth,
        InputOutput,
        visualInfo->visual,
        CWColormap | CWEventMask,
        &windowAttr));

    if(!m_window)
    {
        ec = AppError::NoWindow;
        return;
    }

    auto window = C_RCAST<Window>(m_window);

    XSync(display, 0);

    Atom deleteWindow = XInternAtom(display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(display, window, &deleteWindow, 1);

    XSetWindowAttributes xattr;
    xattr.override_redirect = False;
    XChangeWindowAttributes(display, window, CWOverrideRedirect, &xattr);

    if(x11Conf.hildon)
    {
        int one = 1;
        XChangeProperty(
            display,
            window,
            XInternAtom(display, "_HILDON_NON_COMPOSITED_WINDOW", True),
            XA_INTEGER,
            32,
            PropModeReplace,
            C_RCAST<libc_types::u8*>(&one),
            1);
    }

    XMapWindow(display, window);

    auto& winInfo   = *e.service<PtrNativeWindowInfo>();
    winInfo.display = m_display;
    winInfo.window  = m_window;
}

void Windowing::unload(entity_container&, comp_app::app_error&)
{
    auto display = C_RCAST<Display*>(m_display);
    auto window  = C_RCAST<Window>(m_window);

    XUnmapWindow(display, window);
    XDestroyWindow(display, window);
    XCloseDisplay(display);
}

void Windowing::start_restricted(Proxy&, time_point const&)
{
    XEvent event;
    while(XPending(C_RCAST<Display*>(m_display)))
    {
        XNextEvent(C_RCAST<Display*>(m_display), &event);

        switch(event.type)
        {
        case ClientMessage:
        case UnmapNotify:
            m_shouldClose = true;
            break;
        default:
            break;
        }
    }
}

void Windowing::show()
{
}

void Windowing::close()
{
}

comp_app::size_2d_t Windowing::size() const
{
    typing::geometry::size_2d<unsigned int> out;
    int x, y;
    unsigned int border, depth;

    Window ret_window = C_RCAST<Window>(m_window);

    XGetGeometry(
        C_RCAST<Display*>(m_display),
        C_RCAST<Drawable>(m_window),
        &ret_window,
        &x,
        &y,
        &out.w,
        &out.h,
        &border,
        &depth);
    return out;
}

void Windowing::resize(const comp_app::size_2d_t& newSize)
{
}

comp_app::position_t Windowing::position() const
{
    return {};
}

void Windowing::move(const comp_app::position_t& newPos)
{
}

comp_app::detail::WindowState Windowing::state() const
{
    return WindowState::Normal;
}

void Windowing::setState(comp_app::detail::WindowState state)
{
}

bool Windowing::notifiedClose() const
{
    return m_shouldClose;
}

void WindowInfo::load(AppLoadableService::entity_container& e, app_error& ec)
{
    m_container = &e;

    setName(AppLoader::config<WindowConfig>(e).title);
}

text_type_t WindowInfo::name() const
{
    char* name = nullptr;
    XFetchName(
        C_RCAST<Display*>(m_container->service<Windowing>()->m_display),
        C_RCAST<Window>(m_container->service<Windowing>()->m_window),
        &name);

    text_type_t out = name ? name : "";
    XFree(name);
    return out;
}

void WindowInfo::setName(text_type newName)
{
    XStoreName(
        C_RCAST<Display*>(m_container->service<Windowing>()->m_display),
        C_RCAST<Window>(m_container->service<Windowing>()->m_window),
        newName.c_str());
}

} // namespace x11
