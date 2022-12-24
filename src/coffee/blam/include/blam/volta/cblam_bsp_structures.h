#pragma once

#include "cblam_reflexive.h"
#include "cblam_shaders.h"
#include "cblam_strings.h"
#include "cblam_tag_index.h"
#include "cblam_vertex.h"

namespace blam {
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
        return {1, header_offset};
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
};

struct submesh_header
{
    using pc_vertex   = vert::vertex<vert::uncompressed>;
    using xbox_vertex = vert::vertex<vert::compressed>;

    using pc_light_vertex   = vert::light_vertex<vert::uncompressed>;
    using xbox_light_vertex = vert::light_vertex<vert::compressed>;

    struct dist_light
    {
        Vecf3 color;
        Vecf3 direction;
    };

    tagref_t                         shader;
    u32                              unknown1;
    u32                              mesh_index_offset; /*!< Mesh indices */
    u32                              mesh_index_count;
    Vecf3                            centroid;
    Vecf3                            ambient_col;
    u32                              dist_light_count;
    Array<dist_light, 2>             dist_lights;
    Vecf3                            unknown2;
    Vecf4                            reflect_tint;
    Vecf3                            shadow_dirw;
    Vecf3                            shadow_color;
    Vecf4                            plane;
    u32                              breakable_surface;
    u32                              unknown_count3;
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

    inline reflexive_t<pc_vertex, xbox_t> pc_vertices() const
    {
        auto base = pc_vertices_data;
        base.offset += pc_vertex_data_offset;
        return base;
    }

    inline reflexive_t<pc_light_vertex, xbox_t> pc_light_verts() const
    {
        reflexive_t<pc_light_vertex, xbox_t> out;
        /* Offset to vertex segment */
        out.count  = pc_vertices_data.count;
        out.offset = pc_vertices_data.offset + pc_vertex_data_offset;
        /* Skip normal vertices to find light vertices */
        out.offset += sizeof(pc_vertex) * pc_vertices_data.count;
        return out;
    }

    inline reflexive_t<xbox_light_vertex, xbox_t> xbox_light_verts() const
    {
        reflexive_t<xbox_light_vertex, xbox_t> out;
        /* Offset to vertex segment */
        out.count  = xbox_vertices_data.count;
        out.offset = xbox_vertices_data.offset + vertex_data_offset;
        /* Skip normal vertices to find light vertices */
        out.offset += sizeof(xbox_vertex) * xbox_vertices_data.count;
        return out;
    }

    inline reflexive_t<xbox_vertex, xbox_t> xbox_vertices() const
    {
        auto base = xbox_vertices_data;
        base.offset += vertex_data_offset;
        return base;
    }

    inline reflexive_t<vert::idx_t> indices(header const& head) const;

    inline u32 index_offset() const
    {
        return mesh_index_offset * sizeof(vert::face);
    }
    inline u32 base_vertex() const
    {
        return mesh_index_offset;
    }
    inline u32 index_count() const
    {
        return mesh_index_count * 3;
    }
};

struct alignas(4) submesh_group
{
    i16 lightmap_idx;
    /* Intentionally leave 2 bytes here for padding */
    u32                         unknown[4];
    reflexive_t<submesh_header> material;
};

struct subcluster
{
    bounding_box            bounds;
    reflexive_t<vert::face> indices;
};

struct predicted_resource
{
    u16 unknown[2];
    u32 tag_id;
};

struct cluster_portal
{
    i16                front_cluster;
    i16                back_cluster;
    i32                plane_index;
    Vecf3              centroid;
    scalar             bound_radius;
    u32                unknown[7];
    reflexive_t<Vecf3> vertices;
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
    u32                             unknown2[7];
    reflexive_t<i16>                portals;
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

struct header
{
    tagref_t                         lightmaps;
    u32                              unknown1[37];
    reflexive_t<shader::shader_desc> shaders;
    reflexive_t<byte_t>              collision_header;
    reflexive_t<byte_t>              nodes;
    bounding_box                     world_bounds;
    reflexive_t<byte_t>              leaves;
    reflexive_t<byte_t>              leaf_surfaces;
    reflexive_t<vert::face>          submesh_tri_indices;
    reflexive_t<submesh_group>       submesh_groups;
    reflexive_t<byte_t>              chunk_10;
    reflexive_t<byte_t>              chunk_11;
    reflexive_t<byte_t>              chunk_12;
    reflexive_t<cluster>             clusters;
    i32                              cluster_data_size;
    u32                              unknown2;
    reflexive_t<byte_t>              cluster_data;
    reflexive_t<cluster_portal>      cluster_portals;
    reflexive_t<byte_t>              chunk_16a;
    reflexive_t<breakable_surface>   breakables_surfaces;
    reflexive_t<byte_t>              fog_planes;
    reflexive_t<byte_t>              fog_regions;
    reflexive_t<byte_t>              fog_weather_pallette;
    reflexive_t<byte_t>              chunk_16f;
    reflexive_t<byte_t>              chunk_16g;
    reflexive_t<byte_t>              weather;
    reflexive_t<byte_t>              weather_polyhedra;
    reflexive_t<byte_t>              chunk_19;
    reflexive_t<byte_t>              chunk_20;
    reflexive_t<byte_t>              pathfinding_surface;
    reflexive_t<byte_t>              chunk_24;
    reflexive_t<byte_t>              background_sound;
    reflexive_t<byte_t>              sound_env;
    i32                              sound_pas_data_size;
    u32                              unknown3;
    reflexive_t<byte_t>              sound_pas_data;
    reflexive_t<byte_t>              chunk_26;
    reflexive_t<byte_t>              chunk_27;
    reflexive_t<byte_t>              markers;
    reflexive_t<byte_t>              detail_objects; /*!< aka detail_objects */
    reflexive_t<byte_t>              runtime_decals;
    u32                              unkown4[9];

    inline reflexive_t<vert::face> all_indices() const
    {
        return submesh_tri_indices;
    }
};

inline reflexive_t<vert::idx_t> submesh_header::indices(
    header const& head) const
{
    return {
        .count  = index_count(),
        .offset = index_offset() + head.submesh_tri_indices.offset,
    };
}

} // namespace bsp
} // namespace blam
