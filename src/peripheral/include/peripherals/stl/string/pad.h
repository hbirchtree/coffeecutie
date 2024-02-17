#pragma once

#include <peripherals/identify/compiler/function_inlining.h>

#include <string>

namespace stl_types::str::pad {
template<typename T>
FORCEDINLINE std::basic_string<T>& left(
    std::basic_string<T>& s, T ch, size_t len)
{
    if(len > s.size())
        s.insert(s.begin(), len - s.size(), ch);
    return s;
}

template<typename T>
FORCEDINLINE std::basic_string<T>& right(
    std::basic_string<T>& s, T ch, size_t len)
{
    if(len > s.size())
        s.insert(s.end(), len - s.size(), ch);
    return s;
}

template<typename T>
FORCEDINLINE std::basic_string<T> left(
    std::basic_string<T> const& s, T ch, size_t len)
{
    auto s_copy = s;
    if(len > s_copy.size())
        s_copy.insert(s_copy.begin(), len - s_copy.size(), ch);
    return s_copy;
}

template<typename T>
FORCEDINLINE std::basic_string<T> right(
    std::basic_string<T> const& s, T ch, size_t len)
{
    auto s_copy = s;
    if(len > s_copy.size())
        s_copy.insert(s_copy.end(), len - s_copy.size(), ch);
    return s_copy;
}
} // namespace stl_types::str::pad
