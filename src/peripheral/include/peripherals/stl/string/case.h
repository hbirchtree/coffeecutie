#pragma once

#include <peripherals/identify/compiler/function_inlining.h>

#include <locale>
#include <string>

namespace stl_types::str::fmt {

template<typename CharT>
FORCEDINLINE std::basic_string<CharT> lower(
    typename std::basic_string<CharT> const& st)
{
    typename std::basic_string<CharT> o;
    o.reserve(st.size());
    for(typename std::basic_string<CharT>::value_type c : st)
        o.push_back(std::tolower(c));
    return o;
}

template<typename CharT>
FORCEDINLINE std::basic_string<CharT> upper(std::basic_string<CharT> const& st)
{
    std::basic_string<CharT> o;
    o.reserve(st.size());
    for(typename std::basic_string<CharT>::value_type c : st)
        o.push_back(std::toupper(c));
    return o;
}

template<typename CharT>
FORCEDINLINE std::basic_string<CharT> propercase(
    std::basic_string<CharT> const& input)
{
    std::basic_string<CharT> out;
    out.reserve(input.size());
    size_t i = 0;
    for(typename std::basic_string<CharT>::value_type c : input)
    {
        if((i++) < 1)
            out.push_back(std::toupper(c));
        else
            out.push_back(c);
    }
    return out;
}

} // namespace stl_types::str::fmt
