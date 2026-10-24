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

/*! Puts the local players' cameras at `position` facing `yaw`, flying */
void place_local_cameras(compo::EntityContainer& container, Vecf3 const& position, f32 yaw);

} // namespace world
