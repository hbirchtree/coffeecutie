#pragma once

#include "../common/tag_class.h"
#include "ai.h"
#include "geometry.h"
#include "objects.h"
#include "script.h"
#include "structures.h"

#include <array>

namespace blam::dimeter::scn {

/*!
 * Halo 2 scenario (scnr), Xbox layout from Assembly's Halo2Xbox scnr.xml:
 * placements and palettes, spawns and netgame data, trigger volumes, AI,
 * scripts, cutscene data, structure BSPs and the cluster-side palettes.
 */

struct object_name_t;
struct device_group_t;
struct trigger_volume_t;
struct atmospheric_fog_t;
struct kill_trigger_t;

struct alignas(4) sky_ref_t
{
    tag_reference<tag_class_t::sky> sky;
};

static_assert(sizeof(sky_ref_t) == 8);

enum class scenario_type_t : u16
{
    solo = 0,
    multiplayer,
    main_menu,
    multiplayer_shared,
    single_player_shared,
};

enum class scenario_flags_t : u16
{
    none                                     = 0x0,
    cortana_hack                             = 0x1,
    always_draw_sky                          = 0x2,
    dont_strip_pathfinding                   = 0x4,
    symmetric_multiplayer_map                = 0x8,
    quick_loading_cinematic_only             = 0x10,
    characters_use_previous_mission_weapons  = 0x20,
    lightmaps_smooth_palettes_with_neighbors = 0x40,
    snap_to_white_at_start                   = 0x80,
};
C_FLAGS(scenario_flags_t, u16);

using predicted_resource_type_t = obj::predicted_resource_type_t;

enum class comment_type_t : u32
{
    generic = 0,
};

struct alignas(4) child_scenario_t
{
    tag_reference<tag_class_t::scnr> child;
    u8                               unknown_0[0x10];
};

static_assert(sizeof(child_scenario_t) == 0x18);

struct alignas(4) predicted_resource_t
{
    predicted_resource_type_t type;
    i16                       resource_index_raw;
    tag_index<>                  tag;
};

static_assert(sizeof(predicted_resource_t) == 8);

struct alignas(4) comment_t
{
    Vecf3                 position;
    comment_type_t        type;
    bl_string             name;
    std::array<char, 256> comment;
};

static_assert(sizeof(comment_t) == 0x130);

struct alignas(4) environment_object_t
{
    block_index<geo::structure_bsp_ref_t> bsp;
    i16                                   unknown_0;
    datum_index                           unique_id;
    u32                                   unknown_1;
    tag_class                             object_definition_tag;
    i32                                   object_raw;
    u8                                    unknown_2[0x40 - 0x14];
};

static_assert(sizeof(environment_object_t) == 0x40);

/*! Scripts and triggers address placed objects by these */
struct alignas(4) object_name_t
{
    bl_string name;
    u16       type_raw;            /*!< obj::object_type_t, 0xFFFF = unset */
    i16       placement_index_raw; /*!< into that type's placement block */

    inline obj::object_type_t type() const
    {
        return static_cast<obj::object_type_t>(from_le(type_raw));
    }

