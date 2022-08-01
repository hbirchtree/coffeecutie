#pragma once

#include "rhi_features.h"
#include "rhi_translate.h"
#include "rhi_versioning.h"

#include <glw/extensions/ARB_bindless_texture.h>
#include <glw/extensions/ARB_direct_state_access.h>
#include <glw/extensions/ARB_texture_view.h>
#include <glw/extensions/EXT_direct_state_access.h>
#include <glw/extensions/EXT_texture_filter_anisotropic.h>
#include <glw/extensions/EXT_texture_view.h>
#include <glw/extensions/OES_texture_3D.h>
#include <glw/extensions/OES_texture_view.h>

#include <glw/extensions/ARB_texture_compression_bptc.h>
#include <glw/extensions/ARB_texture_compression_rgtc.h>
#include <glw/extensions/EXT_texture_compression_bptc.h>
#include <glw/extensions/EXT_texture_compression_rgtc.h>
#include <glw/extensions/EXT_texture_compression_s3tc.h>
#include <glw/extensions/IMG_texture_compression_pvrtc.h>
#include <glw/extensions/IMG_texture_compression_pvrtc2.h>
#include <glw/extensions/KHR_texture_compression_astc_hdr.h>
#include <glw/extensions/KHR_texture_compression_astc_ldr.h>
#include <glw/extensions/OES_compressed_ETC1_RGB8_texture.h>

namespace gleam {

struct sampler_t;

struct texture_t : std::enable_shared_from_this<texture_t>
{
    using size_type = size_3d<u32>;

    texture_t(
        features::textures const& features,
        textures::type            type,
        PixDesc const&            data,
        u32                       mipmaps,
        textures::property        properties) :
        m_features(features),
        m_format(data), m_type(type), m_mipmaps(mipmaps), m_flags(properties)
    {
    }

    void        alloc(size_type const& size, bool create_storage = true);
    inline void dealloc()
    {
        cmd::delete_textures(SpanOne<u32>(m_handle));
        m_handle.release();
    }

    size_type size();

    template<class TypeT, class VectorT, class SizeT>
    inline auto view(
        TypeT                        type,
        textures::view_params const& params,
        VectorT const&               offset,
        SizeT const&                 size);

    inline auto sampler();

    features::textures m_features{};
    PixDesc            m_format{};
    hnd                m_handle;
    textures::type     m_type{textures::type::d2};
    u32                m_mipmaps{1};
    size_2d<u32>       m_tex_size;
    u32                m_layers{1};
    textures::property m_flags{textures::property::none};
};

struct sampler_t
{
    sampler_t(stl_types::ShPtr<texture_t> const& source) :
        m_source(source), m_type(source->m_type), m_features(source->m_features)
    {
#if GLEAM_MAX_VERSION_ES != 0x200
        if(m_features.samplers)
        {
            cmd::gen_samplers(SpanOne<u32>(m_handle));
            cmd::bind_sampler(0, m_handle);
            cmd::bind_sampler(0, 0);
        }
#endif
    }

    inline void dealloc()
    {
#if GLEAM_MAX_VERSION_ES != 0x200
        if(m_features.samplers)
        {
            cmd::delete_samplers(SpanOne<u32>(m_handle));
            m_handle.release();
        }
#endif
    }

    inline void setLod(typing::vector_types::Vecf2 const& range)
    {
#if GLEAM_MAX_VERSION_ES != 0x200
        if(m_features.samplers)
        {
            cmd::sampler_parameter(
                m_handle,
                group::sampler_parameter_f::texture_min_lod,
                SpanOne(range.x()));
            cmd::sampler_parameter(
                m_handle,
                group::sampler_parameter_f::texture_max_lod,
                SpanOne(range.y()));
        } else
#endif
        {
            m_lodRange = range;
        }
    }

    inline void setAnisotropy(f32 samples)
    {
#if GLEAM_MAX_VERSION != 0
        if(m_features.anisotropy)
        {
            i32 max_aniso = 0;
            cmd::get_integerv(
                static_cast<group::get_prop>(GL_MAX_TEXTURE_MAX_ANISOTROPY),
                SpanOne(max_aniso));
            if(samples > max_aniso)
              Throw(std::out_of_range("anisotropic sample value out of range"));
            cmd::sampler_parameter(
                m_handle,
                group::sampler_parameter_f::texture_max_anisotropy,
                SpanOne(samples));
            return;
        }
#endif
#if GLEAM_MAX_VERSION_ES != 0x200 && defined(GL_EXT_texture_filter_anisotropic)
        if(m_features.ext.texture_anisotropic)
        {
            // TODO
        }
#endif
    }

    inline void setFiltering(
        typing::Filtering mag,
        typing::Filtering min,
        typing::Filtering mip = typing::Filtering::None)
    {
#if GLEAM_MAX_VERSION_ES != 0x200
        if(m_features.samplers)
        {
            using typing::Filtering;
            cmd::sampler_parameter(
                m_handle,
                group::sampler_parameter_i::texture_mag_filter,
                convert::to<group::sampler_parameter_i>(mag));
            cmd::sampler_parameter(
                m_handle,
                group::sampler_parameter_i::texture_min_filter,
                convert::to<group::sampler_parameter_i>(min, mip));
        } else
#endif
        {
            m_mag = mag, m_min = min, m_mip = mip;
        }
    }

    stl_types::WkPtr<texture_t> m_source;
    features::textures          m_features;
    hnd                         m_handle;
    textures::type              m_type{textures::type::d2};
    typing::vector_types::Vecf2 m_lodRange{0, 0};
    typing::Filtering           m_mag{typing::Filtering::Linear};
    typing::Filtering           m_min{typing::Filtering::Linear};
    typing::Filtering           m_mip{typing::Filtering::None};
};

struct texture_2d_t : texture_t
{
    using texture_t::texture_t;

