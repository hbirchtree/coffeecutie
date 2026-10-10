#pragma once

#include "blam_base_types.h"
#include "blam_effects.h"
#include "blam_reference.h"
#include "blam_shaders.h"
#include "blam_tag_ref.h"

#include <peripherals/enum/helpers.h>

/* Effects, particles and the things that glow: effe, part, pctl, cont, ligh,
 * mgs2, lens, pphy. Colors marked argb are stored a, r, g, b. */

namespace blam::scn {

using framebuffer_blending = shader::chicago::framebuffer_blending;

/*! How a sprite faces, for part */
enum class particle_orientation_t : u16
{
    screen_facing,
    parallel_to_direction,
    perpendicular_to_direction,
};

enum class particle_anchor_t : u16
{
    with_primary,
    with_screen_space,
    zsprite,
};

/*! The sprite shading shared by part, cont and pctl particle states */
struct particle_shader_t
{
    enum class flags_t : u16
    {
        none                      = 0x0,
        sort_bias                 = 0x1,
        nonlinear_tint            = 0x2,
        dont_overdraw_fp_weapon   = 0x4,
    };

    struct animation_t
    {
        shader::animation_src      source;
        shader::animation_function function;
        f32                        period;
        f32                        phase;
        f32                        scale;
    };

    u16                                shader_type;
    u16                                padding;
    flags_t                            flags;
    framebuffer_blending               blend_function;
    shader::chicago::framebuffer_fade_mode fade_mode;
    u16                                bitmap_flags; // 0x1 = unfiltered
    u32                                padding2[7];
    tagref_typed_t<tag_class_t::bitm>  secondary_bitmap;
    particle_anchor_t                  anchor;
    u16                                secondary_bitmap_flags;
    animation_t                        u_animation;
    animation_t                        v_animation;
    animation_t                        rotation_animation;
    Vecf2                              rotation_center;
    u32                                padding3;
    f32                                zsprite_radius_scale;
    u32                                padding4[5];
};

C_FLAGS(particle_shader_t::flags_t, u16)

static_assert(sizeof(particle_shader_t::animation_t) == 16);
static_assert(offsetof(particle_shader_t, secondary_bitmap) == 0x28);
static_assert(offsetof(particle_shader_t, u_animation) == 0x3c);
static_assert(offsetof(particle_shader_t, zsprite_radius_scale) == 0x78);
static_assert(sizeof(particle_shader_t) == 144);

/*! Mass, drag and gravity of a point, aka pphy */
struct point_physics
{
    enum class flags_t : u32
    {
        none                            = 0x0,
        flamethrower_particle_collision = 0x1,
        collides_with_structures        = 0x2,
        collides_with_water_surface     = 0x4,
        uses_simple_wind                = 0x8,
        uses_damped_wind                = 0x10,
        no_gravity                      = 0x20,
    };

    flags_t flags;
    f32     mass_scale;
    f32     water_gravity_scale;
    f32     air_gravity_scale;
    u32     padding[4];
    f32     density;
    f32     air_friction;
    f32     water_friction;
    f32     surface_friction;
    f32     elasticity;
    u32     padding2[3];
};

C_FLAGS(point_physics::flags_t, u32)

static_assert(offsetof(point_physics, density) == 0x20);
static_assert(sizeof(point_physics) == 64);

/*! A single sprite with its own lifetime, aka part */
struct particle
{
    enum class flags_t : u32
    {
        none                            = 0x0,
        can_animate_backwards           = 0x1,
        animation_stops_at_rest         = 0x2,
        animation_starts_on_random_frame = 0x4,
        animate_once_per_frame          = 0x8,
        dies_at_rest                    = 0x10,
        dies_on_contact_with_structure  = 0x20,
        tint_from_diffuse_texture       = 0x40,
        dies_on_contact_with_water      = 0x80,
        dies_on_contact_with_air        = 0x100,
        self_illuminated                = 0x200,
        random_horizontal_mirroring     = 0x400,
        random_vertical_mirroring       = 0x800,
    };

