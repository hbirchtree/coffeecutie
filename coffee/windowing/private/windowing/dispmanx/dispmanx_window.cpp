#include <coffee/windowing/windowing/dispmanx/dispmanx_window.h>

#if defined(COFFEE_RASPBERRY_DMX)

#include <coffee/core/CDebug>

#include <coffee/core/plat/windowmanager/plat_windowtype.h>

#include <bcm_host.h>
#include <EGL/egl.h>

namespace Coffee{
namespace Display{

struct DISPMAN_Data
{
    DISPMANX_DISPLAY_HANDLE_T display;
    EGL_DISPMANX_WINDOW_T window;
};

bool DispmanXWindow::windowPreInit(const CDProperties &props, CString *err)
{
    bcm_host_init();
    m_dmData = nullptr;

    C_UNUSED(props);
    C_UNUSED(err);
    return true;
}

bool DispmanXWindow::windowInit(const CDProperties &props, CString *err)
{
    _cbasic_size_2d<u32> screen_size = {};
    const u16 display_num = 0;

    VC_RECT_T dst_rect = {}, src_rect = {};

    i32 success = graphics_get_display_size(display_num, &screen_size.w, &screen_size.h);

    if(success < 0) return false;

    m_dmData = new DISPMAN_Data;

    dst_rect.width = screen_size.w;
    dst_rect.height = screen_size.h;
    src_rect.width = screen_size.w << 16;
    src_rect.height = screen_size.h << 16;

    m_dmData->display = vc_dispmanx_display_open(display_num);

    if(m_dmData->display == DISPMANX_NO_HANDLE)
        return false;

    auto upd = vc_dispmanx_update_start(0);

    m_dmData->window.element = vc_dispmanx_element_add(upd, m_dmData->display,
                                                       0, &dst_rect, 0, &src_rect,
                                                       DISPMANX_PROTECTION_NONE, 0, 0,
                                                       DISPMANX_NO_ROTATE);

    vc_dispmanx_update_submit_sync(upd);

    m_dmData->window.width = screen_size.w;
    m_dmData->window.height = screen_size.h;

    C_UNUSED(props);
    C_UNUSED(err);
    return true;
}

bool DispmanXWindow::windowPostInit(const CDProperties &props, CString *err)
{
    C_UNUSED(props);
    C_UNUSED(err);
    return true;
}

void DispmanXWindow::windowTerminate()
{
    cDebug("Terminating DISPMANX session");

    auto upd = vc_dispmanx_update_start(0);
    i32 success = vc_dispmanx_element_remove(upd, m_dmData->window.element);
    if(success != 0) return;
    vc_dispmanx_update_submit_sync(upd);
    success = vc_dispmanx_display_close(m_dmData->display);
    if(success != 0) return;
}

CDWindow *DispmanXWindow::window()
{
    CDWindow* win = new CDWindow;

    win->wininfo.dmx.window = m_dmData->window;

    return win;
}

uint32 DispmanXWindow::windowState() const
{
    return Display::CDProperties::FullScreen;
}

void DispmanXWindow::setWindowState(const CDProperties::State &s)
{
    C_UNUSED(s);
}

CSize DispmanXWindow::windowSize() const
{
    return {m_dmData->window.width, m_dmData->window.height};
}

void DispmanXWindow::setWindowSize(const CSize &s)
{
    C_UNUSED(s);
}

CPoint DispmanXWindow::windowPosition() const
{
    return {0,0};
}

void DispmanXWindow::setWindowPosition(const CPoint &p)
{
    C_UNUSED(p);
}

CString DispmanXWindow::windowTitle() const
{
    return {};
}

void DispmanXWindow::setWindowTitle(const CString &tl)
{
    C_UNUSED(tl);
}

bool DispmanXWindow::setWindowIcon(CBitmap &icon)
{
    C_UNUSED(icon);
    return true;
}

bool DispmanXWindow::screensaverMode()
{
    return true;
}

void DispmanXWindow::setScreensaverMode(bool m)
{
    C_UNUSED(m);
}

CString DispmanXWindow::windowLibrary() const
{
    return "DISPMANX";
}

CDMonitor DispmanXWindow::monitor()
{
    return {};
}

bool DispmanXWindow::showWindow()
{
    return true;
}

bool DispmanXWindow::hideWindow()
{
    return true;
}

bool DispmanXWindow::closeWindow()
{
    m_closeFlag = true;
    return true;
}

void DispmanXWindow::popErrorMessage(Severity s, cstring title, cstring msg)
{

}

}
}
#endif
