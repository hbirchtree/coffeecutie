#pragma once

#include "collision.h"
#include "structures.h"

#include <array>

namespace blam::dimeter::geo {
struct model_node_t;
} // namespace blam::dimeter::geo

namespace blam::dimeter::obj {

struct seat_t;
struct magazine_t;
struct model_variant_t;
struct barrel_t;

namespace matg {
struct material_t;
} // namespace matg

/*!
 * Halo 2 object tags: the obje prefix every placeable shares, the unit
 * and item layers, and the leaf types (bipd, vehi, weap, eqip, mach, ctrl,
 * crea, bloc), plus the tags they hang off (hlmt, coll, phmo, char, itmc,
 * vehc) and the globals (matg). Layouts from Assembly's Halo2 plugins,
 * which the Xbox maps share except where the Halo2Xbox set overrides
 * (vehi).
 */

enum class object_type_t : u16
{
    biped = 0,
    vehicle,
    weapon,
    equipment,
    garbage,
    projectile,
    scenery,
    machine,
    control,
    light_fixture,
    sound_scenery,
    crate,
    creature,
};

enum class object_flags_t : u16
{
    none                                           = 0x0,
    does_not_cast_shadow                           = 0x1,
    search_cardinal_direction_lightmaps_on_failure = 0x2,
    not_a_pathfinding_obstacle                     = 0x8,
    extension_of_parent                            = 0x10,
    does_not_cause_collision_damage                = 0x20,
    early_mover                                    = 0x40,
    early_mover_localized_physics                  = 0x80,
    use_static_massive_lightmap_sample             = 0x100,
    object_scales_attachments                      = 0x200,
    inherits_players_appearance                    = 0x400,
    dead_bipeds_cant_localize                      = 0x800,
    attach_to_clusters_by_dynamic_sphere           = 0x1000,
    effects_do_not_spawn_objects_in_multiplayer    = 0x2000,
};
C_FLAGS(object_flags_t, u16);

enum class object_runtime_flags_t : u32
{
    none                  = 0x0,
    change_colors_allowed = 0x1,
};
C_FLAGS(object_runtime_flags_t, u32);

enum class lightmap_shadow_mode_t : u16
{
    default_ = 0,
    never,
    always,
};

enum class sweetener_size_t : u8
{
    small = 0,
    medium,
    large,
};

enum class ai_properties_flags_t : u32
{
    none                         = 0x0,
    destroyable_cover            = 0x1,
    pathfinding_ignore_when_dead = 0x2,
    dynamic_cover                = 0x4,
};
C_FLAGS(ai_properties_flags_t, u32);

/*! Obstacle and leap sizes, as AI sees objects */
enum class ai_size_t : u16
{
    default_ = 0,
    tiny,
    small,
    medium,
    large,
    huge,
    immobile,
};

enum class leap_jump_speed_t : u16
{
    none = 0,
    down,
    step,
    crouch,
    stand,
    storey,
    tower,
    infinite,
};

enum class function_flags_t : u32
{
    none                            = 0x0,
    invert                          = 0x1,
    mapping_does_not_control_active = 0x2,
    always_active                   = 0x4,
    random_time_offset              = 0x8,
};
C_FLAGS(function_flags_t, u32);

enum class attachment_color_t : u16
{
    none = 0,
    primary,
    secondary,
    tertiary,
    quaternary,
};

enum class predicted_resource_type_t : u16
{
    bitmap = 0,
    sound,
    render_model_geometry,
    cluster_geometry,
    cluster_instanced_geometry,
    lightmap_geometry_object_buckets,
    lightmap_geometry_instance_buckets,
    lightmap_cluster_bitmaps,
    lightmap_instance_bitmaps,
};

enum class team_t : u16
{
    default_ = 0,
    player,
    human,
    covenant,
    flood,
    sentinel,
    heretic,
    prophet,
    unused_8,
    unused_9,
    unused_10,
    unused_11,
    unused_12,
    unused_13,
    unused_14,
    unused_15,
};

enum class unit_flags_t : u32
{
    none                            = 0x0,
    circular_aiming                 = 0x1,
    destroyed_after_dying           = 0x2,
    half_speed_interpolation        = 0x4,
    fires_from_camera               = 0x8,
    entrance_inside_bounding_sphere = 0x10,
    doesnt_show_readied_weapon      = 0x20,
    causes_passenger_dialogue       = 0x40,
    resists_pings                   = 0x80,
    melee_attack_is_fatal           = 0x100,
    dont_reface_during_pings        = 0x200,
    has_no_aiming                   = 0x400,
    simple_creature                 = 0x800,
    impact_melee_attaches_to_unit   = 0x1000,
    impact_melee_dies_on_shields    = 0x2000,
    cannot_open_doors_automatically = 0x4000,
    melee_attackers_cannot_attach   = 0x8000,
    not_instantly_killed_by_melee   = 0x10000,
    shield_sapping                  = 0x20000,
    runs_around_flaming             = 0x40000,
    inconsequential                 = 0x80000,
    special_cinematic_unit          = 0x100000,
    ignored_by_autoaiming           = 0x200000,
    shields_fry_infection_forms     = 0x400000,
    acts_as_gunner_for_parent       = 0x2000000,
    controlled_by_parent_gunner     = 0x4000000,
    parents_primary_weapon          = 0x8000000,
    unit_has_boost                  = 0x10000000,
};
C_FLAGS(unit_flags_t, u32);

enum class sound_volume_t : u16
{
    silent = 0,
    medium,
    loud,
    shout,
    quiet,
};

enum class blip_size_t : u16
{
    medium = 0,
    small,
    large,
};

enum class unit_grenade_type_t : u16
{
    human_fragmentation = 0,
    covenant_plasma,
};

enum class seat_flags_t : u32
{
    none                                       = 0x0,
    invisible                                  = 0x1,
    locked                                     = 0x2,
    driver                                     = 0x4,
    gunner                                     = 0x8,
    third_person_camera                        = 0x10,
    allows_weapons                             = 0x20,
    third_person_on_enter                      = 0x40,
    first_person_camera_slaved_to_gun          = 0x80,
    allow_vehicle_communication_animations     = 0x100,
    not_valid_without_driver                   = 0x200,
    allow_ai_noncombatants                     = 0x400,
    boarding_seat                              = 0x800,
    ai_firing_disabled_by_maximum_acceleration = 0x1000,
    boarding_enters_seat                       = 0x2000,
    boarding_need_any_passenger                = 0x4000,
    controls_open_and_close                    = 0x8000,
    invalid_for_player                         = 0x10000,
    invalid_for_non_player                     = 0x20000,
    gunner_player_only                         = 0x40000,
    invisible_under_major_damage               = 0x80000,
};
C_FLAGS(seat_flags_t, u32);

enum class ai_seat_type_t : u16
{
    none = 0,
    passenger,
    gunner,
    small_cargo,
    large_cargo,
    driver,
};

inline std::string_view to_string(object_type_t type)
{
    switch(type)
    {
    case object_type_t::biped:
        return "biped";
    case object_type_t::vehicle:
        return "vehicle";
    case object_type_t::weapon:
        return "weapon";
    case object_type_t::equipment:
        return "equipment";
    case object_type_t::garbage:
        return "garbage";
    case object_type_t::projectile:
        return "projectile";
    case object_type_t::scenery:
        return "scenery";
    case object_type_t::machine:
        return "machine";
    case object_type_t::control:
        return "control";
    case object_type_t::light_fixture:
        return "light fixture";
    case object_type_t::sound_scenery:
        return "sound scenery";
    case object_type_t::crate:
        return "crate";
    case object_type_t::creature:
        return "creature";
    }
    return "?";
}

struct alignas(4) ai_properties_t
{
    ai_properties_flags_t flags;
    string_id             ai_type_name;
    u32                   unknown_0;
    ai_size_t             ai_size;
    leap_jump_speed_t     leap_jump_speed;
};

static_assert(sizeof(ai_properties_t) == 0x10);

struct alignas(4) object_function_t
{
    function_flags_t flags;
    string_id        import_name;
    string_id        export_name;
    string_id        turn_off_with;
    f32              minimum_value;
    data_reference<> default_function;
    string_id        scale_by;
};

static_assert(sizeof(object_function_t) == 0x20);

struct alignas(4) attachment_t
{
    tag_reference<
        tag_class_t::ligh,
        tag_class_t::lsnd,
        tag_class_t::effe,
        tag_class_t::cont,
        tag_class_t::mgs2>
                       type; /*!< ligh, lsnd, effe, ... */
    string_id          marker;
    attachment_color_t change_color;
    i16                unknown_0;
    string_id          primary_scale;
    string_id          secondary_scale;
};

static_assert(sizeof(attachment_t) == 0x18);

struct alignas(4) tag_ref_t
{
    tag_reference<> tag;
};

static_assert(sizeof(tag_ref_t) == 8);

struct alignas(4) change_color_t
{
    tag_block<u8> initial_permutations; /*!< elem 0x20 */
    tag_block<u8> functions;            /*!< elem 0x28 */
};

static_assert(sizeof(change_color_t) == 0x10);

struct alignas(4) predicted_resource_t
{
    predicted_resource_type_t type;
    i16                       resource_index_raw;
    tag_id<>                  tag;
};

static_assert(sizeof(predicted_resource_t) == 8);

/*! obje: the 0xBC-byte prefix of every object tag */
struct alignas(4) object_t
{
    u16                              type_raw;
    object_flags_t                   flags;
    f32                              bounding_radius;
    Vecf3                            bounding_offset;
    f32                              acceleration_scale;
    lightmap_shadow_mode_t           lightmap_shadow_mode;
    sweetener_size_t                 sweetener_size;
    u8                               unknown_0;
    object_runtime_flags_t           runtime_flags;
    f32                              dynamic_light_sphere_radius;
    Vecf3                            dynamic_light_sphere_offset;
    string_id                        default_model_variant;
    tag_reference<tag_class_t::hlmt> model; /*!< hlmt */
    tag_reference<tag_class_t::bloc> crate_object;
    tag_reference<tag_class_t::shad> modifier_shader;
    tag_reference<tag_class_t::effe> creation_effect;
    tag_reference<tag_class_t::foot> material_effects;
    tag_block<ai_properties_t>       ai_properties;
    tag_block<object_function_t>     functions;
    f32                              apply_collision_damage_scale;
    Vecf2                            game_acceleration;
    Vecf2                            game_scale;
    Vecf2                            absolute_acceleration;
    Vecf2                            absolute_scale;
    i16                              hud_text_message_index_raw;
    i16                              unknown_1;
    tag_block<attachment_t>          attachments;
    tag_block<tag_ref_t>             widgets;
    tag_block<u8>                    old_functions; /* elem 0x50 */
    tag_block<change_color_t>        change_colors;
    tag_block<predicted_resource_t>  predicted_resources;

