#pragma once

#include <blam/volta/blam_antr.h>
#include <blam/volta/blam_bitm.h>
#include <blam/volta/blam_bsp_structures.h>
#include <blam/volta/blam_font.h>
#include <blam/volta/blam_mod2.h>
#include <blam/volta/blam_ui.h>

#include "data_cache.h"
#include "graphics_api.h"

#include <coffee/core/debug/formatting.h>

using Coffee::cDebug;
using Coffee::cFatal;
using Coffee::cWarning;
using Coffee::ProfContext;

using libc_types::f32;
using libc_types::i16;
using libc_types::i32;
using libc_types::u16;
using libc_types::u32;
using semantic::Span;
using stl_types::range;
using typing::pix_components;
using typing::geometry::size_2d;
using typing::geometry::size_3d;
using typing::pixels::bit_fmt;
using typing::pixels::comp_flags;
using typing::pixels::pix_fmt;
using typing::pixels::PixDesc;
using typing::vector_types::Matf3;
using typing::vector_types::Matf4;
using typing::vector_types::Quatf;
using typing::vector_types::Vecf2;
using typing::vector_types::Vecf3;
using typing::vector_types::Vecf4;
using typing::vector_types::Veci2;
using typing::vector_types::Veci3;
using typing::vector_types::Vecui2;
using typing::vector_types::Vecui3;

using bitm_format_hash = std::
    tuple<blam::bitm::type_t, pix_fmt, pix_components, bit_fmt, comp_flags>;

/* Camera frustum built from a projection×view matrix (Gribb-Hartmann).
 * Only the four side planes are used; near/far are skipped because the
 * projection matrix in this codebase sets m[2][2]=0 for an infinite far
 * plane, making those two rows unreliable.
 * cam_plane (row3 of MVP) identifies vertices in front of the camera:
 * dot(cam_plane.xyz, v) + cam_plane.w > 0  ↔  clip_w > 0  ↔  in front. */
struct Frustum
{
    std::array<Vecf4, 4> planes;    /* left, right, bottom, top */
    Vecf4                cam_plane; /* row3 of MVP — NOT normalised */

    static Frustum from_mvp(Matf4 const& m)
    {
        auto row = [&](int j) {
            return Vecf4(m[0][j], m[1][j], m[2][j], m[3][j]);
        };
        Vecf4   r0 = row(0), r1 = row(1), r3 = row(3);
        Frustum f;
        f.cam_plane = r3;
        f.planes[0] = r3 + r0; /* left   */
        f.planes[1] = r3 - r0; /* right  */
        f.planes[2] = r3 + r1; /* bottom */
        f.planes[3] = r3 - r1; /* top    */
        for(auto& p : f.planes)
        {
            f32 len = glm::length(Vecf3(p));
            if(len > 0.f)
                p /= len;
        }
        return f;
    }

    /* Returns false if the AABB (bmin..bmax, world space) is entirely behind
     * the camera or entirely outside any of the four side planes.
     * Uses the p-vertex trick: for each plane pick the AABB corner with the
     * highest dot-product against the plane normal; if even that corner is
     * outside, the whole box is. */
    bool aabb_visible(Vecf3 const& bmin, Vecf3 const& bmax) const
    {
        bool cam_valid = glm::dot(Vecf3(cam_plane), Vecf3(cam_plane)) > 1e-10f;
        if(cam_valid)
        {
            Vecf3 p{
                cam_plane.x >= 0.f ? bmax.x : bmin.x,
                cam_plane.y >= 0.f ? bmax.y : bmin.y,
                cam_plane.z >= 0.f ? bmax.z : bmin.z,
            };
            if(glm::dot(Vecf3(cam_plane), p) + cam_plane.w <= 0.f)
                return false;
        }
        for(auto const& plane : planes)
        {
            Vecf3 p{
                plane.x >= 0.f ? bmax.x : bmin.x,
                plane.y >= 0.f ? bmax.y : bmin.y,
                plane.z >= 0.f ? bmax.z : bmin.z,
            };
            if(glm::dot(Vecf3(plane), p) + plane.w < 0.f)
                return false;
        }
        return true;
    }

