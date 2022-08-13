#pragma once

#include <coffee/comp_app/services.h>

namespace emscripten {

struct ControllerInput
    : comp_app::interfaces::ControllerInput,
      comp_app::AppService<ControllerInput, comp_app::ControllerInput>
{
    virtual ~ControllerInput() = default;

    virtual void start_frame(
        comp_app::detail::ContainerProxy&,
        const comp_app::detail::time_point&) final;

    virtual libc_types::u32       count() const final;
    virtual controller_map        state(libc_types::u32 idx) const final;
    virtual comp_app::text_type_t name(libc_types::u32 idx) const final;

  private:
    struct state_t
    {
        stl_types::String name;
        controller_map    state;
    };
    stl_types::Map<libc_types::u32, state_t> m_controllers;
};

struct BatteryProvider
    : comp_app::interfaces::BatteryProvider,
      comp_app::AppService<BatteryProvider, comp_app::BatteryProvider>
{
    virtual ~BatteryProvider() = default;

    virtual PowerSource     source() const final;
    virtual libc_types::u16 percentage() const final;
};

} // namespace emscripten