#pragma once

#include <coffee/comp_app/services.h>
#include <bcm_host.h>
#include <EGL/egl.h>

namespace dispmanx {

struct Windowing
    : comp_app::interfaces::StaticWindowing,
      comp_app::AppService<Windowing, comp_app::Windowing>,
      comp_app::AppLoadableService
{
  public:
    void load(entity_container &e, comp_app::app_error &ec);

    comp_app::size_2d_t size() const;
    comp_app::detail::WindowState state() const;
    void setState(comp_app::detail::WindowState state);
  private:
    libc_types::u32 m_displayId{DISPMANX_ID_HDMI};

    DISPMANX_DISPLAY_HANDLE_T m_display{};
    DISPMANX_UPDATE_HANDLE_T m_update{};
    DISPMANX_ELEMENT_HANDLE_T m_element{};
    EGL_DISPMANX_WINDOW_T m_window{};
};

}
