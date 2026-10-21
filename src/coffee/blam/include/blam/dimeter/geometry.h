#pragma once

#include "../common/tag_class.h"
#include "collision.h"
#include "structures.h"

#include <array>
#include <cstring>

namespace blam::dimeter::scn {
struct sky_ref_t;
struct atmospheric_fog_t;
struct planar_fog_t;
struct tag_ref_t;
} // namespace blam::dimeter::scn

namespace blam::dimeter::obj::matg {
struct material_t;
} // namespace blam::dimeter::obj::matg

namespace blam::dimeter::geo {

struct shader_ref_t;
struct model_node_t;
struct bsp_cluster_t;
struct bsp_geometry_section_t;
struct bsp_instance_t;
struct fog_plane_t;
struct background_sound_palette_t;
struct sound_environment_palette_t;
struct weather_palette_t;
struct conveyor_surface_t;
struct environment_object_palette_t;
struct structure_bsp_ref_t;
struct part_t;
struct subpart_t;

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
 * 1 = texcoord, 2 = normal), (56,32) = the vertex buffer headers
 * (vertex_buffer_t) describing them; mode sections add (100,1) = node map
 * and (68,68)/(84,4)/(92,2) prt-related blocks. The authoring "visibility
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

/*!
 * Vertex buffer formats, from the buffer headers of every Xbox map. Compressed
 * positions are i16 x3 against the model bounds; node indices go through the
 * section's node map (straight to model nodes when it has none), weights are
 * u8 summing to ~255. The float variants with odd ids never ship on Xbox.
 */
enum class vertex_type_t : u8
{
    point                 = 0x01, /*!< f32 x3 */
    point_compressed      = 0x02, /*!< i16 x3 */
    rigid_boned           = 0x04, /*!< i16 x3, node, pad */
    skinned_2             = 0x06, /*!< i16 x3, pad x2, node x2, weight x2 */
    skinned_3             = 0x08, /*!< i16 x3, node x3, weight x3 */
    skinned_4             = 0x0A, /*!< i16 x3, pad x2, node x4, weight x4 */
    texcoord              = 0x18, /*!< f32 x2 */
    texcoord_compressed   = 0x19, /*!< i16 x2 */
    tangent_space_packed  = 0x1B, /*!< normal, binormal, tangent: u32 each */
    unknown_2f            = 0x2F, /*!< 3 bytes, scarab only */
    unknown_30            = 0x30, /*!< 4 bytes, scarab only */
};

/*! Header of one vertex buffer of a section, (56,32) */
struct alignas(1) vertex_buffer_t
{
    vertex_type_t       type;
    u8                  stride;
    std::array<u8, 30>  unknown_0;
};

static_assert(sizeof(vertex_buffer_t) == 32);

/*! Submesh table entry inside the geometry block: one draw range per
 * shader */
struct alignas(2) submesh_t
{
    i16                       unknown_0[2];
    block_index<shader_ref_t> shader;
    u16                       index_start_raw;
    u16                       index_length_raw;
    u8                        padding[62];

    inline u16 index_start() const
    {
        return from_le(index_start_raw);
    }

    inline u16 index_length() const
    {
        return from_le(index_length_raw);
    }
};

static_assert(offsetof(submesh_t, shader) == 4);
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
    classification_t          classification;
    u16                       unknown_0;
    u16                       vertex_count_raw;
    u16                       face_count_raw;
    u8                        unknown_1[12];
    u8                        nodes_per_vertex;
    u8                        unknown_2[15];
    u8                        hardware_node_count;
    u8                        node_map_size; /*!< Entries of the (100,1) map */
    u8                        unknown_2a[6];
    block_index<model_node_t> rigid_node; /* -1 unless rigid */
    u8                        unknown_2b[10];
    raw_ref                   data;
    i32                       data_size_raw;
    i32                       unknown_3;
    i32                       header_size_raw;
    tag_block<resource_t>     resources;
    u8                        unknown_4[12];

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
static_assert(offsetof(model_section_t, node_map_size) == 0x25);
static_assert(offsetof(model_section_t, rigid_node) == 0x2C);
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
    f32 u2_min, u2_max; /*!< Secondary (lightmap) texcoord */
    f32 v2_min, v2_max;
};

static_assert(sizeof(model_bounds_t) == 56);

/*! A compressed stream value inside its model_bounds_t range */
inline f32 dequantize(i16 value, f32 min, f32 max)
{
    f32 n = (static_cast<f32>(value) + 32768.f) / 65535.f;
    return min + n * (max - min);
}

/*! Section index per LOD, L1 (lowest) .. L6 (highest); -1 = absent */
struct alignas(4) model_permutation_t
{
    string_id          name;
    std::array<i16, 6> lod_section_raw;

    inline i16 lod_section(u32 lod) const
    {
        return from_le(lod_section_raw[lod]);
    }

