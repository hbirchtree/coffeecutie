#pragma once

#include <PVRScopeStats.h>
#include <coffee/comp_app/services.h>
#include <map>

struct SPVRScopeImplData;
struct SPVRScopeCounterDef;

namespace pvr {

struct PVRGPUStats
    : comp_app::interfaces::GPUStatProvider
    , comp_app::AppService<PVRGPUStats, comp_app::GPUStatProvider>
    , comp_app::AppLoadableService
{
    void start_frame(compo::ContainerProxy&, const compo::time_point& time);

    std::optional<libc_types::u32>              mem_resident() final;
    std::optional<libc_types::u32>              mem_total() final;
    std::optional<libc_types::u8>               usage() final;
    std::map<std::string_view, libc_types::f32> stats_numeric() final;
    std::map<std::string_view, std::string>     stats_strings() final;
    std::map<std::string_view, stats_desc_t>    stats_description() final;

  protected:
    void load(entity_container& e, comp_app::app_error& ec);
    void unload(entity_container& e, comp_app::app_error& ec);

  private:
    void dump_stats(libc_types::u32 group);

    SPVRScopeImplData*      m_context{nullptr};
    SPVRScopeCounterDef*    m_counters{nullptr};
    unsigned int            m_num_counters{0};
    SPVRScopeCounterReading m_reading = {
        .pfValueBuf          = nullptr,
        .nValueCnt           = 0,
        .nReadingActiveGroup = 0,
    };

    struct counter_reading_t
    {
        libc_types::f32 value;
        bool            is_percentage{false};
    };

    std::map<std::string, counter_reading_t> m_cached_readings;
    compo::time_point                        m_next_dump{};
};

} // namespace pvr
