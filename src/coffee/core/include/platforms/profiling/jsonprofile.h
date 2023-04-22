#pragma once

#include <platforms/profiling.h>

namespace platform {
namespace profiling {

enum class MetricVariant
{
    Value,    /*!< Value is a continuous value */
    Symbolic, /*!< Values are only symbolic, not continuous */
    Marker,   /*!< Marker for eg. VSYNC or other events */

    Image, /*!< Captured framebuffer or other image */
};

namespace json {

extern std::shared_ptr<GlobalState> CreateProfiler();

extern void Push(
    profiling::ThreadState& tdata, profiling::DataPoint const& point);

extern void CaptureMetrics(
    profiling::ThreadState&   tdata,
    std::string_view          name,
    MetricVariant             variant,
    std::string const&        value,
    std::chrono::microseconds ts,
    u32                       index = 0);

extern void CaptureMetrics(
    profiling::ThreadState&   tdata,
    std::string_view          name,
    MetricVariant             variant,
    f32                       value,
    std::chrono::microseconds ts,
    u32                       index = 0);

template<typename T>
FORCEDINLINE void CaptureMetrics(
    cstring                   name,
    MetricVariant             variant,
    T const&                  value,
    std::chrono::microseconds ts,
    u32                       index = 0)
{
    if constexpr(!compile_info::profiler::enabled)
        return;

    auto thread_state = state->GetProfilerTStore();

    if(!thread_state)
        return;

    CaptureMetrics(*thread_state, name, variant, value, ts, index);
}

} // namespace json

} // namespace profiling
} // namespace platform
