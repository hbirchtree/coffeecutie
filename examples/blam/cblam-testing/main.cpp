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
    CResources::FileResourcePrefix("sample_data/cblam_data/");

//    cstring mapstring = GetArgument(argv,argc,"halomap");
    cstring mapstring = "b40.map";
    if(!mapstring)
        return 1;
    Resource mapfile(mapstring);
    if(!FileExists(mapfile))
        return 1;

    Resource bitmfile("bitmaps.map");
    if(!FileMap(bitmfile))
        return 1;
    if(!FileMap(mapfile))
        return 1;
    const file_header_t* map =
            blam_file_header_get(mapfile.data,version_t::pc);
    tag_index_t tags = blam_tag_index_get(map);

    {
        /* Extracting scenario data */
        const blam_scenario* scn = blam_scn_get(map,&tags);

        const blam_scn_bsp_header* sbsp = scn->struct_bsp.data(map,tags.index_magic);
        for(int i=0;i<scn->struct_bsp.count;i++)
        {
            const blam_scn_bsp_header* s_bsp = &sbsp[i];
            //        const blam_scn_chunk* bsp =
            //                (const blam_scn_chunk*)
            //                blam_mptr(map,0,s_bsp->offset);
            cDebug("BSP: {0}",(cstring)blam_mptr(map,tags.index_magic,
                                                 s_bsp->name_ptr));
        }
    }

    {
        /* Extracting texture data */
        const index_item_t* base_idx = blam_tag_index_get_items(map);
        const index_item_t* idx;
        bitm_texture_t tex;
        for(int32 i=0;i<tags.tagCount;i++)
        {
            idx = &base_idx[i];
            if(blam_tagref_match_class(idx,0,blam_index_item_type_bitm))
            {
                int num = 0;
                const bitm_image_t* img = bitm_get(idx,map,tags.index_magic,&num);
                cstring t = blam_index_item_get_string(idx,map,&tags);
                tex = bitm_get_texture(img,bitmfile.data);
                cDebug("Texture: {0},res={1},mip={2}",t,tex.resolution,tex.mipmaps);
            }
        }
    }

    FileUnmap(bitmfile);
    FileUnmap(mapfile);

    return 0;
}

COFFEE_APPLICATION_MAIN(coffee_main)