    /* Returns false if the polygon is entirely outside any single plane.
     * Vertices behind the camera (clip_w ≤ 0) are treated as "inside" all
     * planes because Gribb-Hartmann gives inverted results for them.
     * If ALL vertices are behind the camera the portal is culled.
     * If cam_plane is degenerate (zero — happens on the first frame before
     * the camera quaternion is initialised) the camera-side test is skipped
     * and we fall back to the plain per-plane polygon test. */
    bool polygon_inside(std::vector<Vecf3> const& verts) const
    {
        if(verts.empty())
            return true;

        bool cam_valid = glm::dot(Vecf3(cam_plane), Vecf3(cam_plane)) > 1e-10f;

        if(cam_valid)
        {
            /* All vertices behind camera → portal is behind us. */
            bool any_front = false;
            for(auto const& v : verts)
            {
                if(glm::dot(Vecf3(cam_plane), v) + cam_plane.w > 0.f)
                {
                    any_front = true;
                    break;
                }
            }
            if(!any_front)
                return false;
        }

        /* Small tolerance so a portal whose vertex grazes the frustum boundary
         * doesn't flicker in/out with minor camera movement. */
        constexpr f32 kFrustumEps = 0.1f;

        for(auto const& p : planes)
        {
            bool all_outside = true;
            for(auto const& v : verts)
            {
                /* When cam_plane is valid, behind-camera vertices count as
                 * "inside" — their clip_w < 0 inverts the half-space test. */
                if(cam_valid &&
                   glm::dot(Vecf3(cam_plane), v) + cam_plane.w <= 0.f)
                {
                    all_outside = false;
                    break;
                }
                if(glm::dot(Vecf3(p), v) + p.w >= -kFrustumEps)
                {
                    all_outside = false;
                    break;
                }
            }
            if(all_outside)
                return false;
        }
        return true;
    }
};

struct BSPItem
{
    struct Mesh
    {
        blam::bsp::material const*  mesh{nullptr};
        gleam::draw_command::data_t draw;
        generation_idx_t            light_bitm;
        generation_idx_t            shader;
        u32 cluster_idx{std::numeric_limits<u32>::max()};
        u32 subcluster_idx{std::numeric_limits<u32>::max()};
    };

    struct Group
    {
        blam::bsp::lightmap const* group{nullptr};
        std::vector<Mesh>          meshes{};
    };

    struct Subcluster
    {
        blam::bsp::subcluster const* cluster{nullptr};
        Span<u32 const>              indices;
        u32                          debug_color_idx{0};
    };

    struct Portal
    {
        blam::bsp::cluster_portal const* data{nullptr};
        std::vector<Vecf3>               vertices{};
    };

    struct Cluster
    {
        blam::bsp::cluster const* cluster{nullptr};
        std::vector<Subcluster>   sub{};
        std::vector<Portal>       portals{};
    };

    /* Flat entry for fast position-to-cluster lookup.
     * Sorted by volume ascending so find_cluster() returns on the first hit.
     * bmin/bmax stored inline to avoid pointer chasing. Struct is 32 bytes. */
    struct FlatSubcluster
    {
        Vecf3 bmin, bmax;  /* 12+12 bytes */
        u32   cluster_idx; /* 4 bytes */
        u32   sub_idx;     /* 4 bytes */
    };

    static_assert(sizeof(FlatSubcluster) == 32);

    blam::bsp::header const* mesh{nullptr};
    blam::tag_t const*       tag{nullptr};
    /* Index into the scenario's structure BSP list (bsp_info order); used to
     * match against bsp_switch_trigger source/destination. */
    libc_types::i16 section_idx{-1};
    std::vector<Group>                                      groups;
    std::vector<Cluster>                                    clusters;
    std::vector<FlatSubcluster>                             sorted_subclusters;
    std::vector<gleam::draw_command::data_t>                portals;
    std::vector<u32>                                        portal_color_ptrs;
    std::vector<blam::bsp::background_sound_palette const*> bg_sound_palette;

