#pragma once

#include "../common/collision_bsp.h"
#include "structures.h"

#include <array>

namespace blam::dimeter::coll {

struct plane_t;
struct bsp2d_reference_t;
struct edge_t;
struct surface_t;
struct vertex_t;

/*!
 * Halo 2 collision BSP: the same winged-edge solid BSP as Halo 1's, with
 * 16-bit indices. Found in the sbsp body (whole BSP, world space) and in
 * every instanced geometry definition (definition space). Layout from
 * Assembly's Halo2Xbox sbsp.xml.
 */

struct alignas(2) bsp3d_node_t
{
    block_index<plane_t> plane;
    /* 24-bit, high bit set = leaf, all ones = solid. Back first, like
     * Halo 1; Assembly's XML labels them the other way round */
    std::array<u8, 3> back_child;
    std::array<u8, 3> front_child;

    static inline i32 child(std::array<u8, 3> const& bytes)
    {
        u32 v = bytes[0] | (bytes[1] << 8) | (bytes[2] << 16);
        return v == 0xFFFFFF ? -1 : static_cast<i32>(v);
    }

    static inline bool is_leaf(i32 child)
    {
        return child >= 0 && (child & 0x800000);
    }

    static inline u32 leaf_index(i32 child)
    {
        return static_cast<u32>(child & 0x7FFFFF);
    }
};

static_assert(sizeof(bsp3d_node_t) == 8);

struct alignas(4) plane_t
{
    Vecf3 normal;
    f32   distance;
};

static_assert(sizeof(plane_t) == 16);

enum class leaf_flags_t : u8
{
    none                           = 0x0,
    contains_double_sided_surfaces = 0x1,
};
C_FLAGS(leaf_flags_t, u8);

struct alignas(2) leaf_t
{
    leaf_flags_t                   flags;
    u8                             bsp2d_reference_count;
    block_index<bsp2d_reference_t> first_bsp2d_reference;
};

static_assert(sizeof(leaf_t) == 4);

struct alignas(2) bsp2d_reference_t
{
    block_index<plane_t> plane;
    i16                  bsp2d_node_raw;
};

static_assert(sizeof(bsp2d_reference_t) == 4);

struct alignas(4) bsp2d_node_t
{
    Vecf3 plane;
    i16   left_child_raw;
    i16   right_child_raw;
};

static_assert(sizeof(bsp2d_node_t) == 16);

enum class surface_flags_t : u8
{
    none      = 0x0,
    two_sided = 0x1,
    invisible = 0x2,
    climbable = 0x4,
    breakable = 0x8,
    invalid   = 0x10,
    conveyor  = 0x20,
};
C_FLAGS(surface_flags_t, u8);

struct alignas(2) surface_t
{
    i16                 plane_raw;
    block_index<edge_t> first_edge;
    surface_flags_t     flags;
    u8                  breakable_surface_index;
    i16                 material_raw;

    inline i16 material() const
    {
        return from_le(material_raw);
    }

    inline bool has(surface_flags_t flag) const
    {
        return (static_cast<u8>(flags) & static_cast<u8>(flag)) != 0;
    }
};

static_assert(sizeof(surface_t) == 8);

struct alignas(2) edge_t
{
    block_index<vertex_t>  start_vertex;
    block_index<vertex_t>  end_vertex;
    block_index<edge_t>    forward_edge;
    block_index<edge_t>    reverse_edge;
    block_index<surface_t> left_surface;
    block_index<surface_t> right_surface;
};

static_assert(sizeof(edge_t) == 12);

struct alignas(4) vertex_t
{
    Vecf3               point;
    block_index<edge_t> first_edge;
    i16                 unknown_0;
};

static_assert(sizeof(vertex_t) == 16);

/*! The encodings collision_bsp walks */
struct traits
{
    using node_t    = bsp3d_node_t;
    using plane_t   = coll::plane_t;
    using surface_t = coll::surface_t;
    using edge_t    = coll::edge_t;
    using child_t   = collision_bsp::child_t;