    /*! Most detailed section present */
    inline i16 best_section() const
    {
        for(int lod = 5; lod >= 0; lod--)
            if(lod_section(lod) >= 0)
                return lod_section(lod);
        return -1;
    }
};

static_assert(sizeof(model_permutation_t) == 16);

struct alignas(4) model_region_t
{
    string_id                      name;
    u32                            unknown_0;
    tag_block<model_permutation_t> permutations;
};

static_assert(sizeof(model_region_t) == 16);

/*! Skeleton node; rigid sections are stored in their node's space */
struct alignas(4) model_node_t
{
    string_id                 name;
    block_index<model_node_t> parent;
    block_index<model_node_t> first_child;
    block_index<model_node_t> next_sibling;
    i16                       unknown_0;
    Vecf3                     position;
    Quatf                     rotation; /* x, y, z, w, no conjugate */
    f32                       inverse_scale;
    /*! Inverse of the bind pose: forward, left, up columns, then position */
    std::array<f32, 12>       inverse_transform;
    f32                       distance_from_parent;
};

static_assert(offsetof(model_node_t, position) == 12);
static_assert(offsetof(model_node_t, rotation) == 24);
static_assert(sizeof(model_node_t) == 96);

/*! Shader table entry of mode/sbsp (32 bytes on Xbox); submesh
 * shader_index values index this table */
struct alignas(4) shader_ref_t
{
    u8                               unknown_0[8];
    tag_reference<tag_class_t::shad> shader;
    u8                               unknown_1[16];
};

static_assert(offsetof(shader_ref_t, shader) == 8);
static_assert(sizeof(shader_ref_t) == 32);

/*! shad runtime property: precomputed basics so the diffuse map is
 * reachable without evaluating the shader template */
struct alignas(4) shad_runtime_property_t
{
    tag_reference<tag_class_t::bitm> diffuse;
    tag_reference<tag_class_t::bitm> lightmap_emissive;
    u8                               unknown_0[0x40];
};

static_assert(sizeof(shad_runtime_property_t) == 0x50);

/*! Bitmap slot in the shad postprocess properties; short tag reference
 * (datum only, no class) */
struct alignas(4) shader_map_t
{
    tag_index<tag_class_t::bitm> bitmap;
    u8                        unknown_0[8];
};

static_assert(sizeof(shader_map_t) == 12);

struct alignas(4) vec4_t
{
    f32 x, y, z, w;
};

/*! shad postprocess ("shader properties"): the compiled form of the
 * authoring-time parameters. Bitmaps[i] pairs with TilingData entries;
 * for base maps slot/tile 0 is the usual arrangement. */
/*! Pixel constant: a packed colour, R, G, B, A bytes */
struct alignas(4) pixel_constant_t
{
    u8 r, g, b, a;
};

static_assert(sizeof(pixel_constant_t) == 4);

struct alignas(4) shader_properties_t
{
    tag_index<tag_class_t::stem>   shader_template;
    tag_block<shader_map_t>     bitmaps;
    tag_block<pixel_constant_t> pixel_constants;
    tag_block<vec4_t>           tiling; /* vertex constants, xy = uv scale */
    u8                          unknown_1[96];
};

static_assert(offsetof(shader_properties_t, pixel_constants) == 0x0C);

static_assert(offsetof(shader_properties_t, tiling) == 0x14);
static_assert(sizeof(shader_properties_t) == 124);

/*! shad tag body prefix (Xbox: the authoring-time parameter block is
 * stripped; runtime properties + postprocess survive) */
struct alignas(4) shad_header_t
{
    static constexpr tag_class_t tag_class = tag_class_t::shad;

    tag_reference<tag_class_t::stem>   shader_template;
    string_id                          material_name;
    tag_block<shad_runtime_property_t> runtime_properties;
    u32                                flags;
    tag_block<u8> parameters; /* empty in shipped Xbox caches */
    tag_block<shader_properties_t> properties;
};

static_assert(offsetof(shad_header_t, runtime_properties) == 0x0C);
static_assert(offsetof(shad_header_t, properties) == 0x20);

/*! A point on a node, e.g. where a unit's camera sits */
struct alignas(4) model_marker_t
{
    i8    region;
    i8    permutation;
    i8    node;
    u8    unknown_0;
    Vecf3 translation; /*!< Node space */
    Quatf rotation;    /* x, y, z, w */
    f32   scale;
};

static_assert(sizeof(model_marker_t) == 0x24);

struct alignas(4) marker_group_t
{
    string_id                 name;
    tag_block<model_marker_t> markers;
};

static_assert(sizeof(marker_group_t) == 0xC);

/*! render_model (mode) tag body prefix; only the geometry-relevant blocks */
struct alignas(4) model_header_t
{
    static constexpr tag_class_t tag_class = tag_class_t::mode;

    string_id                  name;
    u32                        flags;
    u8                         unknown_0[12];
    tag_block<model_bounds_t>  bounds;
    tag_block<model_region_t>  regions;
    tag_block<model_section_t> sections;
    u8                         unknown_1[28];
    tag_block<model_node_t>    nodes;
    u8                         unknown_2[8];
    tag_block<marker_group_t>  marker_groups;
    tag_block<shader_ref_t>    shaders;
};

