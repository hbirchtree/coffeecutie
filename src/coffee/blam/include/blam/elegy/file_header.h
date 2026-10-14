#pragma once

#include "structures.h"

namespace blam::elegy {

using bl_path = bl_string_var<256>;

/*! An address range the cache loads into memory */
struct partition_t
{
    be<u32> load_address;
    be<u32> size;
};

/*! Virtual address and size of one of the header's section_t */
struct section_range_t
{
    be<u32> virtual_address;
    be<u32> size;
};

/*!
 * \brief Cache file header for Halo 3 on Xbox 360, 0x3000 bytes, all
 * big-endian. Same "head"/"foot" literals as earlier titles.
 *
 * Every pointer in the file belongs to a section_t; sections[i] gives its
 * virtual base and offset_masks[i] the value adding which yields a file
 * offset. Tag pointers are further relative to virtual_base.
 */
struct file_header_t
{
    bl_header          id;
    be<i32>            version;
    be<u32>            file_size;
    u32                unknown_0;
    be<u32>            index_address; /*!< Tag index header, a tag pointer */
    be<u32>            tag_buffer_address;
    be<u32>            tag_buffer_size;
    bl_path            source_file;
    bl_string          build; /*!< e.g. "11855.07.08.20.2317.halo3_ship" */
    be<cache_type_t>   map_type;
    u16                unknown_1;
    std::array<u32, 6> unknown_2;

    /* string_id tables, debug section pointers */
    be<u32> string_count;
    be<u32> string_table_size;
    be<u32> string_index_address;
    be<u32> string_table_address;

    u32                unknown_3;
    std::array<be<u32>, 2> build_date; /*!< FILETIME, high dword first */
    std::array<u32, 6>     unknown_4;

    bl_string name;
    u32       unknown_5;
    bl_path   scenario; /*!< e.g. "levels\multi\guardian\guardian" */
    u32       unknown_6;

    /* Tag names, debug section pointers; one index entry per tag */
    be<u32> file_count;
    be<u32> file_table_address;
    be<u32> file_table_size;
    be<u32> file_index_address;

    be<u32>             checksum;
    std::array<u32, 8>  unknown_7;
    be<u32>             virtual_base;
    be<u32>             xdk_version;
    std::array<partition_t, 6> partitions;
    std::array<u32, 83>        unknown_8;

    std::array<be<u32>, 4>         offset_masks;
    std::array<section_range_t, 4> sections;

    std::array<u32, 2776> reserved;
    bl_footer             footer;

    inline bool valid() const
    {
        return id == bl_header{'h', 'e', 'a', 'd'} &&
               footer == bl_footer{'f', 'o', 'o', 't'};
    }

    inline section_range_t const& section(section_t s) const
    {
        return sections[static_cast<size_t>(s)];
    }

    /*! File offset of a pointer in section s */
    inline u32 section_offset(section_t s, u32 pointer) const
    {
        return pointer + offset_masks[static_cast<size_t>(s)].value();
    }
};

static_assert(offsetof(file_header_t, index_address) == 0x010);
static_assert(offsetof(file_header_t, tag_buffer_size) == 0x018);
static_assert(offsetof(file_header_t, build) == 0x11C);
static_assert(offsetof(file_header_t, map_type) == 0x13C);
static_assert(offsetof(file_header_t, string_count) == 0x158);
static_assert(offsetof(file_header_t, string_table_address) == 0x164);
static_assert(offsetof(file_header_t, build_date) == 0x16C);
static_assert(offsetof(file_header_t, name) == 0x18C);
static_assert(offsetof(file_header_t, scenario) == 0x1B0);
static_assert(offsetof(file_header_t, file_count) == 0x2B4);
static_assert(offsetof(file_header_t, checksum) == 0x2C4);
static_assert(offsetof(file_header_t, virtual_base) == 0x2E8);
static_assert(offsetof(file_header_t, partitions) == 0x2F0);
static_assert(offsetof(file_header_t, offset_masks) == 0x46C);
static_assert(offsetof(file_header_t, sections) == 0x47C);
static_assert(offsetof(file_header_t, footer) == 0x2FFC);
static_assert(sizeof(file_header_t) == 0x3000);

} // namespace blam::elegy
