#include <coffee/CBlam>
#include <coffee/blam/volta/blam_stl.h>
#include <coffee/core/CFiles>
#include <coffee/core/CDebug>
#include <coffee/core/CApplication>

using namespace Coffee;
using namespace Blam;

struct texture_data_t
{
    bitm_texture_t tex;
    const index_item_t* tag;
    const bitm_image_t* img;
    cstring tag_name;
};

/*!
 * \brief This example employs the COFFEE_APPLICATION_MAIN macro to redirect the main function.
 *   The purpose of this is platform abstraction such that the rest of the code works consistently across platforms.
 */
int coffee_main(int32,cstring_w*)
{
    CResources::FileResourcePrefix("sample_data/cblam_data/");

//    cstring mapstring = GetArgument(argv,argc,"halomap");
    cstring mapstring = "bloodgulch.map";
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
    map_container map(mapfile.data, version_t::pc);

    cDebug("Opening map: {0}, build {1}", C_CAST<cstring>(map), map.map->buildDate);

    tag_index_view index_view(map);

    {
        /* Extracting scenario data */
        const scenario* scn = scn_get(index_view);

        const scn_bsp_header* sbsp = scn->struct_bsp.data(map.map, map.tags.index_magic);
        for(int i=0;i<scn->struct_bsp.count;i++)
        {
            const scn_bsp_header* s_bsp = &sbsp[i];
            //        const blam_scn_chunk* bsp =
            //                (const blam_scn_chunk*)
            //                blam_mptr(map,0,s_bsp->offset);
            cDebug("BSP: {0}",(cstring)blam_mptr(map.map,map.tags.index_magic,
                                                 s_bsp->name_ptr));
        }
    }

    auto debigend = [](cstring src, uint32 len)
    {
        return StrUtil::reverse(StrUtil::encapsulate(src, len));
    };
    auto pred = [&](index_item_t const* e)
    {
        return debigend(e->tagclass[0], 4) == "scnr";
    };

    auto scenario = *std::find_if(index_view.begin(), index_view.end(), pred);

    cDebug("Predicate test: {0} -> {1}", scenario->tagId, map.get_name(scenario));

    Vector<index_item_t const*> texture_vec;
    auto aggregate_func = [](
            Vector<index_item_t const*>& v,
            tag_class_t c, index_item_t const* e)
    {
        if(e->tagclass_e[0] == c)
            v.push_back(e);
    };
    auto aggregator = std::bind(aggregate_func, texture_vec, tag_class_t::bitm, std::placeholders::_1);

    auto textures = std::for_each(index_view.begin(), index_view.end(), aggregator);

    for(auto e : texture_vec)
        cDebug("Texture: {0} -> {1}", e->tagId, map.get_name(e));

    return 0;

    for(auto e : index_view)
    {
        cDebug("Values: {0}", map.get_name(e));
    }

//    {
//        /* Extracting texture data */
//        texture_data_t img;
//        const index_item_t* it = nullptr;
//        while(it = fun(tags, map) /*fun_filter(fun, tags, map, bitmfile, img)*/)
//        {
//            cDebug("Index item: {0} -> {1}, {2}, {3}",
//                   it->tagId,
//                   debigend(it->tagclass[0], 4),
//                    (it->tagclass[1][0] != -1) ? debigend(it->tagclass[1], 4) : CString(),
//                    blam_index_item_get_string(it, map, &tags));
////            cDebug("Texture: {0},res={1},mip={2}",img.tag_name,img.tex.resolution,img.tex.mipmaps);
//        }
//    }

    FileUnmap(bitmfile);
    FileUnmap(mapfile);

    return 0;
}

COFFEE_APPLICATION_MAIN(coffee_main)
