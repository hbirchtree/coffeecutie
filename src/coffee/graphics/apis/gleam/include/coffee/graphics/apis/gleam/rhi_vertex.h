#pragma once

#include <bitset>
#include <peripherals/semantic/enum/data_types.h>

#include "rhi_buffer.h"
#include "rhi_debug.h"
#include "rhi_features.h"
#include "rhi_translate.h"
#include "rhi_versioning.h"

#if defined(GL_OES_vertex_array_object)
#include <glw/extensions/OES_vertex_array_object.h>
#endif

namespace gleam {

namespace detail {

using vattribute_flags = semantic::concepts::graphics::buffers::attribute_flags;

template<typename T>
constexpr inline std::pair<semantic::TypeEnum, vattribute_flags> type_of()
{
    using libc_types::i16;
    using libc_types::i8;
    using libc_types::u16;
    using libc_types::u8;

    using E = semantic::TypeEnum;
    if constexpr(std::is_same_v<T, f64>)
        return {E::BigScalar, vattribute_flags::none};
    if constexpr(std::is_same_v<T, f32>)
        return {E::Scalar, vattribute_flags::none};
    if constexpr(std::is_same_v<T, i64>)
        return {E::LL, vattribute_flags::none};
    if constexpr(std::is_same_v<T, i32>)
        return {E::Int, vattribute_flags::none};
    if constexpr(std::is_same_v<T, i16>)
        return {E::Short, vattribute_flags::none};
    if constexpr(std::is_same_v<T, i8>)
        return {E::Byte, vattribute_flags::none};
    if constexpr(std::is_same_v<T, u64>)
        return {E::ULL, vattribute_flags::none};
    if constexpr(std::is_same_v<T, u32>)
        return {E::UInt, vattribute_flags::none};
    if constexpr(std::is_same_v<T, u16>)
        return {E::UShort, vattribute_flags::none};
    if constexpr(std::is_same_v<T, u8>)
        return {E::UByte, vattribute_flags::none};
    if constexpr(std::is_same_v<T, typing::pixels::f11>)
        return {E::Packed_UFloat, vattribute_flags::packed};
}

template<typename T>
requires semantic::concepts::Vector<T, typename T::value_type, 4>
inline std::tuple<semantic::TypeEnum, vattribute_flags, u32> vector_info_of()
{
    auto [type, flags] = type_of<typename T::value_type>();
    return {type, flags, 4};
}

template<typename T>
requires semantic::concepts::Vector<T, typename T::value_type, 3>
inline std::tuple<semantic::TypeEnum, vattribute_flags, u32> vector_info_of()
{
    auto [type, flags] = type_of<typename T::value_type>();
    return {type, flags, 3};
}

template<typename T>
requires semantic::concepts::Vector<T, typename T::value_type, 2>
inline std::tuple<semantic::TypeEnum, vattribute_flags, u32> vector_info_of()
{
    auto [type, flags] = type_of<typename T::value_type>();
    return {type, flags, 2};
}

template<typename T>
requires(std::is_floating_point_v<T>&& std::is_integral_v<T>)
    //
    inline std::
        tuple<semantic::TypeEnum, vattribute_flags, u32> vector_info_of()
{
    auto [type, flags] = type_of<T>();
    return {type, flags, 1};
}

static_assert(std::is_floating_point_v<i32> || std::is_integral_v<i32>);
static_assert(std::is_floating_point_v<u32> || std::is_integral_v<u32>);
static_assert(std::is_floating_point_v<f32> || std::is_integral_v<f32>);
static_assert(
    !std::is_floating_point_v<
        typing::pixels::f11> && !std::is_integral_v<typing::pixels::f11>);

template<typename T>
requires std::is_same_v<T, typing::pixels::f11>
inline std::tuple<semantic::TypeEnum, vattribute_flags, u32> vector_info_of()
{
    return {semantic::TypeEnum::Packed_UFloat, vattribute_flags::packed, 3};
}

constexpr inline bool vertex_is_int_type(semantic::TypeEnum type)
{
    using T = semantic::TypeEnum;
    switch(type)
    {
    case T::UByte:
    case T::Byte:
    case T::UShort:
    case T::Short:
    case T::UInt:
    case T::Int:
    case T::ULL:
    case T::LL:
        return true;
    default:
        return false;
    }
}

struct vertex_attribute_float_type
{
};
struct vertex_attribute_int_type
{
};

} // namespace detail

static constexpr detail::vertex_attribute_float_type vertex_float_type;
static constexpr detail::vertex_attribute_int_type   vertex_int_type;

struct vertex_attribute
{
    using attribute_flags
        = semantic::concepts::graphics::buffers::attribute_flags;

