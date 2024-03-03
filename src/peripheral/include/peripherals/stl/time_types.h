#pragma once

#include <chrono>
#include <ctime>

#include <peripherals/libc/time_ops.h>
#include <peripherals/libc/types.h>

namespace stl_types {
namespace Chrono {

using seconds_f32  = std::chrono::duration<libc_types::f32>;
using seconds_f64 = std::chrono::duration<libc_types::f64>;

using Clock = std::chrono::steady_clock;

inline std::chrono::time_point<Clock, std::chrono::seconds> from_unix(
    libc_types::u64 ts)
{
    static_assert(
        type_safety::is_pod_v<std::time_t>, "std::time_t is not a POD type");

    std::time_t ts_unix = C_FCAST<std::time_t>(ts);
    auto        immTs   = std::chrono::system_clock::from_time_t(ts_unix);

    return std::chrono::time_point<Clock, std::chrono::seconds>(
        std::chrono::seconds(
            duration_cast<std::chrono::seconds>(immTs.time_since_epoch())
                .count()));
}

inline libc_types::u64 to_unix(Clock::time_point ts)
{
    static_assert(
        type_safety::is_pod_v<std::time_t>, "std::time_t is not a POD type");

    return C_FCAST<libc_types::u64>(std::chrono::system_clock::to_time_t(
        std::chrono::system_clock::time_point(std::chrono::seconds(
            duration_cast<std::chrono::seconds>(ts.time_since_epoch())
                .count()))));
}

template<typename Clock, typename Rep>
inline libc_types::f32 to_f32(std::chrono::time_point<Clock, Rep> const& dur)
{
    return std::chrono::duration_cast<seconds_f32>(dur.time_since_epoch())
        .count();
}

template<typename Rep, typename Ratio>
inline libc_types::f32 to_f32(std::chrono::duration<Rep, Ratio> const& dur)
{
    return std::chrono::duration_cast<seconds_f32>(dur).count();
}

} // namespace Chrono

using Timestamp = libc_types::u64;

} // namespace stl_types