    flags_t                                             flags;
    tagref_typed_t<tag_class_t::bitm>                   bitmap;
    tagref_typed_t<tag_class_t::pphy>                   physics;
    tagref_typed_t<tag_class_t::foot>                   material_effects;
    u32                                                 padding;
    Vecf2                                               lifespan;
    f32                                                 fade_in_time;
    f32                                                 fade_out_time;
    tagref_typed_t<tag_class_t::snd, tag_class_t::effe> collision_effect;
    tagref_typed_t<tag_class_t::snd, tag_class_t::effe> death_effect;
    f32                                                 minimum_size;
    u32                                                 padding2[2];
    Vecf2                                               radius_animation;
    u32                                                 padding3;
    Vecf2                                               animation_rate;
    f32                                                 contact_deterioration;
    f32                                                 fade_start_size;
    f32                                                 fade_end_size;
    u32                                                 padding4;
    i16                                                 first_sequence_index;
    i16                                                 initial_sequence_count;
    i16                                                 looping_sequence_count;
    i16                                                 final_sequence_count;
    u32                                                 padding5[2];
    f32                                                 sprite_size;
    particle_orientation_t                              orientation;
    u16                                                 padding6;
    u32                                                 padding7[9];
    particle_shader_t                                   shader;
};

C_FLAGS(particle::flags_t, u32)

static_assert(offsetof(particle, lifespan) == 0x38);
static_assert(offsetof(particle, collision_effect) == 0x48);
static_assert(offsetof(particle, first_sequence_index) == 0x98);
static_assert(offsetof(particle, sprite_size) == 0xa8);
static_assert(offsetof(particle, shader) == 0xd4);
static_assert(sizeof(particle) == 356);

/*! Many particles moving as one, e.g. smoke, aka pctl */
struct particle_system
{
    enum class creation_physics_t : u16
    {
        default_,
        explosion,
        jet,
    };

    struct physics_constant_t
    {
        f32 k;
    };

    struct state_t
    {
        bl_string                     name;
        Vecf2                         duration_bounds;
        Vecf2                         transition_time_bounds;
        u32                           padding;
        f32                           scale_multiplier;
        f32                           animation_rate_multiplier;
        f32                           rotation_rate_multiplier;
        Vecf4                         color_multiplier; // argb
        f32                           radius_multiplier;
        f32                           minimum_particle_count;
        f32                           particle_creation_rate;
        u32                           padding2[21];
        creation_physics_t            particle_creation_physics;
        u16                           particle_update_physics;
        reference<physics_constant_t> physics_constants;
    };

    struct particle_state_t
    {
        bl_string                         name;
        Vecf2                             duration_bounds;
        Vecf2                             transition_time_bounds;
        tagref_typed_t<tag_class_t::bitm> bitmaps;
        i16                               sequence_index;
        u16                               padding;
        u32                               padding2;
        Vecf2                             scale;
        Vecf2                             animation_rate;
        Vecf2                             rotation_rate;
        Vecf4                             color_1; // argb
        Vecf4                             color_2; // argb
        f32                               radius_multiplier;
        tagref_typed_t<tag_class_t::pphy> point_physics;
        u32                               padding3[18];
        particle_shader_t                 shader;
        reference<physics_constant_t>     physics_constants;
    };

    struct type_t
    {
        enum class flags_t : u32
        {
            none                         = 0x0,
            type_states_loop             = 0x1,
            forward_backward             = 0x2,
            particle_states_loop         = 0x4,
            particle_forward_backward    = 0x8,
            particles_die_in_water       = 0x10,
            particles_die_in_air         = 0x20,
            particles_die_on_ground      = 0x40,
            rotational_sprites_sideways  = 0x80,
            disabled                     = 0x100,
            tint_by_effect_color         = 0x200,
            initial_count_scales         = 0x400,
            minimum_count_scales         = 0x800,
            creation_rate_scales         = 0x1000,
            scale_scales                 = 0x2000,
            animation_rate_scales        = 0x4000,
            rotation_rate_scales         = 0x8000,
            not_in_first_person          = 0x10000,
            not_in_third_person          = 0x20000,
        };