    /* PVS (Potentially Visible Set) data: one bit per cluster per row,
     * row i says which clusters are visible from cluster i */
    Span<libc_types::byte_t const> pvs_data;
    u32                            pvs_row_stride{0};

    /* Collision BSP tree for exact point→cluster lookup. The collision
     * leaf index space is shared with the render leaves, whose cluster
     * field maps into clusters. This is how the original engine resolves
     * the camera cluster; the subcluster AABBs neither tile nor cover the
     * cluster volume, so they are only a fallback. */
    Span<blam::collision::bsp_3d const> tree_nodes;
    Span<blam::collision::plane const>  tree_planes;
    Span<blam::bsp::leaf const>         render_leaves;

    /* Collision surface mesh (winged-edge), same collision BSP source;
     * consumed by the physics subsystem for triangle soup generation. */
    Span<blam::collision::surface const> coll_surfaces;
    Span<blam::collision::edge const>    coll_edges;
    Span<blam::collision::vertex const>  coll_vertices;

    inline bool valid() const
    {
        return mesh;
    }

    /* BFS through the portal graph from from_idx up to max_depth hops.
     * Returns a bitset (one bool per cluster) of reachable clusters.
     * max_depth=1 gives the camera cluster + its immediate portal neighbours.
     */
    inline std::vector<bool> portal_visible_set(
        u32 from_idx, u32 max_depth = std::numeric_limits<u32>::max()) const
    {
        std::vector<bool> visible(clusters.size(), false);
        if(from_idx >= clusters.size())
            return visible;

        std::vector<u32> frontier = {from_idx};
        visible[from_idx]         = true;

        for(u32 depth = 0; depth < max_depth && !frontier.empty(); depth++)
        {
            std::vector<u32> next;
            for(u32 ci : frontier)
            {
                for(auto const& portal : clusters[ci].portals)
                {
                    i32 adj =
                        (portal.data->front_cluster == static_cast<i16>(ci))
                            ? portal.data->back_cluster
                            : portal.data->front_cluster;
                    if(adj >= 0 && static_cast<u32>(adj) < clusters.size() &&
                       !visible[static_cast<u32>(adj)])
                    {
                        visible[static_cast<u32>(adj)] = true;
                        next.push_back(static_cast<u32>(adj));
                    }
                }
            }
            frontier = std::move(next);
        }
        return visible;
    }

    /* Clip a polygon to the inside (dot(n,v)+d >= 0) of a plane,
     * Sutherland–Hodgman style. */
    static std::vector<Vecf3> clip_polygon(
        std::vector<Vecf3> const& poly, Vecf4 const& plane)
    {
        std::vector<Vecf3> out;
        int                n = static_cast<int>(poly.size());
        out.reserve(poly.size() + 2);
        for(int i = 0; i < n; i++)
        {
            Vecf3 const& a  = poly[i];
            Vecf3 const& b  = poly[(i + 1) % n];
            f32          da = glm::dot(Vecf3(plane), a) + plane.w;
            f32          db = glm::dot(Vecf3(plane), b) + plane.w;
            if(da >= 0.f)
                out.push_back(a);
            if((da > 0.f && db < 0.f) || (da < 0.f && db > 0.f))
                out.push_back(a + (b - a) * (da / (da - db)));
        }
        return out;
    }

