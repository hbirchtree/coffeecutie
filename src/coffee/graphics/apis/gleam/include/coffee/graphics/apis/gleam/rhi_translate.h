#pragma once

#include "rhi_features.h"
#include "rhi_versioning.h"

#include <glw/extensions/EXT_disjoint_timer_query.h>
#include <glw/extensions/OES_rgb8_rgba8.h>

#include <glw/extensions/ARB_texture_compression_bptc.h>
#include <glw/extensions/ARB_texture_compression_rgtc.h>
#include <glw/extensions/EXT_texture_compression_bptc.h>
#include <glw/extensions/EXT_texture_compression_dxt1.h>
#include <glw/extensions/EXT_texture_compression_rgtc.h>
#include <glw/extensions/EXT_texture_compression_s3tc.h>
#include <glw/extensions/EXT_texture_compression_s3tc_srgb.h>
#include <glw/extensions/IMG_texture_compression_pvrtc.h>
#include <glw/extensions/IMG_texture_compression_pvrtc2.h>
#include <glw/extensions/KHR_texture_compression_astc_hdr.h>
#include <glw/extensions/KHR_texture_compression_astc_ldr.h>
#include <glw/extensions/OES_compressed_ETC1_RGB8_texture.h>

#include <peripherals/concepts/graphics_api.h>
#include <peripherals/semantic/enum/data_types.h>
#include <peripherals/stl/any_of.h>
#include <peripherals/typing/enum/pixels/filtering.h>

namespace gl::group {

inline texture_unit operator+(texture_unit base_unit, libc_types::u32 index)
{
    return static_cast<texture_unit>(static_cast<u32>(base_unit) + index);
}

} // namespace gl::group

