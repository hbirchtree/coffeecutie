#pragma once

#include "blam_reference.h"
#include "blam_shaders.h"
#include "blam_strings.h"
#include "blam_tag_index.h"
#include "blam_vertex.h"

#include <algorithm>
#include <optional>

namespace blam {
namespace collision {

struct bsp_3d
{
    i32 plane;
    i32 back;
    i32 front;
};

struct plane
{
    Vecf3 plane;
    f32   d;
};

struct leaf
{
    enum flags_t : u16
    {
        none                  = 0x0,
        contains_double_sided = 0x1,
    };

    flags_t flags;
    u16     reference_count;
    u32     first_bsp_2d_reference;
};

static_assert(sizeof(leaf) == 8);

struct bsp_2d_ref
{
    i32 plane;
    i32 node;
};

struct bsp_2d_node
{
    Vecf3 plane;
    i32   left_child;
    i32   right_child;
};

struct surface
{
    i32 plane; /* sign bit set = surface plane is the BSP plane flipped */
    i32 first_edge;

    enum flags_t : u8
    {
        none      = 0x0,
        two_sided = 0x1,
        invisible = 0x2,
        climbable = 0x4,
        breakable = 0x8,
    };

    flags_t flags;
    u8      breakable_surface;
    i16     material;
};

static_assert(sizeof(surface) == 12);

struct edge
{
    i32 start_vertex;
    i32 end_vertex;
    i32 forward_edge;
    i32 reverse_edge;
    i32 left_surface;
    i32 right_surface;
};

struct vertex
{
    Vecf3 point;
    i32   first_edge;
};

static_assert(sizeof(vertex) == 16);

/* Result of a hitscan through the solid-leaf BSP */
struct ray_hit
{
    f32   t; /* fraction along start→end */
    Vecf3 normal;
    i32   plane{-1};
};

template<ptr_tag Heap>
struct bsp_t
{
    template<typename T>
    using ref = reference<T, grbx_t, atlas_type_t::map_file, Heap>;
    using ptr = map_ptr_base<Heap>;

    ref<bsp_3d>      nodes_3d;
    ref<plane>       planes;
    ref<leaf>        leaves;
    ref<bsp_2d_ref>  bsp_2d_refs;
    ref<bsp_2d_node> bsp_2d_nodes;
    ref<surface>     surfaces;
    ref<edge>        edges;
    ref<vertex>      vertices;

    /* Walk the solid-leaf tree from the root down to the leaf containing
     * point. Children with the sign bit set are leaves
     * (index = child & 0x7fffffff); -1 means solid space. Returns nullopt
     * for solid/outside-the-map points and on malformed data. */
    inline std::optional<u32> find_leaf(
        Vecf3 const& point, ptr const& magic) const
    {
        auto nodes_  = nodes_3d.data(magic);
        auto planes_ = planes.data(magic);
        if(nodes_.has_error() || planes_.has_error())
            return std::nullopt;
        auto node_span  = nodes_.value();
        auto plane_span = planes_.value();
        if(node_span.empty() || plane_span.empty())
            return std::nullopt;
        i32 node = 0;
        for(size_t guard = 0; guard <= node_span.size(); guard++)
        {
            if(node == -1)
                return std::nullopt; /* solid space */
            if(node < 0)
                return static_cast<u32>(node) & 0x7fffffffu;
            if(static_cast<u32>(node) >= node_span.size())
                return std::nullopt;
            auto const& n      = node_span[node];
            i32 const   nplane = from_le(n.plane);
            if(nplane < 0 || static_cast<u32>(nplane) >= plane_span.size())
                return std::nullopt;
            auto const& pl = plane_span[nplane];
            // collision node/plane fields are little-endian map data.
            Vecf3 const pn{
                from_le(pl.plane.x), from_le(pl.plane.y), from_le(pl.plane.z)};
            node = glm::dot(pn, point) >= from_le(pl.d) ? from_le(n.front)
                                                        : from_le(n.back);
        }
        return std::nullopt;
    }

