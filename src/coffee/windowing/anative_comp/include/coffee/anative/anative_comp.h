#pragma once

#include <coffee/comp_app/services.h>

namespace anative {

struct Windowing : comp_app::StaticWindowing, comp_app::AppLoadableService
{
    virtual void load(entity_container& e, comp_app::app_error& ec) override;

    virtual comp_app::size_2d_t           size() const override;
    virtual comp_app::detail::WindowState state() const override;
    virtual void setState(comp_app::detail::WindowState state) override;
};

struct ControllerInput : comp_app::ControllerInput
{
    virtual libc_types::u32       count() const override;
    virtual controller_map        state(libc_types::u32 idx) const override;
    virtual comp_app::text_type_t name(libc_types::u32 idx) const override;
};

using Services =
    comp_app::detail::TypeList<Windowing, comp_app::PtrNativeWindowInfo>;

} // namespace anative