    inline object_type_t type() const
    {
        return static_cast<object_type_t>(from_le(type_raw));
    }
};

static_assert(offsetof(object_t, model) == 0x34);
static_assert(offsetof(object_t, attachments) == 0x94);
static_assert(sizeof(object_t) == 0xBC);

struct alignas(4) seat_t
{
    seat_flags_t                     flags;
    string_id                        seat_animation;
    string_id                        seat_marker_name;
    string_id                        entry_marker_name;
    string_id                        boarding_grenade_marker;
    string_id                        boarding_grenade_string;
    string_id                        boarding_melee_string;
    f32                              ping_scale;
    f32                              turnover_time;
    Vecf3                            acceleration_range;
    f32                              acceleration_action_scale;
    f32                              acceleration_attach_scale;
    f32                              ai_scariness;
    ai_seat_type_t                   ai_seat_type;
    block_index<seat_t>              boarding_seat;
    f32                              listener_interpolation_factor;
    Vecf2                            yaw_rate_bounds;
    Vecf2                            pitch_rate_bounds;
    f32                              minimum_speed_reference;
    f32                              maximum_speed_reference;
    f32                              speed_exponent;
    string_id                        camera_marker_name;
    string_id                        camera_submerged_marker_name;
    f32                              pitch_auto_level;
    Vecf2                            pitch_range;
    tag_block<tag_ref_t>             camera_tracks;
    tag_block<tag_ref_t>             unit_hud_interface;
    string_id                        enter_seat_string;
    Vecf2                            yaw_range;
    tag_reference<tag_class_t::weap> built_in_gunner;
    f32                              entry_radius;
    f32                              entry_marker_cone_angle;
    f32                              entry_marker_facing_angle;
    f32                              maximum_relative_velocity;
    string_id                        invisible_seat_region;
    i32                              invisible_seat_region_index_raw;
};

static_assert(offsetof(seat_t, camera_marker_name) == 0x60);
static_assert(offsetof(seat_t, built_in_gunner) == 0x90);
static_assert(sizeof(seat_t) == 0xB0);

struct alignas(4) posture_t
{
    string_id name;
    Vecf3     pill_offset;
};

static_assert(sizeof(posture_t) == 0x10);

struct alignas(4) dialogue_variant_t
{
    i16                              variant_number_raw;
    i16                              unknown_0;
    tag_reference<tag_class_t::udlg> dialogue;
};

static_assert(sizeof(dialogue_variant_t) == 0xC);

struct alignas(4) powered_seat_t
{
    f32 driver_powerup_time;
    f32 driver_powerdown_time;
};

/*! unit: bipeds and vehicles */
struct alignas(4) unit_t : object_t
{
    unit_flags_t                      unit_flags;
    team_t                            default_team;
    sound_volume_t                    constant_sound_volume;
    tag_reference<tag_class_t::effe>  integrated_light_toggle;
    f32                               camera_field_of_view; /* degrees */
    f32                               camera_stiffness;
    string_id                         camera_marker_name;
    string_id                         camera_submerged_marker_name;
    f32                               pitch_auto_level;
    Vecf2                             pitch_range;
    tag_block<tag_ref_t>              camera_tracks;
    Vecf3                             acceleration_range;
    f32                               acceleration_action_scale;
    f32                               acceleration_attach_scale;
    f32                               soft_ping_threshold;
    f32                               soft_ping_interrupt_time;
    f32                               hard_ping_threshold;
    f32                               hard_ping_interrupt_time;
    f32                               hard_death_threshold;
    f32                               feign_death_threshold;
    f32                               feign_death_time;
    f32                               distance_of_evade_animation;
    f32                               distance_of_dive_animation;
    f32                               stunned_movement_threshold;
    f32                               feign_death_chance;
    f32                               feign_repeat_chance;
    tag_reference<tag_class_t::char_> spawned_turret_character;
    std::array<i16, 2>                spawned_actor_count;
    f32                               spawned_velocity;
    f32                               aiming_velocity_maximum;
    f32                               aiming_acceleration_maximum;
    f32                               casual_aiming_modifier;
    f32                               looking_velocity_maximum;
    f32                               looking_acceleration_maximum;
    string_id                         right_hand_node;
    string_id                         left_hand_node;
    string_id                         preferred_gun_node;
    tag_reference<tag_class_t::jpt>   melee_damage;
    tag_reference<tag_class_t::jpt>   boarding_melee_damage;
    tag_reference<tag_class_t::jpt>   boarding_melee_response;
    tag_reference<tag_class_t::jpt>   landing_melee_damage;
    tag_reference<tag_class_t::jpt>   flurry_melee_damage;
    tag_reference<tag_class_t::jpt>   obstacle_smash_damage;
    blip_size_t                       motion_sensor_blip_size;
    i16                               unknown_2;
    tag_block<posture_t>              postures;
    tag_block<tag_ref_t>              new_hud_interfaces;
    tag_block<dialogue_variant_t>     dialogue_variants;
    f32                               grenade_velocity;
    unit_grenade_type_t               grenade_type;
    i16                               grenade_count_raw;
    tag_block<powered_seat_t>         powered_seats;
    tag_block<tag_ref_t>              weapons;
    tag_block<seat_t>                 seats;
    f32                               boost_peak_power;
    f32                               boost_rise_power;
    f32                               boost_peak_time;
    f32                               boost_fall_power;
    f32                               boost_dead_time;
    f32                               lipsync_attack_weight;
    f32                               lipsync_decay_weight;
};

static_assert(offsetof(unit_t, integrated_light_toggle) == 0xC4);
static_assert(offsetof(unit_t, spawned_turret_character) == 0x134);
static_assert(offsetof(unit_t, seats) == 0x1C8);
static_assert(sizeof(unit_t) == 0x1EC);

enum class biped_flags_t : u32
{
    none                            = 0x0,
    turns_without_animating         = 0x1,
    passes_through_other_bipeds     = 0x2,
    immune_to_falling_damage        = 0x4,
    rotate_while_airborne           = 0x8,
    uses_limp_body_physics          = 0x10,
    random_speed_increase           = 0x40,
    spawn_death_children_on_destroy = 0x100,
    stunned_by_emp_damage           = 0x200,
    dead_physics_when_stunned       = 0x400,
    always_ragdoll_when_dead        = 0x800,
};
C_FLAGS(biped_flags_t, u32);

enum class lock_on_flags_t : u32
{
    none                              = 0x0,
    locked_by_human_targeting         = 0x1,
    locked_by_plasma_targeting        = 0x2,
    always_locked_by_plasma_targeting = 0x4,
};
C_FLAGS(lock_on_flags_t, u32);

/*! Biped and creature physics */
enum class character_physics_flags_t : u32
{
    none                           = 0x0,
    centered_at_origin             = 0x1,
    shape_spherical                = 0x2,
    use_player_physics             = 0x4,
    climb_any_surface              = 0x8,
    flying                         = 0x10,
    not_physical                   = 0x20,
    dead_character_collision_group = 0x40,
};
C_FLAGS(character_physics_flags_t, u32);

/*! bipd */
struct alignas(4) biped_t : unit_t
{
    f32                             moving_turning_speed; /* degrees */
    biped_flags_t                   biped_flags;
    f32                             stationary_turning_threshold;
    f32                             jump_velocity;
    f32                             maximum_soft_landing_time;
    f32                             maximum_hard_landing_time;
    f32                             minimum_soft_landing_velocity;
    f32                             minimum_hard_landing_velocity;
    f32                             maximum_hard_landing_velocity;
    f32                             death_hard_landing_velocity;
    f32                             stun_duration;
    f32                             standing_camera_height;
    f32                             crouching_camera_height;
    f32                             crouch_transition_time;
    f32                             camera_interpolation_start;
    f32                             camera_interpolation_end;
    f32                             camera_forward_movement_scale;
    f32                             camera_side_movement_scale;
    f32                             camera_vertical_movement_scale;
    f32                             camera_exclusion_distance;
    f32                             autoaim_width;
    lock_on_flags_t                 lock_on_flags;
    f32                             lock_on_distance;
    block_index<geo::model_node_t>  physics_control_node;
    i16                             unknown_3;
    f32                             cosine_stationary_turning_threshold;
    f32                             crouch_transition_velocity;
    block_index<geo::model_node_t>  pelvis_node;
    block_index<geo::model_node_t>  head_node;
    f32                             headshot_acceleration_scale;
    tag_reference<tag_class_t::jpt> area_damage_effect;
    character_physics_flags_t       physics_flags;
    f32                             height_standing;
    f32                             height_crouching;
    f32                             radius;
    f32                             mass;
    string_id                       living_global_material_name;
    string_id                       dead_global_material_name;
    block_index<matg::material_t>   living_global_material;
    block_index<matg::material_t>   dead_global_material;
    tag_block<u8>                   dead_sphere_shapes; /* phmo sphere_t */
    tag_block<u8>                   pill_shapes;        /* phmo pill_t */
    tag_block<u8>                   sphere_shapes;      /* phmo sphere_t */
    u8                              unknown_4[0x314 - 0x29C];
};

static_assert(offsetof(biped_t, jump_velocity) == 0x1F8);
static_assert(offsetof(biped_t, height_standing) == 0x268);
static_assert(sizeof(biped_t) == 0x314);

enum class vehicle_type_t : u16
{
    human_tank = 0,
    human_jeep,
    human_boat,
    human_plane,
    alien_scout,
    alien_fighter,
    turret,
};

enum class vehicle_flags_t : u32
{
    none                                = 0x0,
    speed_wakes_physics                 = 0x1,
    turn_wakes_physics                  = 0x2,
    driver_power_wakes_physics          = 0x4,
    gunner_power_wakes_physics          = 0x8,
    control_opposite_speed_sets_brake   = 0x10,
    slide_wakes_physics                 = 0x20,
    kills_riders_at_terminal_velocity   = 0x40,
    causes_collision_damage             = 0x80,
    ai_weapon_cannot_rotate             = 0x100,
    ai_does_not_require_driver          = 0x200,
    ai_unused                           = 0x400,
    ai_driver_enable                    = 0x800,
    ai_driver_flying                    = 0x1000,
    ai_driver_can_sidestep              = 0x2000,
    ai_driver_hovering                  = 0x4000,
    vehicle_steers_directly             = 0x8000,
    has_e_brake                         = 0x20000,
    noncombat_vehicle                   = 0x40000,
    no_friction_with_driver             = 0x80000,
    can_trigger_automatic_opening_doors = 0x100000,
    autoaim_when_teamless               = 0x200000,
};
C_FLAGS(vehicle_flags_t, u32);

enum class vehicle_control_t : u16
{
    normal = 0,
    unused,
    tank,
};

enum class specific_vehicle_t : u16
{
    none = 0,
    ghost,
    wraith,
    spectre,
    sentinel_enforcer,
};

enum class training_vehicle_t : u16
{
    none = 0,
    warthog,
    warthog_turret,
    ghost,
    banshee,
    tank,
    wraith,
};

enum class vehicle_size_t : u16
{
    small = 0,
    large,
};

enum class vehicle_physics_flags_t : u32
{
    none    = 0x0,
    invalid = 0x1,
};
C_FLAGS(vehicle_physics_flags_t, u32);

/*! vehi (Halo2Xbox layout) */
struct alignas(4) vehicle_t : unit_t
{
    vehicle_flags_t                  vehicle_flags;
    u16                              vehicle_type_raw;
    vehicle_control_t                control;
    f32                              maximum_forward_speed;
    f32                              maximum_reverse_speed;
    f32                              speed_acceleration;
    f32                              speed_deceleration;
    f32                              maximum_left_turn;
    f32                              maximum_right_turn;
    f32                              wheel_circumference;
    f32                              turn_rate;
    f32                              blur_speed;
    specific_vehicle_t               specific_type;
    training_vehicle_t               player_training_vehicle_type;
    string_id                        flip_message;
    f32                              turn_scale;
    f32                              speed_turn_penalty_power;
    f32                              speed_turn_penalty;
    f32                              maximum_left_slide;
    f32                              maximum_right_slide;
    f32                              slide_acceleration;
    f32                              slide_deceleration;
    f32                              minimum_flipping_angular_velocity;
    f32                              maximum_flipping_angular_velocity;
    vehicle_size_t                   vehicle_size;
    i16                              unknown_3;
    f32                              fixed_gun_yaw;
    f32                              fixed_gun_pitch;
    f32                              overdampen_cusp_angle;
    f32                              overdampen_exponent;
    f32                              crouch_transition_time;
    u32                              unknown_4;
    f32                              engine_moment;
    f32                              engine_maximum_angular_velocity;
    tag_block<u8>                    gears; /* elem 0x44 */
    f32                              flying_torque_scale;
    f32                              seat_entrance_acceleration_scale;
    f32                              seat_exit_acceleration_scale;
    f32                              air_friction_deceleration;
    f32                              thrust_scale;
    tag_reference<tag_class_t::snd>  suspension_sound;
    tag_reference<tag_class_t::snd>  crash_sound;
    tag_reference<>                  unknown_5;
    tag_reference<tag_class_t::effe> special_effect;
    tag_reference<tag_class_t::effe> unknown_effect;
    vehicle_physics_flags_t          physics_flags;
    f32                              ground_friction;
    f32                              ground_depth;
    f32                              ground_damp_factor;
    f32                              ground_moving_friction;
    Vecf2                            ground_maximum_slope;
    u8                               unknown_6[0x2D8 - 0x2C8];
    f32                              anti_gravity_bank_lift;
    f32                              steering_bank_reaction_scale;
    f32                              gravity_scale;
    f32                              radius;
    tag_block<u8>                    anti_gravity_points; /* elem 0x4C */
    tag_block<u8>                    friction_points;     /* elem 0x4C */
    tag_block<u8>                    phantom_shapes;      /* elem 0x2A0 */

