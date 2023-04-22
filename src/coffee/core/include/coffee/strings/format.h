#pragma once

#include <coffee/core/libc_types.h>
#include <peripherals/stl/types.h>
#include <peripherals/concepts/container.h>
#include <peripherals/concepts/string.h>
#include <peripherals/error/result.h>
#include <peripherals/stl/string_ops.h>

namespace Coffee {
namespace Strings {

template<typename T>
std::string to_string(std::optional<T> const& opt)
{
    if(opt.has_value())
        return "Optional(" + to_string(opt.value()) + ")";
    else
        return "Optional(nullptr)";
}

template<typename T, typename E>
std::string to_string(stl_types::result<T, E> const& opt)
{
    if(opt.has_value())
        return "result(" + to_string(opt.value()) + ")";
    else
        return "result(" + to_string(opt.error()) + ")";
}

template<typename Dummy>
requires false std::string to_string(Dummy const&)
{
    return {};
}

template<typename... Args>
std::string to_string_tuple_unpack(Args const&... args)
{
    return ((to_string(args) + ", ") + ...);
}

template<typename... Args>
std::string to_string(std::tuple<Args...> const& a)
{
    auto repr = std::apply(to_string_tuple_unpack<Args...>, a);
    return "tuple(" + repr + ")";
}

template<typename T1, typename T2>
std::string to_string(std::pair<T1, T2> const& a)
{
    return "pair(" + to_string(a.first) + ", " + to_string(a.second) + ")";
}

template<typename T>
requires(
    semantic::concepts::is_container<T> &&
    !semantic::concepts::is_string_container<T>)
    //
    std::string to_string(T const& container)
{
    std::string out;
    out.reserve(1024);
    for(auto const& item : container)
        out.append(to_string(item) + ", ");
    return "[ " + out + " ]";
}

/* Core string resolution */

template<typename... Arg>
FORCEDINLINE std::string cStringFormat(std::string fmt, Arg... args)
{
    size_t i = 0;

    ((fmt = stl_types::str::replace::str<char>(
          fmt, "{" + std::to_string(i++) + "}", to_string(args))),
     ...);

    return fmt;
}

template<typename... Arg>
FORCEDINLINE std::string fmt(cstring fmt, Arg... arg)
{
    return cStringFormat(fmt, arg...);
}

template<typename... Arg>
FORCEDINLINE std::string fmt(std::string_view fmt_, Arg... arg)
{
    return cStringFormat(std::string(fmt_.data(), fmt_.size()), arg...);
}

} // namespace Strings
} // namespace Coffee