    inline i16 placement_index() const
    {
        return from_le(placement_index_raw);
    }
};

static_assert(sizeof(object_name_t) == 0x24);

struct alignas(4) palette_entry_t
{
    object_reference tag;
    u8               unknown_0[32];
};

static_assert(sizeof(palette_entry_t) == 0x28);

struct alignas(4) tag_ref_t
{
    tag_reference<> tag;
};

static_assert(sizeof(tag_ref_t) == 8);

enum class placement_source_t : u8
{
    structure = 0,
    editor,
    dynamic,
    legacy,
};

enum class bsp_policy_t : u8
{
    default_ = 0,
    always_placed,
    manual_bsp_placement,
};

enum class placement_flags_t : u32
{
    none                                 = 0x0,
    not_automatically                    = 0x1,
    not_on_easy                          = 0x2,
    not_on_normal                        = 0x4,
    not_on_hard                          = 0x8,
    lock_type_to_environment_object      = 0x10,
    lock_transform_to_environment_object = 0x20,
    never_placed                         = 0x40,
    lock_name_to_environment_object      = 0x80,
    create_at_rest                       = 0x100,
};
C_FLAGS(placement_flags_t, u32);

enum class transform_flags_t : u16
{
    none     = 0x0,
    mirrored = 0x1,
};
C_FLAGS(transform_flags_t, u16);

/*! One bit per structure BSP of the scenario */
enum class bsp_mask_t : u16
{
    none   = 0x0,
    bsp_0  = 0x1,
    bsp_1  = 0x2,
    bsp_2  = 0x4,
    bsp_3  = 0x8,
    bsp_4  = 0x10,
    bsp_5  = 0x20,
    bsp_6  = 0x40,
    bsp_7  = 0x80,
    bsp_8  = 0x100,
    bsp_9  = 0x200,
    bsp_10 = 0x400,
    bsp_11 = 0x800,
    bsp_12 = 0x1000,
    bsp_13 = 0x2000,
    bsp_14 = 0x4000,
    bsp_15 = 0x8000,
};
C_FLAGS(bsp_mask_t, u16);

enum class change_color_flags_t : u32
{
    none       = 0x0,
    primary    = 0x1,
    secondary  = 0x2,
    tertiary   = 0x4,
    quaternary = 0x8,
};
C_FLAGS(change_color_flags_t, u32);

enum class pathfinding_policy_t : u16
{
    tag_default = 0,
    dynamic,
    cut_out,
    static_,
    none,
};

enum class lightmapping_policy_t : u16
{
    tag_default = 0,
    dynamic,
    per_vertex,
};

enum class multiplayer_game_flags_t : u16
{
    none             = 0x0,
    capture_the_flag = 0x1,
    slayer           = 0x2,
    oddball          = 0x4,
    king_of_the_hill = 0x8,
    juggernaut       = 0x10,
    territories      = 0x20,
    assault          = 0x40,
};
C_FLAGS(multiplayer_game_flags_t, u16);

enum class unit_placement_flags_t : u32
{
    none                    = 0x0,
    dead                    = 0x1,
    closed                  = 0x2,
    not_enterable_by_player = 0x4,
};
C_FLAGS(unit_placement_flags_t, u32);

/*! Equipment and weapon placements */
enum class item_placement_flags_t : u32
{
    none                        = 0x0,
    initially_at_rest           = 0x1,
    accelerates_from_explosions = 0x4,
};
C_FLAGS(item_placement_flags_t, u32);

enum class device_placement_flags_t : u32
{
    none                     = 0x0,
    initially_open           = 0x1,
    initially_off            = 0x2,
    can_change_only_once     = 0x4,
    position_reversed        = 0x8,
    not_usable_from_any_side = 0x10,
};
C_FLAGS(device_placement_flags_t, u32);

enum class machine_placement_flags_t : u32
{
    none                           = 0x0,
    does_not_operate_automatically = 0x1,
    one_sided                      = 0x2,
    never_appears_locked           = 0x4,
    opened_by_melee_attack         = 0x8,
    one_sided_for_player           = 0x10,
    does_not_close_automatically   = 0x20,
};
C_FLAGS(machine_placement_flags_t, u32);

enum class control_placement_flags_t : u32
{
    none                   = 0x0,
    usable_from_both_sides = 0x1,
};
C_FLAGS(control_placement_flags_t, u32);

enum class sound_volume_type_t : u32
{
    sphere = 0,
    vertical_cylinder,
};

enum class light_type_t : u16
{
    sphere = 0,
    orthogonal,
    projective,
    pyramid,
};

enum class light_flags_t : u16
{
    none            = 0x0,
    custom_geometry = 0x1,
    cinematic_only  = 0x4,
};
C_FLAGS(light_flags_t, u16);

enum class lightmap_type_t : u16
{
    use_light_tag_setting = 0,
    dynamic_only,
    dynamic_with_lightmaps,
    lightmaps_only,
};

enum class device_group_flags_t : u32
{
    none                 = 0x0,
    can_change_only_once = 0x1,
};
C_FLAGS(device_group_flags_t, u32);

/*! Object placement, the prefix every placement block shares (scenery,
 * bipeds, vehicles, equipment, weapons, machines, controls, light
 * fixtures, sound scenery, light volumes, crates, creatures) */
struct alignas(4) placement_t
{
    block_index<palette_entry_t> palette;
    block_index<object_name_t>   name;
    placement_flags_t            placement_flags;
    Vecf3                        position;
    Vecf3                        rotation; /*!< yaw, pitch, roll in degrees */
    f32                          scale;
    transform_flags_t            transform_flags;
    bsp_mask_t                   bsp_flags; /*!< manual placement BSPs */
    datum_index                  unique_id;
    block_index<geo::structure_bsp_ref_t> origin_bsp;
    u8                                    type_raw; /*!< obj::object_type_t */
    placement_source_t                    source;
    bsp_policy_t                          bsp_policy;
    u8                                    unknown_0;
    i16                                   editor_folder_raw;