    /* Portal-flow visibility with screen-space rectangles (the approach the
     * original engine uses). Each traversal path carries an NDC rectangle —
     * initially the full viewport — and a portal is only crossed where its
     * polygon, clipped to that rectangle's view pyramid, still has area. The
     * surviving polygon is projected and its NDC bounding box becomes the
     * rectangle for the next cluster, monotonically shrinking along the path.
     * Rectangles stay convex no matter the portal polygon shape, which
     * matters because Halo cluster portals are frequently non-convex
     * (20-30 vertices); cone planes built from such polygons over-cull.
     * A cluster is re-entered only when a new path widens its accumulated
     * rectangle, so the walk terminates without missing multi-path
     * visibility. */
    inline std::vector<bool> portal_visible_set(
        u32          from_idx,
        Vecf3 const& /*camera_pos*/,
        Matf4 const& mvp,
        u32          max_depth = 64) const
    {
        std::vector<bool> visible(clusters.size(), false);
        if(from_idx >= clusters.size())
            return visible;
        visible[from_idx] = true;

        auto row = [&](int j) {
            return Vecf4(mvp[0][j], mvp[1][j], mvp[2][j], mvp[3][j]);
        };
        Vecf4 const r0 = row(0), r1 = row(1), r3 = row(3);

        /* Degenerate MVP (first frame, before the camera is initialised):
         * no frustum information, fall back to plain reachability. */
        if(glm::dot(Vecf3(r3), Vecf3(r3)) <= 1e-10f)
            return portal_visible_set(from_idx, max_depth);

        struct Rect
        {
            f32 x0, y0, x1, y1;
        };

        /* World-space planes through the eye bounding the view pyramid of an
         * NDC rectangle: x_ndc ≥ x0 ⟺ (r0 - x0·r3)·v ≥ 0, etc. */
        auto rect_planes = [&](Rect const& r) -> std::array<Vecf4, 4> {
            return {{
                r0 - r3 * r.x0,
                r3 * r.x1 - r0,
                r1 - r3 * r.y0,
                r3 * r.y1 - r1,
            }};
        };

        struct Entry
        {
            u32  ci;
            u32  depth;
            Rect rect;
        };

        /* Per-cluster accumulated rect, for skipping paths that cannot show
         * anything new. */
        constexpr Rect              empty_rect{1.f, 1.f, -1.f, -1.f};
        std::vector<Rect>           acc(clusters.size(), empty_rect);
        std::vector<libc_types::u8> visits(clusters.size(), 0);

        acc[from_idx]    = Rect{-1.f, -1.f, 1.f, 1.f};
        visits[from_idx] = 1;

        std::vector<Entry> stack = {{from_idx, 0, acc[from_idx]}};

        u32 budget = 4096; /* hard cap on portal crossings per frame */

        while(!stack.empty())
        {
            Entry entry = stack.back();
            stack.pop_back();
            if(entry.depth >= max_depth)
                continue;
            auto const planes = rect_planes(entry.rect);
            for(auto const& portal : clusters[entry.ci].portals)
            {
                if(budget-- == 0)
                    return visible;
                i32 adj =
                    (portal.data->front_cluster == static_cast<i16>(entry.ci))
                        ? portal.data->back_cluster
                        : portal.data->front_cluster;
                u32 ai = static_cast<u32>(adj);
                if(adj < 0 || ai >= clusters.size())
                    continue;

                /* Clip the portal polygon to the rect's view pyramid, plus a
                 * near plane slightly in front of the eye so the projection
                 * below never divides by w ≤ 0. A portal the camera is about
                 * to cross gets clipped at the eye and projects to roughly
                 * the whole rect, which is the desired behaviour. */
                std::vector<Vecf3> poly = clip_polygon(
                    portal.vertices, Vecf4(Vecf3(r3), r3.w - 1e-3f));
                for(auto const& plane : planes)
                {
                    if(poly.size() < 3)
                        break;
                    poly = clip_polygon(poly, plane);
                }
                if(poly.size() < 3)
                    continue;

                visible[ai] = true;

                Rect r = empty_rect;
                for(auto const& v : poly)
                {
                    f32 w = glm::dot(Vecf3(r3), v) + r3.w;
                    f32 x = (glm::dot(Vecf3(r0), v) + r0.w) / w;
                    f32 y = (glm::dot(Vecf3(r1), v) + r1.w) / w;
                    r.x0  = std::min(r.x0, x);
                    r.y0  = std::min(r.y0, y);
                    r.x1  = std::max(r.x1, x);
                    r.y1  = std::max(r.y1, y);
                }
                /* Numerical safety: stay inside the parent rect. */
                r.x0 = std::max(r.x0, entry.rect.x0);
                r.y0 = std::max(r.y0, entry.rect.y0);
                r.x1 = std::min(r.x1, entry.rect.x1);
                r.y1 = std::min(r.y1, entry.rect.y1);
                if(r.x0 >= r.x1 || r.y0 >= r.y1)
                    continue;

                /* Only walk on if this path widens what the cluster can
                 * already show. */
                Rect& a = acc[ai];
                if(r.x0 >= a.x0 && r.y0 >= a.y0 && r.x1 <= a.x1 &&
                   r.y1 <= a.y1)
                    continue;
                a.x0 = std::min(a.x0, r.x0);
                a.y0 = std::min(a.y0, r.y0);
                a.x1 = std::max(a.x1, r.x1);
                a.y1 = std::max(a.y1, r.y1);
                if(visits[ai] >= 8)
                    continue;
                visits[ai]++;

                stack.push_back({ai, entry.depth + 1, r});
            }
        }
        return visible;
    }

