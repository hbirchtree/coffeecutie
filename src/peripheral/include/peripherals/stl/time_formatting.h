#pragma once

#include <iomanip>
#include <sstream>

#include <peripherals/stl/time_types.h>

namespace stl_types {

template<typename Clock = std::chrono::system_clock>
struct Time
{
    template<typename Period = std::chrono::milliseconds>
    STATICINLINE typename Period::rep CurrentTimestamp()
    {
        return std::chrono::duration_cast<Period>(
                   Clock::now().time_since_epoch())
            .count();
    }

    STATICINLINE std::chrono::microseconds::rep Microsecond()
    {
        return CurrentTimestamp<std::chrono::microseconds>();
    }

    STATICINLINE std::chrono::microseconds::rep Millisecond()
    {
        return CurrentTimestamp<std::chrono::milliseconds>();
    }
};

template<typename Clock = std::chrono::system_clock, typename CharT = char>
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

} // namespace stl_types
