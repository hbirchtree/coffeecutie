#include <coffee/core/datastorage/binary/virtualfs.h>

#include <coffee/core/CProfiling>
#include <coffee/core/base/files/url.h>
#include <coffee/core/CMath>
#include <coffee/core/datastorage/compression/libz.h>
#include <peripherals/stl/threads/job_system.h>

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

namespace index_creation {

/*!
 * \brief Insert into a static array the std::string
 * \param src
 * \param ptr
 * \param size
 */
static void CharInsert(CString const& src, char* ptr, szptr size)
{
    if(src.size() > (size - 1))
        Throw(std::out_of_range("entry too long"));

    MemCpy(Bytes::From(src.data(), src.size()), Bytes::From(ptr, size));
}

struct node_info_t
{
    using dir_data_t = VirtualIndex::directory_data_t;

    Vector<Path>            children;
    dir_data_t::node_base_t node;
};

struct node_working_set_t
{
    Vector<VirtualIndex::directory_data_t::node_base_t>& outNodes;
    Map<Path, node_info_t> const&                        stems;
};

struct directory_index_t
{
    VirtualIndex baseIndex;
    szptr        totalSize;

    Vector<VirtualIndex::directory_data_t::node_base_t> nodes;
};

using node_t = VirtualIndex::directory_data_t::node_t;

using child_slice = slice<const Vector<Path>, Vector<Path>::const_iterator>;
using part_slice  = slice<const child_slice, child_slice::iterator>;

static cstring VirtGetPrefix(node_working_set_t& set, part_slice::iterator p)
{
    auto& it = set.stems.find(*p)->second.node;

    return it.is_leaf() ? it.leaf.prefix : it.node.prefix;
}

static u32 VirtRecurseChildren(
    node_working_set_t& set, cstring src, node_info_t const& currentNode);

static u32 VirtPartitionChildren(
    node_working_set_t& set, part_slice const& children, cstring prefix)
{
    auto parentIdx = C_FCAST<u32>(set.outNodes.size());

    auto mid_point = C_FCAST<child_slice::difference_type>(children.size() / 2);
    auto rest      = C_FCAST<child_slice::difference_type>(children.size() % 2);

    /* Turned out empty, just leave the path empty */
    if(mid_point + rest == 0)
    {
        return node_t::sentinel_value;
    }

    /* Create node and assign prefix */
    set.outNodes.emplace_back();
    {
        auto& node = set.outNodes.back();
        CharInsert(prefix, node.node.prefix, MaxPrefixLength);
    }

    /* In this case, we have reached the bottom of the children tree,
     *  and go to the nodes */
    if(children.size() <= 2)
    {
        u32 right_child = node_t::sentinel_value;

        auto left_it    = set.stems.find(*children.begin());
        u32  left_child = VirtRecurseChildren(
            set, left_it->second.node.node.prefix, left_it->second);

        if(children.size() == 2)
        {
            auto right_it = set.stems.find(*(children.begin() + 1));
            right_child   = VirtRecurseChildren(
                set, right_it->second.node.node.prefix, right_it->second);
        }

        auto& node      = set.outNodes.at(parentIdx);
        node.node.left  = left_child;
        node.node.right = right_child;

        return parentIdx;
    }

    /* Directory tree has a minimum size of 3 at this point */

    auto left_slice =
        part_slice(children.begin(), children.begin() + mid_point + rest);
    auto right_slice =
        part_slice(children.begin() + mid_point + rest, children.end());

    u32 left_idx = VirtPartitionChildren(
        set, left_slice, VirtGetPrefix(set, left_slice.end() - 1));
    u32 right_idx = VirtPartitionChildren(
        set, right_slice, VirtGetPrefix(set, right_slice.begin()));

    auto& node      = set.outNodes.at(parentIdx);
    node.node.left  = left_idx;
    node.node.right = right_idx;

    return parentIdx;
}

static u32 VirtRecurseChildren(
    node_working_set_t& set, cstring src, node_info_t const& currentNode)
{
    auto        num_children = currentNode.children.size();
    auto const& childrenRef  = currentNode.children;

    if(num_children == 0 && !currentNode.node.is_leaf())
        return node_t::sentinel_value;

    if(currentNode.node.is_leaf())
    {
        auto currentIdx = C_FCAST<u32>(set.outNodes.size());
        set.outNodes.emplace_back(currentNode.node);
        return currentIdx;
    }

    /* We need to perform partitioning on this node */
    part_slice childrenRange(childrenRef.begin(), childrenRef.end());
    return VirtPartitionChildren(set, childrenRange, src);
}

enum class link_node_type
{
    node,
    leaf,
};

static szptr GetNodeTypeMaxLen(link_node_type t)
{
    if(t == link_node_type::leaf)
        return MaxLeafLength;
    return MaxPrefixLength;
}

static void GenPrefixNodes(
    Map<Path, node_info_t>& stems,
    CString                 prefix,
    CString                 remaining_suffix,
    link_node_type          node_type)
{
    const auto prefix_len = GetNodeTypeMaxLen(node_type);
    auto current_size     = CMath::min(remaining_suffix.size(), prefix_len - 1);

    auto append_prefix = prefix == "./" ? "" : prefix;
    prefix             = (prefix == "./")
                 ? "."
                 : (prefix.back() == '/') ? prefix.substr(0, prefix.size() - 1)
                                          : prefix;

    /* This is the end of the prefix nodes, the final one fits */
    if(current_size == remaining_suffix.size())
    {
        /* The final node, created as a leaf node */
        auto& parent = stems[Path(prefix)];
        auto& node   = stems[Path(append_prefix + remaining_suffix)];

        if(node_type == link_node_type::node)
        {
            node.node.node.left  = node_t::sentinel_value;
            node.node.node.right = node_t::sentinel_value;
            CharInsert(remaining_suffix, node.node.node.prefix, prefix_len);
        } else
        {
            node.node.leaf.set_mask();
            node.node.leaf.fileIdx = 0;
            CharInsert(remaining_suffix, node.node.leaf.prefix, prefix_len);
        }

        parent.children.push_back(Path(append_prefix + remaining_suffix));

        return;
    }

    /* Slicing the current suffix */
    auto cut_prefix = remaining_suffix.substr(0, current_size);
    auto cut_suffix = remaining_suffix.substr(current_size);

    auto new_parent = append_prefix + cut_prefix;

    auto& new_node        = stems[Path(new_parent)];
    auto& new_node_parent = stems[Path(prefix)];

    new_node.node.node.left  = node_t::sentinel_value;
    new_node.node.node.right = node_t::sentinel_value;
    CharInsert(cut_prefix, new_node.node.node.prefix, MaxPrefixLength);

    new_node_parent.children.push_back(Path(new_parent));

    GenPrefixNodes(stems, new_parent, cut_suffix, node_type);
}

static void PrintDirIndex(
    Vector<VirtualIndex::directory_data_t::node_base_t>& nodes)
{
    Deque<Pair<VirtualIndex::directory_data_t::node_base_t*, u32>> q;

    q.push_back({&nodes.front(), 1});

    while(q.size())
    {
        auto node = q.back();
        q.pop_back();

        if(node.first->is_leaf())
        {
            cDebug(
                "{0}> {1}",
                str::transform::multiply('=', node.second),
                node.first->leaf.prefix);
            continue;
        }

        cDebug(
            "{0}{ {1}",
            str::transform::multiply('-', node.second),
            node.first->node.prefix);

        if(node.first->node.left != node_t::sentinel_value)
            q.push_back({&nodes[node.first->node.left], node.second + 1});
        if(node.first->node.right != node_t::sentinel_value)
            q.push_back({&nodes[node.first->node.right], node.second + 1});
    }
}

static directory_index_t GenDirectoryIndex(
    _cbasic_data_chunk<const VFile> const& files)
{
    DProfContext _(VIRTFS_API "Generating directory index");

    using dir_data_t = VirtualIndex::directory_data_t;

    /* Directory-ordered binary tree */
    Map<Path, node_info_t>          stems;
    Vector<dir_data_t::node_base_t> outNodes;

    auto& rootStem = stems[Path(".")];
    CharInsert(".", rootStem.node.node.prefix, MaxPrefixLength);

    Profiler::DeepPushContext(VIRTFS_API "Creating directory stems");

    /* This function requires the sequential searching */
    u64 fileIdx = 0;
    for(auto const& file : files)
    {
        Path filename(file.name);
        Path stem = filename.dirname();

        auto pathComponents = stem.components();
        pathComponents.insert(pathComponents.begin(), Path("."));

        while(stem != Path("."))
        {
            GenPrefixNodes(
                stems,
                stem.dirname().internUrl + "/",
                stem.fileBasename().internUrl,
                link_node_type::node);

            stem = stem.dirname();
        }

        GenPrefixNodes(
            stems,
            filename.dirname().internUrl + "/",
            filename.fileBasename().internUrl,
            link_node_type::leaf);

        auto& node             = stems[filename];
        node.node.leaf.fileIdx = fileIdx;

        fileIdx++;
    }

    Profiler::DeepPopContext();

    Profiler::DeepPushContext(VIRTFS_API "Sorting tree nodes");

    /* Because std::set is hard to use with custom types, we do this */
    /* std::set would have been really nice to have */
    for(auto& stem : stems)
    {
        if(stem.second.node.is_leaf())
            continue;

        auto& children = stem.second.children;

        /* Remove dupes, the above loop tends to do that */
        auto it = std::unique(children.begin(), children.end());
        children.erase(it, children.end());

        std::sort(children.begin(), children.end());
    }

    Profiler::DeepPopContext();

    node_working_set_t work_set = {outNodes, stems};
    {
        DProfContext _(VIRTFS_API "Emitting tree nodes");
        VirtRecurseChildren(work_set, "./", stems[Path(".")]);
    }

    for(auto const& stem : stems)
    {
        cDebug("{0} ({1})", stem.first, stem.second.node.is_leaf());

        for(auto const& child : stem.second.children)
            cDebug("- {0}", child);
    }
    PrintDirIndex(outNodes);

    auto indexSize = sizeof(VirtualIndex) +
                     outNodes.size() * sizeof(dir_data_t::node_base_t);

    VirtualIndex outIndex;

    outIndex.kind                = VirtualIndex::index_t::directory_tree;
    outIndex.next_index          = indexSize;
    outIndex.directory.num_nodes = outNodes.size();

    return {std::move(outIndex), indexSize, std::move(outNodes)};
}

} // namespace index_creation

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
        threads::ParallelForEach(Range<>(filenames.size()), std::move(worker));
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
            data_size = Mem::AlignOffsetForward(8, data_size);

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

    files.resize(0);

    output->resize(output->capacity());
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

            index_creation::CharInsert(
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

        index_creation::directory_index_t dirIndex =
            index_creation::GenDirectoryIndex(files);

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

namespace index_lookup {

using node_base_t = VirtualIndex::directory_data_t::node_base_t;
using node_t      = VirtualIndex::directory_data_t::node_t;

using node_list_t = _cbasic_data_chunk<node_base_t const>;

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

STATICINLINE CString FilterTreeName(cstring name)
{
    return name;
}

static node_base_t const* GetNode(u32 idx, node_list_t const& nodes)
{
    if(idx == node_t::sentinel_value)
        return nullptr;

    return &nodes[idx];
}

} // namespace index_lookup

VFile const* VirtualFS::SearchFile(
    VFS const* vfs, cstring name, vfs_error_code& ec)
{
    auto index =
        index_lookup::FindIndex(vfs, VirtualIndex::index_t::directory_tree, ec);

    if(!index)
    {
        ec = VFSError::NoIndexing;
        return nullptr;
    }

    CString prefix        = index_lookup::FilterTreeName(name);
    szptr   currentPrefix = 0;

    auto nodes = index->directory.nodes(*index);

    auto currentNode = &nodes[0];

    if(currentNode->is_leaf())
        return nullptr;

    for(auto i : Range<>(index->directory.num_nodes))
    {
        cDebug("- {0}", nodes[i].node.prefix);
    }

    while(currentNode)
    {
        if(currentNode->is_leaf())
            return nullptr;

        auto& node = currentNode->node;

        auto left_node  = index_lookup::GetNode(node.left, nodes);
        auto right_node = index_lookup::GetNode(node.right, nodes);

        /* First look for longest match */
        auto left_match =
            left_node
                ? left_node->node.longest_match(prefix.substr(currentPrefix))
                : 0;
        auto right_match =
            right_node
                ? right_node->node.longest_match(prefix.substr(currentPrefix))
                : 0;

        if(left_match > right_match)
        {
            currentNode = left_node;
            currentPrefix += left_node->node.prefix_length();
            continue;
        } else if(right_match > left_match)
        {
            currentNode = right_node;
            currentPrefix += right_node->node.prefix_length();
            continue;
        }

        /* Look for ordering if there is no prefix match */
        if(left_node->node >= prefix.substr(currentPrefix))
        {
            currentNode = left_node;
        } else if(right_node->node <= prefix.substr(currentPrefix))
        {
            currentNode = right_node;
        }
    }

    return nullptr;
}

VFile const* VirtualFS::GetFileTreeExact(
    const VFS* vfs, cstring name, vfs_error_code& ec)
{
    auto index =
        index_lookup::FindIndex(vfs, VirtualIndex::index_t::directory_tree, ec);

    if(!index)
    {
        ec = VFSError::NoIndexing;
        return nullptr;
    }

    CString filename = index_lookup::FilterTreeName(name);

    return nullptr;
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
