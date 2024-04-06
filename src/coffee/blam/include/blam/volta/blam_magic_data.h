#pragma once

#include "blam_base_types.h"
#include "blam_file_header.h"

namespace blam {

struct file_header_t;
struct atlas_view;

struct map_ptr
{
    map_ptr()
    {
        base_ptr = nullptr;
    }

    map_ptr(semantic::Span<const byte_t> const& data, u32 magic = 0)
        : file_offset(magic)
        , max_size(data.size())
    {
        base_ptr = data.data();
    }

    inline map_ptr& operator=(semantic::Bytes const& data)
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

    inline map_ptr ptr_only() const
    {
        return map_ptr(data());
    }

    union
    {
        file_header_t const* header_ptr;
        byte_t const*        base_ptr{nullptr};
    };

    u32 file_offset{0};
    u32 max_size{0};
};

} // namespace blam
