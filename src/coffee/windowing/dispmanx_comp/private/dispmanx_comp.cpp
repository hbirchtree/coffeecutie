#include <coffee/dispmanx/dispmanx_comp.h>

#include <coffee/core/debug/formatting.h>

#include <bcm_host.h>

using Coffee::Logging::cVerbose;

namespace dispmanx {

void Windowing::load(entity_container& e, comp_app::app_error& ec)
{
    cVerbose(12, "Calling bcm_host_init()");
    bcm_host_init();

    libc_types::u32 w, h;
    if(graphics_get_display_size(m_displayId, &w, &h) < 0)
    {
        ec = comp_app::AppError::NoWindow;
        ec = "Failed to query display size";
        return;
    }

    VC_RECT_T dest_rect, src_rect;
    vc_dispmanx_rect_set(&dest_rect, 0, 0, w, h);
    vc_dispmanx_rect_set(&src_rect, 0, 0, w << 16, h << 16);

    cVerbose(12, "Opening display");
    m_display = vc_dispmanx_display_open(m_displayId);
    m_update  = vc_dispmanx_update_start(0);
    cVerbose(12, "Adding element");
    m_element = vc_dispmanx_element_add(
        m_update,
        m_display,
        0,
        &dest_rect,
        0,
        &src_rect,
        DISPMANX_PROTECTION_NONE,
        nullptr,
        nullptr,
        DISPMANX_NO_ROTATE);
    if(!m_element)
    {
        ec = comp_app::AppError::NoWindow;
        ec = "Failed to create DispManX element";
        return;
    }
    m_window = {
        .element = m_element,
        .width   = static_cast<int>(w),
        .height  = static_cast<int>(h),
    };

    auto* ptr_info = e.service<comp_app::PtrNativeWindowInfoService>();
    if(!ptr_info)
    {
        ec = comp_app::AppError::ServiceMissing;
        ec = "Failed to find comp_app::PtrNativeWindowInfoService";
        return;
    }
    ptr_info->window  = &m_window;
    ptr_info->display = &m_display;

    cVerbose(12, "Syncing DispManX");
    vc_dispmanx_update_submit_sync(m_update);
    cVerbose(12, "Sync complete");
}

comp_app::size_2d_t Windowing::size() const
{
    libc_types::u32 w, h;
    graphics_get_display_size(m_displayId, &w, &h);
    return comp_app::size_2d_t::from_values<comp_app::size_2d_t>(w, h);
}

comp_app::window_flags_t Windowing::state() const
{
    return comp_app::window_flags_t::fullscreen;
}

void Windowing::setState(comp_app::window_flags_t)
{
}

} // namespace dispmanx
