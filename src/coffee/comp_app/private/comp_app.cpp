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

libc_types::u64 SysMemoryStats::resident()
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

libc_types::u32 SysCPUClock::cpus()
{
    return platform::info::proc::cpu_count();
}

libc_types::u32 SysCPUClock::cores(libc_types::u32 cpu)
{
    return platform::info::proc::core_count(cpu);
}

SysCPUClock::Governor SysCPUClock::governor(libc_types::u32)
{
    return Governor::Ondemand;
}

libc_types::f64 SysCPUClock::frequency(libc_types::u32 i, libc_types::u32 core)
{
    return platform::info::proc::frequency(true, i, core);
}

libc_types::f32 SysCPUClock::processCpuLoad()
{
    using namespace std::chrono;
    auto now_cpu  = platform::info::proc::cpu_time();
    auto now_wall = steady_clock::now();

    libc_types::f32 result = 0.f;
    if(m_lastCpuTime > 0)
    {
        auto delta_cpu  = static_cast<libc_types::f32>(now_cpu - m_lastCpuTime);
        auto delta_wall = static_cast<libc_types::f32>(
            duration_cast<nanoseconds>(now_wall - m_lastWallTime).count());
        if(delta_wall > 0.f)
            result = delta_cpu / delta_wall * 100.f;
    }
    m_lastCpuTime  = now_cpu;
    m_lastWallTime = now_wall;
    return result;
}

std::vector<interfaces::CPUClockProvider::thread_load_t>
SysCPUClock::threadCpuLoads()
{
    using namespace std::chrono;
    using thread_load_t = interfaces::CPUClockProvider::thread_load_t;

    auto now_ticks = platform::info::proc::thread_cpu_ticks();
    auto now_names = platform::info::proc::thread_names();
    auto now_wall  = steady_clock::now();
    auto tps       = platform::info::proc::ticks_per_second();

    auto delta_wall_ns = static_cast<libc_types::f32>(
        duration_cast<nanoseconds>(now_wall - m_lastThreadWallTime).count());

    std::vector<thread_load_t> loads;
    loads.reserve(now_ticks.size());

    for(auto const& [tid, ticks] : now_ticks)
    {
        libc_types::f32 load = 0.f;
        auto            it   = m_lastThreadTicks.find(tid);
        if(delta_wall_ns > 0.f && tps > 0 && it != m_lastThreadTicks.end())
        {
            auto delta_ticks_ns = static_cast<libc_types::f32>(
                (ticks - it->second) * 1'000'000'000ULL / tps);
            load = delta_ticks_ns / delta_wall_ns * 100.f;
        }

        std::string name;
        if(auto nit = now_names.find(tid); nit != now_names.end())
            name = nit->second;

        loads.push_back({tid, std::move(name), load});
    }

    m_lastThreadTicks    = std::move(now_ticks);
    m_lastThreadWallTime = now_wall;
    return loads;
}

} // namespace comp_app