    /* Native hitscan (Quake-style recursive segment trace through the
     * solid-leaf tree). Returns the first empty→solid boundary along
     * start→end in ray order; leading solid space is skipped, since the
     * sealed-world exterior is solid and rays may legally start there.
     * Plane-level precision: gives hit point/normal/plane; surface +
     * material resolution via the leaf's 2D BSPs is a later refinement. */
    inline std::optional<ray_hit> raycast(
        Vecf3 const& start, Vecf3 const& end, ptr const& magic) const
    {
        auto nodes_  = nodes_3d.data(magic);
        auto planes_ = planes.data(magic);
        if(nodes_.has_error() || planes_.has_error())
            return std::nullopt;
        auto node_span  = nodes_.value();
        auto plane_span = planes_.value();
        if(node_span.empty() || plane_span.empty())
            return std::nullopt;
        ray_hit hit{};
        if(raycast_r(node_span, plane_span, 0, 0.f, 1.f, start, end, hit) &
           ray_hit_found)
            return hit;
        return std::nullopt;
    }

    /* Subsegment classification, as bits: whether it starts and ends in
     * solid space, and whether out holds a hit (the first empty→solid
     * crossing in ray order). A crossing is where the near half ends empty
     * and the far half starts solid; the far half need not stay solid, or a
     * ray passing through a thin solid would lose its entry point. */
    static constexpr int ray_start_solid = 1;
    static constexpr int ray_end_solid   = 2;
    static constexpr int ray_hit_found   = 4;

    template<typename NodeSpan, typename PlaneSpan>
    static int raycast_r(
        NodeSpan const&  nodes,
        PlaneSpan const& planes,
        i32              node,
        f32              t0,
        f32              t1,
        Vecf3 const&     p0,
        Vecf3 const&     p1,
        ray_hit&         out)
    {
        if(node == -1)
            return ray_start_solid | ray_end_solid;
        if(node < 0)
            return 0; /* empty leaf */
        if(static_cast<u32>(node) >= nodes.size())
            return 0;
        auto const& n = nodes[node];
        if(n.plane < 0 || static_cast<u32>(n.plane) >= planes.size())
            return 0;
        auto const& pl = planes[n.plane];
        f32         d0 = glm::dot(pl.plane, p0) - pl.d;
        f32         d1 = glm::dot(pl.plane, p1) - pl.d;
        if(d0 >= 0.f && d1 >= 0.f)
            return raycast_r(nodes, planes, n.front, t0, t1, p0, p1, out);
        if(d0 < 0.f && d1 < 0.f)
            return raycast_r(nodes, planes, n.back, t0, t1, p0, p1, out);
        f32   frac   = std::clamp(d0 / (d0 - d1), 0.f, 1.f);
        f32   tm     = t0 + (t1 - t0) * frac;
        Vecf3 mid    = p0 + (p1 - p0) * frac;
        i32   near_c = d0 >= 0.f ? n.front : n.back;
        i32   far_c  = d0 >= 0.f ? n.back : n.front;
        int   rn     = raycast_r(nodes, planes, near_c, t0, tm, p0, mid, out);
        if(rn & ray_hit_found)
            return rn;
        int rf = raycast_r(nodes, planes, far_c, tm, t1, mid, p1, out);
        int const ends = (rn & ray_start_solid) | (rf & ray_end_solid);
        if(!(rn & ray_end_solid) && (rf & ray_start_solid))
        {
            /* Empty right before the plane, solid right after: the surface,
             * ahead of anything the far half found */
            out.t      = tm;
            out.normal = d0 >= 0.f ? pl.plane : -pl.plane;
            out.plane  = n.plane;
            return ends | ray_hit_found;
        }
        return ends | (rf & ray_hit_found);
    }
};

using bsp       = bsp_t<ptr_tag::bsp>; /* structure BSP collision */
using model_bsp = bsp_t<ptr_tag::map>; /* coll tag node geometry */

} // namespace collision

namespace bsp {

struct header;
struct info;

struct section
{
    using comp_vertex   = vert::vertex<vert::compressed>;
    using comp_lightmap = vert::light_vertex<vert::compressed>;

