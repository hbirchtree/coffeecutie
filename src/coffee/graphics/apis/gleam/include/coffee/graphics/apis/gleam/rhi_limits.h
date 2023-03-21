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

struct api_limits
{
    static u32 get_limit(group::get_prop prop)
    {
        i32 limit{0};
        cmd::get_integerv(prop, SpanOne(limit));
        if(limit == -1)
            limit = std::numeric_limits<i32>::max();
        return static_cast<u32>(limit);
    }

    void set_limits(
        features const&           features,
        std::optional<api_limits> override = std::nullopt)
    {
        m_features = features;
        if(override)
        {
            auto limits = *override;
            buffers     = limits.buffers;
            draws       = limits.draws;
            textures    = limits.textures;
            return;
        }

        using prop = group::get_prop;

        draws = {
            .element_count        = get_limit(prop::max_elements_indices),
            .element_index        = get_limit(prop::max_element_index),
            .element_vertex_count = get_limit(prop::max_elements_vertices),

            .instance_count  = std::numeric_limits<i32>::max(),
            .instance_offset = std::numeric_limits<i32>::max(),
        };
        buffers = {};
        if(m_features.buffer.ubo)
        {
            buffers = {
                .ubo_size     = get_limit(prop::max_uniform_block_size),
                .ubo_vertex   = get_limit(prop::max_vertex_uniform_blocks),
                .ubo_fragment = get_limit(prop::max_fragment_uniform_blocks),
            };
        }
        if(m_features.buffer.ssbo)
        {
            buffers.ssbo_size = std::numeric_limits<i32>::max();
            buffers.ssbo_vertex
                = get_limit(prop::max_vertex_shader_storage_blocks);
            buffers.ssbo_fragment
                = get_limit(prop::max_fragment_shader_storage_blocks);
        }
        if(m_features.program.compute)
        {
            buffers.ssbo_compute
                = get_limit(prop::max_compute_shader_storage_blocks);
            buffers.ubo_compute = get_limit(prop::max_compute_uniform_blocks);
        }
    }

    struct
    {
        u32 ssbo_size{0};
        u32 ubo_size{0};

        u32 ssbo_vertex{0};
        u32 ssbo_fragment{0};
        u32 ssbo_compute{0};

        u32 ubo_vertex{0};
        u32 ubo_fragment{0};
        u32 ubo_compute{0};
    } buffers;
    struct
    {
        u32 element_count;
        u32 element_index;
        u32 element_vertex_count;

        u32 instance_count;
        u32 instance_offset;
    } draws;
    struct
    {
        u32 d2_size;
        u32 d3_size{0};
        u32 cube_size;
    } textures;

    features m_features;
};

} // namespace gleam
