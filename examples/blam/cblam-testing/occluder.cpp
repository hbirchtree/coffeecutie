#include "components.h"
#include <utility>
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
    type_list_t<
        Visibility,
        DebugDraw,
        const BspReference,
        const Model,
        const PlayerCamera,
        const PlayerInfo>,
    type_list_t<BSPCache<V>, RenderingParameters, DebugMarkers, GameEventBus>,
    empty_list_t>;

template<typename V>
struct Occluder : compo::RestrictedSubsystem<Occluder<V>, OccluderManifest<V>>
{
    using type  = Occluder<V>;
    using Proxy = compo::proxy_of<OccluderManifest<V>>;

    i16            last_sky_idx{-1};
    u32            frame_counter{0};
    Vecf3          last_camera_pos{};
    BSPItem const* pvs_bsp{nullptr};
    u32            pvs_cluster{0};
    generation_idx_t
        pvs_bsp_id{}; /* which BSP section the camera is currently in */

    /* Which BSP section is active is global game state; the cluster a camera
     * stands in -- and so its portal-traversal set -- is per viewport. */
    struct viewport_pvs_t
    {
        u32               cluster{0};
        std::vector<bool> visible{}; /* per-cluster, recomputed on change */
        Matf4             last_mvp{};
        u32  last_cluster{std::numeric_limits<u32>::max()};
        bool last_found{false};
    };
    std::map<Visibility::viewport_id, viewport_pvs_t> viewport_pvs;
    BSPItem::portal_scratch
        portal_scratch{}; /* reused walk buffers, see caching_item.h */

    struct eye_marker_t
    {
        u64                        entity{0};
        DebugMarkers::strip_slot_t slot{};
    };

    struct cull_target_t
    {
        Vecf3 pos;
        Matf4 mvp;
        Frustum f;
    };

    std::vector<eye_marker_t> eye_pool;

    bool m_markers_scheduled{true};

    Occluder()
    {
        // Ordering constraint: after PhysicsSystem, before DrawListBuilder
        // Also kept parallel with anything that doesn't conflict
        this->priority = 898;
    }

    bool parallel_safe() const override
    {
        return !m_markers_scheduled;
    }