    inline obj::object_type_t type() const
    {
        return static_cast<obj::object_type_t>(type_raw);
    }
};

static_assert(offsetof(placement_t, origin_bsp) == 0x2C);
static_assert(sizeof(placement_t) == 0x34);

/*! Variant and change colours, after the prefix of most placements */
struct alignas(4) object_variant_t
{
    string_id            variant_name;
    change_color_flags_t active_change_colors;
    std::array<u32, 4>   colors; /*!< primary .. quaternary, ARGB bytes */
};

static_assert(sizeof(object_variant_t) == 0x18);

struct alignas(4) pathfinding_ref_t
{
    block_index<geo::structure_bsp_ref_t> bsp;
    i16                                   pathfinding_object_raw;
};

struct alignas(4) scenery_placement_t : placement_t
{
    object_variant_t             variant;
    pathfinding_policy_t         pathfinding_policy;
    lightmapping_policy_t        lightmapping_policy;
    tag_block<pathfinding_ref_t> pathfinding_references;
    i16                          unknown_1;
    multiplayer_game_flags_t     valid_multiplayer_games;
};

static_assert(sizeof(scenery_placement_t) == 0x5C);

struct alignas(4) unit_placement_t : placement_t
{
    object_variant_t       variant;
    f32                    body_vitality; /* fraction */
    unit_placement_flags_t unit_flags;
};

static_assert(sizeof(unit_placement_t) == 0x54);

using biped_placement_t   = unit_placement_t;
using vehicle_placement_t = unit_placement_t;

struct alignas(4) equipment_placement_t : placement_t
{
    item_placement_flags_t equipment_flags;
};

static_assert(sizeof(equipment_placement_t) == 0x38);

struct alignas(4) weapon_placement_t : placement_t
{
    object_variant_t       variant;
    i16                    rounds_left_raw;
    i16                    rounds_loaded_raw;
    item_placement_flags_t weapon_flags;
};

static_assert(sizeof(weapon_placement_t) == 0x54);

/*! Devices: which groups power and position them */
struct alignas(4) device_placement_t : placement_t
{
    block_index<device_group_t> power_group;
    block_index<device_group_t> position_group;
    device_placement_flags_t    device_flags;
};

static_assert(sizeof(device_placement_t) == 0x3C);

struct alignas(4) machine_placement_t : device_placement_t
{
    machine_placement_flags_t    machine_flags;
    tag_block<pathfinding_ref_t> pathfinding_references;
};

static_assert(sizeof(machine_placement_t) == 0x48);

struct alignas(4) control_placement_t : device_placement_t
{
    control_placement_flags_t  control_flags;
    block_index<object_name_t> custom_object_name;
    i16                        unknown_1;
};

static_assert(sizeof(control_placement_t) == 0x44);

struct alignas(4) light_fixture_placement_t : device_placement_t
{
    Vecf3 color;
    f32   intensity;
    f32   falloff_angle; /*!< degrees */
    f32   cutoff_angle;
};

static_assert(sizeof(light_fixture_placement_t) == 0x54);

struct alignas(4) sound_scenery_placement_t : placement_t
{
    sound_volume_type_t volume_type;
    f32                 height;
    Vecf2               override_distance_bounds;
    Vecf2               override_cone_angle_bounds; /*!< degrees */
    f32                 override_outer_cone_gain;
};

static_assert(sizeof(sound_scenery_placement_t) == 0x50);

struct alignas(4) light_volume_placement_t : device_placement_t
{
    light_type_t    light_type;
    light_flags_t   light_flags;
    lightmap_type_t lightmap_type;
    u16             lightmap_flags; /* TODO: bits unknown */
    f32             lightmap_half_life;
    f32             lightmap_light_scale;
    Vecf3           target_point;
    f32             width;
    f32             height_scale;
    f32             field_of_view; /*!< degrees */
    f32             falloff_distance;
    f32             cutoff_distance;
};

static_assert(sizeof(light_volume_placement_t) == 0x6C);

struct alignas(4) crate_placement_t : placement_t
{
    object_variant_t variant;
};

static_assert(sizeof(crate_placement_t) == 0x4C);

using creature_placement_t = placement_t;

struct alignas(4) device_group_t
{
    bl_string            name;
    f32                  initial_value;
    device_group_flags_t flags;
};

static_assert(sizeof(device_group_t) == 0x28);

/*! What a player starts with; scripts pick one by index */
struct alignas(4) player_starting_profile_t
{
    bl_string                        name;
    f32                              starting_health_damage;
    f32                              starting_shield_damage;
    tag_reference<tag_class_t::weap> primary_weapon;
    i16                              primary_rounds_loaded_raw;
    i16                              primary_rounds_total_raw;
    tag_reference<tag_class_t::weap> secondary_weapon;
    i16                              secondary_rounds_loaded_raw;
    i16                              secondary_rounds_total_raw;
    u8                               starting_frag_grenade_count;
    u8                               starting_plasma_grenade_count;
    u8                               starting_grenade_3_count;
    u8                               unknown_0;
};

static_assert(sizeof(player_starting_profile_t) == 0x44);

enum class team_t : u16
{
    red = 0,
    blue,
    yellow,
    green,
    purple,
    orange,
    brown,
    pink,
    neutral,
};

enum class game_type_t : u16
{
    none = 0,
    capture_the_flag,
    slayer,
    oddball,
    king_of_the_hill,
    race,
    headhunter,
    juggernaut,
    territories,
    stub,
    ignored_3,
    ignored_4,
    all_game_types,
    all_except_ctf,
    all_except_ctf_and_race,
};

enum class spawn_type_t : u16
{
    both = 0,
    initial_only,
    respawn_only,
};

enum class campaign_player_type_t : u16
{
    masterchief = 0,
    dervish,
    chief_multiplayer,
    elite_multiplayer,
};

inline std::string_view to_string(game_type_t type)
{
    switch(type)
    {
    case game_type_t::none:
        return "none";
    case game_type_t::capture_the_flag:
        return "ctf";
    case game_type_t::slayer:
        return "slayer";
    case game_type_t::oddball:
        return "oddball";
    case game_type_t::king_of_the_hill:
        return "koth";
    case game_type_t::race:
        return "race";
    case game_type_t::headhunter:
        return "headhunter";
    case game_type_t::juggernaut:
        return "juggernaut";
    case game_type_t::territories:
        return "territories";
    case game_type_t::stub:
        return "stub";
    case game_type_t::ignored_3:
    case game_type_t::ignored_4:
        return "ignored";
    case game_type_t::all_game_types:
        return "all";
    case game_type_t::all_except_ctf:
        return "all except ctf";
    case game_type_t::all_except_ctf_and_race:
        return "all except ctf and race";
    }
    return "?";
}

struct alignas(4) player_spawn_t
{
    Vecf3  position;
    f32    facing_degrees;
    team_t team;
    block_index<geo::structure_bsp_ref_t>
                                bsp_index; /*!< Structure BSP the spawn is in */
    std::array<game_type_t, 4>  game_types;
    std::array<spawn_type_t, 4> spawn_types;
    std::array<string_id, 2>    unused_names;
    campaign_player_type_t      campaign_player_type;
    i16                         unknown_0;
    u32                         unknown_1;

