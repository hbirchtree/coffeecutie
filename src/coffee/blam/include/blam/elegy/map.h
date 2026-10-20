#pragma once

#include "tag_index.h"

#include <peripherals/semantic/chunk.h>

namespace blam::elegy {

/*!
 * \brief A tag and its data as T, both null when there is no such tag or it
 * is of another group. Dereferences to the data.
 */
template<typename T>
struct tagged
{
    tag_t const* tag{nullptr};
    T const*     data{nullptr};

    inline explicit operator bool() const
    {
        return data != nullptr;
    }

    inline T const* operator->() const
    {
        return data;
    }

    inline T const& operator*() const
    {
        return *data;
    }
};

/*!
 * \brief Read-only view of a Halo 3 cache file, held resident. Wires the
 * header's section masks into pointer translation the way Assembly's
 * ThirdGenHeader does: tag pointers land at
 * sections[tag].virtual_address + offset_masks[tag] + (pointer -
 * virtual_base), debug pointers (names, string_ids) at pointer +
 * offset_masks[debug].
 */
struct map_container
{
    using result_type = result<map_container, map_load_error>;

    file_header_t const* header{nullptr};
    tag_index_t const*   index{nullptr};
    Span<const u8>   data;

    static result_type from_bytes(semantic::BytesConst const& map)
    {
        if(map.size < sizeof(file_header_t))
            return stl_types::failure(map_load_error::map_file_too_small);
        auto const* header = reinterpret_cast<file_header_t const*>(map.data);
        if(!header->valid())
            return stl_types::failure(map_load_error::not_a_map);
        if(header->version.value() != halo3_version)
            return stl_types::failure(
                map_load_error::incompatible_map_version_expected_halo3);
        map_container out{
            .header = header,
            .data   = Span<const u8>(map.data, map.size),
        };
        /* shared.map and friends are all resources, no tags */
        if(header->section(section_t::tag).size.value() == 0)
            return out;
        out.index = out.magic().as<tag_index_t>(header->index_address);
        if(!out.index || !out.index->valid())
            return stl_types::failure(map_load_error::not_a_map);
        return out;
    }

    inline bool has_tags() const
    {
        return index != nullptr;
    }

    inline tag_magic magic() const
    {
        auto const& tag = header->section(section_t::tag);
        return tag_magic{{
            .base         = data.data(),
            .max_size     = static_cast<u32>(data.size()),
            .file_offset  = header->section_offset(
                section_t::tag, tag.virtual_address),
            .virtual_base = header->virtual_base,
        }};
    }

    /*! File offset of the resource section, which raw pages are relative to */
    inline u32 resource_offset() const
    {
        return header->section_offset(
            section_t::resource,
            header->section(section_t::resource).virtual_address);
    }

    inline Span<group_t const> groups() const
    {
        if(!index || index->group_count.value() <= 0)
            return {};
        auto count = static_cast<u32>(index->group_count.value());
        auto const* groups = magic().as<group_t>(index->group_table_address, count);
        if(!groups)
            return {};
        return Span<group_t const>(groups, count);
    }

    inline Span<tag_t const> tags() const
    {
        if(!index || index->tag_count.value() <= 0)
            return {};
        auto count = static_cast<u32>(index->tag_count.value());
        auto const* tags = magic().as<tag_t>(index->tag_table_address, count);
        if(!tags)
            return {};
        return Span<tag_t const>(tags, count);
    }

    inline tag_t const* tag_at(datum_index id) const
    {
        return lookup_tag(
            tags(), id.value(), [](tag_t const& t) { return t.salt.value(); });
    }

    inline datum_index id_of(tag_t const& tag) const
    {
        auto position = static_cast<u32>(&tag - tags().data());
        datum_index out;
        u32         raw = to_be((u32(tag.salt.value()) << 16) | position);
        std::memcpy(out.raw.raw.data(), &raw, sizeof(raw));
        return out;
    }

    inline group_t const* group_of(tag_t const& tag) const
    {
        auto table = groups();
        i16  idx   = tag.group_index;
        if(idx < 0 || static_cast<size_t>(idx) >= table.size())
            return nullptr;
        return &table[static_cast<size_t>(idx)];
    }

    inline bool is(tag_t const& tag, tag_class_t group) const
    {
        auto const* g = group_of(tag);
        return g && g->group == group;
    }

    inline std::string_view name_of(tag_t const& tag) const
    {
        auto table = tags();
        if(table.empty() || &tag < table.data() ||
           &tag >= table.data() + table.size())
            return {};
        return table_string(
            static_cast<u32>(&tag - table.data()),
            header->file_count,
            header->file_index_address,
            header->file_table_address,
            header->file_table_size);
    }

    /*! First tag of a group whose name contains `name` */
    inline tag_t const* find(
        tag_class_t group, std::string_view name = {}) const
    {
        for(auto const& tag : tags())
            if(tag.valid() && is(tag, group) &&
               name_of(tag).find(name) != std::string_view::npos)
                return &tag;
        return nullptr;
    }

    template<typename T>
    inline T const* tag_data(tag_t const& tag) const
    {
        return tag.data<T>(magic());
    }

    /*! The tag `id` names, read as T; checked against T::group when T is
     * one group's (object_t and render_method_t span several) */
    template<typename T>
    inline tagged<T> get(datum_index id) const
    {
        auto const* tag = tag_at(id);
        if(!tag)
            return {};
        if constexpr(requires { T::group; })
            if(!is(*tag, T::group))
                return {};
        auto const* data = tag_data<T>(*tag);
        return data ? tagged<T>{tag, data} : tagged<T>{};
    }

    /*! Name of the tag `ref` points at, empty when it is null */
    inline std::string_view name_of(tag_reference const& ref) const
    {
        auto const* tag = tag_at(ref.id);
        return tag ? name_of(*tag) : std::string_view();
    }

    template<typename T>
    inline tagged<T> get(tag_reference const& ref) const
    {
        return get<T>(ref.id);
    }

    /*! First tag of T's group whose name contains `name` */
    template<typename T>
    inline tagged<T> find(std::string_view name = {}) const
    {
        auto const* tag = find(T::group, name);
        return tag ? get<T>(id_of(*tag)) : tagged<T>{};
    }

    /*! A block's elements in this map, none when it is out of bounds */
    template<typename T>
    inline Span<T const> span(tag_block<T> const& block) const
    {
        return block.span(magic());
    }

    /*!
     * Resolve a string_id. Namespaces are runs of the string table; the
     * offsets come from Assembly's H3_StringIDs.xml. Namespace 0 holds
     * indices past its own run in a block after the others.
     */
    inline std::string_view string(string_id id) const
    {
        constexpr std::array<u32, 9> ns_start = {
            0x0, 0x4B7, 0xA7D, 0xB0F, 0xBAF, 0xB63, 0xBBF, 0xBF0, 0xC04};
        constexpr u32 ns0_size  = 0x4B7;
        constexpr u32 ns0_extra = 0xC11;
        u32 ns  = id.name_space();
        u32 idx = id.index();
        if(ns >= ns_start.size())
            return {};
        u32 global = ns == 0 ? (idx < ns0_size ? idx : idx - ns0_size + ns0_extra)
                             : ns_start[ns] + idx;
        return table_string(
            global,
            header->string_count,
            header->string_index_address,
            header->string_table_address,
            header->string_table_size);
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
    /* Tag names and string_ids: u32 offsets into a buffer of NUL-terminated
     * strings, both addressed through the debug section */
    std::string_view table_string(
        u32 position,
        u32 count,
        u32 index_address,
        u32 buffer_address,
        u32 buffer_size) const;
};

} // namespace blam::elegy
