#include <coffee/CBlam>
#include <coffee/CCore>

using namespace Coffee;
using namespace CResources;
using namespace CBlam;

/*!
 * \brief This example employs the COFFEE_APPLICATION_MAIN macro to redirect the main function.
 *   The purpose of this is platform abstraction such that the rest of the code works consistently across platforms.
 */
int coffee_main(int32 argv,byte_t** argc)
{
    CResources::coffee_file_set_resource_prefix("sample_data/");

    cstring mapstring = coffee_args_get_arg(argv,argc,"halomap");
    if(!mapstring)
        return 1;
    CResource mapfile(mapstring);
    CResource bitmfile("cblam_data/bitmaps.map");
    coffee_file_memmap(bitmfile);
    coffee_file_memmap(mapfile);
    const blam_file_header* map =
            blam_file_header_get(mapfile.data,blam_version::pc);
    blam_tag_index tags = blam_tag_index_get(map);

    const blam_scenario* scn = blam_scn_get(map,&tags);



    const blam_scn_bsp_header* sbsp = scn->struct_bsp.data(map,tags.index_magic);
    for(int i=0;i<scn->struct_bsp.count;i++)
    {
        const blam_scn_bsp_header* s_bsp = &sbsp[i];
//        const blam_scn_chunk* bsp =
//                (const blam_scn_chunk*)
//                blam_mptr(map,0,s_bsp->offset);
        cDebug("Name of BSP: %s",(cstring)blam_mptr(map,tags.index_magic,s_bsp->name_ptr));
    }

    return 0;
}

COFFEE_APPLICATION_MAIN(coffee_main)
