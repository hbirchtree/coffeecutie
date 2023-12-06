#pragma once

#include "rhi_features.h"
#include "rhi_versioning.h"

#include <fmt/format.h>

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

        buffers = {};
        draws = {
            .element_count        = get_limit(prop::max_elements_indices),
            .element_index        = get_limit(prop::max_element_index),
            .element_vertex_count = get_limit(prop::max_elements_vertices),
        };
        shaders = {
            .vertex_uniform_vectors
            = get_limit(prop::max_vertex_uniform_vectors),
            .fragment_uniform_vectors
            = get_limit(prop::max_fragment_uniform_vectors),
        };
        textures = {
            .texture_units = get_limit(prop::max_combined_texture_image_units),
            .d2_size       = get_limit(prop::max_texture_size),
            .cube_size     = get_limit(prop::max_cube_map_texture_size),
        };
        if(m_features.buffer.ubo)
        {
            buffers = {
                .ubo_size     = get_limit(prop::max_uniform_block_size),
                .ubo_vertex   = get_limit(prop::max_vertex_uniform_blocks),
                .ubo_fragment = get_limit(prop::max_fragment_uniform_blocks),
                .ubo_alignment
                = get_limit(prop::uniform_buffer_offset_alignment),
            };
        }
        if(m_features.buffer.ssbo)
        {
            buffers.ssbo_size = std::numeric_limits<i32>::max();
            buffers.ssbo_vertex
                = get_limit(prop::max_vertex_shader_storage_blocks);
            buffers.ssbo_fragment
                = get_limit(prop::max_fragment_shader_storage_blocks);
            buffers.ssbo_alignment
                = get_limit(prop::shader_storage_buffer_offset_alignment);
        }
        if(m_features.program.compute)
        {
            buffers.ssbo_compute
                = get_limit(prop::max_compute_shader_storage_blocks);
            buffers.ubo_compute = get_limit(prop::max_compute_uniform_blocks);
        }
        if(m_features.draw.instancing)
        {
            draws.instance_count  = std::numeric_limits<i32>::max();
            draws.instance_offset = std::numeric_limits<i32>::max();
        }
        if(m_features.texture.texture_3d)
        {
            textures.d3_size       = get_limit(prop::max_3d_texture_size);
            textures.d2_max_layers = get_limit(prop::max_array_texture_layers);
        }
    }

    std::string serialize() const
    {
        return fmt::format(
            // clang-format off
            // So apparently writing JSON in fmt::format is a pain
            "{{"
                "\"buffers\": {{"
                    "\"ssbo_size\":{},"
                    "\"ubo_size\":{},"
                    "\"ssbo_vertex\":{},"
                    "\"ssbo_fragment\":{},"
                    "\"ssbo_compute\":{},"
                    "\"ubo_vertex\":{},"
                    "\"ubo_fragment\":{},"
                    "\"ubo_compute\":{},"
                    "\"ubo_alignment\":{},"
                    "\"ssbo_alignment\":{}"
                "}},"
                "\"draws\": {{"
                    "\"element_count\":{},"
                    "\"element_index\":{},"
                    "\"element_vertex_count\":{},"
                    "\"instance_count\":{},"
                    "\"instance_offset\":{}"
                "}},"
                "\"shaders\": {{"
                    "\"vertex_uniform_vectors\":{},"
                    "\"fragment_uniform_vectors\":{}"
                "}},"
                "\"textures\": {{"
                    "\"texture_units\":{},"
                    "\"d2_size\":{},"
                    "\"d3_size\":{},"
                    "\"d2_max_layers\":{},"
                    "\"cube_size\":{}"
                "}}"
            "}}",
            // clang-format on
            buffers.ssbo_size,
            buffers.ubo_size,
            buffers.ssbo_vertex,
            buffers.ssbo_fragment,
            buffers.ssbo_compute,
            buffers.ubo_vertex,
            buffers.ubo_fragment,
            buffers.ubo_compute,
            buffers.ubo_alignment,
            buffers.ssbo_alignment,
            //
            draws.element_count,
            draws.element_index,
            draws.element_vertex_count,
            draws.instance_count,
            draws.instance_offset,
            //
            shaders.vertex_uniform_vectors,
            shaders.fragment_uniform_vectors,
            //
            textures.texture_units,
            textures.d2_size,
            textures.d3_size,
            textures.d2_max_layers,
            textures.cube_size
            //
            );
    }

    struct buffer_limits_t
    {
        u32 ssbo_size{0};
        u32 ubo_size{0};

        u32 ssbo_vertex{0};
        u32 ssbo_fragment{0};
        u32 ssbo_compute{0};

        u32 ubo_vertex{0};
        u32 ubo_fragment{0};
        u32 ubo_compute{0};

        u32 ubo_alignment{0};
        u32 ssbo_alignment{0};
    } buffers;
    struct draw_limits_t
    {
        u32 element_count;
        u32 element_index;
        u32 element_vertex_count;

        u32 instance_count;
        u32 instance_offset;
    } draws;
    struct shader_limits_t
    {
        u32 vertex_uniform_vectors{0};
        u32 fragment_uniform_vectors{0};
    } shaders;
    struct texture_limits_t
    {
        u32 texture_units{};
        u32 d2_size;
        u32 d3_size{0};
        u32 d2_max_layers{0};
        u32 cube_size{0};
    } textures;

    features m_features;
};

} // namespace gleam
