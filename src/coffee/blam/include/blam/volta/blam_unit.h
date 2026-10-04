#pragma once

#include "blam_base_types.h"
#include "blam_domain_ptrs.h"
#include "blam_mod2.h"
#include "blam_tag_ref.h"

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
};

struct object
{
    object_type                       type;
    object_flags                      flags;
    f32                               bound_radius;
    Vecf3                             bound_offset;
    Vecf3                             origin_offset;
    f32                               acceleration_scale;
    u32                               padding_;
    tagref_typed_t<tag_class_t::mod2> model;
    tagref_typed_t<tag_class_t::antr> anim_graph;
    u32                               padding2[10];
    tagref_typed_t<tag_class_t::coll> collider;
    tagref_typed_t<tag_class_t::pphy> physics;
    tagref_typed_t<tag_class_t::shdr> shader;
    tagref_typed_t<tag_class_t::effe> creation_effect;
    f32                               render_bound_radius;

    struct export_functions_t
    {
        enum class input_t : u32
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
        scenario_ptr<scn::hud_msg> hud_msg;
        mod2::model_ptr<tagref_t>  shader_perm;
    } export_;

    u32 padding[30];

    struct attachment_t
    {
        tagref_t                      type;
        mod2::model_ptr<mod2::marker> marker; // pointing into model markers
        u32                           primary_scale;
        u32                           second_scale;
        u32                           change_color;
    };
    reference<attachment_t> attachments;

    reference<tagref_t> widgets; // antenna, flag, glow, light_volume, lighting

    struct function_t
    {
    };
    reference<function_t> functions;

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
    reference<change_color_t> change_colors;

    struct predicted_resource_t
    {
    };
    reference<predicted_resource_t> predicted_resources;
};

static_assert(offsetof(object, attachments) == 320); // 0x140
static_assert(sizeof(object) == 380);

struct item : object
{
};

struct unit : object
{
    u32 padding[93];
};

static_assert(sizeof(unit) == 752);

/*! Camera and collision sizes, which sit past the object and unit blocks
 *  (only partly decoded above), so they are read from the tag directly */
// TODO: Decode object, unit and biped in full (380, 752 and 1268 bytes in
// the tag) so biped's fields can be read as members, then drop this
struct biped_dimensions
{
    static constexpr u32 tag_offset = 0x400;

    f32 standing_camera_height;
    f32 crouching_camera_height;
    f32 crouch_transition_time;
    u32 padding[6];
    f32 standing_collision_height;
    f32 crouching_collision_height;
    f32 collision_radius;
};

static_assert(offsetof(biped_dimensions, standing_collision_height) == 0x24);
static_assert(offsetof(biped_dimensions, collision_radius) == 0x2c);

struct biped : unit
{
    u32 padding[129];
    biped_dimensions const& dimensions() const
    {
        return *reinterpret_cast<biped_dimensions const*>(
            reinterpret_cast<char const*>(this) + biped_dimensions::tag_offset);
    }
};

static_assert(sizeof(biped) == 1268);

struct vehicle : unit
{
};

struct scenery : unit
{
};

struct weapon : item
{
};

struct equipment : item
{
};

}