    u32 index{0};
    struct
    {
        u64                offset{0};
        u64                stride{0};
        u32                count{4};
        semantic::TypeEnum type{semantic::TypeEnum::Scalar};
        attribute_flags    flags{attribute_flags::none};
    } value;
    struct
    {
        size_t offset{0};
        u32    id{0};
    } buffer;

    template<typename T, typename V>
    static inline constexpr auto from_member(V T::*member)
    {
        auto [type, flags, count] = detail::vector_info_of<V>();
        auto member_info          = stl_types::member_traits(member);
        return vertex_attribute{
            .value = {
                .offset = member_info.offset(),
                .stride = sizeof(T),
                .count  = count,
                .type   = type,
                .flags  = flags,
            }};
    }

    template<typename T, typename V>
    requires(!std::is_floating_point_v<V>) static inline auto from_member(
        V T::*member, detail::vertex_attribute_float_type)
    {
        auto attr = from_member<T, V>(member);
        attr.value.flags
            = attribute_flags::packed | attribute_flags::normalized;
        return attr;
    }

    template<typename T, typename V>
    requires std::is_floating_point_v<V>
    static inline auto from_member(
        V T::*member, detail::vertex_attribute_float_type)
    {
        return from_member<T, V>(member);
    }
};

namespace detail {

inline void vertex_setup_attribute(vertex_attribute const& attr, u32 offset = 0)
{
    cmd::vertex_attrib_pointer(
        attr.index,
        attr.value.count,
        convert::to<group::vertex_attrib_pointer_type>(attr.value.type),
        attr.value.flags & vertex_attribute::attribute_flags::normalized,
        attr.value.stride,
        offset_span::of(attr.buffer.offset + attr.value.offset + offset));
}

} // namespace detail

#if GLEAM_MAX_VERSION >= 0x150 || GLEAM_MAX_VERSION_ES >= 0x300 \
    || defined(GL_OES_vertex_array_object)
struct vertex_array_t
{
    using attribute_type = vertex_attribute;

    vertex_array_t(features::vertices& features, debug::api& debug) :
        m_features(features), m_debug(debug)
    {
    }

    void alloc()
    {
#if GLEAM_MAX_VERSION >= 0x450
        if(m_features.dsa)
        {
            cmd::create_vertex_arrays(SpanOne<u32>(m_handle));
        } else
#endif
#if GLEAM_MAX_VERSION_ES != 0x200
            if(m_features.vertex_arrays)
            cmd::gen_vertex_arrays(SpanOne<u32>(m_handle));
#elif defined(GL_OES_vertex_array_object)
        if(m_features.oes.vertex_arrays)
            gl::oes::vertex_array_object::gen_vertex_arrays(
                SpanOne<u32>(m_handle));
#endif
    }
    void dealloc()
    {
#if GLEAM_MAX_VERSION_ES != 0x200
        if(m_features.vertex_arrays)
            cmd::delete_vertex_arrays(SpanOne<u32>(m_handle));
#elif defined(GL_OES_vertex_array_object)
        if(m_features.oes.vertex_arrays)
            gl::oes::vertex_array_object::delete_vertex_arrays(
                SpanOne<u32>(m_handle));
#endif
        m_handle.release();
    }

    void set_attribute_names(
        std::vector<std::pair<std::string_view, u32>>&& attributes)
    {
        if(m_features.attribute_binding)
            return;
        m_attribute_names = std::move(attributes);
    }

