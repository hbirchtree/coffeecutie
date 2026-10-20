#pragma once

#include "structures.h"

namespace blam::elegy {

/*! Which object a placement uses, by index into its palette */
struct palette_entry_t
{
    tag_reference<>         object;
    std::array<u32, 8>    unknown_0;
};

static_assert(sizeof(palette_entry_t) == 0x30);

enum class placement_flags_t : u32
{
    none              = 0x0,
    not_automatically = 0x1, /*!< Created by script, not at load */
    not_on_easy       = 0x2,
    not_on_normal     = 0x4,
    not_on_hard       = 0x8,
    never_placed       = 0x40,
    create_at_rest     = 0x100,
    store_orientations = 0x200, /*!< Posed by node_orientations */
};
C_FLAGS(placement_flags_t, u32);

/*!
 * \brief A posed object's nodes, relative to their parents. The flags are two
 * node bit vectors back to back, rotations then translations; the shorts
 * follow node by node, four for a rotation (i, j, k, w over 32767), three
 * for a translation.
 */
struct node_orientations_t
{
    be<i16>            node_count;
    be<i16>            unknown_0;
    tag_block<u8>      node_flags;
    tag_block<be<i16>> orientations;
};

static_assert(sizeof(node_orientations_t) == 0x1C);

/*!
 * \brief Head every object placement in the scenario shares (scenery,
 * machines, crates, ...). Rotation is yaw/pitch/roll in radians.
 */
struct object_placement_t
{
    block_index<palette_entry_t> palette;
    be<i16>                      name;
    be<placement_flags_t>        placement_flags;
    be<Vecf3>                    position;
    be<Vecf3>                    rotation;
    be<f32>                      scale;
    tag_block<node_orientations_t> node_orientations;
    be<u16>                      transform_flags;
    be<u16>                      bsp_flags; /*!< One bit per structure BSP, none = all */
    string_id                    light_airprobe;
    datum_index                  unique_id;
    be<i16>                      origin_bsp;
    u8                           type;
    u8                           source;
    u8                           bsp_policy;
    u8                           unknown_0;
    be<i16>                      editor_folder;
    u16                          unknown_1;
    be<i16>                      parent_name;
    string_id                    parent_marker;
    string_id                    connection_marker;
    be<u16>                      attach_bsp_flags;
    u16                          unknown_2;
    string_id                    variant;
    be<u32>                      active_change_colors;
    std::array<be<u32>, 4>       change_colors;

    /*! Whether the level creates it when it loads */
    inline bool placed_at_load() const
    {
        return !stl_types::any_flag_of(
            placement_flags.value(),
            placement_flags_t::not_automatically | placement_flags_t::never_placed);
    }

    /*! Whether it exists in structure BSP `bsp`; no flags means all */
    inline bool in_bsp(i16 bsp) const
    {
        return bsp >= 0 && bsp < 16 && in_bsps(1u << bsp);
    }

    /*! Whether it exists in any of the structure BSPs in `mask` */
    inline bool in_bsps(u32 mask) const
    {
        u16 const flags = bsp_flags;
        return !flags || (flags & mask);
    }

    /*! Scale, with 0 read as 1 */
    inline f32 scale_or_one() const
    {
        f32 const s = scale;
        return s > 0.f ? s : 1.f;
    }

    /*! Yaw, pitch and roll: turns about +Z, then +Y, then +X */
    Quatf orientation() const;

