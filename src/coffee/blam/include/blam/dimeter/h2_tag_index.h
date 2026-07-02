#pragma once

#include "h2_file_header.h"

namespace blam::dimeter {

/*!
 * \brief Header of the tag index ("meta header" in Assembly), located at
 * file_header::meta_offset. On Xbox the *_pointer fields are virtual
 * addresses, on Vista they are offsets relative to this header.
 */
struct alignas(4) tag_index_t
{
    u32         group_table_pointer;
    i32         group_count;
    u32         tag_table_pointer;
    datum_index scenario_id;
    datum_index globals_id;
    u32         unknown_0;
    i32         tag_count;
    bl_tag      magic; /*!< 'tags' */

    inline bool valid() const
    {
        return std::string_view(magic.data.data(), 4) == "sgat"sv;
    }
};

static_assert(offsetof(tag_index_t, scenario_id) == 0x0C);
static_assert(offsetof(tag_index_t, tag_count) == 0x18);
static_assert(sizeof(tag_index_t) == 0x20);

/*!
 * \brief Tag group ("class") table entry; Halo 2 tracks the inheritance chain
 * per group. 0xFFFFFFFF for no parent.
 */
struct alignas(4) group_t
{
    bl_tag group;
    bl_tag parent;
    bl_tag grandparent;

    inline bool has_parent() const
    {
        return static_cast<u32>(parent) != 0xFFFFFFFFu;
    }
};

static_assert(sizeof(group_t) == 0xC);

/*!
 * \brief Tag table entry. Unlike Halo 1 there is no name pointer here; names
 * live in the file header's file table, indexed by the low 16 bits of the
 * datum index (= position in this table).
 */
struct alignas(4) tag_t
{
    bl_tag      tag_class;
    datum_index tag_id;
    u32         pointer_raw; /*!< Virtual address of the tag data */
    i32         size_raw;

    inline u32 pointer() const
    {
        return from_le(pointer_raw);
    }

    inline i32 size() const
    {
        return from_le(size_raw);
    }

    /*! Compares against the human-readable class name, e.g. "bitm"; classes
     * are stored byte-reversed in the file */
    inline bool matches(std::string_view tag_class_name) const
    {
        if(tag_class_name.size() != 4)
            return false;
        for(size_t i = 0; i < 4; i++)
            if(tag_class.data[i] != tag_class_name[3 - i])
                return false;
        return true;
    }

    /*!
     * Null tags exist in the table as padding; they have no data.
     */
    inline bool valid() const
    {
        return static_cast<u32>(tag_class) != 0xFFFFFFFFu && pointer_raw != 0;
    }

    template<typename T>
    inline T const* data(magic_ptr const& magic) const
    {
        if(!valid())
            return nullptr;
        i32 bytes = size();
        if(bytes > 0 && static_cast<u32>(bytes) < sizeof(T))
            return nullptr;
        auto offset = magic.to_offset(
            pointer(), bytes > 0 ? static_cast<u32>(bytes) : sizeof(T));
        if(!offset)
            return nullptr;
        return reinterpret_cast<T const*>(magic.base + *offset);
    }
};

static_assert(sizeof(tag_t) == 0x10);

} // namespace blam::dimeter
