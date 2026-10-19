#pragma once

#include "blam_reference.h"
#include "blam_tag_ref.h"

namespace blam::globals {

using typing::vector_types::Vecf2;

/*! The unit a player spawns as, for multiplayer. Fields past `unit` are only
 *  named as far as they have been checked against real maps. */
struct multiplayer_information
{
    tagref_typed_t<tag_class_t::weap> flag;
    tagref_typed_t<tag_class_t::bipd> unit;
    reference<u32>                    vehicles; /* not decoded */
};

static_assert(offsetof(multiplayer_information, unit) == 0x10);
static_assert(offsetof(multiplayer_information, vehicles) == 0x20);

/*! The unit a player spawns as, for singleplayer. TODO: Decode the movement
 *  and stun fields between */
struct player_information
{
    tagref_typed_t<tag_class_t::bipd> unit;
    u32                               padding[(0xB8 - 0x10) / 4];
    tagref_typed_t<tag_class_t::effe> coop_respawn_effect;
};

static_assert(offsetof(player_information, coop_respawn_effect) == 0xB8);

/*! What a player sees of themselves in first person. TODO: Decode the rest */
struct first_person_interface
{
    tagref_typed_t<tag_class_t::mod2> hands;
};

/*! A grenade type players carry and throw */
struct grenade
{
    i16                               maximum_count;
    i16                               multiplayer_spawn_default;
    tagref_typed_t<tag_class_t::effe> throwing_effect;
    tagref_typed_t<tag_class_t::grhi> hud_interface;
    tagref_typed_t<tag_class_t::eqip> equipment;
    tagref_typed_t<tag_class_t::proj> projectile;
};

static_assert(sizeof(grenade) == 68);

/*! Bitmaps and definitions the interface shares */
struct interface_bitmaps
{
    tagref_typed_t<tag_class_t::font> font_system;
    tagref_typed_t<tag_class_t::font> font_terminal;
    tagref_t                          screen_color_table;
    tagref_t                          hud_color_table;
    tagref_t                          editor_color_table;
    tagref_t                          dialog_color_table;
    tagref_typed_t<tag_class_t::hudg> hud_globals;
    tagref_typed_t<tag_class_t::bitm> motion_sensor_sweep_bitmap;
    tagref_typed_t<tag_class_t::bitm> motion_sensor_sweep_bitmap_mask;
    tagref_typed_t<tag_class_t::bitm> multiplayer_hud_bitmap;
    tagref_t                          localization;
    tagref_typed_t<tag_class_t::hud> hud_digits_definition;
    tagref_typed_t<tag_class_t::bitm> motion_sensor_blip_bitmap;
    tagref_typed_t<tag_class_t::bitm> interface_goo_map1;
    tagref_typed_t<tag_class_t::bitm> interface_goo_map2;
    tagref_typed_t<tag_class_t::bitm> interface_goo_map3;
    u32                               padding[12];
};

static_assert(sizeof(interface_bitmaps) == 304);

/*! Damage the engine deals itself: falls, and units struck by vehicles */
struct falling_damage
{
    u32                              padding[2];
    Vecf2                            harmful_falling_distance;
    tagref_typed_t<tag_class_t::jpt> falling;
    u32                              padding2[2];
    f32                              maximum_falling_distance;
    tagref_typed_t<tag_class_t::jpt> distance;
    tagref_typed_t<tag_class_t::jpt> vehicle_environment_collision;
    tagref_typed_t<tag_class_t::jpt> vehicle_killed_unit;
    tagref_typed_t<tag_class_t::jpt> vehicle_collision;
    tagref_typed_t<tag_class_t::jpt> flaming_death;
    u32                              padding3[7];
};

static_assert(offsetof(falling_damage, falling) == 0x10);
static_assert(offsetof(falling_damage, vehicle_collision) == 0x5c);
static_assert(sizeof(falling_damage) == 152);

/*! Only the blocks that name the player's unit and hands are decoded; everything
 *  ahead of them is skipped by offset, because the rest of the globals tag has
 *  no description we can check a layout against.
 *
 *  Campaign maps carry an empty `multiplayer` block, so `player` is the one
 *  that is always populated. */
struct globals
{
    u32                                padding[0x128 / 4];
    reference<grenade>                 grenades;
    reference<u32>                     rasterizer_data; /* not decoded */
    reference<interface_bitmaps>       interface;
    reference<u32>                     weapon_list; /* not decoded */
    reference<u32>                     cheat_powerups; /* not decoded */
    reference<multiplayer_information> multiplayer;
    reference<player_information>      player;
    reference<first_person_interface>  first_person;
    reference<falling_damage>          falling;
};

static_assert(offsetof(globals, grenades) == 0x128);
static_assert(offsetof(globals, interface) == 0x140);
static_assert(offsetof(globals, multiplayer) == 0x164);
static_assert(offsetof(globals, player) == 0x170);
static_assert(offsetof(globals, first_person) == 0x17c);
static_assert(offsetof(globals, falling) == 0x188);

} // namespace blam::globals
