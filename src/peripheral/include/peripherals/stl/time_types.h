#pragma once

#include <chrono>
#include <ctime>

#include <peripherals/libc/time_ops.h>
#include <peripherals/libc/types.h>
#include <peripherals/stl/functional_types.h>
#include <peripherals/stl/standard_exceptions.h>

namespace stl_types {
namespace Chrono {
using namespace std::chrono;

using seconds_float  = duration<libc_types::scalar>;
using seconds_double = duration<libc_types::bigscalar>;

using Clock = Chrono::steady_clock;

inline time_point<Clock, Chrono::seconds> from_unix(libc_types::u64 ts)
{
    static_assert(
        type_safety::is_pod_v<std::time_t>, "std::time_t is not a POD type");

    std::time_t ts_unix = C_FCAST<std::time_t>(ts);
    auto        immTs   = Chrono::system_clock::from_time_t(ts_unix);

    return time_point<Clock, seconds>(
        seconds(duration_cast<seconds>(immTs.time_since_epoch()).count()));
}

inline libc_types::u64 to_unix(Clock::time_point ts)
{
    static_assert(
        type_safety::is_pod_v<std::time_t>, "std::time_t is not a POD type");

    return C_FCAST<libc_types::u64>(
        Chrono::system_clock::to_time_t(system_clock::time_point(
            seconds(duration_cast<seconds>(ts.time_since_epoch()).count()))));
}

template<typename Clock, typename Rep>
inline libc_types::f32 to_float(
    std::chrono::time_point<Clock, Rep> const& dur)
{
    return duration_cast<seconds_float>(dur.time_since_epoch()).count();
}

template<typename Rep, typename Ratio>
inline libc_types::f32 to_float(std::chrono::duration<Rep, Ratio> const& dur)
{
    return duration_cast<seconds_float>(dur).count();
}

} // namespace Chrono

using Timestamp = libc_types::u64;

} // namespace stl_types
