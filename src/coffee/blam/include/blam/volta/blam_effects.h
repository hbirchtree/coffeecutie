#pragma once

#include "blam_base_types.h"
#include "blam_reference.h"
#include "blam_shaders.h"
#include "blam_tag_ref.h"

#include <peripherals/enum/helpers.h>

namespace blam::scn {

/*! The curve a value follows over time, e.g. a fade */
enum class function_type_t : u16
{
    linear,
    early,
    very_early,
    late,
    very_late,
    cosine,
};

using material_type_t = shader::radiosity_properties::physics_material;

/*! Number of material types, which per-material tables are indexed by */
constexpr u32 material_type_count = 33;

/*! The effects and sounds played when something hits or moves over each
 *  material, aka foot */
struct material_effects
{
    struct material_t
    {
        tagref_typed_t<tag_class_t::effe> effect;
        tagref_typed_t<tag_class_t::snd>  sound;
        u32                               padding[4];
    };

    struct effect_t
    {
        reference<material_t> materials; // indexed by material_type_t
        u32                   padding[4];
    };

    reference<effect_t> effects;
    u32                 padding[32];
};

static_assert(sizeof(material_effects::material_t) == 48);
static_assert(sizeof(material_effects::effect_t) == 28);
static_assert(sizeof(material_effects) == 140);

/*! Camera path for a seat or unit, aka trak */
struct camera_track
{
    struct control_point_t
    {
        Vecf3 position;
        Vecf4 orientation; // quaternion, i j k w
        u32   padding[8];
    };

    u32                        flags; // only an "unused" bit
    reference<control_point_t> control_points;
    u32                        padding[8];
};

static_assert(sizeof(camera_track::control_point_t) == 60);
static_assert(sizeof(camera_track) == 48);

/*! Damage, screen effects and camera shake, aka jpt! */
struct damage_effect
{
    enum class flags_t : u32
    {
        none                            = 0x0,
        do_not_scale_damage_by_distance = 0x1,
    };

    enum class screen_flash_type_t : u16
    {
        none,
        lighten,
        darken,
        max,
        min,
        invert,
        tint,
    };

    enum class screen_flash_priority_t : u16
    {
        low,
        medium,
        high,
    };

    enum class side_effect_t : u16
    {
        none,
        harmless,
        lethal_to_the_unsuspecting,
        emp,
    };

    enum class category_t : u16
    {
        none,
        falling,
        bullet,
        grenade,
        high_explosive,
        sniper,
        melee,
        flame,
        mounted_weapon,
        vehicle,
        plasma,
        needle,
        shotgun,
    };

    enum class damage_flags_t : u32
    {
        none                                = 0x0,
        does_not_hurt_owner                 = 0x1,
        can_cause_headshots                 = 0x2,
        pings_resistant_units               = 0x4,
        does_not_hurt_friends               = 0x8,
        does_not_ping_units                 = 0x10,
        detonates_explosives                = 0x20,
        only_hurts_shields                  = 0x40,
        causes_flaming_death                = 0x80,
        damage_indicators_always_point_down = 0x100,
        skips_shields                       = 0x200,
        only_hurts_one_infection_form       = 0x400,
        can_cause_multiplayer_headshots     = 0x800,
        infection_form_pop                  = 0x1000,
        ignore_seat_scale_for_dir_dmg       = 0x2000,
        forces_hard_ping                    = 0x4000,
        does_not_hurt_players               = 0x8000,
        use_3d_instantaneous_acceleration   = 0x10000,
        allow_any_non_zero_acceleration     = 0x20000,
    };

    Vecf2   radius;
    f32     cutoff_scale;
    flags_t flags;
    u32     padding[5];

    struct
    {
        screen_flash_type_t     type;
        screen_flash_priority_t priority;
        u32                     padding[3];
        f32                     duration;
        function_type_t         fade_function;
        u16                     padding2;
        u32                     padding3[2];
        f32                     maximum_intensity;
        u32                     padding4;
        Vecf4                   color; // a r g b
    } screen_flash;

    struct vibrate_t
    {
        f32             frequency;
        f32             duration;
        function_type_t fade_function;
        u16             padding;
    };

    vibrate_t low_frequency_vibrate;
    u32       padding2[2];
    vibrate_t high_frequency_vibrate;
    u32       padding3[7];

    struct
    {
        f32             duration;
        function_type_t fade_function;
        u16             padding;
        f32             rotation;
        f32             pushback;
        Vecf2           jitter;
        u32             padding2[2];
    } temporary_camera_impulse;

    f32 permanent_camera_impulse_angle;
    u32 padding4[4];

    struct
    {
        f32                        duration;
        function_type_t            falloff_function;
        u16                        padding;
        f32                        random_translation;
        f32                        random_rotation;
        u32                        padding2[3];
        shader::animation_function wobble_function;
        u16                        padding3;
        f32                        wobble_period;
        f32                        wobble_weight;
        u32                        padding4[8];
    } camera_shaking;

    tagref_typed_t<tag_class_t::snd> sound;
    u32                              padding5[28];

    struct breaking_effect_t
    {
        f32 velocity;
        f32 radius;
        f32 exponent;
    };

    breaking_effect_t breaking_effect_forward;
    u32               padding6[3];
    breaking_effect_t breaking_effect_outward;
    u32               padding7[3];

    side_effect_t  side_effect;
    category_t     category;
    damage_flags_t damage_flags;
    f32            aoe_core_radius;
    f32            lower_bound;
    Vecf2          upper_bound;
    f32            vehicle_passthrough_penalty;
    f32            active_camouflage_damage;
    f32            stun;
    f32            maximum_stun;
    f32            stun_time;
    u32            padding8;
    Vecf3          instantaneous_acceleration;

    /* Damage scale against each material, indexed by material_type_t */
    f32 material_modifiers[material_type_count];
    u32 padding9[7];
};

C_FLAGS(damage_effect::flags_t, u32)
C_FLAGS(damage_effect::damage_flags_t, u32)

static_assert(offsetof(damage_effect, screen_flash) == 0x24);
static_assert(offsetof(damage_effect, low_frequency_vibrate) == 0x5c);
static_assert(offsetof(damage_effect, high_frequency_vibrate) == 0x70);
static_assert(offsetof(damage_effect, temporary_camera_impulse) == 0x98);
static_assert(offsetof(damage_effect, permanent_camera_impulse_angle) == 0xb8);
static_assert(offsetof(damage_effect, camera_shaking) == 0xcc);
static_assert(offsetof(damage_effect, sound) == 0x114);
static_assert(offsetof(damage_effect, breaking_effect_forward) == 0x194);
static_assert(offsetof(damage_effect, breaking_effect_outward) == 0x1ac);
static_assert(offsetof(damage_effect, side_effect) == 0x1c4);
static_assert(offsetof(damage_effect, instantaneous_acceleration) == 0x1f4);
static_assert(offsetof(damage_effect, material_modifiers) == 0x200);
static_assert(sizeof(damage_effect) == 672);

} // namespace blam::scn
