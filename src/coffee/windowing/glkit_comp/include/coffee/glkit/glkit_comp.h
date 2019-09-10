#pragma once

#include <coffee/comp_app/services.h>

namespace glkit {

struct Windowing : comp_app::StaticWindowing
{
    virtual comp_app::size_2d_t           size() const final;
    virtual comp_app::detail::WindowState state() const final;
    virtual void setState(comp_app::detail::WindowState state) final;
};

using Services =
    comp_app::detail::TypeList<Windowing, comp_app::PtrNativeWindowInfo>;

} // namespace glkit
