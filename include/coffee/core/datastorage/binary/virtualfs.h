#pragma once

#include <coffee/core/types/tdef/integertypes.h>
#include <coffee/core/types/cdef/memtypes.h>

namespace Coffee{

struct Url;
namespace CResources{
struct Resource;
}

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

static const constexpr szptr MaxFileNameLength = 96;
static const constexpr szptr MagicLength = 8;
static const constexpr char VFSMagic[MagicLength] = "CfVrtFS";

struct VirtualFS
{
    /*!
     * \brief Given bytes from `src', open the VFS inside iff
     *  it is correct. A VFS is only correct if comparing
     *  the file magic completes without error.
     * \param src
     * \param vfs
     * \return
     */
    static bool OpenVFS(Bytes const& src,
                        VirtualFS const* *vfs)
    {
        *vfs = nullptr;

        if(src.size < sizeof(VirtualFS))
            return false;

        VirtualFS* temp_vfs = C_RCAST<VirtualFS*>(src.data);

        if(!MemCmp(VFSMagic,
                   temp_vfs->vfs_header,
                   sizeof(vfs_header)))
            return false;

        *vfs = temp_vfs;

        return true;
    }

    char vfs_header[MagicLength]; /*!< Magic file header */

    szptr num_files; /*!< Number of VFile entries */
    szptr data_offset; /*!< Offset from start of this structure to the data segment */

    /*!
     * \brief Given a VFS, get the handle to a file with the name `name'.
     * \param vfs
     * \param name
     * \return nullptr if file not found
     */
    static VFile const* GetFile(
            VFS const* vfs,
            cstring name);

    /*!
     * \brief Given a VFS, get the handle to the idx'th file.
     * \param vfs
     * \param idx
     * \return nullptr if file not found
     */
    static VFile const* GetFile(
            VFS const* vfs,
            szptr idx);

    /*!
     * \brief Given a VFS and a valid file within, return the
     *  data. If the file is compressed, decompression
     *  will be done internally.
     * \param vfs
     * \param file
     * \return
     */
    static Bytes GetData(
            VFS const* vfs,
            VFile const* file);
};

PACKEDSTRUCT VirtualFile
{
    char name[MaxFileNameLength]; /*!< File name */
    szptr offset; /*!< Offset to file */
    szptr size; /*!< Size of file. If compressed, size of compressed file */
    szptr rsize; /*!< Size of file in memory. If compressed, uncompressed size */
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

FORCEDINLINE VFile const* VirtualFS::GetFile(const VFS *vfs, szptr idx)
{
    VFile const* vf_start = C_RCAST<VFile const*>(&vfs[1]);

    if(idx >= vfs->num_files)
        return nullptr;

    return &vf_start[idx];
}

struct Resource : ByteProvider
{
private:
    VFS const* filesystem;
    VFile const* file;

public:
    Resource(VFS const* base,
             Url const& url);

    /*!
     * \brief Check validity of resource and/or data it returns
     * \return
     */
    bool valid() const;
    /*!
     * \brief Return the data contained in the virtual file.
     *  If the file in question is compressed,
     *  decompression happens here.
     * \return
     */
    Bytes data() const;

    operator Bytes() const
    {
        return data();
    }
};

struct vfs_iterator : Iterator<ForwardIteratorTag, VFile>
{
    static constexpr szptr npos = C_CAST<szptr>(-1);

    /*!
     * \brief constructor for end-iterator
     */
    vfs_iterator():
        m_idx(npos),
        m_file(nullptr)
    {
    }

    vfs_iterator(VFS const* vfs, szptr idx):
        m_vfs(vfs),
        m_idx(idx),
        m_file(VFS::GetFile(vfs, idx))
    {
    }

    vfs_iterator& operator++()
    {
        m_idx++;
        if(m_idx < m_vfs->num_files)
            update_file();
        else
            m_idx = npos;
        return *this;
    }

    bool operator !=(vfs_iterator const& other) const
    {
        return other.m_idx != m_idx;
    }
    bool operator ==(vfs_iterator const& other) const
    {
        return other.m_idx == m_idx;
    }

    VFile const& operator*() const
    {
        if(!m_file)
            throw std::out_of_range("non-existent virtual file");
        return *m_file;
    }

    operator Bytes()
    {
        return VFS::GetData(m_vfs, m_file);
    }

private:
    void update_file()
    {
        m_file = VFS::GetFile(m_vfs, m_idx);
    }

    VFS const* m_vfs;
    szptr m_idx;
    VFile const* m_file;
};

struct vfs_view
{
    using iterator = vfs_iterator;

    vfs_view(Bytes const& base)
    {
        VFS::OpenVFS(base, &m_vfs);
    }

    iterator begin() const
    {
        return iterator(m_vfs, 0);
    }

    iterator end() const
    {
        return iterator();
    }

private:
    VFS const* m_vfs;
};

struct VirtDesc
{
    VirtDesc(cstring fname, Bytes&& data, u32 flags = 0):
        filename(fname),
        data(std::move(data)),
        flags(flags)
    {
    }

    CString filename;
    Bytes data;
    /*!
     * \brief when matching File_Compressed,
     *  compress the input data using zlib.
     */
    u32 flags;
};

extern bool GenVirtFS(
        Vector<VirtDesc> const& filenames,
        Vector<byte_t>* output);

}

}
