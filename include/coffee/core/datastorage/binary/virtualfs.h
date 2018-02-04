#pragma once

#include <coffee/core/types/tdef/integertypes.h>
#include <coffee/core/types/cdef/memtypes.h>

namespace Coffee{

struct Url;
struct Resource;

/*! \brief VirtualFS is used to store an asset file system in a single file.
 * Referring into it is done using filenames, and all the files are
 *  ReadOnly, memory mappable.
 *
 * The constituent files may be compressed on-disk, requiring
 *  in-memory inflation. These VirtualFiles are marked
 *  File_Compressed in the `flags` field.
 *
 * Layout of a .cvfs file:
 *
 * |---------------|----------|----------|--------------|
 * | VFS struct[0] | VFile[0] | VFile[1] | Blob data    |
 * |---------------|----------|----------|--------------|
 *
 * VirtualFS struct serves as a header, with a magic segment.
 * VirtualFile structures refer into the Blob data segment.
 *
 */
namespace VirtFS
{

struct VirtualFile;
struct VirtualFS;

using VFS = VirtualFS;
using VFile = VirtualFile;

enum FileFlags
{
    File_Compressed = 0x1,
};

#define VirtualFS_Magic "CfVirtFS"

struct VirtualFS
{
    char vfs_header[16] = VirtualFS_Magic;

    szptr num_files; /*!< Number of VFile entries */
    szptr data_offset;

    static VFile const* GetFile(
            VFS const* vfs,
            cstring name);

    static Bytes GetData(
            VFS const* vfs,
            VFile const* file);
};

PACKEDSTRUCT VirtualFile
{
    char name[96]; /*!< File name */
    szptr offset; /*!< Offset to file */
    szptr size; /*!< Size of file */
    u32 flags;
};

FORCEDINLINE VFile const* VirtualFS::GetFile(
        VFS const* vfs,
        cstring name)
{
    /* We start finding files after the VirtualFS structure */
    VFile const* vf_start = C_RCAST<VFile const*>(&vfs[1]);

    for(szptr i=0; i<vfs->num_files; i++)
    {
        auto& current_file = vf_start[i];

        if(MemCmp(name, current_file.name, StrLen(name)))
            return &current_file;
    }

    return nullptr;
}

FORCEDINLINE Bytes VirtualFS::GetData(
        VFS const* vfs,
        VFile const*  file)
{
    byte_t const* basePtr = C_RCAST<byte_t const*>(vfs);

    Bytes data;
    data.data = C_CCAST<byte_t*>(basePtr + vfs->data_offset + file->offset);
    data.size = file->size;

    return data;
}

struct Resource
{
private:
    VFS const* filesystem;
    VFile const* file;

public:
    Resource(VFS const* base,
             Url const& url);

    Bytes data() const;
};

}

}
