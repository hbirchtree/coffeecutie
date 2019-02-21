#pragma once

#include <peripherals/identify/identify.h>
#include <peripherals/libc/string_ops.h>
#include <peripherals/stl/types.h>

#include <ctime>

namespace libc {
namespace time {
namespace detail {
using std::time_t;

template<
    typename CharT,
    size_t(*FTime)(CharT*, size_t, const CharT*, std::tm const*),
    CharT FillChar>
FORCEDINLINE std::basic_string<CharT> time_to_str(
    time_t const& tim, const CharT* fmt)
{
    auto                     localTime = std::localtime(&tim);
    std::basic_string<CharT> out;
    out.resize(str::len(fmt) * 10, FillChar);
    out.resize(FTime(&out[0], out.size(), fmt, localTime));
    return out;
}
} // namespace detail

template<
    typename CharT,
    typename std::enable_if<std::is_same<CharT, char>::value>::type* = nullptr>
FORCEDINLINE std::basic_string<CharT> time_to_str(
    detail::time_t const& tim, const CharT* fmt)
{
    return detail::time_to_str<CharT, strftime, ' '>(tim, fmt);
}

template<
    typename CharT,
    typename std::enable_if<std::is_same<CharT, wchar_t>::value>::type* =
        nullptr>
FORCEDINLINE std::basic_string<CharT> time_to_str(
    detail::time_t const& tim, const CharT* fmt)
{
    return detail::time_to_str<CharT, wcsftime, L' '>(tim, fmt);
}

} // namespace time
} // namespace libc
