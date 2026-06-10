#pragma once

#include "services.h"

#include <chrono>

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
    virtual libc_types::u64 resident() final;
};

/* GPU stats from Linux sysfs (devfreq). Works on any DRM driver that exposes a
 * GPU devfreq node (freedreno/msm, panfrost, lima, ...), where GL counter
 * extensions are typically unavailable. Reports raw clocks; usage is derived
 * from devfreq frequency-state residency (trans_stat). */
struct SysGPUStats
    : interfaces::GPUStatProvider
    , AppService<SysGPUStats, GPUStatProvider>
    , AppLoadableService
{
    std::optional<libc_types::u32>              mem_resident() final;
    std::optional<libc_types::u32>              mem_total() final;
    std::optional<libc_types::u8>               usage() final;
    std::map<std::string_view, libc_types::f32> stats_numeric() final;
    std::map<std::string_view, stats_desc_t>    stats_description() final;

  protected:
    void load(entity_container& e, app_error& ec) final;

  private:
    std::string m_devfreq; // /sys/class/devfreq/<node>, empty if none found
    /* trans_stat residency snapshot for delta-based utilization */
    libc_types::u64 m_prev_total_ms{0};
    libc_types::u64 m_prev_busy_ms{0};
    libc_types::u8  m_usage{0};
    std::map<std::string, libc_types::f32> m_numeric;
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
    virtual libc_types::u32 cpus() final;
    virtual libc_types::u32 cores(libc_types::u32 cpu) final;
    virtual Governor        governor(libc_types::u32) final;
    virtual libc_types::f64 frequency(
        libc_types::u32 cpu, libc_types::u32 core) final;
    virtual libc_types::f32 processCpuLoad() final;
    virtual std::vector<interfaces::CPUClockProvider::thread_load_t>
    threadCpuLoads() final;

  private:
    libc_types::u64                            m_lastCpuTime{0};
    std::chrono::steady_clock::time_point      m_lastWallTime{};
    std::map<libc_types::u32, libc_types::u64> m_lastThreadTicks;
    std::chrono::steady_clock::time_point      m_lastThreadWallTime{};
};

} // namespace comp_app
