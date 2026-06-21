#include <coffee/cog_comp/cog_comp.h>

#include <coffee/gexxo/gexxo_api.h>

namespace cog {

comp_app::size_2d_t Windowing::size() const
{
    return gexxo::framebufferSize();
}

comp_app::window_flags_t Windowing::state() const
{
    using F = comp_app::window_flags_t;

    return F::fullscreen_window | F::focused | F::visible;
}

void Windowing::setState(comp_app::window_flags_t)
{
}

comp_app::size_2d_t GraphicsFramebuffer::size() const
{
    return gexxo::framebufferSize();
}

void GraphicsFramebuffer::swapBuffers(comp_app::app_error& ec)
{
    gexxo::swapBuffers();
}

void GraphicsFramebuffer::end_frame(
    comp_app::detail::ContainerProxy&, comp_app::detail::time_point const&)
{
    defaultSwap();
}

} // namespace cog