    void add(attribute_type attribute)
    {
        auto _ = m_debug.scope(__PRETTY_FUNCTION__);
        if(!m_features.dsa)
        {
#if GLEAM_MAX_VERSION_ES != 0x200
            if(m_features.vertex_arrays)
                cmd::bind_vertex_array(m_handle);
#elif defined(GL_OES_vertex_array_object)
            if(m_features.oes.vertex_arrays)
                gl::oes::vertex_array_object::bind_vertex_array(m_handle);
#endif
        }

        bool packed
            = attribute.value.flags & vertex_attribute::attribute_flags::packed;
        bool normalized = attribute.value.flags
                          & vertex_attribute::attribute_flags::normalized;

#if GLEAM_MAX_VERSION >= 0x450
        if(m_features.dsa && m_features.format)
        {
            cmd::enable_vertex_array_attrib(m_handle, attribute.index);
            if(!packed && detail::vertex_is_int_type(attribute.value.type))
                cmd::vertex_array_attrib_i_format(
                    m_handle,
                    attribute.index,
                    attribute.value.count,
                    convert::to<group::vertex_attrib_int>(attribute.value.type),
                    attribute.value.stride);
            else
                cmd::vertex_array_attrib_format(
                    m_handle,
                    attribute.index,
                    attribute.value.count,
                    convert::to<group::vertex_attrib_type>(
                        attribute.value.type),
                    normalized,
                    attribute.value.offset);
        } else
#endif
#if GLEAM_MAX_VERSION >= 0x430 || GLEAM_MAX_VERSION_ES >= 0x310
            if(m_features.format)
        {
            cmd::enable_vertex_attrib_array(attribute.index);
            if(!packed && detail::vertex_is_int_type(attribute.value.type))
                cmd::vertex_attrib_i_format(
                    attribute.index,
                    attribute.value.count,
                    convert::to<group::vertex_attrib_int>(attribute.value.type),
                    attribute.value.offset);
            else
                cmd::vertex_attrib_format(
                    attribute.index,
                    attribute.value.count,
                    convert::to<group::vertex_attrib_type>(
                        attribute.value.type),
                    normalized,
                    attribute.value.offset);
        } else
#endif
            if(m_features.vertex_arrays)
        {
            cmd::enable_vertex_attrib_array(attribute.index);
#if GLEAM_MAX_VERSION_ES != 0x200
            cmd::vertex_attrib_divisor(
                attribute.index,
                static_cast<bool>(
                    attribute.value.flags
                    & vertex_attribute::attribute_flags::instanced));
#endif
        }

        m_attributes.emplace_back(std::move(attribute));

        if(!m_features.dsa)
        {
#if GLEAM_MAX_VERSION_ES != 0x200
            if(m_features.vertex_arrays)
                cmd::bind_vertex_array(0);
#elif defined(GL_OES_vertex_array_object)
            if(m_features.oes.vertex_arrays)
                gl::oes::vertex_array_object::bind_vertex_array(0);
#endif
        }
    }

    template<class T>
    requires(T::value == buffers::type::vertex)
        //
        void set_buffer(T, stl_types::ShPtr<buffer_t> buffer, u32 binding)
    {
        auto _ = m_debug.scope(__PRETTY_FUNCTION__);
        if(!m_features.dsa)
        {
#if GLEAM_MAX_VERSION_ES != 0x200
            if(m_features.vertex_arrays)
                cmd::bind_vertex_array(m_handle);
#elif defined(GL_OES_vertex_array_object)
            if(m_features.oes.vertex_arrays)
                gl::oes::vertex_array_object::bind_vertex_array(m_handle);
#endif
        }

        for(auto const& attribute : m_attributes)
        {
            if(attribute.buffer.id != binding)
                continue;

            bool packed = attribute.value.flags
                          & vertex_attribute::attribute_flags::packed;
            bool instanced = attribute.value.flags
                             & vertex_attribute::attribute_flags::instanced;
            packed = packed && detail::vertex_is_int_type(attribute.value.type);

#if GLEAM_MAX_VERSION >= 0x450
            if(m_features.dsa && m_features.format)
            {
                cmd::vertex_array_vertex_buffer(
                    m_handle,
                    attribute.index,
                    buffer->m_handle,
                    attribute.buffer.offset,
                    attribute.value.stride);
                cmd::vertex_array_binding_divisor(
                    m_handle, attribute.index, instanced ? 1 : 0);
                cmd::vertex_array_attrib_binding(
                    m_handle, attribute.index, attribute.index);
            } else
#endif
#if GLEAM_MAX_VERSION >= 0x430 || GLEAM_MAX_VERSION_ES >= 0x310
                if(m_features.format)
            {
                cmd::bind_vertex_buffer(
                    attribute.index,
                    buffer->m_handle,
                    attribute.buffer.offset,
                    attribute.value.stride);
                cmd::vertex_binding_divisor(attribute.index, instanced ? 1 : 0);
                cmd::vertex_attrib_binding(attribute.index, attribute.index);
            } else
#endif
                if(m_features.vertex_arrays)
            {
                cmd::bind_buffer(
                    group::buffer_target_arb::array_buffer, buffer->m_handle);
#if GLEAM_MAX_VERSION_ES != 0x200
                if(!packed && detail::vertex_is_int_type(attribute.value.type))
                    cmd::vertex_attrib_i_pointer(
                        attribute.index,
                        attribute.value.count,
                        convert::to<group::vertex_attrib_int>(
                            attribute.value.type),
                        attribute.value.stride,
                        offset_span::of(
                            attribute.buffer.offset + attribute.value.offset));
                else
#endif
                    detail::vertex_setup_attribute(attribute);
            } else
                m_buffers.insert({binding, buffer});
        }
        if(!m_features.dsa)
        {
#if GLEAM_MAX_VERSION_ES != 0x200
            if(m_features.vertex_arrays)
                cmd::bind_vertex_array(0);
#elif defined(GL_OES_vertex_array_object)
            if(m_features.oes.vertex_arrays)
                gl::oes::vertex_array_object::bind_vertex_array(0);
#endif
        }
        if(!m_features.format)
            cmd::bind_buffer(group::buffer_target_arb::array_buffer, 0);
    }

