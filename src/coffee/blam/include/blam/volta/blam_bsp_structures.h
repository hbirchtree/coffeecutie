#pragma once

#include "../common/collision_bsp.h"
#include "../common/tag_class.h"
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

using collision_bsp::ray_hit;

/*! The encodings collision_bsp walks */
struct traits
{
    using node_t    = bsp_3d;
    using plane_t   = plane;
    using surface_t = surface;
    using edge_t    = edge;
    using child_t   = collision_bsp::child_t;

    /* Sign bit set = leaf (index = child & 0x7fffffff); -1 = solid */
    static inline child_t child(i32 c)
    {
        if(c == -1)
            return {child_t::none};
        if(c < 0)
            return {child_t::leaf, static_cast<u32>(c) & 0x7fffffffu};
        return {child_t::node, static_cast<u32>(c)};
    }

    static inline i32 plane_of(node_t const& n)
    {
        return from_le(n.plane);
    }

    static inline child_t front(node_t const& n)
    {
        return child(from_le(n.front));
    }

    static inline child_t back(node_t const& n)
    {
        return child(from_le(n.back));
    }

    static inline Vecf3 normal(plane_t const& p)
    {
        return {from_le(p.plane.x), from_le(p.plane.y), from_le(p.plane.z)};
    }

    static inline f32 distance(plane_t const& p)
    {
        return from_le(p.d);
    }

    static inline bool in_front(plane_t const& p, Vecf3 const& point)
    {
        return glm::dot(normal(p), point) >= distance(p);
    }

    static inline i32 first_edge(surface_t const& s)
    {
        return from_le(s.first_edge);
    }

    static inline i32 start_vertex(edge_t const& e)
    {
        return from_le(e.start_vertex);
    }

    static inline i32 end_vertex(edge_t const& e)
    {
        return from_le(e.end_vertex);
    }

    static inline i32 forward_edge(edge_t const& e)
    {
        return from_le(e.forward_edge);
    }

    static inline i32 reverse_edge(edge_t const& e)
    {
        return from_le(e.reverse_edge);
    }

    static inline i32 left_surface(edge_t const& e)
    {
        return from_le(e.left_surface);
    }
};

/*! See collision_bsp::walk_surface() */
template<typename Visitor>
inline u32 walk_surface(
    semantic::Span<surface const> surfaces,
    semantic::Span<edge const>    edges,
    u32                           surface_index,
    Visitor&&                     visit,
    u32                           max_corners = 64)
{
    return collision_bsp::walk_surface<traits>(
        surfaces, edges, surface_index, std::forward<Visitor>(visit), max_corners);
}

template<heap_t Heap>
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

    /* The leaf containing point; nullopt for solid/outside-the-map points
     * and on malformed data */
    inline std::optional<u32> find_leaf(
        Vecf3 const& point, ptr const& magic) const
    {
        auto nodes_  = nodes_3d.data(magic);
        auto planes_ = planes.data(magic);
        if(nodes_.has_error() || planes_.has_error())
            return std::nullopt;
        auto node_span = nodes_.value();
        return collision_bsp::find_leaf<traits>(
            node_span, planes_.value(), point, node_span.size());
    }

    /* See collision_bsp::raycast(). Plane-level precision: gives hit
     * point/normal/plane; surface + material resolution via the leaf's 2D
     * BSPs is a later refinement. */
    inline std::optional<ray_hit> raycast(
        Vecf3 const& start, Vecf3 const& end, ptr const& magic) const
    {
        auto nodes_  = nodes_3d.data(magic);
        auto planes_ = planes.data(magic);
        if(nodes_.has_error() || planes_.has_error())
            return std::nullopt;
        return collision_bsp::raycast<traits>(nodes_.value(), planes_.value(), start, end);
    }
};

