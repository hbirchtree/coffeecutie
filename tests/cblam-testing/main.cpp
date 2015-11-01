#include <coffee/blam/cblam.h>
#include <coffee/core/base/cfiles.h>
#include <coffee/core/plat/application_start.h>
#include <coffee/core/plat/argument_parse.h>


using namespace Coffee;
using namespace CResources;
using namespace CBlam;

/*!
 * \brief This example employs the COFFEE_APPLICATION_MAIN macro to redirect the main function.
 *   The purpose of this is platform abstraction such that the rest of the code works consistently across platforms.
 */
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

    const blam_scenario* scn = blam_scn_get(map,&tags);



    const blam_scn_bsp_header* sbsp = scn->struct_bsp.data(map,tags.index_magic);
    for(int i=0;i<scn->struct_bsp.count;i++)
    {
        const blam_scn_bsp_header* s_bsp = &sbsp[i];
        const blam_mod2_bsp_header* bsp =
                (const blam_mod2_bsp_header*)
                blam_mptr(map,tags.index_magic,s_bsp->offset+s_bsp->magic);
        cDebug("Name of BSP: %s",(cstring)blam_mptr(map,tags.index_magic,s_bsp->name_ptr));
    }

    return 0;
}

COFFEE_APPLICATION_MAIN(coffee_main)
