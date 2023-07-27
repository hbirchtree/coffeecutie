#pragma once

#include <optional>
#include <peripherals/libc/types.h>
#include <string>

namespace platform::info::wmi {
namespace detail {
std::optional<std::string> query(
    std::string const& query, std::wstring const& property);
}

template<typename T>
requires std::is_same_v<T, std::string>
auto query(std::string const& query, std::wstring const& property)
{
    return detail::query(query, property);
}

template<typename T>
requires std::is_same_v<T, libc_types::u32>
    //
    std::optional<libc_types::u32> query(
        std::string const& query, std::wstring const& property)
{
    if(auto v = detail::query(query, property))
        return std::stoi(v.value());
    else
        return std::nullopt;
}

} // namespace platform::win32::wmi