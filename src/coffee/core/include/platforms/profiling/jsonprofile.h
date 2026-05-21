#pragma once

#include <chrono>
#include <memory>
#include <peripherals/constants.h>
#include <peripherals/libc/types.h>
#include <platforms/pimpl_state.h>
#include <string_view>

namespace platform {
namespace profiling {

using libc_types::f32;
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
    u32                       index = 0,
    std::string_view index_name = {});

extern void CaptureMetrics(
    profiling::ThreadState&   tdata,
    std::string_view          name,
    MetricVariant             variant,
    f32                       value,
    std::chrono::microseconds ts,
    u32                       index = 0,
    std::string_view index_name = {});

template<typename T>
FORCEDINLINE void CaptureMetrics(
    std::string_view          name,
    MetricVariant             variant,
    T const&                  value,
    std::chrono::microseconds ts,
    u32                       index = 0,
    std::string_view index_name = {})
{
    if constexpr(!compile_info::profiler::enabled)
        return;

    auto thread_state = state->GetProfilerTStore();

    if(!thread_state)
        return;

    CaptureMetrics(*thread_state, name, variant, value, ts, index, index_name);
}

} // namespace json

} // namespace profiling
} // namespace platform
