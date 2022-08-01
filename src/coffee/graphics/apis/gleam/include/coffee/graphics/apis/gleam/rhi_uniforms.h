#pragma once

#include <array>

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

inline i32 get_program_uniform_location(
    program_t const&              program,
    typing::graphics::ShaderStage stage,
    uniform_key const&            key)
{
    auto [uname, ulocation] = key;
    auto const& features    = program.m_features;
    if(ulocation != invalid_uniform && features.uniform_location)
        return ulocation;

    if(features.separable_programs)
    {
        auto stageprogram = program.m_stages.find(stage);
        if(stageprogram == program.m_stages.end())
            Throw(unknown_shader_stage(
                "stage not defined in program: " +
                std::to_string(C_CAST<int>(stage))));
        ulocation =
            cmd::get_uniform_location(stageprogram->second->m_handle, uname);
    } else
        ulocation = cmd::get_uniform_location(program.m_handle, uname);

    if(ulocation == invalid_uniform)
        Throw(uniform_location_undefined(
            std::string("uniform not found: ") + uname.data()));

    return ulocation;
}

template<typename T, size_t Num>
inline bool apply_single_uniform(
    program_t const&              program,
    typing::graphics::ShaderStage stage,
    std::pair<
        uniform_key,
        semantic::Span<const typing::vectors::tmatrix<T, Num>>>&& uniform)
{
    auto [key, data] = uniform;
    auto ulocation   = get_program_uniform_location(program, stage, key);

#if GLEAM_MAX_VERSION_ES != 0x200
    if(program.m_features.separable_programs)
        cmd::program_uniform(
            program.m_stages.find(stage)->second->m_handle,
            ulocation,
            data.size(),
            false,
            data);
    else
#endif
        cmd::uniform(ulocation, data.size(), false, data);
    return true;
}

template<typename T, size_t Num>
inline bool apply_single_uniform(
    program_t const&              program,
    typing::graphics::ShaderStage stage,
    std::pair<
        uniform_key,
        semantic::Span<const typing::vectors::tvector<T, Num>>>&& uniform)
{
    auto [key, data] = uniform;
    auto ulocation   = get_program_uniform_location(program, stage, key);

#if GLEAM_MAX_VERSION_ES != 0x200
    if(program.m_features.separable_programs)
        cmd::program_uniform(
            program.m_stages.find(stage)->second->m_handle, ulocation, data);
    else
#endif
        cmd::uniform(ulocation, data);
    return true;
}

template<typename T>
requires(std::is_floating_point_v<T> || std::is_integral_v<T>) inline bool apply_single_uniform(
    program_t const&                                  program,
    typing::graphics::ShaderStage                     stage,
    std::pair<uniform_key, semantic::Span<const T>>&& uniform)
{
    auto [key, data] = uniform;
    auto ulocation   = get_program_uniform_location(program, stage, key);

#if GLEAM_MAX_VERSION_ES != 0x200
    if(program.m_features.separable_programs)
        cmd::program_uniform(
            program.m_stages.find(stage)->second->m_handle, ulocation, data);
    else
#endif
        cmd::uniform(ulocation, data.size(), data);
    return true;
}

template<typename Dummy, typename T>
inline bool apply_uniform_list_unwrap(
    program_t const& program, typing::graphics::ShaderStage stage, T&& uniform)
{
    return apply_single_uniform(program, stage, std::move(uniform));
}

template<typename Dummy, typename T, typename... Rest>
requires(sizeof...(Rest) > 0) inline bool apply_uniform_list_unwrap(
    program_t const&              program,
    typing::graphics::ShaderStage stage,
    T&&                           uniform,
    Rest&&... rest)
{
    if(!apply_single_uniform(program, stage, std::move(uniform)))
        return false;
    return apply_uniform_list_unwrap<Rest...>(
        program, stage, std::move(rest)...);
}

template<typename... UType>
inline bool apply_uniform_stage(
    program_t const& program, std::tuple<UType...>&& uniforms)
{
    return std::apply(
        apply_uniform_list_unwrap<UType...>,
        std::tuple_cat(
            std::make_tuple(std::ref(program)), std::move(uniforms)));
}

template<typename... UList>
inline std::optional<error> apply_uniforms(
    program_t const& program, UList&&... uniforms)
{
    (apply_uniform_stage(program, std::move(uniforms)) && ...);

    return std::nullopt;
}

inline semantic::result<u32, error> apply_samplers(
    program_t const&                    program,
    draw_command::sampler_data_t const& samplers,
    u32                                 first_unit = 0)
{
    if(samplers.empty())
        return semantic::success(0);
    u32         max_unit = 0;
    auto const& features =
        std::get<2>(samplers.at(0))->m_source.lock()->m_features;
    for(auto const& sampler : samplers)
    {
        auto [stage, locinfo, sampler_hnd] = sampler;
        auto [uname, index]                = locinfo;
        auto& texture                      = *sampler_hnd->m_source.lock();

        auto ulocation = cmd::get_uniform_location(program.m_handle, uname);

        cmd::active_texture(static_cast<group::texture_unit>(
            static_cast<libc_types::u32>(group::texture_unit::texture0) +
            first_unit + index));
#if GLEAM_MAX_VERSION_ES != 0x200
        if(features.samplers)
            cmd::bind_sampler(first_unit + index, sampler_hnd->m_handle);
#endif
        cmd::bind_texture(convert::to(texture.m_type), texture.m_handle);
        i32 uindex = first_unit + index;
        apply_single_uniform<i32>(
            program, stage, {{uname, ulocation}, SpanOne<const i32>(uindex)});
        max_unit = std::max<u32>(max_unit, first_unit + index);
    }
    return semantic::success(max_unit);
}

} // namespace gleam::detail
