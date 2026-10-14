#pragma once

#include "file_header.h"

namespace blam::elegy {

/*!
 * \brief Header of the tag index, at file_header_t::index_address. All
 * addresses are tag pointers.
 */
struct tag_index_t
{
    be<i32> group_count;
    be<u32> group_table_address;
    be<i32> tag_count;
    be<u32> tag_table_address;
    be<i32> global_count;
    be<u32> global_table_address;
    be<i32> interop_count;
    be<u32> interop_table_address;
    u32     unknown_0;
    bl_tag  magic; /*!< 'tags' */

    inline bool valid() const
    {
        return std::string_view(magic.data.data(), 4) == "tags"sv;
    }
};

static_assert(offsetof(tag_index_t, interop_table_address) == 0x1C);
static_assert(sizeof(tag_index_t) == 0x28);

/*! Tag group table entry with its inheritance chain */
struct group_t
{
    tag_class group;
    tag_class parent;
    tag_class grandparent;
    string_id name;
};

static_assert(sizeof(group_t) == 0x10);

/*!
 * \brief Tag table entry. The group is an index into the group table, and
 * the tag's datum index is (salt << 16 | position in this table). Names
 * live in the header's file table, by the same position.
 */
struct tag_t
{
    be<i16> group_index;
    be<u16> salt;
    be<u32> address;

    /*! Null tags pad the table; they have no group and no data */
    inline bool valid() const
    {
        return group_index.value() >= 0 && address.value() != 0;
    }

    template<typename T>
    inline T const* data(tag_magic const& magic) const
    {
        if(!valid())
            return nullptr;
        return magic.as<T>(address);
    }
};

static_assert(sizeof(tag_t) == 0x8);

} // namespace blam::elegy
