#pragma once

#include "blam_base_types.h"
#include "blam_reference.h"
#include "blam_tag_ref.h"
#include "blam_unit.h"

#include <peripherals/enum/helpers.h>

namespace blam::scn {

/*! An actor's unit, weapon and combat tuning, aka actv */
struct actor_variant
{
    enum class flags_t : u32
    {
        none                                        = 0x0,
        can_shoot_while_flying                      = 0x1,
        interpolate_color_in_hsv                    = 0x2,
        has_unlimited_grenades                      = 0x4,
        movement_switching_try_to_stay_with_friends = 0x8,
        active_camouflage                           = 0x10,
        super_active_camouflage                     = 0x20,
        cannot_use_ranged_weapons                   = 0x40,
        prefer_passenger_seat                       = 0x80,
    };

    enum class movement_type_t : u16
    {
        always_run,
        always_crouch,
        switch_types,
    };

    enum class special_fire_mode_t : u16
    {
        none,
        overcharge,
        secondary_trigger,
    };

    enum class special_fire_situation_t : u16
    {
        never,
        enemy_visible,
        enemy_out_of_sight,
        strafing,
    };

    enum class trajectory_type_t : u16
    {
        toss,
        lob,
        bounce,
    };

    enum class grenade_stimulus_t : u16
    {
        never,
        visible_target,
        seek_cover,
    };

    /*! Firing when the target is new, while moving, or when berserk */
    struct fire_pattern_t
    {
        f32     burst_duration;
        f32     burst_separation;
        f32     rate_of_fire;
        angle_t projectile_error;
    };

    struct change_color_t
    {
        Vecf3 lower_bound;
        Vecf3 upper_bound;
        u32   padding[2];
    };

    flags_t                           flags;
    tagref_typed_t<tag_class_t::actr> actor_definition;
    tagref_typed_t<tag_class_t::unit> unit;
    tagref_typed_t<tag_class_t::actv> major_variant;
    u16 padding[2]; // MCC: metagame type and class, for CEA scoring
    u32 padding2[5];

    movement_type_t movement_type;
    u16             padding3;
    f32             initial_crouch_chance;
    Vecf2           crouch_time;
    Vecf2           run_time;

    tagref_typed_t<tag_class_t::weap> weapon;
    f32                               maximum_firing_distance;
    f32                               rate_of_fire;
    angle_t                           projectile_error;
    Vecf2                             first_burst_delay_time;
    f32                               new_target_firing_pattern_time;
    f32                               surprise_delay_time;
    f32                               surprise_fire_wildly_time;
    f32                               death_fire_wildly_chance;
    f32                               death_fire_wildly_time;
    Vecf2                             desired_combat_range;
    Vecf3                             custom_stand_gun_offset;
    Vecf3                             custom_crouch_gun_offset;

    f32     target_tracking;
    f32     target_leading;
    f32     weapon_damage_modifier;
    f32     damage_per_second;
    f32     burst_origin_radius;
    angle_t burst_origin_angle;
    Vecf2   burst_return_length;
    angle_t burst_return_angle;
    Vecf2   burst_duration;
    Vecf2   burst_separation;
    angle_t burst_angular_velocity;
    u32     padding4;

    f32            special_damage_modifier;
    angle_t        special_projectile_error;
    fire_pattern_t new_target;
    u32            padding5[2];
    fire_pattern_t moving;
    u32            padding6[2];
    fire_pattern_t berserk;
    u32            padding7[2];

    f32                      super_ballistic_range;
    f32                      bombardment_range;
    f32                      modified_vision_range;
    special_fire_mode_t      special_fire_mode;
    special_fire_situation_t special_fire_situation;
    f32                      special_fire_chance;
    f32                      special_fire_delay;
    f32                      melee_range;
    f32                      melee_abort_range;
    Vecf2                    berserk_firing_ranges;
    f32                      berserk_melee_range;
    f32                      berserk_melee_abort_range;
    u32                      padding8[2];

    grenade_type_t     grenade_type;
    trajectory_type_t  trajectory_type;
    grenade_stimulus_t grenade_stimulus;
    i16                minimum_enemy_count;
    f32                enemy_radius;
    u32                padding9;
    f32                grenade_velocity;
    Vecf2              grenade_ranges;
    f32                collateral_damage_radius;
    f32                grenade_chance;
    f32                grenade_check_time;
    f32                encounter_grenade_timeout;
    u32                padding10[5];

    tagref_typed_t<tag_class_t::eqip> equipment;
    i16                               grenade_count[2]; // min, max
    f32                               dont_drop_grenades_chance;
    Vecf2                             drop_weapon_loaded;
    i16                               drop_weapon_ammo[2]; // min, max
    u32                               padding11[7];

    f32 body_vitality;
    f32 shield_vitality;
    f32 shield_sapping_radius;
    i16 forced_shader_permutation;
    u16 padding12;
    u32 padding13[7];

    reference<change_color_t> change_colors;
};

C_FLAGS(actor_variant::flags_t, u32)

static_assert(sizeof(actor_variant::change_color_t) == 32);
static_assert(offsetof(actor_variant, movement_type) == 0x4c);
static_assert(offsetof(actor_variant, weapon) == 0x64);
static_assert(offsetof(actor_variant, desired_combat_range) == 0x9c);
static_assert(offsetof(actor_variant, burst_angular_velocity) == 0xf0);
static_assert(offsetof(actor_variant, new_target) == 0x100);
static_assert(offsetof(actor_variant, moving) == 0x118);
static_assert(offsetof(actor_variant, berserk) == 0x130);
static_assert(offsetof(actor_variant, special_fire_mode) == 0x154);
static_assert(offsetof(actor_variant, grenade_type) == 0x180);
static_assert(offsetof(actor_variant, equipment) == 0x1c0);
static_assert(offsetof(actor_variant, body_vitality) == 0x200);
static_assert(offsetof(actor_variant, change_colors) == 0x22c);
static_assert(sizeof(actor_variant) == 568);

} // namespace blam::scn
