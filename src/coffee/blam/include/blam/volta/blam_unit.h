#pragma once

#include "blam_base_types.h"
#include "blam_bsp_structures.h"
#include "blam_domain_ptrs.h"
#include "blam_effects.h"
#include "blam_mod2.h"
#include "blam_shaders.h"
#include "blam_strings.h"
#include "blam_tag_ref.h"

#include <peripherals/enum/helpers.h>

namespace blam::scn {

// Forward decls
struct hud_msg;

enum class gamemode_t : u16
{
    none,
    ctf,
    slayer,
    oddball,
    king_of_the_hill,
    race,
    terminator,
    stub,
    ignored1,
    ignored2,
    ignored3,
    ignored4,
    all_games,
    all_except_ctf,
    all_except_race_ctf,
};

enum class object_type : u16
{
    biped,
    vehicle,
    weapon,
    equipment,
    garbage,
    projectile,
    scenery,
    machine,
    control,
    light_fixture,
    placeholder,
    sound_scenery,
};

enum class object_flags : u16
{
    no_shadow                  = 0x1,
    transparent_self_occlusion = 0x2,
    bright = 0x4, /* "Brighter than it should be", as in unshaded? */
    not_pathfinding_obstacle = 0x8,
    extension_of_parent      = 0x10,
    cast_shadow_by_default   = 0x20,
    no_anniversary_geometry  = 0x40,
};

C_FLAGS(object_flags, u16)

/*! How loud an object is to AI */
enum class object_noise_t : u16
{
    silent,
    medium,
    loud,
    shout,
    quiet,
};

enum class grenade_type_t : u16
{
    human_fragmentation,
    covenant_plasma,
    grenade_type_2,
    grenade_type_3,
};

/*! Played on weapon events; either kind will do */
using sound_or_effect_t = tagref_typed_t<tag_class_t::snd, tag_class_t::effe>;

/*! Which of the A-D functions something follows */
enum class function_name_t : u16
{
    none,
    A,
    B,
    C,
    D,
};

struct object
{
    object_type  type;
    object_flags flags;
    f32          bound_radius;
    Vecf3        bound_offset;
    Vecf3        origin_offset;
    f32          acceleration_scale;
    u32          scales_change_colors; // cache only

    tagref_typed_t<tag_class_t::mod2> model;
    tagref_typed_t<tag_class_t::antr> anim_graph;
    u32                               padding2[10];
    tagref_typed_t<tag_class_t::coll> collider;
    tagref_typed_t<tag_class_t::phys> physics;
    tagref_typed_t<tag_class_t::shdr> shader;
    tagref_typed_t<tag_class_t::effe> creation_effect;
    u32                               padding3[21];
    f32                               render_bound_radius;

    struct export_functions_t
    {
        enum class input_t : u16
        {
            none,
            body_vitality,
            shield_vitality,
            recent_body_damage,
            recent_shield_damage,
            random_constant,
            umbrella_shield_vitality,
            shield_stun,
            recent_umbrella_shield_vitality,
            umbrella_shield_stun,
            region_00_damage,
            region_01_damage,
            region_02_damage,
            region_03_damage,
            region_04_damage,
            region_05_damage,
            region_06_damage,
            region_07_damage,
            alive,
            compass,
        } inputs[4];

        u32 padding[11];

        scenario_ptr<scn::hud_msg> hud_msg;
        mod2::model_ptr<tagref_t>  shader_perm;
    } export_;

    struct attachment_t
    {
        tagref_typed_t<
            tag_class_t::ligh,
            tag_class_t::mgs2,
            tag_class_t::cont,
            tag_class_t::pctl,
            tag_class_t::effe,
            tag_class_t::lsnd>
                              type;
        bl_string             marker; // model marker to attach to
        shader::animation_src primary_scale;
        shader::animation_src secondary_scale;
        function_name_t       change_color;
        u16                   padding;
        u32                   padding2[4];
    };

    struct widget_t
    {
        tagref_typed_t<
            tag_class_t::ant,
            tag_class_t::glw,
            tag_class_t::mgs2,
            tag_class_t::elec,
            tag_class_t::flag>
            widget;
        u32 padding[4];
    };

    struct function_t
    {
        enum class flags_t : u32
        {
            none          = 0x0,
            invert        = 0x1,
            additive      = 0x2,
            always_active = 0x4,
        };

