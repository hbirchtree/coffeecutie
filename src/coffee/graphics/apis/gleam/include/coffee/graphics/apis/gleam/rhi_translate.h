#pragma once

#include "rhi_versioning.h"

#include <glw/extensions/EXT_disjoint_timer_query.h>
#include <peripherals/concepts/graphics_api.h>
#include <peripherals/semantic/enum/data_types.h>
#include <peripherals/typing/enum/pixels/filtering.h>

namespace gleam::convert {
namespace mappings {
using stl_types::Array;
using stl_types::Pair;

constexpr Array<Pair<textures::type, group::texture_target>, 5> tex_type = {{
    {textures::type::d2, group::texture_target::texture_2d},
    {textures::type::cube, group::texture_target::texture_cube_map},
#if GLEAM_MAX_VERSION >= 0x300 || GLEAM_MAX_VERSION_ES >= 0x300
    {textures::type::d2_array, group::texture_target::texture_2d_array},
    {textures::type::d3, group::texture_target::texture_3d},
    {textures::type::cube_array, group::texture_target::texture_cube_map_array},
#endif
}};

constexpr Array<Pair<buffers::type, group::buffer_target_arb>, 6> buf_type = {{
    {buffers::type::vertex, group::buffer_target_arb::array_buffer},
    {buffers::type::element, group::buffer_target_arb::element_array_buffer},
#if GLEAM_MAX_VERSION >= 0x300 || GLEAM_MAX_VERSION_ES >= 0x300
    {buffers::type::pixel_unpack,
     group::buffer_target_arb::pixel_unpack_buffer},
    {buffers::type::pixel_pack, group::buffer_target_arb::pixel_pack_buffer},
    {buffers::type::shader_writable,
     group::buffer_target_arb::shader_storage_buffer},
    {buffers::type::constants, group::buffer_target_arb::uniform_buffer},
#endif
}};

constexpr Array<Pair<buffers::type, group::buffer_storage_target>, 6>
    buf_storage_type = {{
        {buffers::type::vertex, group::buffer_storage_target::array_buffer},
        {buffers::type::element,
         group::buffer_storage_target::element_array_buffer},
#if GLEAM_MAX_VERSION >= 0x300 || GLEAM_MAX_VERSION_ES >= 0x300
        {buffers::type::pixel_unpack,
         group::buffer_storage_target::pixel_unpack_buffer},
        {buffers::type::pixel_pack,
         group::buffer_storage_target::pixel_pack_buffer},
        {buffers::type::shader_writable,
         group::buffer_storage_target::shader_storage_buffer},
        {buffers::type::constants,
         group::buffer_storage_target::uniform_buffer},
#endif
    }};

constexpr Array<Pair<queries::type, group::query_target>, 5> query_type = {{
#if GLEAM_MAX_VERSION >= 0x300
    {queries::type::time, group::query_target::time_elapsed},
    {queries::type::fragments, group::query_target::samples_passed},
#endif
#if GLEAM_MAX_VERSION_ES >= 0x300
    {queries::type::fragments,
     group::query_target::any_samples_passed_conservative},
#endif
#if GLEAM_MAX_VERSION_ES >= 0x200 && defined(GL_EXT_disjoint_timer_query)
    {queries::type::time,
     static_cast<group::query_target>(
         gl::ext::disjoint_timer_query::values::time_elapsed)},
    {queries::type::timestamp,
     static_cast<group::query_target>(
         gl::ext::disjoint_timer_query::values::timestamp)},
#endif
}};

} // namespace mappings

using namespace semantic::concepts::graphics;
using stl_types::Pair;
using stl_types::Tup;

inline group::texture_target to(textures::type type)
{
    type    = type & textures::type::type_mask;
    auto it = std::find_if(
        mappings::tex_type.begin(),
        mappings::tex_type.end(),
        [type](auto const& v) { return type == v.first; });

    if(it == mappings::tex_type.end())
        Throw(undefined_behavior("failed to match mapping"));
    return it->second;
}

template<typename T = group::buffer_target_arb>
requires(std::is_same_v<T, group::buffer_target_arb>)
    //
    inline group::buffer_target_arb to(buffers::type type)
{
    auto it = std::find_if(
        mappings::buf_type.begin(),
        mappings::buf_type.end(),
        [type](auto const& v) { return type == v.first; });

    if(it == mappings::buf_type.end())
        Throw(undefined_behavior("failed to match mapping"));
    return it->second;
}

template<typename T>
requires(std::is_same_v<T, group::buffer_storage_target>)
    //
    inline group::buffer_storage_target to(buffers::type type)
{
    auto it = std::find_if(
        mappings::buf_storage_type.begin(),
        mappings::buf_storage_type.end(),
        [type](auto const& v) { return type == v.first; });

    if(it == mappings::buf_storage_type.end())
        Throw(undefined_behavior("failed to match mapping"));
    return it->second;
}

#if GLEAM_MAX_VERSION >= 0x150 || GLEAM_MAX_VERSION_ES >= 0x300 || \
    defined(GL_EXT_disjoint_timer_query)
inline group::query_target to(queries::type type)
{
    auto it = std::find_if(
        mappings::query_type.begin(),
        mappings::query_type.end(),
        [type](auto const& v) { return type == v.first; });

    if(it == mappings::query_type.end())
        Throw(undefined_behavior("failed to match mapping"));
    return it->second;
}
#endif

template<typename T>
requires std::is_same_v<T, group::sized_internal_format> ||
    std::is_same_v<T, group::internal_format>
inline Tup<T, group::pixel_type, group::pixel_format> to(PixDesc const& fmt)
{
    using ::enum_helpers::feval;

    using f = T;
    using b = group::pixel_type;
    using p = group::pixel_format;

    using P = typing::pixels::PixFmt;
    using F = typing::pixels::PixFlg;
    using C = typing::PixCmp;
    using M = typing::pixels::CompFlags;
    using B = typing::pixels::BitFmt;

    constexpr stl_types::Array<Pair<P, Tup<f, b, p>>, 22> direct_mapping = {{
#if GLEAM_MAX_VERSION >= 0x300 || GLEAM_MAX_VERSION_ES >= 0x300
        {P::R8, {f::r8, b::unsigned_byte, p::red}},
        {P::R16F, {f::r16f, b::float_, p::rgb}},
        {P::R32F, {f::r32f, b::float_, p::red}},
        {P::RG8, {f::rg8, b::unsigned_byte, p::rg}},
        {P::RG16F, {f::rg16f, b::float_, p::rgb}},
        {P::RG32F, {f::rg32f, b::float_, p::rg}},
#endif

    /* RGB */
#if defined(GL_RGB565) && defined(GL_UNSIGNED_SHORT_5_6_5)
        /* TODO: Ugly hack, needs workaround in wrapper layer */
        {P::RGB565,
         {static_cast<f>(GL_RGB565),
          static_cast<b>(GL_UNSIGNED_SHORT_5_6_5),
          p::rgb}},
#endif
#if GLEAM_MAX_VERSION >= 0x300 || GLEAM_MAX_VERSION_ES >= 0x300
        {P::RGB8, {f::rgb8, b::unsigned_byte, p::rgb}},
        {P::RGB16F, {f::rgb16f, b::float_, p::rgb}},
        {P::RGB32F, {f::rgb32f, b::float_, p::rgb}},
#endif
#if defined(GL_UNSIGNED_INT_5_9_9_9_REV)
        {P::RGB9E5,
         {f::rgb9_e5, static_cast<b>(GL_UNSIGNED_INT_5_9_9_9_REV), p::rgba}},
#endif
#if defined(GL_UNSIGNED_INT_10F_11F_11F_REV)
        {P::R11G11B10F,
         {f::r11f_g11f_b10f,
          static_cast<b>(GL_UNSIGNED_INT_10F_11F_11F_REV),
          p::rgb}},
#endif
#if defined(GL_ETC1_RGB8_OES)
        {P::ETC1, {static_cast<f>(GL_ETC1_RGB8_OES), b::unsigned_byte, p::rgb}},
#endif

        /* RGBA */
        {P::RGBA4, {f::rgba4, b::unsigned_short_4_4_4_4, p::rgba}},
        {P::RGB5A1, {f::rgb5_a1, b::unsigned_short_5_5_5_1, p::rgba}},
#if GLEAM_MAX_VERSION >= 0x300 || GLEAM_MAX_VERSION_ES >= 0x300
        {P::RGBA8, {f::rgba8, b::unsigned_byte, p::rgba}},
        {P::RGBA16F, {f::rgba16f, b::float_, p::rgba}},
        {P::RGBA32F, {f::rgba32f, b::float_, p::rgba}},
#endif

        /* Special formats */
        {P::Depth16,
         {f::depth_component16, b::unsigned_short, p::depth_component}},
#if GLEAM_MAX_VERSION >= 0x300 || GLEAM_MAX_VERSION_ES >= 0x300
        {P::Depth32F, {f::depth_component32f, b::float_, p::depth_component}},
        {P::Depth24Stencil8,
         {f::depth24_stencil8, b::unsigned_byte, p::depth_stencil}},
        {P::Depth32FStencil8,
         {f::depth32f_stencil8, b::unsigned_byte, p::depth_stencil}},
#endif
    }};

    auto it = std::find_if(
        direct_mapping.begin(),
        direct_mapping.end(),
        [pix = fmt.pixfmt](auto const& v) { return v.first == pix; });
    if(it != direct_mapping.end())
        return it->second;

    switch(fmt.pixfmt)
    {
#if defined(GL_KHR_texture_compression_astc_ldr)
    case P::ASTC: {
        switch(fmt.cmpflg)
        {
        case M::ASTC_4x4:
            return {f::compressed_rgba_astc_4x4_khr, b::unsigned_byte, p::rgba};
        case M::ASTC_8x8:
            return {f::compressed_rgba_astc_8x8_khr, b::unsigned_byte, p::rgba};
        case M::ASTC_10x10:
            return {
                f::compressed_rgba_astc_10x10_khr, b::unsigned_byte, p::rgba};
        case M::ASTC_12x12:
            return {
                f::compressed_rgba_astc_12x12_khr, b::unsigned_byte, p::rgba};
        default:
            break;
        }
        break;
    }
#endif
    case P::BCn: {
        switch(fmt.cmpflg)
        {
        case M::BC1: {
#if defined(GL_EXT_texture_compression_s3tc_srgb)
            if(feval(fmt.pixflg & F::sRGB))
            {
                return {
                    f::compressed_srgb_s3tc_dxt1_ext, b::unsigned_byte, p::rgb};
            }
#endif
#if defined(GL_EXT_texture_compression_s3tc) || \
    defined(GL_EXT_texture_compression_dxt1)
            switch(fmt.comp)
            {
            case C::RGB:
                return {
                    f::compressed_rgb_s3tc_dxt1_ext, b::unsigned_byte, p::rgb};
            case C::RGBA:
                return {
                    f::compressed_rgba_s3tc_dxt1_ext,
                    b::unsigned_byte,
                    p::rgba};
            default:
                break;
            }
#endif
            break;
        }
#if defined(GL_EXT_texture_compression_s3tc)
        case M::BC2:
            return {
                f::compressed_rgba_s3tc_dxt3_ext, b::unsigned_byte, p::rgba};
        case M::BC3:
            return {
                f::compressed_rgba_s3tc_dxt5_ext, b::unsigned_byte, p::rgba};
#endif
#if GLEAM_MAX_VERSION >= 0x300
        case M::BC4:
            return {f::compressed_red_rgtc1, b::unsigned_byte, p::red};
        case M::BC5:
            return {f::compressed_rg_rgtc2, b::unsigned_byte, p::rg};
#endif
#if GLEAM_MAX_VERSION >= 0x420
        case M::BC6H:
            return {
                f::compressed_rgb_bptc_unsigned_float,
                b::unsigned_byte,
                p::rgb};
        case M::BC7:
            return {f::compressed_rgba_bptc_unorm, b::unsigned_byte, p::rgba};
#endif
        default:
            break;
        }
    }
#if GLEAM_MAX_VERSION >= 0x430 || GLEAM_MAX_VERSION_ES >= 0x300
    case P::ETC2: {
        switch(fmt.comp)
        {
        case C::RGB:
            return {f::compressed_rgb8_etc2, b::unsigned_byte, p::rgb};
        case C::RGBA: {
            if((fmt.pixflg & F::RGBA_Punchthrough) == F::RGBA_Punchthrough)
                return {
                    f::compressed_rgb8_punchthrough_alpha1_etc2,
                    b::unsigned_byte,
                    p::rgb};
            else
                return {f::compressed_rgba8_etc2_eac, b::unsigned_byte, p::rgb};
            break;
        }
        default:
            break;
        }
    }
#endif
    default:
        break;
    }
    Throw(undefined_behavior("unhandled pixel format"));
}

template<typename T = group::framebuffer_attachment>
requires std::is_same_v<T, group::framebuffer_attachment>
inline group::framebuffer_attachment to(
    render_targets::attachment attachment, u32 i)
{
    using attach = group::framebuffer_attachment;

    switch(attachment)
    {
    case render_targets::attachment::color:
        return static_cast<attach>(
            static_cast<u32>(attach::color_attachment0) + i);
    case render_targets::attachment::depth:
        return attach::depth_attachment;
    case render_targets::attachment::stencil:
        return attach::stencil_attachment;
#if defined(GL_DEPTH_STENCIL_ATTACHMENT)
    case render_targets::attachment::depth_stencil:
        return static_cast<attach>(GL_DEPTH_STENCIL_ATTACHMENT);
#endif
    default:
        break;
    }
    Throw(undefined_behavior("unhandled attachment type"));
}

template<typename T>
requires std::is_same_v<T, group::read_buffer_mode> ||
    std::is_same_v<T, group::color_buffer>
inline T to(render_targets::attachment attachment, u32 i)
{
    using mode = T;
    switch(attachment)
    {
    case render_targets::attachment::front:
        return mode::front;
    case render_targets::attachment::back:
        return mode::back;
    case render_targets::attachment::color:
        return static_cast<mode>(static_cast<u32>(mode::color_attachment0) + i);
    default:
        break;
    }
    Throw(undefined_behavior("unhandled attachment type"));
}

template<typename T = group::buffer_usage_arb>
requires std::is_same_v<T, group::buffer_usage_arb>
inline group::buffer_usage_arb to(semantic::RSCA flags)
{
    using enum_helpers::feval;
    using semantic::RSCA;
    if(feval(flags, RSCA::Persistent))
    {
#if GLEAM_MAX_VERSION >= 0x200 || GLEAM_MAX_VERSION_ES >= 0x300
        if(feval(flags, RSCA::ReadWrite))
            return group::buffer_usage_arb::dynamic_copy;
        if(feval(flags, RSCA::ReadOnly))
            return group::buffer_usage_arb::dynamic_read;
#endif
        return group::buffer_usage_arb::dynamic_draw;
    }

    if(feval(flags, RSCA::Streaming))
    {
#if GLEAM_MAX_VERSION >= 0x200 || GLEAM_MAX_VERSION_ES >= 0x300
        if(feval(flags, RSCA::ReadWrite | RSCA::Streaming))
            return group::buffer_usage_arb::stream_copy;
        if(feval(flags, RSCA::ReadOnly | RSCA::Streaming))
            return group::buffer_usage_arb::stream_read;
#endif
        return group::buffer_usage_arb::stream_draw;
    }

#if GLEAM_MAX_VERSION >= 0x200 || GLEAM_MAX_VERSION_ES >= 0x300
    if(feval(flags, RSCA::ReadOnly))
        return group::buffer_usage_arb::static_draw;
    if(feval(flags, RSCA::WriteOnly))
        return group::buffer_usage_arb::static_read;
#endif
    return group::buffer_usage_arb::static_draw;
}

#if GLEAM_MAX_VERSION >= 0x300 || GLEAM_MAX_VERSION_ES >= 0x300
template<typename T>
requires std::is_same_v<T, group::buffer_storage_mask>
inline group::buffer_storage_mask to(semantic::RSCA flags)
{
    using enum_helpers::feval;
    using semantic::RSCA;
    using mask = group::buffer_storage_mask;
    auto out   = static_cast<mask>(0);
    if(feval(flags, RSCA::ReadOnly))
        out |= mask::map_read_bit;
    if(feval(flags, RSCA::WriteOnly))
        out |= mask::map_write_bit;
#if GLEAM_MAX_VERSION >= 0x440
    if(feval(flags, RSCA::Persistent))
        out |= mask::map_persistent_bit | mask::map_coherent_bit;
    if(feval(flags, RSCA::Streaming))
        out |= mask::client_storage_bit;
#endif
#if defined(GL_SPARSE_STORAGE_BIT_ARB)
    if(feval(flags, RSCA::Sparse))
        out |= mask::sparse_storage_bit_arb;
#endif
    /* Skipping dynamic storage */
    return out;
}
#endif

#if GLEAM_MAX_VERSION >= 0x300 || GLEAM_MAX_VERSION_ES >= 0x300 || \
    defined(GL_OES_mapbuffer)
template<typename T>
requires std::is_same_v<T, group::buffer_access_arb>
inline group::buffer_access_arb to(semantic::RSCA flags)
{
    using enum_helpers::feval;
    using semantic::RSCA;
    if(feval(flags, RSCA::ReadWrite))
        return group::buffer_access_arb::read_write;
    if(feval(flags, RSCA::WriteOnly))
        return group::buffer_access_arb::write_only;
    return group::buffer_access_arb::read_only;
}
#endif

#if GLEAM_MAX_VERSION >= 0x300 || GLEAM_MAX_VERSION_ES >= 0x300
template<typename T>
requires(std::is_same_v<T, group::map_buffer_access_mask>) inline group::
    map_buffer_access_mask to(semantic::RSCA flags, bool whole_buffer)
{
    using enum_helpers::feval;
    using semantic::RSCA;
    using mask = group::map_buffer_access_mask;
    auto out   = mask::map_unsynchronized_bit;
    if(feval(flags, RSCA::ReadOnly))
        out |= mask::map_read_bit;
    if(feval(flags, RSCA::WriteOnly))
        out |= mask::map_write_bit;
#if GLEAM_MAX_VERSION >= 0x440
    if(feval(flags, RSCA::Persistent))
        out |= mask::map_persistent_bit | mask::map_coherent_bit;
#endif
    if(feval(flags, RSCA::Discard))
        out |= whole_buffer ? mask::map_invalidate_buffer_bit
                            : mask::map_invalidate_range_bit;
    /* Skipping dynamic storage */
    return out;
}
#endif

namespace detail {

template<typename T>
requires std::is_same_v<T, group::vertex_attrib_type>
inline T vertex_type_to(semantic::TypeEnum type)
{
    using semantic::TypeEnum;
    switch(type)
    {
    case TypeEnum::Scalar:
        return T::float_;
#if GLEAM_MAX_VERSION >= 0x150
    case TypeEnum::Packed_UFloat:
        return T::unsigned_int_10f_11f_11f_rev;
    case TypeEnum::BigScalar:
        return T::double_;
#endif
    default:
        Throw(undefined_behavior("unhandled vertex type"));
    }
}

template<typename T>
requires std::is_same_v<T, group::vertex_attrib_pointer_type>
inline T vertex_type_to(semantic::TypeEnum type)
{
    using semantic::TypeEnum;
    switch(type)
    {
    case TypeEnum::Scalar:
        return T::float_;
#if GLEAM_MAX_VERSION >= 0x150
    case TypeEnum::Packed_UFloat:
        return T::unsigned_int_10f_11f_11f_rev;
    case TypeEnum::BigScalar:
        return T::double_;
#endif
#if defined(GL_UNSIGNED_INT64_ARB) && defined(GL_INT64_ARB)
    case TypeEnum::LL:
        return T::int64_arb;
    case TypeEnum::ULL:
        return T::unsigned_int64_arb;
#endif
    default:
        Throw(undefined_behavior("unhandled vertex type"));
    }
}

template<typename T>
requires std::is_same_v<T, group::vertex_attrib_int>
inline T vertex_type_to(semantic::TypeEnum type)
{
    Throw(undefined_behavior("unhandled vertex type"));
}

} // namespace detail

template<typename T>
requires std::is_same_v<T, group::vertex_attrib_int> ||
    std::is_same_v<T, group::vertex_attrib_pointer_type> ||
    std::is_same_v<T, group::vertex_attrib_type>
inline T to(semantic::TypeEnum type)
{
    using semantic::TypeEnum;
    switch(type)
    {
    case TypeEnum::Byte:
        return T::byte_;
    case TypeEnum::UByte:
        return T::unsigned_byte;
    case TypeEnum::Short:
        return T::short_;
    case TypeEnum::UShort:
        return T::unsigned_short;
#if GLEAM_MAX_VERSION >= 0x100 || GLEAM_MAX_VERSION_ES >= 0x300
    case TypeEnum::Int:
        return T::int_;
    case TypeEnum::UInt:
        return T::unsigned_int;
#endif
    default:
        return detail::vertex_type_to<T>(type);
    }
}

template<typename T>
requires std::is_same_v<T, group::shader_type>
inline group::shader_type to(typing::graphics::ShaderStage stage)
{
    using stage_t = typing::graphics::ShaderStage;
    switch(stage)
    {
    case stage_t::Vertex:
        return group::shader_type::vertex_shader;
    case stage_t::Fragment:
        return group::shader_type::fragment_shader;
#if GLEAM_MAX_VERSION >= 0x430 || GLEAM_MAX_VERSION_ES >= 0x310
    case stage_t::Compute:
        return group::shader_type::compute_shader;
#endif
    default:
        Throw(undefined_behavior("unhandled shader type"));
    }
}

#if GLEAM_MAX_VERSION >= 0x430 || GLEAM_MAX_VERSION_ES >= 0x310
template<typename T>
requires std::is_same_v<T, group::use_program_stage_mask>
inline group::use_program_stage_mask to(typing::graphics::ShaderStage stage)
{
    using stage_t    = typing::graphics::ShaderStage;
    using stage_mask = group::use_program_stage_mask;
    switch(stage)
    {
    case stage_t::Vertex:
        return stage_mask::vertex_shader_bit;
    case stage_t::Fragment:
        return stage_mask::fragment_shader_bit;
    case stage_t::Compute:
        return stage_mask::compute_shader_bit;
    default:
        Throw(undefined_behavior("unhandled shader stage"));
    }
}
#endif

inline group::primitive_type to(drawing::primitive primitive)
{
    auto type      = primitive & drawing::primitive::primitive_mask;
    auto construct = primitive & drawing::primitive::construction_mask;

    switch(type)
    {
    case drawing::primitive::point:
        return group::primitive_type::points;
    case drawing::primitive::line:
        switch(construct)
        {
        case drawing::primitive::strip:
            return group::primitive_type::line_strip;
        default:
            return group::primitive_type::lines;
        }
    case drawing::primitive::triangle:
        switch(construct)
        {
        case drawing::primitive::strip:
            return group::primitive_type::triangle_strip;
        case drawing::primitive::fan:
            return group::primitive_type::triangle_fan;
        default:
            return group::primitive_type::triangles;
        }
    default:
        Throw(undefined_behavior("invalid primitive type"));
    }
}

template<typename T>
requires std::is_same_v<T, group::draw_elements_type>
inline group::draw_elements_type to(semantic::TypeEnum type)
{
    switch(type)
    {
    case semantic::TypeEnum::UInt:
        return group::draw_elements_type::unsigned_int;
    case semantic::TypeEnum::UShort:
        return group::draw_elements_type::unsigned_short;
    case semantic::TypeEnum::UByte:
        return group::draw_elements_type::unsigned_byte;
    default:
        Throw(undefined_behavior("invalid draw element type"));
    }
}

template<typename T>
requires std::is_same_v<T, group::sampler_parameter_i>
inline GLenum to(
    typing::Filtering f1, typing::Filtering f2 = typing::Filtering::None)
{
    using F = typing::Filtering;

    switch(f1)
    {
    case F::Linear: {
        switch(f2)
        {
        case F::Linear:
            return GL_LINEAR_MIPMAP_LINEAR;
        case F::Nearest:
            return GL_LINEAR_MIPMAP_NEAREST;
        default:
            return GL_LINEAR;
        }
        break;
    }
    case F::Nearest: {
        switch(f2)
        {
        case F::Linear:
            return GL_NEAREST_MIPMAP_LINEAR;
        case F::Nearest:
            return GL_NEAREST_MIPMAP_NEAREST;
        default:
            return GL_NEAREST;
        }
        break;
    }
    default:
        break;
    }
    return GL_NEAREST;
}

} // namespace gleam::convert
