#include <coffee/comp_app/stat_providers.h>
#include <coffee/comp_app/subsystems.h>

#include <platforms/power.h>
#include <platforms/sysinfo.h>

namespace comp_app {

SysCPUTemp::Presence SysCPUTemp::sensorPresence(DeviceClass)
{
    return Presence::Present;
}

libc_types::f32 SysCPUTemp::value(DeviceClass)
{
//    return platform::PowerInfo::CpuTemperature().current;
    Throw(unimplemented_path(""));
}

SysGPUTemp::Presence SysGPUTemp::sensorPresence(DeviceClass)
{
    return Presence::Present;
}

libc_types::f32 SysGPUTemp::value(DeviceClass)
{
//    return platform::PowerInfo::GpuTemperature().current;
    Throw(unimplemented_path(""));
}

libc_types::u32 SysMemoryStats::resident()
{
//    return C_FCAST<libc_types::u32>(platform::SysInfo::MemResident());
    Throw(unimplemented_path(""));
}

interfaces::BatteryProvider::PowerSource SysBattery::source() const
{
//    return platform::PowerInfo::HasBattery() ? PowerSource::Battery
//                                             : PowerSource::AC;
    Throw(unimplemented_path(""));
}

libc_types::u16 SysBattery::percentage() const
{
//    return platform::PowerInfo::BatteryPercentage();
    Throw(unimplemented_path(""));
}

libc_types::u32 SysCPUClock::threads()
{
//    return C_FCAST<libc_types::u32>(platform::SysInfo::ThreadCount());
    Throw(unimplemented_path(""));
}

SysCPUClock::Governor SysCPUClock::governor(libc_types::u32)
{
    return Governor::Ondemand;
}

libc_types::f64 SysCPUClock::frequency(libc_types::u32 i)
{
//    return platform::SysInfo::ProcessorFrequencies(true).at(i);
    Throw(unimplemented_path(""));
}

} // namespace comp_app
