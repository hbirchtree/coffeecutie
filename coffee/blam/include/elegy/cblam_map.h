#ifndef CBLAM_MAP
#define CBLAM_MAP

#include <coffee/core/CTypes>

namespace Coffee{

namespace CBlam{
namespace Elegy{

/*!
 * \brief Possible Blam versions which we may encounter. The different formats work in different ways. For instance, Xbox stores bitmaps and other data in a single file while PC spreads it across "bitmaps.map" and "sounds.map". Both ways are necessary for parsing to happen correctly.
 */
enum blam_versions
{
};

static cstring const blam_header_head = "head"; /*!< Character string for header of file header, used to determine validity of data*/
static cstring const blam_header_foot = "foot"; /*!< Character string for footer of file header, used to determine validity of data*/

/*!
 * \brief A mockup of the Halo 3 file header format, only speculation out of hex editor
 */
struct blam_file_header_h3
{
    byte_t    header[4];
    int32  version;
    uint32  filesize;
    byte_t    unknown1[4];
    uint32  index_header_addr;
    uint32  index_offset_string_size;
    uint32  virt_size;
    byte_t    reserved1[256];
    byte_t    buildDate[32];
    int16   maptype;
    byte_t    unknown3[24];
    int32   string_table_count;
    int32   string_table_size;
    int32   string_index_table_offset;
    int32   string_table_offset;
    byte_t    unknown4[36];
    byte_t    name[32];
    uint32  unknown6;
    byte_t    scenario[260];
    int32   file_table_count;
    int32   file_table_offset;
    int32   file_table_size;
    int32   file_index_table_offset;
    byte_t    unknown5[36];
    uint32  virt_base_addr;
    uint32  xdk_ver;
    struct {
        uint32  load_addr;
        uint32  size;
    } partitions[6];
};

}
}
}

#endif //CBLAM_MAP
