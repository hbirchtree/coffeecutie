#include <coffee/CBlam>
#include <coffee/core/CFiles>
#include <coffee/core/CDebug>
#include <coffee/core/CApplication>

using namespace Coffee;
using namespace CResources;
using namespace CBlam;

/*!
 * \brief This example employs the COFFEE_APPLICATION_MAIN macro to redirect the main function.
 *   The purpose of this is platform abstraction such that the rest of the code works consistently across platforms.
 */
int coffee_main(int32 argv,cstring_w* argc)
{
    CResources::FileResourcePrefix("sample_data/");

//    cstring mapstring = GetArgument(argv,argc,"halomap");
    cstring mapstring = "cblam_data/bloodgulch.map";
    if(!mapstring)
        return 1;
    Resource mapfile(mapstring);
    if(!FileExists(mapfile))
        return 2;
    Resource bitmfile("cblam_data/bitmaps.map");
    FileMap(bitmfile);
    if(!FileMap(mapfile))
        return 3;
    const file_header_t* map =
            blam_file_header_get(mapfile.data,version_t::pc);
    tag_index_t tags = blam_tag_index_get(map);

    const blam_scenario* scn = blam_scn_get(map,&tags);

    const blam_scn_bsp_header* sbsp = scn->struct_bsp.data(map,tags.index_magic);
    for(int i=0;i<scn->struct_bsp.count;i++)
    {
        const blam_scn_bsp_header* s_bsp = &sbsp[i];
//        const blam_scn_chunk* bsp =
//                (const blam_scn_chunk*)
//                blam_mptr(map,0,s_bsp->offset);
        cDebug("Name of BSP: {0}",(cstring)blam_mptr(map,tags.index_magic,
                                                    s_bsp->name_ptr));
    }

    return 0;
}

COFFEE_APPLICATION_MAIN(coffee_main)
