#include <coffee/blam/cblam.h>
#include <coffee/blam/volta/blam_stl.h>
#include <coffee/core/CApplication>
#include <coffee/core/CFiles>
#include <coffee/core/CProfiling>
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

struct Empty
{
};

template<typename T>
void examine_map(Resource&& mapfile, T version)
{
    ProfContext _(typeid(T).name());

    blam::map_container map(mapfile, version);

    if(map.map->is_xbox() && false)
    {
        ProfContext _("Writing cache to file");
        Resource    cache_out = "map_cached.map"_rsc;

        if(FileOpenMap(
               cache_out,
               map.decompressed_store.size,
               RSCA::ReadWrite | RSCA::Persistent))
        {
            Bytes cache_view = cache_out;

            cache_view.insert(cache_view.begin(), 255);
            cache_view.insert(cache_view.begin() + 10, 255);

            MemCpy(map.decompressed_store, cache_view);
            cDebug("Saved!");
        } else
            cDebug("Sadface");
    }

    //    auto map_name = map.tags->tags(map.map)->to_name();
    //    cDebug("Map name: {0}", map_name.to_string(map.magic));
    //    auto target_offset = 37001189;

    //    auto const& scn_tag = map.tags->scenario(map.map);
    //    auto const& scenario = scn_tag.to_name().to_string(map.magic);
    //    cDebug("Scenario name: {0}", scenario);
    //    auto const& scenario_head =
    //    scn_tag.to_reflexive<blam::scn::scenario>(); auto const& scenario_data
    //    = scenario_head.data(map.magic);

    //    cDebug("Scenario stuff: {0}", scenario_data[0].unk_sky.tag.str());

    //    return;

    cDebug(
        "Opening map: {0}, build {1}",
        C_OCAST<cstring>(map),
        map.map->buildDate.str());

    cDebug(
        "Map mapped to region: {0}-{1}",
        map.map,
        C_RCAST<byte_t const*>(map.map) + map.map->decomp_len);

    blam::tag_index_view index_view(map);

    for(auto tag : index_view)
    {
        cDebug(
            "Tag: {0} {1} {2} -> {3} : {4}",
            tag->tagclass[0].str(),
            tag->tagclass[1].str(),
            tag->tagclass[2].str(),
            tag->tag_id,
            map.get_name(tag));
    }

    struct mem_map
    {
        ptroff  start;
        szptr   size;
        CString type;
        CString label;
    };

    Vector<mem_map> mapping;

    auto add_mem_map = [&](auto ptr, cstring label) {
        byte_t const* base_ptr = C_OCAST<Bytes>(mapfile).data;
        byte_t const* ptr_off  = C_RCAST<byte_t const*>(ptr);

        mapping.push_back(
            {ptr_off - base_ptr,
             sizeof(*ptr),
             platform::env::Stacktracer::DemangleSymbol(typeid(ptr).name()),
             label});
    };

    auto add_mem_map_region = [&](auto ptr, szptr size, cstring label) {
        byte_t const* base_ptr = C_OCAST<Bytes>(mapfile).data;
        byte_t const* ptr_off  = C_RCAST<byte_t const*>(ptr);

        mapping.push_back(
            {ptr_off - base_ptr,
             size,
             platform::env::Stacktracer::DemangleSymbol(typeid(ptr).name()),
             label});
    };

    //    auto vertex_region = map.tags->vertex_objects.data(map.magic);
    //    auto index_region = map.tags->index_objects.data(map.magic);

    //    add_mem_map_region(
    //        vertex_region.data, vertex_region.size, "Vertex segment");
    //    add_mem_map_region(index_region.data, index_region.size, "Index
    //    segment");

    add_mem_map(map.map, "file_header");

    auto tag_base = map.tags->tags(map.map);

    {
        /* Extracting scenario data */
        auto scn = &blam::scn::get_scenario(map.map, map.magic);

        add_mem_map(scn, "scenario");

        auto obj_out     = fopen("map.obj", "w+");
        auto portals_out = fopen("portals.obj", "w+");
        auto scenery_out = fopen("scenery.obj", "w+");

        u32 base_vertex  = 0;
        u32 prt_base_idx = 0;

        auto sbsp = scn->bsp_info.data(map.magic);
        for(auto const& bsp_info : sbsp)
        {
            ProfContext _("BSP traverse");

            add_mem_map(&bsp_info, "bsp::info");
            auto        bsp_magic = bsp_info.bsp_magic(map.magic);
            auto const& bsp_data  = bsp_info.to_bsp(bsp_magic);

            auto some_tag = *index_view.find(bsp_info.tag);
            cDebug("BSP name: {0}", map.get_name(some_tag));

            add_mem_map(&bsp_data, "bsp::section");

            cDebug(
                "BSP: {2} {0} -> {1}",
                bsp_data.header_offset,
                bsp_data.tag.str(),
                map.get_name(&bsp_info.tag));

            auto const& bsp_header = bsp_data.to_header().data(bsp_magic)[0];

            add_mem_map(&bsp_header, "bsp::header");

            //            for(auto const& vertex :
            //            bsp_data.xbox_vertices.data(bsp_magic))
            //                cDebug(
            //                    "Pos: {0} {1} {2}",
            //                    vertex.position.x(),
            //                    vertex.position.y(),
            //                    vertex.position.z());

            auto clusters        = bsp_header.clusters.data(bsp_magic);
            auto cluster_portals = bsp_header.cluster_portals.data(bsp_magic);

            for(auto const& portal : cluster_portals)
            {
                ProfContext _("Cluster portal traverse");

                auto vertices = portal.vertices.data(bsp_magic);
                for(auto const& vert : vertices)
                {
                    fprintf(
                        portals_out,
                        "v %f %f %f\n",
                        vert.x(),
                        vert.y(),
                        vert.z());
                }
                fprintf(portals_out, "l ");
                for(auto i : Range<>(vertices.elements))
                    fprintf(portals_out, "%u ", u32(prt_base_idx + i + 1));
                fprintf(portals_out, "%u\n", prt_base_idx + 1);
                cDebug("Cluster portal: {0}", &portal);

                prt_base_idx += vertices.elements;
            }

            for(auto const& cluster : clusters)
            {
                ProfContext _("Cluster traverse");

                auto portals = cluster.portals.data(bsp_magic);
                auto pre_rsc = cluster.predicted_resources.data(bsp_magic);

                cDebug("Cluster {0} {1}", &cluster, cluster.transition_bsp);
                for(auto const& portal : portals)
                    cDebug(" - Portal {0}", portal);

                for(auto const& rsc : pre_rsc)
                {
                    auto it = index_view.find(rsc.tag_id);

                    if(it == index_view.end())
                        continue;

                    cDebug(
                        " - Resource {0}: {1}",
                        map.get_name(*it),
                        (*it)->tagclass[0].str());
                }
            }

            auto breakables = bsp_header.breakables_surfaces.data(bsp_magic);
            for(auto const& breakable : breakables)
            {
                cDebug("Breakable surface: {0}");
            }

            auto lightmap_tag = index_view.find(bsp_header.lightmaps);
            if(lightmap_tag != index_view.end())
            {
                auto const& lightmap_tagv = *lightmap_tag;
                auto const& bitmap_head =
                    lightmap_tagv->to_reflexive<blam::bitm::header_t>().data(
                        map.magic);
                for(auto const& image : bitmap_head[0].images.data(map.magic))
                {
                    auto img_data = image.data(map.magic);
                    cDebug(
                        "Image: {0}x{1}, {2},{3}",
                        image.isize.w,
                        image.isize.h,
                        C_CAST<u32>(image.format),
                        C_CAST<u32>(image.flags));
                }
                cDebug("Lightmap: {0}", map.get_name(*lightmap_tag));
            }

            for(auto const& mesh_head :
                bsp_header.submesh_groups.data(bsp_magic))
            {
                ProfContext _("Mesh header traverse");

                add_mem_map(&mesh_head, "bsp::lightmap_header");

                cDebug("Submesh: {0}", &mesh_head);

                for(auto const& submesh_head :
                    mesh_head.material.data(bsp_magic))
                {
                    add_mem_map(&submesh_head, "bsp::submesh_header");

                    cDebug(
                        " - Submesh header: {0}",
                        map.get_name(&submesh_head.shader));

                    auto index_data =
                        submesh_head.pc_indices(bsp_header).data(bsp_magic);

                    add_mem_map_region(
                        index_data.data, index_data.size, "index data");

                    u32 current_base_off = 0;

                    if(map.map->is_xbox())
                    {
                        auto vertex_data =
                            submesh_head.xbox_vertices().data(bsp_magic);
                        add_mem_map_region(
                            vertex_data.data, vertex_data.size, "vertex data");
                        for(auto const& vert : vertex_data)
                        {
                            fprintf(
                                obj_out,
                                "v %f %f %f\n",
                                vert.position.x(),
                                vert.position.y(),
                                vert.position.z());
                        }

                        current_base_off = vertex_data.elements;
                    } else
                    {
                        auto vertex_data =
                            submesh_head.pc_vertices().data(bsp_magic);
                        add_mem_map_region(
                            vertex_data.data, vertex_data.size, "vertex data");
                        for(auto const& vert : vertex_data)
                        {
                            fprintf(
                                obj_out,
                                "v %f %f %f\n",
                                vert.position.x(),
                                vert.position.y(),
                                vert.position.z());
                        }

                        current_base_off = vertex_data.elements;
                    }

                    for(auto const& idx : index_data)
                    {
                        fprintf(
                            obj_out,
                            "f %u %u %u\n",
                            u32(idx[0] + 1 + base_vertex),
                            u32(idx[1] + 1 + base_vertex),
                            u32(idx[2] + 1 + base_vertex));
                    }

                    base_vertex += current_base_off;
                }
            }
        }

        fclose(obj_out);
        fclose(portals_out);

        u32 base_scenery = 0;

        auto scenery_tags = scn->scenery.ref.data(map.magic);
        for(auto const& scenery : scn->scenery.base.data(map.magic))
        {
            cDebug(
                "Scenery: {0} ({4}) @ {1},{2},{3} -> {5} : {6}",
                scenery.scenery_id,
                scenery.pos.x(),
                scenery.pos.y(),
                scenery.pos.z(),
                scenery.flag,
                scenery_tags[scenery.scenery_id][0].to_name().to_string(
                    map.magic),
                scenery_tags[scenery.scenery_id][0].tag.str());

            base_scenery++;
            fprintf(
                scenery_out,
                "v %f %f %f\nl %u %u\n",
                scenery.pos.x(),
                scenery.pos.y(),
                scenery.pos.z(),
                base_scenery,
                base_scenery);
        }
        for(auto const& equip : scn->mp_equipment.data(map.magic))
        {
            cDebug("Equipment spawn: {0}", map.get_name(&equip.item));

            auto weapon_tag = index_view.find(equip.item);

            if(weapon_tag != index_view.end())
            {
                auto weapon_ref = (*weapon_tag)
                                      ->to_reflexive<blam::scn::weapon_tagref>()
                                      .data(map.magic);
                auto        weapon_data = weapon_ref[0].data(map.magic);
                auto const& weapon_stuff =
                    *index_view.find(weapon_data[0].something);
                cDebug(
                    "Weapon data: {0} {1}",
                    map.get_name(&weapon_data[0].something),
                    map.get_name(weapon_stuff));
            }

            base_scenery++;
            fprintf(
                scenery_out,
                "v %f %f %f\nl %u %u\n",
                equip.pos.x(),
                equip.pos.y(),
                equip.pos.z(),
                base_scenery,
                base_scenery);
        }
        auto vehicle_tags = scn->vehicles.ref.data(map.magic);
        for(auto const& vehicle : scn->vehicles.base.data(map.magic))
        {
            cDebug(
                "Vehicle: {0} {1}",
                vehicle_tags[vehicle.vehicle_id][0].name.to_string(map.magic),
                vehicle.flag);
        }
        for(auto const& device_group : scn->device_groups.data(map.magic))
        {
            cDebug("Device group");
        }

        for(auto const& ui_string : semantic::mem_chunk<blam::tagref_t>::From(
                &scn->ui_text.custom_object_names, 2))
        {
            cDebug("UI string: {0}", ui_string.name.to_string(map.magic));

            auto tag = index_view.find(ui_string);

            if(tag != index_view.end())
            {
                auto const& data =
                    (*tag)->to_reflexive<blam::ui::unicode_string>().data(
                        map.magic)[0];
                for(auto const& sub_string : data.sub_strings.data(map.magic))
                {
                    auto str = sub_string.str(map.magic);
                    cDebug("Sub string: {0}", CString(str.begin(), str.end()));
                }
                cDebug("Data found");
            }
        }

        cDebug(
            "Hud messages: {0}",
            scn->ui_text.hud_text.to_name().to_string(map.magic));
        auto hud_it = index_view.find(scn->ui_text.hud_text);

        if(hud_it != index_view.end())
        {
            auto tag = *hud_it;

            for(auto const& data :
                (*hud_it)->to_reflexive<blam::ui::hud_message>().data(
                    map.magic))
            {
                for(auto const& symbol : data.symbols.data(map.magic))
                    cDebug("Symbol name: {0}", symbol.symbol.str());

                for(auto i : Range<>(data.offsets.count))
                {
                    auto str =
                        data.text.str(map.magic, data.str_offset(map.magic, i));
                    cDebug("Text: {0}", CString(str.begin(), str.end()));
                }

                auto str = data.text.str(map.magic);
                cDebug(
                    "Hud message data: {0}", CString(str.begin(), str.end()));
            }
        }

        //        for(auto const& scenery : scn->scenery.ref.data(map.magic))
        //            cDebug("Scenery tag: {0}", scenery.tag.str());
        //        for(auto const& obj_name : scn->object_names.data(map.magic))
        //            cDebug("Object names: {0}", obj_name.name.str());
        //        for(auto const& globals : scn->globals.data(map.magic))
        //            cDebug("Global: {1} -> {0}", globals.pos,
        //            globals.name.str());
        //        for(auto const& poi :
        //        scn->cutscene_camera_poi.data(map.magic))
        //            cDebug("POI");
        //        for(auto const& poi : scn->cutscene_titles.data(map.magic))
        //            cDebug("Title");
        //        for(auto const& poi : scn->cutscene_flags.data(map.magic))
        //            cDebug("Flags");
        //        for(auto const& poi : scn->participants.data(map.magic))
        //            cDebug("Parts");

        fclose(scenery_out);
    }

    auto scenario_header = &blam::scn::get_scenario(map.map, map.magic);
    cDebug("Map scenario name: {0}", scenario_header);

    for(auto const& child_scn :
        scenario_header->child_scenarios.data(map.magic))
    {
        add_mem_map(&child_scn, "child_scenario");

        cDebug("Scenario name: {0}", map.get_name(&child_scn));
    }

    for(auto const& starting_profile :
        scenario_header->starting_equipment.data(map.magic))
    {
        add_mem_map(&starting_profile, "starting_profile");

        for(auto it : starting_profile.items)
        {
            auto tag_it = index_view.find(it);
            if(tag_it != index_view.end())
            {
                cDebug(
                    "Starting equipment: {0} -> {1}", it.tag.str(), it.tag_id);
                cDebug(" - Name: {0}", map.get_name(*tag_it));
            }
        }
    }

    auto skybox = scenario_header->skyboxes.data(map.magic);

    cDebug(
        "Skybox: {0} ({1}) -> {2}",
        map.get_name(skybox.data),
        C_CAST<int>(skybox[0].tag_class),
        skybox[0].tag_id);

    auto skybox_tag = index_view.find(skybox[0]);

    if(skybox_tag != index_view.end())
        cDebug("Skybox tag: {0}", (*skybox_tag)->tag_id, skybox[0].tag_id);
    else
        cDebug("Failed to find tag");

    return;

    if(false)
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

            for(auto const& loc :
                encounter.playerStartLocations.data(map.magic))
            {
                cDebug(" - location: {0}", loc.pos);
            }
        }

    std::sort(
        mapping.begin(),
        mapping.end(),
        [](mem_map const& m1, mem_map const& m2) {
            return m1.start < m2.start;
        });

    cBasicPrint("Map file:");
    cBasicPrint("|");

    szptr end_last = 0;
    for(auto const& region : mapping)
    {
        if(end_last > region.start)
            cBasicPrint("|   Overlap of {0} bytes", end_last - region.start);

        end_last = region.start + region.size;

        if(end_last < map.map->decomp_len)
            cBasicPrintNoNL("|-> ");
        else
            cBasicPrintNoNL("X   ");

        cBasicPrint(
            "{0}+{1} --> {2}: {3}",
            region.start,
            region.size,
            region.label,
            region.type);
    }

    return;

    Vector<blam::tag_t const*> texture_vec;

    auto it = index_view.begin();
    while(it != index_view.end())
    {
        it = std::find_if(it, index_view.end(), [](blam::tag_t const* e) {
            return e->matches(blam::tag_class_t::bitm);
        });

        if(it == index_view.end())
            break;

        texture_vec.push_back(*it);
        ++it;
    }

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
}

/*!
 * \brief This example employs the COFFEE_APPLICATION_MAIN macro to redirect the
 * main function. The purpose of this is platform abstraction such that the rest
 * of the code works consistently across platforms.
 */
int coffee_main(i32, cstring_w*)
{
    auto bitmfile = "bitmaps.map"_rsc;

    //    examine_map("c10.map"_rsc, blam::pc_version);
    examine_map("bloodgulch.map"_rsc, blam::pc_version);

    cDebug("{0}", str::transform::multiply('=', 80));

    //    examine_map("c10_xbox.map"_rsc, blam::xbox_version);
    //    examine_map("bloodgulch_xbox.map"_rsc, blam::xbox_version);

    //    examine_map("hyrule.map"_rsc);

    return 0;
}

COFFEE_APPLICATION_MAIN(coffee_main)