        bl_string                     name;
        flags_t                       flags;
        u16                           initial_particle_count;
        u16                           padding;
        u16                           complex_sprite_render_mode;
        u16                           padding2;
        f32                           radius;
        u32                           padding3[9];
        creation_physics_t            particle_creation_physics;
        u16                           padding4;
        u32                           physics_flags;
        reference<physics_constant_t> physics_constants;
        reference<state_t>            states;
        reference<particle_state_t>   particle_states;
    };

    u32                               padding[14];
    tagref_typed_t<tag_class_t::pphy> point_physics;
    u16                               system_update_physics;
    u16                               padding2;
    u32                               physics_flags;
    reference<physics_constant_t>     physics_constants;
    reference<type_t>                 particle_types;
};

C_FLAGS(particle_system::type_t::flags_t, u32)

static_assert(offsetof(particle_system::state_t, particle_creation_physics) == 0xb0);
static_assert(sizeof(particle_system::state_t) == 192);
static_assert(offsetof(particle_system::particle_state_t, point_physics) == 0x84);
static_assert(offsetof(particle_system::particle_state_t, shader) == 0xdc);
static_assert(sizeof(particle_system::particle_state_t) == 376);
static_assert(offsetof(particle_system::type_t, particle_creation_physics) == 0x54);
static_assert(sizeof(particle_system::type_t) == 128);
static_assert(offsetof(particle_system, point_physics) == 0x38);
static_assert(sizeof(particle_system) == 104);

/*! A ribbon left behind a moving marker, aka cont */
struct contrail
{
    enum class flags_t : u16
    {
        none                          = 0x0,
        first_point_unfaded           = 0x1,
        last_point_unfaded            = 0x2,
        points_start_pinned_to_media  = 0x4,
        points_start_pinned_to_ground = 0x8,
        points_always_pinned_to_media = 0x10,
        points_always_pinned_to_ground = 0x20,
        edge_effect_fades_slowly      = 0x40,
    };

    enum class render_type_t : u16
    {
        vertical_orientation,
        horizontal_orientation,
        media_mapped,
        ground_mapped,
        viewer_facing,
        double_marker_linked,
    };

    /*! What points look like as they age; each lasts `duration` */
    struct point_state_t
    {
        Vecf2                             duration;
        Vecf2                             transition_duration;
        tagref_typed_t<tag_class_t::pphy> physics;
        u32                               padding[8];
        f32                               width;
        Vecf4                             color_lower_bound; // argb
        Vecf4                             color_upper_bound; // argb
        u32                               scale_flags;
    };

    flags_t                           flags;
    u16                               scale_flags;
    f32                               point_generation_rate; // per second
    Vecf2                             point_velocity;
    f32                               point_velocity_cone_angle;
    f32                               inherited_velocity_fraction;
    render_type_t                     render_type;
    u16                               padding;
    f32                               texture_repeats_u;
    f32                               texture_repeats_v;
    f32                               texture_animation_u;
    f32                               texture_animation_v;
    f32                               animation_rate;
    tagref_typed_t<tag_class_t::bitm> bitmap;
    i16                               first_sequence_index;
    i16                               sequence_count;
    u32                               padding2[25];
    particle_shader_t                 shader;
    reference<point_state_t>          point_states;
};

C_FLAGS(contrail::flags_t, u16)

static_assert(sizeof(contrail::point_state_t) == 104);
static_assert(offsetof(contrail, bitmap) == 0x30);
static_assert(offsetof(contrail, shader) == 0xa8);
static_assert(offsetof(contrail, point_states) == 0x138);
static_assert(sizeof(contrail) == 324);

/*! A timeline of sounds, lights, objects and particles, aka effe */
struct effect
{
    enum class create_in_t : u16
    {
        any_environment,
        air_only,
        water_only,
        space_only,
    };

