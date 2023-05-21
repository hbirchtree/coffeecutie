#pragma once

#include "blam_reflexive.h"
#include "blam_shaders.h"
#include "blam_strings.h"
#include "blam_tag_index.h"
#include "blam_vertex.h"

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
    u16     first_bsp_2d_reference;
};

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
    u16 plane;
    u16 first_edge;
    enum flags_t : u16
    {
        none      = 0x0,
        two_sided = 0x1,
        invisible = 0x2,
        climbable = 0x4,
        breakable = 0x8,
    };
    flags_t flags;
    u16     breakable_surface;
    i16     material;
};

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
    u16   first_edge;
};

struct bsp
{
    reflexive_t<bsp_3d>      nodes_3d;
    reflexive_t<plane>       planes;
    reflexive_t<leaf>        leaves;
    reflexive_t<bsp_2d_ref>  bsp_2d_refs;
    reflexive_t<bsp_2d_node> bsp_2d_nodes;
    reflexive_t<surface>     surfaces;
    reflexive_t<edge>        edges;
    reflexive_t<vertex>      vertices;
};

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
    reflexive_t<comp_vertex, xbox_t>   xbox_vertices;
    reflexive_t<comp_lightmap, xbox_t> xbox_lightmaps;
    bl_tag                             tag;

    /*!
     * \brief to_header
     * Uses bsp_magic()
     * \return
     */
    inline reflexive_t<header, xbox_t> to_header() const
    {
        return {1, {header_offset}};
    }
};

struct info
{
    u32      offset;
    u32      size;
    u32      magic;
    u32      zero;
    tagref_t tag;

    inline magic_data_t bsp_magic(magic_data_t const& map_magic) const
    {
        return {{map_magic.base_ptr, map_magic.max_size}, magic - offset};
    }

    inline section const& to_bsp(magic_data_t const& magic) const
    {
        return *C_RCAST<section const*>(magic.base_ptr + offset);
    }

    inline std::optional<header const*> to_header(
        magic_data_t const& magic) const
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

    tagref_t                        shader;
    u16                             shader_permutation;
    flags_t                         flags; /*!< Mesh indices */
    reflexive_t<vert::face, xbox_t> surfaces;
    Vecf3                           centroid;
    Vecf3                           ambient_col;
    u32                             dist_light_count;
    std::array<dist_light, 2>       dist_lights;
    Vecf4                           reflect_tint;
    Vecf3                           shadow_dirw;
    Vecf3                           shadow_color;
    Vecf4                           plane;
    i32                             breakable_surface;
    u32                             padding1;
    union
    {
        struct
        {
            u32                         garbage[3];
            u32                         count;
            u32                         padding1[4];
            u32                         count2;
            u32                         padding2[3];
            u32                         something;
            u32                         padding3;
            reflexive_t<byte_t, xbox_t> uncompressed_vertices;
            u32                         padding4[3];
            reflexive_t<byte_t, xbox_t> compressed_vertices;
        } pc;
        struct
        {
        } xbox;
        struct
        {
            u32                              pad[3];
            reflexive_t<pc_vertex, xbox_t>   pc_vertices_data;
            u32                              memory_vertex_offset;
            u32                              vert_reflexive;
            u32                              unknown_always_3;
            reflexive_t<xbox_vertex, xbox_t> xbox_vertices_data;
            u32                              memory_lightmap_offset;
            u32                              lightmap_vert_reflexive;
            u32                              unknown_zero[2];
            u32                              unknown_offset1;
            u32                              pc_vertex_data_offset;
            u32                              unkown_zero1;
            u32                              comp_vertex_buffer_size;
            u32                              unkown_zero2;
            u32                              unknown_offset2;
            u32                              vertex_data_offset;
            u32                              unkown_zero3;
        } old;
        u32 all[22];
    };

    C_DEPRECATED_S("not how you get vertices")
    inline reflexive_t<pc_vertex, xbox_t> pc_vertices() const
    {
        //        reflexive_t<pc_vertex, xbox_t> base = pc_vertices_data;
        //        base.offset += pc_vertex_data_offset;
        //        return base;
        return {
            .count  = pc.count,
            .offset = pc.uncompressed_vertices.offset,
        };
    }

    inline reflexive_t<pc_light_vertex, xbox_t> pc_light_verts() const
    {
        //        reflexive_t<pc_light_vertex, xbox_t> out;
        //        /* Offset to vertex segment */
        //        out.count  = pc_vertices_data.count;
        //        out.offset = pc_vertices_data.offset + pc_vertex_data_offset;
        //        /* Skip normal vertices to find light vertices */
        //        out.offset += sizeof(pc_vertex) * pc_vertices_data.count;
        //        return out;
        return {
            .count  = pc.count,
            .offset = static_cast<u32>(
                pc.uncompressed_vertices.offset + pc.count * sizeof(pc_vertex)),
        };
    }

    inline reflexive_t<xbox_light_vertex, xbox_t> xbox_light_verts() const
    {
        //        reflexive_t<xbox_light_vertex, xbox_t> out;
        //        /* Offset to vertex segment */
        //        out.count  = xbox_vertices_data.count;
        //        out.offset = xbox_vertices_data.offset + vertex_data_offset;
        //        /* Skip normal vertices to find light vertices */
        //        out.offset += sizeof(xbox_vertex) * xbox_vertices_data.count;
        //        return out;
        return {};
    }

    inline reflexive_t<xbox_vertex, xbox_t> xbox_vertices() const
    {
        //        auto base = xbox_vertices_data;
        //        base.offset += vertex_data_offset;
        //        return base;
        return {};
    }