    /*! Position, orientation and scale */
    Matf4 transform() const;
};

static_assert(offsetof(object_placement_t, rotation) == 0x14);
static_assert(offsetof(object_placement_t, bsp_flags) == 0x32);
static_assert(offsetof(object_placement_t, origin_bsp) == 0x3C);
static_assert(offsetof(object_placement_t, variant) == 0x54);
static_assert(sizeof(object_placement_t) == 0x6C);

struct scenery_placement_t
{
    object_placement_t   object;
    std::array<u8, 0x48> unknown_0;
};

static_assert(sizeof(scenery_placement_t) == 0xB4);

/*! Placements of other object types: the same head up to the variant, then
 * their own fields. Machines diverge after 0x54. */
template<size_t Size>
struct placement_t
{
    object_placement_t                object;
    std::array<u8, Size - sizeof(object_placement_t)> unknown_0;
};

using biped_placement_t     = placement_t<0x74>;
using vehicle_placement_t   = placement_t<0xA8>;
using equipment_placement_t = placement_t<0x8C>;
using weapon_placement_t    = placement_t<0xA8>;
using machine_placement_t   = placement_t<0x70>;
using crate_placement_t     = placement_t<0xB0>;

struct starting_location_t
{
    be<Vecf3> position;
    be<Vecf2> facing; /*!< yaw, pitch in radians */
    be<i16>   bsp;
    be<i16>   player_type;
};

static_assert(sizeof(starting_location_t) == 0x18);

/*! What a player spawns with */
struct starting_profile_t
{
    std::array<char, 0x20> name;
    be<f32>                health_damage;
    be<f32>                shield_damage;
    tag_reference<>          primary_weapon;
    be<i16>                primary_rounds_loaded;
    be<i16>                primary_rounds_total;
    tag_reference<>          secondary_weapon;
    be<i16>                secondary_rounds_loaded;
    be<i16>                secondary_rounds_total;
    std::array<u8, 4>      grenade_counts; /*!< frag, plasma, spike, firebomb */
};

static_assert(sizeof(starting_profile_t) == 0x54);

enum class multiplayer_object_flags_t : u16
{
    none                 = 0x0,
    editor_only          = 0x1, /*!< Forge and spawn helpers */
    initial_player_spawn = 0x2,
    fixed_boundary       = 0x4,
};
C_FLAGS(multiplayer_object_flags_t, u16);

struct multiplayer_object_t
{
    be<u16>                        game_engines;
    u8                             type;
    u8                             teleporter_passability;
    be<multiplayer_object_flags_t> flags;
    std::array<u8, 0xBE>           unknown_0;
};

static_assert(sizeof(multiplayer_object_t) == 0xC4);

/*! Something attached at a marker of an object's model: a light, an
 * effect, a looping sound */
struct object_attachment_t
{
    tag_reference<> type;
    string_id     marker;
    be<i16>       change_color;
    be<i16>       unknown_0;
    string_id     primary_scale;
    string_id     secondary_scale;
};

static_assert(sizeof(object_attachment_t) == 0x20);

/*! object ("obje") head, shared by scen, bloc, mach, ...: the model and
 * how multiplayer treats it */
/*! A starting colour of a change colour: picked by weight, between the bounds */
struct change_color_permutation_t
{
    be<f32>   weight;
    be<Vecf3> lower_bound;
    be<Vecf3> upper_bound;
    string_id variant; /*!< Empty: any model variant */
};

static_assert(sizeof(change_color_permutation_t) == 0x20);

/*! One of an object's change colours (primary, secondary, ...) */
struct change_color_t
{
    tag_block<change_color_permutation_t> initial_permutations;
    tag_block<std::array<u8, 0x28>>       functions;
};

static_assert(sizeof(change_color_t) == 0x18);

struct object_t
{
    std::array<u8, 0x34>            unknown_0;
    tag_reference<>                   model;
    std::array<u8, 0x78>            unknown_1;
    tag_block<object_attachment_t>  attachments;
    std::array<u8, 0xC>             unknown_2;
    tag_block<change_color_t>       change_colors;
    std::array<u8, 0xC>             unknown_3;
    tag_block<multiplayer_object_t> multiplayer;

    inline bool editor_only(tag_magic const& magic) const
    {
        for(auto const& mp :
            multiplayer.data(magic).value_or(Span<multiplayer_object_t const>()))
            if(enum_helpers::feval(
                   mp.flags.value(), multiplayer_object_flags_t::editor_only))
                return true;
        return false;
    }
};

static_assert(sizeof(object_t) == 0xF8);
static_assert(offsetof(object_t, model) == 0x34);
static_assert(offsetof(object_t, attachments) == 0xBC);
static_assert(offsetof(object_t, change_colors) == 0xD4);
static_assert(offsetof(object_t, multiplayer) == 0xEC);

/*! A weapon's first-person model and animations, per player representation */
struct weapon_first_person_t
{
    tag_reference<> model;      /*!< mode */
    tag_reference<> animations; /*!< jmad */
};

static_assert(sizeof(weapon_first_person_t) == 0x20);

/*! weapon ("weap") */
struct weapon_t
{
    static constexpr tag_class_t tag_class = tag_class_t::weap;

    object_t                         object;
    std::array<u8, 0x324>            unknown_0;
    tag_block<weapon_first_person_t> first_person;
    std::array<u8, 0xB0>             unknown_1;
    be<Vecf3>                        first_person_offset;
};

static_assert(offsetof(weapon_t, first_person) == 0x41C);
static_assert(offsetof(weapon_t, first_person_offset) == 0x4D8);

enum class light_type_t : u16
{
    sphere  = 0,
    frustum = 1,
};

/*! A function a light's colour or intensity follows, as an input (or a
 * constant) maps through it; see function_t */
struct light_function_t
{
    string_id      input;
    string_id      range;
    be<u16>        output_modifier;
    be<i16>        unknown_0;
    string_id      output_modifier_input;
    data_reference function;
};

static_assert(sizeof(light_function_t) == 0x24);

/*! light ("ligh"): a dynamic light, sphere or frustum */
struct light_t
{
    static constexpr tag_class_t tag_class = tag_class_t::ligh;

    be<u32>          flags;
    be<light_type_t> type;
    be<i16>          unknown_0;
    be<f32>          maximum_distance;
    be<f32>          frustum_near_width;
    be<f32>          frustum_height_scale;
    be<f32>          frustum_field_of_view;
    light_function_t color;
    light_function_t intensity;
    tag_reference<>    gel;
    be<f32>          distance_diffusion;
    be<f32>          angular_smoothness;
    be<f32>          percent_spherical;
    be<f32>          destroy_after;
    std::array<u8, 4> priority;
    tag_reference<>    lens_flare;
};

static_assert(offsetof(light_t, color) == 0x18);
static_assert(offsetof(light_t, intensity) == 0x3C);
static_assert(offsetof(light_t, gel) == 0x60);
static_assert(offsetof(light_t, lens_flare) == 0x84);

/*! model ("hlmt"): the render model and its LOD */
struct model_t
{
    static constexpr tag_class_t tag_class = tag_class_t::hlmt;

    tag_reference<>        render_model;
    tag_reference<>        collision_model;
    tag_reference<>        animation;
    tag_reference<>        physics_model;
    std::array<u8, 0x14> unknown_0;
    tag_reference<>        lod_render_model;
};

static_assert(offsetof(model_t, lod_render_model) == 0x54);

} // namespace blam::elegy
