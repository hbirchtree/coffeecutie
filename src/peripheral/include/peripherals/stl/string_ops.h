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
#include <tinyutf8.h>

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

namespace find {

template<typename CharType>
FORCEDINLINE bool starts_with(
    std::basic_string_view<CharType> const& haystack,
    std::basic_string_view<CharType> const& needle)
{
    return haystack.find(needle) == 0;
}

template<typename CharType>
FORCEDINLINE bool ends_with(
    std::basic_string_view<CharType> const& haystack,
    std::basic_string_view<CharType> const& needle)
{
    if(haystack.size() < needle.size())
        return false;
    return haystack.find(needle) == (haystack.size() - needle.size());
}

} // namespace find

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
#if defined(COFFEE_MSVCXX) && \
    (__cplusplus >= 201703L || (defined(_HAS_CXX17) && _HAS_CXX17))
#define NOT_FN std::not_fn
#else
#define NOT_FN(s) std::not1(std::ptr_fun<int, int>(s))
#endif

template<typename CharType>
/* Reference:
 * http://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring
 */
FORCEDINLINE std::basic_string<CharType>& left(std::basic_string<CharType>& s)
{
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int c) {
                return !std::isspace(c);
            }));
    return s;
}

template<typename CharType>
FORCEDINLINE std::basic_string<CharType>& right(std::basic_string<CharType>& s)
{
    s.erase(
        std::find_if(
            s.rbegin(), s.rend(), [](int c) { return !std::isspace(c); })
            .base(),
        s.end());
    return s;
}

#undef NOT_FN

template<typename CharType>
FORCEDINLINE std::basic_string<CharType>& both(std::basic_string<CharType>& s)
{
    return left(right(s));
}

template<typename CharType>
FORCEDINLINE std::basic_string<CharType>& left_zero(
    std::basic_string<CharType>& s)
{
    s.erase(0, s.find_first_not_of('0'));
    return s;
}

template<typename CharType>
FORCEDINLINE std::basic_string<CharType>& right_zero(
    std::basic_string<CharType>& s)
{
    s.erase(s.find_last_not_of('0') + 1, s.size());
    return s;
}

template<typename CharType>
FORCEDINLINE std::basic_string<CharType>& both_zero(
    std::basic_string<CharType>& s)
{
    return left_zero(right_zero(s));
}

template<typename OutCharType, typename InCharType>
FORCEDINLINE std::basic_string<OutCharType> convertformat(
    typename std::basic_string<InCharType> const& input)
{
    return std::basic_string<OutCharType>(input.begin(), input.end());
}
} // namespace trim

namespace convert {

using std::to_string;

template<typename CharType = char>
FORCEDINLINE std::basic_string<CharType> hexify(u64 s, bool trim_zero = false)
{
    if(s == 0)
        return "0";

    u64 ss = s;

    std::basic_string<CharType> str;
    str.resize(C_CAST<size_t>(snprintf(nullptr, 0, "%llx", ss)) + 1);
    snprintf(&str[0], str.size() + 1, "%llx", ss);
    str.resize(str.size() - 1);
    trim::left_zero(str);
    if(trim_zero)
        trim::right_zero(str);
    return str;
}
} // namespace convert

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
} // namespace print

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
    if(query.size() == 0)
        return std::basic_string<CharType>(target.begin(), target.end());

    std::basic_string<CharType> out(target.begin(), target.end());
    for(size_t pos = 0;; pos += replacement.size())
    {
        pos = out.find(query, pos);
        if(pos == decltype(out)::npos)
            break;
        out.erase(pos, query.size());
        out.insert(pos, replacement);
    }
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
struct spliterator : Iterator<std::forward_iterator_tag, CharType>
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

    spliterator operator++(int) const
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
        return source.substr(idx, len());
    }

    bool operator!=(spliterator const& other) const
    {
        return idx != other.idx;
    }

  private:
    typename string_type::size_type len() const
    {
        auto it = next_idx();

        return it - idx;
    }

    typename string_type::size_type next_idx() const
    {
        return source.find(sep, idx + 1);
    }

    string_type                     source;
    sep_type                        sep;
    typename string_type::size_type idx;
};

template<typename CharType>
FORCEDINLINE quick_container<spliterator<CharType>> str(
    std::basic_string<CharType> const& source, CharType sep)
{
    using str_type = std::basic_string<CharType>;

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

} // namespace str
} // namespace stl_types

namespace b64 {

using namespace ::libc_types;

constexpr const char* b64_char =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

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
using ::stl_types::String;

FORCEDINLINE String encode(String const& from)
{
    return stl_types::str::print::hexdump(
        semantic::mem_chunk<const char>::ofContainer(from), false);
}

} // namespace hex
