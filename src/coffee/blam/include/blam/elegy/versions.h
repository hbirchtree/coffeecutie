#pragma once

#include "../volta/blam_base_types.h"

#include <peripherals/stl/any_of.h>

#include <bit>

namespace blam::elegy {

/*!
 * Halo 3 (Xbox 360) cache files report version 11. Layouts are
 * cross-referenced against the Assembly project's H3_Layouts_*.xml and its
 * Halo3 plugins, then checked against retail 11855 maps.
 */
constexpr i32 halo3_version = 11;

struct xbox360_version_t
{
    static constexpr i32 version_v = halo3_version;
    static constexpr auto endian   = std::endian::big;
};

template<typename T>
concept is_game_version = stl_types::is_any_of<T, xbox360_version_t>;

constexpr xbox360_version_t xbox360_version;

} // namespace blam::elegy
