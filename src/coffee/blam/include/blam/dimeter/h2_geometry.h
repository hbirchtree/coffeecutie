#pragma once

#include "h2_structures.h"

#include <cstring>

namespace blam::dimeter::geo {

/*!
 * Halo 2 render geometry (mode + sbsp) lives in per-section "geometry
 * blocks": raw_ref'd blobs holding a small header, the index buffer, and
 * per-attribute vertex streams, described by a list of resource records.
 * Layouts cross-referenced against Reclaimer's Halo2 readers
 * (render_model.cs / scenario_structure_bsp.config.cs / Halo2Common.cs),
 * Xbox variant only.
 */

/*! Describes one stream within a section's geometry block. Offsets are
 * relative to the section's base address (see model_section_t /
 * bsp_section_t).
 *
 * Decoded semantics (verified empirically across Xbox+Vista maps): type0 is
 * the FIELD OFFSET of the corresponding tag_block within the authoring-time
 * section-data struct, and type1 is that block's element size (or, for the
 * vertex-buffers field at offset 56, the vertex buffer index). For BSP
 * cluster data: (0,72) = parts, (8,8) = subparts, (32,2) = strip indices,
 * (48,2) = MOPP reorder, (56,n) = vertex buffer n (0 = position,
 * 1 = texcoord, 2 = normal); mode sections add (100,1) = node map and
 * (68,68)/(84,4)/(92,2) prt-related blocks. The authoring "visibility
 * bounds" block (16,20) is stripped from all shipped caches. */
struct alignas(4) resource_t
{
    u32 type_flags;
    i16 type0_raw;
    i16 type1_raw;
    i32 size_raw;
    i32 offset_raw;

    inline i16 type0() const
    {
        return from_le(type0_raw);
    }

    inline i16 type1() const
    {
        return from_le(type1_raw);
    }

    inline i32 size() const
    {
        return from_le(size_raw);
    }

    inline i32 offset() const
    {
        return from_le(offset_raw);
    }
};

static_assert(sizeof(resource_t) == 16);

/*! Submesh table entry inside the geometry block: one draw range per
 * shader */
struct alignas(2) submesh_t
{
    i16 unknown_0[2];
    i16 shader_index_raw;
    u16 index_start_raw;
    u16 index_length_raw;
    u8  padding[62];

    inline i16 shader_index() const
    {
        return from_le(shader_index_raw);
    }

    inline u16 index_start() const
    {
        return from_le(index_start_raw);
    }

    inline u16 index_length() const
    {
        return from_le(index_length_raw);
    }
};

static_assert(offsetof(submesh_t, shader_index_raw) == 4);
static_assert(sizeof(submesh_t) == 72);

enum class classification_t : i16
{
    worldspace  = 0,
    rigid       = 1,
    rigid_boned = 2,
    skinned     = 3,
};

/*! render_model (mode) section, elem in the tag's sections block */
struct alignas(4) model_section_t
{
    classification_t classification;
    u16              unknown_0;
    u16              vertex_count_raw;
    u16              face_count_raw;
    u8               unknown_1[12];
    u8               nodes_per_vertex;
    u8               unknown_2[35];
    raw_ref          data;
    i32              data_size_raw;
    i32              unknown_3;
    i32              header_size_raw;
    tag_block<resource_t> resources;
    u8               unknown_4[12];

    inline u16 vertex_count() const
    {
        return from_le(vertex_count_raw);
    }

    inline u16 face_count() const
    {
        return from_le(face_count_raw);
    }

    inline i32 data_size() const
    {
        return from_le(data_size_raw);
    }

