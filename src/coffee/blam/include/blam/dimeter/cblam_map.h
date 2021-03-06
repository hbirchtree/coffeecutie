#pragma once

#include <coffee/core/CTypes>

namespace Coffee {

namespace CBlam {
namespace Dimeter {

/*!
 * \brief Possible Blam versions which we may encounter. The different formats
 * work in different ways. For instance, Xbox stores bitmaps and other data in a
 * single file while PC spreads it across "bitmaps.map" and "sounds.map". Both
 * ways are necessary for parsing to happen correctly.
 */
enum blam_versions
{
    blam_version_h2 = 8, /*!< The Halo 2 map version*/
};

/*!
 * \brief Halo 2 header, credits to Assembly project for offsets
 */
struct blam_file_header_h2
{
    byte_t header[4];
    uint32 version;
    uint32 filesize;
    uint32 unknown2;
    uint32 meta_offset;
    uint32 tagdata_offset;
    int32  tagdata_size;
    int32  meta_size;
    uint32 meta_offset_mask;
    byte_t reserved1[264];
    byte_t buildDate[32];
    int32  maptype;
    byte_t unknown3[28];
    int32  strings_block_offset;
    int32  strings_table_count;
    int32  strings_table_size;
    uint32 strings_indextable_offset;
    uint32 strings_table_offset;
    byte_t unknown4[32];
    uint32 unknown8;
    byte_t name[32];
    uint32 unknown7;
    byte_t scenario[260];
    uint32 file_table_count;
    uint32 file_table_offset;
    int32  file_table_size;
    uint32 file_indextable_offset;
    uint32 unknown5[3];
    uint32 raw_table_offset;
    int32  raw_table_size;
    uint32 map_checksum;
    uint32 unknown6;

    byte_t reserved2[1284];

    byte_t footer[4];
};

} // namespace Dimeter
} // namespace CBlam
} // namespace Coffee
