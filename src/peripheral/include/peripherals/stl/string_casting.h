#pragma once

#include <concepts>

#include <peripherals/libc/string_ops.h>

namespace stl_types {

template<typename T>
T cast_string(std::string const& val)
{
    return libc::str::from_string<T>(val.c_str());
}

template<typename T>
T cast_string(std::wstring const& val)
{
    return libc::str::from_string<T>(val.c_str());
}

template<typename T>
T cast_string_view(std::string_view val)
{
    return cast_string<T>(std::string(val.begin(), val.end()));
}

template<typename T>
requires(
    (std::is_integral<T>::value || std::is_floating_point<T>::value) &&
    !std::is_same_v<T, bool>)
    //
    std::string cast_pod(T v)
{
    return std::to_string(v);
}

template<typename T>
requires std::is_same_v<T, bool> std::string cast_pod(T v)
{
    return v ? "true" : "false";
}

} // namespace stl_types