    C_DEPRECATED_S("not how you get indices")
    inline reflexive_t<vert::idx_t> indices(header const& head) const;
};

using node = bounding_box;

struct predicted_resource
{
    u16 unknown[2];
    u32 tag_id;
};

struct subcluster
{
    bounding_box     bounds;
    reflexive_t<u32> indices; /* Points into surfaces on header */
};

struct mirror
{
    Vecf3                             plane;
    f32                               d;
    tagref_typed_t<tag_class_t::shdr> shader;
    reflexive_t<Vecf3>                vertices;
};

struct cluster
{
    i16                             sky;
    i16                             fog;
    i16                             background_sound;
    i16                             sound_env;
    i16                             weather;
    i16                             transition_bsp;
    u32                             unknown1[7];
    reflexive_t<predicted_resource> predicted_resources;
    reflexive_t<subcluster>         sub_clusters;
    u16                             first_lens_flare_marker;
    u16                             lens_flare_marker_count;
    reflexive_t<vert::idx_t>        surface_indices;
    reflexive_t<mirror>             mirrors;
    reflexive_t<i16>                portals;
};

struct cluster_portal
{
    i16                front_cluster;
    i16                back_cluster;
    i32                plane_index;
    Vecf3              centroid;
    f32                bound_radius;
    u32                unknown[7];
    reflexive_t<Vecf3> vertices;
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
    u16                     cluster;
    u16                     surface_reference_count;
    reflexive_t<vert::face> surfaces;
};

struct leaf_surface
{
    u32 surface;
    u32 node;
};

struct alignas(4) lightmap
{
    i16 lightmap_idx;
    /* Intentionally leave 2 bytes here for padding */
    u32                   unknown[4];
    reflexive_t<material> materials;
};

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
    reflexive_t<plane> planes;
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
    bl_string                        name;
    tagref_typed_t<tag_class_t::snd> bg_sound;
    bl_string                        scale_function;
};

struct sound_environment_palette
{
    bl_string                         name;
    tagref_typed_t<tag_class_t::snde> environment;
};

struct marker
{
    bl_string                   name;
    typing::vector_types::Quatf rotation;
    Vecf3                       position;
};

struct detail_object
{
};

struct runtime_decals
{
};

struct leaf_map_leaf
{
    struct face
    {
        u16                node_index;
        reflexive_t<Vecf2> vertices;
    };
    reflexive_t<face> faces;
    reflexive_t<u16>  portal_indices;
};

struct leaf_map_portal
{
    i32                plane_idx;
    i32                back_leaf;
    i32                front_leaf;
    reflexive_t<Vecf3> vertices;
};

struct header
{
    tagref_t                         lightmap_; /* Lightmap texture(s) */
    f32                              vehicle_floor;
    f32                              vehicle_ceiling;
    u32                              unknown1[35];
    reflexive_t<shader::shader_desc> collision_materials;
    reflexive_t<collision::bsp>      collision_header;
    /* Volumes in world-space where leaf surfaces reside */
    reflexive_t<node> nodes;
    bounding_box      world_bounds;
    reflexive_t<leaf> leaves;
    /* Grouping of surfaces, each one a pair of surfaces and node */
    reflexive_t<leaf_surface>                      leaf_surfaces;
    reflexive_t<vert::face>                        surfaces;
    reflexive_t<lightmap>                          lightmaps;
    reflexive_t<tagref_typed_t<tag_class_t::lens>> lens_flares;
    reflexive_t<lens_flare_marker>                 lens_flare_markers;
    u32                                            padding0[3];
    /* Clusters contain some properties such as sky, fog, sound, weather
     * Subclusters contain surface indices
     */
    reflexive_t<cluster>                   clusters;
    i32                                    cluster_data_size;
    u32                                    padding1[4];
    reflexive_t<cluster_portal>            cluster_portals;
    u32                                    padding2[3];
    reflexive_t<breakable_surface>         breakables_surfaces;
    reflexive_t<byte_t>                    fog_planes;
    reflexive_t<byte_t>                    fog_regions;
    u32                                    padding3[9];
    reflexive_t<weather_palette>           weather_palettes;
    reflexive_t<weather_polyhedra>         weather_polyhedras;
    u32                                    padding4[6];
    reflexive_t<pathfinding_surface>       pathfinding_surfaces;
    reflexive_t<pathfinding_edge>          pathfinding_edges;
    reflexive_t<background_sound_palette>  background_sound;
    reflexive_t<sound_environment_palette> sound_env;
    i32                                    sound_pas_data_size;
    u32                                    padding5[10];
    reflexive_t<marker>                    markers;
    reflexive_t<detail_object>             detail_objects;
    reflexive_t<byte_t>                    runtime_decals;
    reflexive_t<leaf_map_leaf>             leaf_map_leaves;
    reflexive_t<leaf_map_portal>           leaf_map_portals;
    u32                                    unkown4[3];

    inline reflexive_t<vert::face> all_indices() const
    {
        return surfaces;
    }
};
static_assert(offsetof(header, surfaces) == 248);
static_assert(offsetof(header, lightmaps) == 260);
static_assert(offsetof(header, clusters) == 308);
static_assert(offsetof(header, cluster_portals) == 340);
static_assert(sizeof(header) == 648);

inline reflexive_t<vert::idx_t> material::indices(header const& head) const
{
    //    return {
    //        .count  = index_count(),
    //        .offset = index_offset() + head.surfaces.offset,
    //    };
    return {
        .count  = surfaces.offset * 3u,
        .offset = static_cast<u32>(surfaces.count * sizeof(vert::face))
                  + head.surfaces.offset,
    };
}

} // namespace bsp
} // namespace blam
