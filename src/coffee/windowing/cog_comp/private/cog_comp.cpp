#include <coffee/cog_comp/cog_comp.h>

namespace cog {

comp_app::size_2d_t Windowing::size() const
{
    return {640, 480};
}

comp_app::detail::WindowState Windowing::state() const
{
    using S = comp_app::detail::WindowState;

    return S::FullScreen | S::Undecorated | S::Focused;
}

void Windowing::setState(comp_app::detail::WindowState)
{
}

} // namespace cog
