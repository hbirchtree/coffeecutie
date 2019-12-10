#pragma once

#include <peripherals/base.h>

#include <algorithm>
#include <cctype>
#include <inttypes.h>

#ifdef COFFEE_USE_IOSTREAMS
#include <iomanip>
#endif

#include <peripherals/libc/types.h>
#include <peripherals/stl/types.h>

namespace stl_types {
namespace str {

using libc_types::szptr;
using libc_types::cstring;
using libc_types::lscalar;
using libc_types::bigscalar;
using libc_types::scalar;
using libc_types::u64;
using libc_types::u32;
using libc_types::u16;
using libc_types::u8;
using libc_types::i64;
using libc_types::i32;
using libc_types::i16;
using libc_types::i8;
using libc_types::c_cptr;
using libc_types::byte_t;

namespace encode {
template<
    typename TargetCharT,
    typename SourceCharT,
    typename std::enable_if<
        !std::is_same<SourceCharT, TargetCharT>::value>::type* = nullptr>
FORCEDINLINE std::basic_string<TargetCharT> to(
    std::basic_string<SourceCharT> const& from)
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
    std::basic_string<CharT> out;
    return out.insert(0, num, character);
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
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int c) {return !std::isspace(c);}));
    return s;
}

template<typename CharType>
FORCEDINLINE std::basic_string<CharType>& right(std::basic_string<CharType>& s)
{
    s.erase(
        std::find_if(s.rbegin(), s.rend(), [](int c) {return !std::isspace(c);}).base(),
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
#define FMT_TYPE const constexpr cstring
#if defined(PRIu8a)
#define FMT_STR(bits, fmt) FMT_TYPE fmt##bits##_fmt = "%" PRI##fmt##bits
#define FMT_PAIR(bits) \
    FMT_STR(bits, u);  \
    FMT_STR(bits, i);  \
    FMT_STR(bits, x);

FMT_PAIR(8)
FMT_PAIR(16)
FMT_PAIR(32)
FMT_PAIR(64)
#else
#define FMT_STR(bits, fmt, prefix) FMT_TYPE fmt##bits##_fmt = "%" prefix #fmt

#define FMT_PAIR(bits, prefix) \
    FMT_STR(bits, u, prefix);  \
    FMT_STR(bits, i, prefix);  \
    FMT_STR(bits, x, prefix);

FMT_PAIR(8, "hh")
FMT_PAIR(16, "h")
FMT_PAIR(32, "")

#if defined(COFFEE_ARCH_LLP64)
FMT_PAIR(64, "ll")
#else
FMT_PAIR(64, "l")
#endif
#endif

FMT_TYPE fmt_size_t_fmt = "%zu";

#undef FMT_TYPE
#undef FMT_STR
#undef FMT_PAIR

/*
 * In the below functions we remove the null-terminator from the strings.
 * It causes a literal NULL to appear in the strings, which is bad, m'kay?
 *
 */

#define SCALAR_CONVERT(ftype, fmt)                                            \
    template<                                                                 \
        typename FType,                                                       \
        typename TargetType = ftype,                                          \
        typename CharType   = char,                                           \
        typename std::enable_if<std::is_same<FType, ftype>::value>::type* =   \
            nullptr>                                                          \
    FORCEDINLINE std::basic_string<CharType> to_string(FType s)               \
    {                                                                         \
        std::basic_string<CharType> str;                                      \
        str.resize(                                                           \
            C_CAST<size_t>(snprintf(nullptr, 0, fmt, C_CAST<ftype>(s))) + 1); \
        snprintf(&str[0], str.size() + 1, fmt, C_CAST<ftype>(s));             \
        str.resize(str.size() - 1);                                           \
        return str;                                                           \
    }

/* Floating-point conversion */
SCALAR_CONVERT(lscalar, "%Lf")
SCALAR_CONVERT(bigscalar, "%f")
SCALAR_CONVERT(scalar, "%f")

#undef SCALAR_CONVERT

/* Unsigned integer conversion */
#define INTEGER_CONVERT(itype, fmt)                                         \
    template<                                                               \
        typename IType,                                                     \
        typename TargetType = itype,                                        \
        typename CharType   = char,                                         \
        typename std::enable_if<                                            \
            type_safety::is_similar<IType, itype>::value>::type* = nullptr> \
    FORCEDINLINE std::basic_string<CharType> to_string(IType s)             \
    {                                                                       \
        if(s == 0)                                                          \
            return "0";                                                     \
        itype                       ss = s;                                 \
        std::basic_string<CharType> str;                                    \
        str.resize(C_CAST<size_t>(snprintf(nullptr, 0, fmt, ss)) + 1);      \
        snprintf(&str[0], str.size() + 1, fmt, ss);                         \
        str.resize(str.size() - 1);                                         \
        return str;                                                         \
    }

INTEGER_CONVERT(u64, u64_fmt)
INTEGER_CONVERT(u32, u32_fmt)
INTEGER_CONVERT(u16, u16_fmt)
INTEGER_CONVERT(u8, u8_fmt)

INTEGER_CONVERT(i64, i64_fmt)
INTEGER_CONVERT(i32, i32_fmt)
INTEGER_CONVERT(i16, i16_fmt)
INTEGER_CONVERT(i8, i8_fmt)

#undef INTEGER_CONVERT

template<
    typename IType,
    typename std::enable_if<std::is_same<IType, bool>::value>::type* = nullptr>
FORCEDINLINE cstring to_string(bool i)
{
    return (i) ? "true" : "false";
}

template<typename CharType = char>
FORCEDINLINE std::basic_string<CharType> hexify(u64 s, bool trim_zero = false)
{
    if(s == 0)
        return "0";

    u64 ss = s;

    std::basic_string<CharType> str;
    str.resize(C_CAST<size_t>(snprintf(nullptr, 0, x64_fmt, ss)) + 1);
    snprintf(&str[0], str.size() + 1, x64_fmt, ss);
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
    c_cptr ptr, szptr len, bool spacing = true, szptr newline_freq = 0)
{
    byte_t const*               data = C_RCAST<byte_t const*>(ptr);
    std::basic_string<CharType> out;
    out.reserve(len * 2 /* Hexadec */ + len * spacing /* Space */);

    for(szptr i = 0; i < len; i++)
    {
        out.append("  ");
        sprintf(&out[i * 2 + i], "%02x", data[i]);
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
template<typename CharType>
FORCEDINLINE std::basic_string<CharType> str_impl(
    std::basic_string<CharType> const& target,
    std::basic_string<CharType> const& query,
    std::basic_string<CharType> const& replacement)
{
    if(query.size() == 0)
        return target;

    std::basic_string<CharType> out = target;
    for(size_t pos = 0;; pos += replacement.size())
    {
        pos = out.find(query, pos);
        if(pos == std::basic_string<CharType>::npos)
            break;
        out.erase(pos, query.size());
        out.insert(pos, replacement);
    }
    return out;
}

template<typename CharType>
FORCEDINLINE std::basic_string<CharType> str(
    std::basic_string<CharType> const& target,
    std::basic_string<CharType> const& query,
    std::basic_string<CharType> const& replacement)
{
    return str_impl<CharType>(target, query, replacement);
}

template<typename CharType>
FORCEDINLINE std::basic_string<CharType> str(
    std::basic_string<CharType> const& target,
    const CharType*                    query,
    const CharType*                    replacement)
{
    return str_impl<CharType>(target, query, replacement);
}

} // namespace replace

namespace split {

template<typename CharType>
struct spliterator : Iterator<std::forward_iterator_tag, CharType>
{
    using string_type = std::basic_string<CharType>;
    using sep_type    = CharType;

    spliterator() : source(nullptr), sep(sep_type()), idx(string_type::npos)
    {
    }

    spliterator(string_type const& source, sep_type sep) :
        source(&source), sep(sep), idx(0)
    {
    }

    spliterator operator++() const
    {
        auto cpy = *this;
        cpy.idx = next_idx();

        if(cpy.idx > source->size())
            cpy.idx = string_type::npos;

        return cpy;
    }

    spliterator& operator++(int)
    {
        idx = next_idx();

        if(idx > source->size())
            idx = string_type::npos;

        return *this;
    }

    string_type operator*() const
    {
        return source->substr(idx, len());
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
        return source->find(sep, idx + 1);
    }

    string_type const*              source;
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
template<typename CharType = char>
FORCEDINLINE std::basic_string<CharType> encode(c_cptr ptr, szptr len)
{
    std::basic_string<CharType> out;
    out.reserve(len / 3 + len % 3 > 0);

    char const* data = C_CAST<char const*>(ptr);

    u32    temp;
    size_t j = 0;
    for(szptr i = 0; i < len / 3; i++)
    {
        temp = C_CAST<u32>((data[j++]) << 16);
        temp += C_CAST<u32>((data[j++]) << 8);
        temp += (data[j++]);
        out.append(1, b64_char[(temp & 0x00FC0000) >> 18]);
        out.append(1, b64_char[(temp & 0x0003F000) >> 12]);
        out.append(1, b64_char[(temp & 0x00000FC0) >> 6]);
        out.append(1, b64_char[(temp & 0x0000003F)]);
    }

    switch(len % 3)
    {
    case 1:
        temp = (data[len - (len % 3) + 1]);
        out.append(1, b64_char[(temp & 0x00FC0000) >> 18]);
        out.append(1, b64_char[(temp & 0x0003F000) >> 12]);
        out.append(1, '=');
        out.append(1, '=');
        break;
    case 2:
        temp = (data[len - (len % 3) + 1]);
        out.append(1, b64_char[(temp & 0x00FC0000) >> 18]);
        out.append(1, b64_char[(temp & 0x0003F000) >> 12]);
        out.append(1, b64_char[(temp & 0x00000FC0) >> 6]);
        out.append(1, '=');
        break;
    }

    return out;
}

} // namespace b64
