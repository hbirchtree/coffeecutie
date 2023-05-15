#pragma once

#include "blam_base_types.h"
#include "blam_file_header.h"

namespace blam {

struct file_header_t;

struct magic_data_t
{
    magic_data_t()
    {
        base_ptr = nullptr;
    }
    magic_data_t(semantic::Span<const byte_t> const& data, u32 magic = 0) :
        magic_offset(magic), max_size(data.size())
    {
        base_ptr = data.data();
    }
    inline magic_data_t& operator=(semantic::Bytes const& data)
    {
        base_ptr     = data.data;
        magic_offset = 0;
        max_size     = data.size;

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
    inline magic_data_t no_magic() const
    {
        return magic_data_t(data());
    }

    union
    {
        file_header_t const* header_ptr;
        byte_t const*        base_ptr{nullptr};
    };
    u32 magic_offset{0};
    u32 max_size{0};
};

} // namespace blam