    u32 header_offset;

    /* Below values are only valid on Xbox */
    bsp_reference<comp_vertex, xbox_t>   xbox_vertices;
    bsp_reference<comp_lightmap, xbox_t> xbox_lightmaps;
    bl_tag                           tag;

    /*!
     * \brief to_header
     * Uses bsp_magic()
     * \return
     */
    inline bsp_reference<header, xbox_t> to_header() const
    {
        // count is a host literal -> store little-endian; header_offset is a
        // little-endian map field -> leave as-is (reference::data un-swaps it).
        return {to_le<u32>(1), {header_offset}};
    }
};

struct info
{
    u32      offset;
    u32      size;
    u32      magic;
    u32      zero;
    tagref_t tag;

    inline bsp_ptr bsp_magic(map_ptr const& map_magic) const
    {
        u32 const off = from_le(offset);
        u32 const len = from_le(size);
        if(off > map_magic.max_size || len > map_magic.max_size - off)
            return {};
        return {{map_magic.base_ptr + off, len}, from_le(magic)};
    }

    inline section const& to_bsp(bsp_ptr const& magic) const
    {
        return *C_RCAST<section const*>(magic.base_ptr);
    }

    inline std::optional<header const*> to_header(map_ptr const& magic) const
    {
        auto res = to_bsp(bsp_magic(magic)).to_header().data(bsp_magic(magic));
        if(!res.has_value())
            return std::nullopt;
        return res.value().data();
    }
};

struct material
{
    enum flags_t : u16
    {
        none      = 0x0,
        coplanar  = 0x1,
        fog_plane = 0x2,
    };

    using pc_vertex   = vert::vertex<vert::uncompressed>;
    using xbox_vertex = vert::vertex<vert::compressed>;

    using pc_light_vertex   = vert::light_vertex<vert::uncompressed>;
    using xbox_light_vertex = vert::light_vertex<vert::compressed>;

    struct dist_light
    {
        Vecf3 color;
        Vecf3 direction;
    };

    tagref_t                      shader;
    u16                           shader_permutation;
    flags_t                       flags; /*!< Mesh indices */
    bsp_reference<vert::face, xbox_t> surfaces;
    Vecf3                         centroid;
    Vecf3                         ambient_col;
    u32                           dist_light_count;
    std::array<dist_light, 2>     dist_lights;
    Vecf4                         reflect_tint;
    Vecf3                         shadow_dirw;
    Vecf3                         shadow_color;
    Vecf4                         plane;
    i32                           breakable_surface;
    u32                           padding1;

    union
    {
        struct
        {
            u32                       garbage[2];
            vert::vertex_type_t       type;
            u32                       count;
            u32                       padding1[4];
            u32                       count2;
            u32                       padding2[3];
            u32                       something;
            u32                       padding3;
            bsp_reference<byte_t, xbox_t> uncompressed_vertices;
            u32                       padding4[3];
            bsp_reference<byte_t, xbox_t> compressed_vertices;
        } pc;

        struct
        {
        } xbox;

        struct
        {
            u32                            pad[3];
            bsp_reference<pc_vertex, xbox_t>   pc_vertices_data;
            u32                            memory_vertex_offset;
            u32                            vert_reflexive;
            u32                            unknown_always_3;
            bsp_reference<xbox_vertex, xbox_t> xbox_vertices_data;
            u32                            memory_lightmap_offset;
            u32                            lightmap_vert_reflexive;
            u32                            unknown_zero[2];
            u32                            unknown_offset1;
            u32                            pc_vertex_data_offset;
            u32                            unkown_zero1;
            u32                            comp_vertex_buffer_size;
            u32                            unkown_zero2;
            u32                            unknown_offset2;
            u32                            vertex_data_offset;
            u32                            unkown_zero3;
        } old;

