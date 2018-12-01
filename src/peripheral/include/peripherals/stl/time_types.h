#pragma once

#include <chrono>
#include <peripherals/libc/types.h>

namespace stl_types {
namespace Chrono {
using namespace std::chrono;

using seconds_float  = duration<libc_types::scalar>;
using seconds_double = duration<libc_types::bigscalar>;

} // namespace Chrono

template<typename Clock = Chrono::system_clock>
struct Time
{
    template<typename Period = Chrono::milliseconds>
    STATICINLINE typename Period::rep CurrentTimestamp()
    {
        return Chrono::duration_cast<Period>(
                    Clock::now().time_since_epoch())
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

using Timestamp = libc_types::u64;

} // namespace stl_types
