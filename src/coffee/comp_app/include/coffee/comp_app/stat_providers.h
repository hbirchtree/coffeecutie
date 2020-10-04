#pragma once

#include "services.h"

namespace comp_app {

struct SysCPUTemp : CPUTempProvider
{
    virtual Presence sensorPresence(DeviceClass = DeviceClass::Default) final;
    virtual libc_types::f32 value(DeviceClass = DeviceClass::Default) final;
};

struct SysGPUTemp : GPUTempProvider
{
    virtual Presence sensorPresence(DeviceClass = DeviceClass::Default) final;
    virtual libc_types::f32 value(DeviceClass = DeviceClass::Default) final;
};

struct SysMemoryStats : MemoryStatProvider
{
    virtual libc_types::u32 resident() final;
};

struct SysBattery : BatteryProvider
{
    virtual PowerSource source() final;
    virtual libc_types::u16 percentage() final;
};

struct SysCPUClock : CPUClockProvider
{
    virtual libc_types::u32 threads() final;
    virtual Governor governor(libc_types::u32) final;
    virtual libc_types::f64 frequency(libc_types::u32) final;
};

}