static_assert(offsetof(model_header_t, bounds) == 0x14);
static_assert(offsetof(model_header_t, sections) == 0x24);
static_assert(offsetof(model_header_t, nodes) == 0x48);
static_assert(offsetof(model_header_t, marker_groups) == 0x58);
static_assert(offsetof(model_header_t, shaders) == 96);

/*! sbsp cluster / section geometry, shared field layout (cluster elem is
 * 176 bytes, bsp section elem 200; both start identically) */
struct alignas(4) bsp_section_common_t
{
    u16                       vertex_count_raw;
    u16                       face_count_raw;
    u8                        unknown_0[20];
    bsp_block<model_bounds_t> bounds; /* compression info, 0x18 */
    u8                        unknown_1[8];
    raw_ref                   data;
    i32                       data_size_raw;
    i32                       header_size_raw;
    i32                       unknown_2;
    bsp_block<resource_t>     resources;

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

/*! Portal between two clusters: a convex polygon with its plane */
struct alignas(4) portal_vertex_t
{
    Vecf3 point;
};

static_assert(sizeof(portal_vertex_t) == 12);

enum class portal_flags_t : u32
{
    ai_cannot_hear   = 0x1,
    one_way          = 0x2,
    door             = 0x4,
    no_way           = 0x8,
    one_way_reversed = 0x10,
    no_one_can_hear  = 0x20,
};

struct alignas(4) cluster_portal_t
{
    block_index<bsp_cluster_t> back;
    block_index<bsp_cluster_t> front;
    i32                        plane_raw;
    Vecf3                      centroid;
    f32                        bounding_radius;
    u32                        flags_raw;
    bsp_block<portal_vertex_t> vertices;

    inline i16 back_cluster() const
    {
        return back.value();
    }

    inline i16 front_cluster() const
    {
        return front.value();
    }

    inline bool has(portal_flags_t flag) const
    {
        return from_le(flags_raw) & static_cast<u32>(flag);
    }
};

static_assert(offsetof(cluster_portal_t, vertices) == 0x1C);
static_assert(sizeof(cluster_portal_t) == 0x24);

/*! Collision BSP leaf -> cluster */
struct alignas(4) bsp_leaf_t
{
    block_index<bsp_cluster_t> cluster;
    i16                        surface_reference_count;
    i32                        first_surface_reference;
};

static_assert(sizeof(bsp_leaf_t) == 8);

struct alignas(2) index16_t
{
    i16 raw;