        enum class bounds_mode_t : u16
        {
            clip,
            clip_and_normalize,
            scale_to_fit,
        };

        flags_t                    flags;
        f32                        period;
        shader::param_src          scale_period_by;
        shader::animation_function function;
        shader::param_src          scale_function_by;
        shader::animation_function wobble_function;
        f32                        wobble_period;
        f32                        wobble_magnitude;
        f32                        square_wave_threshold;
        i16                        step_count;
        function_type_t            map_to;
        i16                        sawtooth_count;
        shader::param_src          add;
        shader::param_src          scale_result_by;
        bounds_mode_t              bounds_mode;
        Vecf2                      bounds;
        u32                        padding;
        u16                        padding2;
        i16                        turn_off_with;
        f32                        scale_by;
        u32                        padding3[63];

        /* Cache only */
        f32 inverse_bounds;
        f32 inverse_sawtooth;
        f32 inverse_step;
        f32 inverse_period;

        bl_string usage;
    };

    struct color_perm_t
    {
        f32   weight;
        Vecf3 lower_bound;
        Vecf3 upper_bound;
    };

    struct change_color_t
    {
        shader::param_src darken_by;
        shader::param_src scale_by;

        enum scale_flags_t : u32
        {
            none         = 0x0,
            blend_in_hsv = 0x1,
            more_hues    = 0x2,
        } scale_flags;

        Vecf3                   lower_bound;
        Vecf3                   upper_bound;
        reference<color_perm_t> permutations;
    };

    reference<attachment_t>            attachments;
    reference<widget_t>                widgets;
    reference<function_t>              functions;
    reference<change_color_t>          change_colors;
    reference<bsp::predicted_resource> predicted_resources; // cache only
};

C_FLAGS(object::function_t::flags_t, u32)

static_assert(sizeof(object::attachment_t) == 72);
static_assert(sizeof(object::widget_t) == 32);
static_assert(offsetof(object::function_t, bounds) == 0x28);
static_assert(offsetof(object::function_t, usage) == 0x148);
static_assert(sizeof(object::function_t) == 360);
static_assert(sizeof(object::change_color_t) == 44);
static_assert(sizeof(object::color_perm_t) == 28);
static_assert(offsetof(object, physics) == 0x80);
static_assert(offsetof(object, render_bound_radius) == 0x104);
static_assert(offsetof(object, export_) == 0x108);
static_assert(offsetof(object, attachments) == 0x140);
static_assert(sizeof(object) == 380);

struct item : object
{
    enum class item_flags_t : u32
    {
        none                    = 0x0,
        always_maintains_z_up   = 0x1,
        destroyed_by_explosions = 0x2,
        unaffected_by_gravity   = 0x4,
    };

    enum class item_function_in_t : u16
    {
        none,
    };

    item_flags_t item_flags;

    /* Guerilla note:
     * Sets which string from tags\ui\hud\hud_item_messages to display
     */
    scenario_ptr<ui::unicode_string_list> message_index;
    i16                                   sort_order; // ???
    f32                                   scale;
    i16                                   hud_message_value_scale;
    u16                                   padding;
    u32                                   padding2[4];
    item_function_in_t                    item_inputs[4];
    u32                                   padding3[41];
    tagref_typed_t<tag_class_t::foot>     material_effect;
    tagref_typed_t<tag_class_t::snd>      collision_sound;
    u32                                   padding4[30];
    Vecf2                                 detonation_delay;
    tagref_typed_t<tag_class_t::effe>     detonating_effect;
    tagref_typed_t<tag_class_t::effe>     detonated_effect;
};

C_FLAGS(item::item_flags_t, u32)

static_assert(offsetof(item, item_inputs) == 0x19c);
static_assert(offsetof(item, material_effect) == 0x248);
static_assert(offsetof(item, detonation_delay) == 0x2e0);
static_assert(sizeof(item) == 776);

struct unit : object
{
    enum class unit_flags_t : u32
    {
        none                             = 0x0,
        circular_aiming                  = 0x1,
        destroyed_after_dying            = 0x2,
        half_speed_interpolation         = 0x4,
        fires_from_camera                = 0x8,
        entrance_inside_bounding_sphere  = 0x10,
        unused                           = 0x20,
        causes_passenger_dialogue        = 0x40,
        resists_pings                    = 0x80,
        melee_attack_is_fatal            = 0x100,
        dont_reface_during_pings         = 0x200,
        has_no_aiming                    = 0x400,
        simple_creature                  = 0x800,
        impact_melee_attaches_to_unit    = 0x1000,
        impact_melee_dies_on_shields     = 0x2000,
        cannot_open_doors_automatically  = 0x4000,
        melee_attackers_cannot_attach    = 0x8000,
        not_instantly_killed_by_melee    = 0x10000,
        shield_sapping                   = 0x20000,
        runs_around_flaming              = 0x40000,
        inconsequential                  = 0x80000,
        special_cinematic_unit           = 0x100000,
        ignored_by_autoaiming            = 0x200000,
        shields_fry_infection_forms      = 0x400000,
        integrated_light_controls_weapon = 0x800000,
        integrated_light_lasts_forever   = 0x1000000,
    };