    /* Returns true if cluster to_idx is visible from cluster from_idx
     * according to the PVS. Falls back to true if no PVS is available. */
    inline bool cluster_visible_from(u32 from_idx, u32 to_idx) const
    {
        if(pvs_data.empty() || pvs_row_stride == 0 || clusters.empty())
            return true;
        if(from_idx >= clusters.size() || to_idx >= clusters.size())
            return true;
        auto const* row = pvs_data.data() + from_idx * pvs_row_stride;
        return (row[to_idx / 8] >> (to_idx % 8)) & 1;
    }

    /* Exact point→cluster lookup by walking the collision BSP tree down to a
     * leaf, then mapping the leaf to a cluster via the render leaf array.
     * Returns nullopt for solid/outside-the-map points and on malformed
     * data. Children with the sign bit set are leaves (index = child &
     * 0x7fffffff); -1 means solid space. */
    inline std::optional<u32> find_cluster_tree(Vecf3 const& point) const
    {
        if(tree_nodes.empty() || tree_planes.empty() || render_leaves.empty())
            return std::nullopt;
        i32 node = 0;
        for(size_t guard = 0; guard <= tree_nodes.size(); guard++)
        {
            if(node == -1)
                return std::nullopt; /* solid space */
            if(node < 0)
            {
                u32 leaf_idx = static_cast<u32>(node) & 0x7fffffffu;
                if(leaf_idx >= render_leaves.size())
                    return std::nullopt;
                i16 cluster = render_leaves[leaf_idx].cluster;
                if(cluster < 0 ||
                   static_cast<u32>(cluster) >= clusters.size())
                    return std::nullopt;
                return static_cast<u32>(cluster);
            }
            if(static_cast<u32>(node) >= tree_nodes.size())
                return std::nullopt;
            auto const& n = tree_nodes[node];
            if(n.plane < 0 ||
               static_cast<u32>(n.plane) >= tree_planes.size())
                return std::nullopt;
            auto const& pl = tree_planes[n.plane];
            node = glm::dot(pl.plane, point) >= pl.d ? n.front : n.back;
        }
        return std::nullopt;
    }

    /* Native hitscan against the collision BSP (Quake-style recursive
     * segment trace through the solid-leaf tree). Returns the first
     * empty→solid boundary along start→end. Plane-level precision: gives
     * hit point/normal/plane; surface+material resolution via the leaf's
     * 2D BSPs is a later refinement. */
    struct ray_hit
    {
        f32   t; /* fraction along start→end */
        Vecf3 normal;
        i32   plane{-1};
    };

    inline std::optional<ray_hit> raycast(
        Vecf3 const& start, Vecf3 const& end) const
    {
        if(tree_nodes.empty() || tree_planes.empty())
            return std::nullopt;
        ray_hit hit{};
        if(raycast_r(0, 0.f, 1.f, start, end, hit) == 2)
            return hit;
        return std::nullopt;
    }

