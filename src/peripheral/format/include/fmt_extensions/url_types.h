#pragma once

#include <fmt/format.h>
#include <url/url.h>

template<>
struct fmt::formatter<platform::url::Url>
{
    template<typename ParseCtx>
    constexpr auto parse(ParseCtx& ctx)
    {
        return ctx.begin();
    }

    template<typename FormatCtx>
    auto format(platform::url::Url const& p, FormatCtx& ctx)
    {
        return fmt::format_to(ctx.out(), "url(path={}, category={})", p.internUrl, p.category);
    }
};

template<>
struct fmt::formatter<platform::url::Path>
{
    template<typename ParseCtx>
    constexpr auto parse(ParseCtx& ctx)
    {
        return ctx.begin();
    }

    template<typename FormatCtx>
    auto format(platform::url::Path const& p, FormatCtx& ctx)
    {
        return fmt::format_to(ctx.out(), "path({})", p.internUrl);
    }
};
