#pragma once

#include <peripherals/identify/compiler/function_inlining.h>
#include <peripherals/libc/types.h>

#include <string>

namespace stl_types::str::replace {
namespace detail {
template<typename CharType>
FORCEDINLINE std::basic_string<CharType> str_impl(
    std::basic_string_view<CharType> const& target,
    std::basic_string_view<CharType> const& query,
    std::basic_string_view<CharType> const& replacement)
{
    std::basic_string<CharType> out(target.begin(), target.end());
    if(query.empty())
        return out;

    for(size_t pos = 0; (pos = out.find(query, pos)) != decltype(out)::npos;
        pos += replacement.size())
        out.replace(pos, query.size(), replacement.data(), replacement.size());

    return out;
}

template<typename CharType>
FORCEDINLINE std::basic_string_view<CharType> as_view(
    std::basic_string_view<CharType> const& in)
{
    return in;
}

template<typename CharType, typename T>
requires std::is_same_v<T, std::basic_string<CharType>>
FORCEDINLINE std::basic_string_view<CharType> as_view(T const& in)
{
    return std::basic_string_view<CharType>(in.data(), in.size());
}
} // namespace detail

template<typename CharType>
FORCEDINLINE std::basic_string<CharType> str(
    auto const& target, auto const& query, auto const& replace)
{
    return detail::str_impl<CharType>(
        detail::as_view<CharType>(target),
        detail::as_view<CharType>(query),
        detail::as_view<CharType>(replace));
}

} // namespace stl_types::str::replace
