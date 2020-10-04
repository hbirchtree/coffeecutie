#include <coffee/comp_app/stat_providers.h>
#include <coffee/comp_app/subsystems.h>

#include <platforms/power.h>
#include <platforms/sysinfo.h>

namespace comp_app {

Windowing::~Windowing()
{
}

SysCPUTemp::Presence SysCPUTemp::sensorPresence(DeviceClass)
{
    return Presence::Present;
}

libc_types::f32 SysCPUTemp::value(DeviceClass)
{
    return platform::PowerInfo::CpuTemperature().current;
}

SysGPUTemp::Presence SysGPUTemp::sensorPresence(DeviceClass)
{
    return Presence::Present;
}

libc_types::f32 SysGPUTemp::value(DeviceClass)
{
    return platform::PowerInfo::GpuTemperature().current;
}

libc_types::u32 SysMemoryStats::resident()
{
    return C_FCAST<libc_types::u32>(platform::SysInfo::MemResident());
}

BatteryProvider::PowerSource SysBattery::source()
{
    return platform::PowerInfo::HasBattery() ? PowerSource::Battery
                                             : PowerSource::AC;
}

libc_types::u16 SysBattery::percentage()
{
    return platform::PowerInfo::BatteryPercentage();
}

libc_types::u32 SysCPUClock::threads()
{
    return C_FCAST<libc_types::u32>(platform::SysInfo::ThreadCount());
}

SysCPUClock::Governor SysCPUClock::governor(libc_types::u32)
{
    return Governor::Ondemand;
}

libc_types::f64 SysCPUClock::frequency(libc_types::u32 i)
{
    return platform::SysInfo::ProcessorFrequencies(true).at(i);
}

} // namespace comp_app
