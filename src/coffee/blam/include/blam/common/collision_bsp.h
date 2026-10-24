#pragma once

#include <glm/glm.hpp>
#include <peripherals/semantic/chunk.h>
#include <peripherals/typing/vectors/vector_types.h>

#include <algorithm>
#include <optional>
#include <type_traits>
#include <vector>

/*!
 * The collision BSP every game shares: a solid-leaf 3D tree over planes,
 * and surfaces bounded by winged edges. Only the encodings differ, so the
 * walks take a traits type per game:
 *
 *   using node_t, plane_t, surface_t, edge_t;
 *   static i32     plane_of(node_t const&);      // < 0 when none
 *   static child_t front(node_t const&), back(node_t const&);
 *   static bool    in_front(plane_t const&, Vecf3 const&);
 *   static Vecf3   normal(plane_t const&);
 *   static f32     distance(plane_t const&);
 *   static i32     first_edge(surface_t const&);
 *   static i32     start_vertex(edge_t const&), end_vertex(edge_t const&),
 *                  forward_edge(edge_t const&), reverse_edge(edge_t const&),
 *                  left_surface(edge_t const&);
 *
 * in_front() stays each game's own expression so the walks round exactly
 * as they did per game.
 */
namespace blam::collision_bsp {

using libc_types::f32;
using libc_types::i32;
using libc_types::u32;
using semantic::Span;
using typing::vector_types::Vecf3;

/*! Where a node's side leads */
struct child_t
{
    enum kind_t : libc_types::u8
    {
        node,
        leaf,
        none, /*!< Solid space */
    };

