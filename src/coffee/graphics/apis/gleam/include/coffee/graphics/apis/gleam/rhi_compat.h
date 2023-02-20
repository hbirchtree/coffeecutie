#pragma once

#include "rhi.h"
#include "rhi_texture.h"
#include "rhi_uniforms.h"

namespace gleam::compat {

using texture_array_base_t =
#if GLEAM_MAX_VERSION_ES == 0x200
    texture_t
#else
    texture_2da_t
#endif
    ;

enum class texture_usage_hint_t
{
    no_hints          = 0x0,
    per_instance      = 0x1,
    per_base_instance = 0x2,
};
C_FLAGS(texture_usage_hint_t, u32);

struct texture_2da_t : texture_array_base_t
{
    texture_2da_t(
        api*                api_,
        PixDesc const&      fmt,
        u32                 mips,
        textures::property  properties = textures::property::none) :
        texture_array_base_t(
            api_->feature_info().texture,
            api_->queue<api::queues::texture_decode>(),
            api_->debug(),
            textures::type::d2_array,
            fmt,
            mips,
            properties),
        m_api(api_), m_compat_active(!m_features.texture_3d)
    {
        if(m_compat_active)
            m_type = textures::type::d2;
    }

    virtual void alloc(size_type const& size, bool create_storage = true)
    {
        if(!m_compat_active)
        {
            texture_array_base_t::alloc(size, create_storage);
            return;
        }
        m_textures.reserve(size.d);
        for(C_UNUSED(auto _) : stl_types::range<>(size.d))
        {
            m_textures.emplace_back(m_api->alloc_texture(
                textures::d2, m_format, m_mipmaps, m_flags));
            m_textures.back()->alloc(size, create_storage);
        }
    }

    template<class T, class VectorT, class SizeT>
    void upload(
        T const& data, VectorT const& offset, SizeT const& size, u32 level = 0)
    {
#if GLEAM_MAX_VERSION_ES != 0x200
        if(!m_compat_active)
        {
            texture_array_base_t::upload(data, offset, size, level);
            return;
        }
#endif
        if(size[2] > 1)
            Throw(std::out_of_range(
                "compat::texture_2da_t: does not support multi-layer uploads"));
        if(offset.z() >= static_cast<i32>(m_textures.size()))
            Throw(std::out_of_range(
                "compat::texture_2da_t: offset out of range"));

        m_textures.at(offset.z())
            ->upload(
                data,
                typing::vector_types::Veci2{offset.x(), offset.y()},
                size_2d<i32>{size[0], size[1]},
                level);
    }

    void set_usage_hint(texture_usage_hint_t hint)
    {
        m_hints |= hint;
    }

    texture_usage_hint_t m_hints{texture_usage_hint_t::no_hints};
    std::vector<std::shared_ptr<texture_2d_t>> m_textures;

  private:
    api* m_api;
    bool m_compat_active{false};
};

using texture_definition_t = std::tuple<
    typing::graphics::ShaderStage,
    uniform_key,
    std::shared_ptr<compat::texture_2da_t>,
    std::shared_ptr<sampler_t>>;

template<typename... Textures>
requires(std::is_same_v<Textures, texture_definition_t>&&...)
    //
    inline auto make_texture_list(Textures&&... defs)
{
    std::vector<texture_definition_t> definitions;
    (definitions.emplace_back(std::move(defs)), ...);
    return definitions;
}

using texture_list = declreturntype(make_texture_list<>);

} // namespace gleam::compat

namespace gleam::detail {

inline bool apply_command_modifier(
    program_t const&      program,
    shader_bookkeeping_t& bookkeeping,
    compat::texture_list& textures)
{
    if(textures.empty())
        return false;

    auto const& features = std::get<2>(textures.at(0))->m_features;

    if(features.texture_3d)
    {
        sampler_list samplers;
        samplers.reserve(textures.size());
        for(auto const& tex : textures)
            samplers.push_back(sampler_definition_t{
                std::get<0>(tex),
                std::get<1>(tex),
                std::get<3>(tex),
            });
        return apply_command_modifier(program, bookkeeping, samplers);
    }

    for(auto const& texture : textures)
    {
        auto [_, locinfo, tex, sampler] = texture;

        if(tex->m_hints != compat::texture_usage_hint_t::no_hints)
            continue;

        if(tex->m_textures.empty())
            continue;

        auto const& src   = tex->m_textures.at(0);
        auto        index = bookkeeping.sampler_idx++;

        cmd::active_texture(
            group::texture_unit::texture0 + bookkeeping.sampler_idx++);
        cmd::bind_texture(group::texture_target::texture_2d, src->m_handle);
        apply_texture_filtering_opts(
            textures::type::d2, sampler->m_min, sampler->m_mag);
        apply_sampler_uniform(
            program, typing::graphics::ShaderStage::All, locinfo, index);
    }

    return true;
}

inline void apply_command_modifier_per_call(
    program_t const&      program,
    shader_bookkeeping_t& bookkeeping,
    compat::texture_list& textures,
    u32                   base_instance = 0,
    u32                   instance_id   = 0)
{
    if(textures.empty())
        return;

    auto const& features = std::get<2>(textures.at(0))->m_features;

    if(features.texture_3d)
        return;

    for(auto const& texture : textures)
    {
        auto [_, locinfo, tex, sampler] = texture;

        if(tex->m_hints == compat::texture_usage_hint_t::no_hints)
            continue;

        if(tex->m_textures.empty())
            continue;

        auto const& src = tex->m_textures.at(
            tex->m_hints == compat::texture_usage_hint_t::per_base_instance
                ? base_instance
                : instance_id);
        auto index = bookkeeping.sampler_idx++;

        cmd::active_texture(group::texture_unit::texture0 + index);
        cmd::bind_texture(group::texture_target::texture_2d, src->m_handle);
        apply_texture_filtering_opts(
            textures::type::d2, sampler->m_min, sampler->m_mag);
        apply_sampler_uniform(
            program, typing::graphics::ShaderStage::All, locinfo, index);
    }
}

inline void undo_command_modifier(
    program_t const& /*program*/,
    shader_bookkeeping_t& /*bookkeeping*/,
    compat::texture_list&& /*textures*/)
{
    //    for(auto const& texture : textures)
    //    {
    //    }
}

} // namespace gleam::detail
