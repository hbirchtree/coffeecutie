#ifndef CBLAM_MAP
#define CBLAM_MAP

#include <coffee/core/coffee_types.h>

namespace Coffee{

namespace CBlam{
namespace Dimeter{

/*!
 * \brief Possible Blam versions which we may encounter. The different formats work in different ways. For instance, Xbox stores bitmaps and other data in a single file while PC spreads it across "bitmaps.map" and "sounds.map". Both ways are necessary for parsing to happen correctly.
 */
enum blam_versions
{
    blam_version_h2     = 8, /*!< The Halo 2 map version*/
};

const static int32 blam_num_map_names = 0; /*!< Number of recognizable map names*/
const static struct mapnames { byte inname[32]; byte outname[32];} blam_map_names[0] = {
}; /*!< A mapping of map names which this library can recognize. These are the stock maps.*/

/*!
 * \brief Halo 2 header, credits to Assembly project for offsets
 */
struct blam_file_header_h2
{
    byte    header[4];
    uint32  version;
    uint32  filesize;
    uint32  unknown2;
    uint32  meta_offset;
    uint32  tagdata_offset;
    int32   tagdata_size;
    int32   meta_size;
    uint32  meta_offset_mask;
    byte    reserved1[264];
    byte    buildDate[32];
    int32   maptype;
    byte    unknown3[28];
    int32   strings_block_offset;
    int32   strings_table_count;
    int32   strings_table_size;
    uint32  strings_indextable_offset;
    uint32  strings_table_offset;
    byte    unknown4[36];
    byte    name[36];
    byte    scenario[260];
    uint32  file_table_count;
    uint32  file_table_offset;
    int32   file_table_size;
    uint32  file_indextable_offset;
    uint32  unknown5[3];
    uint32  raw_table_offset;
    int32   raw_table_size;
    uint32  map_checksum;
    uint32  unknown6;

    byte    reserved2[1284];

    byte    footer[4];
};

/*!
 * \brief Get the proper, full name of a map
 * \param map Map from which we identify a string
 * \return A null-terminated string with a map name or empty (empty if map name is not recognized)
 */
extern cstring blam_file_header_full_mapname(const blam_file_header_h2* map);

}
}
}

#endif //CBLAM_MAP
