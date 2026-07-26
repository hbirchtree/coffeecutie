#pragma once

#include <chrono>
#include <gsl/span>
#include <memory>
#include <peripherals/constants.h>
#include <peripherals/libc/types.h>
#include <platforms/pimpl_state.h>
#include <string_view>

namespace platform {
namespace profiling {

using libc_types::f32;
using libc_types::i64;
using libc_types::u32;

struct datapoint_t;
struct ThreadState;

enum class MetricVariant
{
    Value,    /*!< Value is a continuous value */
    Symbolic, /*!< Values are only symbolic, not continuous */
    Marker,   /*!< Marker for eg. VSYNC or other events */

    Image, /*!< Captured framebuffer or other image */
};

namespace json {

extern std::shared_ptr<GlobalState> CreateProfiler();

extern void Push(ThreadState& tdata, profiling::datapoint_t const& point);

extern void CaptureMetrics(
    ThreadState&              tdata,
    std::string_view          name,
    MetricVariant             variant,
    std::string const&        value,
    std::chrono::microseconds ts,
    u32                       index      = 0,
    std::string_view          index_name = {});

extern void CaptureMetrics_i64(
    profiling::ThreadState&   tdata,
    std::string_view          name,
    MetricVariant             variant,
    i64                       value,
    std::chrono::microseconds ts,
    u32                       index      = 0,
    std::string_view          index_name = {});

extern void CaptureMetrics_f32(
    profiling::ThreadState&   tdata,
    std::string_view          name,
    MetricVariant             variant,
    f32                       value,
    std::chrono::microseconds ts,
    u32                       index      = 0,
    std::string_view          index_name = {});

template<typename T>
FORCEDINLINE void CaptureMetrics(
    std::string_view          name,
    MetricVariant             variant,
    T const&                  value,
    std::chrono::microseconds ts,
    u32                       index      = 0,
    std::string_view          index_name = {})
{
    if constexpr(!compile_info::profiler::enabled)
        return;

    auto thread_state = state->GetProfilerTStore();

    if(!thread_state)
        return;

    if constexpr(std::is_floating_point_v<T>)
        CaptureMetrics_f32(
            *thread_state, name, variant, value, ts, index, index_name);
    else if constexpr(std::is_integral_v<T>)
        CaptureMetrics_i64(
            *thread_state, name, variant, value, ts, index, index_name);
    else
        CaptureMetrics(
            *thread_state, name, variant, value, ts, index, index_name);
}

#if !defined(COFFEE_GEKKO)
void CaptureTrace(
    profiling::ThreadState&         tdata,
    std::string_view                function_name,
    std::vector<std::string> const& args,
    gsl::span<const char> const&    data = {});
#endif

} // namespace json

} // namespace profiling
} // namespace platform