    enum class team_t : u16
    {
        none,
        player,
        human,
        covenant,
        flood,
        sentinel,
        unused6,
        unused7,
        unused8,
        unused9,
    };

    enum class unit_function_in_t : u16
    {
        none,
        driver_seat_power,
        gunner_seat_power,
        aiming_change,
        mouth_aperture,
        integrated_light_power,
        can_blink,
        shield_sapping,
    };

    enum class blip_size_t : u16
    {
        medium,
        small,
        large,
    };

    struct camera_track_t
    {
        tagref_typed_t<tag_class_t::trak> track;
        u32                               padding[3];
    };

    struct hud_interface_t
    {
        tagref_typed_t<tag_class_t::unhi> hud;
        u32                               padding[8];
    };

    struct dialogue_variant_t
    {
        i16                               variant_number;
        u16                               padding;
        u32                               padding2;
        tagref_typed_t<tag_class_t::udlg> dialogue;
    };

    struct powered_seat_t
    {
        u32 padding;
        f32 driver_powerup_time;
        f32 driver_powerdown_time;
        u32 padding2[14];
    };

    struct weapon_t
    {
        tagref_typed_t<tag_class_t::weap> weapon;
        u32                               padding[5];
    };

    struct seat_t
    {
        enum class seat_flags_t : u32
        {
            none                                   = 0x0,
            invisible                              = 0x1,
            locked                                 = 0x2,
            driver                                 = 0x4,
            gunner                                 = 0x8,
            third_person_camera                    = 0x10,
            allows_weapons                         = 0x20,
            third_person_on_enter                  = 0x40,
            first_person_camera_slaved_to_gun      = 0x80,
            allow_vehicle_communication_animations = 0x100,
            not_valid_without_driver               = 0x200,
            allow_ai_noncombatants                 = 0x400,
        };

        seat_flags_t                      flags;
        bl_string                         label;
        bl_string                         marker_name;
        u32                               padding[8];
        Vecf3                             acceleration_scale;
        u32                               padding2[3];
        f32                               yaw_rate;
        f32                               pitch_rate;
        bl_string                         camera_marker_name;
        bl_string                         camera_submerged_marker_name;
        angle_t                           pitch_auto_level;
        Vecf2                             pitch_range;
        reference<camera_track_t>         camera_tracks;
        reference<hud_interface_t>        hud_interfaces;
        u32                               padding3;
        i16                               hud_text_message_index;
        u16                               padding4;
        angle_t                           yaw_minimum;
        angle_t                           yaw_maximum;
        tagref_typed_t<tag_class_t::actv> built_in_gunner;
        u32                               padding5[5];
    };

    unit_flags_t                      unit_flags;
    team_t                            default_team;
    object_noise_t                    constant_sound_volume;
    f32                               rider_damage_fraction;
    tagref_typed_t<tag_class_t::effe> integrated_light_toggle_effect;
    unit_function_in_t                unit_inputs[4];

    angle_t                   camera_field_of_view;
    f32                       camera_stiffness;
    bl_string                 camera_marker_name;
    bl_string                 camera_submerged_marker_name;
    angle_t                   pitch_auto_level;
    Vecf2                     pitch_range;
    reference<camera_track_t> camera_tracks;
    Vecf3                     seat_acceleration_scale;
    u32                       padding[3];

