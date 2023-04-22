#pragma once

#include <array>
#include <peripherals/stl/tuple_foreach.h>
#include <peripherals/stl/tuple_slice.h>

#include "rhi_draw_command.h"
#include "rhi_features.h"
#include "rhi_program.h"
#include "rhi_texture.h"
#include "rhi_translate.h"
#include "rhi_versioning.h"

namespace gleam::detail {

struct uniform_location_undefined : std::runtime_error
{
    using std::runtime_error::runtime_error;
};
struct unknown_shader_stage : std::runtime_error
{
    using std::runtime_error::runtime_error;
};

struct shader_bookkeeping_t
{
    u32 buffer_idx{0};
    u32 sampler_idx{0};
    u32 view_idx{0};

    i32 instanceId{0};
    i32 baseInstance{0};

    typing::vector_types::Veci4 previous_viewport{};
};

inline i32 get_program_uniform_location(
    program_t const&              program,
    typing::graphics::ShaderStage stage,
    uniform_key const&            key)
{
    auto        ulocation = key.location;
    auto const& features  = program.m_features;
    if(ulocation != invalid_uniform && features.uniform_location)
        return ulocation;

    if(features.separable_programs)
    {
        auto stageprogram = program.m_stages.find(stage);
        if(stageprogram == program.m_stages.end())
            Throw(unknown_shader_stage(
                "stage not defined in program: "
                + std::to_string(C_CAST<int>(stage))));
        ulocation = cmd::get_uniform_location(
            stageprogram->second->m_handle, key.name);
    } else
        ulocation = cmd::get_uniform_location(program.m_handle, key.name);

    //    if(ulocation == invalid_uniform)
    //        Throw(uniform_location_undefined(
    //            "uniform not found: "
    //            + std::string(key.name.begin(), key.name.end())));

    return ulocation;
}

inline i32 get_program_buffer_location(
    program_t const& /*program*/,
    typing::graphics::ShaderStage /*stage*/,
    uniform_key const& key)
{
    using namespace std::string_literals;

    if(key.location != invalid_uniform)
        return key.location;
    Throw(uniform_location_undefined(
        "buffer location must be specified: "
        + std::string(key.name.begin(), key.name.end())));
}

template<typename T, int Num>
inline bool apply_single_uniform(
    program_t const&                                            program,
    typing::graphics::ShaderStage                               stage,
    uniform_pair<const typing::vectors::tmatrix<T, Num>> const& uniform)
{
    auto& [key, data] = uniform;
    auto ulocation    = get_program_uniform_location(program, stage, key);

    if(ulocation == invalid_uniform)
        return false;

#if GLEAM_MAX_VERSION >= 0x410 || GLEAM_MAX_VERSION_ES >= 0x310
    if(program.m_features.separable_programs)
        cmd::program_uniform(
            program.m_stages.find(stage)->second->m_handle,
            ulocation,
            false,
            std::move(data));
    else
#endif
        cmd::uniform(ulocation, false, std::move(data));
    return true;
}

template<typename T, int Num>
inline bool apply_single_uniform(
    program_t const&                                            program,
    typing::graphics::ShaderStage                               stage,
    uniform_pair<const typing::vectors::tvector<T, Num>> const& uniform)
{
    auto [key, data] = uniform;
    auto ulocation   = get_program_uniform_location(program, stage, key);

    if(ulocation == invalid_uniform)
        return false;

#if GLEAM_MAX_VERSION >= 0x410 || GLEAM_MAX_VERSION_ES >= 0x310
    if(program.m_features.separable_programs)
        cmd::program_uniform(
            program.m_stages.find(stage)->second->m_handle, ulocation, data);
    else
#endif
        cmd::uniform(ulocation, data);
    return true;
}

template<typename T>
requires(std::is_floating_point_v<T> || std::is_integral_v<T>)
    //
    inline bool apply_single_uniform(
        program_t const&              program,
        typing::graphics::ShaderStage stage,
        uniform_pair<const T> const&  uniform)
{
    auto [key, data] = std::tie(uniform.key, uniform.data);
    auto ulocation   = get_program_uniform_location(program, stage, key);

    if(ulocation == invalid_uniform)
        return false;

#if GLEAM_MAX_VERSION >= 0x410 || GLEAM_MAX_VERSION_ES >= 0x310
    if(program.m_features.separable_programs)
        cmd::program_uniform(
            program.m_stages.find(stage)->second->m_handle, ulocation, data);
    else
#endif
        cmd::uniform(ulocation, data);
    return true;
}

template<typename S, typename... UType>
requires semantic::concepts::graphics::detail::is_uniform_stage<S, UType...>
inline bool apply_command_modifier(
    program_t const& program,
    shader_bookkeeping_t& /*bookkeeping*/,
    std::tuple<S, UType...>& uniforms)
{
    auto stage         = std::move(std::get<0>(uniforms));
    auto only_uniforms = stl_types::tuple::slice<1>(std::move(uniforms));
    stl_types::tuple::for_each(only_uniforms, [&program, &stage](auto& u) {
        apply_single_uniform(program, stage, u);
    });
    return true;
}

template<typename S, typename... UType>
requires semantic::concepts::graphics::detail::is_uniform_stage<S, UType...>
inline bool apply_command_modifier_per_call(
    program_t const&, shader_bookkeeping_t&, std::tuple<S, UType...>&, u32, u32)
{
    return true;
}

template<typename S, typename... UType>
requires semantic::concepts::graphics::detail::is_uniform_stage<S, UType...>
inline void undo_command_modifier(
    program_t const&, shader_bookkeeping_t&, std::tuple<S, UType...>&&)
{
    // Nothing to do here
}

inline bool apply_command_modifier(
    program_t const& program,
    shader_bookkeeping_t& /*bookkeeping*/,
    [[maybe_unused]] buffer_list&                       buffer_info,
    [[maybe_unused]] std::optional<std::pair<u32, u32>> span = {})
{
#if GLEAM_MAX_VERSION >= 0x300 || GLEAM_MAX_VERSION_ES >= 0x300
    for(auto const& buffer_def : buffer_info)
    {
        auto  binding = buffer_def.key.location;
        auto& buffer  = buffer_def.buffer;

        if(!buffer.valid())
            continue;

        if(buffer.m_type == buffers::type::constants)
        {
            /* TODO: Cache blk_idx, slap it in a map<pair<prog, name>, idx> */
            auto blk_idx = cmd::get_uniform_block_index(
                program.m_handle, buffer_def.key.name);
            if(blk_idx == std::numeric_limits<u32>::max())
                continue;
            cmd::uniform_block_binding(program.m_handle, blk_idx, binding);
        }

        auto offset = buffer.m_offset;
        auto size   = buffer.m_size;

        if(span.has_value() && buffer_def.stride != 0)
        {
            auto span_ = *span;
            offset += buffer_def.stride * span_.first;
            size = buffer_def.stride * span_.second;
        }

        cmd::bind_buffer_range(
            convert::to<group::buffer_target_arb>(buffer.m_type),
            binding,
            buffer.handle(),
            offset,
            size);
    }
    return true;
#else
    return false;
#endif
}

inline bool apply_command_modifier_per_call(
    program_t const&, shader_bookkeeping_t&, buffer_list&, u32, u32)
{
    return true;
}

inline void undo_command_modifier(
    program_t const&, shader_bookkeeping_t&, buffer_list&&)
{
}

inline void apply_texture_filtering_opts(
    textures::type    target,
    typing::Filtering min_filter,
    typing::Filtering mag_filter)
{
    cmd::tex_parameter(
        convert::to(target),
        group::texture_parameter_name::texture_min_filter,
        static_cast<i32>(convert::to<group::sampler_parameter_i>(min_filter)));
    cmd::tex_parameter(
        convert::to(target),
        group::texture_parameter_name::texture_mag_filter,
        static_cast<i32>(convert::to<group::sampler_parameter_i>(mag_filter)));
}

inline void apply_sampler_uniform(
    program_t const&              program,
    typing::graphics::ShaderStage stage,
    uniform_key const&            key,
    u32                           index)
{
    auto [uname, ulocation] = key;
    if(ulocation == invalid_uniform)
        ulocation = cmd::get_uniform_location(program.m_handle, uname);
    i32 uindex = static_cast<i32>(index);
    apply_single_uniform<i32>(
        program, stage, {{uname, ulocation}, SpanOne<const i32>(uindex)});
}

inline bool apply_command_modifier(
    program_t const&      program,
    shader_bookkeeping_t& bookkeeping,
    sampler_list&         samplers)
{
    if(samplers.empty())
        return true;
    [[maybe_unused]] auto const& features
        = std::get<2>(samplers.at(0))->m_source.lock()->m_features;

#if GLEAM_MAX_VERSION >= 0x450
    if(features.dsa && false)
    {
        using typing::vector_types::Vecui2;

        std::vector<u32> texture_handles;
        std::vector<u32> sampler_handles;
        for(auto const& sampler : samplers)
        {
            auto [stage, locinfo, sampler_hnd] = sampler;
            texture_handles.push_back(sampler_hnd->m_source.lock()->m_handle);
            sampler_handles.push_back(sampler_hnd->m_handle);
        }
        auto first = bookkeeping.sampler_idx;
        cmd::bind_textures(first, texture_handles);
        cmd::bind_samplers(first, sampler_handles);
        bookkeeping.sampler_idx += static_cast<u32>(texture_handles.size());
        i32 i = 0;
        for(auto const& sampler : samplers)
        {
            auto [stage, locinfo, sampler_hnd] = sampler;
            apply_sampler_uniform(program, stage, locinfo, first + i++);
        }
    }
#endif

    for(auto const& sampler : samplers)
    {
        auto [stage, locinfo, sampler_hnd] = sampler;
        auto& texture                      = *sampler_hnd->m_source.lock();
        u32   index;
        if(features.sampler_binding)
            index = locinfo.location != -1 ? locinfo.location
                                           : bookkeeping.sampler_idx++;
        else
            index = bookkeeping.sampler_idx++;
        cmd::active_texture(group::texture_unit::texture0 + index);
        cmd::bind_texture(convert::to(texture.m_type), texture.m_handle);
#if GLEAM_MAX_VERSION_ES != 0x200
        if(features.samplers)
            cmd::bind_sampler(index, sampler_hnd->m_handle);
        else
#endif
        {
            apply_texture_filtering_opts(
                texture.m_type, sampler_hnd->m_min, sampler_hnd->m_mag);
        }
        if(!features.sampler_binding)
            apply_sampler_uniform(program, stage, locinfo, index);
    }
    return true;
}

inline bool apply_command_modifier_per_call(
    program_t const&, shader_bookkeeping_t&, sampler_list&, u32, u32)
{
    return true;
}

inline void undo_command_modifier(
    program_t const& /*program*/,
    shader_bookkeeping_t& /*bookkeeping*/,
    sampler_list&& /*samplers*/)
{
    //    for(auto const& sampler : samplers)
    //    {
    //    }
}

inline bool apply_command_modifier(
    program_t const&      program,
    shader_bookkeeping_t& bookkeeping,
    texture_list&         textures)
{
    [[maybe_unused]] auto const& features
        = std::get<2>(textures.at(0)).m_features;
    if(textures.empty())
        return true;

#if GLEAM_MAX_VERSION >= 0x450
    if(features.dsa)
    {
        using typing::vector_types::Vecui2;

        std::vector<u32> texture_handles;
        for(auto const& texture : textures)
        {
            auto [stage, locinfo, texture_hnd] = texture;
            texture_handles.push_back(texture_hnd.handle);
        }
        auto first = bookkeeping.sampler_idx;
        cmd::bind_textures(first, texture_handles);
        bookkeeping.sampler_idx += static_cast<u32>(texture_handles.size());
        i32 i = 0;
        for(auto const& texture : textures)
        {
            auto [stage, locinfo, sampler_hnd] = texture;
            auto [uname, ulocation]            = locinfo;
            if(ulocation == invalid_uniform)
                ulocation = cmd::get_uniform_location(program.m_handle, uname);
            i32 uindex = first + i++;
            apply_single_uniform<i32>(
                program,
                stage,
                {{uname, ulocation}, SpanOne<const i32>(uindex)});
        }
    }
#endif

    for(auto const& texture : textures)
    {
        auto [stage, locinfo, texture_hnd] = texture;
        auto [uname, ulocation]            = locinfo;
        auto index                         = bookkeeping.sampler_idx++;

        if(ulocation == invalid_uniform)
            ulocation = cmd::get_uniform_location(program.m_handle, uname);

        cmd::active_texture(group::texture_unit::texture0 + index);
        cmd::bind_texture(convert::to(texture_hnd.type), texture_hnd.handle);
        i32 uindex = index;
        apply_single_uniform<i32>(
            program, stage, {{uname, ulocation}, SpanOne<const i32>(uindex)});
    }
    return true;
}

inline bool apply_command_modifier_per_call(
    program_t const&, shader_bookkeeping_t&, texture_list&, u32, u32)
{
    return true;
}

inline bool apply_command_modifier(
    program_t const& /*program*/,
    shader_bookkeeping_t& bookkeeping,
    view_state&           view_info)
{
    using typing::vector_types::Vecf2;
    using typing::vector_types::Veci2;

    if(bookkeeping.view_idx > 0 && !view_info.indexed)
        return false;

#if GLEAM_MAX_VERSION >= 0x410
    if(view_info.indexed)
    {
        if(auto view = view_info.view; view)
            cmd::viewport_indexedf(
                bookkeeping.view_idx,
                Vecf2(view->x, view->y),
                Vecf2(view->z, view->w));
        if(auto view = view_info.scissor; view)
        {
            cmd::enable(group::enable_cap::scissor_test);
            cmd::scissor_indexed(
                bookkeeping.view_idx,
                view->x,
                view->y,
                Veci2{view->z, view->w});
        }
        if(auto depth = view_info.depth; depth)
        {
            cmd::enable(group::enable_cap::depth_test);
            cmd::depth_range_indexed(
                bookkeeping.view_idx, depth->range->x, depth->range->y);
        }
        bookkeeping.view_idx++;
    } else
#endif
    {
        if(auto view = view_info.view; view)
        {
            cmd::get_integerv(
                group::get_prop::viewport,
                Span<i32>(&bookkeeping.previous_viewport[0], 4));
            cmd::viewport(Veci2{view->x, view->y}, Veci2{view->z, view->w});
        }
        if(auto view = view_info.scissor; view)
        {
            cmd::enable(group::enable_cap::scissor_test);
            cmd::scissor(Veci2{view->x, view->y}, Veci2{view->z, view->w});
        }
        if(auto depth_ = view_info.depth; depth_.has_value())
        {
            auto const& depth = *depth_;
            cmd::enable(group::enable_cap::depth_test);
            if(depth.range)
            {
#if GLEAM_MAX_VERSION >= 0x100
                cmd::depth_range(depth.range->x, depth.range->y);
#else
                cmd::depth_rangef(depth.range->x, depth.range->y);
#endif
            }
            cmd::depth_func(
                depth.reversed ? group::depth_function::gequal
                               : group::depth_function::less);
#if GLEAM_MAX_VERSION >= 0x450
            if(depth.reversed)
                cmd::clip_control(
                    group::clip_control_origin::lower_left,
                    group::clip_control_depth::zero_to_one);
#endif
        }
    }

    return true;
}

inline bool apply_command_modifier_per_call(
    program_t const&, shader_bookkeeping_t&, view_state&, u32, u32)
{
    return true;
}

inline void undo_command_modifier(
    program_t const& /*program*/,
    shader_bookkeeping_t& bookkeeping,
    view_state&&          view_info)
{
    if(view_info.scissor.has_value())
        cmd::disable(group::enable_cap::scissor_test);
    if(view_info.depth.has_value())
        cmd::disable(group::enable_cap::depth_test);
    if(view_info.view.has_value())
        cmd::viewport(
            Veci2{},
            Veci2{
                bookkeeping.previous_viewport[2],
                bookkeeping.previous_viewport[3]});
}

inline bool apply_command_modifier(
    program_t const& /*program*/,
    shader_bookkeeping_t& /*bookkeeping*/,
    cull_state& view_info)
{
    cmd::enable(group::enable_cap::cull_face);
    cmd::cull_face(
        view_info.front_face ? group::triangle_face::front
                             : group::triangle_face::back);
    return true;
}

inline bool apply_command_modifier_per_call(
    program_t const&, shader_bookkeeping_t&, cull_state&, u32, u32)
{
    return true;
}

inline void undo_command_modifier(
    program_t const& /*program*/,
    shader_bookkeeping_t& /*bookkeeping*/,
    cull_state&& /*view_info*/)
{
    cmd::disable(group::enable_cap::cull_face);
}

inline bool apply_command_modifier(
    program_t const& /*program*/,
    shader_bookkeeping_t& /*bookkeeping*/,
    blend_state& view_info)
{
    cmd::enable(group::enable_cap::blend);
    if(view_info.additive)
    {
        cmd::blend_func(
            group::blending_factor::src_alpha, group::blending_factor::one);
    } else if(view_info.multiply)
    {
        cmd::blend_func(
            group::blending_factor::dst_color,
            group::blending_factor::one_minus_src_alpha);
    } else
        cmd::blend_func(
            group::blending_factor::src_alpha,
            group::blending_factor::one_minus_src_alpha);
    return true;
}

inline bool apply_command_modifier_per_call(
    program_t const&, shader_bookkeeping_t&, blend_state&, u32, u32)
{
    return true;
}

inline void undo_command_modifier(
    program_t const& /*program*/,
    shader_bookkeeping_t& /*bookkeeping*/,
    blend_state&& /*view_info*/)
{
    cmd::disable(group::enable_cap::blend);
}

} // namespace gleam::detail
