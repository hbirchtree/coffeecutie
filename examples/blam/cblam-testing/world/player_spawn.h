#pragma once

#include "../components.h"

namespace world {

/*! The Halo 1 loader creates the seats from the scenario; a Halo 2 or 3 map
 * creates the local player itself. Returns it when it was created */
PlayerInfo* ensure_local_player(compo::EntityContainer& container);

/*! Whether `info` is this machine's player in `seat` */
inline bool is_local_seat(PlayerInfo const& info, u32 seat = 0)
{
    return info.seat_idx == seat && !info.is_remote();
}

/*! A Halo 1 map after another game's starts its local players as Halo 1
 * does, flying, rather than as the last map left them */
void reset_players_between_games(compo::EntityContainer& container);

/*! Puts the local players' cameras (their eyes) at `position` facing `yaw`.
 * With a `unit` they walk as it does, on whatever ground the map gives
 * Physics; without one they fly */
void place_local_cameras(
    compo::EntityContainer&           container,
    Vecf3 const&                      position,
    f32                               yaw,
    PlayerInfo::biped_shape_t const* unit = nullptr);

/*! Hands the local players the loaded game's weapons, in hand then slung,
 * and nothing else */
void arm_local_players(
    compo::EntityContainer& container, std::array<BipedEquipment::weapon_t, 2> const& loadout);

} // namespace world
