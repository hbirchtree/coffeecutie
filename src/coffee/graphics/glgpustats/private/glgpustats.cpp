#include <glgpustats/glgpustats.h>

#include <coffee/core/debug/formatting.h>
#include <glw/glw.h>
#include <glw/extensions/AMD_performance_monitor.h>
#include <glw/extensions/ATI_meminfo.h>
#include <glw/extensions/NVX_gpu_memory_info.h>
#include <peripherals/semantic/chunk.h>
#include <peripherals/stl/range.h>
#include <peripherals/stl/string/split.h>

#include <array>
#include <cstring>
#include <limits>
#include <set>

using namespace Coffee::Logging;
using libc_types::f32;
using libc_types::i32;
using libc_types::i64;
using libc_types::u32;
using libc_types::u64;
using libc_types::u8;

namespace glgpustats {

namespace {

/* Pick the same glw version alias glscreenshot uses: highest version that is
 * guaranteed to exist for the linked GL implementation. */
using glw = std::conditional<
    gl::core::enabled,
    gl::core::v33,
#if GLEAM_MAX_VERSION_ES >= 0x300
    gl::es::v30
#else
    gl::es::v20
#endif
    >::type;

using gl::group::get_prop;
using gl::group::string_name;

constexpr auto noerr = gl::error_check::off;

/* Standard GL counter value types returned by GL_AMD_performance_monitor. */
constexpr u32 gl_counter_unsigned_int = 0x1405; // GL_UNSIGNED_INT
constexpr u32 gl_counter_float        = 0x1406; // GL_FLOAT

std::set<std::string> gl_extensions()
{
    std::set<std::string> out;
#if GLEAM_MAX_VERSION >= 0x300
    if constexpr(gl::core::enabled)
    {
        /* Core profiles reject glGetString(GL_EXTENSIONS); enumerate by index. */
        i32 count{0};
        glw::get_integerv(get_prop::num_extensions, semantic::SpanOne(count));
        for(auto i : stl_types::range<i32>(count))
            out.insert(glw::get_stringi(string_name::extensions, i));
        return out;
    }
#endif
    auto joined = glw::get_string(string_name::extensions);
    for(auto ext : stl_types::str::split::str<char>(joined, ' '))
        out.insert(std::string(ext.begin(), ext.end()));
    return out;
}

#ifdef GL_AMD_performance_monitor
namespace pm = gl::amd::performance_monitor;
namespace pv = gl::amd::performance_monitor::values;

std::string amd_group_name(u32 group)
{
    i32 length{0};
    pm::get_perf_monitor_group_string(group, length, semantic::Span<char>(), noerr);
    if(length <= 0)
        return std::to_string(group);
    std::string out(static_cast<size_t>(length), '\0');
    pm::get_perf_monitor_group_string(
        group, length, semantic::Span<char>(out.data(), out.size()), noerr);
    out.resize(static_cast<size_t>(length));
    return out;
}

std::string amd_counter_name(u32 group, u32 counter)
{
    i32 length{0};
    pm::get_perf_monitor_counter_string(
        group, counter, length, semantic::Span<char>(), noerr);
    if(length <= 0)
        return std::to_string(counter);
    std::string out(static_cast<size_t>(length), '\0');
    pm::get_perf_monitor_counter_string(
        group, counter, length,
        semantic::Span<char>(out.data(), out.size()), noerr);
    out.resize(static_cast<size_t>(length));
    return out;
}
#endif

} // namespace

void GLGPUStatsProvider::ensure_init()
{
    if(m_init)
        return;
    m_init = true;

    auto extensions = gl_extensions();

#ifdef GL_NVX_gpu_memory_info
    m_nvx_ok = extensions.contains(gl::nvx::gpu_memory_info::name);
#endif
#ifdef GL_ATI_meminfo
    m_ati_ok = extensions.contains(gl::ati::meminfo::name);
#endif
#ifdef GL_AMD_performance_monitor
    if(extensions.contains(gl::amd::performance_monitor::name))
    {
        amd_setup();
        m_amd_ok = !m_amd_counters.empty();
    }
#endif

    if(m_amd_ok)
        m_strings["GPU stats: AMD_performance_monitor"] =
            std::to_string(m_amd_counters.size()) + " counters";
    if(m_nvx_ok)
        m_strings["GPU stats: NVX_gpu_memory_info"] = "active";
    if(m_ati_ok)
        m_strings["GPU stats: ATI_meminfo"] = "active";

    cDebug(
        "GLGPUStatsProvider: AMD_performance_monitor={} ({} counters), "
        "NVX_gpu_memory_info={}, ATI_meminfo={}",
        m_amd_ok,
        m_amd_counters.size(),
        m_nvx_ok,
        m_ati_ok);
}

void GLGPUStatsProvider::start_frame(
    compo::ContainerProxy&, const compo::time_point& time)
{
    ensure_init();
    if(m_nvx_ok)
        nvx_poll();
    if(m_ati_ok)
        ati_poll();
    if(m_amd_ok)
        amd_poll(time);
}

/* ---- GL_NVX_gpu_memory_info ---------------------------------------------- */

void GLGPUStatsProvider::nvx_poll()
{
#ifdef GL_NVX_gpu_memory_info
    namespace nv = gl::nvx::gpu_memory_info::values;
    auto query   = [](u32 pname) -> i32 {
        i32 value{0};
        glw::get_integerv(
            static_cast<get_prop>(pname), semantic::SpanOne(value), noerr);
        return value;
    };

    i32 dedicated = query(nv::gpu_memory_info_dedicated_vidmem);
    i32 total     = query(nv::gpu_memory_info_total_available_memory);
    i32 available = query(nv::gpu_memory_info_current_available_vidmem);
    i32 evictions = query(nv::gpu_memory_info_eviction_count);
    i32 evicted   = query(nv::gpu_memory_info_evicted_memory);

    /* NVX reports kibibytes; convert to bytes to match the nvml provider. */
    auto kib_to_bytes_f32 = [](i32 kib) -> f32 {
        return static_cast<f32>(static_cast<u64>(kib < 0 ? 0 : kib) * 1024u);
    };
    auto kib_to_bytes_u32 = [](i64 kib) -> u32 {
        u64 bytes = static_cast<u64>(kib < 0 ? 0 : kib) * 1024u;
        return bytes > std::numeric_limits<u32>::max()
                   ? std::numeric_limits<u32>::max()
                   : static_cast<u32>(bytes);
    };

    /* Per spec, DEDICATED_VIDMEM is the physical VRAM size and
     * CURRENT_AVAILABLE_VIDMEM is the currently-unused (free) portion of it, so
     * total VRAM = dedicated and used = dedicated - free. This matches the
     * nvml provider's total/used semantics. NVX is whole-GPU, not per-process. */
    i64 total_kib = dedicated > 0 ? dedicated : total;
    i64 used_kib  = total_kib > available ? total_kib - available : 0;
    m_nvx_total_bytes    = kib_to_bytes_u32(total_kib);
    m_nvx_resident_bytes = kib_to_bytes_u32(used_kib);

    /* Raw spec values only; no derived stats. */
    m_numeric["GPU dedicated VRAM"]         = {kib_to_bytes_f32(dedicated), false};
    m_numeric["GPU total available memory"] = {kib_to_bytes_f32(total), false};
    m_numeric["GPU current free VRAM"]      = {kib_to_bytes_f32(available), false};
    m_numeric["GPU eviction count"]         = {static_cast<f32>(evictions), false};
    m_numeric["GPU evicted memory"]         = {kib_to_bytes_f32(evicted), false};
#endif
}

/* ---- GL_ATI_meminfo ------------------------------------------------------- */

void GLGPUStatsProvider::ati_poll()
{
#ifdef GL_ATI_meminfo
    namespace am = gl::ati::meminfo::values;
    /* Each pname returns 4 ints in KiB: [0] total free in the pool, [1] largest
     * free block, [2] total aux free, [3] largest aux free. We report the raw
     * total-free per pool (free-only; ATI_meminfo exposes no totals). */
    auto pool_free_bytes = [](u32 pname) -> f32 {
        std::array<i32, 4> v{};
        glw::get_integerv(static_cast<get_prop>(pname), semantic::Span<i32>(v), noerr);
        return static_cast<f32>(static_cast<u64>(v[0] < 0 ? 0 : v[0]) * 1024u);
    };

    m_numeric["GPU VBO free memory"] = {pool_free_bytes(am::vbo_free_memory), false};
    m_numeric["GPU texture free memory"] =
        {pool_free_bytes(am::texture_free_memory), false};
    m_numeric["GPU renderbuffer free memory"] =
        {pool_free_bytes(am::renderbuffer_free_memory), false};
#endif
}

/* ---- GL_AMD_performance_monitor ------------------------------------------ */

void GLGPUStatsProvider::amd_setup()
{
#ifdef GL_AMD_performance_monitor
    i32 num_groups{0};
    pm::get_perf_monitor_groups(num_groups, semantic::Span<u32>(), noerr);
    if(num_groups <= 0)
        return;
    std::vector<u32> groups(static_cast<size_t>(num_groups));
    pm::get_perf_monitor_groups(
        num_groups, semantic::Span<u32>(groups.data(), groups.size()), noerr);
    groups.resize(static_cast<size_t>(num_groups));

    u32 monitor{0};
    pm::gen_perf_monitors(semantic::SpanOne(monitor), noerr);
    if(!monitor)
        return;
    m_amd_monitor = monitor;

    for(u32 group : groups)
    {
        i32 num_counters{0}, max_active{0};
        pm::get_perf_monitor_counters(
            group, num_counters, max_active, semantic::Span<u32>(), noerr);
        if(num_counters <= 0)
            continue;
        std::vector<u32> counters(static_cast<size_t>(num_counters));
        pm::get_perf_monitor_counters(
            group, num_counters, max_active,
            semantic::Span<u32>(counters.data(), counters.size()), noerr);
        counters.resize(static_cast<size_t>(num_counters));

        /* A group only allows max_active counters enabled simultaneously. */
        size_t take = counters.size();
        if(max_active > 0 && static_cast<size_t>(max_active) < take)
            take = static_cast<size_t>(max_active);

        std::string      group_name = amd_group_name(group);
        std::vector<u32> selected;
        selected.reserve(take);
        for(size_t i = 0; i < take; i++)
        {
            u32 counter = counters[i];
            u32 type{0};
            pm::get_perf_monitor_counter_info(
                group, counter, pv::counter_type, semantic::SpanOne(type), noerr);
            m_amd_counters.push_back(amd_counter_t{
                .group         = group,
                .counter       = counter,
                .type          = type,
                .is_percentage = type == pv::percentage,
                .name = group_name + " / " + amd_counter_name(group, counter),
            });
            selected.push_back(counter);
        }
        if(!selected.empty())
            pm::select_perf_monitor_counters(
                m_amd_monitor, true, group,
                semantic::Span<u32>(selected.data(), selected.size()), noerr);
    }
#endif
}

void GLGPUStatsProvider::amd_poll(const compo::time_point& time)
{
#ifdef GL_AMD_performance_monitor
    /* Close the window opened on the previous frame so the counters measured a
     * full frame's worth of GPU work. */
    if(m_amd_active)
    {
        pm::end_perf_monitor(m_amd_monitor, noerr);
        m_amd_active  = false;
        m_amd_pending = true;
    }
    if(m_amd_pending && amd_collect())
        m_amd_pending = false;

    /* Throttle to ~1 Hz so we don't stall the pipe every frame. */
    using namespace std::chrono_literals;
    if(!m_amd_pending && time >= m_amd_next)
    {
        m_amd_next = time + 1s;
        pm::begin_perf_monitor(m_amd_monitor, noerr);
        m_amd_active = true;
    }
#else
    (void)time;
#endif
}

bool GLGPUStatsProvider::amd_collect()
{
#ifdef GL_AMD_performance_monitor
    i32 written{0};
    u32 available{0};
    pm::get_perf_monitor_counter_data(
        m_amd_monitor, pv::perfmon_result_available, sizeof(u32),
        semantic::SpanOne(available), written, noerr);
    if(!available)
        return false;

    u32 result_bytes{0};
    pm::get_perf_monitor_counter_data(
        m_amd_monitor, pv::perfmon_result_size, sizeof(u32),
        semantic::SpanOne(result_bytes), written, noerr);
    if(result_bytes < sizeof(u32) * 3)
        return true; // finished, nothing usable

    std::vector<u32> result(result_bytes / sizeof(u32));
    pm::get_perf_monitor_counter_data(
        m_amd_monitor, pv::perfmon_result, static_cast<i32>(result_bytes),
        semantic::Span<u32>(result.data(), result.size()), written, noerr);

    size_t words = written > 0 ? static_cast<size_t>(written) / sizeof(u32)
                               : result.size();
    if(words > result.size())
        words = result.size();

    /* The result block is a packed sequence of
     *   [groupId][counterId][value...]
     * where the value width depends on the counter's type. */
    size_t i = 0;
    while(i + 2 < words)
    {
        u32   group   = result[i++];
        u32   counter = result[i++];
        auto* def     = find_counter(group, counter);
        if(!def)
            break; // unknown layout, can't advance safely

        f32 value = 0.f;
        switch(def->type)
        {
        case gl_counter_float:
        case pv::percentage:
        {
            float as_float;
            std::memcpy(&as_float, &result[i], sizeof(as_float));
            value = as_float;
            i += 1;
            break;
        }
        case pv::unsigned_int64:
        {
            if(i + 1 >= words)
                return true;
            u64 as_u64;
            std::memcpy(&as_u64, &result[i], sizeof(as_u64));
            value = static_cast<f32>(as_u64);
            i += 2;
            break;
        }
        case gl_counter_unsigned_int:
        default:
            value = static_cast<f32>(result[i]);
            i += 1;
            break;
        }
        m_numeric[def->name] =
            reading_t{.value = value, .is_percentage = def->is_percentage};
    }
    return true;
#else
    return true;
#endif
}

GLGPUStatsProvider::amd_counter_t const* GLGPUStatsProvider::find_counter(
    u32 group, u32 counter) const
{
    for(auto const& def : m_amd_counters)
        if(def.group == group && def.counter == counter)
            return &def;
    return nullptr;
}

/* ---- GPUStatProvider accessors ------------------------------------------- */

std::optional<libc_types::u32> GLGPUStatsProvider::mem_resident()
{
    if(!m_nvx_ok)
        return std::nullopt;
    return m_nvx_resident_bytes;
}

std::optional<libc_types::u32> GLGPUStatsProvider::mem_total()
{
    if(!m_nvx_ok)
        return std::nullopt;
    return m_nvx_total_bytes;
}

std::optional<libc_types::u8> GLGPUStatsProvider::usage()
{
    /* Neither extension exposes a portable GPU-busy percentage. */
    return std::nullopt;
}

std::map<std::string_view, libc_types::f32> GLGPUStatsProvider::stats_numeric()
{
    std::map<std::string_view, libc_types::f32> out;
    for(auto const& stat : m_numeric)
        out[stat.first] = stat.second.value;
    return out;
}

std::map<std::string_view, std::string> GLGPUStatsProvider::stats_strings()
{
    std::map<std::string_view, std::string> out;
    for(auto const& stat : m_strings)
        out[stat.first] = stat.second;
    return out;
}

std::map<std::string_view, comp_app::interfaces::GPUStatProvider::stats_desc_t>
GLGPUStatsProvider::stats_description()
{
    using platform::profiling::MetricVariant;
    std::map<std::string_view, stats_desc_t> out;
    for(auto const& stat : m_numeric)
        out[stat.first] = stats_desc_t{
            .type          = MetricVariant::Value,
            .is_percentage = stat.second.is_percentage,
        };
    return out;
}

} // namespace glgpustats
