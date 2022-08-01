#pragma once

#include <iomanip>
#include <sstream>

#include <peripherals/stl/time_types.h>

namespace stl_types {

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

}