    enum class camera_mode_t : u16
    {
        independent_of_camera_mode,
        only_in_first_person,
        only_in_third_person,
        in_first_person_if_possible,
    };

    enum class distribution_t : u16
    {
        start,
        end,
        constant,
        buildup,
        falloff,
        buildup_and_falloff,
    };

    struct location_t
    {
        bl_string marker_name;
    };

    /*! Something created once: a sound, light, object, pctl, decal, damage */
    struct part_t
    {
        create_in_t create_in;
        u16         violence_mode;
        i16         location;
        u16         flags;
        u32         padding[3];
        u32         type_class; // cache only, the tag class of `type`
        tagref_typed_t<
            tag_class_t::jpt,
            tag_class_t::obje,
            tag_class_t::pctl,
            tag_class_t::snd,
            tag_class_t::deca,
            tag_class_t::ligh>
              type;
        u32   padding2[6];
        Vecf2 velocity_bounds;
        f32   velocity_cone_angle;
        Vecf2 angular_velocity_bounds;
        Vecf2 radius_modifier_bounds;
        u32   padding3;
        u32   a_scales_values;
        u32   b_scales_values;
    };

    /*! A burst of part particles */
    struct particle_t
    {
        enum class flags_t : u32
        {
            none                  = 0x0,
            stay_attached_to_marker = 0x1,
            random_initial_angle  = 0x2,
            tint_from_object_color = 0x4,
            interpolate_tint_as_hsv = 0x8,
            across_long_hue_path  = 0x10,
        };

        create_in_t                       create_in;
        u16                               violence_mode;
        camera_mode_t                     create;
        u16                               padding;
        i16                               location;
        u16                               padding2;
        Vecf2                             relative_direction; // yaw, pitch
        Vecf3                             relative_offset;
        Vecf3                             relative_direction_vector; // cache only
        u32                               padding3[10];
        tagref_typed_t<tag_class_t::part> particle_type;
        flags_t                           flags;
        distribution_t                    distribution_function;
        u16                               padding4;
        bl_point_t                        count;
        Vecf2                             distribution_radius;
        u32                               padding5[3];
        Vecf2                             velocity;
        f32                               velocity_cone_angle;
        Vecf2                             angular_velocity;
        u32                               padding6[2];
        Vecf2                             radius;
        u32                               padding7[2];
        Vecf4                             tint_lower_bound; // argb
        Vecf4                             tint_upper_bound; // argb
        u32                               padding8[4];
        u32                               a_scales_values;
        u32                               b_scales_values;
    };

    struct event_t
    {
        u32                   padding;
        f32                   skip_fraction;
        Vecf2                 delay_bounds;
        Vecf2                 duration_bounds;
        u32                   padding2[5];
        reference<part_t>     parts;
        reference<particle_t> particles;
    };

    enum class flags_t : u32
    {
        none                                = 0x0,
        deleted_when_attachment_deactivates = 0x1,
    };

