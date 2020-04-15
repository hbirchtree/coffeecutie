#include <coffee/cog_comp/cog_comp.h>

#include <coffee/gexxo/gexxo_api.h>

namespace cog {

comp_app::size_2d_t Windowing::size() const
{
    return gexxo::framebufferSize();
}

comp_app::detail::WindowState Windowing::state() const
{
    using S = comp_app::detail::WindowState;

    return S::FullScreen | S::Undecorated | S::Focused;
}

void Windowing::setState(comp_app::detail::WindowState)
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

} // namespace cog