        u32 all[22];
    };

    inline bsp_reference<byte_t, xbox_t> vertices() const
    {
        //        reflexive_t<pc_vertex, xbox_t> base = pc_vertices_data;
        //        base.offset += pc_vertex_data_offset;
        //        return base;
        /* the max is there because MCC is slightly different */
        // pc.* are little-endian map fields; un-swap before arithmetic and
        // re-encode the synthesised reference little-endian for
        // reference::data.
        auto const type  = from_le(pc.type);
        auto const count = from_le(pc.count);
        if(type == vert::vertex_type_t::sbsp_uncompressed_vertex)
            return {
                .count =
                    to_le<u32>(static_cast<u32>(count * sizeof(pc_vertex))),
                .offset = to_le(
                    std::max(
                        from_le(pc.uncompressed_vertices.offset),
                        from_le(pc.uncompressed_vertices.count))),
            };
        else
            return {
                .count =
                    to_le<u32>(static_cast<u32>(count * sizeof(xbox_vertex))),
                .offset = to_le(from_le(pc.compressed_vertices.offset)),
            };
    }

    inline bsp_reference<byte_t, xbox_t> light_verts() const
    {
        //        reflexive_t<pc_light_vertex, xbox_t> out;
        //        /* Offset to vertex segment */
        //        out.count  = pc_vertices_data.count;
        //        out.offset = pc_vertices_data.offset + pc_vertex_data_offset;
        //        /* Skip normal vertices to find light vertices */
        //        out.offset += sizeof(pc_vertex) * pc_vertices_data.count;
        //        return out;
        auto const verts = vertices();
        auto const type  = from_le(pc.type);
        auto const count = from_le(pc.count);
        // light verts immediately follow the normal verts in memory; work in
        // host order then re-encode little-endian.
        u32 const light_offset = from_le(verts.offset) + from_le(verts.count);
        if(type == vert::vertex_type_t::sbsp_uncompressed_vertex)
            return {
                .count = to_le<u32>(
                    static_cast<u32>(count * sizeof(pc_light_vertex))),
                .offset = to_le(light_offset),
            };
        else
            return {
                .count = to_le<u32>(
                    static_cast<u32>(count * sizeof(xbox_light_vertex))),
                .offset = to_le(light_offset),
            };
    }

    inline u32 vertex_size() const
    {
        if(pc.type == vert::vertex_type_t::sbsp_uncompressed_vertex)
            return sizeof(pc_vertex);
        else
            return sizeof(xbox_vertex);
    }

    inline bsp_reference<vert::face> indices(header const& head) const;
};

using node = bounding_box;

struct predicted_resource
{
    u16 unknown[2];
    u32 tag_id;
};

struct subcluster
{
    bounding_box   bounds;
    bsp_reference<u32> indices; /* Points into surfaces on header */
};

struct mirror
{
    Vecf3                             plane;
    f32                               d;
    u32                               padding[5];
    tagref_typed_t<tag_class_t::shdr> shader;
    bsp_reference<Vecf3>                  vertices;
};

static_assert(sizeof(mirror) == 64);

struct cluster
{
    i16                           sky;
    i16                           fog;
    i16                           background_sound;
    i16                           sound_env;
    i16                           weather;
    i16                           transition_bsp;
    u32                           unknown1[7];
    bsp_reference<predicted_resource> predicted_resources;
    bsp_reference<subcluster>         sub_clusters;
    u16                           first_lens_flare_marker;
    u16                           lens_flare_marker_count;
    bsp_reference<vert::idx_t>        surface_indices;
    bsp_reference<mirror>             mirrors;
    bsp_reference<i16> portals; /* points into cluster_portals on header */
};

struct cluster_portal
{
    // Does this form a singly-linked list?
    i16              front_cluster;
    i16              back_cluster;
    i32              plane_index;
    Vecf3            centroid;
    f32              bound_radius;
    u32              unknown[7];
    bsp_reference<Vecf3> vertices;
};

struct breakable_surface
{
    u32 _1;
    u32 _2;
    u32 offset;
    u32 offset2;
    u32 count;
    u32 padding[7];
};

struct leaf
{
    u16 vertex_0;
    u16 vertex_1;
    u16 vertex_2;
    i16 padding;
    i16 cluster;
    u16 surface_reference_count;
    u16 surface_reference_index;
    i16 something;
};

static_assert(sizeof(leaf) == 16);

struct leaf_surface
{
    u32 surface;
    i32 node; /* May be -1, indicating no node association? */
};

static_assert(sizeof(leaf_surface) == 8);

struct alignas(4) lightmap
{
    i16 lightmap_idx;
    /* Intentionally leave 2 bytes here for padding */
    u32                 unknown[4];
    bsp_reference<material> materials;
};

static_assert(sizeof(lightmap) == 32);

struct lens_flare_marker
{
};

struct weather_palette
{
    bl_string                         name;
    tagref_typed_t<tag_class_t::part> particle_system;
    bl_string                         scale_function;
    tagref_typed_t<tag_class_t::wind> wind;
    Vecf3                             wind_direction;
    f32                               wind_magnitude;
    bl_string                         wind_scale_function;
};

struct weather_polyhedra
{
    struct
    {
        Vecf3 center;
        f32   radius;
    } sphere;

