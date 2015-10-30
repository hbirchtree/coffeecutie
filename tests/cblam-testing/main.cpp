#include <coffee/blam/cblam.h>
#include <coffee/core/base/cfiles.h>

using namespace Coffee;
using namespace CResources;
using namespace CBlam;

int main()
{
    CResource mapfile("bloodgulch.map");
    CResource bitmfile("bitmaps.map");
    bitmfile.memory_map();
    mapfile.memory_map();
    const blam_file_header* map =
            blam_file_header_get(mapfile.data,blam_version_pc);
    blam_tag_index tags = blam_tag_index_get(map);

    const blam_index_item* base = blam_tag_index_get_items(map);

    const blam_scenario* scn = blam_scn_get(map,&tags);

    return 0;
}
