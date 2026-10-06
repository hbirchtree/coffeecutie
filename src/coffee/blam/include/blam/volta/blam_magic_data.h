#pragma once

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

enum class ptr_tag
{
    map,
    bsp,
    vertex,
};

template<ptr_tag Tag>
struct map_ptr_base
{
    map_ptr_base()
    {
        base_ptr = nullptr;
    }

    map_ptr_base(semantic::Span<const byte_t> const& data, u32 magic = 0)
        requires(Tag == ptr_tag::map)
        : file_offset(magic)
        , max_size(data.size())
    {
        base_ptr = data.data();
    }

    inline map_ptr_base& operator=(semantic::Bytes const& data)
        requires(Tag == ptr_tag::map)
    {
        base_ptr    = data.data;
        file_offset = 0;
        max_size    = data.size;

        return *this;
    }

    auto data() const
    {
        return semantic::Span<const byte_t>(base_ptr, max_size);
    }

    inline version_t map_version() const
    {
        return header_ptr->version;
    }

    inline map_ptr_base ptr_only() const
        requires(Tag == ptr_tag::map)
    {
        return map_ptr_base(data());
    }

    union
    {
        file_header_t const* header_ptr;
        byte_t const*        base_ptr{nullptr};
    };

    u32 file_offset{0};
    u32 max_size{0};

  private:
    friend struct bsp::info;
    template<typename V>
    friend struct tag_index_t;

    map_ptr_base(semantic::Span<const byte_t> const& data, u32 magic)
        requires(Tag != ptr_tag::map)
        : file_offset(magic)
        , max_size(data.size())
    {
        base_ptr = data.data();
    }
};

using bsp_ptr = map_ptr_base<ptr_tag::bsp>;
using map_ptr = map_ptr_base<ptr_tag::map>;
using vertex_ptr = map_ptr_base<ptr_tag::vertex>;

} // namespace blam