    inline game_type_t game_type(u32 i) const
    {
        return game_types[i & 3];
    }
};

static_assert(offsetof(player_spawn_t, game_types) == 0x14);
static_assert(sizeof(player_spawn_t) == 0x34);

/*! Trigger volume: an oriented box; like Halo 1's it is tested as the
 * axis-aligned span position .. position + extents */
struct alignas(4) trigger_volume_t
{
    string_id name;
    block_index<object_name_t>
              object_name; /*!< Attached to that object, else world */
    i16       node_raw;
    string_id node_name;
    Vecf3     forward;
    Vecf3     up;
    Vecf3     position;
    Vecf3     extents;
    f32       unknown_0;
    block_index<kill_trigger_t> kill_trigger_volume;
    i16                         unknown_1;

    inline bool contains(f32 x, f32 y, f32 z) const
    {
        auto in = [](f32 v, f32 a, f32 e) {
            f32 lo = e < 0 ? a + e : a, hi = e < 0 ? a : a + e;
            return v >= lo && v <= hi;
        };
        return in(x, from_le(position[0]), from_le(extents[0])) &&
               in(y, from_le(position[1]), from_le(extents[1])) &&
               in(z, from_le(position[2]), from_le(extents[2]));
    }
};

static_assert(offsetof(trigger_volume_t, position) == 0x24);
static_assert(sizeof(trigger_volume_t) == 0x44);

struct alignas(4) recorded_animation_t
{
    bl_string        name;
    i8               version;
    i8               raw_animation_data;
    i8               unit_control_data_version;
    i8               unknown_0;
    i16              length_raw; /*!< ticks */
    i16              unknown_1;
    u32              unknown_2;
    data_reference<> event_stream;
};

static_assert(sizeof(recorded_animation_t) == 0x34);

enum class netgame_flag_type_t : u16
{
    ctf_flag_spawn = 0,
    ctf_flag_return,
    assault_bomb_spawn,
    assault_bomb_return,
    oddball_spawn,
    unknown,
    race_checkpoint,
    teleporter_source,
    teleporter_destination,
    headhunter_bin,
    territories_flag,
    king_hill_0,
    king_hill_1,
    king_hill_2,
    king_hill_3,
    king_hill_4,
    king_hill_5,
    king_hill_6,
    king_hill_7,
};

enum class netgame_flag_flags_t : u16
{
    none               = 0x0,
    multiple_flag_bomb = 0x1,
    single_flag_bomb   = 0x2,
    neutral_flag_bomb  = 0x4,
};
C_FLAGS(netgame_flag_flags_t, u16);

enum class netgame_equipment_flags_t : u32
{
    none                          = 0x0,
    levitate                      = 0x1,
    destroy_existing_on_new_spawn = 0x2,
};
C_FLAGS(netgame_equipment_flags_t, u32);

enum class respawn_timer_t : u16
{
    on_pick_up = 0,
    on_body_depletion,
};

enum class equipment_class_t : u8
{
    weapon = 0,
    primary_light_land,
    secondary_light_land,
    primary_heavy_land,
    primary_flying,
    secondary_heavy_land,
    primary_turret,
    secondary_turret,
    grenade,
    powerup,
};

enum class starting_equipment_flags_t : u32
{
    none            = 0x0,
    no_grenades     = 0x1,
    plasma_grenades = 0x2,
};
C_FLAGS(starting_equipment_flags_t, u32);

inline std::string_view to_string(netgame_flag_type_t type)
{
    switch(type)
    {
    case netgame_flag_type_t::ctf_flag_spawn:
        return "ctf flag spawn";
    case netgame_flag_type_t::ctf_flag_return:
        return "ctf flag return";
    case netgame_flag_type_t::assault_bomb_spawn:
        return "assault bomb spawn";
    case netgame_flag_type_t::assault_bomb_return:
        return "assault bomb return";
    case netgame_flag_type_t::oddball_spawn:
        return "oddball spawn";
    case netgame_flag_type_t::unknown:
        return "unknown";
    case netgame_flag_type_t::race_checkpoint:
        return "race checkpoint";
    case netgame_flag_type_t::teleporter_source:
        return "teleporter source";
    case netgame_flag_type_t::teleporter_destination:
        return "teleporter destination";
    case netgame_flag_type_t::headhunter_bin:
        return "headhunter bin";
    case netgame_flag_type_t::territories_flag:
        return "territories flag";
    default:
        return "king hill";
    }
}

struct alignas(4) netgame_flag_t
{
    Vecf3                position;
    f32                  facing_degrees;
    u16                  type_raw;
    u16                  team_raw;
    i16                  identifier_raw;
    netgame_flag_flags_t flags;
    string_id            spawn_object_name;
    string_id            spawn_marker_name;