    void start_restricted(Proxy& p, time_point const&)
    {
        BSPCache<V>*         bsp_cache;
        RenderingParameters* rendering;
        DebugMarkers*        markers;
        GameEventBus*        game_bus;
        p.subsystem(bsp_cache);
        p.subsystem(rendering);
        p.subsystem(markers);
        p.subsystem(game_bus);

        if(!rendering->occluder_update)
            return;

        Coffee::ProfContext _("Occluder::frame");

        bool const markers_wanted = markers->available();
        bool const markers_now    = m_markers_scheduled && markers_wanted;
        m_markers_scheduled       = markers_wanted;

        /* available() is gated on RenderingParameters::debug_markers:
         * mapping the marker buffer stalls on GPU sync every frame, which
         * is pure loss when the markers aren't even drawn (release/mobile
         * default). This was ~99% of occluder frame time. */
        if(markers_now)
        {
            Coffee::ProfContext __("Occluder::debug_viz");
            update_debug_viz(p);
        }

        Coffee::Profiler::PushContext("Occluder::pre_section");

        std::map<Visibility::viewport_id, cull_target_t> cull_targets;
        for(auto ent : p.template select<PlayerCamera, PlayerInfo>())
        {
            auto [cam, info] = ent.components();
            if(info.is_remote())
                continue;
            cull_targets[std::make_pair(info.seat_idx, false)] = {
                .pos = cam.camera.position,
                .mvp = cam.matrix,
                .f   = Frustum::from_mvp(cam.matrix),
            };
        }

        const auto primary_view = std::make_pair(0u, false);
        auto&      seat0_cam    = cull_targets[primary_view];

        BSPItem const*   current_bsp{nullptr};
        u32              current_cluster{0};
        generation_idx_t current_bsp_id{};

        /* Teleports (dummy-plug camera events, debug warps) cross no trigger
         * volume; on a large discontinuity re-resolve the section from the
         * camera position. Continuous movement — including noclip through
         * rock — keeps trigger-only semantics. */
        f32 camera_jump = glm::distance(seat0_cam.pos, last_camera_pos);
        last_camera_pos = seat0_cam.pos;
        if(camera_jump > 5.f)
        {
            for(auto& [id, item] : bsp_cache->m_cache)
                if(item.valid() &&
                   item.find_cluster_tree(seat0_cam.pos).has_value())
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
            if(!sw.volume->contains(seat0_cam.pos))
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

        // LOADING STRATEGY:
        // We only select one active BSP; at some point we'll implement
        // a mechanism that teleports the second player to the first player
        // Until then we'll just follow the first player's section transitions
        // This ensures we reserve the ability to evict the entirety of
        // an inactive section, reclaiming the vertex data capacity
        // We still can't do as much with the texture space since it's
        // creeps into so many parts of the architecture such as UI

        if(active_bsp)
        {
            if(auto cluster = active_bsp->find_cluster(seat0_cam.pos);
               cluster.has_value())
            {
                auto [cluster_, sub_] = cluster.value();
                current_bsp           = active_bsp;
                current_cluster       = cluster_;
                current_bsp_id        = active_bsp_id;
            }
        }

        bool section_changed = active_bsp != pvs_bsp;

        /* The active section is culled even while a camera is outside its
         * clusters; other sections stay hidden wholesale. */
        pvs_bsp    = active_bsp;
        pvs_bsp_id = active_bsp_id;
        if(section_changed)
            for(auto& [id, st] : viewport_pvs)
                st.visible.clear(); /* stale per-cluster bits of old section */

        /* Forget viewports that went away, so a returning seat cannot read a
         * stale set back. */
        std::erase_if(viewport_pvs, [&](auto const& kv) {
            return cull_targets.find(kv.first) == cull_targets.end();
        });

        Coffee::Profiler::PopContext(); /* Occluder::section_resolve */

        /* Each viewport resolves its own cluster and visible set: the walk
         * depends on the full view (portal screen rects), so it follows camera
         * motion, but an identical MVP and cluster yields an identical set and
         * idle viewports skip it. */
        bool primary_cluster_changed = false;
        {
            Coffee::ProfContext __("Occluder::portal_visible_set");
            for(auto const& [id, view] : cull_targets)
            {
                auto& st = viewport_pvs[id];

                u32  cluster = st.cluster;
                bool found   = false;
                if(active_bsp)
                    if(auto c = active_bsp->find_cluster(view.pos);
                       c.has_value())
                    {
                        cluster = c.value().first;
                        found   = true;
                    }

                bool changed = section_changed || found != st.last_found ||
                               cluster != st.last_cluster;
                st.last_found   = found;
                st.last_cluster = cluster;
                if(id == primary_view)
                    primary_cluster_changed = changed;

                /* Outside every cluster (noclip through rock): keep the last
                 * valid set. Snapping to all-visible is what used to flash
                 * far-off geometry into view. */
                if(!found)
                    continue;
                st.cluster = cluster;
                if(changed || view.mvp != st.last_mvp)
                {
                    st.visible = pvs_bsp->portal_visible_set(
                        cluster, view.pos, view.mvp, portal_scratch);
                    st.last_mvp = view.mvp;
                }
            }
        }

        pvs_cluster                    = viewport_pvs[primary_view].cluster;
        rendering->current_bsp_cluster = pvs_cluster;

        if(primary_cluster_changed)
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
            rendering->interior = sky_idx < 0;
        }

        BSPItem const* cull_bsp = pvs_bsp;

        /* Whether cluster ci is visible from that viewport's own cluster.
         * Falls back to visible when no set is computed yet. */
        const auto cluster_ok = [](viewport_pvs_t const& st, u32 ci) -> bool {
            if(st.visible.empty() || ci >= st.visible.size())
                return true;
            return st.visible[ci];
        };

        bool periodic = (frame_counter++ % 300) == 0;

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
            for(auto ent : p.template select<BspReference, Visibility>())
            {
                auto [bsp_ref, vis] = ent.components();

                bsp_total++;
                if(bsp_ref.bsp != pvs_bsp_id)
                {
                    /* Different BSP section: hide in every viewport. */
                    vis.hide_all();
                    continue;
                }
                if(bsp_ref.clusters.empty())
                    bsp_no_cluster++;

                /* The cluster set is per viewport, so the PVS test is too;
                 * only the chunk's own cluster list is shared. */
                for(auto const& [idx, view] : cull_targets)
                {
                    auto [seat, mirror] = idx;
                    auto const& st      = viewport_pvs[idx];

                    bool pvs_ok = true;
                    if(!bsp_ref.clusters.empty())
                    {
                        pvs_ok = false;
                        for(u16 ci : bsp_ref.clusters)
                            if(cluster_ok(st, ci))
                            {
                                pvs_ok = true;
                                break;
                            }
                    }

                    vis.set_visibility(
                        pvs_ok &&
                        (!bsp_ref.has_bounds ||
                         view.f.aabb_visible(bsp_ref.bmin, bsp_ref.bmax)),
                        seat,
                        mirror);
                }
                if(vis.visible_any())
                    bsp_visible++;
            }
        }

