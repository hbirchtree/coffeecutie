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
    auto mapfile  = "b40.map"_rsc;
    auto bitmfile = "bitmaps.map"_rsc;

    blam::map_container map(mapfile, blam::version_t::pc);

    cDebug(
        "Opening map: {0}, build {1}",
        C_CAST<cstring>(map),
        map.map->buildDate.str());

    blam::tag_index_view index_view(map);

    {
        /* Extracting scenario data */
        const blam::scn::scenario* scn = blam::scn::get(index_view);

        auto sbsp = scn->struct_bsp.data(map.map, map.tags.index_magic);
        for(auto const& s_bsp : sbsp)
        {
            cDebug(
                "BSP: {0}",
                blam::magic_ptr<cstring>(
                    map.map, map.tags.index_magic, s_bsp.name_ptr));
        }
    }

    auto scenario_header = blam::scn::get(index_view);
    cDebug("Map scenario name: {0}", scenario_header);

    for(auto const& child_scn :
        scenario_header->child_scenarios.data(map.map, map.tags.index_magic))
    {
        cDebug("Scenario name: {0}", map.get_name(&child_scn));
    }

    auto skybox = scenario_header->skybox.data(map.map, map.tags.index_magic);

    cDebug(
        "Skybox: {0} ({1}) -> {2}",
        map.get_name(skybox.data),
        C_CAST<int>(skybox[0].tag_class),
        skybox[0].tagId);

    for(auto tag : index_view)
    {
        if(tag->tagId == skybox[0].tagId)
            cDebug(
                "Matching reference: {0} -> {1}",
                map.get_name(tag),
                str::encapsulate(tag->tagclass.at(0), 4));
    }

    for(auto const& encounter :
        scenario_header->encounters.data(map.map, map.tags.index_magic))
    {
        cDebug("Encounter: {0}", encounter.text.str());
        for(auto const& squad :
            encounter.squads.data(map.map, map.tags.index_magic))
        {
            cDebug(" - name:{0}", squad.name.str());
            cDebug("   - actor type:{0}", squad.ActorType.data);
            cDebug("   - init state:{0}", C_CAST<i16>(squad.InitialState));
            cDebug("   - return state:{0}", C_CAST<i16>(squad.ReturnState));
        }

        for(auto const& loc :
            encounter.playerStartLocations.data(map.map, map.tags.index_magic))
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