    inline netgame_flag_type_t type() const
    {
        return static_cast<netgame_flag_type_t>(from_le(type_raw));
    }

    inline team_t team() const
    {
        return static_cast<team_t>(from_le(team_raw));
    }
};

static_assert(sizeof(netgame_flag_t) == 0x20);

/*! Multiplayer item/vehicle spawn */
struct alignas(4) netgame_equipment_t
{
    netgame_equipment_flags_t  flags;
    std::array<game_type_t, 4> game_types;
    i16                        team_index;
    i16                        spawn_time;
    i16                        respawn_on_empty_time;
    respawn_timer_t            respawn_timer_starts;
    equipment_class_t          classification;
    u8                         unknown_0[0x40 - 0x15];
    Vecf3                      position;
    Vecf3                      orientation; /* degrees */
    tag_reference<tag_class_t::itmc, tag_class_t::vehc>
        collection; /* itmc or vehc */
    u8  unknown_1[0x90 - 0x60];

    inline game_type_t game_type(u32 i) const
    {
        return game_types[i & 3];
    }
};

static_assert(offsetof(netgame_equipment_t, position) == 0x40);
static_assert(offsetof(netgame_equipment_t, collection) == 0x58);
static_assert(sizeof(netgame_equipment_t) == 0x90);

struct alignas(4) starting_equipment_t
{
    starting_equipment_flags_t                      flags;
    std::array<game_type_t, 4>                      game_types;
    u8                                              unknown_0[0x3C - 0xC];
    std::array<tag_reference<tag_class_t::itmc>, 6> item_collections;
    u8                                              unknown_1[0x9C - 0x6C];

    inline game_type_t game_type(u32 i) const
    {
        return game_types[i & 3];
    }
};

static_assert(offsetof(starting_equipment_t, item_collections) == 0x3C);
static_assert(sizeof(starting_equipment_t) == 0x9C);

/*! Entering the volume while in the source BSP switches to the destination */
struct alignas(2) bsp_switch_trigger_t
{
    block_index<trigger_volume_t>         trigger_volume;
    block_index<geo::structure_bsp_ref_t> source_bsp;
    block_index<geo::structure_bsp_ref_t> destination_bsp;
    i16                                   unknown_0[4];
};

static_assert(sizeof(bsp_switch_trigger_t) == 0xE);

struct alignas(4) decal_t
{
    block_index<tag_ref_t> palette;
    i8                     yaw;
    i8                     pitch;
    Vecf3                  position;
};

static_assert(sizeof(decal_t) == 0x10);

struct alignas(4) cutscene_flag_t
{
    u32       unknown_0;
    bl_string name;
    Vecf3     position;
    Vecf2     facing; /*!< yaw, pitch in degrees */
};

static_assert(sizeof(cutscene_flag_t) == 0x38);

enum class camera_point_flags_t : u16
{
    none             = 0x0,
    edit_as_relative = 0x1,
};
C_FLAGS(camera_point_flags_t, u16);

enum class camera_point_type_t : u16
{
    normal = 0,
    ignore_target_orientation,
    dolly,
    ignore_target_updates,
};

struct alignas(4) cutscene_camera_point_t
{
    camera_point_flags_t flags;
    camera_point_type_t  type;
    bl_string            name;
    u32                  unknown_0;
    Vecf3                position;
    Vecf3                orientation; /*!< yaw, pitch, roll in radians */
};

static_assert(offsetof(cutscene_camera_point_t, position) == 0x28);
static_assert(sizeof(cutscene_camera_point_t) == 0x40);

enum class justification_t : u16
{
    left = 0,
    right,
    center,
};

enum class font_t : u16
{
    terminal = 0,
    body_text,
    title,
    super_large,
    large_body_text,
    split_screen_hud_message,
    full_screen_hud_message,
    english_body_text,
    hud_number,
    subtitle,
    main_menu,
};

struct alignas(4) cutscene_title_t
{
    string_id          name;
    std::array<i16, 4> text_bounds; /*!< top, left, bottom, right */
    justification_t    justification;
    font_t             font;
    u32                text_color;
    u32                shadow_color;
    f32                fade_in_time;
    f32                up_time;
    f32                fade_out_time;
};

static_assert(sizeof(cutscene_title_t) == 0x24);

struct alignas(4) scenario_resources_t
{
    tag_block<tag_ref_t> references;
    tag_block<tag_ref_t> script_source;
    tag_block<tag_ref_t> ai_resources;
};

static_assert(sizeof(scenario_resources_t) == 0x18);

struct alignas(4) environment_object_id_t
{
    datum_index                           unique_id;
    block_index<geo::structure_bsp_ref_t> origin_bsp;
    u8                                    type_raw; /*!< obj::object_type_t */
    placement_source_t                    source;

