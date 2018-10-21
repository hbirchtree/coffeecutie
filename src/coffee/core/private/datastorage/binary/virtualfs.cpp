#include <coffee/core/CProfiling>
#include <coffee/core/base/files/url.h>
#include <coffee/core/datastorage/binary/virtualfs.h>
#include <coffee/core/datastorage/compression/libz.h>

#include <coffee/core/CDebug>

#define VIRTFS_API "VirtFS::"

namespace Coffee {
namespace VirtFS {

const char* vfs_error_category::name() const noexcept
{
    return "vfs_error_category";
}

std::string vfs_error_category::message(int error_code) const
{
    using E = VFSError;

    E e = C_CAST<E>(error_code);

    switch(e)
    {
    case E::NotFound:
        return "Virtual file entry not found";
    case E::NotVFS:
        return "File is not a virtual filesystem";
    case E::EndianMismatch:
        return "Virtual filesystem has incompatible endianness";
    case E::CompressionUnsupported:
        return "Compression is not supported";
    case E::CompressionError:
        return "Error while compressing";
    case E::NoFilesProvided:
        return "No files provided";
    case E::FilenameTooLong:
        return "Filename too long";
    }

    C_ERROR_CODE_OUT_OF_BOUNDS();
}

Resource::Resource(const VFS* base, const Url& url) : filesystem(base)
{
    vfs_error_code ec;
    file = VFS::GetFile(base, url.internUrl.c_str(), ec);

    C_ERROR_CHECK(ec);
}

bool Resource::valid() const
{
    return file != nullptr;
}

Bytes Resource::data() const
{
    if(!file)
        return {};

    vfs_error_code ec;
    return VFS::GetData(filesystem, file, ec);
}

static bool VirtDesc_Sort(VirtDesc const& d1, VirtDesc const& d2)
{
    return d1.filename < d2.filename;
}

bool GenVirtFS(
    Vector<VirtDesc>& filenames, Vector<byte_t>* output, vfs_error_code& ec)
{
    DProfContext _(VIRTFS_API "Generating VirtFS");

    /* You can't create a VFS without files */
    if(filenames.size() == 0)
    {
        Profiler::DeepProfile(VIRTFS_API "Failed, no files");
        ec = VFSError::NoFilesProvided;
        return false;
    }

    VFS base_fs = {};

    /* Insert header, calculate file segment offset */
    MemCpy(
        Bytes::From(VFSMagic_Encoded, 2),
        Bytes::From(base_fs.vfs_header, MagicLength));

    base_fs.num_files   = filenames.size();
    base_fs.data_offset = sizeof(VFS) + sizeof(VFile) * base_fs.num_files;

    /* ----------- v2 fields ----------- */
    base_fs.virtfs_version = VFSVersion;
    base_fs.indices_offset = 0; /* Filled in later */
    /* --------------------------------- */

    /* Sort, simplifies creating indexes later */
    std::sort(filenames.begin(), filenames.end(), VirtDesc_Sort);

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
            ec = VFSError::FilenameTooLong;
            return false;
        }

        /* We want to align data to 8-byte boundaries */
        data_size = Mem::AlignOffsetForward(8, data_size);

        file.offset = data_size;
        file.rsize  = filenames[i].data.size;

        MemCpy(fn, Bytes::From(file.name, fn.size()));
        file.flags = filenames[i].flags;

        auto& arr = data_arrays[i];
        if(file.flags & File_Compressed)
        {
#if defined(COFFEE_BUILD_NO_COMPRESSION)
            ec = VFSError::CompressionUnsupported;
            return false;
#else
            Compression::error_code comp_ec;
            Zlib::Compress(filenames[i].data, &arr, {}, comp_ec);

            if(comp_ec)
                ec = VFSError::CompressionError;

            cVerbose(
                10,
                "Compressed file: {0} bytes -> {1} bytes",
                filenames[i].data.size,
                arr.size);
            file.size = arr.size;
#endif
        } else
        {
            arr.data     = filenames[i].data.data;
            arr.size     = filenames[i].data.size;
            arr.elements = filenames[i].data.elements;

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
        if(srcData.size == 0)
        {
            cWarning("File {0} has problems", filenames[i].filename);
            continue;
        }
        MemCpy(
            srcData,
            outputView.at(base_fs.data_offset + files[i].offset, srcData.size));
    }