    inline i16 value() const
    {
        return from_le(raw);
    }
};

/*! sbsp cluster: section geometry plus its portals and instanced geometry */
enum class cluster_flags_t : u16
{
    none                   = 0x0,
    one_way_portal         = 0x1,
    door_portal            = 0x2,
    postprocessed_geometry = 0x4,
    is_the_sky             = 0x8,
};
C_FLAGS(cluster_flags_t, u16);

struct alignas(4) bsp_cluster_t : bsp_section_common_t
{
    u8                                      unknown_3[20];
    f32                                     x_min, x_max;
    f32                                     y_min, y_max;
    f32                                     z_min, z_max;
    block_index<scn::sky_ref_t, i8>         sky; /* into the scenario's skies */
    i8                                      media_index;
    block_index<scn::sky_ref_t, i8>         visible_sky;
    block_index<scn::atmospheric_fog_t, i8> atmospheric_fog;
    i8                                      planar_fog_designator;
    block_index<fog_plane_t, i8>            visible_fog_plane;
    block_index<background_sound_palette_t> background_sound;
    block_index<sound_environment_palette_t> sound_environment;
    block_index<weather_palette_t>           weather;
    block_index<structure_bsp_ref_t>         transition_bsp;
    i16                                      unknown_4[3];
    cluster_flags_t                          flags;
    i16                                      unknown_5;
    bsp_block<u8>                            predicted_resources; /* elem 8 */
    bsp_block<index16_t>                     portals;
    i32                                      checksum;
    bsp_block<index16_t>                     instance_indices;
    bsp_block<index16_t>                     index_reorder;
    data_reference<heap_t::bsp>              collision_mopp;
};

static_assert(offsetof(bsp_cluster_t, x_min) == 0x54);
static_assert(offsetof(bsp_cluster_t, portals) == 0x8C);
static_assert(offsetof(bsp_cluster_t, instance_indices) == 0x98);
static_assert(sizeof(bsp_cluster_t) == 176);

/*! Instanced geometry definition: render section plus its own collision
 * BSP, both in definition space */
struct alignas(4) bsp_geometry_section_t : bsp_section_common_t
{
    u8                       unknown_3[48];
    coll::bsp_t<heap_t::bsp> collision; /* inline, definition space */
    u8                       unknown_4[24];
};

static_assert(offsetof(bsp_geometry_section_t, collision) == 0x70);

static_assert(sizeof(bsp_geometry_section_t) == 200);

/*! Instanced geometry placement: which bsp section, where. Carries a
 * world-space bounding sphere (verified empirically: center tracks the
 * placed geometry, radius matches its extent) — this is the per-instance
 * culling volume the engine pairs with the owning cluster's portal
 * visibility. */
enum class instance_pathfinding_policy_t : u16
{
    cutout = 0,
    static_,
    none,
};

enum class instance_lightmapping_policy_t : u16
{
    per_pixel = 0,
    per_vertex,
};

struct alignas(4) bsp_instance_t
{
    f32 scale;
    f32 transform[12]; /* 4x3 row-major rotation+translation */
    block_index<bsp_geometry_section_t> section;
    u16                                 flags;
    f32                                 unknown_0;
    f32                                 sphere_center[3]; /* world space */
    f32                                 sphere_radius;
    u32                                 checksum;
    string_id                           name;
    instance_pathfinding_policy_t       pathfinding_policy;
    instance_lightmapping_policy_t      lightmapping_policy;
};

static_assert(offsetof(bsp_instance_t, section) == 52);
static_assert(offsetof(bsp_instance_t, sphere_center) == 0x3C);
static_assert(offsetof(bsp_instance_t, sphere_radius) == 0x48);
static_assert(offsetof(bsp_instance_t, name) == 80);
static_assert(sizeof(bsp_instance_t) == 88);

struct alignas(4) collision_material_t
{
    tag_reference<tag_class_t::shad>   old_shader;
    block_index<obj::matg::material_t> global_material;
    block_index<conveyor_surface_t>    conveyor_surface;
    tag_reference<tag_class_t::shad>   shader;
};

static_assert(sizeof(collision_material_t) == 0x14);

enum class fog_plane_flags_t : u16
{
    none                            = 0x0,
    extend_infinitely_while_visible = 0x1,
    do_not_floodfill                = 0x2,
    aggressive_floodfill            = 0x4,
};
C_FLAGS(fog_plane_flags_t, u16);

struct alignas(4) fog_plane_t
{
    block_index<scn::planar_fog_t> scenario_planar_fog;
    i16                            unknown_0;
    Vecf4                          plane; /*!< normal, distance */
    fog_plane_flags_t              flags;
    i16                            priority_raw;
};

static_assert(sizeof(fog_plane_t) == 0x18);

/*! Shared by the sbsp and the scenario's palettes */
struct alignas(4) weather_palette_t
{
    bl_string                        name;
    tag_reference<tag_class_t::weat> weather_system;
    u8                               unknown_0[0x4C - 0x28];
    tag_reference<tag_class_t::wind> wind;
    Vecf3                            wind_direction;
    f32                              wind_magnitude;
    u32                              unknown_1;
    bl_string                        wind_scale_function;
};

static_assert(offsetof(weather_palette_t, wind) == 0x4C);
static_assert(sizeof(weather_palette_t) == 0x88);

enum class sound_scale_flags_t : u32
{
    none                                   = 0x0,
    override_default_scale                 = 0x1,
    use_adjacent_cluster_as_portal_scale   = 0x2,
    use_adjacent_cluster_as_exterior_scale = 0x4,
    scale_with_weather_intensity           = 0x8,
};
C_FLAGS(sound_scale_flags_t, u32);

struct alignas(4) background_sound_palette_t
{
    bl_string                        name;
    tag_reference<tag_class_t::lsnd> background_sound;
    tag_reference<tag_class_t::lsnd> inside_cluster_sound;
    u8                               unknown_0[0x44 - 0x30];
    f32                              cutoff_distance;
    sound_scale_flags_t              scale_flags;
    f32                              interior_scale;
    f32                              portal_scale;
    f32                              exterior_scale;
    f32                              interpolation_speed;
    u8                               unknown_1[8];
};

static_assert(offsetof(background_sound_palette_t, cutoff_distance) == 0x44);
static_assert(sizeof(background_sound_palette_t) == 0x64);

struct alignas(4) sound_environment_palette_t
{
    bl_string                        name;
    tag_reference<tag_class_t::snde> sound_environment;
    f32                              cutoff_distance;
    f32                              interpolation_speed;
    u8                               unknown_0[0x48 - 0x30];
};

static_assert(sizeof(sound_environment_palette_t) == 0x48);

struct alignas(4) weather_polyhedron_t
{
    Vecf3                    center;
    f32                      radius;
    bsp_block<coll::plane_t> planes;
};

static_assert(sizeof(weather_polyhedron_t) == 0x18);

struct alignas(4) conveyor_surface_t
{
    Vecf3 u;
    Vecf3 v;
};

static_assert(sizeof(conveyor_surface_t) == 0x18);

struct alignas(4) breakable_surface_t
{
    block_index<bsp_instance_t> instance;
    i16                         breakable_surface_raw;
    Vecf3                       centroid;
    f32                         radius;
    i32                         collision_surface_raw;
};

static_assert(sizeof(breakable_surface_t) == 0x18);

/*! Named point in the BSP, e.g. for scripted object placement */
struct alignas(4) marker_t
{
    bl_string name;
    Quatf     rotation; /*!< quaternion i, j, k, w */
    Vecf3     position;
};

static_assert(sizeof(marker_t) == 0x3C);

struct alignas(4) runtime_decal_t
{
    Vecf3                       position;
    block_index<scn::tag_ref_t> decal_type;
    i8                          yaw;
    i8                          pitch;
};

static_assert(sizeof(runtime_decal_t) == 0x10);

/*! Which object classes an environment object palette entry stands for */
enum class object_type_flags_t : u32
{
    none          = 0x0,
    biped         = 0x1,
    vehicle       = 0x2,
    weapon        = 0x4,
    equipment     = 0x8,
    garbage       = 0x10,
    projectile    = 0x20,
    scenery       = 0x40,
    machine       = 0x80,
    control       = 0x100,
    light_fixture = 0x200,
    sound_scenery = 0x400,
    crate         = 0x800,
    creature      = 0x1000,
};
C_FLAGS(object_type_flags_t, u32);

struct alignas(4) environment_object_palette_t
{
    object_reference                 definition;
    tag_reference<tag_class_t::mode> model;
    object_type_flags_t              object_type;
};

static_assert(sizeof(environment_object_palette_t) == 0x14);

/*! Objects baked into the BSP at export (the scenario placements they
 * came from are named) */
struct alignas(4) environment_object_t
{
    bl_string                                 name;
    Quatf                                     rotation;
    Vecf3                                     translation;
    block_index<environment_object_palette_t> palette;
    i16                                       unknown_0;
    datum_index                               unique_id;
    tag_class                                 exported_object_type;
    bl_string                                 scenario_object_name;
};

static_assert(offsetof(environment_object_t, unique_id) == 0x40);
static_assert(sizeof(environment_object_t) == 0x68);

/*! Ambience and reverb cluster groups: which clusters share a sound space */
struct alignas(4) sound_cluster_t
{
    i16                  unknown_0[2];
    bsp_block<index16_t> enclosing_portal_designators;
    bsp_block<index16_t> interior_cluster_indices;
};

static_assert(sizeof(sound_cluster_t) == 0x14);

struct alignas(4) transparent_plane_t
{
    i16   section_raw;
    i16   part_raw;
    Vecf4 plane;
};

static_assert(sizeof(transparent_plane_t) == 0x14);

/*! A body of water: its own geometry (same raw layout as a cluster's, no
 * counts) and how it shades */
struct alignas(4) water_definition_t
{
    tag_reference<tag_class_t::shad> shader;
    bsp_block<u8>                    section; /*!< elem 0x44, authoring only */
    raw_ref                          data;
    i32                              data_size_raw;
    i32                              header_size_raw;
    i32                              unknown_0;
    bsp_block<resource_t>            resources;
    u8                               unknown_1[0x34 - 0x28];
    Vecf3                            sun_spot_color;
    Vecf3                            reflection_tint;
    Vecf3                            refraction_tint;
    Vecf3                            horizon_color;
    f32                              sun_specular_power;
    u8                               unknown_2[0xAC - 0x68];

