#pragma once

#include <coffee/comp_app/services.h>

#include <map>
#include <string>
#include <vector>

namespace glgpustats {

/* GPUStatProvider backed by OpenGL extensions, modelled after pvr::PVRGPUStats:
 *
 *  - GL_AMD_performance_monitor : per-frame hardware counters. Every counter
 *    the driver exposes (across all groups, capped at each group's max active
 *    set) is selected, bracketed around one frame and dumped as numeric stats.
 *  - GL_NVX_gpu_memory_info     : VRAM dedicated/total/free + eviction info,
 *    used both for mem_resident()/mem_total() and as extra numeric stats.
 *  - GL_ATI_meminfo             : free VRAM per pool (VBO/texture/renderbuffer)
 *    on AMD/Mesa where NVX is absent; free-only, reported as numeric stats.
 *
 * Both sources are optional and detected at load(); whatever is present is
 * reported, the rest is silently skipped. All GL access happens on the render
 * thread via start_frame(); the accessors only return cached values. */
struct GLGPUStatsProvider
    : comp_app::interfaces::GPUStatProvider
    , comp_app::AppService<GLGPUStatsProvider, comp_app::GPUStatProvider>
    , comp_app::AppLoadableService
{
    void start_frame(compo::ContainerProxy&, const compo::time_point& time);

    /* Reported in bytes (matching the nvml provider). NVX's native unit is
     * KiB; the u32 accessors clamp on >4 GiB GPUs, the f32 numeric stats do
     * not. */
    std::optional<libc_types::u32>              mem_resident() final;
    std::optional<libc_types::u32>              mem_total() final;
    std::optional<libc_types::u8>               usage() final;
    std::map<std::string_view, libc_types::f32> stats_numeric() final;
    std::map<std::string_view, std::string>     stats_strings() final;
    std::map<std::string_view, stats_desc_t>    stats_description() final;

  private:
    /* GL is unavailable at load() time (no context yet), so extension probing
     * and counter setup happen lazily on the first start_frame, which runs on
     * the render thread with the context current. */
    void ensure_init();
    bool m_init{false};

    /* --- GL_AMD_performance_monitor --- */
    void amd_setup();                              // enumerate + select counters
    void amd_poll(const compo::time_point& time);  // drive begin/end/read window
    bool amd_collect();                            // non-blocking result read

    struct amd_counter_t
    {
        libc_types::u32 group;
        libc_types::u32 counter;
        libc_types::u32 type;          // GL counter type enum
        bool            is_percentage;
        std::string     name;          // "<group> / <counter>"
    };

    amd_counter_t const* find_counter(
        libc_types::u32 group, libc_types::u32 counter) const;

    std::vector<amd_counter_t> m_amd_counters;
    libc_types::u32            m_amd_monitor{0};
    bool                       m_amd_ok{false};
    bool                       m_amd_active{false};   // begin issued, not ended
    bool                       m_amd_pending{false};  // ended, result not read
    compo::time_point          m_amd_next{};

    /* --- GL_NVX_gpu_memory_info --- */
    void            nvx_poll();
    bool            m_nvx_ok{false};
    libc_types::u32 m_nvx_total_bytes{0};
    libc_types::u32 m_nvx_resident_bytes{0};

    /* --- GL_ATI_meminfo --- */
    void ati_poll();
    bool m_ati_ok{false};

    struct reading_t
    {
        libc_types::f32 value{0.f};
        bool            is_percentage{false};
    };
    std::map<std::string, reading_t>   m_numeric;  // AMD + NVX numeric readings
    std::map<std::string, std::string> m_strings;  // backend description
};

} // namespace glgpustats