        const auto in_draw_distance =
            [rendering, draw_dist = rendering->draw_distance](
                cull_target_t const& frustum,
                Model const& mod) {
                return glm::distance(mod.position, frustum.pos) < draw_dist;
            };

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

        /* Resolving which cluster a model sits in is the expensive half of
         * model culling, and it depends only on the model -- so it runs once
         * and every viewport shares the answer. Scenery origins commonly sit
         * on/under the ground, i.e. in a solid leaf, so the lookup at the
         * origin misses; probe upward through the model body until it lands in
         * open space. The exact tree is preferred over find_cluster()'s
         * subcluster-AABB fallback, whose overlapping boxes can resolve to the
         * wrong cluster and keep an object that is actually in an invisible
         * one. Accurate assignment is what lets an object be culled with an
         * invisible cluster instead of always drawn -- important on legacy
         * renderers where models are costly. */
        const auto resolve_cluster =
            [](BSPItem const* bsp, Vecf3 const& pos) -> std::optional<u32> {
            for(f32 up : {0.f, 2.f, 5.f, 10.f, 20.f})
                if(auto ci = bsp->find_cluster_tree(pos + Vecf3{0.f, 0.f, up}))
                    return ci;
            if(auto mc = bsp->find_cluster(pos))
                return mc->first;
            return std::nullopt;
        };

        /* Outside the section's world bounds a model belongs to another BSP
         * section, which is hidden wholesale, so hide its objects too. Inside
         * but clusterless is solid space (scenery planted into the ground) --
         * keep it, gated by draw distance. */
        const auto within_section = [](BSPItem const* bsp, Vecf3 const& pos) {
            if(!bsp->valid())
                return true;
            auto [p1, p2] = bsp->mesh->world_bounds.points();
            Vecf3 lo = glm::min(p1, p2), hi = glm::max(p1, p2);
            return !(pos.x < lo.x || pos.x > hi.x || pos.y < lo.y ||
                     pos.y > hi.y || pos.z < lo.z || pos.z > hi.z);
        };

        /* One model against one viewport. Everything here is cheap; the shared
         * cluster lookup above is what must not repeat. */
        const auto classify_model = [&](cull_target_t const&  view,
                                        viewport_pvs_t const& st,
                                        Model const&          model,
                                        std::optional<u32>    cidx,
                                        bool in_section) -> model_vis {
            if(!view.f.sphere_visible(model.position, model_radius))
                return model_vis::frustum_culled;
            if(cidx ? !cluster_ok(st, *cidx) : !in_section)
                return model_vis::pvs_culled;
            return in_draw_distance(view, model) ? model_vis::visible
                                                 : model_vis::dist_culled;
        };

        u32 model_visible = 0, model_pvs_culled = 0, model_frustum_culled = 0,
            model_dist_culled = 0, model_total = 0;

        /* Counters describe the primary viewport, as they did when culling was
         * single-view. */
        const auto cull_models = [&](auto&& entities) {
            for(auto ent : entities)
            {
                auto         ref   = p.template ref<Proxy>(ent.id());
                Model const& model = ref.template get<Model>();
                Visibility&  vis   = ref.template get<Visibility>();

                model_total++;

                if(!cull_bsp)
                {
                    for(auto const& [idx, view] : cull_targets)
                    {
                        bool ok = in_draw_distance(view, model);
                        vis.set_visibility(ok, idx.first, idx.second);
                        if(idx == primary_view)
                            (ok ? model_visible : model_dist_culled)++;
                    }
                    continue;
                }

                auto cidx = resolve_cluster(cull_bsp, model.position);
                if(cidx)
                    vis.interior =
                        cull_bsp->clusters.at(*cidx).cluster->sky < 0;
                const bool in_section =
                    cidx.has_value() ||
                    within_section(cull_bsp, model.position);

                for(auto const& [idx, view] : cull_targets)
                {
                    auto const state = classify_model(
                        view, viewport_pvs[idx], model, cidx, in_section);
                    vis.set_visibility(
                        state == model_vis::visible, idx.first, idx.second);

                    if(idx != primary_view)
                        continue;
                    switch(state)
                    {
                    case model_vis::visible:
                        model_visible++;
                        break;
                    case model_vis::pvs_culled:
                        model_pvs_culled++;
                        break;
                    case model_vis::frustum_culled:
                        model_frustum_culled++;
                        break;
                    case model_vis::dist_culled:
                        model_dist_culled++;
                        break;
                    }
                }
            }
        };

