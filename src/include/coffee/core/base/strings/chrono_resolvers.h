#pragma once

#include <coffee/core/plat/memory/stlstring_ops.h>
#include <peripherals/stl/types.h>

namespace Coffee {
namespace Strings {

template<
    typename Rep,
    typename Ratio,
    typename std::enable_if<
        std::is_signed<Rep>::value && std::is_integral<Rep>::value>::type* =
        nullptr>
inline CString to_string(std::chrono::duration<Rep, Ratio> const& duration)
{
    using namespace str::convert;

    CString out = "duration(";

    ((out += "rep=") += typeid(Rep).name()) += ", ";
    ((out += "ratio=") += to_string(Ratio::num)) += "/";
    (out += str::convert::to_string(Ratio::den)) += ",";
    (out += "dur=") += to_string(duration.count());

    return out + ")";
}

} // namespace Strings
} // namespace Coffee
