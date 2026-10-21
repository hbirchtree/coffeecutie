#pragma once

#include "../common/vertex_pack.h"
#include "resources.h"

#include <vector>

namespace blam::elegy {

enum class vertex_type_t : u8
{
    world          = 0,
    rigid          = 1,
    skinned        = 2,
    particle_model = 3,
    flat_world     = 4,
    flat_rigid     = 5,
    flat_skinned   = 6,
    screen         = 7,
    debug          = 8,
    transparent    = 9,
    particle       = 10,
    contrail       = 11,
    light_volume   = 12,
    chud_simple    = 13,
    chud_fancy     = 14,
    decorator      = 15,
    position_only  = 16,
    patchy_fog     = 17,
    water          = 18,
    ripple         = 19,
    implicit       = 20,
    beam           = 21,
};

enum class primitive_t : u8
{
    point_list     = 0,
    line_list      = 1,
    line_strip     = 2,
    triangle_list  = 3,
    triangle_fan   = 4,
    triangle_strip = 5,
    quad_list      = 6,
};

enum class mesh_flags_t : u8
{
    none                      = 0x0,
    has_vertex_color          = 0x1,
    region_index_for_sorting  = 0x2,
    vertex_buffers_as_indices = 0x4,
    per_instance_lighting     = 0x8,
    unindexed                 = 0x10,
};
C_FLAGS(mesh_flags_t, u8);

enum class part_flags_t : u8
{
    none                       = 0x0,
    dislikes_photons           = 0x1,
    ignored_by_lightmapper     = 0x2,
    has_transparent_sort_plane = 0x4,
    water_surface              = 0x8,
};
C_FLAGS(part_flags_t, u8);

enum class part_type_t : i8
{
    not_drawn            = 0,
    opaque_shadow_only   = 1,
    opaque_shadow_casting = 2,
    opaque_nonshadowing  = 3,
    transparent          = 4,
    lightmap_only        = 5, /*!< Light emitters for the bake */
};

/*! A range of a mesh's index buffer drawn with one material */
struct part_t
{
    be<i16>      material;
    be<i16>      transparent_sorting;
    be<u16>      index_start;
    be<u16>      index_count;
    be<i16>      subpart_start;
    be<u16>      subpart_count;
    part_type_t  type;
    part_flags_t flags;
    be<u16>      budget_vertex_count;

    /*! Whether it is seen; the rest only shadows or lights the bake */
    inline bool drawn() const
    {
        return type == part_type_t::opaque_shadow_casting ||
               type == part_type_t::opaque_nonshadowing || type == part_type_t::transparent;
    }
};

static_assert(sizeof(part_t) == 0x10);

struct subpart_t
{
    be<u16>                index_start;
    be<u16>                index_count;
    block_index<part_t>    part;
    be<u16>                budget_vertex_count;
};

static_assert(sizeof(subpart_t) == 0x8);

struct instance_bucket_t
{
    be<i16>                  mesh;
    be<i16>                  definition;
    tag_block<be<i16>>       instances;
};

static_assert(sizeof(instance_bucket_t) == 0x10);

/*!
 * \brief One draw's worth of geometry. Up to 8 vertex streams by index
 * into the resource's vertex buffers: stream 0 holds the vertices, 1 the
 * lightmap/PRT data, 2 vertex colors.
 */
struct mesh_t
{
    tag_block<part_t>            parts;
    tag_block<subpart_t>         subparts;
    std::array<be<i16>, 8>       vertex_buffers;
    std::array<be<i16>, 2>       index_buffers;
    mesh_flags_t                 flags;
    i8                           rigid_node;
    vertex_type_t                vertex_type;
    u8                           prt_type;
    primitive_t                  index_type;
    std::array<u8, 3>            unknown_0;
    tag_block<instance_bucket_t> instance_buckets;
    tag_block<be<i16>>           water_indices;
};

static_assert(offsetof(mesh_t, vertex_buffers) == 0x18);
static_assert(offsetof(mesh_t, flags) == 0x2C);
static_assert(offsetof(mesh_t, instance_buckets) == 0x34);
static_assert(sizeof(mesh_t) == 0x4C);

enum class compression_flags_t : u16
{
    none                = 0x0,
    compressed_position = 0x1,
    compressed_texcoord = 0x2,
    optimized           = 0x4,
};
C_FLAGS(compression_flags_t, u16);

/*!
 * \brief Bounds the 16-bit unsigned normalized positions and texcoords of
 * rigid and skinned vertices expand into.
 */
struct compression_t
{
    be<compression_flags_t> flags;
    u16                     unknown_0;
    std::array<bounds_f, 3> position;
    std::array<bounds_f, 2> texcoord;

