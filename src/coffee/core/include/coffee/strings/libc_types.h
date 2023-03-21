#pragma once

#include <peripherals/libc/types.h>
#include <peripherals/stl/string_casting.h>
#include <peripherals/stl/string_ops.h>
#include <peripherals/stl/types.h>

#if C_HAS_INCLUDE(<string_view>)
#include <string_view>
#endif

namespace Coffee::Strings {

template<typename... Arg>
CString fmt(std::string_view fmt, Arg... arg);

template<typename CharT, typename TargetChar>
constexpr auto is_cstring_v
    = std::is_same_v<std::decay_t<std::remove_pointer_t<CharT>>, TargetChar>;

template<typename T>
requires(
    std::is_pointer_v<T> && !is_cstring_v<T, char> && !is_cstring_v<T, wchar_t>)
    //
    inline String to_string(T const& v)
{
    return stl_types::str::print::pointerify(v);
}

template<typename T>
requires std::is_same_v<T, std::nullptr_t>
inline CString to_string(T const&)
{
    return "0x0";
}

template<typename T>
requires std::is_same_v<T, bool>
inline std::string_view to_string(T const& v)
{
    return v ? "true" : "false";
}

template<typename CharT>
inline CString to_string(std::basic_string<CharT> const& v)
{
    return CString(v.begin(), v.end());
}

#if C_HAS_INCLUDE(<string_view>)
template<typename CharT>
inline CString to_string(std::basic_string_view<CharT> const& v)
{
    if(v.empty())
        return {};
    return CString(v.begin(), v.end());
}
#endif

template<typename T>
requires((std::is_integral_v<T> || std::is_floating_point_v<T>)&&!std::
             is_same_v<T, bool>)
    //
    inline CString to_string(const T& v)
{
    return cast_pod<T>(v);
}

template<typename T>
requires std::is_convertible_v<T, String>
inline String to_string(T const& value)
{
    return static_cast<CString>(value);
}

} // namespace Coffee::Strings
