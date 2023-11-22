#pragma once

#include <blam/volta/blam_stl.h>
#include <coffee/comp_app/file_mapper.h>
#include <coffee/components/components.h>
#include <coffee/core/files/cfiles.h>
#include <url/url.h>

template<typename V>
struct BlamFiles : compo::SubsystemBase
{
    using type = BlamFiles;

    std::shared_ptr<comp_app::FileMapper::Resource> map_file;
    std::shared_ptr<comp_app::FileMapper::Resource> bitmap_file;

    /* Stash this here, so others can reference it */
    blam::map_container<V> container;
};
