#pragma once

#include <coffee/comp_app/services.h>
#include <coffee/comp_app/subsystems.h>

namespace emscripten {

struct BatteryProvider
    : comp_app::interfaces::BatteryProvider
    , comp_app::AppService<BatteryProvider, comp_app::BatteryProvider>
{
    virtual ~BatteryProvider() = default;

    virtual PowerSource     source() const final;
    virtual libc_types::u16 percentage() const final;
};

using Services = comp_app::subsystem_list<BatteryProvider>;

} // namespace emscripten
