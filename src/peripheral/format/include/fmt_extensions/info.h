#pragma once

#include <fmt/format.h>
#include <peripherals/typing/enum/debug/component.h>
#include <peripherals/typing/enum/debug/error_type.h>
#include <peripherals/typing/enum/debug/severity.h>

namespace semantic::detail {
std::string_view enum_to_string(semantic::debug::Component component);
std::string_view enum_to_string(semantic::debug::Severity severity);
std::string_view enum_to_string(semantic::debug::Type type);
} // namespace semantic::detail

template<>
struct fmt::formatter<semantic::debug::Component>
{
    template<typename ParseCtx>
    constexpr auto parse(ParseCtx& ctx)
    {
        return ctx.begin();
    }

    template<typename FormatCtx>
    auto format(semantic::debug::Component p, FormatCtx& ctx)
    {
        return fmt::format_to(
            ctx.out(), "{}", semantic::detail::enum_to_string(p));
    }
};

template<>
struct fmt::formatter<semantic::debug::Severity>
{
    template<typename ParseCtx>
    constexpr auto parse(ParseCtx& ctx)
    {
        return ctx.begin();
    }

    template<typename FormatCtx>
    auto format(semantic::debug::Severity p, FormatCtx& ctx)
    {
        return fmt::format_to(
            ctx.out(), "{}", semantic::detail::enum_to_string(p));
    }
};

template<>
struct fmt::formatter<semantic::debug::Type>
{
    template<typename ParseCtx>
    constexpr auto parse(ParseCtx& ctx)
    {
        return ctx.begin();
    }

    template<typename FormatCtx>
    auto format(semantic::debug::Type p, FormatCtx& ctx)
    {
        return fmt::format_to(
            ctx.out(), "{}", semantic::detail::enum_to_string(p));
    }
};