    inline vehicle_type_t vehicle_type() const
    {
        return static_cast<vehicle_type_t>(from_le(vehicle_type_raw));
    }
};

static_assert(offsetof(vehicle_t, maximum_forward_speed) == 0x1F4);
static_assert(offsetof(vehicle_t, suspension_sound) == 0x284);
static_assert(sizeof(vehicle_t) == 0x300);

enum class item_flags_t : u32
{
    none                    = 0x0,
    always_maintains_z_up   = 0x1,
    destroyed_by_explosions = 0x2,
    unaffected_by_gravity   = 0x4,
};
C_FLAGS(item_flags_t, u32);

/*! item: weapons and equipment */
struct alignas(4) item_t : object_t
{
    item_flags_t                     item_flags;
    i16                              old_message_index_raw;
    i16                              sort_order_raw;
    f32                              multiplayer_on_ground_scale;
    f32                              campaign_on_ground_scale;
    string_id                        pickup_message;
    string_id                        swap_message;
    string_id                        pickup_or_dual_wield_message;
    string_id                        swap_or_dual_wield_message;
    string_id                        dual_wield_only_message;
    string_id                        picked_up_message;
    string_id                        singular_quantity_message;
    string_id                        plural_quantity_message;
    string_id                        switch_to_message;
    string_id                        switch_to_from_ai_message;
    tag_reference<>                  unused;
    tag_reference<tag_class_t::snd>  collision_sound;
    tag_block<tag_ref_t>             predicted_bitmaps;
    tag_reference<tag_class_t::jpt>  detonation_damage_effect;
    Vecf2                            detonation_delay;
    tag_reference<tag_class_t::effe> detonating_effect;
    tag_reference<tag_class_t::effe> detonation_effect;
};

static_assert(offsetof(item_t, pickup_message) == 0xCC);
static_assert(sizeof(item_t) == 0x12C);

struct alignas(4) magazine_equipment_t
{
    i16                              rounds_raw;
    i16                              unknown_0;
    tag_reference<tag_class_t::eqip> equipment;
};

static_assert(sizeof(magazine_equipment_t) == 0xC);

enum class magazine_flags_t : u32
{
    none                          = 0x0,
    wastes_rounds_when_reloaded   = 0x1,
    every_round_must_be_chambered = 0x2,
};
C_FLAGS(magazine_flags_t, u32);

struct alignas(4) magazine_t
{
    magazine_flags_t                 flags;
    i16                              rounds_recharged_raw;
    i16                              rounds_total_initial_raw;
    i16                              rounds_total_maximum_raw;
    i16                              rounds_loaded_maximum_raw;
    i16                              rounds_inventory_maximum_raw;
    i16                              unknown_0;
    f32                              reload_time;
    i16                              rounds_reloaded_raw;
    i16                              unknown_1;
    f32                              chamber_time;
    u8                               unknown_2[0x34 - 0x1C];
    tag_reference<tag_class_t::effe> reloading_effect;
    tag_reference<tag_class_t::jpt>  reloading_damage_effect;
    tag_reference<tag_class_t::effe> chambering_effect;
    tag_reference<tag_class_t::jpt>  chambering_damage_effect;
    tag_block<magazine_equipment_t>  equipment;

    inline i16 rounds_total_initial() const
    {
        return from_le(rounds_total_initial_raw);
    }

    inline i16 rounds_loaded_maximum() const
    {
        return from_le(rounds_loaded_maximum_raw);
    }
};

static_assert(sizeof(magazine_t) == 0x5C);

enum class trigger_flags_t : u32
{
    none                        = 0x0,
    autofire_single_action_only = 0x1,
};
C_FLAGS(trigger_flags_t, u32);

enum class trigger_input_t : u16
{
    right_trigger = 0,
    left_trigger,
    melee_attack,
    automated_fire,
};

enum class trigger_behavior_t : u16
{
    spew = 0,
    latch,
    latch_autofire,
    charge,
    latch_zoom,
    latch_rocketlauncher,
};

enum class trigger_prediction_t : u16
{
    none = 0,
    spew,
    charge,
};

enum class trigger_action_t : u16
{
    fire = 0,
    charge,
    track,
    fire_other,
};

enum class overcharged_action_t : u16
{
    none = 0,
    explode,
    discharge,
};

struct alignas(4) trigger_t
{
    trigger_flags_t                  flags;
    trigger_input_t                  input;
    trigger_behavior_t               behavior;
    block_index<barrel_t>            primary_barrel;
    block_index<barrel_t>            secondary_barrel;
    trigger_prediction_t             prediction;
    i16                              unknown_0;
    f32                              autofire_time;
    f32                              autofire_throw;
    trigger_action_t                 secondary_action;
    trigger_action_t                 primary_action;
    f32                              charging_time;
    f32                              charged_time;
    overcharged_action_t             overcharged_action;
    i16                              unknown_1;
    f32                              charged_illumination;
    f32                              spew_time;
    tag_reference<tag_class_t::effe> charging_effect;
    tag_reference<tag_class_t::jpt>  charging_damage_effect;
};

static_assert(sizeof(trigger_t) == 0x40);

enum class barrel_flags_t : u32
{
    none                                 = 0x0,
    tracks_fired_projectile              = 0x1,
    random_firing_effects                = 0x2,
    can_fire_with_partial_ammo           = 0x4,
    projectiles_use_weapon_origin        = 0x8,
    ejects_during_chamber                = 0x10,
    use_error_when_unzoomed              = 0x20,
    projectile_vector_cannot_be_adjusted = 0x40,
    projectiles_have_identical_error     = 0x80,
    projectiles_fire_parallel            = 0x100,
    cant_fire_when_others_firing         = 0x200,
    cant_fire_when_others_recovering     = 0x400,
    dont_clear_fire_bit_after_recovering = 0x800,
    stagger_fire_across_multiple_markers = 0x1000,
    fires_locked_projectiles             = 0x2000,
};
C_FLAGS(barrel_flags_t, u32);

enum class barrel_prediction_t : u16
{
    none = 0,
    continuous,
    instant,
};

struct alignas(4) barrel_t
{
    barrel_flags_t          flags;
    Vecf2                   rounds_per_second;
    f32                     acceleration_time;
    f32                     deceleration_time;
    f32                     barrel_spin_scale;
    f32                     blurred_rate_of_fire;
    std::array<i16, 2>      shots_per_fire;
    f32                     fire_recovery_time;
    f32                     soft_recovery_fraction;
    block_index<magazine_t> magazine;
    i16                     rounds_per_shot_raw;
    i16                     minimum_rounds_loaded_raw;
    i16                     rounds_between_tracers_raw;
    string_id               optional_barrel_marker_name;
    barrel_prediction_t     prediction_type;
    u16                     firing_noise;
    f32                     acceleration_time_2;
    u8                      unknown_0[0xEC - 0x3C];

