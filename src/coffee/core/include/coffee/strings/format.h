#pragma once

#include <coffee/core/libc_types.h>
#include <coffee/core/stl_types.h>
#include <peripherals/error/result.h>
#include <peripherals/stl/string_ops.h>

namespace Coffee {
namespace Strings {

template<typename T>
CString to_string(Optional<T> const& opt)
{
    if(opt.has_value())
        return "Optional(" + to_string(opt.value()) + ")";
    else
        return "Optional(nullptr)";
}

template<typename T, typename E>
CString to_string(stl_types::result<T, E> const& opt)
{
    if(opt.has_value())
        return "result(" + to_string(opt.value()) + ")";
    else
        return "result(" + to_string(opt.error()) + ")";
}

template<typename Dummy>
requires false CString to_string(Dummy const&)
{
    return {};
}

template<typename... Args>
CString to_string_tuple_unpack(Args const&... args)
{
    return ((to_string(args) + ", ") + ...);
}

template<typename... Args>
CString to_string(std::tuple<Args...> const& a)
{
    auto repr = std::apply(to_string_tuple_unpack<Args...>, a);
    return "tuple(" + repr + ")";
}

/* Core string resolution */

template<typename... Arg>
FORCEDINLINE CString cStringFormat(String fmt, Arg... args)
{
    size_t i = 0;

    ((fmt = stl_types::str::replace::str<char>(
         fmt, "{" + std::to_string(i++) + "}", to_string(args))),
     ...);

    return fmt;
}

template<typename... Arg>
FORCEDINLINE CString fmt(cstring fmt, Arg... arg)
{
    return cStringFormat(fmt, arg...);
}

template<typename... Arg>
FORCEDINLINE CString fmt(std::string_view fmt_, Arg... arg)
{
    return cStringFormat(String(fmt_.data(), fmt_.size()), arg...);
}

} // namespace Strings
} // namespace Coffee
