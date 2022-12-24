#pragma once

#include "rhi_features.h"
#include "rhi_translate.h"
#include "rhi_versioning.h"

#include <glw/extensions/OES_mapbuffer.h>

namespace gleam {

struct buffer_slice_t;

struct buffer_t : std::enable_shared_from_this<buffer_t>
{
    buffer_t(
        features::buffers features, buffers::type type, semantic::RSCA access) :
        m_features(features),
        m_type(type), m_access(access)
    {
    }

    inline bool immutable()
    {
        return (m_access & semantic::RSCA::Immutable) != semantic::RSCA::None;
    }

    inline void alloc()
    {
#if GLEAM_MAX_VERSION >= 0x450
        if(m_features.dsa)
            cmd::create_buffers(SpanOne<u32>(m_handle));
        else
#endif
            cmd::gen_buffers(SpanOne<u32>(m_handle));
    }

    inline void dealloc()
    {
        cmd::delete_buffers(SpanOne<u32>(m_handle));
        m_handle.release();
    }

    inline void commit(size_t size)
    {
#if GLEAM_MAX_VERSION >= 0x450
        if(m_features.dsa && immutable())
        {
            cmd::named_buffer_storage(
                m_handle,
                null_span<>{size},
                convert::to<group::buffer_storage_mask>(m_access));
        } else if(m_features.dsa)
        {
            cmd::named_buffer_data(
                m_handle,
                null_span<>{size},
                convert::to<group::vertex_buffer_object_usage>(m_access));
        } else
#endif
#if GLEAM_MAX_VERSION >= 0x430
            if(m_features.storage && immutable())
        {
            cmd::bind_buffer(convert::to(m_type), m_handle);
            cmd::buffer_storage(
                convert::to<group::buffer_storage_target>(m_type),
                null_span<>{size},
                convert::to<group::buffer_storage_mask>(m_access));
        } else
#endif
        {
            cmd::bind_buffer(convert::to(m_type), m_handle);
            cmd::buffer_data(
                convert::to(m_type),
                null_span<>{size},
                convert::to<group::buffer_usage_arb>(m_access));
        }
    }

    template<typename Span>
    requires semantic::concepts::Span<Span>
    inline void commit(Span const& data)
    {
#if GLEAM_MAX_VERSION >= 0x450
        if(m_features.dsa && immutable())
        {
            cmd::named_buffer_storage(
                m_handle,
                data,
                convert::to<group::buffer_storage_mask>(m_access));
        } else if(m_features.dsa)
        {
            cmd::named_buffer_data(
                m_handle,
                data,
                convert::to<group::vertex_buffer_object_usage>(m_access));
        } else
#endif
#if GLEAM_MAX_VERSION >= 0x430
            if(m_features.storage && immutable())
        {
            cmd::bind_buffer(convert::to(m_type), m_handle);
            cmd::buffer_storage(
                convert::to<group::buffer_storage_target>(m_type),
                data,
                convert::to<group::buffer_storage_mask>(m_access));
        } else
#endif
        {
            cmd::bind_buffer(convert::to(m_type), m_handle);
            cmd::buffer_data(convert::to(m_type), data, convert::to(m_access));
        }
    }