    /*! [0, 1] to the bounds */
    static inline f32 expand(f32 unorm, bounds_f const& b)
    {
        return b.min + unorm * (b.max - b.min);
    }
};

static_assert(sizeof(compression_t) == 0x2C);

/*!
 * \brief render_geometry, embedded in mode, sbsp and sLdT. The vertex and
 * index buffers are in the resource it names.
 */
/*! An instance's lightmap texcoords: a static_per_pixel vertex buffer */
struct instance_lightmap_texcoords_t
{
    tag_block<std::array<u8, 0x60>> texcoords;
    be<i16>                         vertex_buffer;
    u16                             unknown_0;
};

static_assert(sizeof(instance_lightmap_texcoords_t) == 0x10);

struct render_geometry_t
{
    be<u32>                               flags;
    tag_block<mesh_t>                     meshes;
    tag_block<compression_t>              compression;
    tag_block<std::array<u8, 0x30>>       part_sorting_positions;
    tag_block<std::array<u8, 0x18>>       user_data;
    std::array<u32, 3>                    per_mesh_temporary;
    tag_block<std::array<u8, 0x20>>       per_mesh_mopp;
    tag_block<tag_block<u8>>              per_mesh_node_map; /*!< Mesh node index -> model node */
    tag_block<std::array<u8, 0xC>>        per_mesh_subpart_visibility;
    tag_block<std::array<u8, 0x20>>       per_mesh_prt;
    tag_block<instance_lightmap_texcoords_t> per_instance_lightmap_texcoords;
    datum_index                           resource;
    u32                                   unknown_0;
};

static_assert(offsetof(render_geometry_t, compression) == 0x10);
static_assert(offsetof(render_geometry_t, resource) == 0x7C);
static_assert(sizeof(render_geometry_t) == 0x84);

enum class vertex_format_t : i16
{
    invalid                 = 0,
    world                   = 1,
    rigid                   = 2,
    skinned                 = 3,
    static_per_pixel        = 4, /*!< Lightmap texcoord */
    static_per_vertex       = 6,
    ambient_prt             = 9,
    linear_prt              = 10,
    quadratic_prt           = 11,
    static_per_vertex_color = 14,
    tiny_position           = 20,
    decorator               = 24,
    particle_model          = 25,
    world2                  = 30,
};

struct vertex_buffer_definition_t
{
    be<i32>             count;
    be<vertex_format_t> format;
    be<i16>             stride;
    data_reference      data;
};

static_assert(sizeof(vertex_buffer_definition_t) == 0x1C);

struct index_buffer_definition_t
{
    be<i32>        type; /*!< primitive_t */
    data_reference data;

