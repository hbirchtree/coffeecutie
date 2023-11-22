#pragma once

#include "rhi_buffer.h"
#include "rhi_debug.h"
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

#include <glw/texture_format.h>

#include <coffee/core/task_queue/task.h>
#include <future>

namespace gleam {

struct sampler_t;

struct texture_t : std::enable_shared_from_this<texture_t>
{
    static constexpr auto debug_identifier = group::object_identifier::texture;

    using size_type = size_3d<u32>;

    texture_t(
        features::textures const& features,
        rq::runtime_queue*&       decoder_queue,
        debug::api&               debug,
        textures::type            type,
        PixDesc const&            data,
        u32                       mipmaps,
        textures::property        properties) :
        m_features(features),
        m_debug(debug), m_decoder_queue(decoder_queue), m_format(data),
        m_type(type), m_mipmaps(mipmaps), m_flags(properties)
    {
    }
    virtual ~texture_t() = default;

    virtual void alloc(size_type const& size, bool create_storage = true);
    inline void  dealloc()
    {
        cmd::delete_textures(SpanOne<u32>(m_handle));
        m_handle.release();
    }

    size_type size();

    template<class TypeT, typename... Args>
    inline auto view(
        TypeT type, textures::view_params const& params, Args&&... args);

    inline auto sampler();

#if defined(GLEAM_ENABLE_SOFTWARE_BCN) || defined(GLEAM_ENABLE_SOFTWARE_PVRTC)
    bool                   requires_software_decode();
    std::optional<PixDesc> software_decode_format();

    std::future<std::vector<char>> software_decode(
        semantic::Span<const char>&& data, size_3d<i32> size, i32 mipmap);
    template<class Data, class SizeT>
    requires(SizeT::length() == 3)
        //
        inline auto software_decode_cast(
            Data&& data, SizeT const& size, i32 mipmap)
    {
        return software_decode(
            semantic::mem_chunk<const char>::ofBytes(data.data(), data.size())
                .view,
            size_3d<i32>{size[0], size[1], size[2]},
            mipmap);
    }
    template<class Data, class SizeT>
    requires(SizeT::length() == 2)
        //
        inline auto software_decode_cast(
            Data&& data, SizeT const& size, i32 mipmap)
    {
        return software_decode(
            semantic::mem_chunk<const char>::ofBytes(data.data(), data.size())
                .view,
            size_3d<i32>{size[0], size[1], 1},
            mipmap);
    }
#else
    constexpr bool requires_software_decode()
    {
        return false;
    }
    std::optional<PixDesc> software_decode_format()
    {
        return std::nullopt;
    }
    template<class Data, class SizeT>
    inline auto software_decode_cast(Data&&, SizeT const&, i32)
    {
        return std::future<std::vector<char>>();
    }
#endif

    gl::tex::texture_format_t const& format_description() const;

    inline void set_channel_swizzle(
        group::texture_parameter_name      channel,
        std::optional<textures::swizzle_t> value)
    {
        if(!value.has_value())
            return;
#if GLEAM_MAX_VERSION >= 0x450
        if(m_features.dsa)
        {
            cmd::texture_parameter(
                m_handle, channel, static_cast<i32>(convert::to(*value)));
        } else
#endif
        {
            cmd::bind_texture(convert::to(m_type), m_handle);
            cmd::tex_parameter(
                convert::to(m_type),
                channel,
                static_cast<i32>(convert::to(*value)));
            cmd::bind_texture(convert::to(m_type), 0);
        }
    }

    inline void set_swizzle(
        std::optional<textures::swizzle_t> red,
        std::optional<textures::swizzle_t> green,
        std::optional<textures::swizzle_t> blue,
        std::optional<textures::swizzle_t> alpha)
    {
        set_channel_swizzle(
            group::texture_parameter_name::texture_swizzle_r, red);
        set_channel_swizzle(
            group::texture_parameter_name::texture_swizzle_g, green);
        set_channel_swizzle(
            group::texture_parameter_name::texture_swizzle_b, blue);
        set_channel_swizzle(
            group::texture_parameter_name::texture_swizzle_a, alpha);
    }

