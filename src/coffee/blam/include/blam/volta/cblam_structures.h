#pragma once

#include <coffee/core/libc_types.h>
#include <coffee/core/types/pixel_components.h>
#include <coffee/core/types/pixel_format.h>
#include <coffee/core/types/point.h>
#include <coffee/core/types/rgba.h>
#include <coffee/core/types/size.h>
#include <peripherals/error/result.h>
#include <peripherals/semantic/chunk.h>
#include <peripherals/stl/any_of.h>
#include <peripherals/stl/functional_types.h>
#include <peripherals/typing/enum/graphics/texture_types.h>
#include <peripherals/typing/vectors/vector_types.h>

#include <codecvt>
#include <locale>

#include "blam_tag_classes.h"
#include "cblam_base_types.h"

namespace blam {

using typing::pixels::BitFmt;
using typing::pixels::CompFlags;
using typing::pixels::PixFmt;
using typing::vector_types::Quatf;
using TexType = typing::graphics::TexType;

enum class map_load_error
{
    no_error,

    map_file_too_small,
    not_a_map,
    decompression_error,

    incompatible_map_version_expected_pc,
    incompatible_map_version_expected_xbox,
    incompatible_map_version_expected_custom_edition,

    incompatible_endianness,
};

/*!
 * \brief Function pointers for blam bitmap processing, raw function pointer is
 * much faster than Function
 */
using BitmProcess = u32 (*)(u32, u16, byte_t);

using namespace std::string_view_literals;

constexpr std::string_view header_head = "daeh"sv; /*!< Header of file header*/
constexpr std::string_view header_foot = "toof"sv; /*!< Footer of file header*/

constexpr i32 blam_num_map_names = 28; /*!< Number of recognizable map names*/
using map_pair = std::pair<std::string_view, std::string_view>;
constexpr std::array<map_pair, 28> map_names = {{
    // Single player maps
    {"a10"sv, "Pillar of Autumn"sv},
    {"a30"sv, "Halo"sv},
    {"a50"sv, "Truth and Reconciliation"sv},
    {"b30"sv, "Silent Cartographer"sv},
    {"b40"sv, "Assault on the Control Room"sv},
    {"c10"sv, "343 Guilty Spark"sv},
    {"c20"sv, "The Library"sv},
    {"c40"sv, "Two Betrayals"sv},
    {"d20"sv, "Keyes"sv},
    {"d40"sv, "The Maw"sv},

    // Multi-player maps
    {"beavercreek"sv, "Beaver Creek"sv},
    {"bloodgulch"sv, "Blood Gulch"sv},
    {"boardingaction"sv, "Boarding Action"sv},
    {"carousel"sv, "Derelict"sv},
    {"chillout"sv, "Chill Out"sv},
    {"deathisland"sv, "Death Island"sv},
    {"damnation"sv, "Damnation"sv},
    {"dangercanyon"sv, "Danger Canyon"sv},
    {"gephyrophobia"sv, "Gephyrophobia"sv},
    {"hangemhigh"sv, "Hang 'em High"sv},
    {"icefields"sv, "Ice Fields"sv},
    {"longest"sv, "Longest"sv},
    {"prisoner"sv, "Prisoner"sv},
    {"putput"sv, "Chiron TL34"sv},
    {"ratrace"sv, "Rat Race"sv},
    {"sidewinder"sv, "Sidewinder"sv},
    {"timberland"sv, "Timberland"sv},
    {"wizard"sv, "Wizard"sv},
}}; /*!< A mapping of map names which this library can recognize. These are
      the stock maps.*/

} // namespace blam