    f32 soft_ping_threshold;
    f32 soft_ping_interrupt_time;
    f32 hard_ping_threshold;
    f32 hard_ping_interrupt_time;
    f32 hard_death_threshold;
    f32 feign_death_threshold;
    f32 feign_death_time;
    f32 distance_of_evade_anim;
    f32 distance_of_dive_anim;
    u32 padding2;
    f32 stunned_movement_threshold;
    f32 feign_death_chance;
    f32 feign_repeat_chance;

    tagref_typed_t<tag_class_t::actv> spawned_actor;
    i16                               spawned_actor_count[2]; // min, max
    f32                               spawned_velocity;
    angle_t                           aiming_velocity_maximum;
    angle_t                           aiming_acceleration_maximum;
    f32                               casual_aiming_modifier;
    angle_t                           looking_velocity_maximum;
    angle_t                           looking_acceleration_maximum;
    u32                               padding3[2];
    f32                               ai_vehicle_radius;
    f32                               ai_danger_radius;
    tagref_typed_t<tag_class_t::jpt>  melee_damage;
    blip_size_t                       motion_sensor_blip_size;
    u16                               padding4;
    u16 padding5[2]; // MCC: metagame type and class, for CEA scoring
    u32 padding6[2];

    reference<hud_interface_t>    hud_interfaces;
    reference<dialogue_variant_t> dialogue_variants;
    f32                           grenade_velocity;
    grenade_type_t                grenade_type;
    i16                           grenade_count;
    i16                           soft_ping_interrupt_ticks; // cache only
    i16                           hard_ping_interrupt_ticks; // cache only
    reference<powered_seat_t>     powered_seats;
    reference<weapon_t>           weapons;
    reference<seat_t>             seats;
};

C_FLAGS(unit::unit_flags_t, u32)
C_FLAGS(unit::seat_t::seat_flags_t, u32)

static_assert(sizeof(unit::camera_track_t) == 28);
static_assert(sizeof(unit::hud_interface_t) == 48);
static_assert(sizeof(unit::dialogue_variant_t) == 24);
static_assert(sizeof(unit::powered_seat_t) == 68);
static_assert(sizeof(unit::weapon_t) == 36);
static_assert(offsetof(unit::seat_t, camera_tracks) == 0xd0);
static_assert(offsetof(unit::seat_t, built_in_gunner) == 0xf8);
static_assert(sizeof(unit::seat_t) == 284);
static_assert(offsetof(unit, camera_field_of_view) == 0x1a0);
static_assert(offsetof(unit, camera_tracks) == 0x1f4);
static_assert(offsetof(unit, spawned_actor) == 0x24c);
static_assert(offsetof(unit, melee_damage) == 0x288);
static_assert(offsetof(unit, hud_interfaces) == 0x2a8);
static_assert(offsetof(unit, seats) == 0x2e4);
static_assert(sizeof(unit) == 752);

struct biped : unit
{
    enum class biped_flags_t : u32
    {
        none                              = 0x0,
        turns_without_animating           = 0x1,
        uses_player_physics               = 0x2,
        flying                            = 0x4,
        physics_pill_centered_at_origin   = 0x8,
        spherical                         = 0x10,
        passes_through_other_bipeds       = 0x20,
        can_climb_any_surface             = 0x40,
        immune_to_falling_damage          = 0x80,
        rotate_while_airborne             = 0x100,
        uses_limp_body_physics            = 0x200,
        has_no_dying_airborne             = 0x400,
        random_speed_increase             = 0x800,
        unit_uses_old_ntsc_player_physics = 0x1000,
    };

    enum class biped_function_in_t : u16
    {
        none,
        flying_velocity,
    };

    struct contact_point_t
    {
        u32       padding[8];
        bl_string marker_name;
    };

    angle_t                          moving_turning_speed;
    biped_flags_t                    biped_flags;
    angle_t                          stationary_turning_threshold;
    u32                              padding[4];
    biped_function_in_t              biped_inputs[4];
    tagref_typed_t<tag_class_t::jpt> dont_use;

    angle_t bank_angle;
    f32     bank_apply_time;
    f32     bank_decay_time;
    f32     pitch_ratio;
    f32     max_velocity;
    f32     max_sidestep_velocity;
    f32     acceleration;
    f32     deceleration;
    angle_t angular_velocity_maximum;
    angle_t angular_acceleration_maximum;
    f32     crouch_velocity_modifier;
    u32     padding2[2];