    struct plane
    {
        Vecf3 plane;
        f32   d;
    };

    bsp_reference<plane> planes;
};

struct pathfinding_surface
{
    u32 data;
};

struct pathfinding_edge
{
    f32 midpoint;
};

struct background_sound_palette
{
    bl_string                         name;
    tagref_typed_t<tag_class_t::lsnd> bg_sound;
    // Guerilla says there's a scale function here?
    u32                               padding[17];
};

struct sound_environment_palette
{
    bl_string                         name;
    tagref_typed_t<tag_class_t::snde> environment;
    u32                               padding[8];
};

static_assert(sizeof(background_sound_palette) == 116);
static_assert(sizeof(sound_environment_palette) == 80);

struct marker
{
    bl_string                   name;
    typing::vector_types::Quatf rotation;
    Vecf3                       position;
};

struct detail_object
{
};

struct runtime_decal
{
};

struct leaf_map_leaf
{
    struct face
    {
        u16              node_index;
        bsp_reference<Vecf2> vertices;
    };

    bsp_reference<face> faces;
    bsp_reference<u16>  portal_indices;
};

struct leaf_map_portal
{
    i32              plane_idx;
    i32              back_leaf;
    i32              front_leaf;
    bsp_reference<Vecf3> vertices;
};

/* What a collision surface is made of: its shader carries the physics
 * material (dirt, metal, ...) that footsteps and impacts sound like */
struct collision_material
{
    tagref_t shader;
    u32      padding;
};

static_assert(sizeof(collision_material) == 20);

struct header
{
    tagref_t                       lightmap_; /* Lightmap texture(s) */
    f32                            vehicle_floor;
    f32                            vehicle_ceiling;
    u32                            unknown1[35];
    bsp_reference<collision_material>  collision_materials;
    bsp_reference<collision::bsp>      collision_header;
    /* Volumes in world-space where leaf surfaces reside */
    bsp_reference<node> nodes;
    bounding_box    world_bounds;
    bsp_reference<leaf> leaves;
    /* Grouping of surfaces, each one a pair of surfaces and node */
    bsp_reference<leaf_surface>                      leaf_surfaces;
    bsp_reference<vert::face>                        surfaces;
    bsp_reference<lightmap>                          lightmaps;
    bsp_reference<tagref_typed_t<tag_class_t::lens>> lens_flares;
    bsp_reference<lens_flare_marker>                 lens_flare_markers;
    u32                                          padding0[3];
    /* Clusters contain some properties such as sky, fog, sound, weather
     * Subclusters contain surface indices
     */
    bsp_reference<cluster>                   clusters;
    i32                                  cluster_data_size;
    u32                                  padding1[4];
    bsp_reference<cluster_portal>        cluster_portals;
    u32                                  padding2[3];
    bsp_reference<breakable_surface>     breakables_surfaces;
    bsp_reference<byte_t>                fog_planes;
    bsp_reference<byte_t>                fog_regions;
    u32                                  padding3[9];
    bsp_reference<weather_palette>       weather_palettes;
    bsp_reference<weather_polyhedra>     weather_polyhedras;
    u32                                  padding4[6];
    bsp_reference<pathfinding_surface>   pathfinding_surfaces;
    bsp_reference<pathfinding_edge>      pathfinding_edges;
    bsp_reference<background_sound_palette> background_sound;
    bsp_reference<sound_environment_palette> sound_env;
    i32                                  sound_pas_data_size;
    u32                                  padding5[10];
    bsp_reference<marker>                    markers;
    bsp_reference<detail_object>             detail_objects;
    bsp_reference<runtime_decal>             runtime_decals;
    bsp_reference<leaf_map_leaf>             leaf_map_leaves;
    bsp_reference<leaf_map_portal>           leaf_map_portals;
    u32                                  unkown4[3];

