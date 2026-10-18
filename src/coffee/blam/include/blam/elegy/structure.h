#pragma once

#include "collision.h"
#include "model.h"
#include "objects.h"

namespace blam::elegy {

/*! A cubemap sampled for a cluster: where, and which image of the BSP's cubemap bitmap */
struct cluster_cubemap_t
{
    be<Vecf3> position;
    be<i16>   scenario_cubemap;
    be<i16>   bitmap_index;
};

static_assert(sizeof(cluster_cubemap_t) == 0x10);

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
    tag_block<cluster_cubemap_t>      cubemaps;
};

static_assert(offsetof(cluster_t, portals) == 0x3C);
static_assert(offsetof(cluster_t, collision_mopp) == 0x90);
static_assert(offsetof(cluster_t, mesh) == 0x9C);
static_assert(offsetof(cluster_t, pvs_bound_objects) == 0xC4);
static_assert(sizeof(cluster_t) == 0xDC);

struct instanced_geometry_definition_t;

/*! A placement of an instanced_geometry_definition_t */
enum class instance_flags_t : u16
{
    none                       = 0x0,
    not_in_lightprobes         = 0x1,
    render_only                = 0x2, /*!< Seen, never collided with (vistas) */
    does_not_block_aoe_damage  = 0x4,
    collidable                 = 0x8,
    decal_spacing              = 0x10,
};
C_FLAGS(instance_flags_t, u16);

struct instanced_geometry_t
{
    be<f32>                                       scale;
    be<Vecf3>                                     forward;
    be<Vecf3>                                     left;
    be<Vecf3>                                     up;
    be<Vecf3>                                     position;
    block_index<instanced_geometry_definition_t> definition;
    be<instance_flags_t>                          flags;
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

    /*! Its definition's space to the world: scaled basis and position */
    Matf4 transform() const;
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
    be<i16>       seam_mapping; /*!< Seals the BSP at this seam, -1 if not a sealer */
    u16           unknown_0;

    inline bool seam_sealer() const
    {
        return seam_mapping.value() >= 0;
    }
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
    static constexpr std::string_view group = "sbsp";

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

/*! Baked lighting stored per vertex, in a static_per_vertex buffer */
struct lightmap_per_vertex_t
{
    tag_block<std::array<u8, 0x20>> probes;
    be<i32>                         vertex_buffer;
};

static_assert(sizeof(lightmap_per_vertex_t) == 0x10);

/*! Where a cluster's or instance's light comes from; -1 for none */
struct lightmap_target_t
{
    be<i16> texture_array_index;
    be<i16> per_vertex;
};

struct lightmap_instance_t
{
    lightmap_target_t target;
    be<i16>           probe;
    u16               unknown_0;
};

static_assert(sizeof(lightmap_instance_t) == 0x8);

/*! Light at one point: dominant light, quadratic SH per colour channel */
struct lightmap_probe_t
{
    std::array<be<i16>, 4>  dominant_direction;
    std::array<be<i16>, 4>  dominant_intensity;
    std::array<be<i16>, 27> sh;
    u16                     unknown_0;
};

static_assert(sizeof(lightmap_probe_t) == 0x48);

/*!
 * \brief One BSP's baked lighting. Texels hold linear SH (4 coefficients)
 * plus the dominant light intensity, each as two DXT5 slices: the length
 * of the RGB vector is alpha0 * alpha1 * max_luminance, its direction
 * (rgb0 + rgb1) * 2 - 2. max_luminance per vector is in `compression`.
 */
struct lightmap_bsp_t
{
    be<u16>                          flags;
    be<i16>                          bsp; /*!< Into the scenario's structure BSPs */
    be<i32>                          checksum;
    std::array<be<Vecf3>, 18>        compression;
    tag_reference                    lightprobe_texture;
    tag_reference                    intensity_texture;
    tag_block<lightmap_per_vertex_t> per_vertex;
    tag_block<lightmap_target_t>     clusters;
    tag_block<lightmap_instance_t>   instances;
    tag_block<lightmap_probe_t>      probes;
    render_geometry_t                geometry;
};

static_assert(offsetof(lightmap_bsp_t, lightprobe_texture) == 0xE0);
static_assert(offsetof(lightmap_bsp_t, per_vertex) == 0x100);
static_assert(offsetof(lightmap_bsp_t, probes) == 0x124);
static_assert(offsetof(lightmap_bsp_t, geometry) == 0x130);
static_assert(sizeof(lightmap_bsp_t) == 0x1B4);

/*! scenario_lightmap ("sLdT"), one entry per BSP */
struct scenario_lightmap_t
{
    static constexpr std::string_view group = "sLdT";