    features::textures  m_features{};
    debug::api&         m_debug;
    rq::runtime_queue*& m_decoder_queue;
    PixDesc             m_format{};
    hnd                 m_handle{0};
    textures::type      m_type{textures::type::d2};
    u32                 m_mipmaps{1};
    size_2d<u32>        m_tex_size;
    u32                 m_layers{1};
    textures::property  m_flags{textures::property::none};
};

struct sampler_t
{
    sampler_t(std::shared_ptr<texture_t> const& source) :
        m_source(source), m_features(source->m_features), m_type(source->m_type)
    {
    }

    inline void alloc()
    {
#if GLEAM_MAX_VERSION_ES != 0x200
        if(m_features.samplers)
        {
            [[maybe_unused]] auto _ = m_source.lock()->m_debug.scope();
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
            [[maybe_unused]] auto _ = m_source.lock()->m_debug.scope();
            cmd::delete_samplers(SpanOne<u32>(m_handle));
            m_handle.release();
        }
#endif
    }

    inline void set_lod(typing::vector_types::Vecf2 const& range)
    {
#if GLEAM_MAX_VERSION_ES != 0x200
        if(m_features.samplers)
        {
            [[maybe_unused]] auto _ = m_source.lock()->m_debug.scope();
            cmd::sampler_parameter(
                m_handle,
                group::sampler_parameter_f::texture_min_lod,
                SpanOne(range[0]));
            cmd::sampler_parameter(
                m_handle,
                group::sampler_parameter_f::texture_max_lod,
                SpanOne(range[1]));
        } else
#endif
        {
            m_lodRange = range;
        }
    }

    inline void set_anisotropic([[maybe_unused]] f32 samples)
    {
        [[maybe_unused]] auto _         = m_source.lock()->m_debug.scope();
        [[maybe_unused]] i32  max_aniso = 0;
#if GLEAM_MAX_VERSION >= 0x460
        if(m_features.anisotropy)
        {
            cmd::get_integerv(
                group::get_prop::max_texture_max_anisotropy,
                SpanOne(max_aniso));
            if(samples > max_aniso)
                Throw(
                    std::out_of_range("anisotropic sample value out of range"));
            cmd::sampler_parameter(
                m_handle,
                group::sampler_parameter_f::texture_max_anisotropy,
                SpanOne(samples));
            return;
        }
#endif
#if defined(GL_EXT_texture_filter_anisotropic) && GLEAM_MAX_VERSION_ES != 0x200
        if(m_features.ext.texture_anisotropic)
        {
            cmd::get_integerv(
                group::get_prop::max_texture_max_anisotropy_ext,
                SpanOne(max_aniso));
            if(samples > max_aniso)
                Throw(
                    std::out_of_range("anisotropic sample value out of range"));
            cmd::sampler_parameter(
                m_handle,
                group::sampler_parameter_f::texture_max_anisotropy_ext,
                SpanOne(samples));
        }
#endif
    }

