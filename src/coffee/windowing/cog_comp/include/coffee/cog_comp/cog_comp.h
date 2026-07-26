#pragma once

#include <coffee/comp_app/services.h>

namespace cog {

struct Windowing
    : comp_app::interfaces::StaticWindowing
    , comp_app::AppService<Windowing, comp_app::Windowing>
{
    virtual comp_app::size_2d_t      size() const final;
    virtual comp_app::window_flags_t state() const final;
    virtual void                     setState(comp_app::window_flags_t) final;
};

struct GraphicsFramebuffer
    : comp_app::interfaces::GraphicsFramebuffer
    , comp_app::AppService<GraphicsFramebuffer, comp_app::GraphicsFramebuffer>
{
    virtual comp_app::size_2d_t size() const final;
    virtual void                swapBuffers(comp_app::app_error& ec) final;

    virtual void end_frame(
        comp_app::detail::ContainerProxy&   c,
        comp_app::detail::time_point const& t) final;
};

using Services = type_safety::type_list_t<Windowing, GraphicsFramebuffer>;

} // namespace cog