    inline i32 data_size() const
    {
        return from_le(data_size_raw);
    }

    inline i32 base_address() const
    {
        return from_le(header_size_raw) + 8;
    }
};

static_assert(offsetof(water_definition_t, resources) == 0x20);
static_assert(offsetof(water_definition_t, sun_spot_color) == 0x34);

static_assert(sizeof(water_definition_t) == 0xAC);

/*! Decorators: grass, pebbles and the like. The scenario's decorator palette
 * names the sets (decr); each structure BSP caches its placements in raw
 * blocks, grouped by cluster and set */
struct alignas(4) decorator_placement_t
{
    /*! Bits 9-15 a turn in 112ths (unverified), 16-19 the scale between the
     * permutation's bounds; TODO: bits 20-31 */
    u32 internal;
    /*! Within the placement's 8 wu cell: x and y 10 bits, z 12 */
    u32 compressed_position;
    u8  tint[4];
    u8  light[4]; /*!< Baked light colour */
    /*! Baked light's direction, unit: 11:11:10 bits over -1 to 1 */
    u32 compressed_light_direction;
    f32 unknown_0; /* TODO: 0.4 to 1.1, likely the light's weight */

    /*! How far between the permutation's scale bounds, 0 to 1 */
    inline f32 scale() const
    {
        return static_cast<f32>((from_le(internal) >> 16) & 0xF) / 15.f;
    }

    inline f32 yaw() const
    {
        return static_cast<f32>((from_le(internal) >> 9) & 0x7F) * (6.2831853f / 112.f);
    }