    inline void set_filtering(
        typing::Filtering mag,
        typing::Filtering min,
        typing::Filtering mip = typing::Filtering::None)
    {
#if GLEAM_MAX_VERSION_ES != 0x200
        if(m_features.samplers)
        {
            [[maybe_unused]] auto _ = m_source.lock()->m_debug.scope();
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

    std::weak_ptr<texture_t>    m_source;
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
        auto [ifmt1, type, layout] = convert::to<group::internal_format>(
            software_decode_format().value_or(m_format), m_features);
        auto is_compressed = format_description().is_compressed()
                             && !requires_software_decode();

#if GLEAM_MAX_VERSION >= 0x450
        if(m_features.dsa && is_compressed)
        {
            cmd::compressed_texture_sub_image_2d(
                m_handle, level, offset, size, ifmt1, data);
        } else if(m_features.dsa && requires_software_decode())
        {
            auto bits = software_decode_cast(std::move(data), size, level);
            rq::runtime_queue::Queue(
                rq::runtime_queue::GetCurrentQueue().value(),
                rq::dependent_task<std::vector<char>, void>::CreateSink(
                    std::move(bits),
                    [this, offset, size, level, layout = layout, type = type](
                        std::vector<char> const* bits) {
                        cmd::texture_sub_image_2d(
                            m_handle, level, offset, size, layout, type, *bits);
                    }))
                .has_value();
        } else if(m_features.dsa)
        {
            cmd::texture_sub_image_2d(
                m_handle, level, offset, size, layout, type, data);
        } else
#endif
            if(is_compressed)
        {
            cmd::bind_texture(group::texture_target::texture_2d, m_handle);
            cmd::compressed_tex_sub_image_2d(
                group::texture_target::texture_2d,
                level,
                offset,
                size,
                ifmt1,
                data);
            cmd::bind_texture(group::texture_target::texture_2d, 0);
        } else if(requires_software_decode())
        {
            auto bits = software_decode_cast(std::move(data), size, level);
            rq::runtime_queue::Queue(
                rq::runtime_queue::GetCurrentQueue().value(),
                rq::dependent_task<std::vector<char>, void>::CreateSink(
                    std::move(bits),
                    [this, offset, size, level, layout = layout, type = type](
                        std::vector<char> const* bits) {
                        cmd::bind_texture(
                            group::texture_target::texture_2d, m_handle);
                        cmd::tex_sub_image_2d(
                            group::texture_target::texture_2d,
                            level,
                            offset,
                            size,
                            layout,
                            type,
                            *bits);
                        cmd::bind_texture(group::texture_target::texture_2d, 0);
                    }))
                .has_value();
        } else
        {
            cmd::bind_texture(group::texture_target::texture_2d, m_handle);
            cmd::tex_sub_image_2d(
                group::texture_target::texture_2d,
                level,
                offset,
                size,
                layout,
                type,
                data);
            cmd::bind_texture(group::texture_target::texture_2d, 0);
        }
        return std::nullopt;
    }
};

#if GLEAM_MAX_VERSION >= 0x120 || GLEAM_MAX_VERSION_ES >= 0x300
struct texture_2da_t : texture_t
{
    using texture_t::texture_t;