namespace gleam::convert {
namespace mappings {
using std::array;
using std::pair;

constexpr array<pair<textures::type, group::texture_target>, 5> tex_type = {{
    {textures::type::d2, group::texture_target::texture_2d},
    {textures::type::cube, group::texture_target::texture_cube_map},
#if GLEAM_MAX_VERSION >= 0x300 || GLEAM_MAX_VERSION_ES >= 0x300
    {textures::type::d2_array, group::texture_target::texture_2d_array},
    {textures::type::d3, group::texture_target::texture_3d},
#endif
#if GLEAM_MAX_VERSION >= 0x300 || GLEAM_MAX_VERSION_ES >= 0x310
    {textures::type::cube_array, group::texture_target::texture_cube_map_array},
#endif
}};

constexpr array<pair<buffers::type, group::buffer_target_arb>, 7> buf_type = {{
    {buffers::type::vertex, group::buffer_target_arb::array_buffer},
    {buffers::type::element, group::buffer_target_arb::element_array_buffer},
#if GLEAM_MAX_VERSION >= 0x300 || GLEAM_MAX_VERSION_ES >= 0x300
    {buffers::type::pixel_unpack,
     group::buffer_target_arb::pixel_unpack_buffer},
    {buffers::type::pixel_pack, group::buffer_target_arb::pixel_pack_buffer},
    {buffers::type::constants, group::buffer_target_arb::uniform_buffer},
#endif
#if GLEAM_MAX_VERSION >= 0x430 || GLEAM_MAX_VERSION_ES >= 0x310
    {buffers::type::shader_writable,
     group::buffer_target_arb::shader_storage_buffer},
    {buffers::type::draw, group::buffer_target_arb::draw_indirect_buffer},
#endif
}};

constexpr array<pair<buffers::type, group::buffer_storage_target>, 7>
    buf_storage_type = {{
        {buffers::type::vertex, group::buffer_storage_target::array_buffer},
        {buffers::type::element,
         group::buffer_storage_target::element_array_buffer},
#if GLEAM_MAX_VERSION >= 0x300 || GLEAM_MAX_VERSION_ES >= 0x300
        {buffers::type::pixel_unpack,
         group::buffer_storage_target::pixel_unpack_buffer},
        {buffers::type::pixel_pack,
         group::buffer_storage_target::pixel_pack_buffer},
        {buffers::type::constants,
         group::buffer_storage_target::uniform_buffer},
#endif
#if GLEAM_MAX_VERSION >= 0x430 || GLEAM_MAX_VERSION_ES >= 0x310
        {buffers::type::shader_writable,
         group::buffer_storage_target::shader_storage_buffer},
        {buffers::type::draw,
         group::buffer_storage_target::draw_indirect_buffer},
#endif
    }};

constexpr array<pair<queries::type, group::query_target>, 5> query_type = {{
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
#endif
#if 0
    // TODO: Where did this come from
    {queries::type::timestamp,
     static_cast<group::query_target>(
         gl::ext::disjoint_timer_query::values::timestamp)},
#endif
}};

} // namespace mappings

using namespace semantic::concepts::graphics;

template<typename T = group::texture_target>
requires std::is_same_v<T, group::texture_target>
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

#if GLEAM_MAX_VERSION >= 0x430 || GLEAM_MAX_VERSION_ES >= 0x320
template<typename T>
requires std::is_same_v<T, group::copy_image_sub_data_target>
inline group::copy_image_sub_data_target to(textures::type type)
{
    using std::array;
    using std::pair;
    using copy_t = group::copy_image_sub_data_target;

    constexpr array<pair<textures::type, copy_t>, 5> tex_copy_type = {{
        {textures::type::d2, copy_t::texture_2d},
        {textures::type::cube, copy_t::texture_cube_map},
        {textures::type::d2_array, copy_t::texture_2d_array},
        {textures::type::d3, copy_t::texture_3d},
        {textures::type::cube_array, copy_t::texture_cube_map_array},
    }};

    type = type & textures::type::type_mask;

    auto it = std::find_if(
        tex_copy_type.begin(), tex_copy_type.end(), [type](auto const& v) {
            return type == v.first;
        });

    if(it == tex_copy_type.end())
        Throw(undefined_behavior("failed to match mapping"));
    return it->second;
}
#endif

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

namespace detail {

template<typename T>
requires std::is_same_v<T, group::internal_format>
inline std::
    tuple<group::internal_format, group::pixel_type, group::pixel_format>
    to_internal(PixDesc const& fmt)
{
    using f = group::internal_format;
    using b = group::pixel_type;
    using p = group::pixel_format;

    using P = typing::pixels::PixFmt;
    using B = typing::pixels::BitFmt;

    if(fmt.pixfmt == P::RGBA8 && fmt.bfmt == B::UByte)
        return {f::rgba, b::unsigned_byte, p::rgba};
    if(fmt.pixfmt == P::RGB8 && fmt.bfmt == B::UByte)
        return {f::rgb, b::unsigned_byte, p::rgb};

    if(fmt.pixfmt == P::RGB565 && fmt.bfmt == B::UShort_565)
        return {f::rgb, b::unsigned_short_5_6_5, p::rgb};
    if(fmt.pixfmt == P::RGB5A1 && fmt.bfmt == B::UShort_5551)
        return {f::rgba, b::unsigned_short_5_5_5_1, p::rgba};
    if(fmt.pixfmt == P::RGBA4 && fmt.bfmt == B::UShort_4444)
        return {f::rgba, b::unsigned_short_4_4_4_4, p::rgba};
    return {};
}

template<typename T>
requires std::is_same_v<T, group::sized_internal_format>
inline std::
    tuple<group::sized_internal_format, group::pixel_type, group::pixel_format>
    to_internal(PixDesc const&)
{
    return {};
}

} // namespace detail

// clang-format off
template<typename T>
requires std::is_same_v<T, group::sized_internal_format> || std::
    is_same_v<T, group::internal_format>
        //
std::tuple<T, group::pixel_type, group::pixel_format> to(
    PixDesc const& fmt, [[maybe_unused]] features::textures const& features);

// clang-format on

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
requires stl_types::
    is_any_of<T, group::buffer_usage_arb, group::vertex_buffer_object_usage>
    inline T to(
        [[maybe_unused]] features::buffers const& features,
        semantic::RSCA                            flags)
{
    using enum_helpers::feval;
    using semantic::RSCA;
    if(feval(flags, RSCA::Persistent) && features.mapping)
    {
#if GLEAM_MAX_VERSION >= 0x200 || GLEAM_MAX_VERSION_ES >= 0x300
        if(feval(flags, RSCA::ReadWrite))
            return T::dynamic_copy;
        if(feval(flags, RSCA::ReadOnly))
            return T::dynamic_read;
#endif
        return T::dynamic_draw;
    }

    if(feval(flags, RSCA::Streaming) && features.mapping)
    {
#if GLEAM_MAX_VERSION >= 0x200 || GLEAM_MAX_VERSION_ES >= 0x300
        if(feval(flags, RSCA::ReadWrite | RSCA::Streaming))
            return T::stream_copy;
        if(feval(flags, RSCA::ReadOnly | RSCA::Streaming))
            return T::stream_read;
#endif
        return T::stream_draw;
    }

#if GLEAM_MAX_VERSION >= 0x200 || GLEAM_MAX_VERSION_ES >= 0x300
    if(feval(flags, RSCA::ReadOnly))
        return T::static_draw;
    if(feval(flags, RSCA::WriteOnly) && features.mapping)
        return T::static_read;
#endif
    return T::static_draw;
}

#if GLEAM_MAX_VERSION >= 0x440
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

#if GLEAM_MAX_VERSION >= 0x300 || GLEAM_MAX_VERSION_ES >= 0x310 || \
    defined(GL_OES_mapbuffer)
template<typename T>
requires std::is_same_v<T, group::buffer_access_arb>
inline group::buffer_access_arb to(
    [[maybe_unused]] features::buffers const& features,
    [[maybe_unused]] semantic::RSCA           flags)
{
    using enum_helpers::feval;
    using semantic::RSCA;
#if GLEAM_MAX_VERSION >= 0x300 || GLEAM_MAX_VERSION_ES >= 0x310
    if(features.mapping)
    {
        if(feval(flags, RSCA::ReadWrite))
            return group::buffer_access_arb::read_write;
        if(feval(flags, RSCA::WriteOnly))
            return group::buffer_access_arb::write_only;
        return group::buffer_access_arb::read_only;
    }
#endif
#if defined(GL_OES_mapbuffer)
    return group::buffer_access_arb::write_only_oes;
#endif
    Throw(undefined_behavior("no flag mapped"));
}
#endif

#if GLEAM_MAX_VERSION >= 0x300 || GLEAM_MAX_VERSION_ES >= 0x300
template<typename T>
requires(std::is_same_v<T, group::map_buffer_access_mask>)
inline group::map_buffer_access_mask to(
    [[maybe_unused]] features::buffers const& features,
    semantic::RSCA                            flags,
    bool                                      whole_buffer)
{
    using enum_helpers::feval;
    using semantic::RSCA;
    using mask = group::map_buffer_access_mask;
    mask out   = static_cast<mask>(0);
    if(feval(flags, RSCA::ReadOnly))
        out |= mask::map_read_bit;
    if(feval(flags, RSCA::WriteOnly))
        out |= mask::map_write_bit;
#if GLEAM_MAX_VERSION >= 0x440
    if(feval(flags, RSCA::Persistent) && features.persistence &&
       features.storage)
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
#if GLEAM_MAX_VERSION >= 0x440
    case TypeEnum::Packed_UFloat:
        return T::unsigned_int_10f_11f_11f_rev;
#endif
#if GLEAM_MAX_VERSION >= 0x150
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
inline T vertex_type_to(semantic::TypeEnum /*type*/)
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

inline GLenum to(textures::swizzle_t channel)
{
    switch(channel)
    {
    case textures::swizzle_t::red:
        return GL_RED;
    case textures::swizzle_t::green:
        return GL_GREEN;
    case textures::swizzle_t::blue:
        return GL_BLUE;
    case textures::swizzle_t::alpha:
        return GL_ALPHA;
    case textures::swizzle_t::one:
        return GL_ONE;
    case textures::swizzle_t::zero:
        return GL_ZERO;
    }
    __builtin_unreachable();
}

} // namespace gleam::convert
