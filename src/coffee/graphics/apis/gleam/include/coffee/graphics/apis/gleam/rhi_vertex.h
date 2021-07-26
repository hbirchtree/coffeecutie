#pragma once

#include <bitset>
#include <peripherals/semantic/enum/data_types.h>

#include "rhi_buffer.h"
#include "rhi_debug.h"
#include "rhi_features.h"
#include "rhi_translate.h"
#include "rhi_versioning.h"

namespace gleam {

struct vertex_attribute
{
    enum attribute_flags
    {
        none       = 0x0,
        packed     = 0x1,
        normalized = 0x2,
        instanced  = 0x4,
    };

    u32 index{0};
    struct
    {
        u64                offset{0};
        u64                stride{0};
        u32                count{4};
        semantic::TypeEnum type{semantic::TypeEnum::Scalar};
        attribute_flags    flags{none};
    } value;
    struct
    {
        size_t offset{0};
        u32    id{0};
    } buffer;
};

C_FLAGS(vertex_attribute::attribute_flags, u32);

namespace detail {

inline void vertex_setup_attribute(vertex_attribute const& attr, u32 offset = 0)
{
    cmd::vertex_attrib_pointer(
        attr.index,
        attr.value.count,
        convert::to<group::vertex_attrib_pointer_type>(attr.value.type),
        attr.value.flags & vertex_attribute::normalized,
        attr.value.stride,
        offset_span::of(attr.buffer.offset + attr.value.offset + offset));
}

inline bool vertex_is_int_type(semantic::TypeEnum type)
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

} // namespace detail

#if GLEAM_MAX_VERSION >= 0x150 || GLEAM_MAX_VERSION_ES >= 0x300
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
            cmd::gen_vertex_arrays(SpanOne<u32>(m_handle));
    }
    void dealloc()
    {
        cmd::delete_vertex_arrays(SpanOne<u32>(m_handle));
        m_handle.release();
    }

    void add(attribute_type attribute)
    {
        auto _ = m_debug.scope(__PRETTY_FUNCTION__);
        if(!m_features.dsa)
            cmd::bind_vertex_array(m_handle);

        bool packed     = attribute.value.flags & attribute_type::packed;
        bool normalized = attribute.value.flags & attribute_type::normalized;

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
        {
            cmd::enable_vertex_attrib_array(attribute.index);
            cmd::vertex_attrib_divisor(
                attribute.index,
                static_cast<bool>(
                    attribute.value.flags & attribute_type::instanced));
        }

        m_attributes.emplace_back(std::move(attribute));

        if(!m_features.dsa)
            cmd::bind_vertex_array(0);
    }

    template<class T>
    requires(T::value == buffers::type::vertex)
        //
        void set_buffer(T, stl_types::ShPtr<buffer_t> buffer, u32 binding)
    {
        auto _ = m_debug.scope(__PRETTY_FUNCTION__);
        if(!m_features.dsa)
            cmd::bind_vertex_array(m_handle);

        for(auto const& attribute : m_attributes)
        {
            if(attribute.buffer.id != binding)
                continue;

            bool packed    = attribute.value.flags & attribute_type::packed;
            bool instanced = attribute.value.flags & attribute_type::instanced;
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
            {
                cmd::bind_buffer(
                    group::buffer_target_arb::array_buffer, buffer->m_handle);
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
                    detail::vertex_setup_attribute(attribute);
            }
        }
        if(!m_features.dsa)
            cmd::bind_vertex_array(0);
        if(!m_features.format)
            cmd::bind_buffer(group::buffer_target_arb::array_buffer, 0);
    }

    template<class T>
    requires(T::value == buffers::type::element)
        //
        void set_buffer(T, stl_types::ShPtr<buffer_t> buffer)
    {
        if(!m_features.dsa)
            cmd::bind_vertex_array(m_handle);

#if GLEAM_MAX_VERSION >= 0x450
        if(m_features.dsa && m_features.format)
        {
            cmd::vertex_array_element_buffer(m_handle, buffer->m_handle);
        } else
#endif
        {
            cmd::bind_buffer(
                group::buffer_target_arb::element_array_buffer,
                buffer->m_handle);
        }

        if(!m_features.dsa)
            cmd::bind_vertex_array(0);
    }

    features::vertices                m_features;
    debug::api                        m_debug;
    stl_types::Vector<attribute_type> m_attributes;
    hnd                               m_handle;
};
#endif

struct vertex_array_legacy_t
{
    using attribute_type = vertex_attribute;

    vertex_array_legacy_t(features::vertices& features) : m_features(features)
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

    void apply(u32 offset = 0)
    {
        for(auto const& attribute : m_attributes)
        {
            auto const& buffer = m_buffers.at(attribute.buffer.id).lock();
            cmd::bind_buffer(
                group::buffer_target_arb::array_buffer, buffer->m_handle);
            cmd::enable_vertex_attrib_array(attribute.index);
            detail::vertex_setup_attribute(attribute, offset);
        }
    }
    void remove()
    {
        for(auto const& attribute : m_attributes)
            cmd::disable_vertex_attrib_array(attribute.index);
    }

    features::vertices                              m_features;
    stl_types::Vector<attribute_type>               m_attributes;
    stl_types::Map<u32, stl_types::WkPtr<buffer_t>> m_buffers;
};

} // namespace gleam
