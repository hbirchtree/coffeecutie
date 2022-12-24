#pragma once

#include <peripherals/semantic/enum/data_types.h>

#include "rhi_program.h"
#include "rhi_query.h"
#include "rhi_rendertarget.h"
#include "rhi_versioning.h"
#include "rhi_vertex.h"

namespace gleam {

struct sampler_t;

constexpr libc_types::i32 invalid_uniform = -1;
struct uniform_key
{
    std::string_view name;
    libc_types::i32  location{invalid_uniform};
};

struct view_state
{
    using rect        = std::optional<typing::vector_types::Veci4>;
    using depth_range = std::optional<typing::vector_types::Vecd2>;

    bool        indexed{false};
    rect        view;
    rect        scissor;
    depth_range depth;
};

struct cull_state
{
    bool front_face{true};
};

struct blend_state
{
};

template<typename UType>
struct uniform_pair
{
    template<typename SpanType>
    uniform_pair(uniform_key&& key, SpanType&& data)
        : key(key), data(data)
    {}

    uniform_key           key;
    semantic::Span<UType> data;
};

template<typename SpanType>
uniform_pair(uniform_key&&, SpanType&&)
    -> uniform_pair<const typename SpanType::value_type>;

// using uniform_pair = std::pair<uniform_key, semantic::Span<UType>>;

template<typename... UType>
using uniform_list = std::tuple<uniform_pair<UType>...>;

template<typename... UType>
//
inline auto make_uniform_list(
    typing::graphics::ShaderStage stage, uniform_pair<UType>&&... uniforms)
    -> std::tuple<typing::graphics::ShaderStage, uniform_pair<UType>...>
{
    return std::
        make_tuple<typing::graphics::ShaderStage, uniform_pair<UType>...>(
            std::move(stage), std::move(uniforms)...);
}

using buffer_definition_t
    = std::tuple<typing::graphics::ShaderStage, uniform_key, buffer_slice_t>;

template<typename... Buffers>
requires(std::is_same_v<Buffers, buffer_definition_t>&&...)
    //
    inline auto make_buffer_list(Buffers&&... defs)
{
    std::vector<buffer_definition_t> definitions;
    (definitions.emplace_back(std::move(defs)), ...);
    return definitions;
}

using buffer_list = declreturntype(make_buffer_list<>);

using sampler_definition_t = std::tuple<
    typing::graphics::ShaderStage,
    uniform_key,
    std::shared_ptr<sampler_t>>;

template<typename... Samplers>
requires(std::is_same_v<Samplers, sampler_definition_t>&&...)
    //
    inline auto make_sampler_list(Samplers&&... defs)
{
    std::vector<sampler_definition_t> definitions;
    (definitions.emplace_back(std::move(defs)), ...);
    return definitions;
}

using sampler_list = declreturntype(make_sampler_list<>);

struct draw_command
{
    using vertex_type =
#if GLEAM_MAX_VERSION_ES != 0x200 || defined(GL_OES_vertex_array_object)
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

    stl_types::WkPtr<program_t>      program;
    stl_types::WkPtr<vertex_type>    vertices;
    stl_types::WkPtr<rendertarget_t> render_target;
    struct call_spec_t
    {
        bool indexed{false};
        bool instanced{false};

        drawing::primitive mode{drawing::primitive::triangle};
    } call;
    struct data_t
    {
        struct
        {
            u32 count{0}, offset{0};
        } arrays;
        struct
        {
            u32 count{0};
            u64 offset{0};
            u64 vertex_offset{0};

            semantic::TypeEnum type{semantic::TypeEnum::UInt};
        } elements;
        struct
        {
            u32 count{1}, offset{0};
        } instances;
    };
    stl_types::Vector<data_t>  data;
    stl_types::ShPtr<buffer_t> data_indirect;

    stl_types::WkPtr<query_type> conditional_query;
};

} // namespace gleam