        Coffee::Profiler::PushContext("Occluder::model_cull_static");
        cull_models(p.select(PositioningStatic));
        Coffee::Profiler::PopContext(); /* Occluder::model_cull_static */

        Coffee::Profiler::PushContext("Occluder::model_cull_dynamic");
        cull_models(p.select(PositioningDynamic));
        Coffee::Profiler::PopContext(); /* Occluder::model_cull_dynamic */

        debug_clusters();
    }

    void compute_player_visibility()
    {
    }

    void update_debug_viz(Proxy& p)
    {
        DebugMarkers* markers{};
        p.subsystem(markers);

        markers->map();

        u32 player_i = 0;
        for(auto ent : p.template select<PlayerCamera, PlayerInfo>())
        {
            auto [cam, info] = ent.components();
            /* Grow the pool on demand instead of capping at a compile-time
             * 16 — bounded only by acquire_strip()'s buffer capacity. */
            if(player_i >= eye_pool.size())
            {
                auto slot = markers->acquire_strip(7);
                if(!slot.valid())
                    break; /* buffer exhausted; stop drawing further eyes */

                compo::EntityRecipe marker;
                marker.components     = {compo::type_hash_v<DebugDraw>()};
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


            std::array<Vecf3, 7> points = {{
                Vecf3{},
                Vecf3{-.1f, .1f, -.1f},
                Vecf3{.1f, .1f, -.1f},
                Vecf3{},
                Vecf3{-.1f, -.1f, -.1f},
                Vecf3{.1f, -.1f, -.1f},
                Vecf3{},
            }};

            Matf4 const to_world = glm::transpose(cam.rotation);
            auto rotate = [&to_world](Vecf3 const& vert) -> Vecf3 {
                return Vecf3(to_world * Vecf4(vert, 1.f));
            };

            auto const& pos = cam.camera.position;
            for(auto& p : points)
                p = rotate(p) + pos;

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
            if(auto* draw =
                   p.template get<DebugDraw>(eye_pool[player_i].entity))
                draw->data.arrays.count = 0;

        markers->unmap();
    }

    void debug_occluder_probe()
    {
        // /* Diagnostic (BLAM_OCCLUDER_PROBE=1): raycast a grid of view
        //  * directions and report each hit surface's cluster and whether the
        //  * portal walk marked it visible. FALSE rows are either portal-walk
        //  * under-reach or face→cluster mis-assignment. */
        // if(::getenv("BLAM_OCCLUDER_PROBE") && current_bsp &&
        //    (frame_counter % 60) == 0)
        // {
        //     auto [camera_pos, camera_mvp, frustum] = seat0_cam;
        //     Matf4 inv       = glm::inverse(camera_mvp);
        //     auto  unproject = [&inv](f32 x, f32 y, f32 z) {
        //         Vecf4 p = inv * Vecf4{x, y, z, 1.f};
        //         return Vecf3(p) / p.w;
        //     };
        //     cDebug(
        //         "Occluder probe: camera cluster {} at ({:.1f},{:.1f},{:.1f})",
        //         pvs_cluster,
        //         camera_pos.x,
        //         camera_pos.y,
        //         camera_pos.z);
        //     for(f32 ny = -0.8f; ny <= 0.81f; ny += 0.1f)
        //         for(f32 nx = -0.9f; nx <= 0.91f; nx += 0.1f)
        //         {
        //             /* Reversed-Z with infinite far plane: z=0 is at infinity,
        //              * so unproject a mid-depth point and shoot from the eye. */
        //             Vecf3 origin = camera_pos;
        //             Vecf3 dir =
        //                 glm::normalize(unproject(nx, ny, 0.5f) - camera_pos);
        //             auto hit =
        //                 current_bsp->raycast(origin, origin + dir * 500.f);
        //             if(!hit)
        //                 continue;
        //             Vecf3              hp = origin + dir * (500.f * hit->t);
        //             std::optional<u32> hc;
        //             for(f32 back : {0.1f, 0.5f, 1.f, 2.f})
        //                 if((hc = current_bsp->find_cluster_tree(
        //                         hp - dir * back)))
        //                     break;
        //             if(!hc)
        //                 continue;
        //             bool vis = *hc < pvs_visible.size() ? bool(pvs_visible[*hc])
        //                                                 : true;
        //             if(!vis)
        //                 cDebug(
        //                     "  probe ndc=({:+.1f},{:+.1f}) t={:.3f}"
        //                     " hit=({:.1f},{:.1f},{:.1f}) cluster={} vis={}",
        //                     nx,
        //                     ny,
        //                     hit->t,
        //                     hp.x,
        //                     hp.y,
        //                     hp.z,
        //                     *hc,
        //                     vis);
        //         }
        // }
    }

    void debug_clusters()
    {
        // if((cluster_changed || periodic) && false)
        // {
        //     u32 total_clusters =
        //         current_bsp ? static_cast<u32>(current_bsp->clusters.size())
        //                     : 0;
        //     if(current_bsp)
        //     {
        //         cDebug(
        //             "Occluder [frame {}]: cluster {}/{}"
        //             " bsp=({:.1f},{:.1f},{:.1f})"
        //             " | BSP {}/{} visible ({} no-cluster)"
        //             " | models {}/{} visible ({} PVS-culled, {} frustum-culled,"
        //             " {} dist-culled)",
        //             frame_counter,
        //             current_cluster,
        //             total_clusters,
        //             camera_pos.x,
        //             camera_pos.y,
        //             camera_pos.z,
        //             bsp_visible,
        //             bsp_total,
        //             bsp_no_cluster,
        //             model_visible,
        //             model_total,
        //             model_pvs_culled,
        //             model_frustum_culled,
        //             model_dist_culled);

        //         /* Print current cluster's subcluster bounds */
        //         {
        //             auto const& cc = current_bsp->clusters.at(current_cluster);
        //             u32         si = 0;
        //             for(auto const& sub : cc.sub)
        //             {
        //                 auto [bmin, bmax] = sub.cluster->bounds.points();
        //                 cDebug(
        //                     "  cluster[{}] sub[{}] bounds:"
        //                     " ({:.1f},{:.1f},{:.1f})..({:.1f},{:.1f},{:.1f})",
        //                     current_cluster,
        //                     si++,
        //                     bmin.x,
        //                     bmin.y,
        //                     bmin.z,
        //                     bmax.x,
        //                     bmax.y,
        //                     bmax.z);
        //             }
        //         }

        //         /* Show portal-reachable cluster count */
        //         {
        //             u32 reachable = 0;
        //             for(bool v : pvs_visible)
        //                 if(v)
        //                     reachable++;
        //             cDebug(
        //                 "  portal-reachable clusters: {}/{}",
        //                 reachable,
        //                 static_cast<u32>(current_bsp->clusters.size()));
        //         }

        //         /* Per-portal frustum debug: show pass/cull for each portal
        //          * of the camera cluster, with centroid and front-vertex count.
        //          */
        //         {
        //             Frustum frustum = Frustum::from_mvp(camera_mvp);
        //             cDebug(
        //                 "  cam_plane=({:.3f},{:.3f},{:.3f},{:.3f})",
        //                 frustum.cam_plane.x,
        //                 frustum.cam_plane.y,
        //                 frustum.cam_plane.z,
        //                 frustum.cam_plane.w);
        //             auto const& cc = current_bsp->clusters.at(current_cluster);
        //             u32         pi = 0;
        //             for(auto const& portal : cc.portals)
        //             {
        //                 i32  adj = (portal.data->front_cluster ==
        //                            static_cast<i16>(current_cluster))
        //                                ? portal.data->back_cluster
        //                                : portal.data->front_cluster;
        //                 bool near =
        //                     glm::distance(camera_pos, portal.data->centroid) <=
        //                     portal.data->bound_radius;
        //                 bool poly = frustum.polygon_inside(portal.vertices);

        //                 u32 front_count = 0;
        //                 for(auto const& v : portal.vertices)
        //                     if(glm::dot(Vecf3(frustum.cam_plane), v) +
        //                            frustum.cam_plane.w >
        //                        0.f)
        //                         front_count++;

        //                 cDebug(
        //                     "  portal[{}]→cluster[{}] "
        //                     "centroid=({:.1f},{:.1f},{:.1f})"
        //                     " r={:.1f} verts={} front={}"
        //                     " near={} poly={} result={}",
        //                     pi++,
        //                     adj,
        //                     portal.data->centroid.x,
        //                     portal.data->centroid.y,
        //                     portal.data->centroid.z,
        //                     portal.data->bound_radius,
        //                     portal.vertices.size(),
        //                     front_count,
        //                     near,
        //                     poly,
        //                     (near || poly) ? "PASS" : "CULL");
        //             }
        //         }

        //         /* Sample first 5 model and BSP centroid positions */
        //         {
        //             u32 sample = 0;
        //             for(auto ent : p.select(PositioningStatic))
        //             {
        //                 if(sample++ >= 5)
        //                     break;
        //                 auto         ref   = p.template ref<Proxy>(ent.id());
        //                 Model const& model = ref.template get<Model>();
        //                 auto         bsp_p = model.position;
        //                 auto   mc    = current_bsp->find_cluster(bsp_p);
        //                 cDebug(
        //                     "  model[{}] scenario=({:.1f},{:.1f},{:.1f})"
        //                     " bsp=({:.1f},{:.1f},{:.1f}) cluster={} visible={}",
        //                     sample - 1,
        //                     model.position.x,
        //                     model.position.y,
        //                     model.position.z,
        //                     bsp_p.x,
        //                     bsp_p.y,
        //                     bsp_p.z,
        //                     mc.has_value() ? std::to_string(mc.value().first)
        //                                    : std::string("none"),
        //                     ref.template get<Visibility>().visible);
        //             }
        //         }
        //         {
        //             u32 sample = 0;
        //             for(auto ent : p.select(ObjectBsp))
        //             {
        //                 if(sample++ >= 5)
        //                     break;
        //                 auto                ref = p.template ref<Proxy>(ent.id());
        //                 BspReference const& bsp_ref =
        //                     ref.template get<BspReference>();
        //                 bool has_cluster = bsp_ref.cluster_idx !=
        //                                    std::numeric_limits<u32>::max();
        //                 cDebug(
        //                     "  bsp[{}] cluster={} visible={}",
        //                     sample - 1,
        //                     has_cluster ? std::to_string(bsp_ref.cluster_idx)
        //                                 : std::string("none"),
        //                     ref.template get<Visibility>().visible);
        //             }
        //         }
        //     } else
        //     {
        //         cDebug(
        //             "Occluder [frame {}]: camera outside all BSP clusters"
        //             " | bsp=({:.1f},{:.1f},{:.1f})",
        //             frame_counter,
        //             camera_pos.x,
        //             camera_pos.y,
        //             camera_pos.z);

        //         /* Print BSP world bounds + first cluster bounds so we can
        //          * see the coordinate space the BSP lives in */
        //         for(auto ent : p.select(ObjectBsp))
        //         {
        //             auto ref = p.template ref<Proxy>(ent.id());
        //             BspReference const& bsp_ref =
        //                 ref.template get<BspReference>();
        //             BSPItem const& bsp = bsp_cache->find(bsp_ref.bsp)->second;
        //             if(!bsp.valid())
        //                 break;
        //             auto [wmin, wmax] = bsp.mesh->world_bounds.points();
        //             cDebug(
        //                 "  BSP world_bounds: ({:.1f},{:.1f},{:.1f})"
        //                 " .. ({:.1f},{:.1f},{:.1f})",
        //                 wmin.x,
        //                 wmin.y,
        //                 wmin.z,
        //                 wmax.x,
        //                 wmax.y,
        //                 wmax.z);
        //             u32 ci = 0;
        //             for(auto const& cluster : bsp.clusters)
        //             {
        //                 for(auto const& sub : cluster.sub)
        //                 {
        //                     auto [bmin, bmax] = sub.cluster->bounds.points();
        //                     cDebug(
        //                         "  cluster[{}] subcluster bounds:"
        //                         " ({:.1f},{:.1f},{:.1f})"
        //                         " .. ({:.1f},{:.1f},{:.1f})",
        //                         ci,
        //                         bmin.x,
        //                         bmin.y,
        //                         bmin.z,
        //                         bmax.x,
        //                         bmax.y,
        //                         bmax.z);
        //                 }
        //                 if(++ci >= 3)
        //                     break; /* Only print first few clusters */
        //             }
        //             break; /* One BSP is enough */
        //         }
        //     }
        // }
    }
};

void alloc_occluder(compo::EntityContainer& container)
{
    ProfContext _;
    container.register_subsystem_inplace<Occluder<halo_version>>();
}