    template<class T, class VectorT, class SizeT>
    std::optional<error> upload(
        T const& data, VectorT const& offset, SizeT const& size, i32 level = 0)
    {
        auto [ifmt1, type, layout] =
            convert::to<group::internal_format>(m_format);
        cmd::tex_sub_image_2d(
            group::texture_target::texture_2d,
            level,
            offset,
            size,
            type,
            layout,
            data);
        return std::nullopt;
    }
};

struct texture_2da_t : texture_t
{
    using texture_t::texture_t;

    template<class T, class VectorT, class SizeT>
    std::optional<error> upload(
        T const& data, VectorT const& offset, SizeT const& size, i32 level = 0)
    {
        auto [ifmt1, type, layout] =
            convert::to<group::internal_format>(m_format);
        cmd::bind_texture(group::texture_target::texture_2d_array, m_handle);
        cmd::tex_sub_image_3d(
            group::texture_target::texture_2d_array,
            level,
            offset,
            size,
            layout,
            type,
            data);
        return std::nullopt;
    }
};

struct texture_3d_t : texture_t
{
    using texture_t::texture_t;

    template<class T, class VectorT, class SizeT>
    std::optional<error> upload(
        T const& data, VectorT const& offset, SizeT const& size, i32 level = 0)
    {
        auto [ifmt1, type, layout] =
            convert::to<group::internal_format>(m_format);
        cmd::bind_texture(group::texture_target::texture_3d, m_handle);
        cmd::tex_sub_image_3d(
            group::texture_target::texture_3d,
            level,
            offset,
            size,
            layout,
            type,
            data);
        return std::nullopt;
    }
};

#if GLEAM_MAX_VERSION >= 430 || defined(GL_EXT_texture_view) || \
    defined(GL_OES_texture_view)
template<class TypeT, class VectorT, class SizeT>
requires std::is_same_v<TypeT, textures::type_d2> &&
    semantic::concepts::Vector<VectorT, u32, 3> &&
    semantic::concepts::Size3D<SizeT, u32>
inline auto make_texture_view(
    texture_t&                   origin,
    TypeT                        type,
    textures::view_params const& params,
    VectorT const&               offset,
    SizeT const&                 size)
{
    using output_type = typename std::conditional<
        std::is_same_v<TypeT, textures::type_d2>,
        texture_2d_t,
        typename std::conditional<
            std::is_same_v<TypeT, textures::type_d2_array>,
            texture_t,
            texture_t>::type>::type;

    auto out = MkShared<output_type>(
        origin.m_features,
        TypeT::value,
        params.format,
        params.mip.count,
        origin.m_flags);

#if defined(GL_TEXTURE_IMMUTABLE_FORMAT)
    if constexpr(compile_info::debug_mode)
    {
        i32 is_immutable{0};
#if GLEAM_MAX_VERSION >= 450
        if(origin.m_features.dsa)
        {
            cmd::get_texture_parameter(
                origin.m_handle,
                static_cast<group::get_texture_parameter>(
                    GL_TEXTURE_IMMUTABLE_FORMAT),
                SpanOne(is_immutable));
        } else
#endif
        {
            cmd::bind_texture(convert::to(origin.m_type), origin.m_handle);
            cmd::get_tex_parameter(
                convert::to(origin.m_type),
                static_cast<group::get_texture_parameter>(
                    GL_TEXTURE_IMMUTABLE_FORMAT),
                SpanOne(is_immutable));
        }
        if(!is_immutable)
            return decltype(out)(nullptr);
    }
#endif

    out->m_features.dsa = false;
    out->alloc({});
    auto arguments = std::make_tuple(
        out->m_handle.hnd,
        convert::to(TypeT::value),
        origin.m_handle.hnd,
        std::get<0>(convert::to<group::sized_internal_format>(params.format)),
        params.layer.min_,
        params.layer.count,
        params.mip.min_,
        params.mip.count);
#if GLEAM_MAX_VERSION >= 430
    if(origin.m_features.views)
        std::apply(cmd::texture_view, arguments);
    else
#endif
#if defined(GL_ARB_texture_view)
        if(origin.m_features.arb.texture_view)
        std::apply(gl::arb::texture_view::texture_view, arguments);
#endif
#if defined(GL_EXT_texture_view)
    if(origin.m_features.ext.texture_view)
        std::apply(gl::ext::texture_view::texture_view, arguments);
#endif
#if defined(GL_OES_texture_view)
    else if(origin.m_features.oes.texture_view)
        std::apply(gl::oes::texture_view::texture_view, arguments);
#endif
    return out;
}
#else
template<typename... Args>
inline auto make_texture_view(Args...)
{
    return nullptr;
}
#endif

template<class TypeT, class VectorT, class SizeT>
requires semantic::concepts::Vector<VectorT, u32, 3> &&
    semantic::concepts::Size3D<SizeT, u32>
inline auto make_texture_view(
    texture_t&                   origin,
    TypeT                        type,
    textures::view_params const& params,
    VectorT const&               offset,
    SizeT const&                 size)
{
    return nullptr;
}

template<class TypeT, class VectorT, class SizeT>
auto texture_t::view(
    TypeT                        type,
    textures::view_params const& params,
    VectorT const&               offset,
    SizeT const&                 size)
{
    return make_texture_view(*this, type, params, offset, size);
}

inline auto texture_t::sampler()
{
    if(!m_features.samplers)
        Throw(undefined_behavior("this isn't gonna go well anyways"));
#if GLEAM_MAX_VERSION_ES != 0x200
    return stl_types::MkShared<sampler_t>(this->shared_from_this());
#else
    return nullptr;
#endif
}

} // namespace gleam