    kind_t kind;
    u32    index{0};
};

/*!
 * The leaf holding a point; nullopt in solid space, on malformed data, or
 * past `max_nodes` nodes (a cycle)
 */
template<typename Tr>
inline std::optional<u32> find_leaf(
    Span<typename Tr::node_t const>  nodes,
    Span<typename Tr::plane_t const> planes,
    Vecf3 const&                     point,
    size_t                           max_nodes)
{
    u32 node = 0;
    for(size_t step = 0; step < max_nodes; step++)
    {
        if(node >= nodes.size())
            return std::nullopt;
        auto const& n     = nodes[node];
        i32 const   plane = Tr::plane_of(n);
        if(plane < 0 || static_cast<size_t>(plane) >= planes.size())
            return std::nullopt;
        child_t const child = Tr::in_front(planes[plane], point) ? Tr::front(n) : Tr::back(n);
        if(child.kind == child_t::none)
            return std::nullopt;
        if(child.kind == child_t::leaf)
            return child.index;
        node = child.index;
    }
    return std::nullopt;
}

/*! Result of a hitscan through the solid-leaf BSP */
struct ray_hit
{
    f32   t; /* fraction along start to end */
    Vecf3 normal;
    i32   plane{-1};
};

/* Subsegment classification, as bits: whether it starts and ends in solid
 * space, and whether out holds a hit (the first empty to solid crossing in
 * ray order). A crossing is where the near half ends empty and the far half
 * starts solid; the far half need not stay solid, or a ray passing through
 * a thin solid would lose its entry point. */
constexpr int ray_start_solid = 1;
constexpr int ray_end_solid   = 2;
constexpr int ray_hit_found   = 4;

template<typename Tr>
inline int raycast_r(
    Span<typename Tr::node_t const>  nodes,
    Span<typename Tr::plane_t const> planes,
    child_t                          at,
    f32                              t0,
    f32                              t1,
    Vecf3 const&                     p0,
    Vecf3 const&                     p1,
    ray_hit&                         out)
{
    if(at.kind == child_t::none)
        return ray_start_solid | ray_end_solid;
    if(at.kind == child_t::leaf)
        return 0; /* empty leaf */
    if(at.index >= nodes.size())
        return 0;
    auto const& n     = nodes[at.index];
    i32 const   plane = Tr::plane_of(n);
    if(plane < 0 || static_cast<size_t>(plane) >= planes.size())
        return 0;
    auto const& pl     = planes[plane];
    Vecf3 const normal = Tr::normal(pl);
    f32 const   dist   = Tr::distance(pl);
    f32         d0     = glm::dot(normal, p0) - dist;
    f32         d1     = glm::dot(normal, p1) - dist;
    if(d0 >= 0.f && d1 >= 0.f)
        return raycast_r<Tr>(nodes, planes, Tr::front(n), t0, t1, p0, p1, out);
    if(d0 < 0.f && d1 < 0.f)
        return raycast_r<Tr>(nodes, planes, Tr::back(n), t0, t1, p0, p1, out);
    f32     frac   = std::clamp(d0 / (d0 - d1), 0.f, 1.f);
    f32     tm     = t0 + (t1 - t0) * frac;
    Vecf3   mid    = p0 + (p1 - p0) * frac;
    child_t near_c = d0 >= 0.f ? Tr::front(n) : Tr::back(n);
    child_t far_c  = d0 >= 0.f ? Tr::back(n) : Tr::front(n);
    int     rn     = raycast_r<Tr>(nodes, planes, near_c, t0, tm, p0, mid, out);
    if(rn & ray_hit_found)
        return rn;
    int       rf   = raycast_r<Tr>(nodes, planes, far_c, tm, t1, mid, p1, out);
    int const ends = (rn & ray_start_solid) | (rf & ray_end_solid);
    if(!(rn & ray_end_solid) && (rf & ray_start_solid))
    {
        /* Empty right before the plane, solid right after: the surface,
         * ahead of anything the far half found */
        out.t      = tm;
        out.normal = d0 >= 0.f ? normal : -normal;
        out.plane  = plane;
        return ends | ray_hit_found;
    }
    return ends | (rf & ray_hit_found);
}

/*!
 * Native hitscan (Quake-style recursive segment trace through the
 * solid-leaf tree). Returns the first empty to solid boundary along
 * start to end in ray order; leading solid space is skipped, since the
 * sealed-world exterior is solid and rays may legally start there.
 */
template<typename Tr>
inline std::optional<ray_hit> raycast(
    Span<typename Tr::node_t const>  nodes,
    Span<typename Tr::plane_t const> planes,
    Vecf3 const&                     start,
    Vecf3 const&                     end)
{
    if(nodes.empty() || planes.empty())
        return std::nullopt;
    ray_hit hit{};
    if(raycast_r<Tr>(nodes, planes, {child_t::node, 0}, 0.f, 1.f, start, end, hit) & ray_hit_found)
        return hit;
    return std::nullopt;
}

/*!
 * Walks a surface's edge loop, calling `visit(vertex_index)` for each
 * corner in order. Returns the corner count, 0 on a broken loop; corners
 * before the break have been visited.
 */
template<typename Tr, typename Visitor>
inline u32 walk_surface(
    Span<typename Tr::surface_t const> surfaces,
    Span<typename Tr::edge_t const>    edges,
    u32                                surface,
    Visitor&&                          visit,
    u32                                max_corners = 64)
{
    if(surface >= surfaces.size())
        return 0;
    i32 const first = Tr::first_edge(surfaces[surface]);
    i32       e     = first;
    u32       count = 0;
    do
    {
        if(e < 0 || static_cast<size_t>(e) >= edges.size())
            return 0;
        auto const& edge = edges[e];
        if(Tr::left_surface(edge) == static_cast<i32>(surface))
        {
            visit(Tr::start_vertex(edge));
            e = Tr::forward_edge(edge);
        } else
        {
            visit(Tr::end_vertex(edge));
            e = Tr::reverse_edge(edge);
        }
        count++;
    } while(e != first && count < max_corners);
    return e == first ? count : 0;
}

/*!
 * Fans every surface into triangles, `emit(a, b, c)` with vertex indices
 * (or `emit(a, b, c, surface)`).
 * A surface is left out when `skip(surface_index)` says so, or when a
 * corner is not in `usable` (vertices at infinity bound the BSP). A broken
 * loop still fans the corners before the break.
 */
template<typename Tr, typename Skip, typename Emit>
inline void fan_surfaces(
    Span<typename Tr::surface_t const> surfaces,
    Span<typename Tr::edge_t const>    edges,
    std::vector<bool> const&           usable,
    Skip&&                             skip,
    Emit&&                             emit)
{
    std::vector<i32> polygon;
    for(u32 s = 0; s < surfaces.size(); s++)
    {
        if(skip(s))
            continue;
        polygon.clear();
        bool ok = true;
        walk_surface<Tr>(surfaces, edges, s, [&](i32 vertex) {
            if(vertex >= 0 && static_cast<size_t>(vertex) < usable.size() && usable[vertex])
                polygon.push_back(vertex);
            else
                ok = false;
        });
        if(!ok)
            continue;
        for(size_t i = 2; i < polygon.size(); i++)
            if constexpr(std::is_invocable_v<Emit, i32, i32, i32, u32>)
                emit(polygon[0], polygon[i - 1], polygon[i], s);
            else
                emit(polygon[0], polygon[i - 1], polygon[i]);
    }
}

} // namespace blam::collision_bsp
