#pragma once

#include "rhi_debug.h"
#include "rhi_features.h"
#include "rhi_translate.h"
#include "rhi_versioning.h"

#include <glw/extensions/ARB_invalidate_subdata.h>
#include <glw/extensions/OES_mapbuffer.h>

namespace gleam {

struct buffer_slice_t;

struct buffer_t : std::enable_shared_from_this<buffer_t>
{
    static constexpr auto debug_identifier = group::object_identifier::buffer;

    buffer_t(
        features::buffers features,
        workarounds       workarounds,
        buffers::type     type,
        semantic::RSCA    access) :
        m_features(features),
        m_workarounds(workarounds), m_type(type), m_access(access)
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
        if(m_workarounds.buffer.emulated_mapbuffer)
            m_allocation.resize(size);
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
                convert::to<group::vertex_buffer_object_usage>(
                    m_features, m_access));
        } else
#endif
#if GLEAM_MAX_VERSION >= 0x440
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
                convert::to<group::buffer_usage_arb>(m_features, m_access));
        }
        if(m_workarounds.buffer.emulated_mapbuffer)
        {
            /* Populate the client-side storage with the same data, for sync */
            m_allocation.resize(size);
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
                convert::to<group::vertex_buffer_object_usage>(
                    m_features, m_access));
        } else
#endif
#if GLEAM_MAX_VERSION >= 0x440
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
            cmd::buffer_data(
                convert::to(m_type), data, convert::to(m_features, m_access));
        }
        if(m_workarounds.buffer.emulated_mapbuffer)
        {
            /* Populate the client-side storage with the same data, for sync */
            auto bytes
                = semantic::mem_chunk<const char>::ofContainer(data).view;
            m_allocation.resize(bytes.size());
            std::copy(bytes.begin(), bytes.end(), m_allocation.begin());
        }
    }

    template<typename Span>
    requires semantic::concepts::Span<Span>
    inline void update(size_t offset, Span const& data)
    {
#if GLEAM_MAX_VERSION >= 0x450
        if(m_features.dsa)
        {
            cmd::named_buffer_sub_data(m_handle, offset, data);
        } else
#endif
        {
            cmd::bind_buffer(convert::to(m_type), m_handle);
            cmd::buffer_sub_data(convert::to(m_type), offset, data);
        }
    }

    template<typename T = std::byte>
    inline Span<T> map(size_t offset, std::optional<size_t> size = std::nullopt)
    {
        auto   buffer_size = this->size();
        size_t actual_size = size.value_or(buffer_size - offset);

        if(offset + actual_size > buffer_size)
            Throw(std::out_of_range("offset out of range of buffer"));

        void* pointer = nullptr;
        do
        {
#if GLEAM_MAX_VERSION >= 0x450
            if(m_features.dsa)
            {
                pointer = cmd::map_named_buffer_range(
                    m_handle,
                    offset,
                    actual_size,
                    convert::to<group::map_buffer_access_mask>(
                        m_features, m_access, actual_size == buffer_size));
                break;
            }
#endif
#if GLEAM_MAX_VERSION >= 0x300 || GLEAM_MAX_VERSION_ES >= 0x300
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
                break;
            }
#endif
#if defined(GL_OES_mapbuffer)
            if(m_features.oes.mapbuffer)
            {
                auto target = convert::to(m_type);
                cmd::bind_buffer(target, m_handle);
                pointer = gl::oes::mapbuffer::map_buffer(
                    target,
                    convert::to<group::buffer_access_arb>(
                        m_features, m_access));
                pointer = reinterpret_cast<char*>(pointer) + offset;
                break;
            }
#endif
            if(m_workarounds.buffer.emulated_mapbuffer)
            {
                if(m_allocation.size() != buffer_size)
                    Throw(undefined_behavior(
                        "client-storage desynced from buffer"));
                m_mapping = {offset, actual_size};
                pointer   = m_allocation.data() + offset;
                break;
            }
            return Span<T>();
        } while(false);

        return mem_chunk<T>::ofBytes(pointer, actual_size).view;
    }

    template<typename T>
    inline auto map_all()
    {
        if(m_pointer_all.empty())
            m_pointer_all = map<std::byte>(0);
        return mem_chunk<T>::ofContainer(m_pointer_all);
    }

    inline void unmap(void* = nullptr)
    {
        m_pointer_all = {};
#if GLEAM_MAX_VERSION >= 0x450
        if(m_features.dsa && m_features.mapping)
        {
            cmd::unmap_named_buffer(m_handle);
            return;
        }
#endif
#if GLEAM_MAX_VERSION >= 0x150 || GLEAM_MAX_VERSION_ES >= 0x300
        if(m_features.mapping)
        {
            cmd::bind_buffer(convert::to(m_type), m_handle);
            cmd::unmap_buffer(convert::to(m_type));
            return;
        }
#endif
#if defined(GL_OES_mapbuffer)
        if(m_features.oes.mapbuffer)
        {
            auto target = convert::to(m_type);
            cmd::bind_buffer(target, m_handle);
            gl::oes::mapbuffer::unmap_buffer(target);
            return;
        }
#endif
        if(m_workarounds.buffer.emulated_mapbuffer)
        {
            cmd::bind_buffer(convert::to(m_type), m_handle);
            cmd::buffer_sub_data(
                convert::to(m_type),
                m_mapping.first,
                semantic::Span<char>(
                    m_allocation.data() + m_mapping.first, m_mapping.second));
            m_mapping = {};
            return;
        }
    }

    inline void discard()
    {
#if GLEAM_MAX_VERSION >= 0x430
        if(m_features.invalidate)
        {
            cmd::invalidate_buffer_data(m_handle);
            return;
        }
#endif
#if defined(GL_ARB_invalidate_subdata)
        if(m_features.arb.invalidate)
        {
            gl::arb::invalidate_subdata::invalidate_buffer_data(m_handle);
            return;
        }
#endif
    }

    inline void setState(buffers::property, u32)
    {
    }

    inline size_t size() const
    {
        /* TODO: Cache the size, Emscripten is slow on this */

        if(m_workarounds.buffer.emulated_mapbuffer && m_allocation.size())
            return m_allocation.size();
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

    std::vector<char>         m_allocation;
    std::pair<size_t, size_t> m_mapping{0, 0};
    hnd                       m_handle;
    features::buffers         m_features;
    workarounds               m_workarounds;
    buffers::type             m_type;
    semantic::RSCA            m_access;
    semantic::Span<std::byte> m_pointer_all;
};

