#define GLM_FORCE_SWIZZLE 1

#include "occluder.h"

#include "caching.h"
#include "data.h"
#include "map_marker.h"
#include "selected_version.h"

#include <coffee/core/CProfiling>
#include <coffee/core/debug/formatting.h>

template<typename V>
using OccluderManifest = compo::SubsystemManifest<
    type_list_t<BspReference, Model, PlayerCamera, PlayerInfo, DebugDraw>,
    type_list_t<
        BSPCache<V>,
        BlamResources,
        RenderingParameters,
        DebugMarkers,
        GameEventBus>,
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
    std::vector<bool> pvs_visible{}; /* portal-traversal visible set,
                                        recomputed on view change */
    Matf4 last_pvs_mvp{};            /* view the set was computed for */
    BSPItem::portal_scratch
        portal_scratch{}; /* reused walk buffers, see caching_item.h */

    struct eye_marker_t
    {
        u64                         entity{0};
        DebugMarkers::strip_slot_t  slot{};
    };
    std::vector<eye_marker_t> eye_pool;

    void start_restricted(Proxy& p, time_point const&)
    {
        BSPCache<V>*         bsp_cache;
        BlamResources*       resources;
        RenderingParameters* rendering;
        DebugMarkers*        markers;
        GameEventBus*        game_bus;
        p.subsystem(bsp_cache);
        p.subsystem(resources);
        p.subsystem(rendering);
        p.subsystem(markers);
        p.subsystem(game_bus);

        if(!rendering->occluder_update)
            return;

        Coffee::ProfContext _("Occluder::frame");

        /* available() is gated on RenderingParameters::debug_markers:
         * mapping the marker buffer stalls on GPU sync every frame, which
         * is pure loss when the markers aren't even drawn (release/mobile
         * default). This was ~99% of occluder frame time. */
        if(markers->available())
        {
            Coffee::ProfContext __("Occluder::debug_viz");
            update_debug_viz(p);
        }

        Coffee::Profiler::PushContext("Occluder::pre_section");

        Vecf3 camera_pos{};
        Matf4 camera_mvp = glm::identity<Matf4>();
        for(auto ent : p.template select<PlayerCamera>())
        {
            auto* info = p.template get<PlayerInfo>(ent.id());
            auto* cam  = p.template get<PlayerCamera>(ent.id());
            if(info && cam && info->seat_idx == 0)
            {
                camera_pos = cam->camera->position;
                camera_mvp = cam->matrix;
                break;
            }
        }


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

        Coffee::Profiler::PopContext(); /* Occluder::pre_section */

        Coffee::Profiler::PushContext("Occluder::section_resolve");
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

            if(auto cluster = active_bsp->find_cluster(camera_pos);
               cluster.has_value())
            {
                auto [cluster_, sub_] = cluster.value();
                current_bsp           = active_bsp;
                current_cluster       = cluster_;
                current_bsp_id        = active_bsp_id;
            }
        }

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

        Coffee::Profiler::PopContext(); /* Occluder::section_resolve */

        /* The visible set depends on the full view (portal screen rects),
         * so it must follow camera motion — but an identical MVP and
         * cluster yields an identical set, so idle frames skip the walk. */
        bool view_changed =
            cluster_changed || camera_mvp != last_pvs_mvp;
        if(current_bsp && view_changed)
        {
            Coffee::ProfContext __("Occluder::portal_visible_set");
            pvs_cluster  = current_cluster;
            pvs_visible  = pvs_bsp->portal_visible_set(
                pvs_cluster, camera_pos, camera_mvp, portal_scratch);
            last_pvs_mvp = camera_mvp;
        } else if(current_bsp)
            pvs_cluster = current_cluster;
        /* else: camera outside the active section's clusters (noclip through
         * rock) — keep the last valid set; empty set = all-visible within the
         * active section. Snapping to all-visible across sections is what
         * used to flash far-off geometry into view. */

        rendering->current_bsp_cluster = pvs_cluster;

        /* Diagnostic (BLAM_OCCLUDER_PROBE=1): raycast a grid of view
         * directions and report each hit surface's cluster and whether the
         * portal walk marked it visible. FALSE rows are either portal-walk
         * under-reach or face→cluster mis-assignment. */
        if(::getenv("BLAM_OCCLUDER_PROBE") && current_bsp &&
           (frame_counter % 60) == 0)
        {
            Matf4 inv = glm::inverse(camera_mvp);
            auto  unproject = [&inv](f32 x, f32 y, f32 z) {
                Vecf4 p = inv * Vecf4{x, y, z, 1.f};
                return Vecf3(p) / p.w;
            };
            cDebug(
                "Occluder probe: camera cluster {} at ({:.1f},{:.1f},{:.1f})",
                pvs_cluster,
                camera_pos.x,
                camera_pos.y,
                camera_pos.z);
            for(f32 ny = -0.8f; ny <= 0.81f; ny += 0.1f)
                for(f32 nx = -0.9f; nx <= 0.91f; nx += 0.1f)
                {
                    /* Reversed-Z with infinite far plane: z=0 is at infinity,
                     * so unproject a mid-depth point and shoot from the eye. */
                    Vecf3 origin = camera_pos;
                    Vecf3 dir =
                        glm::normalize(unproject(nx, ny, 0.5f) - camera_pos);
                    auto hit =
                        current_bsp->raycast(origin, origin + dir * 500.f);
                    if(!hit)
                        continue;
                    Vecf3 hp = origin + dir * (500.f * hit->t);
                    std::optional<u32> hc;
                    for(f32 back : {0.1f, 0.5f, 1.f, 2.f})
                        if((hc = current_bsp->find_cluster_tree(
                                hp - dir * back)))
                            break;
                    if(!hc)
                        continue;
                    bool vis = *hc < pvs_visible.size() ? bool(pvs_visible[*hc])
                                                        : true;
                    if(!vis)
                        cDebug(
                            "  probe ndc=({:+.1f},{:+.1f}) t={:.3f}"
                            " hit=({:.1f},{:.1f},{:.1f}) cluster={} vis={}",
                            nx,
                            ny,
                            hit->t,
                            hp.x,
                            hp.y,
                            hp.z,
                            *hc,
                            vis);
                }
        }

        if(cluster_changed)
        {
            /* Publish the cluster change neutrally; the sound system (and any
             * other interested subsystem) resolves its own concerns from this.
             * The occluder stays free of audio specifics. */
            GameEvent           cev{GameEvent::ClusterChanged};
            ClusterChangedEvent cc{
                .bsp     = current_bsp,
                .cluster = current_cluster,
            };
            game_bus->inject(cev, &cc);

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
         * Pass 1 – cluster PVS (portal traversal): a chunk is PVS-visible when
         *   ANY of its owner clusters is — boundary-straddling chunks carry
         *   their full owner set, so they hide with their neighborhood instead
         *   of staying visible forever (the "checkerboard").
         * Pass 2 – frustum test against the chunk's own AABB (computed from
         *   its vertices at load; tighter than the subcluster AABB and present
         *   on every chunk). */
        if(cull_bsp)
        {
            Coffee::ProfContext __("Occluder::bsp_cull");
            /* Fused select: dense walk over the BspReference container with
             * the payload prefetched — BspReference and the ObjectBsp tag
             * are 1:1 (same recipe), so this visits the same set as
             * select(ObjectBsp) without the tag scan and per-entity
             * container lookups. */
            for(auto ent : p.template select<BspReference>())
            {
                BspReference& bsp_ref = ent.template get<BspReference>();

                bsp_total++;
                if(bsp_ref.bsp == pvs_bsp_id)
                {
                    bool pvs_ok = true;
                    if(!bsp_ref.clusters.empty())
                    {
                        pvs_ok = false;
                        for(u16 ci : bsp_ref.clusters)
                            if(cluster_ok(ci))
                            {
                                pvs_ok = true;
                                break;
                            }
                    } else
                        bsp_no_cluster++;

                    bsp_ref.visible =
                        pvs_ok && (!bsp_ref.has_bounds ||
                                   frustum.aabb_visible(
                                       bsp_ref.bmin, bsp_ref.bmax));
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
            frustum_culled,
            dist_culled,
        };
        /* Conservative bounding radius for frustum-culling models; mod2
         * headers carry no decoded bounding sphere, and the largest placed
         * objects (trees, vehicles) stay within ~5 world units of their
         * origin. */
        constexpr f32 model_radius = 5.f;
        const auto classify_model =
            [&](BSPItem const* bsp, Model const& model) -> model_vis {
            auto pos = model.position;
            /* Cheapest test first: outside the view frustum hides the model
             * regardless of cluster, and skips the BSP-tree walks below. */
            if(!frustum.sphere_visible(pos, model_radius))
                return model_vis::frustum_culled;
            /* Resolve the model's cluster by the EXACT BSP-tree lookup. Scenery
             * origins commonly sit on/under the ground, i.e. in a solid leaf, so
             * the lookup at the origin misses; probe upward through the model
             * body until it lands in open space. The exact tree is preferred
             * over find_cluster()'s subcluster-AABB fallback, whose overlapping
             * boxes can resolve to the wrong cluster and keep an object that is
             * actually in an invisible one. Accurate assignment is what lets an
             * object be culled with an invisible cluster instead of always
             * drawn — important on legacy renderers where models are costly. */
            std::optional<u32> cidx;
            for(f32 up : {0.f, 2.f, 5.f, 10.f, 20.f})
                if(auto ci = bsp->find_cluster_tree(pos + Vecf3{0.f, 0.f, up}))
                {
                    cidx = ci;
                    break;
                }
            if(!cidx)
                if(auto mc = bsp->find_cluster(pos))
                    cidx = mc->first;
            if(cidx)
            {
                if(!cluster_ok(*cidx))
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

        u32 model_visible = 0, model_pvs_culled = 0, model_frustum_culled = 0,
            model_dist_culled = 0, model_total = 0;

        Coffee::Profiler::PushContext("Occluder::model_cull_static");
        for(auto ent : p.select(PositioningStatic))
        {
            auto   ref   = p.template ref<Proxy>(ent.id());
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
                case model_vis::frustum_culled:
                    model.visible = false;
                    model_frustum_culled++;
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
        Coffee::Profiler::PopContext(); /* Occluder::model_cull_static */

        Coffee::Profiler::PushContext("Occluder::model_cull_dynamic");
        for(auto ent : p.select(PositioningDynamic))
        {
            auto   ref   = p.template ref<Proxy>(ent.id());
            Model& model = ref.template get<Model>();
            if(cull_bsp)
                model.visible =
                    classify_model(cull_bsp, model) == model_vis::visible;
            else
                model.visible = in_draw_distance(model);
        }
        Coffee::Profiler::PopContext(); /* Occluder::model_cull_dynamic */

        if((cluster_changed || periodic) && false)
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
                    " | models {}/{} visible ({} PVS-culled, {} frustum-culled,"
                    " {} dist-culled)",
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
                    model_frustum_culled,
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
                    for(auto ent : p.select(PositioningStatic))
                    {
                        if(sample++ >= 5)
                            break;
                        auto   ref   = p.template ref<Proxy>(ent.id());
                        Model& model = ref.template get<Model>();
                        auto   bsp_p = model.position;
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
                    for(auto ent : p.select(ObjectBsp))
                    {
                        if(sample++ >= 5)
                            break;
                        auto          ref = p.template ref<Proxy>(ent.id());
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
                for(auto ent : p.select(ObjectBsp))
                {
                    auto           ref     = p.template ref<Proxy>(ent.id());
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

    void update_debug_viz(Proxy& p)
    {
        DebugMarkers* markers{};
        p.subsystem(markers);

        markers->map();

        u32 player_i = 0;
        for(auto ent : p.template select<PlayerCamera>())
        {
            auto* cam  = p.template get<PlayerCamera>(ent.id());
            auto* info = p.template get<PlayerInfo>(ent.id());
            if(!cam || !info)
                continue;

            /* Grow the pool on demand instead of capping at a compile-time
             * 16 — bounded only by acquire_strip()'s buffer capacity. */
            if(player_i >= eye_pool.size())
            {
                auto slot = markers->acquire_strip(7);
                if(!slot.valid())
                    break; /* buffer exhausted; stop drawing further eyes */

                compo::EntityRecipe marker;
                marker.components = {compo::type_hash_v<DebugDraw>()};
                auto       marker_ent = p.create_entity(marker);
                DebugDraw& draw       = marker_ent.template get<DebugDraw>();
                draw.data.arrays      = {
                         .count  = slot.vert_count,
                         .offset = slot.vert_offset,
                };
                draw.color_ptr = slot.color_idx;

                eye_pool.push_back({marker_ent.id(), slot});
            }
            auto const& eye = eye_pool[player_i];

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

            if(auto* draw = p.template get<DebugDraw>(eye.entity))
                draw->data.arrays.count = eye.slot.vert_count;
            markers->put_strip(
                eye.slot.vert_offset,
                eye.slot.color_idx,
                points,
                Vecf3{.5f, 0, 1.f});
            player_i++;
        }

        for(; player_i < eye_pool.size(); ++player_i)
            if(auto* draw = p.template get<DebugDraw>(eye_pool[player_i].entity))
                draw->data.arrays.count = 0;

        markers->unmap();
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