    /*! Offset into its cell, world units */
    inline Vecf3 offset() const
    {
        u32 const p = from_le(compressed_position);
        return Vecf3(
            static_cast<f32>(p & 0x3FF) * (8.f / 1023.f),
            static_cast<f32>((p >> 10) & 0x3FF) * (8.f / 1023.f),
            static_cast<f32>((p >> 20) & 0xFFF) * (8.f / 4095.f));
    }
};

static_assert(sizeof(decorator_placement_t) == 24);

/*! A sprite decorator's quad corner: the pivot in world space and the
 * corner's offset in a frame turned about `axis` to face the viewer */
struct alignas(4) decorator_sprite_vertex_t
{
    Vecf3 position;
    Vecf3 offset; /*!< x along the view, y across, z up */
    Vecf3 axis;
    Vecf2 texcoord;
    f32   unknown_0;
};

static_assert(sizeof(decorator_sprite_vertex_t) == 48);

/*! Body of a cache block's raw data: `blkh`, then fields the block's
 * resources fill (their type0 is the field's offset past the 8-byte
 * prefix; data lands at header_size + 8 + resource offset) */
struct alignas(4) decorator_block_t
{
    u32                                   magic; /* 'blkh' */
    u32                                   size;
    tag_block<decorator_placement_t>      placements;
    u8                                    unknown_0[0x50 - 0x10];
    tag_block<decorator_sprite_vertex_t>  sprite_vertices;
    tag_block<u16>                        sprite_indices;
    u8                                    unknown_1[0x88 - 0x60];
};

static_assert(sizeof(decorator_block_t) == 0x88);

struct alignas(4) decorator_cache_block_t
{
    raw_ref               data;
    u32                   size;
    u32                   header_size;
    u32                   data_size;
    bsp_block<resource_t> resources;
    u32                   owner;
    u32                   unknown_0;
    i32                   unknown_1;
    u32                   unknown_2[2];
};

static_assert(sizeof(decorator_cache_block_t) == 44);

/*! Placements of one set in one cluster, with their sprite geometry if the
 * set's class is a sprite */
struct alignas(4) decorator_group_t
{
    u8  set;         /*!< Into the scenario's decorator palette */
    u8  type;        /*!< Class of the set */
    u8  permutation; /*!< Of the class */
    u8  compressed_radius;
    i16 cluster;
    i16 cache_block;
    i16 placement_start;
    i16 placement_count;
    i16 vertex_start;
    i16 vertex_count;
    i16 index_start;
    i16 index_count;
    u8  unknown_0[4];
};

static_assert(sizeof(decorator_group_t) == 24);

/*! Octree node over the decorators: children by octant (x = 4, y = 2,
 * z = 1); a leaf is one 8 wu cell, holding one cache block */
struct alignas(4) decorator_cell_t
{
    i16 children[8];
    i16 cache_block;
    i16 group_count;
    i16 first_group;
    i16 unknown_0;
};

static_assert(sizeof(decorator_cell_t) == 24);

struct alignas(4) decorator_cache_t
{
    Vecf3                               grid_center; /*!< Of the root cell */
    u32                                 cell_count;  /*!< 8 wu cells per root side */
    bsp_block<decorator_cache_block_t>  cache_blocks;
    bsp_block<decorator_group_t>        groups;
    bsp_block<decorator_cell_t>         cells;
    u32                                 unknown_0[2];
};

static_assert(sizeof(decorator_cache_t) == 0x30);

/*! decr: a decorator set's shaders and classes; models carry their own
 * small meshes inline */
struct alignas(4) decorator_shader_t
{
    tag_reference<tag_class_t::shad> shader;
};

struct alignas(4) decorator_permutation_t
{
    string_id name;
    u32       unknown_0;
    u8        shader; /*!< Into the set's shaders */
    u8        unknown_1[3];
    f32       scale_min;
    f32       scale_max;
    u8        color_min[4];
    u8        color_max[4];
    f32       unknown_2[3];
};

static_assert(sizeof(decorator_permutation_t) == 40);

enum class decorator_type_t : u32
{
    model               = 0,
    floating_decal      = 1,
    projected_decal     = 2,
    screen_facing_quad  = 3,
    axis_rotating_quad  = 4,
    cross_quad          = 5,
};

struct alignas(4) decorator_class_t
{
    string_id                           name;
    decorator_type_t                    type;
    f32                                 unknown_0;
    tag_block<decorator_permutation_t>  permutations;
};

static_assert(sizeof(decorator_class_t) == 20);

/*! A model decorator's mesh, a range of the set's indices; permutations
 * pick theirs by name */
struct alignas(4) decorator_model_t
{
    string_id name;
    i16       index_start;
    i16       index_count;
};

static_assert(sizeof(decorator_model_t) == 8);

struct alignas(4) decorator_model_vertex_t
{
    Vecf3 position;
    Vecf3 normal;
    Vecf3 tangent;
    Vecf3 binormal;
    Vecf2 texcoord;
};

static_assert(sizeof(decorator_model_vertex_t) == 56);

struct alignas(4) decorator_set_t
{
    tag_block<decorator_shader_t>       shaders;
    f32                                 unknown_0;
    f32                                 unknown_1;
    tag_block<decorator_class_t>        classes;
    tag_block<decorator_model_t>        models;
    tag_block<decorator_model_vertex_t> vertices;
    tag_block<u16>                      indices;
};

static_assert(offsetof(decorator_set_t, classes) == 0x10);
static_assert(sizeof(decorator_set_t) == 0x30);

/*! sbsp tag body (Xbox offsets, Assembly's sbsp.xml); pathfinding,
 * decorator and MOPP data stay opaque */
struct alignas(4) bsp_header_t
{
    static constexpr tag_class_t tag_class = tag_class_t::sbsp;

