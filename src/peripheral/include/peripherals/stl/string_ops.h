#pragma once

#include <peripherals/base.h>

#include <algorithm>
#include <cctype>
#include <inttypes.h>

#ifdef COFFEE_USE_IOSTREAMS
#include <iomanip>
#endif

#include <peripherals/libc/types.h>
#include <peripherals/semantic/chunk.h>
#include <peripherals/stl/types.h>

namespace stl_types::str {

using libc_types::bigscalar;
using libc_types::byte_t;
using libc_types::c_cptr;
using libc_types::cstring;
using libc_types::i16;
using libc_types::i32;
using libc_types::i64;
using libc_types::i8;
using libc_types::lscalar;
using libc_types::scalar;
using libc_types::szptr;
using libc_types::u16;
using libc_types::u32;
using libc_types::u64;
using libc_types::u8;

namespace encode {
template<typename TargetCharT, typename T>
requires(!std::is_same_v<typename T::value_type, TargetCharT>)
    //
    FORCEDINLINE std::basic_string<TargetCharT> to(T const& from)
{
    // TODO: use proper conversions here, eg. iconv
    return std::basic_string<TargetCharT>(from.begin(), from.end());
}
} // namespace encode

template<typename CharType>
FORCEDINLINE std::basic_string<CharType> encapsulate(
    const CharType* src, size_t len)
{
    std::basic_string<CharType> target;
    target.insert(0, src, len);
    return target;
}

template<typename CharType>
FORCEDINLINE std::basic_string<CharType> encapsulate(const CharType* src)
{
    std::basic_string<CharType> target;
    target = src;
    return target;
}

template<typename CharType, typename CharType2 = CharType>
FORCEDINLINE std::basic_string_view<CharType> encapsulate_view(
    const CharType2* src, std::optional<size_t> num_chars = std::nullopt)
{
    auto casted_ptr = reinterpret_cast<const CharType*>(src);
    if(num_chars)
        return std::basic_string_view<CharType>(casted_ptr, *num_chars);
    else
        return std::basic_string_view<CharType>(casted_ptr);
}

template<typename CharType, class Container>
requires std::is_same_v<CharType, typename Container::value_type>
    //
    FORCEDINLINE std::basic_string_view<CharType> encapsulate_view(
        Container const& src)
{
    return std::basic_string_view<CharType>(std::begin(src), std::end(src));
}

template<typename CharType, class Container>
requires(!std::is_same_v<CharType, typename Container::value_type>)
    //
    FORCEDINLINE std::basic_string_view<CharType> encapsulate_view(
        Container const& src)
{
    auto adapter = semantic::mem_chunk<const CharType>::ofContainer(src).view;
    return std::basic_string_view<CharType>(adapter.data(), adapter.size());
}

namespace transform {

template<typename CharType>
FORCEDINLINE std::basic_string<CharType> printclean(
    std::basic_string<CharType> const& src_)
{
    std::basic_string<CharType> src = src_;

    src.erase(
        std::remove_if(
            src.begin(),
            src.end(),
            [](char c) { return !(std::isprint(c) || std::isspace(c)); }),
        src.end());

    return src;
}

template<typename CharType>
FORCEDINLINE std::basic_string<CharType> reverse(
    std::basic_string<CharType> const& src_)
{
    std::basic_string<CharType> src = src_;
    std::reverse(src.begin(), src.end());
    return src;
}

template<typename CharT>
FORCEDINLINE std::basic_string<CharT> multiply(CharT character, size_t num)
{
    return std::basic_string<CharT>(num, character);
}

} // namespace transform

} // namespace stl_types::str
