#include <coffee/comp_app/stat_providers.h>
#include <coffee/comp_app/subsystems.h>

#include <platforms/sysinfo.h>

#if defined(COFFEE_LINUX)
#include <filesystem>
#include <fstream>
#include <sstream>
#endif

namespace comp_app {

namespace {
#if defined(COFFEE_LINUX)
namespace fs = std::filesystem;

std::optional<std::string> read_sysfs_line(fs::path const& p)
{
    std::ifstream f(p);
    if(!f.is_open())
        return std::nullopt;
    std::string line;
    std::getline(f, line);
    return line;
}

std::optional<libc_types::i64> read_sysfs_i64(fs::path const& p)
{
    auto line = read_sysfs_line(p);
    if(!line || line->empty())
        return std::nullopt;
    try
    {
        return static_cast<libc_types::i64>(std::stoll(*line));
    } catch(std::exception const&)
    {
        return std::nullopt;
    }
}

/* First /sys/class/devfreq node whose name ends in ".gpu" (the convention used
 * by msm/freedreno, panfrost, lima, ...). */
std::string find_gpu_devfreq()
{
    std::error_code ec;
    fs::path        base{"/sys/class/devfreq"};
    if(!fs::is_directory(base, ec))
        return {};
    for(auto const& entry : fs::directory_iterator(base, ec))
    {
        auto name = read_sysfs_line(entry.path() / "name");
        std::string node = name && !name->empty() ? *name
                                                  : entry.path().filename().string();
        if(node.size() > 4 && node.compare(node.size() - 4, 4, ".gpu") == 0)
            return entry.path().string();
    }
    return {};
}

/* First thermal zone whose type contains "gpu". Returns the temp file path. */
std::optional<fs::path> find_gpu_thermal()
{
    std::error_code ec;
    fs::path        base{"/sys/class/thermal"};
    if(!fs::is_directory(base, ec))
        return std::nullopt;
    for(auto const& entry : fs::directory_iterator(base, ec))
    {
        auto type = read_sysfs_line(entry.path() / "type");
        if(type && type->find("gpu") != std::string::npos)
            return entry.path() / "temp";
    }
    return std::nullopt;
}

/* Parse devfreq trans_stat: each data row's last column is cumulative time(ms)
 * spent at that frequency; the first data row is the lowest (idle) frequency.
 * Returns {total_ms, busy_ms} where busy = total - idle. */
std::pair<libc_types::u64, libc_types::u64> read_trans_stat(fs::path const& p)
{
    std::ifstream f(p);
    if(!f.is_open())
        return {0, 0};
    libc_types::u64 total_ms = 0, idle_ms = 0;
    bool            first_data_row = true;
    std::string     line;
    while(std::getline(f, line))
    {
        std::istringstream ss(line);
        std::string        tok, last;
        bool               is_data = false;
        while(ss >> tok)
        {
            if(tok == "*")
                continue; // marker for the current frequency
            if(tok == ":" || tok == "From" || tok == "To" ||
               tok.find("time") != std::string::npos)
            {
                is_data = false;
                break;
            }
            is_data = true;
            last    = tok;
        }
        if(!is_data || last.empty())
            continue;
        try
        {
            auto t = static_cast<libc_types::u64>(std::stoull(last));
            total_ms += t;
            if(first_data_row)
            {
                idle_ms        = t;
                first_data_row = false;
            }
        } catch(std::exception const&)
        {
        }
    }
    return {total_ms, total_ms > idle_ms ? total_ms - idle_ms : 0};
}
#endif
} // namespace

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
#if defined(COFFEE_LINUX)
    if(auto temp = find_gpu_thermal())
        if(auto milli = read_sysfs_i64(*temp))
            return static_cast<libc_types::f32>(*milli) / 1000.f; // m°C -> °C
#endif
    return 0.f;
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
#if defined(COFFEE_LINUX)
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
#else
    return 0.f;
#endif
}

std::vector<interfaces::CPUClockProvider::thread_load_t> SysCPUClock::
    threadCpuLoads()
{
#if defined(COFFEE_LINUX)
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
#else
    return {};
#endif
}

void SysGPUStats::load(entity_container&, app_error&)
{
#if defined(COFFEE_LINUX)
    m_devfreq = find_gpu_devfreq();
    if(!m_devfreq.empty())
        std::tie(m_prev_total_ms, m_prev_busy_ms) =
            read_trans_stat(fs::path(m_devfreq) / "trans_stat");
#endif
}

std::optional<libc_types::u32> SysGPUStats::mem_resident()
{
    return std::nullopt;
}

std::optional<libc_types::u32> SysGPUStats::mem_total()
{
    return std::nullopt;
}

std::optional<libc_types::u8> SysGPUStats::usage()
{
#if defined(COFFEE_LINUX)
    if(m_devfreq.empty())
        return std::nullopt;
    auto [total, busy] = read_trans_stat(fs::path(m_devfreq) / "trans_stat");
    auto d_total       = total - m_prev_total_ms;
    auto d_busy        = busy - m_prev_busy_ms;
    m_prev_total_ms    = total;
    m_prev_busy_ms     = busy;
    if(d_total > 0)
        m_usage = static_cast<libc_types::u8>(d_busy * 100 / d_total);
    return m_usage;
#else
    return std::nullopt;
#endif
}

std::map<std::string_view, libc_types::f32> SysGPUStats::stats_numeric()
{
    /* Do NOT clear m_numeric: the profiler keys metrics by string_view into
     * these keys and serializes them at export, so the backing strings must
     * stay alive for the whole run. std::map keys are stable across updates. */
#if defined(COFFEE_LINUX)
    if(m_devfreq.empty())
        return {};
    fs::path base(m_devfreq);
    if(auto cur = read_sysfs_i64(base / "cur_freq"))
        m_numeric["GPU clock current (Hz)"] = static_cast<libc_types::f32>(*cur);
    if(auto max = read_sysfs_i64(base / "max_freq"))
        m_numeric["GPU clock max (Hz)"] = static_cast<libc_types::f32>(*max);
    if(auto min = read_sysfs_i64(base / "min_freq"))
        m_numeric["GPU clock min (Hz)"] = static_cast<libc_types::f32>(*min);
#endif
    std::map<std::string_view, libc_types::f32> out;
    for(auto const& stat : m_numeric)
        out[stat.first] = stat.second;
    return out;
}

std::map<std::string_view, comp_app::interfaces::GPUStatProvider::stats_desc_t>
SysGPUStats::stats_description()
{
    using platform::profiling::MetricVariant;
    std::map<std::string_view, stats_desc_t> out;
    for(auto const& stat : m_numeric)
        out[stat.first] = stats_desc_t{
            .type          = MetricVariant::Value,
            .is_percentage = false,
        };
    return out;
}

} // namespace comp_app
