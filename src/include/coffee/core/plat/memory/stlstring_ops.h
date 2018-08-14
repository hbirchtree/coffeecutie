#pragma once

#include "../plat_quirks_toggling.h"

#include <algorithm>
#include <cctype>
#include <inttypes.h>

#ifdef COFFEE_USE_IOSTREAMS
#include <iomanip>
#endif

#include "../../coffee_assert_macros.h"
#include "../../coffee_mem_macros.h"
#include "../../types/tdef/integertypes.h"
#include "../../types/tdef/stltypes.h"

namespace Coffee {

namespace str {

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
    const CharType* src, szptr len)
{
    std::basic_string<CharType> target;
    target.insert(0, src, len);
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
FORCEDINLINE std::basic_string<CharT> multiply(CharT character, szptr num)
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
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), NOT_FN(std::isspace)));
    return s;
}

template<typename CharType>
FORCEDINLINE std::basic_string<CharType>& right(std::basic_string<CharType>& s)
{
    s.erase(
        std::find_if(s.rbegin(), s.rend(), NOT_FN(std::isspace)).base(),
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
}

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
        typename std::enable_if<std::is_same<FType, ftype>::value>::type* =   \
            nullptr>                                                          \
    FORCEDINLINE CString to_string(FType s)                                   \
    {                                                                         \
        CString str;                                                          \
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
        typename std::enable_if<std::is_same<IType, itype>::value>::type* = \
            nullptr>                                                        \
    FORCEDINLINE CString to_string(IType s)                                 \
    {                                                                       \
        if(s == 0)                                                          \
            return "0";                                                     \
        itype   ss = s;                                                     \
        CString str;                                                        \
        str.resize(C_CAST<size_t>(snprintf(nullptr, 0, fmt, ss)) + 1);      \
        snprintf(&str[0], str.size() + 1, fmt, ss);                         \
        str.resize(str.size() - 1);                                         \
        return str;                                                         \
    }

#define INTEGER_COERCE_CONVERT(target_type, coerce_type)                      \
    template<                                                                 \
        typename IType,                                                       \
        typename TargetType = target_type,                                    \
        typename CoerceType = coerce_type,                                    \
        typename std::enable_if<                                              \
            std::is_same<IType, target_type>::value &&                        \
            !std::is_same<target_type, coerce_type>::value>::type* = nullptr> \
    FORCEDINLINE CString to_string(IType s)                                   \
    {                                                                         \
        return to_string<coerce_type>(C_FCAST<coerce_type>(s));               \
    }

// INTEGER_CONVERT(size_t, fmt_size_t_fmt)

INTEGER_CONVERT(u64, u64_fmt)
INTEGER_CONVERT(u32, u32_fmt)
INTEGER_CONVERT(u16, u16_fmt)
INTEGER_CONVERT(u8, u8_fmt)

INTEGER_CONVERT(i64, i64_fmt)
INTEGER_CONVERT(i32, i32_fmt)
INTEGER_CONVERT(i16, i16_fmt)
INTEGER_CONVERT(i8, i8_fmt)

INTEGER_COERCE_CONVERT(long long, i64)
INTEGER_COERCE_CONVERT(unsigned long long, u64)

#if defined(COFFEE_WINDOWS)
INTEGER_CONVERT(unsigned long, u32_fmt)
#else
INTEGER_COERCE_CONVERT(unsigned long, u64)
INTEGER_COERCE_CONVERT(long, i64)
#endif

#undef INTEGER_COERCE_CONVERT
#undef INTEGER_CONVERT

template<
    typename IType,
    typename std::enable_if<std::is_same<IType, bool>::value>::type* = nullptr>
FORCEDINLINE cstring to_string(bool i)
{
    return (i) ? "true" : "false";
}

FORCEDINLINE CString hexify(u64 s, bool trim_zero = false)
{
    if(s == 0)
        return "0";

    u64 ss = s;

    CString str;
    str.resize(C_CAST<size_t>(snprintf(nullptr, 0, x64_fmt, ss)) + 1);
    snprintf(&str[0], str.size() + 1, x64_fmt, ss);
    str.resize(str.size() - 1);
    trim::left_zero(str);
    if(trim_zero)
        trim::right_zero(str);
    return str;
}
}

namespace print {
FORCEDINLINE CString pointerify(u64 const& ptr)
{
#ifndef COFFEE_LOWFAT
    return "0x" + convert::hexify(ptr);
#else
    return {};
#endif
}
template<
    typename T,
    typename std::enable_if<std::is_pointer<T>::value, bool>::type* = nullptr>
FORCEDINLINE CString pointerify(T ptr)
{
    return pointerify(C_FCAST<u64>(ptr));
}

FORCEDINLINE CString
             hexdump(c_cptr ptr, szptr len, bool spacing = true, szptr newline_freq = 0)
{
    byte_t const* data = C_RCAST<byte_t const*>(ptr);
    CString       out;
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
} // namespace str

namespace b64 {
const constexpr cstring b64_char =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

/* Reference:
 * https://en.wikibooks.org/wiki/Algorithm_Implementation/Miscellaneous/Base64
 */
FORCEDINLINE CString encode(c_cptr ptr, szptr len)
{
    CString out;
    out.reserve(len / 3 + len % 3 > 0);

    byte_t const* data = C_CAST<byte_t const*>(ptr);

    u32   temp;
    szptr j = 0;
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

FORCEDINLINE bool decode(byte_t const* i_ptr, szptr i_len, Vector<byte_t>* out)
{
    /*TODO: Implement Base64 decoding*/
    C_UNUSED(i_ptr);
    C_UNUSED(i_len);
    C_UNUSED(out);
    return false;
}
} // namespace b64

} // namespace Coffee
