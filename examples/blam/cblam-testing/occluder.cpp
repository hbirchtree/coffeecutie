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
        BSPCache<V>*   bsp_cache;
        BlamCamera*    camera;
        BlamResources* resources;
        p.subsystem(bsp_cache);
        p.subsystem(camera);
        p.subsystem(resources);

        auto camera_pos = -camera->player(0).camera.position;

        // for(auto& ent : p.select(ObjectBsp))
        // {
        //     auto          ref     = p.template ref<Proxy>(ent);
        //     BspReference& bsp_ref = ref.template get<BspReference>();
        // }

        Span<Vecf3> portal_colors = resources->debug_line_colors->map<Vecf3>(0);
        Span<Vecf3> portal_pos    = resources->debug_lines->map<Vecf3>(
            sizeof(Vecf3) * 6, sizeof(Vecf3) * (18 + 16 * 7));

        // portal_pos[0] = camera_pos;
        // portal_pos[1] = camera_pos + Vecf3{5, 0, 5};
        // portal_pos[2] = camera_pos;
        // portal_pos[3] = camera_pos + Vecf3{0, 5, 5};
        // portal_pos[4] = camera_pos;
        // portal_pos[5] = camera_pos + Vecf3{5, 5, 0};

        // for(auto i : range<u32>(3))
        //     portal_colors[3 + i] = Vecf3{1, 0, 1};

        u32 player_i = 0;
        for(auto const& player : camera->viewports)
        {
            auto pos = player.camera.position * Vecf3(-1);

            if(camera->focused_player == player_i)
                pos = Vecf3(0);

            std::array<Vecf3, 7> points = {{
                pos + Vecf3{-0.2f, 0, 0},
                pos + Vecf3{-0.1f, 0, .1f},
                pos + Vecf3{0.1f, 0, .1f},
                pos + Vecf3{0.2f, 0, 0},
                pos + Vecf3{0.1f, 0, -.1f},
                pos + Vecf3{-0.1f, 0, -.1f},
                pos + Vecf3{-0.2f, 0, 0},
            }};

            portal_colors[6 + player_i] = Vecf3{.5f, 0, 1.f};
            std::copy(
                points.begin(),
                points.end(),
                portal_pos.begin() + 18 + 7 * player_i++);
        }

        resources->debug_lines->unmap();
        resources->debug_line_colors->unmap();

        return;

        BSPItem const* current_bsp{nullptr};
        u32            current_cluster{0};

        for(auto& ent : p.select(ObjectBsp))
        {
            auto          ref     = p.template ref<Proxy>(ent);
            BspReference& bsp_ref = ref.template get<BspReference>();

            BSPItem const& bsp = bsp_cache->find(bsp_ref.bsp)->second;
            for(auto const& cluster : bsp.clusters)
                for(auto const& sub : cluster.sub)
                    portal_colors[sub.debug_color_idx] = Vecf3(1, 0, 0);

            if(auto cluster = bsp.find_cluster(camera_pos); cluster.has_value())
            {
                auto [cluster_, sub_] = cluster.value();
                current_bsp           = &bsp;
                current_cluster       = cluster_;
                bsp_ref.visible       = true;
                auto const& sub       = bsp.clusters.at(cluster_).sub.at(sub_);
                portal_colors[sub.debug_color_idx] = Vecf3(0, 1, 0);
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
            auto   ref   = p.template ref<Proxy>(ent);
            Model& model = ref.template get<Model>();
            if(auto cluster = current_bsp->find_cluster(-model.position);
               !cluster.has_value())
                model.visible = false;
            else
                model.visible = cluster.value().first == current_cluster;
            // model.visible = true;
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
