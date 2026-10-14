#pragma once

#include "selected_version.h"

#include <array>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

/* Maps put in the browser's OPFS by BlamMapUpload (examples/blam/map-upload).
 * /pc/bloodgulch.map is requested as /maps/pc/bloodgulch.map, a file in
 * comp_app::FileMapper's store, never fetched from the network. On other
 * platforms these functions do nothing. */
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

/* Directories map-upload stores into, Halo 2 included */
constexpr std::array<std::string_view, 5> upload_dirs = {
    "/pc/", "/custom/", "/xbox/", "/mcc/", "/h2xbox/"};

/* FileMapper's store, where uploads are read from */
constexpr std::string_view store_root = "/maps";

/* Makes store_root offline-only, so files under it are only ever read
 * from the store, never requested over the network */
void register_storage();

/* /xbox/bloodgulch.map -> /maps/xbox/bloodgulch.map, anything else as is */
std::string resolve(std::string_view path);

/* Files under directory/, as listed when the page started */
std::vector<std::string> list(std::string_view directory);

/* /maps/<prefix>/ui.map when it was uploaded for this build's version */
std::optional<std::string> default_map();

} // namespace offline_maps
