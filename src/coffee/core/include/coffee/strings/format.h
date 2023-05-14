#pragma once

#include <fmt_extensions/format.h>

namespace Coffee::Strings {

template<typename... Args>
auto cStringFormat(::fmt::format_string<Args...> fmt, Args&&... args)
{
    return ::fmt::format(fmt, args...);
}

template<typename... Args>
auto fmt(::fmt::format_string<Args...> fmt, Args&&... args)
{
    return ::fmt::format(fmt, std::forward<Args&&>(args)...);
}

}
