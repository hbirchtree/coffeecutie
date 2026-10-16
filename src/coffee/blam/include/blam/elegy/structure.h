#pragma once

#include "collision.h"
#include "model.h"
#include "objects.h"

namespace blam::elegy {

struct cluster_t
{
    std::array<bounds_f, 3>           bounds;
    i8                                sky;
    i8                                atmosphere;
    i8                                camera_fx;
    u8                                unknown_0;
    std::array<be<i16>, 7>            sound;
    be<i16>                           first_decal;
    be<i16>                           decal_count;
    be<u16>                           flags;
    std::array<u32, 3>                predicted_resources;
    tag_block<be<i16>>                portals;
    std::array<u8, 0x48>              collision;
    tag_block<std::array<u8, 0x40>>   collision_mopp;
    block_index<mesh_t>               mesh;
    u16                               unknown_1;
    tag_block<i8>                     seam_indices;
    tag_block<std::array<u8, 0x30>>   decorator_groups;
    tag_block<std::array<u8, 0x1>>    unknown_2;
    tag_block<std::array<u8, 0x4>>    pvs_bound_objects;
    tag_block<std::array<u8, 0x10>>   cubemaps;
};

static_assert(offsetof(cluster_t, portals) == 0x3C);
static_assert(offsetof(cluster_t, collision_mopp) == 0x90);
static_assert(offsetof(cluster_t, mesh) == 0x9C);
static_assert(offsetof(cluster_t, pvs_bound_objects) == 0xC4);
static_assert(sizeof(cluster_t) == 0xDC);

struct instanced_geometry_definition_t;

/*! A placement of an instanced_geometry_definition_t */
struct instanced_geometry_t
{
    be<f32>                                       scale;
    be<Vecf3>                                     forward;
    be<Vecf3>                                     left;
    be<Vecf3>                                     up;
    be<Vecf3>                                     position;
    block_index<instanced_geometry_definition_t> definition;
    be<u16>                                       flags;
    be<i16>                                       lightmap_texcoords;
    u16                                           unknown_0;
    be<i16>                                       subcluster_bitvector;
    be<i16>                                       render_number;
    be<Vecf3>                                     sphere_center;
    be<f32>                                       sphere_radius;
    be<i32>                                       checksum;
    string_id                                     name;
    be<u16>                                       pathfinding_policy;
    be<u16>                                       lightmapping_policy;
    be<f32>                                       lightmap_resolution_scale;
    tag_block<std::array<u8, 0x70>>               physics;
    std::array<u32, 3>                            unknown_1;
};

static_assert(offsetof(instanced_geometry_t, definition) == 0x34);
static_assert(offsetof(instanced_geometry_t, name) == 0x54);
static_assert(sizeof(instanced_geometry_t) == 0x78);

/*! Seam between this BSP and another, shared by edge and cluster */
struct seam_identifier_t
{
    std::array<u8, 0x10>            id;
    tag_block<be<i32>>              edges;
    tag_block<std::array<u8, 0x10>> clusters;
};

static_assert(sizeof(seam_identifier_t) == 0x28);

struct collision_material_t
{
    tag_reference render_method;
    be<i16>       global_material;
    be<i16>       conveyor_surface;
    be<i16>       seam_mapping;
    u16           unknown_0;
};

static_assert(sizeof(collision_material_t) == 0x18);

enum class portal_flags_t : u32
{
    none             = 0x0,
    ai_cant_hear     = 0x1,
    one_way          = 0x2,
    door             = 0x4,
    no_way           = 0x8,
    one_way_reversed = 0x10,
    no_one_can_hear  = 0x20,
};
C_FLAGS(portal_flags_t, u32);

/*! A convex opening between two clusters, on a collision plane */
struct cluster_portal_t
{
    be<i16>              back_cluster;
    be<i16>              front_cluster;
    be<i32>              plane;
    be<Vecf3>            centroid;
    be<f32>              bounding_radius;
    be<portal_flags_t>   flags;
    tag_block<be<Vecf3>> vertices;
};

static_assert(offsetof(cluster_portal_t, vertices) == 0x1C);
static_assert(sizeof(cluster_portal_t) == 0x28);

/*!
 * \brief scenario_structure_bsp ("sbsp"), the parts that place geometry.
 * Its own render geometry ships stripped: the lightmapper moves it to the
 * scenario_lightmap (sLdT) tag, which also has the meshes clusters and
 * instances index.
 */
struct structure_bsp_t
{
    be<i32>                               import_checksum;
    be<i32>                               import_version;
    string_id                             name;
    tag_block<seam_identifier_t>          seams;
    tag_block<std::array<be<i16>, 2>>     edge_to_seam_edge;
    tag_block<collision_material_t>       collision_materials;
    /*! Collision leaf -> cluster, -1 for none */
    tag_block<i8>                         leaves;
    std::array<bounds_f, 3>               world_bounds;
    tag_block<coll::structure_surface_t>  structure_surfaces;
    tag_block<coll::surface_triangle_t>   surface_triangles;
    tag_block<cluster_portal_t>           cluster_portals;
    std::array<u8, 0x3C>                  unknown_0;
    tag_block<cluster_t>                  clusters;
    tag_block<geometry_material_t>        materials;
    std::array<u8, 0xE4>                  unknown_1;
    tag_block<instanced_geometry_t>       instances;
    tag_block<tag_reference>              decorator_sets;
    render_geometry_t                     decorator_geometry;
    std::array<u8, 0x94>                  unknown_2;
    render_geometry_t                     geometry;
    tag_block<std::array<u8, 0x14>>       widgets;
    std::array<u32, 3>                    raw_resources;
    datum_index                           tag_resources;
    u32                                   unknown_3;
    be<i32>                               use_resource_items;
};

static_assert(offsetof(structure_bsp_t, leaves) == 0x30);
static_assert(offsetof(structure_bsp_t, structure_surfaces) == 0x54);
static_assert(offsetof(structure_bsp_t, cluster_portals) == 0x6C);
static_assert(offsetof(structure_bsp_t, clusters) == 0xB4);
static_assert(offsetof(structure_bsp_t, materials) == 0xC0);
static_assert(offsetof(structure_bsp_t, instances) == 0x1B0);
static_assert(offsetof(structure_bsp_t, decorator_geometry) == 0x1C8);
static_assert(offsetof(structure_bsp_t, geometry) == 0x2E0);
static_assert(offsetof(structure_bsp_t, tag_resources) == 0x37C);

/*! One instance definition: its collision and its mesh */
struct instanced_geometry_definition_t
{
    be<i32>                                   checksum;
    be<Vecf3>                                 sphere_offset;
    be<f32>                                   sphere_radius;
    coll::collision_bsp_t                     collision;
    resource_block<std::array<u8, 1>>         unknown_1;
    resource_block<coll::mopp_code_t>         mopp_codes;
    resource_block<std::array<u8, 1>>         unknown_2;
    resource_block<coll::structure_surface_t> structure_surfaces;
    resource_block<coll::surface_triangle_t>  surface_triangles;
    block_index<mesh_t>                       mesh;
    block_index<compression_t>                compression;
    be<f32>                                   unknown_0;
};

static_assert(offsetof(instanced_geometry_definition_t, mopp_codes) == 0x80);
static_assert(offsetof(instanced_geometry_definition_t, mesh) == 0xB0);
static_assert(sizeof(instanced_geometry_definition_t) == 0xB8);

/*! structure_bsp_tag_resources, root of the sbsp's tag_resources */
struct structure_bsp_resources_t
{
    resource_block<coll::collision_bsp_t>           collision_bsps;
    resource_block<instanced_geometry_definition_t> instanced_geometry;
};

struct lightmap_bsp_t
{
    be<u16>                 flags;
    be<i16>                 bsp; /*!< Into the scenario's structure BSPs */
    std::array<u8, 0x12C>   unknown_0;
    render_geometry_t       geometry;
};

static_assert(offsetof(lightmap_bsp_t, geometry) == 0x130);
static_assert(sizeof(lightmap_bsp_t) == 0x1B4);

/*! scenario_lightmap ("sLdT"), one entry per BSP */
struct scenario_lightmap_t
{
    u32                       unknown_0;
    tag_block<lightmap_bsp_t> bsps;
};

static_assert(offsetof(scenario_lightmap_t, bsps) == 0x4);

struct scenario_bsp_t
{
    tag_reference         bsp;
    tag_reference         design;
    tag_reference         lighting;
    std::array<u8, 0x3C>  unknown_0;
};

static_assert(sizeof(scenario_bsp_t) == 0x6C);

/*! scenario ("scnr"): BSPs, scenery and where players start */
struct scenario_t
{
    std::array<u8, 0x14>             unknown_0;
    tag_block<scenario_bsp_t>        bsps;
    std::array<u8, 0x9C>             unknown_1;
    tag_block<scenery_placement_t>   scenery;
    tag_block<palette_entry_t>       scenery_palette;
    std::array<u8, 0x180>            unknown_2;
    tag_block<starting_location_t>   starting_locations;
};

static_assert(offsetof(scenario_t, bsps) == 0x14);
static_assert(offsetof(scenario_t, scenery) == 0xBC);
static_assert(offsetof(scenario_t, scenery_palette) == 0xC8);
static_assert(offsetof(scenario_t, starting_locations) == 0x254);

} // namespace blam::elegy
