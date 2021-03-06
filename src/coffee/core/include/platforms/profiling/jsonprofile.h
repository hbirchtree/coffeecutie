#pragma once

#include <platforms/profiling.h>

namespace platform {
namespace profiling {

enum class MetricVariant
{
    Value,    /*!< Value is a continuous value */
    Symbolic, /*!< Values are only symbolic, not continuous */
    Marker,   /*!< Marker for eg. VSYNC or other events */
};

namespace json {

extern ShPtr<GlobalState> CreateProfiler();

extern void Push(
    profiling::ThreadState& tdata, profiling::DataPoint const& point);

extern void CaptureMetrics(
    profiling::ThreadState& tdata,
    cstring                 name,
    MetricVariant           variant,
    f32                     value,
    Chrono::microseconds    ts,
    u32                     index = 0);

FORCEDINLINE void CaptureMetrics(
    cstring              name,
    MetricVariant        variant,
    f32                  value,
    Chrono::microseconds ts,
    u32                  index = 0)
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
