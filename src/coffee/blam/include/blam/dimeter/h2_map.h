#pragma once

#include "h2_tag_index.h"

#include <peripherals/semantic/chunk.h>

#include <cstring>

namespace blam::dimeter {

/*!
 * \brief Resolves raw_refs across the cache file set. Slots follow
 * raw_file_t; missing side files just fail resolution for refs into them.
 */
struct raw_pool
{
    std::array<Span<const byte_t>, 4> files;

    inline optional<Span<const byte_t>> resolve(raw_ref ref, u32 size) const
    {
        if(!ref.valid())
            return std::nullopt;
        auto const& file = files[static_cast<u8>(ref.file())];
        u32         offset = ref.offset();
        if(file.empty() || offset > file.size() ||
           size > file.size() - offset)
            return std::nullopt;
        return file.subspan(offset, size);
    }
};

/*!
 * \brief Read-only view of a Halo 2 cache file. Wires together the file
 * header, the tag index and the pointer masks needed to resolve virtual
 * addresses, mirroring how Assembly's SecondGenCacheFile derives them:
 *
 * - Xbox: the index area mask is group_table_pointer - sizeof(tag_index_t)
 *   (the group table directly follows the index header), and the meta mask is
 *   first_tag.pointer - tag_table_size (tag data directly follows the index
 *   stream in the file).
 * - Vista: index pointers are already offsets relative to the index header,
 *   and the meta mask is stored in the file header.
 */
template<typename V>
requires is_game_version<V>
struct map_container
{
    using header_type = file_header_t<V>;
    using result_type = result<map_container, map_load_error>;

    header_type const* header{nullptr};
    tag_index_t const* index{nullptr};
    u32                index_mask{0};
    u32                meta_mask{0};

    Span<const byte_t> data;

    static result_type from_bytes(semantic::BytesConst const& map, V)
    {
        if(map.size < sizeof(header_type))
            return map_load_error::map_file_too_small;

        auto const* header
            = reinterpret_cast<header_type const*>(map.data);
        if(!header->valid())
            return map_load_error::not_a_map;

        u32 meta_offset = from_le(header->meta_offset);
        if(meta_offset > map.size - sizeof(tag_index_t))
            return map_load_error::not_a_map;
        auto const* index
            = reinterpret_cast<tag_index_t const*>(map.data + meta_offset);
        /* Vista maps ship compressed; the header survives but the index magic
         * won't be found in that case */
        if(!index->valid())
            return map_load_error::not_a_map;

        /* Same version value either way, so tell the layouts apart by the
         * group table pointer: a virtual address on Xbox, a small
         * header-relative offset on Vista */
        u32 group_ptr = from_le(index->group_table_pointer);
        bool is_vista = group_ptr < from_le(header->meta_size);
        if(is_vista != std::is_same_v<V, vista_version_t>)
            return map_load_error::incompatible_map_version_expected_halo2;

        map_container out{
            .header = header,
            .index  = index,
            .data   = Span<const byte_t>(map.data, map.size),
        };

        if constexpr(std::is_same_v<V, xbox_version_t>)
        {
            out.index_mask = group_ptr - sizeof(tag_index_t);
            auto tags = out.tags();
            if(tags.empty())
                return map_load_error::not_a_map;
            out.meta_mask
                = tags[0].pointer() - from_le(header->tag_table_size);
        } else
        {
            out.index_mask = 0;
            out.meta_mask  = from_le(header->meta_offset_mask);
        }

        return out;
    }

    /*! Resolves pointers within the tag index (group + tag tables) */
    inline magic_ptr index_magic() const
    {
        return magic_ptr{
            .base        = data.data(),
            .max_size    = static_cast<u32>(data.size()),
            .meta_offset = from_le(header->meta_offset),
            .mask        = index_mask,
        };
    }

    /*! Resolves pointers within tag data (tag_t::data, tag_block) */
    inline magic_ptr magic() const
    {
        return magic_ptr{
            .base        = data.data(),
            .max_size    = static_cast<u32>(data.size()),
            .meta_offset = from_le(header->meta_offset),
            .mask        = meta_mask,
        };
    }

    inline Span<group_t const> groups() const
    {
        i32 count = from_le(index->group_count);
        if(count <= 0)
            return {};
        magic_ptr   magic  = index_magic();
        auto const* groups = magic.as<group_t>(
            from_le(index->group_table_pointer), static_cast<u32>(count));
        if(!groups)
            return {};
        return Span<group_t const>(groups, static_cast<size_t>(count));
    }

    inline Span<tag_t const> tags() const
    {
        i32 count = from_le(index->tag_count);
        if(count <= 0)
            return {};
        magic_ptr   magic = index_magic();
        auto const* tags  = magic.as<tag_t>(
            from_le(index->tag_table_pointer), static_cast<u32>(count));
        if(!tags)
            return {};
        return Span<tag_t const>(tags, static_cast<size_t>(count));
    }

    inline tag_t const* tag_at(datum_index id) const
    {
        auto table = tags();
        u16  idx   = id.index();
        if(!id.valid() || idx >= table.size())
            return nullptr;
        return &table[idx];
    }

    inline tag_t const* scenario() const
    {
        return tag_at(index->scenario_id);
    }

    inline tag_t const* globals() const
    {
        return tag_at(index->globals_id);
    }

    /*!
     * Tag names are stored in the header's file table, one entry per tag
     * table slot, looked up by position.
     */
    inline std::string_view tag_name(tag_t const& tag) const
    {
        auto table = tags();
        if(table.empty() || &tag < table.data())
            return {};
        auto position = static_cast<u32>(&tag - table.data());
        return table_string(
            position,
            from_le(header->file_table_count),
            from_le(header->file_index_offset),
            from_le(header->file_table_offset),
            from_le(header->file_table_size));
    }

    /*! Resolve a string_id through the header's string table */
    inline std::string_view string(string_id id) const
    {
        return table_string(
            id.index(),
            from_le(header->string_count),
            from_le(header->string_index_offset),
            from_le(header->string_table_offset),
            from_le(header->string_table_size));
    }

    inline std::string_view name() const
    {
        return header->name.str();
    }

    inline std::string_view scenario_name() const
    {
        return header->scenario.str();
    }

    inline std::string_view build() const
    {
        return header->build.str();
    }

  private:
    /* Both the tag name table and the string_id table are an array of u32
     * offsets plus a packed buffer of NUL-terminated strings, addressed by
     * absolute file offsets from the header */
    inline std::string_view table_string(
        u32 position,
        u32 count,
        u32 index_offset,
        u32 buffer_offset,
        u32 buffer_size) const
    {
        u32 map_size = static_cast<u32>(data.size());
        if(position >= count)
            return {};
        if(index_offset > map_size || count > (map_size - index_offset) / 4)
            return {};
        if(buffer_offset > map_size || buffer_size > map_size - buffer_offset)
            return {};
        auto const* offsets
            = reinterpret_cast<u32 const*>(data.data() + index_offset);
        u32 string_offset = from_le(offsets[position]);
        if(string_offset >= buffer_size)
            return {};
        auto const* start = reinterpret_cast<char const*>(
            data.data() + buffer_offset + string_offset);
        auto max_len = static_cast<size_t>(buffer_size - string_offset);
        return std::string_view(start, ::strnlen(start, max_len));
    }
};

} // namespace blam::dimeter