    angle_t maximum_slope_angle;
    angle_t downhill_falloff_angle;
    angle_t downhill_cutoff_angle;
    f32     downhill_velocity_scale;
    angle_t uphill_falloff_angle;
    angle_t uphill_cutoff_angle;
    f32     uphill_velocity_scale;
    u32     padding3[6];

    tagref_typed_t<tag_class_t::foot> footsteps;
    u32                               padding4[6];
    f32                               jump_velocity;
    u32                               padding5[7];

    f32 maximum_soft_landing_time;
    f32 maximum_hard_landing_time;
    f32 minimum_soft_landing_velocity;
    f32 minimum_hard_landing_velocity;
    f32 maximum_hard_landing_velocity;
    f32 death_hard_landing_velocity;
    u32 padding6[5];

    f32 standing_camera_height;
    f32 crouching_camera_height;
    f32 crouch_transition_time;
    u32 padding7[6];
    f32 standing_collision_height;
    f32 crouching_collision_height;
    f32 collision_radius;
    u32 padding8[10];
    f32 autoaim_width;
    u32 padding9[27];

    /* Cache only */
    f32 cosine_stationary_turning_threshold;
    f32 crouch_camera_velocity;
    f32 cosine_maximum_slope_angle;
    f32 negative_sine_downhill_falloff_angle;
    f32 negative_sine_downhill_cutoff_angle;
    f32 sine_uphill_falloff_angle;
    f32 sine_uphill_cutoff_angle;
    i16 pelvis_node_index;
    i16 head_node_index;

    reference<contact_point_t> contact_points;
};

C_FLAGS(biped::biped_flags_t, u32)

static_assert(sizeof(biped::contact_point_t) == 64);
static_assert(offsetof(biped, dont_use) == 0x314);
static_assert(offsetof(biped, footsteps) == 0x38c);
static_assert(offsetof(biped, standing_camera_height) == 0x400);
static_assert(offsetof(biped, standing_collision_height) == 0x424);
static_assert(offsetof(biped, collision_radius) == 0x42c);
static_assert(offsetof(biped, contact_points) == 0x4e8);
static_assert(sizeof(biped) == 1268);

struct vehicle : unit
{
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
        unused                              = 0x10000,
        has_ebrake                          = 0x20000,
        noncombat_vehicle                   = 0x40000,
        no_friction_with_driver             = 0x80000,
        can_trigger_automatic_opening_doors = 0x100000,
        autoaim_when_teamless               = 0x200000,
    };

    enum class vehicle_type_t : u16
    {
        human_tank,
        human_jeep,
        human_boat,
        human_plane,
        alien_scout,
        alien_fighter,
        turret,
    };

    enum class vehicle_function_in_t : u16
    {
        none,
        speed_absolute,
        speed_forward,
        speed_backward,
        slide_absolute,
        slide_left,
        slide_right,
        speed_slide_maximum,
        turn_absolute,
        turn_left,
        turn_right,
        crouch,
        jump,
        walk,
        velocity_air,
        velocity_water,
        velocity_ground,
        velocity_forward,
        velocity_left,
        velocity_up,
        left_tread_position,
        right_tread_position,
        left_tread_velocity,
        right_tread_velocity,
        front_left_tire_position,
        front_right_tire_position,
        back_left_tire_position,
        back_right_tire_position,
        front_left_tire_velocity,
        front_right_tire_velocity,
        back_left_tire_velocity,
        back_right_tire_velocity,
        wingtip_contrail,
        hover,
        thrust,
        engine_hack,
        wingtip_contrail_new,
    };

    vehicle_flags_t vehicle_flags;
    vehicle_type_t  vehicle_type;
    u16             padding;

    f32                   maximum_forward_speed;
    f32                   maximum_reverse_speed;
    f32                   speed_acceleration;
    f32                   speed_deceleration;
    f32                   maximum_left_turn;
    f32                   maximum_right_turn;
    f32                   wheel_circumference;
    f32                   turn_rate;
    f32                   blur_speed;
    vehicle_function_in_t vehicle_inputs[4];
    u32                   padding2[3];

    f32 maximum_left_slide;
    f32 maximum_right_slide;
    f32 slide_acceleration;
    f32 slide_deceleration;
    f32 minimum_flipping_angular_velocity;
    f32 maximum_flipping_angular_velocity;
    u32 padding3[6];
    f32 fixed_gun_yaw;
    f32 fixed_gun_pitch;
    u32 padding4[6];