    u32                       unknown_0;
    tag_block<lightmap_bsp_t> bsps;
};

static_assert(offsetof(scenario_lightmap_t, bsps) == 0x4);

struct scenario_bsp_t
{
    tag_reference         bsp;
    tag_reference         design;
    tag_reference         lighting;
    std::array<u8, 0x18>  unknown_0;
    tag_reference         cubemaps; /*!< One cube image per cubemap point */
    tag_reference         wind;
    u32                   unknown_1;
};

static_assert(offsetof(scenario_bsp_t, cubemaps) == 0x48);

static_assert(sizeof(scenario_bsp_t) == 0x6C);

/*! scenario ("scnr"): BSPs, scenery and where players start */
enum class atmosphere_flags_t : u16
{
    none                     = 0x0,
    enable_atmosphere        = 0x1,
    override_real_sun_values = 0x2,
    patchy_fog               = 0x4,
};
C_FLAGS(atmosphere_flags_t, u16);

/*!
 * \brief One atmosphere of a sky_atm_parameters ("skya") tag: the sun it
 * scatters (when overriding the real one), and Rayleigh/Mie scattering
 * with the coefficients already worked out (betas, theta prefixes).
 */
struct atmosphere_setting_t
{
    be<atmosphere_flags_t> flags;
    u16                    unknown_0;
    string_id              name;
    be<f32>                sun_pitch;   /*!< Degrees above the horizon */
    be<f32>                sun_heading; /*!< Degrees */
    be<Vecf3>              sun_color;
    be<f32>                sun_intensity;
    be<f32>                sea_level;
    be<f32>                rayleigh_height_scale;
    be<f32>                mie_height_scale;
    be<f32>                rayleigh_multiplier;
    be<f32>                mie_multiplier;
    be<f32>                sun_phase_function; /*!< Henyey-Greenstein g */
    be<f32>                desaturation;
    be<f32>                distance_bias;
    be<f32>                max_fog_thickness;
    be<Vecf3>              beta_m; /*!< Rayleigh */
    be<Vecf3>              beta_p; /*!< Mie */
    be<Vecf3>              beta_m_theta_prefix;
    be<Vecf3>              beta_p_theta_prefix;
    std::array<u8, 0x18>   patchy_fog;
    tag_reference          weather_effect;
    be<f32>                runtime_weight;
    be<f32>                runtime_effect_weight;
};

static_assert(offsetof(atmosphere_setting_t, beta_m) == 0x44);
static_assert(sizeof(atmosphere_setting_t) == 0xA4);

/*! sky_atm_parameters ("skya"): patchy fog sheets and the atmospheres */
struct sky_atmosphere_t
{
    static constexpr std::string_view group = "skya";

    be<u16>                         flags;
    u16                             unknown_0;
    tag_reference                   fog_bitmap;
    std::array<u8, 0x20>            fog_sheets;
    tag_block<atmosphere_setting_t> settings;
    tag_block<std::array<u8, 0x14>> underwater;
};

static_assert(offsetof(sky_atmosphere_t, settings) == 0x34);
static_assert(sizeof(sky_atmosphere_t) == 0x4C);

/*! camera_fx_settings ("cfxs"), the exposure part; values in stops */
struct camera_fx_settings_t
{
    static constexpr std::string_view group = "cfxs";

    be<u16>  flags;
    u16      unknown_0;
    be<f32>  exposure; /*!< Target when auto exposure is off */
    be<f32>  maximum_change;
    be<f32>  blend_speed;
    bounds_f exposure_range;
    be<f32>  auto_exposure_brightness;
    be<f32>  auto_exposure_delay;
};

enum class camera_fx_flags_t : u8
{
    none                     = 0x0,
    force_exposure           = 0x1,
    force_auto_exposure      = 0x2,
    override_exposure_bounds = 0x4,
    override_inherent_bloom  = 0x8,
    override_bloom_intensity = 0x10,
};
C_FLAGS(camera_fx_flags_t, u8);

/*! What a cluster's camera_fx index names: settings plus overrides */
struct camera_fx_palette_t
{
    string_id         name;
    tag_reference     settings; /*!< Empty: globals\default */
    camera_fx_flags_t flags;
    u8                unknown_0;
    u16               unknown_1;
    be<f32>           forced_exposure;
    be<f32>           forced_auto_exposure_brightness;
    bounds_f          exposure_range;
    be<f32>           inherent_bloom;
    be<f32>           bloom_intensity;
};

static_assert(sizeof(camera_fx_palette_t) == 0x30);

/*! A sky: scenery modelled around the level, and the BSPs it shows in */
struct scenario_sky_t
{
    tag_reference object;
    be<i16>       name;
    be<u16>       active_bsps; /*!< One bit per structure BSP, none = all */
};

static_assert(sizeof(scenario_sky_t) == 0x14);

/*! A zone set: which structure BSPs are loaded together */
struct zone_set_t
{
    string_id name;
    be<i32>   pvs;
    be<u32>   flags;
    be<u32>   bsps; /*!< One bit per structure BSP */
    be<u32>   required_designer_zones;
    be<u32>   forbidden_designer_zones;
    be<u32>   cinematic_zones;
    be<i32>   hint_previous;
    be<i32>   audibility;
};

static_assert(sizeof(zone_set_t) == 0x24);

/*! An oriented box: from position along forward, left and up by extents */
struct trigger_volume_t
{
    string_id name;
    be<i16>   object_name;
    be<i16>   node;
    string_id node_name;
    be<Vecf3> forward;
    be<Vecf3> up;
    be<Vecf3> position;
    be<Vecf3> extents;
    be<f32>   unknown_0;
    be<i16>   kill_trigger;
    be<i16>   editor_folder;