using bsp       = bsp_t<heap_t::bsp>; /* structure BSP collision */
using model_bsp = bsp_t<heap_t::map>; /* coll tag node geometry */

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
        return {{map_magic.base + off, len}, from_le(magic)};
    }

    inline section const& to_bsp(bsp_ptr const& magic) const
    {
        return *C_RCAST<section const*>(magic.base);
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
            bsp_reference<u8, xbox_t> uncompressed_vertices;
            u32                       padding4[3];
            bsp_reference<u8, xbox_t> compressed_vertices;
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

    inline bsp_reference<u8, xbox_t> vertices() const
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
                to_le<u32>(static_cast<u32>(count * sizeof(pc_vertex))),
                to_le(
                    std::max(
                        from_le(pc.uncompressed_vertices.offset),
                        from_le(pc.uncompressed_vertices.count))),
            };
        else
            return {
                to_le<u32>(static_cast<u32>(count * sizeof(xbox_vertex))),
                to_le(from_le(pc.compressed_vertices.offset)),
            };
    }

    inline bsp_reference<u8, xbox_t> light_verts() const
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
                to_le<u32>(
                    static_cast<u32>(count * sizeof(pc_light_vertex))),
                to_le(light_offset),
            };
        else
            return {
                to_le<u32>(
                    static_cast<u32>(count * sizeof(xbox_light_vertex))),
                to_le(light_offset),
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
    u16         unknown[2];
    tag_index<> tag_id;
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

/*! A surface where a fog volume meets the air; the fog is on its back side */
struct fog_plane
{
    i16   front_region; /*!< Into fog_regions */
    i16   material_type;
    Vecf3 normal;
    f32   offset;
    u32   padding2[3];
};

static_assert(sizeof(fog_plane) == 32);

struct fog_region
{
    u32 padding[9];
    i16 fog_palette;
    i16 weather_palette;
};

static_assert(sizeof(fog_region) == 40);

struct fog_palette
{
    bl_string                        name;
    tagref_typed_t<tag_class_t::fog> fog;
    u32                              padding;
    bl_string                        fog_scale_function;
    u32                              padding2[13];
};

static_assert(sizeof(fog_palette) == 136);

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
    Vecf3 position;
    i8    direction[3]; /* -127..127 */
    i8    lens_flare;   /* index into the bsp's lens_flares */
};

static_assert(sizeof(lens_flare_marker) == 16);

struct weather_palette
{
    bl_string                         name;
    tagref_typed_t<tag_class_t::rain> particle_system;
    u32                               padding;
    bl_string                         scale_function;
    u32                               padding2[11];
    tagref_typed_t<tag_class_t::wind> wind;
    Vecf3                             wind_direction;
    f32                               wind_magnitude;
    u32                               padding3;
    bl_string                         wind_scale_function;
    u32                               padding4[11];
};

static_assert(offsetof(weather_palette, wind) == 0x80);
static_assert(sizeof(weather_palette) == 240);

/*! Where weather does not fall: inside every plane */
struct weather_polyhedra
{
    struct
    {
        Vecf3 center;
        f32   radius;
    } sphere;
    u32 padding;

    struct plane
    {
        Vecf3 plane;
        f32   d;
    };

    bsp_reference<plane> planes;
};

static_assert(sizeof(weather_polyhedra) == 32);

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

/* Grass and other sprites over the ground, binned into 8-unit cells. A cell
 * holds one layer per set bit of `layers`, one per scenario detail object
 * collection, each with its own count and z reference. */
struct detail_object
{
    static constexpr f32 cell_size = 8.f;

    struct cell
    {
        i16 x, y, z;
        i16 offset_z;
        u32 layers;
        i32 first_instance;
        i32 first_count; /*!< counts/z_references index of the first layer */
        u32 padding[3];
    };

    struct instance
    {
        u8  x, y, z; /*!< x, y in 1/256 cell, z through the z reference */
        u8  data;    /*!< sprite index, running across the collection's types */
        u16 color;   /*!< baked lighting, R5G6B5 */
    };

    bsp_reference<cell>     cells;
    bsp_reference<instance> instances;
    bsp_reference<i16>      counts;
    bsp_reference<Vecf4>    z_references;
    u32                     flags;
    u32                     padding[3];

    static inline Vecf3 position(
        cell const& c, Vecf4 const& z_ref, instance const& i)
    {
        f32 const u = i.x / 256.f;
        f32 const v = i.y / 256.f;
        f32 const z = from_le(c.z) + from_le(z_ref.w) +
                      from_le(c.offset_z) / 255.f + from_le(z_ref.x) * u +
                      from_le(z_ref.y) * v + from_le(z_ref.z) * (i.z / 255.f);
        return Vecf3(from_le(c.x) + u, from_le(c.y) + v, z) * cell_size;
    }
};

static_assert(sizeof(detail_object::cell) == 32);
static_assert(sizeof(detail_object::instance) == 6);
static_assert(sizeof(detail_object) == 64);

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
    static constexpr tag_class_t tag_class = tag_class_t::sbsp;

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
    u32                                          padding0[3];
    bsp_reference<tagref_typed_t<tag_class_t::lens>> lens_flares;
    bsp_reference<lens_flare_marker>                 lens_flare_markers;
    /* Clusters contain some properties such as sky, fog, sound, weather
     * Subclusters contain surface indices
     */
    bsp_reference<cluster>                   clusters;
    i32                                  cluster_data_size;
    u32                                  padding1[4];
    bsp_reference<cluster_portal>        cluster_portals;
    u32                                  padding2[3];
    bsp_reference<breakable_surface>     breakables_surfaces;
    bsp_reference<fog_plane>         fog_planes;
    bsp_reference<fog_region>        fog_regions;
    bsp_reference<fog_palette>       fog_palettes;
    u32                                  padding3[6];
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
        surfaces.offset,
        to_le(
            static_cast<u32>(from_le(surfaces.count) * sizeof(vert::face)) +
            from_le(head.surfaces.offset)),
    };
}

} // namespace bsp
} // namespace blam