    inline obj::object_type_t type() const
    {
        return static_cast<obj::object_type_t>(type_raw);
    }
};

static_assert(sizeof(environment_object_id_t) == 8);

struct alignas(4) old_structure_physics_t
{
    data_reference<>                   mopp_code;
    tag_block<environment_object_id_t> environment_object_identifiers;
    u8                                 unknown_0[4];
    Vecf3                              mopp_bounds_min;
    Vecf3                              mopp_bounds_max;
};

static_assert(sizeof(old_structure_physics_t) == 0x2C);

struct alignas(4) unit_seats_mapping_t
{
    datum_index unit;
    u32         seats; /*!< bit n = seat n */
};

static_assert(sizeof(unit_seats_mapping_t) == 8);

struct alignas(2) kill_trigger_t
{
    block_index<trigger_volume_t> trigger_volume;
};

static_assert(sizeof(kill_trigger_t) == 2);

/*! Per-cluster palette choice: index into the matching palette */
template<typename T>
struct alignas(2) palette_index_t
{
    block_index<T> index;
    i16            unknown_0;
};

struct alignas(4) cluster_centroid_t
{
    Vecf3 centroid;
};

/*! Per-BSP, per-cluster sound, weather and fog assignments */
struct alignas(4) cluster_data_t
{
    tag_reference<tag_class_t::sbsp> bsp;
    tag_block<palette_index_t<geo::background_sound_palette_t>>
        background_sounds;
    tag_block<palette_index_t<geo::sound_environment_palette_t>>
                                                       sound_environments;
    i32                                                bsp_checksum;
    tag_block<cluster_centroid_t>                      centroids;
    tag_block<palette_index_t<geo::weather_palette_t>> weather;
    tag_block<palette_index_t<atmospheric_fog_t>>      atmospheric_fog;
};

static_assert(sizeof(cluster_data_t) == 0x34);

enum class spawn_overload_type_t : u16
{
    enemy = 0,
    friend_,
    enemy_vehicle,
    friendly_vehicle,
    empty_vehicle,
    oddball_inclusion,
    oddball_exclusion,
    hill_inclusion,
    hill_exclusion,
    last_race_flag,
    dead_ally,
    controlled_territory,
};

struct alignas(4) spawn_overload_t
{
    spawn_overload_type_t type;
    i16                   unknown_0;
    f32                   inner_radius;
    f32                   outer_radius;
    f32                   weight;
};

static_assert(sizeof(spawn_overload_t) == 0x10);

enum class team_flags_t : u32
{
    none    = 0x0,
    red     = 0x1,
    blue    = 0x2,
    yellow  = 0x4,
    green   = 0x8,
    purple  = 0x10,
    orange  = 0x20,
    brown   = 0x40,
    pink    = 0x80,
    neutral = 0x100,
};
C_FLAGS(team_flags_t, u32);

enum class game_flags_t : u32
{
    none             = 0x0,
    slayer           = 0x1,
    oddball          = 0x2,
    king_of_the_hill = 0x4,
    capture_the_flag = 0x8,
    race             = 0x10,
    headhunter       = 0x20,
    juggernaut       = 0x40,
    territories      = 0x80,
};
C_FLAGS(game_flags_t, u32);

enum class spawn_zone_flags_t : u32
{
    none                  = 0x0,
    disabled_if_flag_home = 0x1,
    disabled_if_flag_away = 0x2,
    disabled_if_bomb_home = 0x4,
    disabled_if_bomb_away = 0x8,
};
C_FLAGS(spawn_zone_flags_t, u32);

/*! Multiplayer spawn influence zone */
struct alignas(4) spawn_zone_t
{
    string_id          name;
    team_flags_t       relevant_team;
    game_flags_t       relevant_games;
    spawn_zone_flags_t flags;
    Vecf3              position;
    f32                lower_height;
    f32                upper_height;
    f32                inner_radius;
    f32                outer_radius;
    f32                weight;
};

static_assert(sizeof(spawn_zone_t) == 0x30);

struct alignas(4) spawn_data_t
{
    f32                         dynamic_spawn_lower_height;
    f32                         dynamic_spawn_upper_height;
    f32                         game_objective_reset_height;
    u8                          unknown_0[0x48 - 0xC];
    tag_block<spawn_overload_t> dynamic_spawn_overloads;
    tag_block<spawn_zone_t>     static_respawn_zones;
    tag_block<spawn_zone_t>     static_initial_spawn_zones;
};

static_assert(sizeof(spawn_data_t) == 0x60);

struct alignas(4) atmospheric_fog_t
{
    string_id name;
    Vecf3     color;
    f32       spread_distance;
    u32       unknown_0;
    f32       maximum_density;
    f32       start_distance;
    f32       opaque_distance;
    Vecf3     secondary_color;
    u32       unknown_1;
    f32       secondary_maximum_density;
    f32       secondary_start_distance;
    f32       secondary_opaque_distance;
    u8        unknown_2[0xF4 - 0x40];
};

static_assert(sizeof(atmospheric_fog_t) == 0xF4);

struct alignas(4) planar_fog_t
{
    string_id                       name;
    tag_reference<tag_class_t::fog> planar_fog;
    i16                             unknown_0[2];
};

static_assert(sizeof(planar_fog_t) == 0x10);

struct alignas(4) flock_source_t
{
    Vecf3 position;
    Vecf2 starting; /*!< yaw, pitch in degrees */
    f32   radius;
    f32   weight;
};

static_assert(sizeof(flock_source_t) == 0x1C);

struct alignas(4) flock_sink_t
{
    Vecf3 position;
    f32   radius;
};

enum class flock_flags_t : u16
{
    none                      = 0x0,
    hard_boundaries_on_volume = 0x1,
    initially_stopped         = 0x2,
};
C_FLAGS(flock_flags_t, u16);

struct alignas(4) flock_t
{
    block_index<geo::structure_bsp_ref_t> bsp;
    i16                                   unknown_0;
    block_index<trigger_volume_t>         bounding_trigger_volume;
    flock_flags_t                         flags;
    f32                                   ecology_margin;
    tag_block<flock_source_t>             sources;
    tag_block<flock_sink_t>               sinks;
    f32                                   production_frequency;
    Vecf2                                 scale;
    tag_reference<tag_class_t::crea>      creature;
    std::array<i16, 2>                    boid_count;
    u8                                    unknown_1[0x80 - 0x34];
    string_id                             name;
};

static_assert(offsetof(flock_t, creature) == 0x28);
static_assert(sizeof(flock_t) == 0x84);

struct alignas(2) bsp_transition_volume_t
{
    i16                           unknown_0;
    i16                           bsp_index_key_raw;
    block_index<trigger_volume_t> trigger_volume;
    i16                           unknown_1;
};

static_assert(sizeof(bsp_transition_volume_t) == 8);

struct alignas(4) bsp_lighting_t
{
    tag_reference<tag_class_t::sbsp> bsp;
    u8                               unknown_0[8];
};

static_assert(sizeof(bsp_lighting_t) == 0x10);

/*! scnr body (Xbox offsets, 0x3E0 bytes) */
struct alignas(4) scenario_t
{
    static constexpr tag_class_t tag_class = tag_class_t::scnr;