    /*! Whether `point` is inside */
    bool contains(Vecf3 const& point) const;
};

static_assert(sizeof(trigger_volume_t) == 0x44);

/*! Entering a trigger volume starts loading one zone set and switches to
 * another */
struct zone_set_switch_trigger_t
{
    be<u16> flags;
    be<i16> begin_zone_set;
    be<i16> trigger_volume;
    be<i16> commit_zone_set;
};

static_assert(sizeof(zone_set_switch_trigger_t) == 0x8);

struct scenario_t
{
    static constexpr std::string_view group = "scnr";

    std::array<u8, 0x14>             unknown_0;
    tag_block<scenario_bsp_t>        bsps;
    tag_reference                    structure_seams;
    tag_block<scenario_sky_t>        skies;
    tag_block<std::array<u8, 0x2C>>  zone_set_pvs;
    tag_block<std::array<u8, 0x64>>  zone_set_audibility;
    tag_block<zone_set_t>            zone_sets;
    tag_block<std::array<u8, 0xC>>   lighting_zone_sets;
    std::array<u8, 0x50>             unknown_1;
    tag_block<scenery_placement_t>   scenery;
    tag_block<palette_entry_t>       scenery_palette;
    tag_block<biped_placement_t>     bipeds;
    tag_block<palette_entry_t>       biped_palette;
    tag_block<vehicle_placement_t>   vehicles;
    tag_block<palette_entry_t>       vehicle_palette;
    tag_block<equipment_placement_t> equipment;
    tag_block<palette_entry_t>       equipment_palette;
    tag_block<weapon_placement_t>    weapons;
    tag_block<palette_entry_t>       weapon_palette;
    std::array<u8, 0xC>              unknown_5;
    tag_block<machine_placement_t>   machines;
    tag_block<palette_entry_t>       machine_palette;
    std::array<u8, 0xF0>             unknown_6;
    tag_block<starting_profile_t>    starting_profiles;
    tag_block<starting_location_t>   starting_locations;
    tag_block<trigger_volume_t>      trigger_volumes;
    std::array<u8, 0xC>              unknown_8;
    tag_block<zone_set_switch_trigger_t> zone_set_switch_triggers;
    std::array<u8, 0x280>            unknown_3;
    tag_block<camera_fx_palette_t>   camera_fx;
    std::array<u8, 0xD8>             unknown_4;
    tag_block<crate_placement_t>     crates;
    tag_block<palette_entry_t>       crate_palette;
    std::array<u8, 0xB0>             unknown_7;
    tag_reference                    camera_effects; /*!< cfxs where no cluster names one */
};

static_assert(offsetof(scenario_t, bsps) == 0x14);
static_assert(offsetof(scenario_t, skies) == 0x30);
static_assert(offsetof(scenario_t, scenery) == 0xBC);
static_assert(offsetof(scenario_t, scenery_palette) == 0xC8);
static_assert(offsetof(scenario_t, bipeds) == 0xD4);
static_assert(offsetof(scenario_t, vehicles) == 0xEC);
static_assert(offsetof(scenario_t, weapons) == 0x11C);
static_assert(offsetof(scenario_t, machines) == 0x140);
static_assert(offsetof(scenario_t, zone_sets) == 0x54);
static_assert(offsetof(scenario_t, starting_profiles) == 0x248);
static_assert(offsetof(scenario_t, starting_locations) == 0x254);
static_assert(offsetof(scenario_t, trigger_volumes) == 0x260);
static_assert(offsetof(scenario_t, zone_set_switch_triggers) == 0x278);
static_assert(offsetof(scenario_t, crates) == 0x5E8);
static_assert(offsetof(scenario_t, camera_fx) == 0x504);
static_assert(offsetof(scenario_t, camera_effects) == 0x6B0);

} // namespace blam::elegy
