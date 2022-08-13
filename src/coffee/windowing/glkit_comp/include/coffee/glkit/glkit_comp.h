#pragma once

#include <coffee/comp_app/services.h>

namespace glkit {

struct Windowing : comp_app::interfaces::StaticWindowing,
                   comp_app::AppService<Windowing, comp_app::Windowing>
{
    virtual comp_app::size_2d_t           size() const final;
    virtual comp_app::detail::WindowState state() const final;
    virtual void setState(comp_app::detail::WindowState) final;
};

using Services =
    comp_app::detail::TypeList<Windowing>;

} // namespace glkit