    template<class T>
    requires(T::value == buffers::type::element)
        //
        void set_buffer(T, stl_types::ShPtr<buffer_t> buffer)
    {
        if(!m_features.dsa)
        {
#if GLEAM_MAX_VERSION_ES != 0x200
            if(m_features.vertex_arrays)
                cmd::bind_vertex_array(m_handle);
#elif defined(GL_OES_vertex_array_object)
            if(m_features.oes.vertex_arrays)
                gl::oes::vertex_array_object::bind_vertex_array(m_handle);
#endif
        }

#if GLEAM_MAX_VERSION >= 0x450
        if(m_features.dsa && m_features.format)
        {
            cmd::vertex_array_element_buffer(m_handle, buffer->m_handle);
        } else
#endif
            if(m_features.vertex_arrays)
        {
            cmd::bind_buffer(
                group::buffer_target_arb::element_array_buffer,
                buffer->m_handle);
        } else
            m_element_buffer = buffer;

        if(!m_features.dsa)
        {
#if GLEAM_MAX_VERSION_ES != 0x200
            if(m_features.vertex_arrays)
                cmd::bind_vertex_array(0);
#elif defined(GL_OES_vertex_array_object)
            if(m_features.oes.vertex_arrays)
                gl::oes::vertex_array_object::bind_vertex_array(0);
#endif
        }
    }

    features::vertices                            m_features;
    debug::api                                    m_debug;
    std::vector<attribute_type>                   m_attributes;
    std::vector<std::pair<std::string_view, u32>> m_attribute_names;
    std::map<u32, std::weak_ptr<buffer_t>>        m_buffers;
    std::weak_ptr<buffer_t>                       m_element_buffer;
    hnd                                           m_handle;
};
#endif

struct vertex_array_legacy_t
{
    using attribute_type = vertex_attribute;

    vertex_array_legacy_t(features::vertices& features, debug::api& debug) :
        m_features(features)
    {
    }

    void alloc()
    {
    }
    void dealloc()
    {
    }

    void add(attribute_type attribute)
    {
        m_attributes.push_back(attribute);
    }

    template<class T>
    requires(T::value == buffers::type::vertex)
        //
        void set_buffer(T, stl_types::ShPtr<buffer_t> buffer, u32 binding)
    {
        m_buffers.insert({binding, buffer});
    }

    template<class T>
    requires(T::value == buffers::type::element)
        //
        void set_buffer(T, stl_types::ShPtr<buffer_t> buffer)
    {
        m_element_buffer = buffer;
    }

    features::vertices                            m_features;
    std::vector<attribute_type>                   m_attributes;
    std::vector<std::pair<std::string_view, u32>> m_attribute_names;
    std::map<u32, std::weak_ptr<buffer_t>>        m_buffers;
    std::weak_ptr<buffer_t>                       m_element_buffer;
};

} // namespace gleam
