#pragma once

#include <coffee/comp_app/services.h>

namespace glkit {

struct Windowing
    : comp_app::interfaces::StaticWindowing
    , comp_app::AppService<Windowing, comp_app::Windowing>
{
    virtual comp_app::size_2d_t      size() const final;
    virtual comp_app::window_flags_t state() const final;
    virtual void setState(comp_app::window_flags_t) final;
};

struct DisplayInfo
    : comp_app::interfaces::SingleDisplayInfo
    , comp_app::AppService<DisplayInfo, comp_app::DisplayInfo>
{
    virtual comp_app::size_2d_t virtualSize() const final;
    virtual comp_app::size_2d_t physicalSize(libc_types::u32) const final;
    virtual libc_types::f32     dpi(libc_types::u32) const final;
    virtual libc_types::f32     diagonal(libc_types::u32) const final;
};

using Services = comp_app::detail::TypeList<Windowing, DisplayInfo>;

} // namespace glkit