    f32     ai_sideslip_distance;
    f32     ai_destination_radius;
    f32     ai_avoidance_distance;
    f32     ai_pathfinding_radius;
    f32     ai_charge_repeat_timeout;
    f32     ai_strafing_abort_range;
    Vecf2   ai_oversteering_bounds;
    angle_t ai_steering_maximum;
    f32     ai_throttle_maximum;
    f32     ai_move_position_time;
    u32     padding5;

    tagref_typed_t<tag_class_t::snd>  suspension_sound;
    tagref_typed_t<tag_class_t::snd>  crash_sound;
    tagref_typed_t<tag_class_t::foot> material_effects;
    tagref_typed_t<tag_class_t::effe> effect;
};

C_FLAGS(vehicle::vehicle_flags_t, u32)

static_assert(offsetof(vehicle, vehicle_inputs) == 0x31c);
static_assert(offsetof(vehicle, maximum_left_slide) == 0x330);
static_assert(offsetof(vehicle, fixed_gun_yaw) == 0x360);
static_assert(offsetof(vehicle, ai_oversteering_bounds) == 0x398);
static_assert(offsetof(vehicle, suspension_sound) == 0x3b0);
static_assert(sizeof(vehicle) == 1008);

// TODO: Scenery is a BasicObject in the tag (object + 128 bytes, 508 total),
// not a unit
struct scenery : unit
{
};

struct weapon : item
{
    enum class weapon_flags_t : u32
    {
        none                                 = 0x0,
        vertical_heat_display                = 0x1,
        mutually_exclusive_triggers          = 0x2,
        attacks_automatically_on_bump        = 0x4,
        must_be_readied                      = 0x8,
        doesnt_count_toward_maximum          = 0x10,
        aim_assists_only_when_zoomed         = 0x20,
        prevents_grenade_throwing            = 0x40,
        must_be_picked_up                    = 0x80,
        holds_triggers_when_dropped          = 0x100,
        prevents_melee_attack                = 0x200,
        detonates_when_dropped               = 0x400,
        cannot_fire_at_maximum_age           = 0x800,
        secondary_trigger_overrides_grenades = 0x1000,
        does_not_depower_active_camo_in_mp   = 0x2000,
        enables_integrated_night_vision      = 0x4000,
        ais_use_weapon_melee_damage          = 0x8000,
        prevents_crouching                   = 0x10000,
        uses_3rd_person_camera               = 0x20000,
    };

    enum class secondary_trigger_mode_t : u16
    {
        normal,
        slaved_to_primary,
        inhibits_primary,
        loads_alternate_ammunition,
        loads_multiple_primary_ammunition,
    };

    enum class weapon_function_in_t : u16
    {
        none,
        heat,
        primary_ammunition,
        secondary_ammunition,
        primary_rate_of_fire,
        secondary_rate_of_fire,
        ready,
        primary_ejection_port,
        secondary_ejection_port,
        overheated,
        primary_charged,
        secondary_charged,
        illumination,
        age,
        integrated_light,
        primary_firing,
        secondary_firing,
        primary_firing_on,
        secondary_firing_on,
    };

    enum class movement_penalized_t : u16
    {
        always,
        when_zoomed,
        when_zoomed_or_reloading,
    };

    enum class weapon_type_t : u16
    {
        undefined,
        shotgun,
        needler,
        plasma_pistol,
        plasma_rifle,
        rocket_launcher,
    };

    struct magazine_object_t
    {
        i16                               rounds;
        u16                               padding[5];
        tagref_typed_t<tag_class_t::eqip> equipment;
    };

    struct magazine_t
    {
        enum class flags_t : u32
        {
            none                          = 0x0,
            wastes_rounds_when_reloaded   = 0x1,
            every_round_must_be_chambered = 0x2,
        };

        flags_t flags;
        i16     rounds_recharged;
        i16     rounds_total_initial;
        i16     rounds_reserved_maximum;
        i16     rounds_loaded_maximum;
        u32     padding[2];
        f32     reload_time;
        i16     rounds_reloaded;
        u16     padding2;
        f32     chamber_time;
        u32     padding3[6];

