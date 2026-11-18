#pragma once

#include "loading/blam_files.h"
#include "selected_version.h"

#include <coffee/components/entity_container.h>
#include <peripherals/typing/vectors/vector_types.h>

/*! Whether going from the eye at `before` to the eye at `after` is a hop
 *  through one of the map's teleporters: off a teleport_from flag onto a
 *  teleport_to flag of the same channel */
bool teleporter_hop(
    BlamFiles<halo_version> const&      files,
    typing::vector_types::Vecf3 const& before,
    typing::vector_types::Vecf3 const& after);

/*! Multiplayer teleporters: a player who steps onto a teleport_from flag
 *  comes out at a teleport_to flag of the same channel */
void alloc_teleporters(compo::EntityContainer& e);
