#pragma once

#include "../common/magic.h"
#include "blam_base_types.h"
#include "blam_file_header.h"

namespace blam {

struct file_header_t;
struct atlas_view;

namespace bsp {
struct info;
}
template<typename V>
struct tag_index_t;

/*! Halo 1 pointers are file offsets shifted by the map's magic */
template<heap_t Heap>
struct map_ptr_base : address_range
{
    map_ptr_base() = default;

    map_ptr_base(semantic::Span<const u8> const& data, u32 magic = 0)
        requires(Heap == heap_t::map)
        : address_range{data.data(), static_cast<u32>(data.size()), 0, magic}
    {
    }

    inline map_ptr_base& operator=(semantic::Bytes const& data)
        requires(Heap == heap_t::map)
    {
        base         = data.data;
        max_size     = data.size;
        file_offset  = 0;
        virtual_base = 0;

        return *this;
    }

    auto data() const
    {
        return semantic::Span<const u8>(base, max_size);
    }

    inline file_header_t const* header() const
    {
        return reinterpret_cast<file_header_t const*>(base);
    }

    inline version_t map_version() const
    {
        return header()->version;
    }

    inline map_ptr_base ptr_only() const
        requires(Heap == heap_t::map)
    {
        return map_ptr_base(data());
    }

  private:
    friend struct bsp::info;
    template<typename V>
    friend struct tag_index_t;

    map_ptr_base(semantic::Span<const u8> const& data, u32 magic)
        requires(Heap != heap_t::map)
        : address_range{data.data(), static_cast<u32>(data.size()), 0, magic}
    {
    }
};

using bsp_ptr    = map_ptr_base<heap_t::bsp>;
using map_ptr    = map_ptr_base<heap_t::map>;
using vertex_ptr = map_ptr_base<heap_t::vertex>;

} // namespace blam