    inline i16 rounds_per_shot() const
    {
        return from_le(rounds_per_shot_raw);
    }
};

static_assert(sizeof(barrel_t) == 0xEC);

struct alignas(4) first_person_t
{
    tag_reference<tag_class_t::mode> model;
    tag_reference<tag_class_t::jmad> animations;
};

enum class weapon_flags_t : u32
{
    none                                        = 0x0,
    vertical_heat_display                       = 0x1,
    mutually_exclusive_triggers                 = 0x2,
    attacks_automatically_on_bump               = 0x4,
    must_be_readied                             = 0x8,
    doesnt_count_toward_maximum                 = 0x10,
    aim_assists_only_when_zoomed                = 0x20,
    prevents_grenade_throwing                   = 0x40,
    must_be_picked_up                           = 0x80,
    holds_triggers_when_dropped                 = 0x100,
    prevents_melee_attack                       = 0x200,
    detonates_when_dropped                      = 0x400,
    cannot_fire_at_maximum_age                  = 0x800,
    secondary_trigger_overrides_grenades        = 0x1000,
    does_not_depower_active_camo_in_multiplayer = 0x2000,
    enables_integrated_night_vision             = 0x4000,
    ais_use_weapon_melee_damage                 = 0x8000,
    forces_no_binoculars                        = 0x10000,
    loop_fp_firing_animation                    = 0x20000,
    prevents_sprinting                          = 0x40000,
    cannot_fire_while_boosting                  = 0x80000,
    prevents_driving                            = 0x100000,
    prevents_gunning                            = 0x200000,
    can_be_dual_wielded                         = 0x400000,
    can_only_be_dual_wielded                    = 0x800000,
    melee_only                                  = 0x1000000,
    cant_fire_if_parent_dead                    = 0x2000000,
    weapon_ages_with_each_kill                  = 0x4000000,
    weapon_uses_old_dual_fire_error_code        = 0x8000000,
    primary_trigger_melee_attacks               = 0x10000000,
    cannot_be_used_by_player                    = 0x20000000,
};
C_FLAGS(weapon_flags_t, u32);

enum class secondary_trigger_mode_t : u16
{
    normal = 0,
    slaved_to_primary,
    inhibits_primary,
    loads_alternate_ammunition,
    loads_multiple_primary_ammunition,
};

enum class damage_reporting_type_t : u8
{
    guardians = 0,
    falling_damage,
    generic_collision_damage,
    generic_melee_damage,
    generic_explosion,
    magnum_pistol,
    plasma_pistol,
    needler,
    smg,
    plasma_rifle,
    battle_rifle,
    carbine,
    shotgun,
    sniper_rifle,
    beam_rifle,
    rocket_launcher,
    flak_cannon,
    brute_shot,
    disintegrator,
    brute_plasma_rifle,
    energy_sword,
    frag_grenade,
    plasma_grenade,
    flag_melee_damage,
    bomb_melee_damage,
    bomb_explosion_damage,
    ball_melee_damage,
    human_turret,
    plasma_turret,
    banshee,
    ghost,
    mongoose,
    scorpion,
    spectre_driver,
    spectre_gunner,
    warthog_driver,
    warthog_gunner,
    wraith,
    tank,
    sentinel_beam,
    sentinel_rpg,
    teleporter,
};

enum class movement_penalty_t : u16
{
    always = 0,
    when_zoomed,
    when_zoomed_or_reloading,
};

enum class multiplayer_weapon_type_t : u16
{
    none = 0,
    ctf_flag,
    oddball_ball,
    headhunter_head,
    juggernaut_powerup,
};

enum class weapon_type_t : u16
{
    undefined = 0,
    shotgun,
    needler,
    plasma_pistol,
    plasma_rifle,
    rocket_launcher,
};

enum class tracking_type_t : u16
{
    no_tracking = 0,
    human_tracking,
    plasma_tracking,
};

/*! weap */
struct alignas(4) weapon_t : item_t
{
    weapon_flags_t                   weapon_flags;
    string_id                        unknown_3;
    secondary_trigger_mode_t         secondary_trigger_mode;
    i16                              maximum_alternate_shots_loaded_raw;
    f32                              turn_on_time;
    f32                              ready_time;
    tag_reference<tag_class_t::effe> ready_effect;
    tag_reference<tag_class_t::jpt>  ready_damage_effect;
    f32                              heat_recovery_threshold;
    f32                              overheated_threshold;
    f32                              heat_detonation_threshold;
    f32                              heat_detonation_fraction;
    f32                              heat_loss_per_second;
    f32                              heat_illumination;
    f32                              overheated_heat_loss_per_second;
    tag_reference<tag_class_t::effe> overheated;
    tag_reference<tag_class_t::jpt>  overheated_damage_effect;
    tag_reference<tag_class_t::effe> detonation;
    tag_reference<tag_class_t::jpt>  detonation_damage_effect_2;
    tag_reference<tag_class_t::jpt>  player_melee_damage;
    tag_reference<tag_class_t::jpt>  player_melee_response;
    f32                              magnetism_angle;
    f32                              magnetism_range;
    f32                              throttle_magnitude;
    f32                              throttle_minimum_distance;
    f32                              throttle_maximum_adjustment_angle;
    Vecf2                            damage_pyramid_angles;
    f32                              damage_pyramid_depth;
    std::array<tag_reference<tag_class_t::jpt>, 8>
        melee_damage; /* 3 hits + lunge, damage and response each */
    damage_reporting_type_t          melee_damage_reporting_type;
    u8                               unknown_4;
    i16                              magnification_levels_raw;
    Vecf2                            magnification_range;
    f32                              autoaim_angle;
    f32                              autoaim_range;
    f32                              magnetism_angle_2;
    f32                              magnetism_range_2;
    f32                              deviation_angle;
    u8                               unknown_5[0x22C - 0x21C];
    movement_penalty_t               movement_penalized;
    i16                              unknown_6;
    f32                              forward_movement_penalty;
    f32                              sideways_movement_penalty;
    f32                              ai_scariness;
    f32                              weapon_power_on_time;
    f32                              weapon_power_off_time;
    tag_reference<tag_class_t::effe> weapon_power_on_effect;
    tag_reference<tag_class_t::effe> weapon_power_off_effect;
    f32                              age_heat_recovery_penalty;
    f32                              age_rate_of_fire_penalty;
    f32                              age_misfire_start;
    f32                              age_misfire_chance;
    tag_reference<tag_class_t::snd>  pickup_sound;
    tag_reference<tag_class_t::snd>  zoom_in_sound;
    tag_reference<tag_class_t::snd>  zoom_out_sound;
    f32                              active_camo_ding;
    f32                              active_camo_regrowth_rate;
    string_id                        handle_node;
    string_id                        weapon_class; /*!< e.g. rifle, pistol */
    string_id                        weapon_name;
    multiplayer_weapon_type_t        multiplayer_weapon_type;
    weapon_type_t                    weapon_type;
    tracking_type_t                  tracking_type;
    u8                               unknown_7[0x2A8 - 0x296];
    tag_block<first_person_t>        first_person;
    tag_reference<tag_class_t::nhdt> new_hud_interface;
    tag_block<predicted_resource_t>  weapon_predicted_resources;
    tag_block<magazine_t>            magazines;
    tag_block<trigger_t>             triggers;
    tag_block<barrel_t>              barrels;
    u8                               unknown_8[0x31C - 0x2D8];
};

static_assert(offsetof(weapon_t, ready_effect) == 0x140);
static_assert(offsetof(weapon_t, melee_damage) == 0x1BC);
static_assert(offsetof(weapon_t, handle_node) == 0x284);
static_assert(offsetof(weapon_t, magazines) == 0x2C0);
static_assert(sizeof(weapon_t) == 0x31C);

enum class powerup_type_t : u16
{
    none = 0,
    double_speed,
    over_shield,
    active_camouflage,
    full_spectrum_vision,
    health,
    grenade,
};

/*! eqip */
struct alignas(4) equipment_t : item_t
{
    u16                             powerup_type_raw;
    unit_grenade_type_t             grenade_type;
    f32                             powerup_time;
    tag_reference<tag_class_t::snd> pickup_sound;

    inline powerup_type_t powerup_type() const
    {
        return static_cast<powerup_type_t>(from_le(powerup_type_raw));
    }
};

static_assert(sizeof(equipment_t) == 0x13C);

enum class device_flags_t : u32
{
    none                = 0x0,
    position_loops      = 0x1,
    allow_interpolation = 0x4,
};
C_FLAGS(device_flags_t, u32);

enum class device_lightmap_flags_t : u16
{
    none                   = 0x0,
    dont_use_in_lightmap   = 0x1,
    dont_use_in_lightprobe = 0x2,
};
C_FLAGS(device_lightmap_flags_t, u16);

/*! device: machines, controls and light fixtures */
struct alignas(4) device_t : object_t
{
    device_flags_t                   device_flags;
    /* Times are stored as their reciprocals (1 / seconds) */
    f32                              power_transition_time;
    f32                              power_acceleration_time;
    f32                              position_transition_time;
    f32                              position_acceleration_time;
    f32                              depowered_position_transition_time;
    f32                              depowered_position_acceleration_time;
    device_lightmap_flags_t          lightmap_flags;
    i16                              unknown_2;
    tag_reference<tag_class_t::effe> open_up;
    tag_reference<tag_class_t::effe> close_down;
    tag_reference<tag_class_t::effe> opened;
    tag_reference<tag_class_t::effe> closed;
    tag_reference<tag_class_t::effe> depowered;
    tag_reference<tag_class_t::effe> repowered;
    f32                              delay_time;
    tag_reference<tag_class_t::effe> delay_effect;
    f32                              automatic_activation_radius;
};

static_assert(offsetof(device_t, open_up) == 0xDC);
static_assert(sizeof(device_t) == 0x11C);

enum class machine_type_t : u16
{
    door = 0,
    platform,
    gear,
};

enum class machine_flags_t : u16
{
    none                 = 0x0,
    pathfinding_obstacle = 0x1,
    but_not_when_open    = 0x2,
    elevator             = 0x4,
};
C_FLAGS(machine_flags_t, u16);

enum class collision_response_t : u16
{
    pause_until_crushed = 0,
    reverse_directions,
};

enum class machine_pathfinding_policy_t : u16
{
    discs = 0,
    sectors,
    cut_out,
    none,
};

/*! mach */
struct alignas(4) machine_t : device_t
{
    u16                            machine_type_raw;
    machine_flags_t                machine_flags;
    f32                            door_open_time;
    Vecf2                          door_occlusion_bounds;
    collision_response_t           collision_response;
    block_index<geo::model_node_t> elevator_node;
    machine_pathfinding_policy_t   pathfinding_policy;
    i16                            unknown_3;

