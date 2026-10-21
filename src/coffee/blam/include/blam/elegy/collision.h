#pragma once

#include "../common/collision_bsp.h"
#include "resources.h"

#include <optional>

namespace blam::elegy::coll {

/*!
 * \brief A node of the solid-leaf BSP: front child, back child and plane
 * packed into 64 bits. A child with bit 23 set is a leaf, all ones none.
 */
struct bsp3d_node_t
{
    be<u64> raw;

    inline u16 plane() const
    {
        return raw.value() & 0xFFFF;
    }

    inline u32 back_child() const
    {
        return (raw.value() >> 16) & 0xFFFFFF;
    }

    inline u32 front_child() const
    {
        return (raw.value() >> 40) & 0xFFFFFF;
    }

    static inline bool is_none(u32 child)
    {
        return child == 0xFFFFFF;
    }

    static inline bool is_leaf(u32 child)
    {
        return (child & 0x800000) != 0;
    }

    static inline u32 leaf_index(u32 child)
    {
        return child & 0x7FFFFF;
    }
};

static_assert(sizeof(bsp3d_node_t) == 8);

struct plane_t
{
    be<Vecf3> normal;
    be<f32>   distance;
};

static_assert(sizeof(plane_t) == 16);

enum class leaf_flags_t : u16
{
    none                           = 0x0,
    contains_double_sided_surfaces = 0x1,
};
C_FLAGS(leaf_flags_t, u16);

/*! A leaf: a run of bsp2d references, one per surface plane it touches */
struct leaf_t
{
    be<leaf_flags_t> flags;
    be<u16>          bsp2d_reference_count;
    be<u32>          first_bsp2d_reference;
};

static_assert(sizeof(leaf_t) == 8);

/*! Plane (bit 15: flipped) and the bsp2d tree on it (bit 15: a surface) */
struct bsp2d_reference_t
{
    be<u16> plane;
    be<u16> bsp2d_node;
};

static_assert(sizeof(bsp2d_reference_t) == 4);

/*! 2D split (a, b, d) with children; bit 15 of a child: a surface */
struct bsp2d_node_t
{
    be<Vecf3> plane;
    be<u16>   left_child;
    be<u16>   right_child;
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

/*! A convex polygon, its corners an edge loop from first_edge */
struct surface_t
{
    be<u16>         plane;
    be<u16>         first_edge;
    be<i16>         material;
    be<i16>         breakable_surface_set;
    be<i16>         breakable_surface;
    surface_flags_t flags;
    u8              best_plane_vertex;
};

static_assert(sizeof(surface_t) == 12);

/*! Winged edge: next edges around the surface on either side */
struct edge_t
{
    be<u16> start_vertex;
    be<u16> end_vertex;
    be<u16> forward_edge;
    be<u16> reverse_edge;
    be<u16> left_surface;
    be<u16> right_surface;
};

static_assert(sizeof(edge_t) == 12);

struct vertex_t
{
    be<Vecf3> point;
    be<u16>   first_edge;
    be<u16>   sink;
};

static_assert(sizeof(vertex_t) == 16);

/*!
 * \brief collision_bsp, inside resources: the world's (structure bsp tag
 * resources) and every instanced geometry definition's. The arrays live in
 * the page data.
 */
struct collision_bsp_t
{
    resource_block<bsp3d_node_t>      bsp3d_nodes;
    resource_block<plane_t>           planes;
    resource_block<leaf_t>            leaves;
    resource_block<bsp2d_reference_t> bsp2d_references;
    resource_block<bsp2d_node_t>      bsp2d_nodes;
    resource_block<surface_t>         surfaces;
    resource_block<edge_t>            edges;
    resource_block<vertex_t>          vertices;
};

static_assert(sizeof(collision_bsp_t) == 0x60);

/*! Havok MOPP code over a collision bsp's surfaces */
struct mopp_code_t
{
    u32                runtime_0;
    be<i16>            size;
    be<i16>            count;
    be<u32>            user_data;
    be<i32>            type;
    be<Vecf4>          offset; /*!< xyz origin, w the quantisation scale */
    u32                runtime_1;
    be<i32>            data_size;
    be<u32>            data_capacity;
    u32                runtime_2;
    resource_block<u8> data;
    u32                runtime_3;
};

static_assert(offsetof(mopp_code_t, offset) == 0x10);
static_assert(offsetof(mopp_code_t, data) == 0x30);
static_assert(sizeof(mopp_code_t) == 0x40);

/*! A collision surface's render triangles: a run of the mapping below */
struct structure_surface_t
{
    be<i16> first_triangle_mapping;
    be<i16> triangle_mapping_count;
};

static_assert(sizeof(structure_surface_t) == 4);

struct surface_triangle_t
{
    be<i16> triangle;
    be<i16> section;
};

static_assert(sizeof(surface_triangle_t) == 4);

/*! The encodings collision_bsp walks */
struct traits
{
    using node_t    = bsp3d_node_t;
    using plane_t   = coll::plane_t;
    using surface_t = coll::surface_t;
    using edge_t    = coll::edge_t;
    using child_t   = collision_bsp::child_t;

    static inline child_t child(u32 c)
    {
        if(bsp3d_node_t::is_none(c))
            return {child_t::none};
        if(bsp3d_node_t::is_leaf(c))
            return {child_t::leaf, bsp3d_node_t::leaf_index(c)};
        return {child_t::node, c};
    }

    static inline i32 plane_of(node_t const& n)
    {
        return n.plane();
    }

    static inline child_t front(node_t const& n)
    {
        return child(n.front_child());
    }

    static inline child_t back(node_t const& n)
    {
        return child(n.back_child());
    }

    static inline Vecf3 normal(plane_t const& p)
    {
        return p.normal.value();
    }

    static inline f32 distance(plane_t const& p)
    {
        return p.distance.value();
    }

    static inline bool in_front(plane_t const& plane, Vecf3 const& p)
    {
        f32 side = glm::dot(plane.normal.value(), p) - plane.distance.value();
        return side >= 0.f;
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

/*! A collision bsp resolved from its resource */
struct bsp_view_t
{
    Span<bsp3d_node_t const>      nodes;
    Span<plane_t const>           planes;
    Span<leaf_t const>            leaves;
    Span<bsp2d_reference_t const> bsp2d_references;
    Span<bsp2d_node_t const>      bsp2d_nodes;
    Span<surface_t const>         surfaces;
    Span<edge_t const>            edges;
    Span<vertex_t const>          vertices;

    static inline bsp_view_t from(resource_view const& view, collision_bsp_t const& bsp)
    {
        return {
            .nodes            = view.block(bsp.bsp3d_nodes),
            .planes           = view.block(bsp.planes),
            .leaves           = view.block(bsp.leaves),
            .bsp2d_references = view.block(bsp.bsp2d_references),
            .bsp2d_nodes      = view.block(bsp.bsp2d_nodes),
            .surfaces         = view.block(bsp.surfaces),
            .edges            = view.block(bsp.edges),
            .vertices         = view.block(bsp.vertices),
        };
    }

    /*! The leaf holding a point, nullopt in solid space or on bad data */
    std::optional<u32> find_leaf(Vecf3 const& p) const;

    /*! See collision_bsp::walk_surface() */
    template<typename Visitor>
    inline u32 walk_surface(u32 surface, Visitor&& visit) const
    {
        return collision_bsp::walk_surface<traits>(surfaces, edges, surface, std::forward<Visitor>(visit));
    }
};

} // namespace blam::elegy::coll
