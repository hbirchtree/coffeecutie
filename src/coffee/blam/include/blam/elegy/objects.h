#pragma once

#include "structures.h"

namespace blam::elegy {

/*! Which object a placement uses, by index into its palette */
struct palette_entry_t
{
    tag_reference         object;
    std::array<u32, 8>    unknown_0;
};

static_assert(sizeof(palette_entry_t) == 0x30);

/*!
 * \brief Head every object placement in the scenario shares (scenery,
 * machines, crates, ...). Rotation is yaw/pitch/roll in radians.
 */
struct object_placement_t
{
    block_index<palette_entry_t> palette;
    be<i16>                      name;
    be<u32>                      placement_flags;
    be<Vecf3>                    position;
    be<Vecf3>                    rotation;
    be<f32>                      scale;
    tag_block<std::array<u8, 0x1C>> node_orientations;
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

struct starting_location_t
{
    be<Vecf3> position;
    be<Vecf2> facing; /*!< yaw, pitch in radians */
    be<i16>   bsp;
    be<i16>   player_type;
};

static_assert(sizeof(starting_location_t) == 0x18);

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

/*! object ("obje") head, shared by scen, bloc, mach, ...: the model and
 * how multiplayer treats it */
struct object_t
{
    std::array<u8, 0x34>            unknown_0;
    tag_reference                   model;
    std::array<u8, 0xA8>            unknown_1;
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

static_assert(offsetof(object_t, model) == 0x34);
static_assert(offsetof(object_t, multiplayer) == 0xEC);

/*! model ("hlmt"): the render model and its LOD */
struct model_t
{
    tag_reference        render_model;
    tag_reference        collision_model;
    tag_reference        animation;
    tag_reference        physics_model;
    std::array<u8, 0x14> unknown_0;
    tag_reference        lod_render_model;
};

static_assert(offsetof(model_t, lod_render_model) == 0x54);

} // namespace blam::elegy
