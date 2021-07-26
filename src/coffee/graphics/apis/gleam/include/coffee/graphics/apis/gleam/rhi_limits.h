#pragma once

#include "rhi_features.h"
#include "rhi_versioning.h"

namespace gleam {
namespace limits {

struct tex
{
    struct
    {
        u32 cube, d2, d2_array, d3;
    } size;
    struct
    {
    } fmt;
};

} // namespace limits

struct hw_limits
{
    static auto u32_span(u32& value)
    {
        return SpanOne<i32>(value);
    }

    hw_limits(features& feat) : m_features(feat)
    {
    }

    template<typename T>
    requires std::is_same_v<T, limits::tex> limits::tex limits_of()
    {
        using get = group::get_prop;

        limits::tex out;

        cmd::get_integerv(get::max_texture_size, u32_span(out.size.d2));
        cmd::get_integerv(get::max_3d_texture_size, u32_span(out.size.d3));
        cmd::get_integerv(
            get::max_cube_map_texture_size, u32_span(out.size.cube));

        return out;
    }

    features m_features;
};

} // namespace gleam