    flags_t               flags;
    i16                   loop_start_event;
    i16                   loop_stop_event;
    f32                   maximum_damage_radius;
    u32                   padding[7];
    reference<location_t> locations;
    reference<event_t>    events;
};

C_FLAGS(effect::flags_t, u32)
C_FLAGS(effect::particle_t::flags_t, u32)

static_assert(sizeof(effect::location_t) == 32);
static_assert(offsetof(effect::part_t, type) == 0x18);
static_assert(offsetof(effect::part_t, velocity_bounds) == 0x40);
static_assert(sizeof(effect::part_t) == 104);
static_assert(offsetof(effect::particle_t, particle_type) == 0x54);
static_assert(offsetof(effect::particle_t, count) == 0x6c);
static_assert(offsetof(effect::particle_t, velocity) == 0x84);
static_assert(offsetof(effect::particle_t, radius) == 0xa0);
static_assert(offsetof(effect::particle_t, tint_lower_bound) == 0xb0);
static_assert(sizeof(effect::particle_t) == 232);
static_assert(offsetof(effect::event_t, parts) == 0x2c);
static_assert(sizeof(effect::event_t) == 68);
static_assert(offsetof(effect, locations) == 0x28);
static_assert(sizeof(effect) == 64);

/*! A dynamic light, aka ligh */
struct light
{
    enum class flags_t : u32
    {
        none                     = 0x0,
        dynamic                  = 0x1,
        no_specular              = 0x2,
        dont_light_own_object    = 0x4,
        supersize_in_first_person = 0x8,
        first_person_flashlight  = 0x10,
        dont_fade_active_camouflage = 0x20,
    };

    flags_t                           flags;
    f32                               radius;
    Vecf2                             radius_modifier;
    f32                               falloff_angle;
    f32                               cutoff_angle;
    f32                               lens_flare_only_radius;
    f32                               cos_falloff_angle;
    f32                               cos_cutoff_angle;
    f32                               specular_radius_multiplier;
    f32                               sin_cutoff_angle;
    u32                               padding[2];
    u32                               interpolation_flags;
    Vecf4                             color_lower_bound; // argb
    Vecf4                             color_upper_bound; // argb
    u32                               padding2[3];
    tagref_typed_t<tag_class_t::bitm> primary_cube_map;
    u16                               padding3;
    shader::animation_function        texture_animation_function;
    f32                               texture_animation_period;
    tagref_typed_t<tag_class_t::bitm> secondary_cube_map;
    u16                               padding4;
    shader::animation_function        yaw_function;
    f32                               yaw_period;
    u16                               padding5;
    shader::animation_function        roll_function;
    f32                               roll_period;
    u16                               padding6;
    shader::animation_function        pitch_function;
    f32                               pitch_period;
    u32                               padding7[2];
    tagref_typed_t<tag_class_t::lens> lens_flare;
    u32                               padding8[6];
    f32                               intensity;
    Vecf3                             color;
    u32                               padding9[4];
    f32                               duration; // seconds, 0 = forever
    u16                               padding10;
    function_type_t                   falloff_function;
    u32                               padding11[25];
};

C_FLAGS(light::flags_t, u32)

static_assert(offsetof(light, color_lower_bound) == 0x38);
static_assert(offsetof(light, primary_cube_map) == 0x64);
static_assert(offsetof(light, lens_flare) == 0xac);
static_assert(offsetof(light, intensity) == 0xd4);
static_assert(offsetof(light, duration) == 0xf4);
static_assert(sizeof(light) == 352);

/*! A glowing beam of sprites, e.g. a plasma bolt, aka mgs2 */
struct light_volume
{
    struct frame_t
    {
        u32   padding[4];
        f32   offset_from_marker;
        f32   offset_exponent;
        f32   length;
        u32   padding2[8];
        f32   radius_hither;
        f32   radius_yon;
        f32   radius_exponent;
        u32   padding3[8];
        Vecf4 tint_color_hither; // argb
        Vecf4 tint_color_yon;    // argb
        f32   tint_color_exponent;
        f32   brightness_exponent;
        u32   padding4[8];
    };

