#pragma once

#include <peripherals/identify/compiler/function_inlining.h>
#include <peripherals/libc/types.h>

#include <string>

namespace stl_types::str::replace {
namespace detail {
template<typename CharType>
FORCEDINLINE std::basic_string<CharType> str_impl(
    std::basic_string_view<CharType> const& target,
    std::basic_string_view<CharType> const& /*query*/,
    std::basic_string_view<CharType> const& /*replacement*/)
{
    //    if(query.size() == 0)
    //        return std::basic_string<CharType>(target.begin(), target.end());

    //    std::basic_string<CharType> out(target.begin(), target.end());
    //    for(size_t pos = 0;; pos += replacement.size())
    //    {
    //        pos = out.find(query, pos);
    //        if(pos == decltype(out)::npos || pos >= out.size())
    //            break;
    //        out.erase(pos, query.size());
    //        out.insert(out.begin() + pos, replacement.begin(),
    //        replacement.end()); out.replace();
    //    }
    std::basic_string<CharType> out(target.begin(), target.end());
    // TODO: FIX
    //    if(query.empty())
    //        return out;
    //    for(size_t pos = 0;; pos += replacement.size())
    //    {
    //        pos = out.find(query, pos);
    //        if(pos == decltype(out)::npos)
    //            break;
    //        out.replace(pos, query.size(), replacement.data(),
    //        replacement.size());
    //    }
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

// template<typename CharType>
// FORCEDINLINE std::basic_string<CharType> str(
//     std::basic_string<CharType> const& target,
//     std::basic_string<CharType> const& query,
//     std::basic_string<CharType> const& replace)
//{
//     return str_impl<CharType>(
//         target,
//         std::basic_string_view<CharType>(query.begin(), query.end()),
//         std::basic_string_view<CharType>(replace.begin(), replace.end()));
// }

} // namespace stl_types::str::replace
