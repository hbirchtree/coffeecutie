#pragma once

#include <chrono>
#include <ctime>
#include <iomanip>
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
        std::is_pod<std::time_t>::value, "std::time_t is not a POD type");

    std::time_t ts_unix = C_FCAST<std::time_t>(ts);
    auto        immTs   = Chrono::system_clock::from_time_t(ts_unix);

    return time_point<Clock, seconds>(
        seconds(duration_cast<seconds>(immTs.time_since_epoch()).count()));
}

inline libc_types::u64 to_unix(Clock::time_point ts)
{
    static_assert(
        std::is_pod<std::time_t>::value, "std::time_t is not a POD type");

    return C_FCAST<libc_types::u64>(
        Chrono::system_clock::to_time_t(system_clock::time_point(
            seconds(duration_cast<seconds>(ts.time_since_epoch()).count()))));
}

template<class Timepoint>
inline libc_types::scalar to_float(Timepoint const& dur)
{
    return duration_cast<seconds_float>(
               time_point_cast<steady_clock::time_point>(dur)
                   .time_since_epoch())
        .count();
}

} // namespace Chrono

template<typename Clock = Chrono::system_clock>
struct Time
{
    template<typename Period = Chrono::milliseconds>
    STATICINLINE typename Period::rep CurrentTimestamp()
    {
        return Chrono::duration_cast<Period>(Clock::now().time_since_epoch())
            .count();
    }

    STATICINLINE Chrono::microseconds::rep Microsecond()
    {
        return CurrentTimestamp<Chrono::microseconds>();
    }

    STATICINLINE Chrono::microseconds::rep Millisecond()
    {
        return CurrentTimestamp<Chrono::milliseconds>();
    }
};

template<typename Clock = Chrono::system_clock, typename CharT = char>
struct TimeFormatter
{
    STATICINLINE typename std::basic_string<CharT> String(const CharT* fmt)
    {
        auto time = std::time(nullptr);

        std::basic_string<CharT> out;

#if __cplusplus >= 201403L
        auto                           localTime = std::localtime(&time);
        std::basic_stringstream<CharT> ss(out);
        ss << std::put_time<CharT>(localTime, fmt);
        out = ss.str();
#else
        out = libc::time::time_to_str(time, fmt);
#endif

        return out;
    }
};

using Timestamp = libc_types::u64;

} // namespace stl_types