    u8                                      unknown_0[12];
    bsp_block<collision_material_t>         collision_materials;
    bsp_block<coll::bsp_t<heap_t::bsp>>     collision; /* world space */
    u8                                      unknown_0b[16];
    bsp_block<bsp_leaf_t>                   leaves;
    f32                                     x_min, x_max;
    f32                                     y_min, y_max;
    f32                                     z_min, z_max;
    bsp_block<u8>                           surface_references; /* elem 8 */
    data_reference<heap_t::bsp>             cluster_data;
    bsp_block<cluster_portal_t>             portals;
    bsp_block<fog_plane_t>                  fog_planes;
    u8                                      unknown_1[0x84 - 0x6C];
    bsp_block<weather_palette_t>            weather_palette;
    bsp_block<weather_polyhedron_t>         weather_polyhedra;
    bsp_block<u8>                           detail_objects; /* elem 0x24 */
    bsp_block<bsp_cluster_t>                clusters;
    bsp_block<shader_ref_t>                 shaders;
    bsp_block<index16_t>                    sky_owner_cluster;
    bsp_block<conveyor_surface_t>           conveyor_surfaces;
    bsp_block<breakable_surface_t>          breakable_surfaces;
    bsp_block<u8>                           pathfinding_data; /* elem 0x74 */
    bsp_block<u8>                           pathfinding_edges;
    bsp_block<background_sound_palette_t>   background_sound_palette;
    bsp_block<sound_environment_palette_t>  sound_environment_palette;
    data_reference<heap_t::bsp>             sound_pas_data;
    bsp_block<marker_t>                     markers;
    bsp_block<runtime_decal_t>              runtime_decals;
    bsp_block<environment_object_palette_t> environment_object_palette;
    bsp_block<environment_object_t>         environment_objects;
    u8                                      unknown_2[0x138 - 0x10C];
    bsp_block<bsp_geometry_section_t>       sections;
    bsp_block<bsp_instance_t>               instances;
    bsp_block<sound_cluster_t>              ambience_sound_clusters;
    bsp_block<sound_cluster_t>              reverb_sound_clusters;
    bsp_block<transparent_plane_t>          transparent_planes;
    u8                                      unknown_3[0x1C0 - 0x160];
    f32                                     vehicle_spherical_limit_radius;
    Vecf3                                   vehicle_spherical_limit_center;
    bsp_block<u8>                           debug_info; /* elem 0x58 */
    tag_reference<tag_class_t::decr>        decorators;
    data_reference<heap_t::bsp>             mopp_codes;
    u32                                     unknown_4;
    Vecf3                                   mopp_bounds_min;
    Vecf3                                   mopp_bounds_max;
    data_reference<heap_t::bsp>             breakable_surface_mopp_codes;
    bsp_block<u8>                 breakable_surface_key_table; /* elem 0x20 */
    bsp_block<water_definition_t> water_definitions;
    bsp_block<u8>                 portal_device_mapping;   /* elem 0x10 */
    bsp_block<u8>                 audibility;              /* elem 0x34 */
    bsp_block<u8>                 object_fake_lightprobes; /* elem 0x5C */
    bsp_block<decorator_cache_t>  decorator_cache;
};

static_assert(offsetof(bsp_header_t, collision) == 0x14);
static_assert(offsetof(bsp_header_t, leaves) == 0x2C);
static_assert(offsetof(bsp_header_t, portals) == 0x5C);
static_assert(offsetof(bsp_header_t, x_min) == 52);
static_assert(offsetof(bsp_header_t, weather_palette) == 0x84);
static_assert(offsetof(bsp_header_t, clusters) == 156);
static_assert(offsetof(bsp_header_t, shaders) == 164);
static_assert(offsetof(bsp_header_t, background_sound_palette) == 0xD4);
static_assert(offsetof(bsp_header_t, markers) == 0xEC);
static_assert(offsetof(bsp_header_t, environment_objects) == 0x104);
static_assert(offsetof(bsp_header_t, sections) == 312);
static_assert(offsetof(bsp_header_t, instances) == 320);
static_assert(offsetof(bsp_header_t, vehicle_spherical_limit_radius) == 0x1C0);
static_assert(offsetof(bsp_header_t, mopp_bounds_min) == 0x1EC);
static_assert(offsetof(bsp_header_t, water_definitions) == 0x214);
static_assert(sizeof(bsp_header_t) == 0x23C);

/*! First 16 bytes of a BSP region: the sbsp body follows it, the ltmp body
 * is wherever lightmap_pointer says (both relative to the region's magic) */
struct alignas(4) bsp_region_header_t
{
    u32    size_raw;
    u32    sbsp_pointer_raw;
    u32    lightmap_pointer_raw;
    bl_tag tag; /*!< 'sbsp' */

