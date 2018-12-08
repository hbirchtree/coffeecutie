#pragma once

#include <coffee/core/CEnvironment>
#include <coffee/core/CMath>
#include <coffee/core/CProfiling>
#include <coffee/core/datastorage/binary/virtualfs.h>
#include <peripherals/libc/memory_ops.h>
#include <peripherals/stl/threads/job_system.h>

#include <coffee/strings/libc_types.h>
#include <coffee/strings/url_types.h>

#include <coffee/core/CDebug>

#define VIRTFS_API "VirtFS::"

namespace Coffee {
namespace VirtFS {

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

using node_t      = VirtualIndex::directory_data_t::node_t;
using node_base_t = VirtualIndex::directory_data_t::node_base_t;

using child_slice = slice<const Vector<Path>, Vector<Path>::const_iterator>;
using part_slice  = slice<const child_slice, child_slice::iterator>;

static cstring VirtGetPrefix(node_working_set_t& set, part_slice::iterator p)
{
    auto& it = set.stems.find(*p)->second.node;

    return it.is_leaf() ? it.leaf.prefix : it.node.prefix;
}

static u32 VirtRecurseChildren(
    node_working_set_t& set, cstring src, node_info_t const& currentNode);

/*!
 * \brief VirtPartitionChildren creates waypoint nodes for traversing the files,
 *  and do not point to files. Files are added in VirtRecurseChildren().
 * \param set
 * \param children The children set, subdivided upon iteration
 * \param prefix Prefix of the node to be waypoint node to be created
 * \param flags Keeps track of how prefixes are considered
 *               when traversing the tree.
 * \return
 */
static u32 VirtPartitionChildren(
    node_working_set_t& set,
    part_slice const&   children,
    cstring             prefix,
    u32                 flags = 0)
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
        node.flags = flags;
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

    /* We slice the child tree in half, left and right side for space reasons */
    auto left_slice =
        part_slice(children.begin(), children.begin() + mid_point + rest);
    auto right_slice =
        part_slice(children.begin() + mid_point + rest, children.end());

    auto left_head  = left_slice.end() - 1;
    auto right_head = right_slice.begin();

    u32 left_idx =
        VirtPartitionChildren(set, left_slice, VirtGetPrefix(set, left_head));
    u32 right_idx =
        VirtPartitionChildren(set, right_slice, VirtGetPrefix(set, right_head));

    auto& node      = set.outNodes.at(parentIdx);
    node.node.left  = left_idx;
    node.node.right = right_idx;

    return parentIdx;
}

/*!
 * \brief VirtRecurseChildren check if node is leaf,
 *  or go through children if it's a directory node
 * \param set
 * \param src Parent node
 * \param currentNode Node to be examined
 * \return Index of created node
 */
static u32 VirtRecurseChildren(
    node_working_set_t& set, cstring src, node_info_t const& currentNode)
{
    auto        num_children = currentNode.children.size();
    auto const& childrenRef  = currentNode.children;

    /* Degenerate node */
    if(num_children == 0 && !currentNode.node.is_leaf())
        return node_t::sentinel_value;

    /* Create final file node */
    if(currentNode.node.is_leaf())
    {
        auto currentIdx = C_FCAST<u32>(set.outNodes.size());
        set.outNodes.emplace_back(currentNode.node);
        set.outNodes.back().flags = node_base_t::prefix_carry;
        return currentIdx;
    }

    /* We need to perform partitioning on this node */
    part_slice childrenRange(childrenRef.begin(), childrenRef.end());
    return VirtPartitionChildren(
        set, childrenRange, src, currentNode.node.flags);
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
            node.node.flags      = 0;
            node.node.node.left  = node_t::sentinel_value;
            node.node.node.right = node_t::sentinel_value;
            node.node.flags =
                node_base_t::prefix_carry | node_base_t::prefix_directory;
            CharInsert(remaining_suffix, node.node.node.prefix, prefix_len);
        } else
        {
            node.node.flags = 0;
            node.node.leaf.set_mask();
            node.node.leaf.fileIdx = 0;
            node.node.flags        = node_base_t::prefix_carry;
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
    new_node.node.flags      = node_base_t::prefix_carry;
    CharInsert(cut_prefix, new_node.node.node.prefix, MaxPrefixLength);

    new_node_parent.children.push_back(Path(new_parent));

    GenPrefixNodes(stems, new_parent, cut_suffix, node_type);
}

static void PrintDirIndex(
    Vector<VirtualIndex::directory_data_t::node_base_t>& nodes)
{
    if(!Env::ExistsVar("VIRTFS_GRAPH"))
        return;

    Deque<Pair<VirtualIndex::directory_data_t::node_base_t*, Pair<u32, u32>>> q;

    q.push_back({&nodes.front(), {0, 0}});

    cBasicPrint("digraph vfs {");

    while(q.size())
    {
        auto node = q.back();
        q.pop_back();

        if(node.first->is_leaf())
        {
            cBasicPrint(
                "    n{0} -> n{1}; n{1} [label=\"{2}\",color={3}];",
                node.second.first,
                node.second.second,
                node.first->leaf.prefix,
                (node.first->flags & node_base_t::prefix_directory)
                    ? "blue"
                    : ((node.first->flags & node_base_t::prefix_carry)
                           ? "green"
                           : "white"));
            continue;
        }

        cBasicPrint(
            "    n{0} -> n{1}; n{1} [label=\"{2}\",color={3}];",
            node.second.first,
            node.second.second,
            node.first->node.prefix,
            (node.first->flags & node_base_t::prefix_directory)
                ? "blue"
                : ((node.first->flags & node_base_t::prefix_carry) ? "green"
                                                                   : "white"));

        if(node.first->node.left != node_t::sentinel_value)
            q.push_back({&nodes[node.first->node.left],
                         {node.second.second, node.first->node.left}});
        if(node.first->node.right != node_t::sentinel_value)
            q.push_back({&nodes[node.first->node.right],
                         {node.second.second, node.first->node.right}});
    }

    cBasicPrint("}");
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

} // namespace VirtFS
} // namespace Coffee
