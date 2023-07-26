#pragma once

#include <peripherals/identify/compiler/function_inlining.h>
#include <peripherals/libc/types.h>

#include <algorithm>
#include <string>

namespace stl_types::str::trim {

template<class StrType>
/* Reference:
 * http://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring
 */
FORCEDINLINE StrType left(StrType const& s)
{
    auto start = std::find_if(
        s.begin(), s.end(), [](int c) { return !std::isspace(c); });
    return StrType(s.data() + (start - s.begin()), s.end() - start);
}

template<class StrType>
FORCEDINLINE StrType right(StrType const& s)
{
    auto end = std::find_if(s.rbegin(), s.rend(), [](int c) {
                   return !std::isspace(c);
               }).base();
    return StrType(s.data(), end - s.begin());
}

template<class StrType>
FORCEDINLINE StrType both(StrType const& s)
{
    return left(right(s));
}

template<class StrType>
FORCEDINLINE StrType left_zero(StrType const& s)
{
    auto end = s.find_first_not_of('0');
    return StrType(s.data() + end, s.size() - end);
}

template<class StrType>
FORCEDINLINE StrType right_zero(StrType const& s)
{
    auto end = s.find_first_not_of('0');
    return StrType(s.data(), end);
}

template<class StrType>
FORCEDINLINE StrType both_zero(StrType const& s)
{
    return left_zero(right_zero(s));
}

} // namespace trim