    tag_reference<>                      unused;
    tag_block<sky_ref_t>                 skies;
    u16                                  type_raw;
    scenario_flags_t                     flags;
    tag_block<child_scenario_t>          child_scenarios;
    f32                                  local_north; /*!< degrees */
    tag_block<predicted_resource_t>      predicted_resources;
    u8                                   unknown_0[8]; /*!< functions block */
    data_reference<>                     editor_scenario_data;
    tag_block<comment_t>                 comments;
    tag_block<environment_object_t>      environment_objects;
    tag_block<object_name_t>             object_names;
    tag_block<scenery_placement_t>       scenery;
    tag_block<palette_entry_t>           scenery_palette;
    tag_block<biped_placement_t>         bipeds;
    tag_block<palette_entry_t>           biped_palette;
    tag_block<vehicle_placement_t>       vehicles;
    tag_block<palette_entry_t>           vehicle_palette;
    tag_block<equipment_placement_t>     equipment;
    tag_block<palette_entry_t>           equipment_palette;
    tag_block<weapon_placement_t>        weapons;
    tag_block<palette_entry_t>           weapon_palette;
    tag_block<device_group_t>            device_groups;
    tag_block<machine_placement_t>       machines;
    tag_block<palette_entry_t>           machine_palette;
    tag_block<control_placement_t>       controls;
    tag_block<palette_entry_t>           control_palette;
    tag_block<light_fixture_placement_t> light_fixtures;
    tag_block<palette_entry_t>           light_fixture_palette;
    tag_block<sound_scenery_placement_t> sound_scenery;
    tag_block<palette_entry_t>           sound_scenery_palette;
    tag_block<light_volume_placement_t>  light_volumes;
    tag_block<palette_entry_t>           light_volume_palette;
    tag_block<player_starting_profile_t> player_starting_profiles;
    tag_block<player_spawn_t>            player_starting_locations;
    tag_block<trigger_volume_t>          trigger_volumes;
    tag_block<recorded_animation_t>      recorded_animations;
    tag_block<netgame_flag_t>            netgame_flags;
    tag_block<netgame_equipment_t>       netgame_equipment;
    tag_block<starting_equipment_t>      starting_equipment;
    tag_block<bsp_switch_trigger_t>      bsp_switch_triggers;
    tag_block<decal_t>                   decals;
    tag_block<tag_ref_t>                 decal_palette;
    tag_block<palette_entry_t>           detail_object_collection_palette;
    tag_block<ai::style_ref_t>           style_palette;
    tag_block<ai::squad_group_t>         squad_groups;
    tag_block<ai::squad_t>               squads;
    tag_block<ai::zone_t>                zones;
    tag_block<ai::mission_scene_t>       mission_scenes;
    tag_block<ai::character_ref_t>       character_palette;
    u8 unknown_1[8]; /*!< AI pathfinding data block */
    tag_block<ai::animation_reference_t> ai_animation_references;
    tag_block<ai::named_reference_t>     ai_script_references;
    tag_block<ai::named_reference_t>     ai_recording_references;
    tag_block<ai::conversation_t>        ai_conversations;
    data_reference<>                     script_syntax_data;
    data_reference<>                     script_string_data;
    tag_block<hsc::script_t>             scripts;
    tag_block<hsc::global_t>             globals;
    tag_block<hsc::script_reference_t>   script_references;
    u8                                  unknown_2[8]; /*!< source files block */
    tag_block<hsc::scripting_data_t>    scripting_data;
    tag_block<cutscene_flag_t>          cutscene_flags;
    tag_block<cutscene_camera_point_t>  cutscene_camera_points;
    tag_block<cutscene_title_t>         cutscene_titles;
    tag_reference<tag_class_t::unic>    custom_object_names;
    tag_reference<tag_class_t::unic>    chapter_title_text;
    tag_reference<tag_class_t::hmt>     hud_messages;
    tag_block<geo::structure_bsp_ref_t> structure_bsps;
    tag_block<scenario_resources_t>     scenario_resources;
    tag_block<old_structure_physics_t>  old_structure_physics;
    tag_block<unit_seats_mapping_t>     unit_seats_mapping;
    tag_block<kill_trigger_t>           kill_triggers;
    tag_block<hsc::expression_t>        script_expressions;
    tag_block<ai::order_t>              orders;
    tag_block<ai::trigger_t>            ai_triggers;
    tag_block<geo::background_sound_palette_t>  background_sound_palette;
    tag_block<geo::sound_environment_palette_t> sound_environment_palette;
    tag_block<geo::weather_palette_t>           weather_palette;
    u8                        unknown_3[0x290 - 0x268]; /*!< null blocks */
    tag_block<cluster_data_t> cluster_data;
    std::array<i32, 32>       object_salts;
    tag_block<spawn_data_t>   spawn_data;
    tag_reference<tag_class_t::sfx>    sound_effect_collection;
    tag_block<crate_placement_t>       crates;
    tag_block<palette_entry_t>         crate_palette;
    tag_reference<tag_class_t::gldf>   global_lighting;
    tag_block<atmospheric_fog_t>       atmospheric_fog_palette;
    tag_block<planar_fog_t>            planar_fog_palette;
    tag_block<flock_t>                 flocks;
    tag_reference<tag_class_t::unic>   subtitles;
    tag_block<u8>                      decorators; /* elem 0x30 */
    tag_block<creature_placement_t>    creatures;
    tag_block<palette_entry_t>         creature_palette;
    tag_block<tag_ref_t>               decorator_palette;
    tag_block<bsp_transition_volume_t> bsp_transition_volumes;
    tag_block<bsp_lighting_t>          structure_bsp_lighting;
    u8                                 unknown_4[0x3E0 - 0x390];

