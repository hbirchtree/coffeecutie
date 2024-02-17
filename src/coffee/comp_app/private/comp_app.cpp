#include <coffee/comp_app/stat_providers.h>
#include <coffee/comp_app/subsystems.h>

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
    return platform::info::memory::resident();
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
    libc_types::u32 out;
    for(auto cpu : stl_types::range(platform::info::proc::cpu_count()))
        out +=
            C_FCAST<libc_types::u32>(platform::info::proc::thread_count(cpu));
    return out;
}

SysCPUClock::Governor SysCPUClock::governor(libc_types::u32)
{
    return Governor::Ondemand;
}

libc_types::f64 SysCPUClock::frequency(libc_types::u32 i)
{
    return platform::info::proc::frequency(true, i);
}

} // namespace comp_app
