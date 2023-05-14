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

#include <cppcodec/base64_default_rfc4648.hpp>
#include <tinyutf8/tinyutf8.h>

namespace stl_types {
namespace str {

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
requires (!std::is_same_v<CharType, typename Container::value_type>)
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

namespace trim {

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

namespace convert {

using std::to_string;

template<typename CharType = char>
FORCEDINLINE std::basic_string<CharType> hexify(u64 s)
{
    if(s == 0)
        return "0";

    long long unsigned int ss = s;

    std::basic_string<CharType> str;
    str.resize(C_CAST<size_t>(snprintf(nullptr, 0, "%llx", ss)) + 1);
    snprintf(&str[0], str.size() + 1, "%llx", ss);
    str.resize(str.size() - 1);
    trim::left_zero(str);
    return str;
}
} // namespace convert

namespace pad {
template<typename T>
FORCEDINLINE std::basic_string<T>& left(
    std::basic_string<T>& s, T ch, szptr len)
{
    if(len > s.size())
        s.insert(s.begin(), len - s.size(), ch);
    return s;
}

template<typename T>
FORCEDINLINE std::basic_string<T>& right(
    std::basic_string<T>& s, T ch, szptr len)
{
    if(len > s.size())
        s.insert(s.end(), len - s.size(), ch);
    return s;
}

template<typename T>
FORCEDINLINE std::basic_string<T> left(
    std::basic_string<T> const& s, T ch, szptr len)
{
    auto s_copy = s;
    if(len > s_copy.size())
        s_copy.insert(s_copy.begin(), len - s_copy.size(), ch);
    return s_copy;
}

template<typename T>
FORCEDINLINE std::basic_string<T> right(
    std::basic_string<T> const& s, T ch, szptr len)
{
    auto s_copy = s;
    if(len > s_copy.size())
        s_copy.insert(s_copy.end(), len - s_copy.size(), ch);
    return s_copy;
}
} // namespace pad

namespace print {

template<typename CharType = char>
FORCEDINLINE std::basic_string<CharType> pointer_pad(u64 ptr, u32 pad = 0)
{
    return pad::left<CharType>(convert::hexify(ptr), '0', pad);
}

} // namespace print

namespace fmt {
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
    szptr i = 0;
    for(typename std::basic_string<CharT>::value_type c : input)
    {
        if((i++) < 1)
            out.push_back(std::toupper(c));
        else
            out.push_back(c);
    }
    return out;
}
} // namespace fmt

namespace replace {
namespace detail {
template<typename CharType>
FORCEDINLINE std::basic_string<CharType> str_impl(
    std::basic_string_view<CharType> const& target,
    std::basic_string_view<CharType> const& query,
    std::basic_string_view<CharType> const& replacement)
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
//        out.insert(out.begin() + pos, replacement.begin(), replacement.end());
//        out.replace();
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
//        out.replace(pos, query.size(), replacement.data(), replacement.size());
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

} // namespace replace

namespace split {

template<typename CharType>
struct spliterator
    : Iterator<std::forward_iterator_tag, std::basic_string_view<CharType>>
{
    using string_type = std::basic_string_view<CharType>;
    using sep_type    = CharType;

    spliterator() : source(), sep(sep_type()), idx(string_type::npos)
    {
    }

    spliterator(string_type const& source, sep_type sep) :
        source(source), sep(sep), idx(0)
    {
    }

    [[nodiscard]] spliterator operator++(int) const
    {
        auto cpy = *this;
        cpy.idx  = next_idx();

        if(cpy.idx > source.size())
            cpy.idx = string_type::npos;

        return cpy;
    }

    spliterator& operator++()
    {
        idx = next_idx();

        if(idx > source.size())
            idx = string_type::npos;

        return *this;
    }

    string_type operator*() const
    {
        u8 adjustment = idx != 0 ? 1 : 0;
        return source.substr(idx + adjustment, len() - adjustment);
    }

    bool operator!=(spliterator const& other) const
    {
        return idx != other.idx;
    }

    operator string_type() const
    {
        return *(*this);
    }

  private:
    typename string_type::size_type len() const
    {
        return next_idx() - idx;
    }

    typename string_type::size_type next_idx() const
    {
        auto next = source.find(sep, idx + 1);
        return next;
    }

    string_type                     source;
    sep_type                        sep;
    typename string_type::size_type idx;
};

template<typename CharType>
FORCEDINLINE quick_container<spliterator<CharType>> str(
    std::basic_string_view<CharType> const& source, CharType sep)
{
    using str_type = std::basic_string_view<CharType>;

    auto it = source.find(sep);

    if(it == str_type::npos)
        return quick_container<spliterator<CharType>>(
            []() { return spliterator<CharType>(); },
            []() { return spliterator<CharType>(); });

    return quick_container<spliterator<CharType>>(
        [&]() { return spliterator<CharType>(source, sep); },
        []() { return spliterator<CharType>(); });
}

} // namespace split

namespace print {
template<typename CharType = char>
FORCEDINLINE std::basic_string<CharType> pointerify(u64 const& ptr)
{
    return "0x" + convert::hexify(ptr);
}
template<
    typename T,
    typename CharType                                               = char,
    typename std::enable_if<std::is_pointer<T>::value, bool>::type* = nullptr>
FORCEDINLINE std::basic_string<CharType> pointerify(T ptr)
{
    return pointerify(C_FCAST<u64>(ptr));
}

template<typename CharType = char>
FORCEDINLINE std::basic_string<CharType> hexdump(
    semantic::mem_chunk<const char> const& data,
    bool                                   spacing      = true,
    szptr                                  newline_freq = 0)
{
    std::basic_string<CharType> out;
    out.reserve(data.size * 2 /* Hexadec */ + data.size * spacing /* Space */);

    for(szptr i = 0; i < data.size; i++)
    {
        out.append("00");
        sprintf(&out[out.size() - 2], "%02x", data[i]);
        if(newline_freq == 0 || (i + 1) % newline_freq != 0)
        {
            if(spacing)
                out.append(" ");
        } else
            out.append("\n");
    }

    return out;
}

template<typename CharType = char>
FORCEDINLINE std::basic_string<CharType> indent(
    std::basic_string_view<CharType> const& in,
    std::basic_string_view<CharType> const& padding)
{
    std::basic_string<CharType> out = replace::str(in, "\n", padding);
    out.insert(out.begin(), padding.begin(), padding.end());
    return out;
}

} // namespace print

} // namespace str
} // namespace stl_types

namespace b64 {

using namespace ::libc_types;

constexpr const char* b64_char
    = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

/* Reference:
 * https://en.wikibooks.org/wiki/Algorithm_Implementation/Miscellaneous/Base64
 */
template<typename T>
FORCEDINLINE std::string encode(semantic::Span<T> const& data_)
{
    auto converted = C_OCAST<semantic::Span<u8 const>>(data_);
    return base64::encode(converted.data(), converted.size());
}

} // namespace b64

namespace hex {

using namespace ::libc_types;

FORCEDINLINE std::string encode(std::string const& from)
{
    return stl_types::str::print::hexdump(
        semantic::mem_chunk<const char>::ofContainer(from), false);
}

} // namespace hex