    template<class T, class VectorT, class SizeT>
    std::optional<error> upload(
        T const& data, VectorT const& offset, SizeT const& size, i32 level = 0)
    {
        auto [ifmt1, type, layout] = convert::to<group::internal_format>(
            software_decode_format().value_or(m_format), m_features);
        auto is_compressed = format_description().is_compressed()
                             && !requires_software_decode();

#if GLEAM_MAX_VERSION >= 0x450
        if(m_features.dsa && is_compressed)
        {
            cmd::compressed_texture_sub_image_3d(
                m_handle, level, offset, size, ifmt1, data);
        } else if(m_features.dsa && requires_software_decode())
        {
            auto bits = software_decode_cast(std::move(data), size, level);
            rq::runtime_queue::Queue(
                rq::runtime_queue::GetCurrentQueue().value(),
                rq::dependent_task<std::vector<char>, void>::CreateSink(
                    std::move(bits),
                    [this, offset, size, level, layout = layout, type = type](
                        std::vector<char> const* bits) {
                        cmd::texture_sub_image_3d(
                            m_handle, level, offset, size, layout, type, *bits);
                    }))
                .has_value();
        } else if(m_features.dsa)
        {
            cmd::texture_sub_image_3d(
                m_handle, level, offset, size, layout, type, data);
        } else
#endif
            if(is_compressed)
        {
            cmd::bind_texture(
                group::texture_target::texture_2d_array, m_handle);
            cmd::compressed_tex_sub_image_3d(
                group::texture_target::texture_2d_array,
                level,
                offset,
                size,
                ifmt1,
                data);
            cmd::bind_texture(group::texture_target::texture_2d_array, 0);
        } else if(requires_software_decode())
        {
            auto bits = software_decode_cast(std::move(data), size, level);
            rq::runtime_queue::Queue(
                rq::runtime_queue::GetCurrentQueue().value(),
                rq::dependent_task<std::vector<char>, void>::CreateSink(
                    std::move(bits),
                    [this, offset, size, level, layout = layout, type = type](
                        std::vector<char> const* bits) {
                        cmd::bind_texture(
                            group::texture_target::texture_2d_array, m_handle);
                        cmd::tex_sub_image_3d(
                            group::texture_target::texture_2d_array,
                            level,
                            offset,
                            size,
                            layout,
                            type,
                            *bits);
                        cmd::bind_texture(
                            group::texture_target::texture_2d_array, 0);
                    }))
                .has_value();
        } else
        {
            cmd::bind_texture(
                group::texture_target::texture_2d_array, m_handle);
            cmd::tex_sub_image_3d(
                group::texture_target::texture_2d_array,
                level,
                offset,
                size,
                layout,
                type,
                data);
            cmd::bind_texture(group::texture_target::texture_2d_array, 0);
        }
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
        auto [ifmt1, type, layout] = convert::to<group::internal_format>(
            software_decode_format().value_or(m_format), m_features);
        auto is_compressed = format_description().is_compressed();
#if GLEAM_MAX_VERSION >= 0x450
        if(m_features.dsa && is_compressed)
        {
            cmd::compressed_texture_sub_image_3d(
                m_handle, level, offset, size, ifmt1, data);
        } else if(m_features.dsa)
        {
            cmd::texture_sub_image_3d(
                m_handle, level, offset, size, layout, type, data);
        } else
#endif
            if(is_compressed)
        {
            cmd::bind_texture(group::texture_target::texture_3d, m_handle);
            cmd::compressed_tex_sub_image_3d(
                group::texture_target::texture_3d,
                level,
                offset,
                size,
                ifmt1,
                data);
        } else
        {
            cmd::bind_texture(group::texture_target::texture_3d, m_handle);
            cmd::tex_sub_image_3d(
                group::texture_target::texture_3d,
                level,
                offset,
                size,
                layout,
                type,
                data);
        }
        return std::nullopt;
    }
};

#if GLEAM_MAX_VERSION >= 0x410 || GLEAM_MAX_VERSION_ES >= 0x320
struct texture_cube_array_t : texture_t
{
    using texture_t::texture_t;

