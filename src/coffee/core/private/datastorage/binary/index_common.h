#pragma once

#include <coffee/virtfs/virtualfs.h>

namespace vfs::index_common {

static stl_types::result<index_t const*, error> FindIndex(
    fs_t const* vfs, index_t::index_type kind)
{
    if(vfs->version() == Version::v1)
    {
        return error::version_mismatch;
    }

    if(vfs->ext_index.num == 0)
    {
        return error::no_indexing;
    }

    auto index = vfs->indices().data();

    for(C_UNUSED(auto i) : stl_types::Range<>(vfs->ext_index.num + 1))
    {
        if(index->kind == kind)
            return index;

        index = index->next();
    }

    return error::index_not_found;
}

} // namespace vfs::index_common
