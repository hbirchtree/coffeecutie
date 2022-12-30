#include <coffee/glkit/glkit_comp.h>

#include <coffee/foreign/foreign.h>

namespace glkit {

comp_app::size_2d_t Windowing::size() const
{
//    int winSize[2] = {};

//    CoffeeForeignSignalHandleNA(
//        CoffeeForeign_GetWinSize, winSize, nullptr, nullptr);
//    Throw(unimplemented_path("GLKit screen size"));

    return comp_app::size_2d_t{0, 0};
}

comp_app::detail::WindowState Windowing::state() const
{
    return comp_app::detail::WindowState::FullScreen;
}

void Windowing::setState(comp_app::detail::WindowState)
{
}

}
