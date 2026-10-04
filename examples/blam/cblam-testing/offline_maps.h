#pragma once

#include "selected_version.h"

#include <optional>
#include <string>
#include <string_view>
#include <vector>

/* Maps put in the browser's IndexedDB by BlamMapUpload
 * (examples/blam/map-upload). They live in the Emscripten Fetch cache
 * (database emscripten_filesystem, object store FILES), keyed by paths like
 * /pc/bloodgulch.map, which is where emscripten_fetch() looks before going to
 * the network. On other platforms these functions do nothing. */
namespace offline_maps {

/* Storage prefix for each game version, shared with map-upload's shell */
constexpr std::string_view prefix_for(blam::version_t version)
{
    switch(version)
    {
    case blam::version_t::pc:
        return "/pc";
    case blam::version_t::custom_edition:
        return "/custom";
    case blam::version_t::xbox:
        return "/xbox";
    case blam::version_t::mcc:
        return "/mcc";
    default:
        return {};
    }
}

constexpr std::string_view prefix = prefix_for(halo_version::version_v);

/* Makes every upload prefix offline-only, so files under them are only
 * ever read from IndexedDB, never requested over the network */
void register_storage();

/* Files under directory/, as listed when the page started */
std::vector<std::string> list(std::string_view directory);

/* /<prefix>/ui.map when it was uploaded for this build's game version */
std::optional<std::string> default_map();

} // namespace offline_maps
