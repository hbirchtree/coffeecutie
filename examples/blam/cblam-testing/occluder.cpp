#define GLM_FORCE_SWIZZLE 1

#include "occluder.h"

#include "caching.h"
#include "data.h"
#include "selected_version.h"

#include <coffee/core/debug/formatting.h>

template<typename V>
using OccluderManifest = compo::SubsystemManifest<
    type_list_t<BspReference, Model>,
    type_list_t<BSPCache<V>, BlamCamera, BlamResources>,
    empty_list_t>;

template<typename V>
struct Occluder : compo::RestrictedSubsystem<Occluder<V>, OccluderManifest<V>>
{
    using type  = Occluder<V>;
    using Proxy = compo::proxy_of<OccluderManifest<V>>;

    void start_restricted(Proxy& p, time_point const&)
    {

        BSPCache<V>* bsp_cache;
        BlamCamera*  camera;
        BlamResources* resources;
        p.subsystem(bsp_cache);
        p.subsystem(camera);
        p.subsystem(resources);

        auto camera_pos = -camera->camera.position;

        for(auto& ent : p.select(ObjectBsp))
        {
            auto          ref     = p.template ref<Proxy>(ent);
            BspReference& bsp_ref = ref.template get<BspReference>();

        }

        return;

        Span<Vecf3> portal_colors = resources->debug_line_colors->map<Vecf3>(0);
        Span<Vecf3> portal_pos = resources->debug_lines->map<Vecf3>(72, 72);

        portal_pos[0] = camera_pos;
        portal_pos[1] = camera_pos + Vecf3{5, 0, 5};
        portal_pos[2] = camera_pos;
        portal_pos[3] = camera_pos + Vecf3{0, 5, 5};
        portal_pos[4] = camera_pos;
        portal_pos[5] = camera_pos + Vecf3{5, 5, 0};

        resources->debug_lines->unmap();

        BSPItem const* current_bsp{nullptr};
//        u32            current_cluster{0};

        for(auto& ent : p.select(ObjectBsp))
        {
            auto          ref     = p.template ref<Proxy>(ent);
            BspReference& bsp_ref = ref.template get<BspReference>();

            BSPItem const& bsp = bsp_cache->find(bsp_ref.bsp)->second;
            for(auto const& cluster : bsp.clusters)
                for(auto const& sub : cluster.sub)
                    portal_colors[sub.debug_color_idx] = Vecf3(1, 0, 0);

            if(auto cluster = bsp.find_cluster(camera_pos);
               cluster.has_value())
            {
                auto [cluster_, sub_] = cluster.value();
                current_bsp           = &bsp;
//                current_cluster       = cluster_;
                bsp_ref.visible       = true;
                auto const& sub       = bsp.clusters.at(cluster_).sub.at(sub_);
                portal_colors[sub.debug_color_idx] = Vecf3(0, 1, 0);
                //                cDebug(
                //                    "Bounding box: {}+{} -> {}",
                //                    bounds.origin,
                //                    bounds.extents,
                //                    camera->camera.position);
            } else
            {
                bsp_ref.visible = false;
            }
        }

        resources->debug_line_colors->unmap();

        if(!current_bsp)
            return;

        for(auto& ent : p.select(PositioningStatic))
        {
            auto   ref      = p.template ref<Proxy>(ent);
            Model& model    = ref.template get<Model>();
            if(auto cluster = current_bsp->find_cluster(-model.position);
               !cluster.has_value())
                model.visible = false;
            else
//                model.visible = cluster.value().first == current_cluster;
                model.visible = true;
        }
    }
    void end_restricted(Proxy&, time_point const&)
    {
    }
};

void alloc_occluder(compo::EntityContainer& container)
{
    container.register_subsystem_inplace<Occluder<halo_version>>();
}
