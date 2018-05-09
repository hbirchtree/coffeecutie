#include <coffee/core/CDebug>
#include <coffee/core/CProfiling>
#include <coffee/core/base/files/url.h>
#include <coffee/core/datastorage/binary/virtualfs.h>
#include <coffee/core/datastorage/compression/libz.h>

#define VIRTFS_API "VirtFS::"

namespace Coffee {
namespace VirtFS {

Resource::Resource(const VFS* base, const Url& url) :
    filesystem(base), file(VFS::GetFile(base, url.internUrl.c_str()))
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

bool GenVirtFS(const Vector<VirtDesc>& filenames, Vector<byte_t>* output)
{
    DProfContext _(VIRTFS_API "Generating VirtFS");

    /* You can't create a VFS without files */
    if(filenames.size() == 0)
    {
        Profiler::DeepProfile(VIRTFS_API "Failed, no files");
        return false;
    }

    VFS base_fs = {};

    /* Insert header, calculate file segment offset */
    MemCpy(
        Bytes::From(VFSMagic_Encoded, 2),
        Bytes::From(base_fs.vfs_header, MagicLength));

    //    MemCpy(base_fs.vfs_header, VFSMagic, MagicLength);
    base_fs.num_files   = filenames.size();
    base_fs.data_offset = sizeof(VFS) + sizeof(VFile) * base_fs.num_files;

    /* Pre-allocate vectors */
    Vector<VFile> files;
    files.resize(filenames.size());
    Vector<Bytes> data_arrays;
    data_arrays.resize(filenames.size());

    szptr data_size = 0;
    /* Define the data segment, compress data if necessary */
    for(auto i : Range<>(filenames.size()))
    {
        auto& file = files[i];

        auto const& fn = filenames[i].filename;

        if(fn.size() * sizeof(fn[0]) > MaxFileNameLength)
        {
            cWarning(
                VIRTFS_API "Filename is too long for VirtFS: {0} > {1}",
                fn.size(),
                MaxFileNameLength);
            Profiler::DeepProfile(VIRTFS_API "Failed, filename too long");
            return false;
        }

        /* We want to align data to 8-byte boundaries */
        data_size = AlignOffsetForward(8, data_size);
        //        szptr alignment = AlignOffset(8, data_size);

        file.offset = data_size;
        file.rsize  = filenames[i].data.size;

        MemCpy(fn, Bytes::From(file.name, fn.size()));
        //        MemCpy(file.name, fn.c_str(), fn.size());
        file.flags = filenames[i].flags;

        auto& arr = data_arrays[i];
        if(file.flags & File_Compressed)
        {
            Zlib::Compress(filenames[i].data, &arr, {});
            cVerbose(
                10,
                "Compressed file: {0} bytes -> {1} bytes",
                filenames[i].data.size,
                arr.size);
            file.size = arr.size;
        } else
        {
            arr.data  = filenames[i].data.data;
            arr.size  = filenames[i].data.size;
            file.size = filenames[i].data.size;
        }

        data_size += file.size;
    }

    /* Final size has been determined */
    output->reserve(base_fs.data_offset + data_size);

    /* Header is copied straight into array */
    MemCpy(Bytes::From(base_fs), *output);

    /* For each file, insert the data into the array */
    for(auto i : Range<>(filenames.size()))
        MemCpy(Bytes::Create(files[i]), *output);

    output->resize(output->capacity());
    Bytes outputView = Bytes::CreateFrom(*output);

    for(auto i : Range<>(filenames.size()))
    {
        auto const& srcData = data_arrays[i];
        MemCpy(
            srcData,
            outputView.at(base_fs.data_offset + files[i].offset, srcData.size));
    }

    Profiler::DeepProfile(VIRTFS_API "Creation successful");
    return true;
}

Bytes Coffee::VirtFS::VirtualFS::GetData(const VFS* vfs, const VFile* file)
{
    byte_t const* basePtr = C_RCAST<byte_t const*>(vfs);

    Bytes data;

    if(file)
    {
        byte_t* srcPtr =
            C_CCAST<byte_t*>(basePtr + vfs->data_offset + file->offset);
        szptr srcSize = file->size;

        if(file->flags & File_Compressed)
        {
            if(!Zlib::Decompress(Bytes(srcPtr, srcSize, srcSize), &data, {}))
                cWarning(VIRTFS_API "Failed to decompress file");
        } else
        {
            data.data = srcPtr;
            data.size = srcSize;
        }
    }

    data.elements = data.size;

    return data;
}

ResourceResolver<Resource> VirtualFS::GetResolver(const VirtualFS* vfs)
{
    if(!vfs)
        Throw(undefined_behavior(VIRTFS_API "got null vfs!"));

    return {[=](Url const& path) { return VirtFS::Resource(vfs, path); },
            [=](Path const& query, Vector<Url>& output) {
                vfs_view view(Bytes::From(*vfs));

                auto it = view.begin();
                while((it = view.starting_with(query, it)) != view.end())
                {
                    output.push_back(MkUrl((*it).name));
                    ++it;
                }

                return true;
            }};
}

} // namespace VirtFS
} // namespace Coffee