    bl_string                         attachment_marker;
    u16                               padding;
    u16                               flags;
    u32                               padding2[4];
    f32                               near_fade_distance;
    f32                               far_fade_distance;
    f32                               perpendicular_brightness_scale;
    f32                               parallel_brightness_scale;
    shader::animation_src             brightness_scale_source;
    u16                               padding3;
    u32                               padding4[5];
    tagref_typed_t<tag_class_t::bitm> map;
    i16                               sequence_index;
    i16                               count;
    u32                               padding5[18];
    shader::animation_src             frame_animation_source;
    u16                               padding6;
    u32                               padding7[25];
    reference<frame_t>                frames;
    u32                               padding8[8];
};

static_assert(offsetof(light_volume::frame_t, radius_hither) == 0x3c);
static_assert(offsetof(light_volume::frame_t, tint_color_hither) == 0x68);
static_assert(sizeof(light_volume::frame_t) == 176);
static_assert(offsetof(light_volume, near_fade_distance) == 0x34);
static_assert(offsetof(light_volume, map) == 0x5c);
static_assert(offsetof(light_volume, frame_animation_source) == 0xb8);
static_assert(offsetof(light_volume, frames) == 0x120);
static_assert(sizeof(light_volume) == 332);

/*! Screen-space glare from a bright point, aka lens */
struct lens_flare
{
    enum class flags_t : u16
    {
        none                   = 0x0,
        sun                    = 0x1,
        no_occlusion_test      = 0x2,
        only_in_first_person   = 0x4,
        only_in_third_person   = 0x8,
        fade_in_more_quickly   = 0x10,
        fade_out_more_quickly  = 0x20,
        scale_by_marker        = 0x40,
    };

    enum class scaled_by_t : u16
    {
        none,
        rotation,
        rotation_and_strafing,
        distance_from_center,
    };

    struct reflection_t
    {
        enum class flags_t : u16
        {
            none                            = 0x0,
            align_rotation_with_screen_center = 0x1,
            radius_not_scaled_by_distance   = 0x2,
            radius_scaled_by_occlusion      = 0x4,
            occluded_by_solid_objects       = 0x8,
        };

        flags_t                    flags;
        u16                        padding;
        i16                        bitmap_index;
        u16                        padding2;
        u32                        padding3[5];
        f32                        position; // along the line through center
        f32                        rotation_offset;
        u32                        padding4;
        Vecf2                      radius;
        scaled_by_t                radius_scaled_by;
        u16                        padding5;
        Vecf2                      brightness;
        scaled_by_t                brightness_scaled_by;
        u16                        padding6;
        Vecf4                      tint_color;        // argb
        Vecf4                      color_lower_bound; // argb
        Vecf4                      color_upper_bound; // argb
        u16                        more_flags;
        shader::animation_function animation_function;
        f32                        animation_period;
        f32                        animation_phase;
        u32                        padding7;
    };

    f32                               falloff_angle;
    f32                               cutoff_angle;
    f32                               cos_falloff_angle;
    f32                               cos_cutoff_angle;
    f32                               occlusion_radius;
    u16                               occlusion_offset_direction;
    u16                               padding;
    f32                               near_fade_distance;
    f32                               far_fade_distance;
    tagref_typed_t<tag_class_t::bitm> bitmap;
    flags_t                           flags;
    u16                               padding2;
    u32                               padding3[19];
    u16                               rotation_function;
    u16                               padding4;
    f32                               rotation_function_scale;
    u32                               padding5[6];
    f32                               horizontal_scale;
    f32                               vertical_scale;
    u32                               padding6[7];
    reference<reflection_t>           reflections;
    u32                               padding7[8];
};

C_FLAGS(lens_flare::flags_t, u16)
C_FLAGS(lens_flare::reflection_t::flags_t, u16)

static_assert(offsetof(lens_flare::reflection_t, position) == 0x1c);
static_assert(offsetof(lens_flare::reflection_t, tint_color) == 0x40);
static_assert(offsetof(lens_flare::reflection_t, animation_function) == 0x72);
static_assert(sizeof(lens_flare::reflection_t) == 128);
static_assert(offsetof(lens_flare, bitmap) == 0x20);
static_assert(offsetof(lens_flare, rotation_function) == 0x80);
static_assert(offsetof(lens_flare, horizontal_scale) == 0xa0);
static_assert(offsetof(lens_flare, reflections) == 0xc4);
static_assert(sizeof(lens_flare) == 240);

} // namespace blam::scn
