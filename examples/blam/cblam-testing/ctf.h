#pragma once

#include "blam_files.h"
#include "selected_version.h"

#include <blam/volta/blam_globals.h>
#include <coffee/components/entity_container.h>

/*! matg's capture-the-flag flag, a weapon; null without one */
inline blam::tag_t const* flag_weapon(BlamFiles<halo_version> const& files)
{
    auto const&                        magic = files.container.magic;
    blam::tag_index_view<halo_version> index(files.container);
    auto                               globals_tag = index.tag_of("globals\\globals");
    if(!globals_tag.has_value())
        return nullptr;
    auto globals = (*globals_tag)->template data<blam::globals::globals>(magic);
    if(!globals.has_value())
        return nullptr;
    auto mp = globals.value()->multiplayer.data(magic);
    if(!mp.has_value() || mp.value().empty() || !mp.value()[0].flag.valid())
        return nullptr;
    auto it = index.find(mp.value()[0].flag);
    return it != index.end() ? &*it : nullptr;
}

/*! The flags of capture the flag, at each team's base: the game type's
 *  rules give them out and send them back */
void alloc_ctf_flags(compo::EntityContainer& e);
