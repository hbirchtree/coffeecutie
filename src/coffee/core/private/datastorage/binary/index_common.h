#pragma once

#include <coffee/core/datastorage/binary/virtualfs.h>

namespace Coffee {
namespace VirtFS {

namespace index_common {

static VirtualIndex const* FindIndex(
    VFS const* vfs, VirtualIndex::index_t kind, vfs_error_code& ec)
{
    if(vfs->version() == Version::v1)
    {
        ec = VFSError::VersionMismatch;
        return nullptr;
    }

    if(vfs->ext_index.num == 0)
    {
        ec = VFSError::NoIndexing;
        return nullptr;
    }

    VirtualIndex const* index = vfs->indices();

    for(C_UNUSED(auto i) : Range<>(vfs->ext_index.num + 1))
    {
        if(index->kind == kind)
            return index;

        index = index->next();
    }

    return nullptr;
}

} // namespace index_common

} // namespace VirtFS
} // namespace Coffee
