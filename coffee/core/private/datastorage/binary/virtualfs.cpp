#include <coffee/core/datastorage/binary/virtualfs.h>
#include <coffee/core/base/files/url.h>
#include <coffee/core/CProfiling>
#include <coffee/core/CDebug>
#include <coffee/core/datastorage/compression/libz.h>

#define VIRTFS_API "VirtFS::"

namespace Coffee{
namespace VirtFS{

Resource::Resource(const VFS *base, const Url &url):
    filesystem(base),
    file(VFS::GetFile(base, url.internUrl.c_str()))
{
}

bool Resource::valid() const
{
    return file != nullptr;
}

Bytes Resource::data() const
{
    if(!file)
        return {};

    return VFS::GetData(filesystem, file);
}

bool GenVirtFS(const Vector<VirtDesc> &filenames, Vector<byte_t> *output)
{
    DProfContext _(VIRTFS_API "Generating VirtFS");
    /* You can't create a VFS without files */
    if(filenames.size() == 0)
    {
        Profiler::DeepProfile(VIRTFS_API "Failed to create VirtFS");
        return false;
    }

    VFS base_fs = {};

    MemCpy(base_fs.vfs_header, VFSMagic, MagicLength);
    base_fs.num_files = filenames.size();
    base_fs.data_offset =
            sizeof(VFS) +
            sizeof(VFile) * base_fs.num_files;

    Vector<VFile> files;
    files.resize(filenames.size());
    Vector<Bytes> data_arrays;
    data_arrays.resize(filenames.size());

    szptr data_size = 0;
    for(auto i : Range<>(filenames.size()))
    {
        auto& file = files[i];
        static constexpr szptr max_fname = sizeof(file.name) * sizeof(file.name[0]);

        auto const& fn = filenames[i].filename;

        if(fn.size() * sizeof(fn[0]) > max_fname)
        {
            cWarning(VIRTFS_API "Filename is too long for VirtFS: {0} > {1}", fn.size(), max_fname);
            return false;
        }

        file.offset = data_size;
        file.rsize = filenames[i].data.size;
        MemCpy(file.name, fn.c_str(), fn.size());
        file.flags = filenames[i].flags;

        auto& arr = data_arrays[i];
        if(file.flags & File_Compressed)
        {
            Zlib::Compress(filenames[i].data,
                           &arr, {});
            file.size = arr.size;
        }else
        {
            arr.data = filenames[i].data.data;
            arr.size = filenames[i].data.size;
            file.size = filenames[i].data.size;
        }

        data_size += file.size;
    }

    output->resize(base_fs.data_offset + data_size);

    MemCpy(output->data(), &base_fs, sizeof(VFS));

    for(auto i : Range<>(filenames.size()))
    {
        auto header_offset = sizeof(VFS) + sizeof(VFile) * i;
        auto final_offset = base_fs.data_offset + files[i].offset;

        MemCpy(&(*output)[header_offset], &files[i],
               sizeof(VFile));
        MemCpy(&(*output)[final_offset], data_arrays[i].data,
               data_arrays[i].size);
    }

    Profiler::DeepProfile(VIRTFS_API "VirtFS creation successful");
    return true;
}

Bytes Coffee::VirtFS::VirtualFS::GetData(const VFS *vfs, const VFile *file)
{
    byte_t const* basePtr = C_RCAST<byte_t const*>(vfs);

    Bytes data;

    if(file)
    {
        byte_t* srcPtr = C_CCAST<byte_t*>(basePtr
                                          + vfs->data_offset
                                          + file->offset);
        szptr srcSize = file->size;

        if(file->flags & File_Compressed)
        {
            if(!Zlib::Decompress(Bytes(srcPtr, srcSize), &data, {}))
                cWarning(VIRTFS_API "Failed to decompress file");
        }else
        {
            data.data = srcPtr;
            data.size = srcSize;
        }
    }

    return data;
}

}
}