struct buffer_slice_t
{
    inline bool valid() const
    {
        return !m_parent.expired() && m_size > 0;
    }

    inline hnd& handle() const
    {
        auto parent = m_parent.lock();
        return parent->handle();
    }
    inline auto buffer()
    {
        auto parent = m_parent.lock();
        return parent->map_all<std::byte>().view.subspan(m_offset, m_size);
    }
    template<typename T>
    inline Span<T> buffer_cast()
    {
        auto slice = buffer();
        return mem_chunk<T>::ofContainer(slice).view;
    }
    inline auto unmap()
    {
        auto parent = m_parent.lock();
        return parent->unmap();
    }
    inline buffer_slice_t slice(
        size_t offset, std::optional<size_t> size = std::nullopt) const
    {
        return {
            .m_parent = m_parent,
            .m_type   = m_type,
            .m_offset = m_offset + offset,
            .m_size   = size.value_or(m_size),
        };
    }

    std::weak_ptr<gleam::buffer_t> m_parent;
    buffers::type                  m_type{buffers::type::vertex};
    size_t                         m_offset{0};
    size_t                         m_size{0};
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

struct circular_buffer_t
{
    enum class sync_status
    {
        fail,
        ok,
    };

    circular_buffer_t(
        debug::api&                 debug,
        std::shared_ptr<buffer_t>&& buffer,
        u64                         timeout = 5000) :
        m_debug(debug),
        m_buffer(std::move(buffer)), m_buffer_size(m_buffer->size()),
        m_timeout(timeout)
    {
    }
    circular_buffer_t(circular_buffer_t&&) = default;

    struct fence_options
    {
        fence_options(size_t start, size_t end) : start(start), ptr(end)
        {
        }
        fence_options(fence_options&& other) :
            start(other.start), ptr(other.ptr), used(other.used)
        {
            other.used = true;
        }
        ~fence_options()
        {
            if(!used)
                Throw(undefined_behavior("fence was not set"));
        }

        size_t start{0};
        size_t ptr;
        bool   used{false};
    };

    template<typename Span>
    requires semantic::concepts::Span<Span>
    //
    [[nodiscard]] sync_status move_fences(Span const& data)
    {
        // TODO: Check for available writable space according to fences
        auto required_end = m_current_write + data.size();
        // ...
        auto remaining_space = m_buffer_size - m_current_write;
        if(remaining_space >= data.size())
            // There's enough space at the end of the buffer to write
            return sync_status::ok;
        // There's not enough space at the end,
        // so we'll write it at the start of the buffer
        // We'll start by finding a large enough region and checking its fence
        auto fence_it = std::find_if(
            m_fences.begin(), m_fences.end(), [&data](auto const& fence) {
                return fence.first >= data.size();
            });
        if(fence_it == m_fences.end())
            return sync_status::fail;
        GLsync             target_fence = fence_it->second;
        group::sync_status status;
        u64                timeout = 0;
        while(true)
        {
            status = cmd::client_wait_sync(
                target_fence,
                group::sync_object_mask::sync_flush_commands_bit,
                timeout);
            switch(status)
            {
            case group::sync_status::already_signaled:
            case group::sync_status::condition_satisfied:
                m_current_write = 0;
                return sync_status::ok;
            case group::sync_status::wait_failed:
                return sync_status::fail;
            case group::sync_status::timeout_expired:
                timeout = m_timeout;
                break;
            }
            m_debug.message("circular buffer waiting on reset, consider "
                            "increasing buffer size");
        }
    }

    template<typename Span>
    requires semantic::concepts::Span<Span>
    //
    [[nodiscard]] fence_options push(sync_status /*status*/, Span const& data)
    {
        m_buffer->update(m_current_write, data);
        auto written_size = data.size() * sizeof(typename Span::value_type);
        m_current_write += written_size;
        return fence_options(m_current_write - written_size, m_current_write);
    }

    void add_fence(fence_options&& ptr)
    {
        m_fences.push_back(std::make_pair(
            ptr.ptr,
            cmd::fence_sync(
                group::sync_condition::sync_gpu_commands_complete,
                group::sync_behavior_flags::none)));
        ptr.used = true;
    }

    buffer_t& buffer()
    {
        return *m_buffer;
    }

  private:
    debug::api&                            m_debug;
    std::shared_ptr<buffer_t>              m_buffer;
    size_t                                 m_current_write{0};
    size_t                                 m_buffer_size{0};
    std::vector<std::pair<size_t, GLsync>> m_fences;
    u64                                    m_timeout{0};
};

} // namespace gleam