    inline machine_type_t machine_type() const
    {
        return static_cast<machine_type_t>(from_le(machine_type_raw));
    }
};

static_assert(offsetof(machine_t, collision_response) == 0x12C);

enum class control_type_t : u16
{
    toggle_switch = 0,
    on_button,
    off_button,
    call_button,
};

enum class triggers_when_t : u16
{
    touched_by_player = 0,
    destroyed,
};

/*! ctrl */
struct alignas(4) control_t : device_t
{
    control_type_t                   control_type;
    triggers_when_t                  triggers_when;
    f32                              call_value;
    string_id                        action_string;
    tag_reference<tag_class_t::effe> on;
    tag_reference<tag_class_t::effe> off;
    tag_reference<tag_class_t::effe> deny;
};

static_assert(sizeof(control_t) == 0x140);

enum class crate_flags_t : u16
{
    none                          = 0x0,
    does_not_block_area_of_effect = 0x1,
};
C_FLAGS(crate_flags_t, u16);

/*! bloc */
struct alignas(4) crate_t : object_t
{
    crate_flags_t crate_flags;
    i16           unknown_2;
};

static_assert(sizeof(crate_t) == 0xC0);

enum class creature_flags_t : u32
{
    none                     = 0x0,
    infection_form           = 0x2,
    immune_to_falling_damage = 0x4,
    rotate_while_airborne    = 0x8,
    zapped_by_shields        = 0x10,
    attach_upon_impact       = 0x20,
    not_on_motion_sensor     = 0x40,
};
C_FLAGS(creature_flags_t, u32);

/*! crea, the fields before its physics */
struct alignas(4) creature_t : object_t
{
    creature_flags_t              creature_flags;
    team_t                        default_team;
    blip_size_t                   motion_sensor_blip_size;
    f32                           turning_velocity_maximum;
    f32                           turning_acceleration_maximum;
    f32                           casual_turning_modifier;
    f32                           autoaim_width;
    character_physics_flags_t     physics_flags;
    f32                           height_standing;
    f32                           height_crouching;
    f32                           radius;
    f32                           mass;
    string_id                     living_global_material_name;
    string_id                     dead_global_material_name;
    block_index<matg::material_t> living_global_material;
    block_index<matg::material_t> dead_global_material;
};

static_assert(offsetof(creature_t, height_standing) == 0xD8);

enum class state_property_flags_t : u8
{
    none             = 0x0,
    blurred          = 0x1,
    hella_blurred    = 0x2,
    shielded         = 0x4,
    battery_depleted = 0x8,
};
C_FLAGS(state_property_flags_t, u8);

enum class damage_state_t : u16
{
    default_ = 0,
    minor_damage,
    medium_damage,
    major_damage,
    destroyed,
};

enum class permutation_flags_t : u8
{
    none                            = 0x0,
    copy_states_to_all_permutations = 0x1,
};
C_FLAGS(permutation_flags_t, u8);

/*! Closest-first draw order of a variant region's permutations */
enum class sort_order_t : u16
{
    no_sorting = 0,
    closest_5,
    closest_4,
    closest_3,
    closest_2,
    closest_1,
    same_as_model,
    farthest_1,
    farthest_2,
    farthest_3,
    farthest_4,
    farthest_5,
};

struct alignas(4) variant_state_t
{
    string_id                        permutation_name;
    i8                               runtime_permutation;
    state_property_flags_t           property_flags;
    damage_state_t                   state;
    tag_reference<tag_class_t::effe> looping_effect;
    string_id                        looping_effect_marker_name;
    f32                              initial_probability;
};

static_assert(sizeof(variant_state_t) == 0x18);

struct alignas(4) variant_permutation_t
{
    string_id           name;
    i8                  runtime_permutation; /*!< into the mode region's */
    permutation_flags_t flags;
    i16                 unknown_0;
    f32                 probability;
    tag_block<variant_state_t> states;
    std::array<i8, 5>          runtime_state_permutations;
    u8                         unknown_1[7];
};

static_assert(sizeof(variant_permutation_t) == 0x20);

struct alignas(4) variant_region_t
{
    string_id                        name; /*!< the mode region */
    i8                               runtime_region;
    u8                               runtime_flags; /* TODO: bits unknown */
    block_index<model_variant_t>     parent_variant;
    tag_block<variant_permutation_t> permutations;
    sort_order_t                     sort_order;
    i16                              unknown_0;
};

static_assert(sizeof(variant_region_t) == 0x14);

struct alignas(4) variant_object_t
{
    string_id        parent_marker;
    string_id        child_marker;
    object_reference child_object;
};

static_assert(sizeof(variant_object_t) == 0x10);

/*! hlmt variant: which permutation of each region makes up a named look */
struct alignas(4) model_variant_t
{
    string_id                        name;
    std::array<i8, 16>               runtime_model_region_indices;
    tag_block<variant_region_t>      regions;
    tag_block<variant_object_t>      objects;
    u8                               unknown_0[8];
    string_id                        dialogue_sound_effect;
    tag_reference<tag_class_t::udlg> dialogue;
};

static_assert(offsetof(model_variant_t, regions) == 0x14);
static_assert(sizeof(model_variant_t) == 0x38);

enum class shadow_fade_t : u16
{
    super_high_detail = 0,
    high_detail,
    medium_detail,
    low_detail,
    super_low_detail,
    never,
};

/*! hlmt */
struct alignas(4) hlmt_t
{
    tag_reference<tag_class_t::mode> render_model;    /*!< mode */
    tag_reference<tag_class_t::coll> collision_model; /*!< coll */
    tag_reference<tag_class_t::jmad> animation;       /*!< jmad */
    tag_reference<tag_class_t::phys> physics;         /*!< phys */
    tag_reference<tag_class_t::phmo> physics_model;   /*!< phmo */
    f32                              disappear_distance;
    f32                              begin_fade_distance;
    u32                              unknown_0;
    f32                              reduce_to_l1;
    f32                              reduce_to_l2;
    f32                              reduce_to_l3;
    f32                              reduce_to_l4;
    f32                              reduce_to_l5;
    u32                              unknown_1;
    shadow_fade_t                    shadow_fade_distance;
    i16                              unknown_2;
    tag_block<model_variant_t>       variants;
};

static_assert(offsetof(hlmt_t, physics_model) == 0x20);
static_assert(offsetof(hlmt_t, variants) == 0x50);

/*! coll: collision model, one winged-edge BSP per region permutation */
namespace coll {

struct alignas(4) material_t
{
    string_id name;
};

struct node_t;

struct alignas(4) bsp_ref_t
{
    block_index<node_t>               node;
    i16                               unknown_0;
    dimeter::coll::bsp_t<heap_t::tag> bsp;
};

static_assert(sizeof(bsp_ref_t) == 0x44);

struct alignas(4) permutation_t
{
    string_id            name;
    tag_block<bsp_ref_t> bsps;
    tag_block<u8>        bsp_physics; /*!< elem 0x74, Havok */
};

static_assert(sizeof(permutation_t) == 0x14);

struct alignas(4) region_t
{
    string_id                name;
    tag_block<permutation_t> permutations;
};

static_assert(sizeof(region_t) == 0xC);

enum class pathfinding_sphere_flags_t : u16
{
    none              = 0x0,
    remains_when_open = 0x1,
    vehicle_only      = 0x2,
    with_sectors      = 0x4,
};
C_FLAGS(pathfinding_sphere_flags_t, u16);

struct alignas(4) pathfinding_sphere_t
{
    block_index<node_t>        node;
    pathfinding_sphere_flags_t flags;
    Vecf3                      center;
    f32                        radius;
};

static_assert(sizeof(pathfinding_sphere_t) == 0x14);

struct alignas(4) node_t
{
    string_id           name;
    i16                 unknown_0;
    block_index<node_t> parent;
    block_index<node_t> next_sibling;
    block_index<node_t> first_child;
};

static_assert(sizeof(node_t) == 0xC);

enum class model_flags_t : u32
{
    none                = 0x0,
    contains_open_edges = 0x1,
};
C_FLAGS(model_flags_t, u32);

struct alignas(4) model_t
{
    u8                              unknown_0[0x10];
    model_flags_t                   flags;
    tag_block<material_t>           materials;
    tag_block<region_t>             regions;
    tag_block<pathfinding_sphere_t> pathfinding_spheres;
    tag_block<node_t>               nodes;
};

static_assert(sizeof(model_t) == 0x34);

} // namespace coll

/*! phmo: Havok physics model */
namespace phmo {

struct node_t;
struct material_t;
struct region_t;

enum class motion_type_t : u16
{
    sphere = 0,
    stabilized_sphere,
    box,
    stabilized_box,
    keyframed,
    fixed,
};

enum class shape_type_t : u16
{
    sphere = 0,
    pill,
    box,
    triangle,
    polyhedron,
    multi_sphere,
    unused_0,
    unused_1,
    unused_2,
    unused_3,
    unused_4,
    unused_5,
    unused_6,
    unused_7,
    list,
    mopp,
};

enum class rigid_body_flags_t : u16
{
    none                               = 0x0,
    no_collisions_with_self            = 0x1,
    only_collide_with_environment      = 0x2,
    disable_effects                    = 0x4,
    does_not_interact_with_environment = 0x8,
    best_early_mover_body              = 0x10,
    has_no_phantom_power_version       = 0x20,
};
C_FLAGS(rigid_body_flags_t, u16);

enum class body_size_t : u16
{
    default_ = 0,
    tiny,
    small,
    medium,
    large,
    huge,
    extra_huge,
};

enum class collision_quality_t : u8
{
    none = 0,
    debris_simple_toi,
    moving,
    critical,
    bullet,
    character,
    fixed,
};

struct alignas(4) rigid_body_t
{
    block_index<node_t>       node;
    block_index<region_t>     region;
    i16                       permutation_raw;
    i16                       unknown_0;
    Vecf3                     bounding_sphere_offset;
    f32                       bounding_sphere_radius;
    rigid_body_flags_t        flags;
    u16                       motion_type_raw;
    block_index<rigid_body_t> no_phantom_power_alternative;
    body_size_t               size;
    f32                       inertia_tensor_scale;
    f32                       linear_damping;
    f32                       angular_damping;
    Vecf3                     center_of_mass_offset;
    u16                       shape_type_raw;
    i16   shape_index_raw; /*!< into the block shape_type names */
    f32   mass;
    Vecf4 center_of_mass;
    std::array<Vecf4, 3> inertia_tensor;
    f32                  bounding_sphere_pad;
    u8                   unknown_1[0x8C - 0x84];
    collision_quality_t  collision_quality_override_type;
    u8                   unknown_2;
    u16                  runtime_flags; /* TODO: bits unknown */

    inline motion_type_t motion_type() const
    {
        return static_cast<motion_type_t>(from_le(motion_type_raw));
    }

    inline shape_type_t shape_type() const
    {
        return static_cast<shape_type_t>(from_le(shape_type_raw));
    }

