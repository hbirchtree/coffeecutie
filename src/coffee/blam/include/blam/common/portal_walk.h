#pragma once

#include <peripherals/typing/vectors/vector_types.h>

#include <array>
#include <limits>
#include <vector>

/*!
 * Portal-flow visibility over a cluster graph, format-agnostic: Halo 1
 * and 2 both store a cluster list where `clusters[i].portals` is a range
 * whose items have `front_cluster()`, `back_cluster()` and a `vertices`
 * polygon in the space `mvp` transforms from.
 */
namespace blam::portal_walk {

using libc_types::f32;
using libc_types::i16;
using libc_types::i32;
using libc_types::u32;
using typing::vector_types::Matf4;
using typing::vector_types::Vecf3;
using typing::vector_types::Vecf4;

/* BFS through the portal graph from from_idx up to max_depth hops.
 * Returns a bitset (one bool per cluster) of reachable clusters.
 * max_depth=1 gives the camera cluster + its immediate portal neighbours.
 */
template<typename Clusters>
inline std::vector<bool> reachable_set(
    Clusters const& clusters,
    u32             from_idx,
    u32             max_depth = std::numeric_limits<u32>::max())
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
                i32 adj = (portal.front_cluster() == static_cast<i16>(ci))
                              ? portal.back_cluster()
                              : portal.front_cluster();
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
 * Sutherland–Hodgman style, into a caller-provided buffer so the
 * per-frame portal walk never allocates. */
inline void clip_polygon(
    std::vector<Vecf3> const& poly, Vecf4 const& plane, std::vector<Vecf3>& out)
{
    out.clear();
    int n = static_cast<int>(poly.size());
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
}

/* Screen-space rectangle in NDC */
struct rect_t
{
    f32 x0, y0, x1, y1;
};

/* Reusable buffers for visible_set: the walk clips polygons per
 * portal crossing (up to 5 planes each) and tracks per-cluster entry
 * rects — with per-call vectors that was ~10-20k allocations per frame
 * on slow allocators (measured as multi-ms p95 spikes on Cortex-A8).
 * Own one of these per caller and hand it in every frame. */
struct scratch_t
{
    struct entry_t
    {
        u32    ci;
        u32    depth;
        rect_t rect;
    };

    std::vector<bool>                visible;
    std::vector<std::vector<rect_t>> entered;
    std::vector<entry_t>             stack;
    std::vector<Vecf3>               poly_a;
    std::vector<Vecf3>               poly_b;

    void reset(size_t num_clusters)
    {
        visible.assign(num_clusters, false);
        /* keep inner vectors' capacity, just empty them */
        if(entered.size() < num_clusters)
            entered.resize(num_clusters);
        for(auto& e : entered)
            e.clear();
        stack.clear();
    }
};

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
template<typename Clusters>
inline std::vector<bool> const& visible_set(
    Clusters const& clusters,
    u32             from_idx,
    Matf4 const&    mvp,
    scratch_t&      scratch,
    u32             max_depth = 64)
{
    using Rect = rect_t;

    scratch.reset(clusters.size());
    std::vector<bool>& visible = scratch.visible;
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
        return visible = reachable_set(clusters, from_idx, max_depth), visible;

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

    /* Per-cluster list of rects the cluster was already entered with. A
     * new path is skipped only when its rect is contained in a SINGLE
     * previous entry rect — collapsing the entries into one accumulated
     * bounding box (the previous approach) over-covers: a narrow middle
     * path can sit inside the bbox of two disjoint side paths without
     * either having actually covered it, and the portals only visible
     * through that middle strip are then never crossed (seen as far-away
     * clusters missing from the visible set). */
    constexpr Rect empty_rect{1.f, 1.f, -1.f, -1.f};
    constexpr u32  max_entries = 8;
    auto&          entered     = scratch.entered;

    Rect const full_rect{-1.f, -1.f, 1.f, 1.f};
    entered[from_idx].push_back(full_rect);

    auto& stack = scratch.stack;
    stack.push_back({from_idx, 0, full_rect});

    u32 budget = 4096; /* hard cap on portal crossings per frame */

    while(!stack.empty())
    {
        auto entry = stack.back();
        stack.pop_back();
        if(entry.depth >= max_depth)
            continue;
        auto const planes = rect_planes(entry.rect);
        for(auto const& portal : clusters[entry.ci].portals)
        {
            if(budget-- == 0)
                return visible;
            i32 adj = (portal.front_cluster() == static_cast<i16>(entry.ci))
                          ? portal.back_cluster()
                          : portal.front_cluster();
            u32 ai  = static_cast<u32>(adj);
            if(adj < 0 || ai >= clusters.size())
                continue;

            /* Clip the portal polygon to the rect's view pyramid, plus a
             * near plane slightly in front of the eye so the projection
             * below never divides by w ≤ 0. A portal the camera is about
             * to cross gets clipped at the eye and projects to roughly
             * the whole rect, which is the desired behaviour. Ping-pongs
             * between two scratch buffers, no allocation steady-state. */
            auto* in  = &scratch.poly_a;
            auto* out = &scratch.poly_b;
            clip_polygon(portal.vertices, Vecf4(Vecf3(r3), r3.w - 1e-3f), *in);
            for(auto const& plane : planes)
            {
                if(in->size() < 3)
                    break;
                clip_polygon(*in, plane, *out);
                std::swap(in, out);
            }
            auto const& poly = *in;
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

            /* Only walk on if no single previous entry into this cluster
             * already covered this rect. */
            auto& prev    = entered[ai];
            bool  covered = false;
            for(Rect const& e : prev)
                if(r.x0 >= e.x0 && r.y0 >= e.y0 && r.x1 <= e.x1 && r.y1 <= e.y1)
                {
                    covered = true;
                    break;
                }
            if(covered || prev.size() >= max_entries)
                continue;
            prev.push_back(r);

            stack.push_back({ai, entry.depth + 1, r});
        }
    }
    return visible;
}

} // namespace blam::portal_walk
