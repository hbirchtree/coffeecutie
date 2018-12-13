#include <coffee/core/datastorage/binary/virtualfs.h>

#include <coffee/core/datastorage/compression/libz.h>

#include "directory_index.h"

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
    case E::VersionMismatch:
        return "Version mismatch";
    case E::NoIndexing:
        return "File does not contain index";
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

/*!
 * \brief This is used as a predicate for the VirtFS generation
 * \param d1
 * \param d2
 * \return
 */
static bool VirtDesc_Sort(VirtDesc const& d1, VirtDesc const& d2)
{
    return d1.filename < d2.filename;
}

bool GenVirtFS(
    Vector<VirtDesc>&     filenames,
    Vector<byte_t>*       output,
    vfs_error_code&       ec,
    generation_settings&& settings)
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
    base_fs.virtfs_version    = VFSVersion;
    base_fs.backcomp_sentinel = std::numeric_limits<u32>::max();
    base_fs.files_offset      = sizeof(VFS);

    base_fs.ext_index.num    = 0; /* Filled in later */
    base_fs.ext_index.offset = 0; /* Filled in later */
    /* --------------------------------- */

    /* Sort, simplifies creating indexes later */
    std::sort(filenames.begin(), filenames.end(), VirtDesc_Sort);

    /* Pre-allocate vectors */
    Vector<VFile> files;
    files.resize(filenames.size());
    Vector<Bytes> data_arrays;
    data_arrays.resize(filenames.size());

    {
        DProfContext __(VIRTFS_API "Compressing files");

        Function<void(szptr&)> worker = [&](szptr& i) {
            auto& file     = files[i];
            auto& src_file = filenames[i];
            auto& in_data  = filenames[i].data;
            auto& out_data = data_arrays[i];

            if(src_file.flags & File_Compressed)
            {

#if defined(COFFEE_BUILD_NO_COMPRESSION)
                ec = VFSError::CompressionUnsupported;
                return false;
#else
                DProfContext _(VIRTFS_API "Compression worker");

                Compression::error_code comp_ec;
                Zlib::Compress(in_data, &out_data, {}, comp_ec);

                if(comp_ec)
                {
                    ec = VFSError::CompressionError;
                }

                file.size = out_data.size;
#endif
            }
        };
        threads::ParallelForEach(
            Range<>(filenames.size()), std::move(worker), settings.workers);
    }

    {
        DProfContext __(VIRTFS_API "Setting up VFS descriptors");
        szptr        data_size = 0;
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
            data_size =
                libc::align::align<libc::align::dir_forward>(8, data_size);

            file.offset = data_size;
            file.rsize  = filenames[i].data.size;

            MemCpy(fn, Bytes::From(file.name, fn.size()));
            file.flags = filenames[i].flags;

            auto& arr = data_arrays[i];
            if(!(file.flags & File_Compressed))
            {
                arr.data     = filenames[i].data.data;
                arr.size     = filenames[i].data.size;
                arr.elements = filenames[i].data.elements;

                file.size = filenames[i].data.size;
            }

            data_size += file.size;
        }

        output->reserve(base_fs.data_offset + data_size);
    }

    /* Final size has been determined */

    {
        DProfContext _(VIRTFS_API "Copy VFS header");
        /* Header is copied straight into array */
        MemCpy(Bytes::From(base_fs), *output);
    }

    {
        DProfContext _(VIRTFS_API "Copy file descriptors");
        /* Copy file descriptors to VFS */
        MemCpy(Bytes::CreateFrom(files), *output);
    }

    {
        DProfContext _(VIRTFS_API "Free'ing input files");
        files.resize(0);
    }
    {
        DProfContext _(VIRTFS_API "Allocating output buffer");
        output->resize(output->capacity());
    }

    Bytes outputView = Bytes::CreateFrom(*output);

    for(auto i : Range<>(filenames.size()))
    {
        DProfContext _(VIRTFS_API "Copying output data");

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

    data_arrays.resize(0);

    /* ---------- Added in v2 ---------- */

    outputView.as<VFS>().data->ext_index.offset = output->size();

    /* Index creation */
    {
        DProfContext _(VIRTFS_API "Creating extension index");

        /* Create extension buckets */
        Map<CString, Vector<u64>> extension_buckets;

        u64 i = 0;

        for(auto const& file : vfs_view(outputView))
        {
            Path filename(file.name);

            if(filename.extension().size() >= MaxExtensionLength)
                continue;

            extension_buckets[filename.extension()].push_back(i);
            i++;
        }

        for(auto& index : extension_buckets)
        {
            VirtualIndex outIndex;
            outIndex.kind       = VirtualIndex::index_t::file_extension;
            outIndex.next_index = sizeof(VirtualIndex) +
                                  index.second.size() * sizeof(index.second[0]);

            auto extensionData =
                Bytes::From(outIndex.extension.ext, MaxExtensionLength);

            MemClear(extensionData);

            dir_index::CharInsert(
                index.first, outIndex.extension.ext, MaxExtensionLength);
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

        outputView = Bytes::CreateFrom(*output);

        outputView.as<VFS>()[0].ext_index.num += extension_buckets.size();
    }

    outputView = Bytes::CreateFrom(*output);

    /* Ensure that we can create a node hierarchy of n^2 size.
     * If this test fails, boy, you got a big filesystem on your hands.
     * And maybe you should consider just using the native filesystem.
     */
    if((filenames.size() * filenames.size()) < std::numeric_limits<u32>::max())
    {
        DProfContext _(VIRTFS_API "Inserting directory index");

        auto vfsRef = outputView.as<VFS>().data;
        auto files  = _cbasic_data_chunk<const VFile>::From(
            vfsRef->files(), vfsRef->num_files * sizeof(VFile));

        dir_index::directory_index_t dirIndex = dir_index::Generate(files);

        auto prevSize = output->size();
        output->resize(output->size() + dirIndex.totalSize);
        outputView = Bytes::CreateFrom(*output);

        auto index_ref = outputView.at(prevSize);
        auto data_ref  = outputView.at(prevSize + sizeof(VirtualIndex));

        MemCpy(Bytes::From(dirIndex.baseIndex), index_ref);
        MemCpy(Bytes::CreateFrom(dirIndex.nodes), data_ref);

        outputView = Bytes::CreateFrom(*output);

        outputView.as<VFS>()[0].ext_index.num++;
    }

    /* --------------------------------- */

    Profiler::DeepProfile(VIRTFS_API "Creation successful");
    return true;
}

directory_data_t::result_t VirtualFS::SearchFile(
    VFS const* vfs, cstring name, vfs_error_code& ec, search_strategy strat)
{
    return dir_index::lookup::SearchFile(vfs, name, ec, strat);
}

Bytes Coffee::VirtFS::VirtualFS::GetData(
    const VFS* vfs, const VFile* file, vfs_error_code& ec)
{
    DProfContext _(VIRTFS_API "Getting data");

    byte_t const* basePtr = C_RCAST<byte_t const*>(vfs->data());

    Bytes data;

    if(file)
    {
        byte_t* srcPtr  = C_CCAST<byte_t*>(basePtr + file->offset);
        szptr   srcSize = file->size;

        if(file->flags & File_Compressed)
        {
#if defined(COFFEE_BUILD_NO_COMPRESSION)
            ec = VFSError::CompressionUnsupported;
            return {};
#else
            DProfContext _(VIRTFS_API "Decompressing file");

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
    using namespace platform::url;

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
                    output.push_back(constructors::MkUrl((*it).name));
                    ++it;
                }

                return true;
            }};
}

} // namespace VirtFS
} // namespace Coffee
