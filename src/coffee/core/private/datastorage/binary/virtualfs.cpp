#include <coffee/virtfs/virtualfs.h>

#include <corez/zlib.h>
#include <corezstd/zstd.h>

#include <peripherals/concepts/data_codec.h>
#include <peripherals/stl/parallel_for_each.h>

#include "directory_index.h"

using Coffee::DProfContext;
using Coffee::Profiler;

using Zlib = semantic::codec_stream_adapter<zlib::stream_codec>;
using ZStd = zstd::codec;

namespace vfs {

Resource::Resource(const fs_t* base, const Url& url) : filesystem(base)
{
    if(auto src = fs_t::GetFile(base, url.internUrl.c_str()); src.has_error())
        file = nullptr;
    else
        file = src.value();
}

bool Resource::valid() const
{
    return file != nullptr;
}

mem_chunk<const u8> Resource::data() const
{
    if(!file)
        return {};
    return fs_t::GetData(filesystem, file).value();
}

/*!
 * \brief This is used as a predicate for the VirtFS generation
 * \param d1
 * \param d2
 * \return
 */
static bool VirtDesc_Sort(desc const& d1, desc const& d2)
{
    return d1.filename < d2.filename;
}

stl_types::result<std::vector<libc_types::byte_t>, error> generate(
    std::vector<desc>& filenames, generation_settings&& settings)
{
    DProfContext _(VIRTFS_API "Generating VirtFS");
    using namespace Coffee;

    if(settings.workers == 0)
        settings.workers = std::thread::hardware_concurrency();

    /* You can't create a fs without files */
    if(filenames.size() == 0)
    {
        Profiler::DeepProfile(VIRTFS_API "Failed, no files");
        return error::no_files_provided;
    }

    fs_t base_fs = {};

    /* Insert header, calculate file segment offset */
    {
        auto src
            = mem_chunk<const u8>::ofBytes(VFSMagic_Encoded, sizeof(u32) * 2);
        auto dst = mem_chunk<u8>::ofBytes(base_fs.vfs_header, MagicLength);
        std::copy(src.begin(), src.end(), dst.begin());
    }

    base_fs.num_files   = filenames.size();
    base_fs.data_offset = sizeof(fs_t) + sizeof(file_t) * base_fs.num_files;

    /* ----------- v2 fields ----------- */
    base_fs.virtfs_version    = VFSVersion;
    base_fs.backcomp_sentinel = std::numeric_limits<u32>::max();
    base_fs.files_offset      = sizeof(fs_t);

    base_fs.ext_index.num    = 0; /* Filled in later */
    base_fs.ext_index.offset = 0; /* Filled in later */
    /* --------------------------------- */

    /* Sort, simplifies creating indexes later */
    std::sort(filenames.begin(), filenames.end(), VirtDesc_Sort);

    /* Pre-allocate vectors */
    std::vector<file_t> files;
    files.resize(filenames.size());
    std::vector<mem_chunk<const u8>> data_arrays;
    data_arrays.resize(filenames.size());

    {
        using stl_types::Range;

        DProfContext __(VIRTFS_API "Compressing files");

        std::function<void(szptr)> worker = [&](szptr i) {
            auto& file     = files[i];
            auto& src_file = filenames[i];
            auto& in_data  = filenames[i].data;
            auto  out_data = mem_chunk<char>::ofBytes(
                const_cast<u8*>(data_arrays[i].data), data_arrays[i].size);

            if(src_file.flags & File_Compressed)
            {

#if defined(COFFEE_BUILD_NO_COMPRESSION)
                ec = fsError::CompressionUnsupported;
                return false;
#else
                DProfContext _(VIRTFS_API "Compression worker");

                switch(settings.file_codec)
                {
#if defined(COFFEE_WINDOWS)
                case compression::codec::deflate_ms:
#else
                case compression::codec::deflate:
#endif
                {
                    Zlib::compress(in_data, out_data);
                    break;
                }
                case compression::codec::zstd:
                {
                    ZStd::compress(in_data, out_data);
                    break;
                }
                default: {
                    //                    ec =
                    //                    fsError::UnsupportedCompressionCodec;
                    break;
                }
                }

                file.codec = settings.file_codec;
                file.size  = out_data.size;
                out_data.updatePointers(semantic::Ownership::Owned);
#endif
            }
        };
        stl_types::parallel_for_each(
            stl_types::Range<>(filenames.size()),
            std::move(worker),
            settings.workers);
    }

    std::vector<libc_types::u8> output;

    {
        DProfContext __(VIRTFS_API "Setting up fs descriptors");
        szptr        data_size = 0;
        /* Define the data segment, compress data if necessary */

        for(auto i : stl_types::Range<>(filenames.size()))
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
                return error::filename_too_long;
            }

            /* We want to align data to 8-byte boundaries */
            data_size
                = libc::align::align<libc::align::dir_forward>(8, data_size);

            file.offset = data_size;
            file.rsize  = filenames[i].data.size();

            MemCpy(fn, Bytes::ofBytes(file.name_, fn.size()));
            file.flags = filenames[i].flags;

            auto& arr = data_arrays[i];
            if(!(file.flags & File_Compressed))
            {
                arr       = mem_chunk<const u8>::ofContainer(filenames[i].data);
                file.size = filenames[i].data.size();
            }

            data_size += file.size;
        }