    inline u32 lightmap_pointer() const
    {
        return from_le(lightmap_pointer_raw);
    }
};

static_assert(sizeof(bsp_region_header_t) == 16);

/*! scnr structure-bsp entry: BSP tag meta lives in its own file region
 * with its own virtual base, unlike every other tag. Pointers inside it
 * translate as file = meta_address + (vaddr - magic), and the sbsp tag
 * body sits at file offset meta_address + 16. */
struct alignas(4) structure_bsp_ref_t
{
    u32 meta_address_raw; /*!< File offset of the BSP region */
    i32 size_raw;
    u32 magic_raw; /*!< Virtual address of the region */
    u32 unknown_0;
    tag_reference<tag_class_t::sbsp> bsp;
    tag_reference<tag_class_t::ltmp> lightmap; /*!< ltmp */
    u8                               unknown_1[0x40 - 0x20];
    block_index<scn::sky_ref_t> default_sky; /* into the scenario's skies */
    i16                         unknown_2;

    inline u32 meta_address() const
    {
        return from_le(meta_address_raw);
    }

    inline u32 magic() const
    {
        return from_le(magic_raw);
    }

    /*! The region's own translation; file = meta_address + (ptr - magic) */
    inline bsp_magic region_magic(tag_magic const& map_magic) const
    {
        if(meta_address() > map_magic.max_size)
            return {};
        return bsp_magic{{
            .base         = map_magic.base,
            .max_size     = map_magic.max_size,
            .file_offset  = meta_address(),
            .virtual_base = magic(),
        }};
    }

    inline bsp_region_header_t const* region(tag_magic const& map_magic) const
    {
        if(!map_magic.base || meta_address() > map_magic.max_size ||
           map_magic.max_size - meta_address() < sizeof(bsp_region_header_t))
            return nullptr;
        return reinterpret_cast<bsp_region_header_t const*>(
            map_magic.base + meta_address());
    }

    /*! The sbsp body directly follows the region header */
    inline bsp_header_t const* body(tag_magic const& map_magic) const
    {
        auto const* head = region(map_magic);
        if(!head || map_magic.max_size - meta_address() <
                        sizeof(bsp_region_header_t) + sizeof(bsp_header_t))
            return nullptr;
        return reinterpret_cast<bsp_header_t const*>(head + 1);
    }
};

static_assert(offsetof(structure_bsp_ref_t, bsp) == 16);
static_assert(offsetof(structure_bsp_ref_t, lightmap) == 24);
static_assert(sizeof(structure_bsp_ref_t) == 68);

/*! The one piece of scnr needed for geometry: the structure bsps block at
 * offset 528 (Xbox) */
struct alignas(4) scnr_bsps_t
{
    u8                             unknown_0[528];
    tag_block<structure_bsp_ref_t> structure_bsps;
};

static_assert(offsetof(scnr_bsps_t, structure_bsps) == 528);

/*! Draw range within a section, one per shader ("part" in the section
 * data); streamed as resource (0,72) */
struct alignas(4) part_t
{
    i16 type_raw; /*!< 0 not drawn, 1 shadow only, 2 opaque shadow-casting,
                       3 opaque non-shadowing, 4 transparent, 5 lightmap */
    u16                       flags;
    block_index<shader_ref_t> material;
    u16                       strip_start_raw;
    u16                       strip_length_raw;
    block_index<subpart_t>    first_subpart;
    i16                       subpart_count_raw;
    u8                        node_info[2];
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
    block_index<part_t> part;
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
    u16    index_count_raw; /*!< strip/list u16 index total */
    u8     unknown_3[54];
    f32    centroid[3]; /*!< roughly the section center */
    u32    unknown_4;
};

static_assert(offsetof(block_header_t, part_count_raw) == 8);
static_assert(offsetof(block_header_t, index_count_raw) == 40);
static_assert(offsetof(block_header_t, centroid) == 96);
static_assert(sizeof(block_header_t) == 112);

/*! Geometry block header: index count for the section's index buffer
 * (u16 strip or list indices) at offset 40 on Xbox */
inline u16 block_index_count(Span<const u8> block)
{
    if(block.size() < 42)
        return 0;
    u16 count;
    std::memcpy(&count, block.data() + 40, 2);
    return from_le(count);
}

} // namespace blam::dimeter::geo
