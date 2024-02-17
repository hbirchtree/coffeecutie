#pragma once

#include "services.h"

namespace comp_app {

struct SysCPUTemp
    : interfaces::CPUTempProvider
    , AppService<SysCPUTemp, CPUTempProvider>
{
    virtual Presence sensorPresence(DeviceClass = DeviceClass::Default) final;
    virtual libc_types::f32 value(DeviceClass = DeviceClass::Default) final;
};

struct SysGPUTemp
    : interfaces::GPUTempProvider
    , AppService<SysGPUTemp, GPUTempProvider>
{
    virtual Presence sensorPresence(DeviceClass = DeviceClass::Default) final;
    virtual libc_types::f32 value(DeviceClass = DeviceClass::Default) final;
};

struct SysMemoryStats
    : interfaces::MemoryStatProvider
    , AppService<SysMemoryStats, MemoryStatProvider>
{
    virtual libc_types::u32 resident() final;
};

struct SysBattery
    : interfaces::BatteryProvider
    , AppService<SysBattery, BatteryProvider>
{
    virtual PowerSource     source() const final;
    virtual libc_types::u16 percentage() const final;
};

struct SysCPUClock
    : interfaces::CPUClockProvider
    , AppService<SysCPUClock, CPUClockProvider>
{
    virtual libc_types::u32 threads() final;
    virtual Governor        governor(libc_types::u32) final;
    virtual libc_types::f64 frequency(libc_types::u32) final;
};

} // namespace comp_app
