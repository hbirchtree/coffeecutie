#pragma once

#include "../volta/blam_base_types.h"

#include <peripherals/stl/any_of.h>

namespace blam::dimeter {

/*!
 * Halo 2 cache files all report version 8 in the file header; the Xbox and
 * Vista releases are distinguished by their (incompatible) header layouts.
 * Structure layouts are cross-referenced against the Assembly project's
 * H2X_Layouts_Core.xml / H2V_Layouts_Core.xml and its SecondGen loader.
 */
struct xbox_version_t
{
    static constexpr auto version_v = version_t::halo2;
};

struct vista_version_t
{
    static constexpr auto version_v = version_t::halo2;
};

template<typename T>
concept is_game_version =
    stl_types::is_any_of<T, xbox_version_t, vista_version_t>;

constexpr xbox_version_t  xbox_version;
constexpr vista_version_t vista_version;

} // namespace blam::dimeter
