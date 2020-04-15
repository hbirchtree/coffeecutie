#pragma once

#include <coffee/core/base.h>
#include <peripherals/stl/string_casting.h>
#include <peripherals/stl/type_safety.h>
#include <peripherals/stl/types.h>

#if C_HAS_INCLUDE(<string_view>)
#include <string_view>
#endif

namespace Coffee {
namespace Strings {

template<
    typename CharT,
    typename TargetChar,
    typename UnwrappedT = typename std::remove_const<
        typename std::remove_pointer<CharT>::type>::type>
struct is_cstring
{
    static constexpr bool value = std::is_same<UnwrappedT, TargetChar>::value;
};

template<
    typename T,
    typename std::enable_if<is_cstring<T, char>::value>::type* = nullptr>
inline CString to_string(T* v)
{
    CString out;
    if(v)
        out = v;
    else
        out = "0x0";
    return out;
}

template<
    typename T,
    typename std::enable_if<is_cstring<T, wchar_t>::value>::type* = nullptr>
inline CString to_string(T* v)
{
    CString out;
    if(v)
    {
        CWString out_w = v;
        out            = CString(out_w.begin(), out_w.end());
    } else
        out = "0x0";
    return out;
}

template<
    typename T,
    typename std::enable_if<
        std::is_pointer<T>::value && !is_cstring<T, char>::value &&
        !is_cstring<T, wchar_t>::value>::type* = nullptr>
inline CString to_string(T const& v)
{
    return str::print::pointerify(v);
}

template<
    typename T,
    typename std::enable_if<std::is_same<T, std::nullptr_t>::value>::type* =
        nullptr>
inline CString to_string(T const&)
{
    return "0x0";
}

template<
    typename T,
    typename std::enable_if<std::is_same<T, bool>::value>::type* = nullptr>
inline cstring to_string(T const& v)
{
    return (v) ? "true" : "false";
}

template<template<typename CharT> class StringType, typename CharT>
inline CString to_string(StringType<CharT> const& v)
{
    return CString(v.begin(), v.end());
}

#if C_HAS_INCLUDE(<string_view>)
template<typename CharT>
inline CString to_string(std::basic_string_view<CharT> const& v)
{
    if(v.empty())
        return {};
    return CString(v.data(), v.length());
}
#endif

template<
    typename T,
    typename std::enable_if<
        std::is_integral<T>::value && !std::is_same<T, bool>::value>::type* =
        nullptr>
inline CString to_string(const T& v)
{
    return cast_pod<T>(v);
}

template<
    typename T,
    typename std::enable_if<std::is_convertible<T, CString>::value>::type* =
        nullptr,
    typename std::enable_if<!std::is_pointer<T>::value>::type* = nullptr,
    typename std::enable_if<!std::is_same<T, std::nullptr_t>::value>::type* =
        nullptr>
inline CString to_string(T const& value)
{
    return static_cast<CString>(value);
}

inline CString to_string(const scalar& v)
{
    CString out = str::convert::to_string<scalar>(v);
    return str::trim::right_zero(out);
}
inline CString to_string(const bigscalar& v)
{
    CString out = str::convert::to_string<bigscalar>(v);
    return str::trim::right_zero(out);
}

template<typename... Arg>
CString cStringFormat(cstring fmt, Arg... args);

inline CString cStringResolve(CString const& fmt, size_t)
{
    return fmt;
}

} // namespace Strings
} // namespace Coffee