    /* Subsegment classification:
     *   0 = no hit, contains empty space (leading solid is skipped — the
     *       sealed-world exterior is solid, so rays may legally start there)
     *   1 = entirely solid
     *   2 = hit recorded in out (first empty→solid crossing in ray order)
     */
    inline int raycast_r(
        i32          node,
        f32          t0,
        f32          t1,
        Vecf3 const& p0,
        Vecf3 const& p1,
        ray_hit&     out) const
    {
        if(node == -1)
            return 1; /* solid space */
        if(node < 0)
            return 0; /* empty leaf */
        if(static_cast<u32>(node) >= tree_nodes.size())
            return 0;
        auto const& n = tree_nodes[node];
        if(n.plane < 0 || static_cast<u32>(n.plane) >= tree_planes.size())
            return 0;
        auto const& pl = tree_planes[n.plane];
        f32         d0 = glm::dot(pl.plane, p0) - pl.d;
        f32         d1 = glm::dot(pl.plane, p1) - pl.d;
        if(d0 >= 0.f && d1 >= 0.f)
            return raycast_r(n.front, t0, t1, p0, p1, out);
        if(d0 < 0.f && d1 < 0.f)
            return raycast_r(n.back, t0, t1, p0, p1, out);
        f32 frac = glm::clamp(d0 / (d0 - d1), 0.f, 1.f);
        f32 tm   = t0 + (t1 - t0) * frac;
        Vecf3 mid = p0 + (p1 - p0) * frac;
        i32 near_c = d0 >= 0.f ? n.front : n.back;
        i32 far_c  = d0 >= 0.f ? n.back : n.front;
        int rn = raycast_r(near_c, t0, tm, p0, mid, out);
        if(rn == 2)
            return 2;
        int rf = raycast_r(far_c, tm, t1, mid, p1, out);
        if(rf == 2)
            return 2;
        if(rn == 0 && rf == 1)
        {
            /* near side passed through empty space, far side is solid
             * right at the crossing → surface here */
            out.t      = tm;
            out.normal = d0 >= 0.f ? pl.plane : -pl.plane;
            out.plane  = n.plane;
            return 2;
        }
        return (rn == 1 && rf == 1) ? 1 : 0;
    }

    inline std::optional<std::pair<u32, u32>> find_cluster(
        Vecf3 const& point) const
    {
        /* Prefer the exact BSP-tree lookup; subcluster AABBs overlap and do
         * not cover the full cluster volume, so they are only a fallback
         * (and provide the subcluster index for debug visualisation). */
        if(auto ci = find_cluster_tree(point); ci.has_value())
        {
            auto const& subs = clusters[*ci].sub;
            for(u32 si = 0; si < static_cast<u32>(subs.size()); si++)
            {
                auto [p1, p2] = subs[si].cluster->bounds.points();
                Vecf3 lo = glm::min(p1, p2), hi = glm::max(p1, p2);
                if(lo.x <= point.x && point.x <= hi.x && lo.y <= point.y &&
                   point.y <= hi.y && lo.z <= point.z && point.z <= hi.z)
                    return std::pair{*ci, si};
            }
            return std::pair{*ci, 0u};
        }

        /* sorted_subclusters is sorted by volume ascending, so the first
         * hit is the smallest-volume (most specific) containing subcluster. */
        for(auto const& fs : sorted_subclusters)
        {
            if(fs.bmin.x <= point.x && point.x <= fs.bmax.x &&
               fs.bmin.y <= point.y && point.y <= fs.bmax.y &&
               fs.bmin.z <= point.z && point.z <= fs.bmax.z)
                return std::pair{fs.cluster_idx, fs.sub_idx};
        }
        return std::nullopt;
    }

    /* Returns nullopt when either position falls outside all clusters.
     * Otherwise returns true if target is PVS-visible from observer. */
    inline std::optional<bool> visible_from(
        Vecf3 const& observer, Vecf3 const& target) const
    {
        auto from = find_cluster(observer);
        if(!from)
            return std::nullopt;
        auto to = find_cluster(target);
        if(!to)
            return std::nullopt;
        return cluster_visible_from(from->first, to->first);
    }
};