    inline i16 shape_index() const
    {
        return from_le(shape_index_raw);
    }
};

static_assert(offsetof(rigid_body_t, shape_type_raw) == 0x38);
static_assert(sizeof(rigid_body_t) == 0x90);

enum class material_flags_t : u16
{
    none                               = 0x0,
    does_not_collide_with_fixed_bodies = 0x1,
};
C_FLAGS(material_flags_t, u16);

struct alignas(4) material_t
{
    string_id        name;
    string_id        global_material_name;
    i16              phantom_type_raw;
    material_flags_t flags;
};

static_assert(sizeof(material_t) == 0xC);

/*! Every shape starts with this */
struct alignas(4) shape_t
{
    string_id               name;
    block_index<material_t> material;
    u16                     flags; /* TODO: bits unknown */
    f32                     relative_mass_scale;
    f32                     friction;
    f32                     restitution;
    f32                     volume;
    f32                     mass;
    i16                     mass_distribution_raw;
    i8                      phantom_type;
    i8                      collision_group;
    u8                      havok_0[0xC];
};

static_assert(sizeof(shape_t) == 0x2C);

struct alignas(4) sphere_t : shape_t
{
    f32                  radius;
    u8                   havok_1[0x10];
    std::array<Vecf4, 3> rotation;
    Vecf4                translation;
};

static_assert(sizeof(sphere_t) == 0x80);

struct alignas(4) pill_t : shape_t
{
    f32   radius;
    Vecf4 bottom;
    Vecf4 top;
};

static_assert(sizeof(pill_t) == 0x50);

struct alignas(4) box_t : shape_t
{
    f32                  radius;
    Vecf4                half_extents;
    u8                   havok_1[0x10];
    std::array<Vecf4, 3> rotation;
    Vecf4                translation;
};

static_assert(sizeof(box_t) == 0x90);

struct alignas(4) region_t
{
    string_id     name;
    tag_block<u8> permutations; /*!< elem 0x10 */
};

enum class node_flags_t : u16
{
    none             = 0x0,
    does_not_animate = 0x1,
};
C_FLAGS(node_flags_t, u16);

struct alignas(4) node_t
{
    string_id           name;
    node_flags_t        flags;
    block_index<node_t> parent;
    block_index<node_t> sibling;
    block_index<node_t> child;
};

static_assert(sizeof(node_t) == 0xC);

struct alignas(4) model_t
{
    u32                     flags; /* TODO: bits unknown */
    f32                     mass;
    f32                     low_frequency_deactivation_scale;
    f32                     high_frequency_deactivation_scale;
    u8                      unknown_0[0x28 - 0x10];
    tag_block<u8>           phantom_types; /* elem 0x68 */
    tag_block<u8>           node_edges;    /* elem 0x18 */
    tag_block<rigid_body_t> rigid_bodies;
    tag_block<material_t>   materials;
    tag_block<sphere_t>     spheres;
    tag_block<u8>           multi_spheres; /* elem 0xB0 */
    tag_block<pill_t>       pills;
    tag_block<box_t>        boxes;
    tag_block<u8>           triangles;                  /* elem 0x60 */
    tag_block<u8>           polyhedra;                  /* elem 0x100 */
    tag_block<u8>           polyhedron_four_vectors;    /* elem 0x30 */
    tag_block<u8>           polyhedron_plane_equations; /* elem 0x10 */
    tag_block<u8>           mass_distributions;         /* elem 0x40 */
    tag_block<u8>           lists;                      /* elem 0x38 */
    tag_block<u8>           list_shapes;                /* elem 8 */
    tag_block<u8>           mopps;                      /* elem 0x14 */
    data_reference<>        mopp_codes;
    tag_block<u8>           hinge_constraints;   /* elem 0x78 */
    tag_block<u8>           ragdoll_constraints; /* elem 0x94 */
    tag_block<region_t>     regions;
    tag_block<node_t>       nodes;
    u8                      unknown_1[0xE0 - 0xD0];
    tag_block<u8>           point_to_path_curves;        /* elem 0x10 */
    tag_block<u8>           limited_hinge_constraints;   /* elem 0x84 */
    tag_block<u8>           ball_and_socket_constraints; /* elem 0x78 */
    tag_block<u8>           stiff_spring_constraints;    /* elem 0x7C */
    tag_block<u8>           prismatic_constraints;       /* elem 0x84 */
    tag_block<u8>           phantoms;                    /* elem 0x20 */
};

static_assert(offsetof(model_t, rigid_bodies) == 0x38);
static_assert(offsetof(model_t, regions) == 0xC0);
static_assert(sizeof(model_t) == 0x110);

} // namespace phmo

/*! char: an AI character, the unit it drives and how it behaves */
namespace chr {

struct alignas(4) variant_t
{
    string_id                    name;
    block_index<model_variant_t> index; /*!< the unit's hlmt variants */
    i16                          unknown_0;
    string_id                    designator;
};

static_assert(sizeof(variant_t) == 0xC);

enum class general_flags_t : u32
{
    none           = 0x0,
    swarm          = 0x1,
    flying         = 0x2,
    dual_wields    = 0x4,
    uses_gravemind = 0x8,
};
C_FLAGS(general_flags_t, u32);

enum class character_type_t : u16
{
    elite = 0,
    jackal,
    grunt,
    hunter,
    engineer,
    assassin,
    player,
    marine,
    crew,
    combat_form,
    infection_form,
    carrier_form,
    monitor,
    sentinel,
    none,
    mounted_weapon,
    brute,
    prophet,
    bugger,
    juggernaut,
};

struct alignas(4) general_t
{
    general_flags_t  flags;
    character_type_t type;
    i16              unknown_0;
    f32              scariness;
};

static_assert(sizeof(general_t) == 0xC);

struct alignas(4) vitality_t
{
    u32   flags; /* TODO: bits unknown */
    f32   normal_body_vitality;
    f32   normal_shield_vitality;
    f32   legendary_body_vitality;
    f32   legendary_shield_vitality;
    f32   body_recharge_fraction;
    f32   soft_ping_threshold_with_shields;
    f32   soft_ping_threshold_no_shields;
    f32   soft_ping_minimum_interrupt_time;
    f32   hard_ping_threshold_with_shields;
    f32   hard_ping_threshold_no_shields;
    f32   hard_ping_minimum_interrupt_time;
    f32   current_damage_decay_delay;
    f32   current_damage_decay_time;
    f32   recent_damage_decay_delay;
    f32   recent_damage_decay_time;
    f32   body_recharge_delay_time;
    f32   body_recharge_time;
    f32   shield_recharge_delay_time;
    f32   shield_recharge_time;
    f32   stun_threshold;
    Vecf2 stun_time_bounds;
    f32   extended_shield_damage_threshold;
    f32   extended_body_damage_threshold;
    f32   suicide_radius;
    f32   runtime_body_recharge_velocity;
    f32   runtime_shield_recharge_velocity;
};

static_assert(sizeof(vitality_t) == 0x70);

struct alignas(4) perception_t
{
    u32 flags; /* TODO: bits unknown */
    f32 maximum_vision_distance;
    f32 central_vision_angle;
    f32 maximum_vision_angle;
    f32 peripheral_vision_angle;
    f32 peripheral_distance;
    f32 hearing_distance;
    f32 notice_projectile_chance;
    f32 notice_vehicle_chance;
    f32 combat_perception_time;
    f32 guard_perception_time;
    f32 non_combat_perception_time;
    f32 first_acknowledge_surprise_distance;
};

static_assert(sizeof(perception_t) == 0x34);

enum class movement_flags_t : u32
{
    none                          = 0x0,
    danger_crouch_allow_movement  = 0x1,
    no_side_step                  = 0x2,
    prefer_to_combat_near_friends = 0x4,
    hop_to_cover_path_segments    = 0x8,
    perch                         = 0x10,
    has_flying_mode               = 0x20,
    disallow_crouch               = 0x40,
};
C_FLAGS(movement_flags_t, u32);

enum class movement_hint_flags_t : u32
{
    none         = 0x0,
    vault_step   = 0x1,
    vault_crouch = 0x2,
    mount_step   = 0x20,
    mount_crouch = 0x40,
    mount_stand  = 0x80,
    hoist_crouch = 0x800,
    hoist_stand  = 0x1000,
};
C_FLAGS(movement_hint_flags_t, u32);

struct alignas(4) movement_t
{
    movement_flags_t      flags;
    f32                   pathfinding_radius;
    f32                   destination_radius;
    f32                   dive_grenade_chance;
    ai_size_t             obstacle_leap_minimum_size;
    ai_size_t             obstacle_leap_maximum_size;
    ai_size_t             obstacle_ignore_size;
    ai_size_t             obstacle_smashable_size;
    i16                   unknown_0;
    leap_jump_speed_t     jump_height;
    movement_hint_flags_t movement_hints;
    f32                   throttle_scale;
};

static_assert(sizeof(movement_t) == 0x24);

struct alignas(4) placement_t
{
    u32 unknown_0;
    std::array<f32, 4>
        few_upgrade_chance; /*!< easy, normal, heroic, legendary */
    std::array<f32, 4> normal_upgrade_chance;
    std::array<f32, 4> many_upgrade_chance;
};

static_assert(sizeof(placement_t) == 0x34);

struct alignas(4) look_t
{
    Vecf2 maximum_aiming_deviation; /*!< yaw, pitch in degrees */
    Vecf2 maximum_looking_deviation;
    Vecf2 runtime_aiming_deviation_cosines;
    Vecf2 runtime_looking_deviation_cosines;
    f32   noncombat_look_delta_left;
    f32   noncombat_look_delta_right;
    f32   combat_look_delta_left;
    f32   combat_look_delta_right;
    Vecf2 noncombat_idle_looking;
    Vecf2 noncombat_idle_aiming;
    Vecf2 combat_idle_looking;
    Vecf2 combat_idle_aiming;
};

static_assert(sizeof(look_t) == 0x50);

struct alignas(4) swarm_t
{
    i16   scatter_killed_count_raw;
    i16   unknown_0;
    f32   scatter_radius;
    f32   scatter_time;
    Vecf2 hound_distance;
    f32   perlin_offset_scale;
    Vecf2 offset_period;
    f32   perlin_idle_movement_threshold;
    f32   perlin_combat_movement_threshold;
};

static_assert(sizeof(swarm_t) == 0x28);

struct alignas(4) ready_t
{
    Vecf2 ready_time_bounds;
};

enum class engage_flags_t : u32
{
    none                           = 0x0,
    engage_perch                   = 0x1,
    fight_constant_movement        = 0x2,
    flight_fight_constant_movement = 0x4,
};
C_FLAGS(engage_flags_t, u32);

struct alignas(4) engage_t
{
    engage_flags_t flags;
    f32            crouch_danger_threshold;
    f32            stand_danger_threshold;
    f32            fight_danger_move_threshold;
};

static_assert(sizeof(engage_t) == 0x10);

enum class charge_flags_t : u32
{
    none                    = 0x0,
    offhand_melee_allowed   = 0x1,
    berserk_whenever_charge = 0x2,
};
C_FLAGS(charge_flags_t, u32);

struct alignas(4) charge_t
{
    charge_flags_t                   flags;
    f32                              melee_consider_range;
    f32                              melee_chance;
    f32                              melee_attack_range;
    f32                              melee_abort_range;
    f32                              melee_attack_timeout;
    f32                              melee_attack_delay_timer;
    Vecf2                            melee_leap_range;
    f32                              melee_leap_chance;
    f32                              ideal_leap_velocity;
    f32                              maximum_leap_velocity;
    f32                              melee_leap_ballistic;
    f32                              melee_delay_timer;
    tag_reference<tag_class_t::weap> berserk_weapon;
};

static_assert(sizeof(charge_t) == 0x40);

struct alignas(4) evasion_t
{
    f32 danger_threshold;
    f32 delay_timer;
    f32 chance;
    f32 proximity_threshold;
    f32 dive_retreat_chance;
};

static_assert(sizeof(evasion_t) == 0x14);

struct alignas(4) cover_t
{
    u32   flags; /* TODO: bits unknown */
    Vecf2 hide_behind_cover_time;
    f32   cover_vitality_threshold;
    f32   cover_shield_fraction;
    f32   cover_check_delay;
    f32   emerge_shield_fraction;
    f32   cover_danger_threshold;
    f32   danger_upper_threshold;
    Vecf2 cover_chance;
    f32   proximity_self_preserve;
    f32   disallow_cover_distance;
    f32   proximity_melee_distance;
    f32   unreachable_enemy_danger_threshold;
    f32   scary_target_threshold;
};

static_assert(sizeof(cover_t) == 0x40);

enum class retreat_flags_t : u32
{
    none                 = 0x0,
    zig_zag_when_fleeing = 0x1,
};
C_FLAGS(retreat_flags_t, u32);

struct alignas(4) retreat_t
{
    retreat_flags_t                  flags;
    f32                              shield_threshold;
    f32                              scary_target_threshold;
    f32                              danger_threshold;
    f32                              proximity_threshold;
    Vecf2                            forced_cower_time_bounds;
    Vecf2                            cower_timeout_bounds;
    f32                              proximity_ambush_threshold;
    f32                              awareness_ambush_threshold;
    f32                              leader_dead_retreat_chance;
    f32                              peer_dead_retreat_chance;
    f32                              second_peer_dead_retreat_chance;
    f32                              zig_zag_angle;
    f32                              zig_zag_period;
    f32                              retreat_grenade_chance;
    tag_reference<tag_class_t::weap> backup_weapon;
};

static_assert(sizeof(retreat_t) == 0x4C);

enum class search_flags_t : u32
{
    none                  = 0x0,
    crouch_on_investigate = 0x1,
    walk_on_pursuit       = 0x2,
};
C_FLAGS(search_flags_t, u32);

struct alignas(4) search_t
{
    search_flags_t flags;
    Vecf2          search_time;
    Vecf2          uncover_distance_bounds;
};

static_assert(sizeof(search_t) == 0x14);

struct alignas(4) pre_search_t
{
    u32   flags; /* TODO: bits unknown */
    Vecf2 minimum_presearch_time;
    Vecf2 maximum_presearch_time;
    f32   minimum_certainty_radius;
    u32   unknown_0;
    Vecf2 minimum_suppressing_time;
};

static_assert(sizeof(pre_search_t) == 0x24);

struct alignas(4) idle_t
{
    u32   unknown_0;
    Vecf2 idle_pose_delay_time;
};

static_assert(sizeof(idle_t) == 0xC);

struct alignas(4) vocalization_t
{
    f32 look_comment_time;
    f32 look_long_comment_time;
};

enum class boarding_flags_t : u32
{
    none              = 0x0,
    airborne_boarding = 0x1,
};
C_FLAGS(boarding_flags_t, u32);

struct alignas(4) boarding_t
{
    boarding_flags_t flags;
    f32              maximum_distance;
    f32              abort_distance;
    f32              maximum_speed;
};

static_assert(sizeof(boarding_t) == 0x10);

struct alignas(4) boss_t
{
    u32 unknown_0;
    f32 flurry_damage_threshold;
    f32 flurry_time;
};

static_assert(sizeof(boss_t) == 0xC);

struct alignas(4) firing_pattern_t
{
    f32   rate_of_fire;
    f32   target_tracking;
    f32   target_leading;
    f32   burst_origin_radius;
    f32   burst_origin_angle;
    Vecf2 burst_return_length;
    f32   burst_return_angle;
    Vecf2 burst_duration;
    Vecf2 burst_separation;
    f32   weapon_damage_modifier;
    f32   projectile_error;
    f32   burst_angular_velocity;
    f32   maximum_error_angle;
};

static_assert(sizeof(firing_pattern_t) == 0x40);

struct alignas(4) firing_pattern_properties_t
{
    tag_reference<tag_class_t::weap> weapon;
    tag_block<firing_pattern_t>      patterns;
};

static_assert(sizeof(firing_pattern_properties_t) == 0x10);

enum class trajectory_type_t : u16
{
    toss = 0,
    lob,
    bounce,
};

struct alignas(4) grenades_t
{
    u32                flags; /* TODO: bits unknown */
    u16                grenade_type;
    trajectory_type_t  trajectory_type;
    i16                unknown_0;
    i16                minimum_enemy_count_raw;
    f32                enemy_radius;
    f32                grenade_ideal_velocity;
    f32                grenade_velocity;
    Vecf2              grenade_ranges;
    f32                collateral_damage_radius;
    f32                grenade_chance;
    f32                grenade_throw_delay;
    f32                grenade_uncover_chance;
    f32                anti_vehicle_grenade_chance;
    std::array<i16, 2> grenade_count;
    f32                dont_drop_grenades_chance;
};

static_assert(sizeof(grenades_t) == 0x3C);

enum class vehicle_ai_flags_t : u32
{
    none                            = 0x0,
    passengers_adopt_original_squad = 0x1,
};
C_FLAGS(vehicle_ai_flags_t, u32);

struct alignas(4) vehicle_t
{
    tag_reference<tag_class_t::vehi> unit;
    tag_reference<tag_class_t::styl> style;
    vehicle_ai_flags_t               flags;
    f32                              ai_pathfinding_radius;
    f32                              ai_destination_radius;
    f32                              ai_deceleration_distance;
    f32                              ai_turning_radius;
    f32                              ai_inner_turning_radius;
    f32                              ai_ideal_turning_radius;
    f32                              ai_banshee_steering_maximum;
    f32                              ai_maximum_steering_angle;
    f32                              ai_maximum_steering_delta;
    f32                              ai_oversteering_scale;
    Vecf2                            ai_oversteering_bounds;
    f32                              ai_sideslip_distance;
    f32                              ai_avoidance_distance;
    f32                              ai_minimum_urgency;
    f32                              ai_throttle_maximum;
    f32                              ai_goal_minimum_throttle_scale;
    f32                              ai_turn_minimum_throttle_scale;
    f32                              ai_direction_minimum_throttle_scale;
    f32                              ai_acceleration_scale;
    f32                              ai_throttle_blend;
    f32                              theoretical_maximum_speed;
    f32                              error_scale;
    f32                              ai_allowable_aim_deviation_angle;
    f32                              ai_charge_tight_angle_distance;
    f32                              ai_charge_tight_angle;
    f32                              ai_charge_repeat_timeout;
    f32                              ai_charge_look_ahead_time;
    f32                              ai_charge_consider_distance;
    f32                              ai_charge_abort_distance;
    f32                              vehicle_ram_timeout;
    f32                              ram_paralysis_time;
    f32                              ai_cover_damage_threshold;
    f32                              ai_cover_minimum_distance;
    f32                              ai_cover_time;
    f32                              ai_cover_minimum_boost_distance;
    f32                              turtling_recent_damage_threshold;
    f32                              turtling_minimum_time;
    f32                              turtling_timeout;
    u16                              obstacle_ignore_size;
    i16                              unknown_0;
};

static_assert(sizeof(vehicle_t) == 0xB4);

enum class weapons_flags_t : u32
{
    none                            = 0x0,
    bursting_inhibits_movement      = 0x1,
    must_crouch_to_shoot            = 0x2,
    use_extended_safe_to_save_range = 0x4,
};
C_FLAGS(weapons_flags_t, u32);

struct alignas(4) weapons_t
{
    weapons_flags_t                  flags;
    tag_reference<tag_class_t::weap> weapon;
    f32                              maximum_firing_range;
    f32                              minimum_firing_range;
    Vecf2                            normal_combat_range;
    f32                              bombardment_range;
    f32                              maximum_special_target_distance;
    Vecf2                            timid_combat_range;
    Vecf2                            aggressive_combat_range;
    f32                              super_ballistic_range;
    Vecf2                            ballistic_firing_bounds;
    Vecf2                            ballistic_fraction_bounds;
    Vecf2                            first_burst_delay_time;
    f32                              surprise_delay_time;
    u8                               unknown_0[0xCC - 0x54];
};

static_assert(sizeof(weapons_t) == 0xCC);

struct alignas(4) character_t
{
    u32                               flags; /* TODO: bits unknown */
    tag_reference<tag_class_t::char_> parent_character;
    tag_reference<tag_class_t::bipd, tag_class_t::vehi>
                                           unit; /*!< bipd or vehi */
    tag_reference<tag_class_t::crea>       creature;
    tag_reference<tag_class_t::styl>       style;
    tag_reference<tag_class_t::char_>      major_character;
    tag_block<variant_t>                   variants;
    tag_block<general_t>                   general;
    tag_block<vitality_t>                  vitality;
    tag_block<placement_t>                 placement;
    tag_block<perception_t>                perception;
    tag_block<look_t>                      look;
    tag_block<movement_t>                  movement;
    tag_block<swarm_t>                     swarm;
    tag_block<ready_t>                     ready;
    tag_block<engage_t>                    engage;
    tag_block<charge_t>                    charge;
    tag_block<evasion_t>                   evasion;
    tag_block<cover_t>                     cover;
    tag_block<retreat_t>                   retreat;
    tag_block<search_t>                    search;
    tag_block<pre_search_t>                pre_search;
    tag_block<idle_t>                      idle;
    tag_block<vocalization_t>              vocalization;
    tag_block<boarding_t>                  boarding;
    tag_block<boss_t>                      boss;
    tag_block<weapons_t>                   weapons;
    tag_block<firing_pattern_properties_t> firing_pattern;
    tag_block<grenades_t>                  grenades;
    tag_block<vehicle_t>                   vehicle;
};

static_assert(offsetof(character_t, weapons) == 0xCC);
static_assert(sizeof(character_t) == 0xEC);

} // namespace chr

/*! itmc / vehc: weighted choice of what a spawn drops */
struct alignas(4) item_permutation_t
{
    f32 weight;
    tag_reference<tag_class_t::weap, tag_class_t::eqip, tag_class_t::vehi> item;
    string_id variant_name;
};

static_assert(sizeof(item_permutation_t) == 0x10);

struct alignas(4) item_collection_t
{
    tag_block<item_permutation_t> permutations;
    i16                           spawn_time_raw;
    i16                           unknown_0;

