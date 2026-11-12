#pragma once

#include "caching/caching_item.h"

#include <peripherals/typing/vectors/vector_types.h>

#include <optional>
#include <unordered_map>
#include <vector>

/* The walkable collision surfaces of a BSP section as a graph: a node per
 * surface flat enough to stand on, a link where two share an edge or a
 * step no higher than a knee stands between them. Routes are found over
 * the nodes and pulled tight across the edges they cross */
struct NavMesh
{
    using Vecf3 = typing::vector_types::Vecf3;
    using Vecf2 = typing::vector_types::Vecf2;
    using f32   = libc_types::f32;
    using u32   = libc_types::u32;
    using i32   = libc_types::i32;
    using i64   = libc_types::i64;

    /* The edge crossed into `to`, its ends as seen walking there */
    struct link_t
    {
        u32   to;
        Vecf3 left;
        Vecf3 right;
    };

    struct node_t
    {
        Vecf3 center;
        Vecf3 normal;
        f32   d; /* plane: dot(normal, p) == d */
        u32   first_vertex;
        u32   vertex_count;
        u32   first_link;
        u32   link_count;
    };

    std::vector<node_t> nodes;
    std::vector<Vecf3>  vertices;
    std::vector<link_t> links;

    bool empty() const
    {
        return nodes.empty();
    }

    void clear();
    void build(BSPItem const& item);

    /* The node under a pair of feet, if any */
    std::optional<u32> locate(Vecf3 const& feet) const;

    /* Points to walk through from `from` to reach `to`, `to` last; empty
     * when either end is off the mesh or no way joins them */
    std::vector<Vecf3> route(Vecf3 const& from, Vecf3 const& to, u32 max_expansions = 4000) const;

    /* A world unit is about three metres */
    static constexpr f32 min_normal_z = .7f;  /* a floor: no steeper than 45 degrees */
    static constexpr f32 step_height  = .15f; /* wu a riser may rise and be stepped over */
    static constexpr f32 body_radius  = .25f; /* wu a route keeps off an edge's ends, and the walls they meet */
    static constexpr f32 cell_size    = 4.f;  /* wu, the lookup grid */
    static constexpr f32 feet_below   = .25f; /* wu a floor may lie over the feet and still be theirs */
    static constexpr f32 feet_above   = 1.f;  /* wu a floor may lie under the feet */

  private:
    static i64 cell_of(f32 x, f32 y);
    std::vector<u32> const* cell(i64 key) const;
    bool contains(node_t const& node, Vecf2 const& at) const;
    f32  height_at(node_t const& node, Vecf2 const& at) const;

    std::unordered_map<i64, std::vector<u32>> m_grid;
};
