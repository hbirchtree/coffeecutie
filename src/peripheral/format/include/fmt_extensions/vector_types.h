#pragma once

#include <fmt/format.h>
#include <peripherals/typing/vectors/vector_types.h>
#include <peripherals/typing/geometry/size.h>

template<>
struct fmt::formatter<glm::quat>
{
    template<typename ParseCtx>
    constexpr auto parse(ParseCtx& ctx)
    {
        return ctx.begin();
    }

    template<typename FormatCtx>
    auto format(glm::quat const& p, FormatCtx& ctx)
    {
        return fmt::format_to(
            ctx.out(), "quat({}, {}, {}, {})", p.x, p.y, p.z, p.w);
    }
};

template<>
struct fmt::formatter<glm::vec2>
{
    template<typename ParseCtx>
    constexpr auto parse(ParseCtx& ctx)
    {
        return ctx.begin();
    }

    template<typename FormatCtx>
    auto format(glm::vec2 const& p, FormatCtx& ctx)
    {
        return fmt::format_to(ctx.out(), "vec2({}, {})", p.x, p.y);
    }
};

template<>
struct fmt::formatter<glm::vec3>
{
    template<typename ParseCtx>
    constexpr auto parse(ParseCtx& ctx)
    {
        return ctx.begin();
    }

    template<typename FormatCtx>
    auto format(glm::vec3 const& p, FormatCtx& ctx)
    {
        return fmt::format_to(ctx.out(), "vec3({}, {}, {})", p.x, p.y, p.z);
    }
};

template<>
struct fmt::formatter<glm::vec4>
{
    template<typename ParseCtx>
    constexpr auto parse(ParseCtx& ctx)
    {
        return ctx.begin();
    }

    template<typename FormatCtx>
    auto format(glm::vec4 const& p, FormatCtx& ctx)
    {
        return fmt::format_to(
            ctx.out(), "vec4({}, {}, {})", p.x, p.y, p.z, p.w);
    }
};

template<>
struct fmt::formatter<glm::mat3>
{
    template<typename ParseCtx>
    constexpr auto parse(ParseCtx& ctx)
    {
        return ctx.begin();
    }

    template<typename FormatCtx>
    auto format(glm::mat3 const& p, FormatCtx& ctx)
    {
        return fmt::format_to(ctx.out(), "mat3({}, {}, {})", p[0], p[1], p[2]);
    }
};

template<>
struct fmt::formatter<glm::mat4>
{
    template<typename ParseCtx>
    constexpr auto parse(ParseCtx& ctx)
    {
        return ctx.begin();
    }

    template<typename FormatCtx>
    auto format(glm::mat4 const& p, FormatCtx& ctx)
    {
        return fmt::format_to(
            ctx.out(), "mat4({}, {}, {}, {})", p[0], p[1], p[2], p[3]);
    }
};

template<typename T>
struct fmt::formatter<typing::geometry::size_2d<T>>
{
    template<typename ParseCtx>
    constexpr auto parse(ParseCtx& ctx)
    {
        return ctx.begin();
    }

    template<typename FormatCtx>
    auto format(typing::geometry::size_2d<T> const& p, FormatCtx& ctx)
    {
        return fmt::format_to(
            ctx.out(), "size2({}, {})", p.w, p.h);
    }
};

template<typename T>
struct fmt::formatter<typing::geometry::size_3d<T>>
{
    template<typename ParseCtx>
    constexpr auto parse(ParseCtx& ctx)
    {
        return ctx.begin();
    }

    template<typename FormatCtx>
    auto format(typing::geometry::size_3d<T> const& p, FormatCtx& ctx)
    {
        return fmt::format_to(
            ctx.out(), "size3({}, {}, {})", p.w, p.h, p.d);
    }
};