    inline bsp_reference<vert::face> all_indices() const
    {
        return surfaces;
    }

    /* Exact point→cluster lookup: walk the collision BSP down to a leaf,
     * then map the leaf to a cluster via the render leaf array — the
     * collision leaf index space maps 1:1 onto the render leaves, whose
     * cluster field resolves to the visibility cluster. This is how the
     * original engine resolves the camera cluster. Returns nullopt for
     * solid/outside-the-map points. */
    inline std::optional<u32> cluster_for_point(
        Vecf3 const& point, bsp_ptr const& magic) const
    {
        auto coll = collision_header.data(magic, single_value);
        if(coll.has_error())
            return std::nullopt;
        auto leaf_idx = coll.value()->find_leaf(point, magic);
        if(!leaf_idx)
            return std::nullopt;
        auto leaves_ = leaves.data(magic);
        if(leaves_.has_error())
            return std::nullopt;
        auto leaf_span = leaves_.value();
        if(*leaf_idx >= leaf_span.size())
            return std::nullopt;
        i16 cluster = from_le(leaf_span[*leaf_idx].cluster);
        if(cluster < 0)
            return std::nullopt;
        return static_cast<u32>(cluster);
    }

    /* Hitscan against the structure BSP's collision tree, see
     * collision::bsp::raycast. */
    inline std::optional<collision::ray_hit> raycast(
        Vecf3 const& start, Vecf3 const& end, bsp_ptr const& magic) const
    {
        auto coll = collision_header.data(magic, single_value);
        if(coll.has_error())
            return std::nullopt;
        return coll.value()->raycast(start, end, magic);
    }
};

static_assert(offsetof(header, collision_materials) == 164);
static_assert(offsetof(header, leaves) == 224);
static_assert(offsetof(header, surfaces) == 248);
static_assert(offsetof(header, lightmaps) == 260);
static_assert(offsetof(header, clusters) == 308);
static_assert(offsetof(header, cluster_portals) == 340);
static_assert(offsetof(header, sound_env) == 0x208);
static_assert(sizeof(header) == 648);

inline bsp_reference<vert::face> material::indices(header const& head) const
{
    //    return {
    //        .count  = index_count(),
    //        .offset = index_offset() + head.surfaces.offset,
    //    };
    // surfaces.offset/count are reused as (index count, byte offset into
    // head.surfaces). count stays raw LE (reference::data un-swaps it); the
    // computed offset is host-order arithmetic re-encoded little-endian.
    return {
        .count  = surfaces.offset,
        .offset = to_le(
            static_cast<u32>(from_le(surfaces.count) * sizeof(vert::face)) +
            from_le(head.surfaces.offset)),
    };
}

} // namespace bsp
} // namespace blam
