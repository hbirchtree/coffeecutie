#pragma once

#include "blam_reference.h"
#include "blam_tag_ref.h"

namespace blam::globals {

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

/*! The unit a player spawns as, for singleplayer. */
struct player_information
{
    tagref_typed_t<tag_class_t::bipd> unit;
};

/*! Only the two blocks that name the player's unit are decoded; everything
 *  ahead of them is skipped by offset, because the rest of the globals tag has
 *  no description we can check a layout against.
 *
 *  Campaign maps carry an empty `multiplayer` block, so `player` is the one
 *  that is always populated. */
struct globals
{
    u32                                padding[0x164 / 4];
    reference<multiplayer_information> multiplayer;
    reference<player_information>      player;
};

static_assert(offsetof(globals, multiplayer) == 0x164);
static_assert(offsetof(globals, player) == 0x170);

} // namespace blam::globals
