#pragma once

#include "bytecode_v1.h"
#include "bytecode_v2.h"
#include "cblam_structures.h"

namespace blam {
namespace hsc {

namespace bc {

enum class cheats : i16
{
    cheat_all_powerups,
    cheat_all_weapons,
    cheat_spawn_warthog,
    cheat_all_vehicles,
    cheat_teleport_to_camera,
    cheat_active_camouflage,
    cheat_active_camouflage_local_player,
    cheats_load,
};

} // namespace bc

using xbox_opcode_t = bc::v1;
using pc_opcode_t   = bc::v2;

} // namespace hsc
} // namespace blam