    static inline child_t child(std::array<u8, 3> const& bytes)
    {
        i32 const c = bsp3d_node_t::child(bytes);
        if(c < 0)
            return {child_t::none};
        if(bsp3d_node_t::is_leaf(c))
            return {child_t::leaf, bsp3d_node_t::leaf_index(c)};
        return {child_t::node, static_cast<u32>(c)};
    }

    static inline i32 plane_of(node_t const& n)
    {
        return n.plane.value();
    }

    static inline child_t front(node_t const& n)
    {
        return child(n.front_child);
    }

    static inline child_t back(node_t const& n)
    {
        return child(n.back_child);
    }

    static inline Vecf3 normal(plane_t const& p)
    {
        return {from_le(p.normal[0]), from_le(p.normal[1]), from_le(p.normal[2])};
    }

    static inline f32 distance(plane_t const& p)
    {
        return from_le(p.distance);
    }

    static inline bool in_front(plane_t const& plane, Vecf3 const& p)
    {
        f32 d = from_le(plane.normal[0]) * p.x + from_le(plane.normal[1]) * p.y +
                from_le(plane.normal[2]) * p.z - from_le(plane.distance);
        return d >= 0;
    }

    static inline i32 first_edge(surface_t const& s)
    {
        return s.first_edge.value();
    }

    static inline i32 start_vertex(edge_t const& e)
    {
        return e.start_vertex.value();
    }

    static inline i32 end_vertex(edge_t const& e)
    {
        return e.end_vertex.value();
    }

    static inline i32 forward_edge(edge_t const& e)
    {
        return e.forward_edge.value();
    }

    static inline i32 reverse_edge(edge_t const& e)
    {
        return e.reverse_edge.value();
    }

    static inline i32 left_surface(edge_t const& e)
    {
        return e.left_surface.value();
    }
};

/*! The leaf holding the point; nullopt in solid space or on malformed data */
inline std::optional<u32> find_leaf(
    Span<bsp3d_node_t const> nodes,
    Span<plane_t const>      planes,
    f32                      x,
    f32                      y,
    f32                      z)
{
    return collision_bsp::find_leaf<traits>(nodes, planes, Vecf3(x, y, z), 4096);
}

/*! Lives in the BSP region for sbsp and instanced geometry, in tag data for
 * collision models; the heap says which pointer resolves it */
template<heap_t Heap>
struct alignas(4) bsp_t
{
    template<typename T>
    using block = tag_block<T, Heap>;

    block<bsp3d_node_t>      bsp3d_nodes;
    block<plane_t>           planes;
    block<leaf_t>            leaves;
    block<bsp2d_reference_t> bsp2d_references;
    block<bsp2d_node_t>      bsp2d_nodes;
    block<surface_t>         surfaces;
    block<edge_t>            edges;
    block<vertex_t>          vertices;

    /*! See find_leaf() */
    inline std::optional<u32> find_leaf(
        f32 x, f32 y, f32 z, magic_ptr_base<Heap> const& magic) const
    {
        auto nodes  = bsp3d_nodes.data(magic);
        auto planes = this->planes.data(magic);
        if(nodes.has_error() || planes.has_error())
            return std::nullopt;
        return coll::find_leaf(nodes.value(), planes.value(), x, y, z);
    }
};

static_assert(sizeof(bsp_t<heap_t::bsp>) == 64);

/*! See collision_bsp::walk_surface() */
template<typename Visitor>
inline u32 walk_surface(
    Span<surface_t const> surfaces,
    Span<edge_t const>    edges,
    u32                   surface_index,
    Visitor&&             visit)
{
    return collision_bsp::walk_surface<traits>(
        surfaces, edges, surface_index, std::forward<Visitor>(visit));
}

} // namespace blam::dimeter::coll
