#pragma once

#include "caching.h"
#include "components.h"
#include "data.h"
#include "map_marker.h"
#include <blam/volta/blam_collision.h>

constexpr auto model_lod = blam::mod2::lod_high_ext;

void alloc_resource_loader(compo::EntityContainer& e);

template<typename Version>
void preload_player_bipeds(EntityContainer& e, MapChangedEvent<Version>& data)
{
    blam::tag_index_view tag_index(data.container);
    ModelCache<Version>* model_cache;
    e.subsystem(model_cache);

    typename decltype(tag_index)::iterator bipeds[2] = {
        tag_index.find("characters\\cyborg_mp\\cyborg_mp"),
        tag_index.find("characters\\cyborg\\cyborg")};
    for(u32 i = 0; i < 2; ++i)
    {
        auto it = bipeds[i];
        if(it == tag_index.end())
            continue;
        blam::scn::biped const* biped =
            (*it).template data<blam::scn::biped>(data.container.magic);
        model_cache->predict_regions(biped->model);
    }
}

/* Debug visualisation of model collision geometry (coll tags).
 *
 * For each loaded vehicle, resolve its object->collider coll tag, walk the
 * per-node collision BSPs, and emit each collision surface as a wireframe
 * line-strip loop through the debug-line system. Vertices are bone-local,
 * so each node's geometry is placed by world = model.transform *
 * world_bind[bone]; the bind transform is recovered from the mod2 skeleton
 * (world_bind = inverse(inv_bind)), matched to the coll node by bone name.
 *
 * This is bind-pose only — animated bone transforms (and feeding the
 * per-node meshes into Bullet as a btCompoundShape) come later. Drawn
 * under RenderingParameters::debug_markers. Runs after
 * load_scenario_scenery so the Model entities and their ModelItems exist;
 * it remaps the debug buffer and continues the marker cursor past the
 * triggers/markers already written by load_scenario_bsp. */
template<typename Version>
void load_collision_debug(EntityContainer& e, MapChangedEvent<Version>& data)
{
    ProfContext _(__FUNCTION__);
    using namespace compo;

    auto                 magic = data.container.magic;
    blam::tag_index_view index(data.container);

    auto&         model_cache = e.subsystem_cast<ModelCache<Version>>();
    DebugMarkers& dm          = e.subsystem_cast<DebugMarkers>();

    /* Continue the cursor from where load_scenario_bsp left it (do not
     * reset — that region holds the trigger/marker geometry). */
    dm.map();

    EntityRecipe coll_marker;
    coll_marker.tags       = ObjectGC;
    coll_marker.components = {type_hash_v<DebugDraw>()};

    u32 vehicle_count = 0, surface_count = 0;

    /* Collect Model entity ids first: create_entity below mutates the
     * entity registry and would invalidate a live select<Model> query. */
    std::vector<u64> model_ids;
    for(auto ent : e.select<Model>())
        model_ids.push_back(ent.id());

    for(u64 id : model_ids)
    {
        Model* model = e.get<Model>(id);
        if(!model || !model->origin_object)
            continue;

        auto obj =
            model->origin_object->template data<blam::scn::object>(magic);
        if(obj.has_error())
            continue;
        if(obj.value()->type != blam::scn::object_type::vehicle)
            continue;

        blam::tagref_t const& coll_ref = obj.value()->collider;
        if(!coll_ref.valid())
            continue;
        auto coll_it = index.find(coll_ref);
        if(coll_it == index.end())
            continue;
        auto coll_data = (*coll_it).template data<blam::coll::header>(magic);
        if(coll_data.has_error())
            continue;
        auto const* coll = coll_data.value();

        /* Bind-pose bone transforms (coll nodes mirror the mod2 skeleton
         * by name, not necessarily by index). */
        ModelItem<Version>&          mitem = model_cache.get(model->model);
        Span<blam::mod2::bone const> bones{};
        if(mitem.header)
            if(auto b = mitem.header->bones.data(magic); b.has_value())
                bones = b.value();

        auto nodes_opt = coll->nodes.data(magic);
        if(nodes_opt.has_error())
            continue;

        bool any = false;
        for(auto const& node : nodes_opt.value())
        {
            Matf4 world_bind(1);
            for(u32 i = 0; i < bones.size() && i < mitem.inv_bind.size(); i++)
                if(bones[i].name.str() == node.name.str())
                {
                    world_bind = glm::inverse(mitem.inv_bind[i]);
                    break;
                }
            Matf4 world = model->transform * world_bind;

            auto bsps_opt = node.bsps.data(magic);
            if(bsps_opt.has_error())
                continue;
            for(auto const& bsp : bsps_opt.value())
            {
                auto verts_opt = bsp.vertices.data(magic);
                auto edges_opt = bsp.edges.data(magic);
                auto surfs_opt = bsp.surfaces.data(magic);
                if(verts_opt.has_error() || edges_opt.has_error() ||
                   surfs_opt.has_error())
                    continue;
                auto verts = verts_opt.value();
                auto edges = edges_opt.value();
                auto surfs = surfs_opt.value();

                std::vector<Vecf3> loop;
                for(u32 si = 0; si < surfs.size(); si++)
                {
                    loop.clear();
                    i32 const first = surfs[si].first_edge;
                    i32       ed    = first;
                    u32       guard = 0;
                    do
                    {
                        if(ed < 0 || static_cast<u32>(ed) >= edges.size())
                            break;
                        auto const& edge = edges[ed];
                        u32         vi;
                        if(edge.left_surface == static_cast<i32>(si))
                        {
                            vi = static_cast<u32>(edge.start_vertex);
                            ed = edge.forward_edge;
                        } else
                        {
                            vi = static_cast<u32>(edge.end_vertex);
                            ed = edge.reverse_edge;
                        }
                        if(vi < verts.size())
                            loop.push_back(
                                Vecf3(world * Vecf4(verts[vi].point, 1.f)));
                    } while(ed != first && ++guard < 32);

                    if(loop.size() >= 2)
                    {
                        loop.push_back(loop.front()); /* close the loop */
                        auto mk = e.create_entity(coll_marker);
                        mk.get<DebugDraw>() =
                            dm.create_loop(loop, Vecf3{0.f, 1.f, 0.2f});
                        surface_count++;
                    }
                }
                any = true;
            }
        }
        if(any)
            vehicle_count++;
    }

    dm.unmap();
    cDebug(
        "load_collision_debug: {} vehicles, {} collision surfaces",
        vehicle_count,
        surface_count);
}
