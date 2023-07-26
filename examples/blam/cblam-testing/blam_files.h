#pragma once

#include <blam/volta/blam_stl.h>
#include <coffee/components/components.h>
#include <coffee/core/files/cfiles.h>
#include <url/url.h>

struct BlamFiles : compo::SubsystemBase
{
    using type = BlamFiles;

    platform::url::Url map_directory;

    std::unique_ptr<Coffee::Resource> map_file;
    std::unique_ptr<Coffee::Resource> bitmap_file;

    std::vector<platform::url::Url> maps;

    blam::magic_data_t map_magic;
    blam::magic_data_t bitm_magic;

    compo::time_point last_updated;
};
