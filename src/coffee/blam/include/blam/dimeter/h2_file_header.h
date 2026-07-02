#pragma once

#include "h2_structures.h"

namespace blam::dimeter {

using bl_path = bl_string_var<256>;

/*!
 * \brief Cache file header for Halo 2 on Xbox, 2048 bytes like Halo 1's.
 * Same "head"/"foot" literals and a version value of 8. Field offsets from
 * Assembly's H2X_Layouts_Core.xml.
 *
 * The meta area (tag index + tag data) starts at meta_offset;
 * tag_table_size is the size of the tag index stream, so tag data starts at
 * meta_offset + tag_table_size in the file.
 */
struct alignas(4) file_header_xbox_t
{
    bl_header    id;             /*!< 'head' */
    version_t    version;        /*!< Always version_t::halo2 (8) */
    u32          file_size;
    u32          unknown_0;
    u32          meta_offset;    /*!< File offset of the tag index */
    u32          tag_table_size; /*!< Size of the tag index stream */
    u32          tag_data_size;
    u32          meta_size;
    bl_path      source_file;    /*!< Build machine path, usually empty */
    bl_string    build;          /*!< e.g. "02.09.27.09809" */
    cache_type_t map_type;
    u32          crc;
    u32          unknown_1[3];

    /* Low-detail texture data, stored outside the tag data */
    u32 lowres_bitmap_count;
    u32 lowres_bitmap_offset;
    u32 lowres_bitmap_size;

    /* string_id tables; the block is count * 128-byte fixed entries, the
     * index + table pair is the packed variant (offsets into the table) */
    u32 string_block_offset;
    u32 string_count;
    u32 string_table_size;
    u32 string_index_offset;
    u32 string_table_offset;

    u32 unknown_2;
    u32 build_date[2]; /*!< FILETIME, high dword last in memory */
    u32 unknown_3[6];  /*!< FILETIMEs of shared map dependencies */

    bl_string name;
    u32       unknown_4;
    bl_path   scenario; /*!< e.g. "scenarios\multi\halo\beavercreek\..." */
    u32       unknown_5;

    /* Tag name table ("file table"); index holds offsets into the buffer,
     * one per entry in the tag table */
    u32 file_table_count;
    u32 file_table_offset;
    u32 file_table_size;
    u32 file_index_offset;

    u32 checksum;

    u32       reserved[330];
    bl_footer footer; /*!< 'foot' */

    inline bool valid() const
    {
        return id == bl_header{'d', 'a', 'e', 'h'} &&
               from_le(version) == version_t::halo2 &&
               footer == bl_footer{'t', 'o', 'o', 'f'};
    }
};

static_assert(offsetof(file_header_xbox_t, meta_offset) == 0x010);
static_assert(offsetof(file_header_xbox_t, source_file) == 0x020);
static_assert(offsetof(file_header_xbox_t, build) == 0x120);
static_assert(offsetof(file_header_xbox_t, map_type) == 0x140);
static_assert(offsetof(file_header_xbox_t, lowres_bitmap_count) == 0x154);
static_assert(offsetof(file_header_xbox_t, string_block_offset) == 0x160);
static_assert(offsetof(file_header_xbox_t, string_index_offset) == 0x16C);
static_assert(offsetof(file_header_xbox_t, build_date) == 0x178);
static_assert(offsetof(file_header_xbox_t, name) == 0x198);
static_assert(offsetof(file_header_xbox_t, scenario) == 0x1BC);
static_assert(offsetof(file_header_xbox_t, file_table_count) == 0x2C0);
static_assert(offsetof(file_header_xbox_t, checksum) == 0x2D0);
static_assert(offsetof(file_header_xbox_t, footer) == 0x7FC);
static_assert(sizeof(file_header_xbox_t) == 0x800);

/*!
 * \brief Cache file header for Halo 2 Vista. Mostly the Xbox layout shifted
 * by 0xC (source_file moves from 0x20 to 0x2C), with the meta pointer mask
 * stored in the header instead of being derived from the tag index, plus
 * trailing fields for localization and the raw data table. Field offsets from
 * Assembly's H2V_Layouts_Core.xml.
 */
struct alignas(4) file_header_vista_t
{
    bl_header    id;
    version_t    version;
    u32          file_size;
    u32          unknown_0;
    u32          meta_offset;
    u32          tag_table_size;
    u32          tag_data_size;
    u32          meta_size;
    u32          meta_offset_mask; /*!< Virtual address of the tag index */
    u32          unknown_1[2];
    bl_path      source_file;
    bl_string    build; /*!< e.g. "11081.07.04.30.0934.main" */
    cache_type_t map_type;
    u32          crc;
    u32          unknown_2[3];

    u32 lowres_bitmap_count;
    u32 lowres_bitmap_offset;
    u32 lowres_bitmap_size;

    u32 string_block_offset;
    u32 string_count;
    u32 string_table_size;
    u32 string_index_offset;
    u32 string_table_offset;

    u32 unknown_3;
    u32 build_date[2];
    u32 unknown_4[6];

    bl_string name;
    u32       unknown_5;
    bl_path   scenario;
    u32       unknown_6;

    u32 file_table_count;
    u32 file_table_offset;
    u32 file_table_size;
    u32 file_index_offset;

    /* Vista multiplayer maps have no matg tag, so language data is referenced
     * from the header instead */
    u32 locale_globals_offset;
    u32 locale_globals_size;

    datum_index sound_gestalt;

    u32 raw_table_offset;
    u32 raw_table_size;

    u32 checksum;

    u32       reserved[322];
    bl_footer footer;

    inline bool valid() const
    {
        return id == bl_header{'d', 'a', 'e', 'h'} &&
               from_le(version) == version_t::halo2 &&
               footer == bl_footer{'t', 'o', 'o', 'f'};
    }
};

static_assert(offsetof(file_header_vista_t, meta_offset) == 0x010);
static_assert(offsetof(file_header_vista_t, meta_offset_mask) == 0x020);
static_assert(offsetof(file_header_vista_t, source_file) == 0x02C);
static_assert(offsetof(file_header_vista_t, build) == 0x12C);
static_assert(offsetof(file_header_vista_t, map_type) == 0x14C);
static_assert(offsetof(file_header_vista_t, lowres_bitmap_count) == 0x160);
static_assert(offsetof(file_header_vista_t, string_block_offset) == 0x16C);
static_assert(offsetof(file_header_vista_t, string_index_offset) == 0x178);
static_assert(offsetof(file_header_vista_t, build_date) == 0x184);
static_assert(offsetof(file_header_vista_t, name) == 0x1A4);
static_assert(offsetof(file_header_vista_t, scenario) == 0x1C8);
static_assert(offsetof(file_header_vista_t, file_table_count) == 0x2CC);
static_assert(offsetof(file_header_vista_t, locale_globals_offset) == 0x2DC);
static_assert(offsetof(file_header_vista_t, raw_table_offset) == 0x2E8);
static_assert(offsetof(file_header_vista_t, checksum) == 0x2F0);
static_assert(offsetof(file_header_vista_t, footer) == 0x7FC);
static_assert(sizeof(file_header_vista_t) == 0x800);

template<typename V>
requires is_game_version<V>
using file_header_t = std::conditional_t<
    std::is_same_v<V, xbox_version_t>,
    file_header_xbox_t,
    file_header_vista_t>;

} // namespace blam::dimeter