        output.reserve(base_fs.data_offset + data_size);
    }

    /* Final size has been determined */

    {
        DProfContext _(VIRTFS_API "Copy fs header");
        /* Header is copied straight into array */
        MemCpy(Bytes::ofBytes(base_fs), output);
    }

    {
        DProfContext _(VIRTFS_API "Copy file descriptors");
        /* Copy file descriptors to fs */
        MemCpy(Bytes::ofContainer(files), output);
    }

    {
        DProfContext _(VIRTFS_API "Allocating output buffer");
        output.resize(output.capacity());
    }

    Bytes outputView = Bytes::ofContainer(output);

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
            *outputView.at(
                base_fs.data_offset + files[i].offset, srcData.size));
    }

    {
        DProfContext _(VIRTFS_API "Free'ing input files");
        files.resize(0);
    }

    data_arrays.resize(0);

    /* ---------- Added in v2 ---------- */

    outputView.as<fs_t>().data->ext_index.offset = output.size();

    /* Index creation */
    {
        DProfContext _(VIRTFS_API "Creating extension index");

        /* Create extension buckets */
        std::map<std::string, std::vector<u64>> extension_buckets;

        u64 i = 0;

        for(auto const& file : view::of(outputView.view))
        {
            Path filename(file.name());

            if(filename.extension().size() >= MaxExtensionLength)
                continue;

            extension_buckets[filename.extension()].push_back(i);
            i++;
        }

        for(auto& index : extension_buckets)
        {
            struct index_t outIndex;
            outIndex.kind = index_t::index_type::file_extension;
            outIndex.next_index
                = sizeof(index) + index.second.size() * sizeof(index.second[0]);

            auto extensionData
                = Bytes::ofBytes(outIndex.extension.ext, MaxExtensionLength);

            MemClear(extensionData);

            C_UNUSED(auto ext) = dir_index::CharInsert(
                index.first, outIndex.extension.ext, MaxExtensionLength);
            outIndex.extension.num_files = index.second.size();

            auto start = output.size();

            output.resize(
                output.size() + sizeof(index)
                + index.second.size() * sizeof(index.second[0]));

            MemCpy(
                Bytes::ofBytes(outIndex),
                *Bytes::ofContainer(output).at(start));

            start += sizeof(index);

            MemCpy(
                Bytes::ofContainer(index.second),
                *Bytes::ofContainer(output).at(start));
        }

        outputView = Bytes::ofContainer(output);

        outputView.as<fs_t>()[0].ext_index.num += extension_buckets.size();
    }

    /* Ensure that we can create a node hierarchy of n^2 size.
     * If this test fails, boy, you got a big filesystem on your hands.
     * And maybe you should consider just using the native filesystem.
     */
    if((filenames.size() * filenames.size()) < std::numeric_limits<u32>::max())
    {
        DProfContext _(VIRTFS_API "Inserting directory index");

        auto vfsRef = outputView.as<fs_t>().data;
        auto files  = vfsRef->files();

        dir_index::directory_index_t dirIndex = dir_index::Generate(files);

        auto prevSize = output.size();
        output.resize(output.size() + dirIndex.totalSize);
        outputView = Bytes::ofContainer(output);

        auto index_ref = *outputView.at(prevSize);
        auto data_ref  = *outputView.at(prevSize + sizeof(index_t));

        MemCpy(Bytes::ofBytes(dirIndex.baseIndex), index_ref);
        MemCpy(Bytes::ofBytes(dirIndex.nodes), data_ref);

        outputView = Bytes::ofContainer(output);

        outputView.as<fs_t>()[0].ext_index.num++;
    }

    /* --------------------------------- */

    Profiler::DeepProfile(VIRTFS_API "Creation successful");
    return output;
}

