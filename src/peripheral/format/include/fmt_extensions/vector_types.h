#pragma once

#include <fmt/format.h>
#include <peripherals/typing/geometry/size.h>
#include <peripherals/typing/vectors/vector_types.h>

/* A spec on the vector is handed to the component type's own formatter, so
 * "{:.3f}" prints vec3(1.000, 2.000, 3.000). One definition covers every
 * length and component type. */
template<glm::length_t N, typename T, glm::qualifier Q>
struct fmt::formatter<glm::vec<N, T, Q>>
{
    fmt::formatter<T> element;

    template<typename ParseCtx>
    constexpr auto parse(ParseCtx& ctx)
    {
        return element.parse(ctx);
    }

    template<typename FormatCtx>
    auto format(glm::vec<N, T, Q> const& p, FormatCtx& ctx) const
    {
        auto out = fmt::format_to(ctx.out(), "vec{}(", N);
        for(glm::length_t i = 0; i < N; i++)
        {
            if(i > 0)
                out = fmt::format_to(out, ", ");
            ctx.advance_to(out);
            out = element.format(p[i], ctx);
        }
        return fmt::format_to(out, ")");
    }
};

template<typename T, glm::qualifier Q>
struct fmt::formatter<glm::qua<T, Q>>
{
    fmt::formatter<T> element;

    template<typename ParseCtx>
    constexpr auto parse(ParseCtx& ctx)
    {
        return element.parse(ctx);
    }

    template<typename FormatCtx>
    auto format(glm::qua<T, Q> const& p, FormatCtx& ctx) const
    {
        /* Named rather than indexed: quat storage order is a GLM build
         * option, xyzw is not. */
        T const components[] = {p.x, p.y, p.z, p.w};

        auto out = fmt::format_to(ctx.out(), "quat(");
        for(int i = 0; i < 4; i++)
        {
            if(i > 0)
                out = fmt::format_to(out, ", ");
            ctx.advance_to(out);
            out = element.format(components[i], ctx);
        }
        return fmt::format_to(out, ")");
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
    auto format(glm::mat3 const& p, FormatCtx& ctx) const
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
    auto format(glm::mat4 const& p, FormatCtx& ctx) const
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
    auto format(typing::geometry::size_2d<T> const& p, FormatCtx& ctx) const
    {
        return fmt::format_to(ctx.out(), "size2({}, {})", p.w, p.h);
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
    auto format(typing::geometry::size_3d<T> const& p, FormatCtx& ctx) const
    {
        return fmt::format_to(ctx.out(), "size3({}, {}, {})", p.w, p.h, p.d);
    }
};