    /* ---------- Added in v2 ---------- */

    outputView.as<VFS>().data->indices_offset = output->size();

    /* Index creation */
    {
        /* Create extension buckets */
        Map<CString, Vector<u64>> extension_buckets;

        u64 i = 0;

        for(auto const& file : vfs_view(outputView))
        {
            Path filename(file.name);
            extension_buckets[filename.extension()].push_back(i);
            i++;
        }

        for(auto& index : extension_buckets)
        {
            /* We limit size of extensions */
            if(index.first.size() >= MaxExtensionLength)
                continue;

            VirtualIndex outIndex;
            outIndex.kind       = VirtualIndex::index_t::file_extension;
            outIndex.next_index = sizeof(VirtualIndex) +
                                  index.second.size() * sizeof(index.second[0]);

            auto extensionData =
                Bytes::From(outIndex.extension.ext, MaxExtensionLength);

            MemClear(extensionData);

            MemCpy(
                Bytes::From(index.first.data(), index.first.size()),
                extensionData);
            outIndex.extension.num_files = index.second.size();

            auto start = output->size();

            output->resize(
                output->size() + sizeof(VirtualIndex) +
                index.second.size() * sizeof(index.second[0]));

            MemCpy(
                Bytes::Create(outIndex), Bytes::CreateFrom(*output).at(start));

            start += sizeof(VirtualIndex);

            MemCpy(
                Bytes::CreateFrom(index.second),
                Bytes::CreateFrom(*output).at(start));
        }
    }

    outputView = Bytes::CreateFrom(*output);

    /* Ensure that we can create a node hierarchy of n^2 size */
    if((filenames.size() * filenames.size()) < std::numeric_limits<u32>::max())
    {
        /* Directory-ordered binary tree */

        for(auto const& file : vfs_view(outputView))
        {
            Path filename(file.name);

            auto baseFname = filename.fileBasename();

            Path stem = filename.dirname();

            while(stem != Path("."))
            {
                cDebug("Stem: {0}", stem);
                stem = stem.dirname();
            }
        }
    }

    /* --------------------------------- */

    Profiler::DeepProfile(VIRTFS_API "Creation successful");
    return true;
}

Bytes Coffee::VirtFS::VirtualFS::GetData(
    const VFS* vfs, const VFile* file, vfs_error_code& ec)
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
#if defined(COFFEE_BUILD_NO_COMPRESSION)
            ec = VFSError::CompressionUnsupported;
            return {};
#else
            Compression::error_code comp_ec;
            Zlib::Decompress(
                Bytes(srcPtr, srcSize, srcSize), &data, {}, comp_ec);

            if(comp_ec)
            {
                ec = VFSError::CompressionError;
                return {};
            }
#endif
        } else
        {
            data.data = srcPtr;
            data.size = srcSize;
        }
    } else
        ec = VFSError::NotFound;

    data.elements = data.size;

    return data;
}

ResourceResolver<Resource> VirtualFS::GetResolver(const VirtualFS* vfs)
{
    if(!vfs)
        Throw(undefined_behavior(VIRTFS_API "got null vfs!"));

    return {[=](Url const& path) { return VirtFS::Resource(vfs, path); },
            [=](Path const& query, Vector<Url>& output) {
                DProfContext _(DTEXT(VIRTFS_API "Resolving filesystem"));

                vfs_view view(Bytes::From(*vfs));

                DProfContext __(DTEXT(VIRTFS_API "Checking files"));
                auto         it = view.begin();
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