    inline scenario_type_t type() const
    {
        return static_cast<scenario_type_t>(from_le(type_raw));
    }

    /*! The expression and string tables of this scenario's scripts */
    inline hsc::syntax_t syntax(tag_magic const& magic) const
    {
        hsc::syntax_t out;
        if(auto nodes = script_expressions.data(magic); !nodes.has_error())
            out.nodes = nodes.value();
        if(auto strings = script_string_data.data(magic); !strings.has_error())
            out.strings = strings.value();
        if(auto list = scripts.data(magic); !list.has_error())
            out.scripts = list.value();
        if(auto list = globals.data(magic); !list.has_error())
            out.globals = list.value();
        return out;
    }
};

static_assert(offsetof(scenario_t, skies) == 0x08);
static_assert(offsetof(scenario_t, object_names) == 0x48);
static_assert(offsetof(scenario_t, scenery) == 0x50);
static_assert(offsetof(scenario_t, device_groups) == 0xA0);
static_assert(offsetof(scenario_t, machines) == 0xA8);
static_assert(offsetof(scenario_t, light_fixtures) == 0xC8);
static_assert(offsetof(scenario_t, player_starting_profiles) == 0xF8);
static_assert(offsetof(scenario_t, player_starting_locations) == 0x100);
static_assert(offsetof(scenario_t, netgame_flags) == 0x118);
static_assert(offsetof(scenario_t, netgame_equipment) == 0x120);
static_assert(offsetof(scenario_t, bsp_switch_triggers) == 0x130);
static_assert(offsetof(scenario_t, squad_groups) == 0x158);
static_assert(offsetof(scenario_t, squads) == 0x160);
static_assert(offsetof(scenario_t, zones) == 0x168);
static_assert(offsetof(scenario_t, character_palette) == 0x178);
static_assert(offsetof(scenario_t, script_syntax_data) == 0x1A8);
static_assert(offsetof(scenario_t, scripts) == 0x1B8);
static_assert(offsetof(scenario_t, globals) == 0x1C0);
static_assert(offsetof(scenario_t, scripting_data) == 0x1D8);
static_assert(offsetof(scenario_t, cutscene_flags) == 0x1E0);
static_assert(offsetof(scenario_t, structure_bsps) == 0x210);
static_assert(offsetof(scenario_t, script_expressions) == 0x238);
static_assert(offsetof(scenario_t, orders) == 0x240);
static_assert(offsetof(scenario_t, ai_triggers) == 0x248);
static_assert(offsetof(scenario_t, cluster_data) == 0x290);
static_assert(offsetof(scenario_t, spawn_data) == 0x318);
static_assert(offsetof(scenario_t, crates) == 0x328);
static_assert(offsetof(scenario_t, flocks) == 0x350);
static_assert(offsetof(scenario_t, creatures) == 0x368);
static_assert(offsetof(scenario_t, structure_bsp_lighting) == 0x388);
static_assert(sizeof(scenario_t) == 0x3E0);

using item_collection_t = obj::item_collection_t;

} // namespace blam::dimeter::scn
