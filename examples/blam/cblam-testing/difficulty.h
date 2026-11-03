#pragma once

#include "blam_files.h"
#include "selected_version.h"

#include <blam/volta/blam_globals.h>

/* The globals' difficulty block, null without one */
inline blam::globals::difficulty const* difficulty_block(BlamFiles<halo_version> const& files)
{
    blam::tag_index_view<halo_version> index(files.container);
    auto globals_tag = index.tag_of("globals\\globals");
    if(!globals_tag.has_value())
        return nullptr;
    auto globals = (*globals_tag)->template data<blam::globals::globals>(files.container.magic);
    if(!globals.has_value())
        return nullptr;
    auto levels = globals.value()->difficulty_levels.data(files.container.magic);
    return levels.has_value() && !levels.value().empty() ? &levels.value()[0] : nullptr;
}

/* A row's scale at a level (easy 0 to legendary 3), 1 without a block */
inline libc_types::f32 difficulty_scale(
    blam::globals::difficulty const* block,
    libc_types::f32 const (blam::globals::difficulty::*row)[4],
    libc_types::u16 level)
{
    return block ? (block->*row)[std::min<libc_types::u16>(level, 3)] : 1.f;
}