    /*! Resource offsets are relative to this position within the block */
    inline i32 base_address() const
    {
        return data_size() - from_le(header_size_raw) - 4;
    }
};

static_assert(offsetof(model_section_t, nodes_per_vertex) == 20);
static_assert(offsetof(model_section_t, data) == 56);
static_assert(offsetof(model_section_t, header_size_raw) == 68);
static_assert(offsetof(model_section_t, resources) == 72);
static_assert(sizeof(model_section_t) == 92);

/*! Vertex/texcoord compression bounds for mode sections; Xbox vertex
 * streams store i16-normalized values inside these ranges */
struct alignas(4) model_bounds_t
{
    f32 x_min, x_max;
    f32 y_min, y_max;
    f32 z_min, z_max;
    f32 u_min, u_max;
    f32 v_min, v_max;
    u8  padding[16];
};

static_assert(sizeof(model_bounds_t) == 56);

struct alignas(4) model_region_t
{
    string_id name;
    u32       unknown_0;
    tag_block<u8> permutations; /* elem 16: name sid, 6× lod section idx */
};

static_assert(sizeof(model_region_t) == 16);

/*! Shader table entry of mode/sbsp (32 bytes on Xbox); submesh
 * shader_index values index this table */
struct alignas(4) shader_ref_t
{
    u8            unknown_0[8];
    tag_reference shader;
    u8            unknown_1[16];
};

static_assert(offsetof(shader_ref_t, shader) == 8);
static_assert(sizeof(shader_ref_t) == 32);

/*! shad runtime property: precomputed basics so the diffuse map is
 * reachable without evaluating the shader template */
struct alignas(4) shad_runtime_property_t
{
    tag_reference diffuse;
    tag_reference lightmap_emissive;
    u8            unknown_0[0x40];
};

static_assert(sizeof(shad_runtime_property_t) == 0x50);

/*! Bitmap slot in the shad postprocess properties; short tag reference
 * (datum only, no class) */
struct alignas(4) shader_map_t
{
    datum_index bitmap;
    u8          unknown_0[8];
};

static_assert(sizeof(shader_map_t) == 12);

struct alignas(4) vec4_t
{
    f32 x, y, z, w;
};

/*! shad postprocess ("shader properties"): the compiled form of the
 * authoring-time parameters. Bitmaps[i] pairs with TilingData entries;
 * for base maps slot/tile 0 is the usual arrangement. */
struct alignas(4) shader_properties_t
{
    datum_index shader_template;
    tag_block<shader_map_t> bitmaps;
    u8 unknown_0[8];
    tag_block<vec4_t> tiling; /* 0x14; xy = uv scale */
    u8 unknown_1[96];
};

static_assert(offsetof(shader_properties_t, tiling) == 0x14);
static_assert(sizeof(shader_properties_t) == 124);

/*! shad tag body prefix (Xbox: the authoring-time parameter block is
 * stripped; runtime properties + postprocess survive) */
struct alignas(4) shad_header_t
{
    tag_reference shader_template;
    string_id     material_name;
    tag_block<shad_runtime_property_t> runtime_properties;
    u32           flags;
    tag_block<u8> parameters; /* empty in shipped Xbox caches */
    tag_block<shader_properties_t> properties; /* 0x20 */
};

static_assert(offsetof(shad_header_t, runtime_properties) == 0x0C);
static_assert(offsetof(shad_header_t, properties) == 0x20);

/*! render_model (mode) tag body prefix; only the geometry-relevant blocks */
struct alignas(4) model_header_t
{
    string_id name;
    u32       flags;
    u8        unknown_0[12];
    tag_block<model_bounds_t>   bounds;   /* 0x14 */
    tag_block<model_region_t>   regions;  /* 0x1C */
    tag_block<model_section_t>  sections; /* 0x24 */
    u8 unknown_1[52];
    tag_block<shader_ref_t>     shaders;  /* 0x60 */
};

static_assert(offsetof(model_header_t, bounds) == 0x14);
static_assert(offsetof(model_header_t, sections) == 0x24);
static_assert(offsetof(model_header_t, shaders) == 96);

/*! sbsp cluster / section geometry, shared field layout (cluster elem is
 * 176 bytes, bsp section elem 200; both start identically) */
struct alignas(4) bsp_section_common_t
{
    u16 vertex_count_raw;
    u16 face_count_raw;
    u8  unknown_0[20];
    tag_block<u8> bounds;
    u8  unknown_1[8];
    raw_ref data;
    i32 data_size_raw;
    i32 header_size_raw;
    i32 unknown_2;
    tag_block<resource_t> resources;

    inline u16 vertex_count() const
    {
        return from_le(vertex_count_raw);
    }

    inline u16 face_count() const
    {
        return from_le(face_count_raw);
    }

    inline i32 data_size() const
    {
        return from_le(data_size_raw);
    }

    inline i32 base_address() const
    {
        return from_le(header_size_raw) + 8;
    }
};

static_assert(offsetof(bsp_section_common_t, data) == 40);
static_assert(offsetof(bsp_section_common_t, resources) == 56);

struct alignas(4) bsp_cluster_t : bsp_section_common_t
{
    u8 unknown_3[112];
};

static_assert(sizeof(bsp_cluster_t) == 176);

struct alignas(4) bsp_geometry_section_t : bsp_section_common_t
{
    u8 unknown_3[136];
};

static_assert(sizeof(bsp_geometry_section_t) == 200);

/*! Instanced geometry placement: which bsp section, where. Carries a
 * world-space bounding sphere (verified empirically: center tracks the
 * placed geometry, radius matches its extent) — this is the per-instance
 * culling volume the engine pairs with the owning cluster's portal
 * visibility. */
struct alignas(4) bsp_instance_t
{
    f32       scale;
    f32       transform[12]; /* 4x3 row-major rotation+translation */
    i16       section_index_raw;
    u16       flags;
    f32       unknown_0;
    f32       sphere_center[3]; /* 0x3C, world space */
    f32       sphere_radius;    /* 0x48 */
    u32       checksum;
    string_id name;
    u8        unknown_1[4];