template<typename V>
struct ModelItem
{
    struct SubModel
    {
        blam::mod2::part const*     header{nullptr};
        gleam::draw_command::data_t draw;
        generation_idx_t            shader;
    };

    struct LOD
    {
        blam::mod2::geometry_header<V> const* header{nullptr};
        std::vector<SubModel>                 sub;
    };

    blam::mod2::header<V> const* header{nullptr};
    blam::tag_t const*           tag{nullptr};
    LOD                          mesh;

    std::vector<Matf4>        bone_matrices; /* per-bone skinning matrices (world_anim * inv_bind) */
    std::vector<Matf4>        inv_bind;      /* inverse bind-world matrices, built from mod2 bone axes */
    i32                       bone_base{-1};
    blam::antr::header const* antr_hdr{nullptr};   /* non-null → animate this model each frame */
    u32                       anim_idx{0};          /* animation index within antr_hdr */
    u32                       anim_frame_count{0};  /* frame_count for anim_idx */

    inline bool valid() const
    {
        return header;
    }
};

struct ModelAssembly
{
    blam::mod2::region const*                header;
    semantic::Span<blam::mod2::region const> regions;
    std::vector<generation_idx_t>            models;
};

struct BitmapItem
{
    blam::bitm::header_t const* header{nullptr};
    blam::tag_t const*          tag{nullptr};

    struct
    {
        blam::bitm::image_t const* mip{nullptr};
        PixDesc                    fmt;
        bitm_format_hash           bucket;

        u32   layer{0};
        Vecf2 offset{};
        Vecf2 scale{};
        f32   bias{0.f};
    } image;

    struct
    {
        u32 base{0};
        u32 last{5};
    } mipmaps;

    inline bool valid() const
    {
        return header;
    }
};

struct ShaderItem
{
    blam::shader::radiosity_properties const* header{nullptr};
    blam::tag_t const*                        tag{nullptr};
    blam::tag_class_t tag_class{blam::tag_class_t::undefined};

    inline blam::tag_class_t type()
    {
        if(!tag)
            Throw(undefined_behavior("invalid shader item"));
        return tag_class;
    }

    struct soso_t
    {
        generation_idx_t base_bitm;
        generation_idx_t multi_bitm;
        generation_idx_t detail_bitm;
        generation_idx_t reflection_bitm;
    };

    struct senv_t
    {
        generation_idx_t base_bitm;
        generation_idx_t primary_bitm;
        generation_idx_t secondary_bitm;
        generation_idx_t micro_bitm;

        generation_idx_t bump;

        generation_idx_t self_illum;

        generation_idx_t reflection_bitm;
    };

    struct schi_t
    {
        std::array<generation_idx_t, 4> maps;
        std::array<generation_idx_t, 4> layers;
    };

    struct scex_t
    {
        std::array<generation_idx_t, 4> maps;
        std::array<generation_idx_t, 4> layers;
        generation_idx_t                lens_flare;
    };

    struct swat_t
    {
        generation_idx_t base;
        generation_idx_t reflection;
        generation_idx_t bump;
    };

    struct sotr_t
    {
        std::array<generation_idx_t, 4> maps;
        std::array<generation_idx_t, 4> layers;
    };

    struct smet_t
    {
        generation_idx_t map;
    };

    struct sgla_t
    {
        generation_idx_t diffuse;
        generation_idx_t reflection_cube;
        generation_idx_t bump;
    };

    struct spla_t
    {
        generation_idx_t primary_noise;
        generation_idx_t secondary_noise;
    };

    union
    {
        scex_t scex;
        schi_t schi;
        senv_t senv;
        soso_t soso;
        sotr_t sotr;
        swat_t swat;
        smet_t smet;
        sgla_t sgla;
        spla_t spla;
    };

    inline bool valid() const
    {
        return header;
    }
};