stl_types::result<directory_data_t::result_t, error> fs_t::SearchFile(
    fs_t const*                             vfs,
    std::string_view                      name,
    search_strategy                       strat,
    directory_data_t::cached_index const* filter)
{
    return dir_index::lookup::SearchFile(vfs, name, strat, filter);
}

stl_types::result<mem_chunk<const u8>, error> fs_t::GetData(
    const fs_t* vfs, const file_t* file)
{
    DProfContext _(VIRTFS_API "Getting data");

    auto basePtr = vfs->data();

    mem_chunk<const u8> data;

    if(file)
    {
        auto srcData = basePtr.subspan(file->offset, file->size);

        if(file->flags & File_Compressed)
        {
#if defined(COFFEE_BUILD_NO_COMPRESSION)
            ec = fsError::CompressionUnsupported;
            return {};
#else
            DProfContext _(VIRTFS_API "Decompressing file");
            auto writable = data.as<char>();

            switch(file->codec)
            {
#if defined(COFFEE_WINDOWS)
            case compression::codec::deflate_ms:
#else
            case compression::codec::deflate:
#endif
            {
                Zlib::decompress(srcData, writable);
                break;
            }
            case compression::codec::zstd:
            {
                ZStd::decompress(srcData, writable);
                break;
            }
            default: {
                return error::compression_codec_unsupported;
            }
            }
#endif
        } else
        {
            data           = mem_chunk<const u8>::ofContainer(srcData);
            data.ownership = semantic::Ownership::Borrowed;
        }
    } else
        return error::not_found;

    data.updatePointers(data.ownership);

    return data;
}

Coffee::ResourceResolver<Resource> fs_t::GetResolver(const fs_t* vfs)
{
    using namespace platform::url;

    if(!vfs)
        Throw(undefined_behavior(VIRTFS_API "got null vfs!"));

    return {
        [=](Url const& path) { return Resource(vfs, path); },
        [=](Path const& query, Vector<Url>& output) {
            DProfContext _(DTEXT(VIRTFS_API "Resolving filesystem"));

            if(vfs->supportsIndex(index_t::index_type::directory_tree))
            {
                auto index = fs_t::SearchFile(
                                 vfs,
                                 query.internUrl.c_str(),
                                 search_strategy::earliest)
                                 .value();

                DProfContext __(DTEXT(VIRTFS_API "Checking files"));

                /* Every child node necessarily has a lower index than its
                 * parent
                 */
                auto node_start = index.index.sub_root;
                auto files      = vfs->files();
                auto nodes      = index.index.nodes();

                for(auto i :
                    Range<>(index.index.node_match.size() - node_start))
                {
                    auto const idx = node_start + i;

                    if(!index.index.node_match.at(idx))
                        continue;

                    if(!nodes[idx].is_leaf())
                        continue;

                    auto const& node = files[nodes[idx].leaf.fileIdx];

                    output.push_back(constructors::MkUrl(node.name().data()));
                }
            } else
            {
                vfs::view view = vfs::view::of(BytesConst::ofBytes(*vfs).view);

                auto it = view.begin();
                while((it = view.starting_with(query, it)) != view.end())
                {
                    output.push_back(constructors::MkUrl((*it).name().data()));
                    ++it;
                }
            }

            return true;
        }};
}

} // namespace vfs
