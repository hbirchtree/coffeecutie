#define GLM_FORCE_SWIZZLE 1

#include "occluder.h"

#include "caching.h"
#include "data.h"
#include "selected_version.h"

#include <coffee/core/debug/formatting.h>

template<typename V>
using OccluderManifest = compo::SubsystemManifest<
    type_list_t<BspReference, Model, PlayerCamera, PlayerInfo>,
    type_list_t<BSPCache<V>, BlamResources, RenderingParameters>,
    empty_list_t>;

template<typename V>
struct Occluder : compo::RestrictedSubsystem<Occluder<V>, OccluderManifest<V>>
{
    using type  = Occluder<V>;
    using Proxy = compo::proxy_of<OccluderManifest<V>>;

    u32            last_cluster{std::numeric_limits<u32>::max()};
    bool           last_found{false};
    i16            last_sky_idx{-1};
    u32            frame_counter{0};
    Vecf3          last_camera_pos{};
    BSPItem const* pvs_bsp{nullptr};
    u32            pvs_cluster{0};
    generation_idx_t
        pvs_bsp_id{}; /* which BSP section the camera is currently in */
    std::vector<bool> pvs_visible{}; /* portal-traversal visible set, recomputed
                                        every frame */

    void start_restricted(Proxy& p, time_point const&)
    {
        if constexpr(compile_info::platform::is_android)
            return;

        BSPCache<V>*         bsp_cache;
        BlamResources*       resources;
        RenderingParameters* rendering;
        p.subsystem(bsp_cache);
        p.subsystem(resources);
        p.subsystem(rendering);

        if(!rendering->occluder_update)
            return;

        Vecf3 camera_pos{};
        Matf4 camera_mvp = glm::identity<Matf4>();
        for(auto& ent : p.template select<PlayerCamera>())
        {
            auto* info = p.template get<PlayerInfo>(ent.id);
            auto* cam  = p.template get<PlayerCamera>(ent.id);
            if(info && cam && info->seat_idx == 0)
            {
                camera_pos = cam->camera->position;
                camera_mvp = cam->matrix;
                break;
            }
        }

        auto to_bsp_space = [](Vecf3 const& p) -> Vecf3 { return p; };

        Span<Vecf3> portal_colors = resources->debug_line_colors->map<Vecf3>(0);
        Span<Vecf3> portal_pos    = resources->debug_lines->map<Vecf3>(
            sizeof(Vecf3) * 6, sizeof(Vecf3) * (18 + 16 * 7));

        u32 player_i = 0;
        for(auto& ent : p.template select<PlayerCamera>())
        {
            auto* cam  = p.template get<PlayerCamera>(ent.id);
            auto* info = p.template get<PlayerInfo>(ent.id);
            if(!cam || !info)
                continue;
            if(player_i >= 16)
                break;

            auto pos = cam->camera->position;

            if(info->seat_idx == 0)
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

        BSPItem const*   current_bsp{nullptr};
        u32              current_cluster{0};
        generation_idx_t current_bsp_id{};

        /* Teleports (dummy-plug camera events, debug warps) cross no trigger
         * volume; on a large discontinuity re-resolve the section from the
         * camera position. Continuous movement — including noclip through
         * rock — keeps trigger-only semantics. */
        f32 camera_jump = glm::distance(camera_pos, last_camera_pos);
        last_camera_pos = camera_pos;
        if(camera_jump > 5.f)
        {
            for(auto& [id, item] : bsp_cache->m_cache)
                if(item.valid() &&
                   item.find_cluster_tree(camera_pos).has_value())
                {
                    if(item.section_idx != bsp_cache->active_section)
                        cDebug(
                            "BSP teleport: section {} → {}",
                            bsp_cache->active_section,
                            item.section_idx);
                    bsp_cache->active_section = item.section_idx;
                    break;
                }
        }

        /* Structure BSP switching: the active section changes only when the
         * camera crosses a bsp-switch trigger volume whose source is the
         * active section — flying into another section's space without
         * crossing one keeps that section hidden, exactly like the original
         * engine. Everything below (cluster lookup, PVS, culling) is
         * restricted to the active section, so inactive sections cost
         * nothing per frame. */
        for(auto const& sw : bsp_cache->bsp_switches)
        {
            if(sw.source != bsp_cache->active_section)
                continue;
            if(!sw.volume->contains(camera_pos))
                continue;
            cDebug(
                "BSP switch: section {} → {} ('{}')",
                sw.source,
                sw.destination,
                sw.volume->name.str());
            bsp_cache->active_section = sw.destination;
            break;
        }

        BSPItem const*   active_bsp{nullptr};
        generation_idx_t active_bsp_id{};
        for(auto& [id, item] : bsp_cache->m_cache)
            if(item.valid() && item.section_idx == bsp_cache->active_section)
            {
                active_bsp    = &item;
                active_bsp_id = {id, bsp_cache->generation};
                break;
            }

        if(active_bsp)
        {
            for(auto const& cluster : active_bsp->clusters)
                for(auto const& sub : cluster.sub)
                    portal_colors[sub.debug_color_idx] = Vecf3(1, 0, 0);

            if(auto cluster = active_bsp->find_cluster(camera_pos);
               cluster.has_value())
            {
                auto [cluster_, sub_] = cluster.value();
                current_bsp           = active_bsp;
                current_cluster       = cluster_;
                current_bsp_id        = active_bsp_id;
                auto const& subs      = active_bsp->clusters.at(cluster_).sub;
                if(sub_ < subs.size())
                    portal_colors[subs.at(sub_).debug_color_idx] =
                        Vecf3(0, 1, 0);
            }
        }

        resources->debug_line_colors->unmap();

        /* When the camera enters a new cluster, recompute the portal-traversal
         * visible set. When between clusters, keep the last valid set so
         * culling doesn't snap to all-visible at cluster boundaries. */
        bool section_changed = active_bsp != pvs_bsp;
        bool cluster_changed = section_changed ||
                               (current_bsp != nullptr) != last_found ||
                               current_cluster != last_cluster;
        last_found   = current_bsp != nullptr;
        last_cluster = current_cluster;

        /* The active section is culled even while the camera is outside its
         * clusters; other sections stay hidden wholesale. */
        pvs_bsp    = active_bsp;
        pvs_bsp_id = active_bsp_id;
        if(section_changed)
            pvs_visible.clear(); /* stale per-cluster bits of old section */

        if(current_bsp)
        {
            pvs_cluster = current_cluster;
            pvs_visible = pvs_bsp->portal_visible_set(
                pvs_cluster, camera_pos, camera_mvp);
        }
        /* else: camera outside the active section's clusters (noclip through
         * rock) — keep the last valid set; empty set = all-visible within the
         * active section. Snapping to all-visible across sections is what
         * used to flash far-off geometry into view. */

        rendering->current_bsp_cluster = pvs_cluster;

        if(cluster_changed)
        {
            auto* sound_bus = bsp_cache->sound_bus;
            if(sound_bus)
            {
                BackgroundSoundTransitionEvent
                    trans; /* sound = nullptr by default */
                if(current_bsp)
                {
                    i16 bg_idx = current_bsp->clusters.at(current_cluster)
                                     .cluster->background_sound;
                    if(bg_idx >= 0 &&
                       static_cast<u32>(bg_idx) <
                           current_bsp->bg_sound_palette.size() &&
                       current_bsp->bg_sound_palette[bg_idx])
                    {
                        trans.sound = &static_cast<blam::tagref_t const&>(
                            current_bsp->bg_sound_palette[bg_idx]->bg_sound);
                    }
                }
                SoundEvent ev = {
                    .type      = SoundEvent::background_sound_transition,
                    .entity_id = 0,
                };
                sound_bus->process(ev, &trans);
            }

            i16 sky_idx =
                current_bsp
                    ? current_bsp->clusters.at(current_cluster).cluster->sky
                    : -1;
            if(sky_idx != last_sky_idx)
            {
                last_sky_idx = sky_idx;
                if(sky_idx >= 0 &&
                   static_cast<u32>(sky_idx) < bsp_cache->sky_palette.size() &&
                   bsp_cache->sky_palette[sky_idx])
                {
                    auto const& sky = *bsp_cache->sky_palette[sky_idx];
                    auto        world_data =
                        resources->world_store->map<materials::world_data>(0);
                    world_data[0].fog.indoor_color =
                        Vecf4(sky.indoor_fog.color, sky.indoor_fog.density);
                    world_data[0].fog.indoor_ambient = Vecf4(
                        sky.indoor_ambient.color, sky.indoor_ambient.power);
                    world_data[0].fog.outdoor_color =
                        Vecf4(sky.outdoor_fog.color, sky.outdoor_fog.density);
                    world_data[0].fog.outdoor_ambient = Vecf4(
                        sky.outdoor_ambient.color, sky.outdoor_ambient.power);
                    world_data[0].fog.distances = Vecf4(
                        sky.indoor_fog.start_distance,
                        sky.indoor_fog.opaque_distance,
                        sky.outdoor_fog.start_distance,
                        sky.outdoor_fog.opaque_distance);
                    if(sky.outdoor_fog.opaque_distance < 1)
                        world_data[0].fog.distances.w = 1000.f;
                    if(auto lights = sky.lights.data(bsp_cache->magic))
                        for(auto const& light : lights.value())
                        {
                            Vecf3 dir =
                                glm::mat3_cast(
                                    glm::quat(
                                        Vecf3{
                                            0,
                                            light.radiosity.direction.x,
                                            0}) *
                                    glm::quat(
                                        Vecf3{
                                            0,
                                            0,
                                            light.radiosity.direction.y})) *
                                Vecf3{0, 0, 1};
                            world_data[0].lighting[0].light_direction =
                                Vecf4{dir, light.radiosity.test_distance};
                            world_data[0].lighting[0].light_color = Vecf4{
                                light.radiosity.color, light.radiosity.power};
                        }
                    resources->world_store->unmap();
                }
            }
        }

        BSPItem const* cull_bsp = pvs_bsp;

        /* Returns true if cluster ci is visible from the camera cluster.
         * Falls back to visible when no set is computed yet. */
        const auto cluster_ok = [&](u32 ci) -> bool {
            if(pvs_visible.empty() || ci >= pvs_visible.size())
                return true;
            return pvs_visible[ci];
        };

        bool periodic = (frame_counter++ % 300) == 0;

        Frustum const frustum = Frustum::from_mvp(camera_mvp);

        u32 bsp_visible = 0, bsp_total = 0, bsp_no_cluster = 0;

        /* Cull BSP meshes: two-pass.
         * Pass 1 – cluster PVS (portal-cone traversal).
         * Pass 2 – subcluster AABB frustum test (when subcluster_idx is valid).
         * Meshes without subcluster assignment skip pass 2 and rely on PVS
         * alone. */
        if(cull_bsp)
        {
            for(auto& ent : p.select(ObjectBsp))
            {
                auto          ref     = p.template ref<Proxy>(ent);
                BspReference& bsp_ref = ref.template get<BspReference>();

                bsp_total++;
                if(bsp_ref.bsp == pvs_bsp_id)
                {
                    /* Same BSP section as camera: apply portal + AABB culling.
                     */
                    if(bsp_ref.cluster_idx != std::numeric_limits<u32>::max())
                    {
                        bool pvs_ok = cluster_ok(bsp_ref.cluster_idx);
                        if(pvs_ok &&
                           bsp_ref.subcluster_idx !=
                               std::numeric_limits<u32>::max() &&
                           bsp_ref.cluster_idx < cull_bsp->clusters.size() &&
                           bsp_ref.subcluster_idx <
                               cull_bsp->clusters[bsp_ref.cluster_idx]
                                   .sub.size())
                        {
                            auto const& sub =
                                cull_bsp->clusters[bsp_ref.cluster_idx]
                                    .sub[bsp_ref.subcluster_idx];
                            auto [bmin, bmax] = sub.cluster->bounds.points();
                            bsp_ref.visible = frustum.aabb_visible(bmin, bmax);
                        } else
                        {
                            bsp_ref.visible = pvs_ok;
                        }
                    } else
                    {
                        bsp_ref.visible = true;
                        bsp_no_cluster++;
                    }
                } else
                {
                    /* Different BSP section: hide entirely. */
                    bsp_ref.visible = false;
                }
                if(bsp_ref.visible)
                    bsp_visible++;
            }
        }

        const auto in_draw_distance =
            [&camera_pos, rendering, draw_dist = rendering->draw_distance](
                Model const& mod) {
                return glm::distance(mod.position, camera_pos) < draw_dist;
            };

        /* Cull a model against the camera's BSP section.
         * - In a cluster → portal-traversal set + draw distance.
         * - No cluster but inside this section's world bounds → origin is in
         *   solid space (scenery planted into the ground); keep it, gated by
         *   draw distance.
         * - Outside the section's world bounds → belongs to another BSP
         *   section, which is hidden wholesale, so hide its objects too.
         *   These used to fall through to the distance check alone, which is
         *   why far-away models never disappeared. */
        enum class model_vis
        {
            visible,
            pvs_culled,
            dist_culled,
        };
        const auto classify_model =
            [&](BSPItem const* bsp, Model const& model) -> model_vis {
            auto pos = to_bsp_space(model.position);
            if(auto mc = bsp->find_cluster(pos); mc.has_value())
            {
                if(!cluster_ok(mc->first))
                    return model_vis::pvs_culled;
                return in_draw_distance(model) ? model_vis::visible
                                               : model_vis::dist_culled;
            }
            if(bsp->valid())
            {
                auto [p1, p2] = bsp->mesh->world_bounds.points();
                Vecf3 lo = glm::min(p1, p2), hi = glm::max(p1, p2);
                if(pos.x < lo.x || pos.x > hi.x || pos.y < lo.y ||
                   pos.y > hi.y || pos.z < lo.z || pos.z > hi.z)
                    return model_vis::pvs_culled;
            }
            return in_draw_distance(model) ? model_vis::visible
                                           : model_vis::dist_culled;
        };

        u32 model_visible = 0, model_pvs_culled = 0, model_dist_culled = 0,
            model_total = 0;

        for(auto& ent : p.select(PositioningStatic))
        {
            auto   ref   = p.template ref<Proxy>(ent);
            Model& model = ref.template get<Model>();

            model_total++;
            if(cull_bsp)
            {
                switch(classify_model(cull_bsp, model))
                {
                case model_vis::visible:
                    model.visible = true;
                    model_visible++;
                    break;
                case model_vis::pvs_culled:
                    model.visible = false;
                    model_pvs_culled++;
                    break;
                case model_vis::dist_culled:
                    model.visible = false;
                    model_dist_culled++;
                    break;
                }
            } else
            {
                model.visible = in_draw_distance(model);
                if(model.visible)
                    model_visible++;
                else
                    model_dist_culled++;
            }
        }
        for(auto& ent : p.select(PositioningDynamic))
        {
            auto   ref   = p.template ref<Proxy>(ent);
            Model& model = ref.template get<Model>();
            if(cull_bsp)
                model.visible =
                    classify_model(cull_bsp, model) == model_vis::visible;
            else
                model.visible = in_draw_distance(model);
        }

        if(cluster_changed || periodic)
        {
            u32 total_clusters =
                current_bsp ? static_cast<u32>(current_bsp->clusters.size())
                            : 0;
            if(current_bsp)
            {
                cDebug(
                    "Occluder [frame {}]: cluster {}/{}"
                    " bsp=({:.1f},{:.1f},{:.1f})"
                    " | BSP {}/{} visible ({} no-cluster)"
                    " | models {}/{} visible ({} PVS-culled, {} dist-culled)",
                    frame_counter,
                    current_cluster,
                    total_clusters,
                    camera_pos.x,
                    camera_pos.y,
                    camera_pos.z,
                    bsp_visible,
                    bsp_total,
                    bsp_no_cluster,
                    model_visible,
                    model_total,
                    model_pvs_culled,
                    model_dist_culled);

                /* Print current cluster's subcluster bounds */
                {
                    auto const& cc = current_bsp->clusters.at(current_cluster);
                    u32         si = 0;
                    for(auto const& sub : cc.sub)
                    {
                        auto [bmin, bmax] = sub.cluster->bounds.points();
                        cDebug(
                            "  cluster[{}] sub[{}] bounds:"
                            " ({:.1f},{:.1f},{:.1f})..({:.1f},{:.1f},{:.1f})",
                            current_cluster,
                            si++,
                            bmin.x,
                            bmin.y,
                            bmin.z,
                            bmax.x,
                            bmax.y,
                            bmax.z);
                    }
                }

                /* Show portal-reachable cluster count */
                {
                    u32 reachable = 0;
                    for(bool v : pvs_visible)
                        if(v)
                            reachable++;
                    cDebug(
                        "  portal-reachable clusters: {}/{}",
                        reachable,
                        static_cast<u32>(current_bsp->clusters.size()));
                }

                /* Per-portal frustum debug: show pass/cull for each portal
                 * of the camera cluster, with centroid and front-vertex count.
                 */
                {
                    Frustum frustum = Frustum::from_mvp(camera_mvp);
                    cDebug(
                        "  cam_plane=({:.3f},{:.3f},{:.3f},{:.3f})",
                        frustum.cam_plane.x,
                        frustum.cam_plane.y,
                        frustum.cam_plane.z,
                        frustum.cam_plane.w);
                    auto const& cc = current_bsp->clusters.at(current_cluster);
                    u32         pi = 0;
                    for(auto const& portal : cc.portals)
                    {
                        i32  adj = (portal.data->front_cluster ==
                                   static_cast<i16>(current_cluster))
                                       ? portal.data->back_cluster
                                       : portal.data->front_cluster;
                        bool near =
                            glm::distance(camera_pos, portal.data->centroid) <=
                            portal.data->bound_radius;
                        bool poly = frustum.polygon_inside(portal.vertices);

                        u32 front_count = 0;
                        for(auto const& v : portal.vertices)
                            if(glm::dot(Vecf3(frustum.cam_plane), v) +
                                   frustum.cam_plane.w >
                               0.f)
                                front_count++;

                        cDebug(
                            "  portal[{}]→cluster[{}] "
                            "centroid=({:.1f},{:.1f},{:.1f})"
                            " r={:.1f} verts={} front={}"
                            " near={} poly={} result={}",
                            pi++,
                            adj,
                            portal.data->centroid.x,
                            portal.data->centroid.y,
                            portal.data->centroid.z,
                            portal.data->bound_radius,
                            portal.vertices.size(),
                            front_count,
                            near,
                            poly,
                            (near || poly) ? "PASS" : "CULL");
                    }
                }

                /* Sample first 5 model and BSP centroid positions */
                {
                    u32 sample = 0;
                    for(auto& ent : p.select(PositioningStatic))
                    {
                        if(sample++ >= 5)
                            break;
                        auto   ref   = p.template ref<Proxy>(ent);
                        Model& model = ref.template get<Model>();
                        auto   bsp_p = to_bsp_space(model.position);
                        auto   mc    = current_bsp->find_cluster(bsp_p);
                        cDebug(
                            "  model[{}] scenario=({:.1f},{:.1f},{:.1f})"
                            " bsp=({:.1f},{:.1f},{:.1f}) cluster={} visible={}",
                            sample - 1,
                            model.position.x,
                            model.position.y,
                            model.position.z,
                            bsp_p.x,
                            bsp_p.y,
                            bsp_p.z,
                            mc.has_value() ? std::to_string(mc.value().first)
                                           : std::string("none"),
                            model.visible);
                    }
                }
                {
                    u32 sample = 0;
                    for(auto& ent : p.select(ObjectBsp))
                    {
                        if(sample++ >= 5)
                            break;
                        auto          ref = p.template ref<Proxy>(ent);
                        BspReference& bsp_ref =
                            ref.template get<BspReference>();
                        bool has_cluster = bsp_ref.cluster_idx !=
                                           std::numeric_limits<u32>::max();
                        cDebug(
                            "  bsp[{}] cluster={} visible={}",
                            sample - 1,
                            has_cluster ? std::to_string(bsp_ref.cluster_idx)
                                        : std::string("none"),
                            bsp_ref.visible);
                    }
                }
            } else
            {
                cDebug(
                    "Occluder [frame {}]: camera outside all BSP clusters"
                    " | bsp=({:.1f},{:.1f},{:.1f})",
                    frame_counter,
                    camera_pos.x,
                    camera_pos.y,
                    camera_pos.z);

                /* Print BSP world bounds + first cluster bounds so we can
                 * see the coordinate space the BSP lives in */
                for(auto& ent : p.select(ObjectBsp))
                {
                    auto           ref     = p.template ref<Proxy>(ent);
                    BspReference&  bsp_ref = ref.template get<BspReference>();
                    BSPItem const& bsp = bsp_cache->find(bsp_ref.bsp)->second;
                    if(!bsp.valid())
                        break;
                    auto [wmin, wmax] = bsp.mesh->world_bounds.points();
                    cDebug(
                        "  BSP world_bounds: ({:.1f},{:.1f},{:.1f})"
                        " .. ({:.1f},{:.1f},{:.1f})",
                        wmin.x,
                        wmin.y,
                        wmin.z,
                        wmax.x,
                        wmax.y,
                        wmax.z);
                    u32 ci = 0;
                    for(auto const& cluster : bsp.clusters)
                    {
                        for(auto const& sub : cluster.sub)
                        {
                            auto [bmin, bmax] = sub.cluster->bounds.points();
                            cDebug(
                                "  cluster[{}] subcluster bounds:"
                                " ({:.1f},{:.1f},{:.1f})"
                                " .. ({:.1f},{:.1f},{:.1f})",
                                ci,
                                bmin.x,
                                bmin.y,
                                bmin.z,
                                bmax.x,
                                bmax.y,
                                bmax.z);
                        }
                        if(++ci >= 3)
                            break; /* Only print first few clusters */
                    }
                    break; /* One BSP is enough */
                }
            }
        }
    }

    void end_restricted(Proxy&, time_point const&)
    {
    }
};

void alloc_occluder(compo::EntityContainer& container)
{
    ProfContext _;
    container.register_subsystem_inplace<Occluder<halo_version>>();
}