    inline i16 spawn_time() const
    {
        return from_le(spawn_time_raw);
    }
};

static_assert(sizeof(item_collection_t) == 0xC);

/*! matg: the globals this build was linked with */
namespace matg {

struct alignas(4) grenade_t
{
    i16                              maximum_count_raw;
    i16                              unknown_0;
    tag_reference<tag_class_t::effe> throwing_effect;
    u8                               unknown_1[0x1C - 0xC];
    tag_reference<tag_class_t::eqip> equipment;
    tag_reference<tag_class_t::proj> projectile;
};

static_assert(sizeof(grenade_t) == 0x2C);

struct alignas(4) multiplayer_information_t
{
    tag_reference<tag_class_t::weap> flag;
    tag_reference<tag_class_t::bipd> unit;
    tag_block<tag_ref_t>             vehicles;
    tag_reference<tag_class_t::shad> hill_shader;
    tag_reference<tag_class_t::shad> flag_shader;
    tag_reference<tag_class_t::weap> ball;
    tag_block<tag_ref_t>             sounds;
    tag_reference<tag_class_t::unic> in_game_text;
    u8                               unknown_0[0x68 - 0x40];
    tag_block<u8>                    general_events; /* elem 0xA8 */
    tag_block<u8>                    slayer_events;
    tag_block<u8>                    ctf_events;
    tag_block<u8>                    oddball_events;
    tag_block<u8>                    unknown_events;
    tag_block<u8>                    king_events;
};

static_assert(sizeof(multiplayer_information_t) == 0x98);

struct alignas(4) player_information_t
{
    tag_reference<>                  unused;
    u8                               unknown_0[0x24 - 8];
    f32                              walking_speed;
    u32                              unknown_1;
    f32                              run_forward;
    f32                              run_backward;
    f32                              run_sideways;
    f32                              run_acceleration;
    f32                              sneak_forward;
    f32                              sneak_backward;
    f32                              sneak_sideways;
    f32                              sneak_acceleration;
    f32                              airborne_acceleration;
    u8                               unknown_2[0x60 - 0x50];
    Vecf3                            grenade_origin;
    u8                               unknown_3[0x78 - 0x6C];
    f32                              stun_movement_penalty;
    f32                              stun_turning_penalty;
    f32                              stun_jumping_penalty;
    f32                              minimum_stun_time;
    f32                              maximum_stun_time;
    u8                               unknown_4[0x94 - 0x8C];
    Vecf2                            first_person_idle_time;
    f32                              first_person_skip_fraction;
    u8                               unknown_5[0xB0 - 0xA0];
    tag_reference<tag_class_t::effe> coop_respawn_effect;
    i32                              binoculars_zoom_count_raw;
    Vecf2                            binoculars_zoom_range;
    tag_reference<tag_class_t::snd>  binoculars_zoom_in_sound;
    tag_reference<tag_class_t::snd>  binoculars_zoom_out_sound;
    u8                               unknown_6[0xE4 - 0xD4];
    tag_reference<tag_class_t::snd>  active_camouflage_on;
    tag_reference<tag_class_t::snd>  active_camouflage_off;
    tag_reference<tag_class_t::snd>  active_camouflage_error;
    tag_reference<tag_class_t::snd>  active_camouflage_ready;
    tag_reference<tag_class_t::snd>  flashlight_on;
    tag_reference<tag_class_t::snd>  flashlight_off;
    tag_reference<tag_class_t::snd>  ice_cream;
};

static_assert(offsetof(player_information_t, coop_respawn_effect) == 0xB0);
static_assert(sizeof(player_information_t) == 0x11C);

struct alignas(4) player_representation_t
{
    tag_reference<tag_class_t::mode> first_person_hands;
    tag_reference<tag_class_t::mode> first_person_body;
    u8                               unknown_0[0xB0 - 0x10];
    tag_reference<tag_class_t::bipd> third_person_unit;
    string_id                        third_person_variant;
};

static_assert(sizeof(player_representation_t) == 0xBC);

enum class material_flags_t : u16
{
    none      = 0x0,
    flammable = 0x1,
    biomass   = 0x2,
};
C_FLAGS(material_flags_t, u16);

enum class material_type_t : u16
{
    dirt = 0,
    sand,
    stone,
    snow,
    wood,
    metal_hollow,
    metal_thin,
    metal_thick,
    rubber,
    glass,
    force_field,
    grunt,
    hunter_armor,
    hunter_skin,
    elite,
    jackal,
    jackal_energy_shield,
    engineer_skin,
    engineer_force_field,
    flood_combat_form,
    flood_carrier_form,
    cyborg_armor,
    cyborg_energy_shield,
    human_armor,
    human_skin,
    sentinel,
    monitor,
    plastic,
    water,
    leaves,
    elite_energy_shield,
    ice,
    hunter_shield,
};

enum class sweetener_inheritance_flags_t : u32
{
    none            = 0x0,
    sound_small     = 0x1,
    sound_medium    = 0x2,
    sound_large     = 0x4,
    sound_rolling   = 0x8,
    sound_grinding  = 0x10,
    sound_melee     = 0x20,
    effect_small    = 0x80,
    effect_medium   = 0x100,
    effect_large    = 0x200,
    effect_rolling  = 0x400,
    effect_grinding = 0x800,
    effect_melee    = 0x1000,
};
C_FLAGS(sweetener_inheritance_flags_t, u32);

enum class language_t : u32
{
    english = 0,
    japanese,
    german,
    french,
    spanish,
    italian,
    korean,
    chinese,
    portuguese,
};

struct alignas(4) material_t
{
    string_id                        name;
    string_id                        parent_name;
    i16                              unknown_0;
    material_flags_t                 flags;
    material_type_t                  old_material_type;
    i16                              unknown_1;
    string_id                        general_armor;
    string_id                        specific_armor;
    u32                              physics_flags; /* TODO: bits unknown */
    f32                              friction;
    f32                              restitution;
    f32                              density;
    tag_reference<>                  old_material_physics;
    tag_reference<tag_class_t::bsdt> breakable_surface;
    std::array<tag_reference<tag_class_t::snd>, 6>
        sound_sweeteners; /* small, medium, large, rolling, grinding,
                             melee */
    tag_reference<>                                 unknown_2;
    std::array<tag_reference<tag_class_t::effe>, 6> effect_sweeteners;
    tag_reference<>                                 unknown_3;
    sweetener_inheritance_flags_t                   sweetener_inheritance_flags;
    tag_reference<tag_class_t::foot>                material_effects;
};

static_assert(offsetof(material_t, sound_sweeteners) == 0x38);
static_assert(sizeof(material_t) == 0xB4);

struct alignas(4) globals_t
{
    u8                                   unknown_0[0xAC];
    language_t                           language;
    tag_block<u8>                        havok_cleanup_resources; /* elem 8 */
    tag_block<u8>                        collision_damage; /* elem 0x48 */
    tag_block<u8>                        sound_globals;    /* elem 0x24 */
    tag_block<u8>                        ai_globals;       /* elem 0x168 */
    tag_block<u8>                        damage_table;     /* elem 8 */
    tag_block<u8>                        unknown_1;
    tag_block<tag_ref_t>                 sounds;
    tag_block<u8>                        camera;         /* elem 0x14 */
    tag_block<u8>                        player_control; /* elem 0x80 */
    tag_block<u8>                        difficulty;     /* elem 0x284 */
    tag_block<grenade_t>                 grenades;
    tag_block<u8>                        rasterizer_data; /* elem 0x108 */
    tag_block<u8>                        interface_tags;  /* elem 0x98 */
    tag_block<tag_ref_t>                 weapon_list;
    u8                                   unknown_2[8];
    tag_block<multiplayer_information_t> multiplayer_information;
    tag_block<player_information_t>      player_information;
    tag_block<player_representation_t>   player_representation;
    tag_block<u8>                        falling_damage; /* elem 0x68 */
    tag_block<u8>                        old_materials;  /* elem 0x24 */
    tag_block<material_t>                materials;
    tag_block<u8>                        multiplayer_ui; /* elem 0x20 */
    tag_block<u8>                        profile_colors; /* elem 0xC */
    u8                                   unknown_3[8];
    tag_block<u8>                        runtime_level_data; /* elem 8 */
    tag_block<u8>                        ui_level_data;      /* elem 0x18 */
    u8                                   unknown_4[0x284 - 0x180];
};

static_assert(offsetof(globals_t, multiplayer_information) == 0x128);
static_assert(offsetof(globals_t, materials) == 0x150);
static_assert(sizeof(globals_t) == 0x284);

} // namespace matg

struct alignas(4) fog_t
{
    Vecf3 color;
    f32   maximum_density;
    f32   start_distance;
    f32   opaque_distance;
};

static_assert(sizeof(fog_t) == 0x18);

struct alignas(4) sky_fog_t
{
    Vecf3 color;
    f32   density;
};

static_assert(sizeof(sky_fog_t) == 0x10);

/*! Radiosity a sky light casts: the sun's colour and disc */
struct alignas(4) sky_radiosity_t
{
    u32                flags; /* TODO: flags */
    Vecf3              color;
    f32                power;
    f32                test_distance;
    std::array<u32, 3> unknown_0;
    f32                diameter; /*!< Radians */
};

static_assert(sizeof(sky_radiosity_t) == 0x28);

struct alignas(4) sky_light_t
{
    Vecf3                            direction_vector;
    Vecf2                            direction; /*!< Yaw, pitch in radians */
    tag_reference<tag_class_t::lens> lens_flare;
    tag_block<u8>                    fog;          /*!< elem 0x2C */
    tag_block<u8>                    fog_opposite; /*!< elem 0x2C */
    tag_block<sky_radiosity_t>       radiosity;
};

static_assert(sizeof(sky_light_t) == 0x34);

struct alignas(4) sky_t
{
    tag_reference<tag_class_t::mode> render_model; /*!< mode */
    tag_reference<tag_class_t::jmad> animation_graph;
    u32                              flags; /* TODO: flags */
    f32                              render_model_scale;
    f32                              movement_scale;
    tag_block<u8>                    cube_map; /*!< elem 0xC */
    Vecf3                            indoor_ambient;
    u32                              unknown_0;
    Vecf3                            outdoor_ambient;
    u32                              unknown_1;
    f32                              fog_spread_distance;
    tag_block<fog_t>                 atmospheric_fog;
    tag_block<fog_t>                 secondary_fog;
    tag_block<sky_fog_t>             sky_fog;
    tag_block<u8>                    patchy_fog; /*!< elem 0x50 */
    std::array<f32, 4>               bloom_override;
    tag_block<sky_light_t>           lights;
    f32                              global_rotation;
    tag_block<u8>                    shader_functions; /*!< elem 0x24 */
    tag_block<u8>                    animations;       /*!< elem 0x24 */
    std::array<u32, 3>               unknown_2;
    /*! Shown where the sky model leaves gaps, below the horizon; only set
     * where it shows (zanzibar's open sea) */
    Vecf3                            clear_color;
};

static_assert(offsetof(sky_t, outdoor_ambient) == 0x34);
static_assert(offsetof(sky_t, sky_fog) == 0x58);
static_assert(offsetof(sky_t, lights) == 0x78);
static_assert(offsetof(sky_t, clear_color) == 0xA0);
static_assert(sizeof(sky_t) == 0xAC);

/*! One sprite of a lens flare, placed along the line from the light
 * through the screen centre */
struct alignas(4) lens_reflection_t
{
    u16   flags; /* TODO: flags */
    i16   unknown_0;
    i16   image; /*!< Into the flare bitmap */
    i16   unknown_1;
    f32   position; /*!< 0 at the light, 0.5 at the screen centre */
    f32   rotation_offset;
    Vecf2 radius;     /*!< Fraction of the screen height */
    Vecf2 brightness;
    f32   modulation;
    Vecf3 color;
};

static_assert(sizeof(lens_reflection_t) == 0x30);

/*! lens: lens flare */
struct alignas(4) lens_t
{
    f32                                falloff_angle;
    f32                                cutoff_angle;
    f32                                cos_falloff_angle;
    f32                                cos_cutoff_angle;
    f32                                occlusion_radius;
    u16                                occlusion_offset;
    u16                                occlusion_inner_scale;
    f32                                near_fade_distance;
    f32                                far_fade_distance;
    tag_reference<tag_class_t::bitm>   bitmap;
    u16                                flags; /* TODO: flags */
    i16                                unknown_0;
    u16                                rotation_function;
    i16                                unknown_1;
    f32                                rotation_scale;
    Vecf2                              corona_scale;
    u16                                falloff_function;
    i16                                unknown_2;
    tag_block<lens_reflection_t>       reflections;
    u16                                flags_2; /* TODO: flags */
    i16                                unknown_3;
    tag_block<u8>                      brightness; /*!< elem 8 */
    tag_block<u8>                      color;      /*!< elem 8 */
    tag_block<u8>                      rotation;   /*!< elem 8 */
};

static_assert(offsetof(lens_t, reflections) == 0x40);
static_assert(sizeof(lens_t) == 0x64);

} // namespace blam::dimeter::obj
