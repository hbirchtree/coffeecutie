#include <pvr/pvr_components.h>

#include <PVRScopeStats.h>
#include <coffee/core/debug/formatting.h>
#include <magic_enum.hpp>
#include <string_view>

using namespace Coffee::Logging;

namespace pvr {

void PVRGPUStats::start_frame(
    compo::ContainerProxy&, const compo::time_point& time)
{
    if(!m_context || time < m_next_dump)
        return;

    using namespace std::chrono_literals;
    m_next_dump = time + 1s;

    dump_stats(0xFFFFFFFF);
    dump_stats(0);
    dump_stats(1);
    dump_stats(4);
    dump_stats(5);
    dump_stats(9);
}

std::optional<libc_types::u32> PVRGPUStats::mem_resident()
{
    return std::nullopt;
}

std::optional<libc_types::u32> PVRGPUStats::mem_total()
{
    return std::nullopt;
}

std::optional<libc_types::u8> PVRGPUStats::usage()
{
    return std::nullopt;
}

std::map<std::string_view, libc_types::f32> PVRGPUStats::stats_numeric()
{
    using libc_types::u16;
    using namespace std::string_view_literals;

    std::map<std::string_view, libc_types::f32> out;
    for(auto const& stat : m_cached_readings)
        out[stat.first] = stat.second.value;
    return out;
}

std::map<std::string_view, std::string> PVRGPUStats::stats_strings()
{
    return {};
}

std::map<std::string_view, comp_app::interfaces::GPUStatProvider::stats_desc_t>
PVRGPUStats::stats_description()
{
    using platform::profiling::MetricVariant;
    using namespace std::string_view_literals;

    std::map<std::string_view, stats_desc_t> out;
    for(auto const& stat : m_cached_readings)
        out[stat.first] = stats_desc_t{
            .type          = MetricVariant::Value,
            .is_percentage = stat.second.is_percentage,
        };
    return out;
}

void PVRGPUStats::load(entity_container& e, comp_app::app_error& ec)
{
    auto code = PVRScopeInitialise(&m_context);
    if(code != ePVRScopeInitCodeOk)
    {
        cWarning("Failed to init PVRScope: {}", magic_enum::enum_name(code));
        m_context = nullptr;
        return;
    }
    cDebug("Initialized PVRScope: {}", PVRScopeGetDescription());
    PVRScopeGetCounters(m_context, &m_num_counters, &m_counters, &m_reading);
    for(auto i : stl_types::range(m_num_counters))
    {
        cDebug(
            " - {} : percentage={}, group={}",
            m_counters[i].pszName,
            m_counters[i].nBoolPercentage,
            m_counters[i].nGroup);
    }
}

void PVRGPUStats::unload(entity_container& e, comp_app::app_error& ec)
{
    if(!m_context)
        return;
    PVRScopeDeInitialise(&m_context, &m_counters, &m_reading);
    m_num_counters = 0;
}

void PVRGPUStats::dump_stats(libc_types::u32 group)
{
    PVRScopeSetGroup(m_context, group);
    if(PVRScopeReadCounters(m_context, &m_reading))
    {
        size_t reading_i{0};
        for(auto i : stl_types::range(m_num_counters))
        {
            if(m_counters[i].nGroup != group)
            {
                if(m_counters[i].nGroup == 0xFFFFFFFF)
                    reading_i++;
                continue;
            }
            m_cached_readings[m_counters[i].pszName] = counter_reading_t{
                .value = m_reading.pfValueBuf[reading_i++],
                .is_percentage
                = static_cast<bool>(m_counters[i].nBoolPercentage),
            };
        }
    }
}

} // namespace pvr