    inline i16 section_index() const
    {
        return from_le(section_index_raw);
    }
};

static_assert(offsetof(bsp_instance_t, section_index_raw) == 52);
static_assert(offsetof(bsp_instance_t, sphere_center) == 0x3C);
static_assert(offsetof(bsp_instance_t, sphere_radius) == 0x48);
static_assert(offsetof(bsp_instance_t, name) == 80);
static_assert(sizeof(bsp_instance_t) == 88);

/*! sbsp tag body, geometry-relevant fields only (Xbox offsets) */
struct alignas(4) bsp_header_t
{
    u8 unknown_0[52];
    f32 x_min, x_max;
    f32 y_min, y_max;
    f32 z_min, z_max;
    u8 unknown_1[80];
    tag_block<bsp_cluster_t> clusters; /* 156 */
    tag_block<shader_ref_t>  shaders;  /* 164 */
    u8 unknown_2[140];
    tag_block<bsp_geometry_section_t> sections; /* 312 */
    tag_block<bsp_instance_t>         instances; /* 320 */
};

static_assert(offsetof(bsp_header_t, clusters) == 156);
static_assert(offsetof(bsp_header_t, shaders) == 164);
static_assert(offsetof(bsp_header_t, sections) == 312);
static_assert(offsetof(bsp_header_t, instances) == 320);

/*! scnr structure-bsp entry: BSP tag meta lives in its own file region
 * with its own virtual base, unlike every other tag. Pointers inside it
 * translate as file = meta_address + (vaddr - magic), and the sbsp tag
 * body sits at file offset meta_address + 16. */
struct alignas(4) structure_bsp_ref_t
{
    u32           meta_address_raw; /*!< File offset of the BSP region */
    i32           size_raw;
    u32           magic_raw;        /*!< Virtual address of the region */
    u32           unknown_0;
    tag_reference bsp;
    u8            unknown_1[44];

    inline u32 meta_address() const
    {
        return from_le(meta_address_raw);
    }

    inline u32 magic() const
    {
        return from_le(magic_raw);
    }
};

static_assert(offsetof(structure_bsp_ref_t, bsp) == 16);
static_assert(sizeof(structure_bsp_ref_t) == 68);

/*! The one piece of scnr needed for geometry: the structure bsps block at
 * offset 528 (Xbox) */
struct alignas(4) scnr_bsps_t
{
    u8 unknown_0[528];
    tag_block<structure_bsp_ref_t> structure_bsps;
};

static_assert(offsetof(scnr_bsps_t, structure_bsps) == 528);

/*! Draw range within a section, one per shader ("part" in the section
 * data); streamed as resource (0,72) */
struct alignas(4) part_t
{
    i16 type_raw; /*!< 0 not drawn, 1 shadow only, 2 opaque shadow-casting,
                       3 opaque non-shadowing, 4 transparent, 5 lightmap */
    u16 flags;
    i16 material_index_raw;
    u16 strip_start_raw;
    u16 strip_length_raw;
    i16 first_subpart_raw;
    i16 subpart_count_raw;
    u8  node_info[2];
    f32 centroid[3]; /*!< The only spatial hint shipped per part */
    u8  node_indices[4];
    f32 node_weights[3];
    f32 lod_mipmap_magic;
    u8  unknown_0[24];
};

static_assert(offsetof(part_t, centroid) == 0x10);
static_assert(sizeof(part_t) == 72);

/*! Sub-draw-range of a part; streamed as resource (8,8). The authoring
 * pipeline pairs each subpart with a bounding sphere for fine-grained
 * culling, but shipped caches strip the sphere block and write -1 here —
 * runtime visibility on Xbox is portal clipping only, so bounds for
 * chunk-level culling must be derived from the vertex data. */
struct alignas(2) subpart_t
{
    i16 index_start_raw;
    i16 index_length_raw;
    i16 visibility_bounds_index_raw; /*!< -1 in every shipped cache */
    i16 part_index_raw;
};

static_assert(sizeof(subpart_t) == 8);

/*! Geometry block header ('blkh'), prefixed to every section blob */
struct alignas(4) block_header_t
{
    bl_tag magic; /*!< 'blkh' */
    u32    unknown_0;
    u16    part_count_raw;
    u8     unknown_1[2];
    u16    subpart_count_raw;
    u8     unknown_2[26];
    u16    index_count_raw; /*!< strip/list u16 index total, offset 40 */
    u8     unknown_3[54];
    f32    centroid[3]; /*!< offset 96; roughly the section center */
    u32    unknown_4;
};

static_assert(offsetof(block_header_t, part_count_raw) == 8);
static_assert(offsetof(block_header_t, index_count_raw) == 40);
static_assert(offsetof(block_header_t, centroid) == 96);
static_assert(sizeof(block_header_t) == 112);

/*! Geometry block header: index count for the section's index buffer
 * (u16 strip or list indices) at offset 40 on Xbox */
inline u16 block_index_count(Span<const byte_t> block)
{
    if(block.size() < 42)
        return 0;
    u16 count;
    std::memcpy(&count, block.data() + 40, 2);
    return from_le(count);
}

} // namespace blam::dimeter::geo
