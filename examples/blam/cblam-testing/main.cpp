#include <coffee/blam/cblam.h>
#include <coffee/blam/volta/blam_stl.h>
#include <coffee/core/CApplication>
#include <coffee/core/CFiles>
#include <coffee/core/base/renderer/eventapplication.h>
#include <coffee/core/stl_types.h>
#include <coffee/graphics/apis/CGLeamRHI>
#include <coffee/image/cimage.h>
#include <coffee/interfaces/full_launcher.h>
#include <coffee/strings/libc_types.h>
#include <coffee/strings/vector_types.h>
#include <coffee/windowing/renderer/renderer.h>
#include <peripherals/stl/string_ops.h>

#include <coffee/core/CDebug>

using namespace Coffee;
using namespace Blam;

struct Empty
{
};

/*!
 * \brief This example employs the COFFEE_APPLICATION_MAIN macro to redirect the
 * main function. The purpose of this is platform abstraction such that the rest
 * of the code works consistently across platforms.
 */
int coffee_main(i32, cstring_w*)
{
    auto mapfile  = "bloodgulch.map"_rsc;
    auto bitmfile = "bitmaps.map"_rsc;

    blam::map_container map(mapfile, blam::version_t::pc);

    cDebug(
        "Opening map: {0}, build {1}",
        C_CAST<cstring>(map),
        map.map->buildDate.str());

    cDebug(
        "Map mapped to region: {0}-{1}",
        mapfile.data,
        C_RCAST<byte_t*>(mapfile.data) + mapfile.size);

    blam::tag_index_view index_view(map);

    {
        u32 last_tag = 0;
        for(auto tag : index_view)
        {
            cDebug(
                "Tag: {0} greater? {1}", tag->tag_id, tag->tag_id > last_tag);
            last_tag = tag->tag_id;
        }
    }

    {
        /* Extracting scenario data */
        const blam::scn::scenario* scn = blam::scn::get(index_view);

        auto sbsp = scn->bsp_info.data(map.magic);
        for(auto const& s_bsp : sbsp)
        {
            cDebug("BSP: {0}", map.get_name(&s_bsp.tag));

            auto const& bsp_data = *C_RCAST<blam::scn::bsp::section const*>(
                C_RCAST<byte_t const*>(map.map) + s_bsp.offset);

            cDebug(
                "BSP offset: {0} -> {1}",
                bsp_data.header_offset,
                bsp_data.tag.str());

            auto bsp_offset =
                bsp_data.header_offset - (s_bsp.magic - s_bsp.offset);

            auto const& bsp_header = *C_RCAST<blam::scn::bsp::header const*>(
                C_RCAST<byte_t const*>(map.map) + (bsp_offset));

            cDebug("Lightmap: {0}", bsp_header.lightmaps.tag.str());
        }
    }

    auto scenario_header = blam::scn::get(index_view);
    cDebug("Map scenario name: {0}", scenario_header);

    for(auto const& child_scn :
        scenario_header->child_scenarios.data(map.magic))
    {
        cDebug("Scenario name: {0}", map.get_name(&child_scn));
    }

    for(auto const& starting_profile :
        scenario_header->starting_equipment.data(map.magic))
    {
        for(auto it : starting_profile.items)
        {
            cDebug("Starting equipment: {0} -> {1}", it.tag.str(), it.tag_id);
            auto tag_it = index_view.find(it.tag_id);
            if(tag_it != index_view.end())
                cDebug(" - Name: {0}", map.get_name(*tag_it));
            else
                cDebug(" - Name: not found");
        }
    }

    auto skybox = scenario_header->skyboxes.data(map.magic);

    cDebug(
        "Skybox: {0} ({1}) -> {2}",
        map.get_name(skybox.data),
        C_CAST<int>(skybox[0].tag_class),
        skybox[0].tag_id);

    auto skybox_tag = index_view.find(skybox[0].tag_id);

    if(skybox_tag != index_view.end())
        cDebug("Skybox tag: {0}", (*skybox_tag)->tag_id, skybox[0].tag_id);
    else
        cDebug("Failed to find tag");

    for(auto tag : index_view)
    {
        if(tag->tag_id == skybox[0].tag_id)
            cDebug(
                "Matching reference: {0} -> {1}",
                map.get_name(tag),
                tag->tagclass[0].str());
    }

    for(auto const& encounter : scenario_header->encounters.data(map.magic))
    {
        cDebug("Encounter: {0}", encounter.text.str());
        for(auto const& squad : encounter.squads.data(map.magic))
        {
            cDebug(" - name:{0}", squad.name.str());
            cDebug("   - actor type:{0}", squad.ActorType.data);
            cDebug("   - init state:{0}", C_CAST<i16>(squad.InitialState));
            cDebug("   - return state:{0}", C_CAST<i16>(squad.ReturnState));
        }

        for(auto const& loc : encounter.playerStartLocations.data(map.magic))
        {
            cDebug(" - location: {0}", loc.pos);
        }
    }

    //    cDebug(
    //        "Predicate test: {0} -> {1}", scenario->tagId,
    //        map.get_name(scenario));

    Vector<blam::index_item_t const*> texture_vec;

    auto it = index_view.begin();
    while(it != index_view.end())
    {
        it =
            std::find_if(it, index_view.end(), [](blam::index_item_t const* e) {
                return e->matches(blam::tag_class_t::bitm);
            });

        if(it == index_view.end())
            break;

        texture_vec.push_back(*it);
        ++it;
    }

    return 0;

    for(auto texture : texture_vec)
    {
        cDebug(
            "Texture: {0} -> {1}",
            map.get_name(texture),
            texture->matches(blam::tag_class_t::bitm));
    }

    for(auto e : index_view)
    {
        cDebug("Values: {0}", map.get_name(e));
    }

    return 0;
}

COFFEE_APPLICATION_MAIN(coffee_main)
