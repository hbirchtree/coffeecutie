#include <coffee/blam/cblam.h>
#include <coffee/core/base/cfiles.h>
#include <coffee/core/plat/application_start.h>
#include <coffee/core/plat/argument_parse.h>

/*!
 * \brief This example employs the COFFEE_APPLICATION_MAIN macro to redirect the main function.
 *   The purpose of this is platform abstraction such that the rest of the code works consistently across platforms.
 */

using namespace Coffee;
using namespace CResources;
using namespace CBlam;

int coffee_main(int32 argv,byte** argc)
{
    cstring mapstring = coffee_args_get_arg(argv,argc,"halomap");
    CResource mapfile(mapstring);
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

COFFEE_APPLICATION_MAIN(coffee_main)