    inline primitive_t primitive() const
    {
        return static_cast<primitive_t>(type.value());
    }
};

static_assert(sizeof(index_buffer_definition_t) == 0x18);

/*! render_geometry_api_resource_definition, the resource's root */
struct render_geometry_api_t
{
    resource_block<u8> unknown_0;
    resource_block<u8> unknown_1;
    resource_block<d3d_structure<vertex_buffer_definition_t>> vertex_buffers;
    resource_block<d3d_structure<index_buffer_definition_t>>  index_buffers;
};

static_assert(sizeof(render_geometry_api_t) == 0x30);

/*! Uncompressed BSP vertex */
struct world_vertex_t
{
    be<Vecf3> position;
    be<Vecf2> texcoord;
    be<u32>   normal;
    be<u32>   tangent;
    be<u32>   binormal;
};

static_assert(sizeof(world_vertex_t) == 0x20);

/*! Object vertex, position and texcoord expanded through compression_t */
struct rigid_vertex_t
{
    std::array<be<u16>, 4> position;
    std::array<be<u16>, 2> texcoord;
    be<u32>                normal;
    be<u32>                tangent;
    be<u32>                binormal;
};

static_assert(sizeof(rigid_vertex_t) == 0x18);

/*! Decorator mesh vertex: the rigid layout without its tangent frame */
struct decorator_vertex_t
{
    std::array<be<u16>, 4> position;
    std::array<be<u16>, 2> texcoord;
    be<u32>                normal;
};

static_assert(sizeof(decorator_vertex_t) == 0x10);

struct skinned_vertex_t
{
    rigid_vertex_t    base;
    std::array<u8, 4> nodes;
    std::array<u8, 4> weights;
};

static_assert(sizeof(skinned_vertex_t) == 0x20);

/*! Signed normalized 10:11:11 vector (D3DDECLTYPE_DHEN3N), x in the low
 * bits */
inline Vecf3 unpack_dhen3n(u32 v)
{
    return vertex_pack::unpack_10_11_11(v);
}

inline f32 unpack_unorm16(u16 v)
{
    return static_cast<f32>(v) / 65535.f;
}

/*! A vertex of any format, expanded */
struct vertex_t
{
    Vecf3              position;
    Vecf2              texcoord;
    Vecf3              normal;
    Vecf3              tangent;
    Vecf3              binormal;
    std::array<u8, 4>  nodes{};
    Vecf4              weights{};
};

/*!
 * \brief Lightmap texcoords from a static_per_pixel vertex buffer: two
 * unsigned 16-bit normalized values per vertex. Empty if the buffer is
 * missing or of another format.
 */
std::vector<Vecf2> lightmap_texcoords(resource_view const& view, i16 vertex_buffer);

/*! Baked light at a vertex: linear SH (D3DX order) and dominant intensity */
struct vertex_light_t
{
    std::array<Vecf3, 4> sh;
    Vecf3                intensity;
};

/*!
 * \brief Per-vertex baked light from a static_per_vertex buffer: five
 * signed 8-bit RGBE values (exponent first, scale 2^(e * 31.75)), the
 * dominant light intensity then the four SH coefficients.
 */
std::vector<vertex_light_t> vertex_lights(resource_view const& view, i32 vertex_buffer);

/*!
 * \brief A mesh's buffers, located in its geometry resource
 */
struct mesh_buffers
{
    vertex_buffer_definition_t const* vertices{nullptr};
    Span<u8 const>                vertex_bytes;
    index_buffer_definition_t const*  indices{nullptr};
    Span<u8 const>                index_bytes;

    static inline mesh_buffers from(
        resource_view const& view, mesh_t const& mesh)
    {
        mesh_buffers out;
        auto const* api = view.root_as<render_geometry_api_t>();
        if(!api)
            return out;
        auto vbs = view.block(api->vertex_buffers);
        auto ibs = view.block(api->index_buffers);
        i16  vb  = mesh.vertex_buffers[0];
        i16  ib  = mesh.index_buffers[0];
        if(vb >= 0 && static_cast<size_t>(vb) < vbs.size())
            if((out.vertices = view.resolve(vbs[static_cast<size_t>(vb)])))
                out.vertex_bytes = view.bytes(out.vertices->data);
        if(ib >= 0 && static_cast<size_t>(ib) < ibs.size())
            if((out.indices = view.resolve(ibs[static_cast<size_t>(ib)])))
                out.index_bytes = view.bytes(out.indices->data);
        return out;
    }

    inline u32 vertex_count() const
    {
        return vertices ? static_cast<u32>(vertices->count.value()) : 0;
    }

    /*! Unindexed meshes (decorators) draw their vertices in order */
    inline u32 index_count() const
    {
        return indices ? static_cast<u32>(index_bytes.size() / 2) : vertex_count();
    }

    inline u16 index(u32 i) const
    {
        if(!indices)
            return static_cast<u16>(i);
        u16 out;
        std::memcpy(&out, index_bytes.data() + i * 2, 2);
        return from_be(out);
    }

    /*!
     * Expand the vertex stream. Rigid and skinned vertices need the mesh's
     * compression bounds; world vertices are stored as-is.
     */
    bool decode(
        compression_t const* compression, std::vector<vertex_t>& out) const;

    /*! Triangles of a part (or the whole buffer), strips unrolled */
    void triangles(
        primitive_t                       type,
        u32                               start,
        u32                               count,
        std::vector<std::array<u16, 3>>& out) const;
};

} // namespace blam::elegy