        sound_or_effect_t reloading_effect;
        sound_or_effect_t chambering_effect;

        u32                          padding4[3];
        reference<magazine_object_t> magazine_objects;
    };

    struct firing_effect_t
    {
        i16 shot_count[2]; // lower, upper
        u32 padding[8];

        sound_or_effect_t firing_effect;
        sound_or_effect_t misfire_effect;
        sound_or_effect_t empty_effect;

        tagref_typed_t<tag_class_t::jpt> firing_damage;
        tagref_typed_t<tag_class_t::jpt> misfire_damage;
        tagref_typed_t<tag_class_t::jpt> empty_damage;
    };

    struct trigger_t
    {
        enum class flags_t : u32
        {
            none                                    = 0x0,
            tracks_fired_projectile                 = 0x1,
            random_firing_effects                   = 0x2,
            can_fire_with_partial_ammo              = 0x4,
            does_not_repeat_automatically           = 0x8,
            locks_in_on_off_state                   = 0x10,
            projectiles_use_weapon_origin           = 0x20,
            sticks_when_dropped                     = 0x40,
            ejects_during_chamber                   = 0x80,
            discharging_spews                       = 0x100,
            analog_rate_of_fire                     = 0x200,
            use_error_when_unzoomed                 = 0x400,
            projectile_vector_cannot_be_adjusted    = 0x800,
            projectiles_have_identical_error        = 0x1000,
            projectile_is_client_side_only          = 0x2000,
            use_original_unit_adjust_projectile_ray = 0x4000,
        };

        enum class prediction_type_t : u16
        {
            none,
            continuous,
            instant,
        };

        enum class overcharged_action_t : u16
        {
            none,
            explode,
            discharge,
        };

        enum class distribution_function_t : u16
        {
            point,
            horizontal_fan,
        };

        flags_t              flags;
        Vecf2                maximum_rate_of_fire;
        f32                  acceleration_time;
        f32                  deceleration_time;
        f32                  blurred_rate_of_fire;
        u32                  padding[2];
        i16                  magazine; // index into weapon::magazines
        i16                  rounds_per_shot;
        i16                  minimum_rounds_loaded;
        i16                  projectiles_between_contrails;
        u32                  padding2;
        prediction_type_t    prediction_type;
        object_noise_t       firing_noise;
        Vecf2                error;
        f32                  error_acceleration_time;
        f32                  error_deceleration_time;
        u32                  padding3[2];
        f32                  charging_time;
        f32                  charged_time;
        overcharged_action_t overcharged_action;
        u16                  padding4;
        f32                  charged_illumination;
        f32                  spew_time;
        sound_or_effect_t    charging_effect;

        distribution_function_t           distribution_function;
        i16                               projectiles_per_shot;
        angle_t                           distribution_angle;
        u32                               padding5;
        angle_t                           minimum_error;
        Vecf2                             error_angle;
        Vecf3                             first_person_offset;
        u32                               padding6;
        tagref_typed_t<tag_class_t::proj> projectile;
        f32                               ejection_port_recovery_time;
        f32                               illumination_recovery_time;
        u32                               padding7[3];
        f32                               heat_generated_per_round;
        f32                               age_generated_per_round;
        u32                               padding8;
        f32                               overload_time;
        u32                               padding9[10];

        /* Cache only */
        f32 illumination_recovery_rate;
        f32 ejection_port_recovery_rate;
        f32 firing_acceleration_rate;
        f32 firing_deceleration_rate;
        f32 error_acceleration_rate;
        f32 error_deceleration_rate;

        reference<firing_effect_t> firing_effects;
    };

    weapon_flags_t           weapon_flags;
    bl_string                label;
    secondary_trigger_mode_t secondary_trigger_mode;
    i16                      maximum_alternate_shots_loaded;
    weapon_function_in_t     weapon_inputs[4];
    f32                      ready_time;
    sound_or_effect_t        ready_effect;

    f32 heat_recovery_threshold;
    f32 overheated_threshold;
    f32 heat_detonation_threshold;
    f32 heat_detonation_fraction;
    f32 heat_loss_rate;
    f32 heat_illumination;
    u32 padding[4];

    sound_or_effect_t overheated;
    sound_or_effect_t overheat_detonation;