    template<class T, class VectorT, class SizeT>
    std::optional<error> upload(
        std::array<T, 6> const& data,
        VectorT const&          offset,
        SizeT const&            size,
        i32                     level = 0)
    {
        auto [ifmt1, type, layout] = convert::to<group::internal_format>(
            software_decode_format().value_or(m_format), m_features);
        auto is_compressed = format_description().is_compressed()
                             && !requires_software_decode();
        VectorT offset_mul = offset;
        offset_mul[2]      = offset_mul[2] * 6;
        for(auto const& face : data)
        {
#if GLEAM_MAX_VERSION >= 0x450
            if(m_features.dsa && is_compressed)
            {
                cmd::compressed_texture_sub_image_3d(
                    m_handle, level, offset_mul, size, ifmt1, face);
            } else if(m_features.dsa)
            {
                cmd::texture_sub_image_3d(
                    m_handle, level, offset_mul, size, layout, type, face);
            } else
#endif
                if(is_compressed)
            {
                cmd::bind_texture(
                    group::texture_target::texture_cube_map_array, m_handle);
                cmd::compressed_tex_sub_image_3d(
                    group::texture_target::texture_cube_map_array,
                    level,
                    offset_mul,
                    size,
                    ifmt1,
                    face);
                cmd::bind_texture(
                    group::texture_target::texture_cube_map_array, 0);
            } else if(requires_software_decode())
            {
                auto bits = software_decode_cast(std::move(face), size, level);
                rq::runtime_queue::Queue(
                    rq::runtime_queue::GetCurrentQueue().value(),
                    rq::dependent_task<std::vector<char>, void>::CreateSink(
                        std::move(bits),
                        [this,
                         offset,
                         size,
                         level,
                         layout = layout,
                         type   = type](std::vector<char> const* bits) {
                            cmd::bind_texture(
                                group::texture_target::texture_cube_map_array,
                                m_handle);
                            cmd::tex_sub_image_3d(
                                group::texture_target::texture_cube_map_array,
                                level,
                                offset,
                                size,
                                layout,
                                type,
                                *bits);
                            cmd::bind_texture(
                                group::texture_target::texture_cube_map_array,
                                0);
                        }))
                    .has_value();
            } else
            {
                cmd::bind_texture(
                    group::texture_target::texture_cube_map_array, m_handle);
                cmd::tex_sub_image_3d(
                    group::texture_target::texture_cube_map_array,
                    level,
                    offset_mul,
                    size,
                    layout,
                    type,
                    face);
                cmd::bind_texture(
                    group::texture_target::texture_cube_map_array, 0);
            }
            offset_mul[2] += 1;
        }
        return std::nullopt;
    }
};
#endif
#endif

namespace detail {

template<typename Texture, typename Args>
inline void make_immutable_view(
    [[maybe_unused]] Texture& texture, [[maybe_unused]] Args&& arguments)
{
#if GLEAM_MAX_VERSION >= 0x430
    if(texture.m_features.views)
    {
        std::apply<decltype(cmd::texture_view<>)>(
            cmd::texture_view<>, std::move(arguments));
        return;
    }
#endif
#if defined(GL_ARB_texture_view)
    if(texture.m_features.arb.texture_view)
    {
        std::apply(gl::arb::texture_view::texture_view, arguments);
        return;
    }
#endif
#if defined(GL_EXT_texture_view)
    if(texture.m_features.ext.texture_view)
    {
        std::apply(gl::ext::texture_view::texture_view, arguments);
        return;
    }
#endif
#if defined(GL_OES_texture_view)
    if(texture.m_features.oes.texture_view)
    {
        std::apply(gl::oes::texture_view::texture_view, arguments);
        return;
    }
#endif
}

template<typename TypeT, typename View>
inline void make_copied_view(
    [[maybe_unused]] View&                        view,
    [[maybe_unused]] texture_t&                   texture,
    [[maybe_unused]] textures::view_params const& params)
{
#if GLEAM_MAX_VERSION >= 0x430 || GLEAM_MAX_VERSION_ES >= 0x320
    auto size_ = texture.size();
    if(texture.m_features.image_copy)
    {
        // clang-format off
        cmd::copy_image_sub_data(
            texture.m_handle,
            convert::to<group::copy_image_sub_data_target>(texture.m_type),
            params.mip.min_,
            0, 0, params.layer.min_,
            view.m_handle,
            convert::to<group::copy_image_sub_data_target>(view.m_type),
            0,
            0, 0, 0,
            size_.w, size_.h, params.layer.count);
        // clang-format on
    } else
#endif
    {
        // TODO: Something...
    }
}

} // namespace detail

#if GLEAM_MAX_VERSION_ES != 0x200                                  \
    && (GLEAM_MAX_VERSION >= 0x430 || defined(GL_EXT_texture_view) \
        || defined(GL_OES_texture_view))
template<
    class TypeT,
    class OutputType = typename std::conditional_t<
        std::is_same_v<TypeT, textures::type_d2>,
        texture_2d_t,
        typename std::conditional_t<
            std::is_same_v<TypeT, textures::type_d2_array>,
            texture_2da_t,
            texture_t>>>
requires std::is_same_v<TypeT, textures::type_d2>
inline auto make_texture_view(
    texture_t& origin,
    TypeT /*type*/,
    textures::view_params const& params,
    std::weak_ptr<OutputType>    existing_view = {})
{
    using output_type = OutputType;

    std::shared_ptr<output_type> out;

    if(auto view = existing_view.lock(); view)
        out = view;
    else
        out = std::make_shared<output_type>(
            origin.m_features,
            std::ref(origin.m_decoder_queue),
            std::ref(origin.m_debug),
            TypeT::value,
            params.format,
            params.mip.count,
            origin.m_flags);

    i32 is_immutable{0};
#if defined(GL_TEXTURE_IMMUTABLE_FORMAT)
#if GLEAM_MAX_VERSION >= 0x450
    if(origin.m_features.dsa)
    {
        cmd::get_texture_parameter(
            origin.m_handle,
            group::get_texture_parameter::texture_immutable_format,
            SpanOne(is_immutable));
    } else
#endif
    {
        cmd::bind_texture(convert::to(origin.m_type), origin.m_handle);
        cmd::get_tex_parameter(
            convert::to(origin.m_type),
            group::get_texture_parameter::texture_immutable_format,
            SpanOne(is_immutable));
    }
#endif

    out->m_features.dsa = false;

    if(is_immutable)
    {
        out->alloc({});
        auto arguments = std::make_tuple(
            out->m_handle.hnd,
            convert::to(TypeT::value),
            origin.m_handle.hnd,
            std::get<0>(convert::to<group::sized_internal_format>(
                params.format, origin.m_features)),
            params.layer.min_,
            params.layer.count,
            params.mip.min_,
            params.mip.count);
        detail::make_immutable_view(origin, std::move(arguments));
    } else if(origin.m_features.image_copy)
    {
        auto fmt = origin.software_decode_format().value_or(origin.m_format);
        auto uninitialized = out->m_handle == 0u;
        auto mismatched    = fmt.pixfmt != out->m_format.pixfmt
                          || fmt.bfmt != out->m_format.bfmt
                          || fmt.cmpflg != out->m_format.cmpflg
                          || origin.m_tex_size.w != out->m_tex_size.w
                          || origin.m_tex_size.h != out->m_tex_size.h;

        if(uninitialized || mismatched)
        {
            auto view_size = size_3d<u32>{
                .w = origin.m_tex_size.w,
                .h = origin.m_tex_size.h,
                .d = static_cast<u32>(params.layer.count),
            };
            if(!uninitialized)
                out->dealloc();
            out->m_format = params.format;
            out->alloc(view_size);
        }
        detail::make_copied_view<TypeT>(*out, origin, params);
    } else
        return decltype(out)(nullptr);

    return out;
}
#else
template<typename... Args>
inline auto make_texture_view(Args&&...)
{
    return nullptr;
}
#endif

template<class TypeT, typename... Args>
auto texture_t::view(
    TypeT type, textures::view_params const& params, Args&&... args)
{
    return make_texture_view(
        *this, type, params, std::forward<Args&&>(args)...);
}

inline auto texture_t::sampler()
{
    return std::make_shared<sampler_t>(this->shared_from_this());
}

enum class pbo_error
{
    no_error,
    no_buffer_provided,
    not_supported,
    texture_state_not_supported,
};

template<typename T, typename... Args>
[[nodiscard]] inline std::optional<pbo_error> texture_upload_from(
    texture_2d_t& texture, buffer_slice_t& pbo, Args&&... args)
{
    using buf_target = group::buffer_target_arb;
    using enum_helpers::feval;

    auto source_buffer = pbo.m_parent.lock();

    if(!source_buffer)
        return pbo_error::no_buffer_provided;
    if(!source_buffer->m_features.pbo)
        return pbo_error::not_supported;
    if(feval(texture.m_flags, textures::property::immutable))
        return pbo_error::texture_state_not_supported;

    cmd::bind_buffer(buf_target::pixel_unpack_buffer, source_buffer->m_handle);
    texture.upload(offset_span{pbo.m_offset}, std::forward<Args&&>(args)...);
    cmd::bind_buffer(buf_target::pixel_unpack_buffer, 0);
    return std::nullopt;
}

} // namespace gleam