    inline Span<std::byte> map(
        size_t offset, std::optional<size_t> size = std::nullopt)
    {
        auto   buffer_size = this->size();
        size_t actual_size = size.value_or(buffer_size - offset);

        if(offset + actual_size > buffer_size)
            Throw(std::out_of_range("offset out of range of buffer"));

        void* pointer = nullptr;
#if GLEAM_MAX_VERSION >= 0x450
        if(m_features.dsa)
        {
            pointer = cmd::map_named_buffer_range(
                m_handle,
                offset,
                actual_size,
                convert::to<group::map_buffer_access_mask>(
                    m_features, m_access, actual_size == buffer_size));
        } else
#endif
#if GLEAM_MAX_VERSION >= 0x150 || GLEAM_MAX_VERSION_ES >= 0x300
            if(m_features.mapping)
        {
            auto target = convert::to(m_type);
            cmd::bind_buffer(target, m_handle);
            pointer = cmd::map_buffer_range(
                target,
                offset,
                actual_size,
                convert::to<group::map_buffer_access_mask>(
                    m_features, m_access, actual_size == buffer_size));
        }
#endif
#if GLEAM_MAX_VERSION_ES == 0x200 && defined(GL_OES_mapbuffer)
        if(m_features.oes.mapbuffer)
        {
            auto target = convert::to(m_type);
            cmd::bind_buffer(target, m_handle);
            pointer = gl::oes::mapbuffer::map_buffer(
                target, convert::to<group::buffer_access_arb>(m_access));
        } else
            return Span<std::byte>();
#endif

        return Span<std::byte>(
            reinterpret_cast<std::byte*>(pointer), actual_size);
    }

    inline void unmap(void* = nullptr)
    {
#if GLEAM_MAX_VERSION >= 0x450
        if(m_features.dsa)
            cmd::unmap_named_buffer(m_handle);
        else
#endif
#if GLEAM_MAX_VERSION >= 0x150 || GLEAM_MAX_VERSION_ES >= 0x300
        {
            cmd::bind_buffer(convert::to(m_type), m_handle);
            cmd::unmap_buffer(convert::to(m_type));
        }
#endif
#if GLEAM_MAX_VERSION_ES == 0x200 && defined(GL_OES_mapbuffer)
        if(m_features.oes.mapbuffer)
        {
            auto target = convert::to(m_type);
            cmd::bind_buffer(target, m_handle);
            gl::oes::mapbuffer::unmap_buffer(target);
        }
#endif
    }

    inline void setState(buffers::property, u32)
    {
    }

    inline size_t size() const
    {
        i32 out{0};
#if GLEAM_MAX_VERSION >= 0x450
        if(m_features.dsa)
        {
            cmd::get_named_buffer_parameter(
                m_handle, group::buffer_prop_arb::buffer_size, SpanOne(out));
        } else
#endif
        {
            cmd::bind_buffer(convert::to(m_type), m_handle);
            cmd::get_buffer_parameter(
                convert::to(m_type),
                group::buffer_prop_arb::buffer_size,
                SpanOne(out));
        }

        return static_cast<size_t>(out);
    }

    inline hnd& handle()
    {
        return m_handle;
    }

    inline buffer_slice_t slice(
        size_t offset, std::optional<size_t> size = std::nullopt);

    hnd               m_handle;
    features::buffers m_features;
    buffers::type     m_type;
    semantic::RSCA    m_access;
};

struct buffer_slice_t
{
    inline bool valid()
    {
        return !m_parent.expired() && m_size > 0;
    }

    inline hnd& handle()
    {
        auto parent = m_parent.lock();
        return parent->handle();
    }
    inline auto buffer()
    {
        auto parent = m_parent.lock();
        return parent->map(m_offset, m_size);
    }
    template<typename T>
    inline Span<T> buffer_cast()
    {
        auto parent = m_parent.lock();
        auto slice  = parent->map(m_offset, m_size);
        return mem_chunk<T>::ofBytes(slice.data(), slice.size()).view;
    }
    inline buffer_slice_t slice(
        size_t offset, std::optional<size_t> size = std::nullopt)
    {
        return {
            .m_parent = m_parent,
            .m_type   = m_type,
            .m_offset = m_offset + offset,
            .m_size   = size.value_or(m_size),
        };
    }

    stl_types::WkPtr<gleam::buffer_t> m_parent;
    buffers::type                     m_type{buffers::type::vertex};
    size_t                            m_offset{0};
    size_t                            m_size{0};
};

inline buffer_slice_t buffer_t::slice(size_t offset, std::optional<size_t> size)
{
    return buffer_slice_t{
        .m_parent = shared_from_this(),
        .m_type   = m_type,
        .m_offset = offset,
        .m_size   = size.value_or(this->size()),
    };
}

} // namespace gleam
