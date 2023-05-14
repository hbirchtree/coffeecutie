#pragma once

#include <boost/outcome/std_result.hpp>
#include <fmt/format.h>
#include <fmt/std.h>
#include <fmt/ranges.h>
#include <optional>

template<typename T, typename E, typename P>
struct fmt::formatter<boost::outcome_v2::std_result<T, E, P>>
{
    template<typename ParseCtx>
    constexpr auto parse(ParseCtx& ctx)
    {
        return ctx.begin();
    }

    template<typename FormatCtx>
    auto format(boost::outcome_v2::std_result<T, E, P> const& res, FormatCtx& ctx)
    {
        if(res.has_value())
            return fmt::format_to(ctx.out(), "result({0})", res.value());
        else
            return fmt::format_to(ctx.out(), "result(error({0}))", res.error());
    }
};

template<typename T>
struct fmt::formatter<std::optional<T>>
{
    template<typename ParseCtx>
    constexpr auto parse(ParseCtx& ctx)
    {
        return ctx.begin();
    }

    template<typename FormatCtx>
    auto format(std::optional<T> const& res, FormatCtx& ctx)
    {
        if(res.has_value())
            return fmt::format_to(ctx.out(), "optional({0})", res.value());
        return fmt::format_to(ctx.out(), "optional(nullopt)");
    }
};
