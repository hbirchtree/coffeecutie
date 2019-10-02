#pragma once

#include <coffee/comp_app/services.h>

namespace cog {

struct Windowing : comp_app::StaticWindowing
{
    virtual comp_app::size_2d_t           size() const final;
    virtual comp_app::detail::WindowState state() const final;
    virtual void setState(comp_app::detail::WindowState) final;
};

using Services = type_safety::type_list_t<Windowing>;

} // namespace cog