    tagref_typed_t<tag_class_t::jpt>  player_melee_damage;
    tagref_typed_t<tag_class_t::jpt>  player_melee_response;
    u32                               padding2[2];
    tagref_typed_t<tag_class_t::actv> actor_firing_parameters;

    f32     near_reticle_range;
    f32     far_reticle_range;
    f32     intersection_reticle_range;
    u16     padding3;
    i16     zoom_levels;
    Vecf2   zoom_magnification_range;
    angle_t autoaim_angle;
    f32     autoaim_range;
    angle_t magnetism_angle;
    f32     magnetism_range;
    angle_t deviation_angle;
    u32     padding4;

    movement_penalized_t movement_penalized;
    u16                  padding5;
    f32                  forward_movement_penalty;
    f32                  sideways_movement_penalty;
    u32                  padding6;
    f32                  minimum_target_range;
    f32                  looking_time_modifier;
    u32                  padding7;

    f32 light_power_on_time;
    f32 light_power_off_time;

    sound_or_effect_t light_power_on_effect;
    sound_or_effect_t light_power_off_effect;

    f32 age_heat_recovery_penalty;
    f32 age_rate_of_fire_penalty;
    f32 age_misfire_start;
    f32 age_misfire_chance;
    u32 padding8[3];

    tagref_typed_t<tag_class_t::mod2> first_person_model;
    tagref_typed_t<tag_class_t::antr> first_person_animations;
    u32                               padding9;
    tagref_typed_t<tag_class_t::wphi> hud_interface;
    tagref_typed_t<tag_class_t::snd>  pickup_sound;
    tagref_typed_t<tag_class_t::snd>  zoom_in_sound;
    tagref_typed_t<tag_class_t::snd>  zoom_out_sound;
    u32                               padding10[3];
    f32                               active_camo_ding;
    f32                               active_camo_regrowth_rate;
    u32                               padding11[3];
    u16                               padding12;
    weapon_type_t                     weapon_type;

    reference<bsp::predicted_resource> more_predicted_resources; // cache only
    reference<magazine_t>              magazines;
    reference<trigger_t>               triggers;
};

C_FLAGS(weapon::weapon_flags_t, u32)
C_FLAGS(weapon::magazine_t::flags_t, u32)
C_FLAGS(weapon::trigger_t::flags_t, u32)

static_assert(sizeof(weapon::magazine_object_t) == 28);
static_assert(offsetof(weapon::magazine_t, reloading_effect) == 0x38);
static_assert(sizeof(weapon::magazine_t) == 112);
static_assert(offsetof(weapon::firing_effect_t, firing_damage) == 0x54);
static_assert(sizeof(weapon::firing_effect_t) == 132);
static_assert(offsetof(weapon::trigger_t, prediction_type) == 0x2c);
static_assert(offsetof(weapon::trigger_t, charging_effect) == 0x5c);
static_assert(offsetof(weapon::trigger_t, projectile) == 0x94);
static_assert(offsetof(weapon::trigger_t, firing_effects) == 0x108);
static_assert(sizeof(weapon::trigger_t) == 276);
static_assert(offsetof(weapon, label) == 0x30c);
static_assert(offsetof(weapon, ready_effect) == 0x33c);
static_assert(offsetof(weapon, actor_firing_parameters) == 0x3bc);
static_assert(offsetof(weapon, zoom_magnification_range) == 0x3dc);
static_assert(offsetof(weapon, movement_penalized) == 0x3fc);
static_assert(offsetof(weapon, light_power_on_effect) == 0x420);
static_assert(offsetof(weapon, first_person_model) == 0x45c);
static_assert(offsetof(weapon, hud_interface) == 0x480);
static_assert(offsetof(weapon, weapon_type) == 0x4e2);
static_assert(offsetof(weapon, triggers) == 0x4fc);
static_assert(sizeof(weapon) == 1288);

struct equipment : item
{
    enum class powerup_type_t : u16
    {
        none,
        double_speed,
        over_shield,
        active_camouflage,
        full_spectrum_vision,
        health,
        grenade,
    };

    powerup_type_t                   powerup_type;
    grenade_type_t                   grenade_type;
    f32                              powerup_time;
    tagref_typed_t<tag_class_t::snd> pickup_sound;
    u32                              padding[36];
};

static_assert(offsetof(equipment, pickup_sound) == 0x310);
static_assert(sizeof(equipment) == 944);

} // namespace blam::scn
