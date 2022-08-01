#pragma once

#include <peripherals/semantic/enum/data_types.h>

#include "rhi_program.h"
#include "rhi_query.h"
#include "rhi_versioning.h"
#include "rhi_vertex.h"

namespace gleam {

constexpr libc_types::i32 invalid_uniform = -1;
using uniform_key = std::pair<std::string_view, libc_types::i32>;

template<typename... UType>
using uniform_list =
    std::tuple<std::pair<uniform_key, semantic::Span<UType>>...>;

template<typename... UType>
inline auto make_uniform_list(
    typing::graphics::ShaderStage stage,
    std::pair<uniform_key, semantic::Span<UType>>&&... uniforms)
{
    return std::make_tuple<
        typing::graphics::ShaderStage,
        std::pair<uniform_key, semantic::Span<UType>>...>(
        std::move(stage), std::move(uniforms)...);
}

struct sampler_t;

struct draw_command
{
    using vertex_type =
#if GLEAM_MAX_VERSION_ES != 0x200
        vertex_array_t;
#else
        vertex_array_legacy_t;
#endif
    using query_type =
#if GLEAM_MAX_VERSION_ES != 0x200
        query_t;
#else
        null_query_t;
#endif
    using sampler_data_t = std::vector<std::tuple<
        typing::graphics::ShaderStage,
        uniform_key,
        std::shared_ptr<sampler_t>>>;
    using buffer_data_t  = std::vector<std::tuple<
        typing::graphics::ShaderStage,
        uniform_key,
        std::shared_ptr<sampler_t>>>;

    stl_types::WkPtr<program_t>   program;
    stl_types::WkPtr<vertex_type> vertices;
    struct
    {
        bool indexed{false};
        bool instanced{false};

        drawing::primitive mode{drawing::primitive::triangle};
    } call;
    struct
    {
        struct
        {
            u32 count{0}, offset{0};
        } arrays;
        struct
        {
            u32 count{0};
            u64 offset{0};

            semantic::TypeEnum type{semantic::TypeEnum::UInt};
        } elements;
        struct
        {
            u32 count{0}, offset{0};
        } instances;
    } data;

    stl_types::WkPtr<query_type> conditional_query;

    sampler_data_t samplers;
    buffer_data_t  buffers;
};

} // namespace gleam
