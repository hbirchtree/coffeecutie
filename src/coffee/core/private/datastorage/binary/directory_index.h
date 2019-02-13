#pragma once

#include <coffee/core/CEnvironment>
#include <coffee/core/CMath>
#include <coffee/core/CProfiling>
#include <coffee/core/datastorage/binary/virtualfs.h>
#include <peripherals/libc/memory_ops.h>
#include <peripherals/stl/threads/job_system.h>

#include <coffee/strings/libc_types.h>
#include <coffee/strings/url_types.h>

#include "index_common.h"

#include <coffee/core/CDebug>

#define VIRTFS_API "VirtFS::"

namespace Coffee {
namespace VirtFS {

namespace dir_index {

/*!
 * \brief Insert into a static array the std::string
 * \param src
 * \param ptr
 * \param size
 */
NO_DISCARD static u8 CharInsert(CString const& src, char* ptr, szptr size)
{
    if(src.size() > (size - 1))
        Throw(std::out_of_range("entry too long"));

    MemCpy(Bytes::From(src.data(), src.size()), Bytes::From(ptr, size));
    return C_FCAST<u8>(src.size());
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
        node.node.len_info.prefix =
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

/*!
 * \brief When a node's name is too long to fit in 40/32 bytes, we split it.
 * When splitting the names, the prefix is always carried.
 * \param stems
 * \param prefix
 * \param remaining_suffix
 * \param node_type
 */
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
            node.node.flags =
                node_base_t::prefix_carry | node_base_t::prefix_directory;
            node.node.node.len_info.prefix =
                CharInsert(remaining_suffix, node.node.node.prefix, prefix_len);
        } else
        {
            node.node.leaf.set_mask();
            node.node.leaf.fileIdx = 0;
            node.node.flags        = node_base_t::prefix_carry;
            node.node.node.len_info.prefix =
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
    new_node.node.node.len_info.prefix =
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

namespace opt_detail {

static void AdoptNode(
    node_working_set_t& set, directory_data_t::child_t<u32>* from_to)
{
    u32   from   = *from_to;
    auto& source = set.outNodes.at(from);
    *from_to     = source.node.left;
}

static bool NeedsOptimization(node_working_set_t& set, u32 idx = 0)
{
    auto& node = set.outNodes.at(idx);

    /* Can't do anything with leaves */
    if(node.is_leaf())
        return false;

    /* Run the process for the children first */
    if(node.node.left.valid())
        if(NeedsOptimization(set, node.node.left))
            AdoptNode(set, &node.node.left);
    if(node.node.right.valid())
        if(NeedsOptimization(set, node.node.right))
            AdoptNode(set, &node.node.right);

    /* Left node will always exist, only check right */
    if(node.node.right.valid())
        return false;

    /* Check if this node carries or is a directory, in which case it's useful
     */
    if(node.flags != 0)
        return false;

    /* In this case, the node does not do anything productive */
    return true;
}

} // namespace opt_detail

static directory_index_t Generate(_cbasic_data_chunk<const VFile> const& files)
{
    DProfContext _(VIRTFS_API "Generating directory index");

    using dir_data_t = VirtualIndex::directory_data_t;

    /* Directory-ordered binary tree */
    Map<Path, node_info_t>          stems;
    Vector<dir_data_t::node_base_t> outNodes;

    /* This part is only used as the root of the tree,
     *  and is never included in paths. */
    auto& rootStem = stems[Path(".")];
    rootStem.node.node.len_info.prefix =
        CharInsert(".", rootStem.node.node.prefix, MaxPrefixLength);

    Profiler::DeepPushContext(VIRTFS_API "Creating directory stems");

    /* We create nodes for every file, splitting them if necessary.
     * We call them stems because a node may be the stem of a file's name,
     *  and we can use lexicographical ordering to find them. */
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

    /* With the created stems, we sort their children to allow
     *  binary partitioning later. This allows efficient log N lookups. */
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

    /* Finally construct a binary tree of the files and their children.
     * Binary trees were chosen for simplicity in on-disk storage. */
    node_working_set_t work_set = {outNodes, stems};
    {
        DProfContext _(VIRTFS_API "Emitting tree nodes");
        VirtRecurseChildren(work_set, "./", stems[Path(".")]);
    }

    /* As a last pass, we optimize the binary tree,
     *  fixing nodes with only a single child.
     * This is a very minor improvement. */
    {
        DProfContext _(VIRTFS_API "Optimizing tree");
        opt_detail::NeedsOptimization(work_set);
    }

    {
        DProfContext _(VIRTFS_API "Printing tree");
        PrintDirIndex(outNodes);
    }

    auto indexSize = sizeof(VirtualIndex) +
                     outNodes.size() * sizeof(dir_data_t::node_base_t);

    VirtualIndex outIndex;

    outIndex.kind                = VirtualIndex::index_t::directory_tree;
    outIndex.next_index          = indexSize;
    outIndex.directory.num_nodes = outNodes.size();

    return {std::move(outIndex), indexSize, std::move(outNodes)};
}

namespace lookup {

using node_base_t = VirtualIndex::directory_data_t::node_base_t;
using node_t      = VirtualIndex::directory_data_t::node_t;

using node_list_t = _cbasic_data_chunk<node_base_t const>;

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

static bool SelectNodes(
    directory_data_t::cached_index& cache_idx,
    node_base_t const*              nodes,
    u32                             idx = 0)
{
    auto const& node = nodes[idx];

    if(node.is_leaf())
        return cache_idx.node_match.at(idx);

    auto left = node.node.left.valid()
                    ? SelectNodes(cache_idx, nodes, node.node.left)
                    : false;
    auto right = node.node.right.valid()
                     ? SelectNodes(cache_idx, nodes, node.node.right)
                     : false;

    return (cache_idx.node_match.at(idx) = left || right);
}

static u32 GetFirstSelected(
    directory_data_t::cached_index& cache_idx,
    node_base_t const*              nodes,
    u32                             idx = 0)
{
    if(!cache_idx.node_match.at(idx))
        return node_t::sentinel_value;

    auto const& node = nodes[idx];

    if(node.is_leaf())
        return cache_idx.node_match.at(idx);

    u8 num_matches = 0;

    if(node.node.left.valid())
        num_matches += cache_idx.node_match.at(node.node.left);
    if(node.node.right.valid())
        num_matches += cache_idx.node_match.at(node.node.right);

    if(num_matches == 2)
        return idx;
    else if(num_matches == 1)
    {
        auto valid_left =
            node.node.left.valid() && cache_idx.node_match.at(node.node.left);
        auto valid_right =
            node.node.right.valid() && cache_idx.node_match.at(node.node.right);

        return valid_left
                   ? GetFirstSelected(cache_idx, nodes, node.node.left)
                   : valid_right
                         ? GetFirstSelected(cache_idx, nodes, node.node.right)
                         : node_t::sentinel_value;
    } else
        return node_t::sentinel_value;
}

directory_data_t::result_t SearchFile(
    VFS const*                            vfs,
    cstring                               name,
    vfs_error_code&                       ec,
    search_strategy                       strat,
    directory_data_t::cached_index const* filter)
{
    using node_base_t = VirtualIndex::directory_data_t::node_base_t;

    auto index =
        index_common::FindIndex(vfs, VirtualIndex::index_t::directory_tree, ec);

    if(!index)
    {
        ec = VFSError::NoIndexing;
        return {{}, {}, strat};
    }

    CString prefix        = lookup::FilterTreeName(name);
    szptr   currentPrefix = 0;
    szptr   prefixLen     = prefix.size();

    auto nodes = index->directory.nodes(*index);

    auto currentNode = &nodes[0];

    if(filter)
    {
        currentNode = &nodes[filter->sub_root];
    }

    if(currentNode->is_leaf())
        return {{}, {}, strat};

    /* exact matching can achieve log N by not doing any backtracking */
    if(strat == search_strategy::exact)
        while(currentNode)
        {
            /* Exact match */
            if(currentPrefix +
                   currentNode->longest_match(prefix, currentPrefix) ==
               prefixLen)
                return {{}, {currentNode, index}, strat};

            /* Can't go further with leaf node */
            if(currentNode->is_leaf())
                return {{}, {}, strat};

            /* It's longer than our prefix, mismatch */
            if(currentPrefix > prefixLen)
                return {{}, {}, strat};

            auto& node = currentNode->node;

            auto left_node  = lookup::GetNode(node.left, nodes);
            auto right_node = lookup::GetNode(node.right, nodes);

            /* If a filter is applied, don't traverse unmarked nodes */
            if(filter)
            {
                if(node.left.valid() && !filter->node_match.at(node.left))
                    left_node = nullptr;
                if(node.left.valid() && !filter->node_match.at(node.right))
                    right_node = nullptr;
            }

            /* First look for longest match within each tree */
            auto left_match =
                left_node ? left_node->longest_match(prefix, currentPrefix) : 0;
            auto right_match =
                right_node ? right_node->longest_match(prefix, currentPrefix)
                           : 0;

            if(left_match > right_match)
            {
                currentNode = left_node;
                if(left_node->flags & node_base_t::prefix_directory)
                    currentPrefix += left_node->prefix_length() + 1;
                else if(left_node->flags & node_base_t::prefix_carry)
                    currentPrefix += left_node->prefix_length();
                continue;
            } else if(right_match > left_match)
            {
                currentNode = right_node;
                if(right_node->flags & node_base_t::prefix_directory)
                    currentPrefix += right_node->prefix_length() + 1;
                else if(right_node->flags & node_base_t::prefix_carry)
                    currentPrefix += right_node->prefix_length();
                continue;
            }

            using lex_order = libc::str::lexical_order;
            using namespace ::enum_helpers;

            auto left_lex_order = libc::str::cmp_enum(
                left_node->node.prefix,
                &prefix[currentPrefix],
                (std::min<
                    u32>)(prefix.size() - currentPrefix, left_node->prefix_length()));
            auto right_lex_order = libc::str::cmp_enum(
                right_node->node.prefix,
                &prefix[currentPrefix],
                (std::min<
                    u32>)(prefix.size() - currentPrefix, right_node->prefix_length()));

            /* Look for ordering if there is no prefix match */
            auto left_order =
                left_node ? (feval(left_lex_order & lex_order::aequal)) : false;
            auto right_order =
                right_node ? (feval(right_lex_order & lex_order::bequal))
                           : false;

            if(left_order)
            {
                currentNode = left_node;
                continue;
            } else if(right_order)
            {
                currentNode = right_node;
                continue;
            }

            /* Failed to select entry */
            return {{}, {}, strat};
        }
    /* Earliest matching creates a bitmap of matching nodes.
     * This can be used later to combine bitmaps, ANDing or ORing them. */
    else if(strat == search_strategy::earliest)
    {
        directory_data_t::cached_index cache_index;
        /* 2 values per node: flag for left and right */
        cache_index.node_match.resize(index->directory.num_nodes * 2);
        cache_index.virt_index = index;

        Deque<Pair<u32, szptr>> disco_nodes; /* node_idx, prefix match length */
        disco_nodes.push_back({0, 0});

        while(!disco_nodes.empty())
        {
            auto node_data = disco_nodes.front();
            currentNode    = lookup::GetNode(node_data.first, nodes);
            disco_nodes.pop_front();

            /* Check if leaf node matches the prefix */
            if(currentNode->is_leaf())
            {
                auto match_len = node_data.second;
                match_len +=
                    currentNode->longest_match(prefix, node_data.second);

                if(match_len >= prefixLen)
                    cache_index.node_match.at(node_data.first) = 1;

                continue;
            }

            auto match_len = node_data.second;
            auto prefix_match =
                currentNode->longest_match(prefix, node_data.second);

            match_len += currentNode->flags & node_base_t::prefix_directory
                             ? prefix_match + 1
                             : currentNode->flags & node_base_t::prefix_carry
                                   ? prefix_match
                                   : 0;

            /* !filter shortcircuits if-statement, very important */
            if(currentNode->node.left.valid())
                if(!filter || filter->node_match.at(currentNode->node.left))
                    disco_nodes.push_back({currentNode->node.left, match_len});
            if(currentNode->node.right.valid())
                if(!filter || filter->node_match.at(currentNode->node.right))
                    disco_nodes.push_back({currentNode->node.right, match_len});
        }

        if(SelectNodes(cache_index, nodes.data))
            cache_index.sub_root = GetFirstSelected(cache_index, nodes.data);

        return {cache_index, {}, strat};
    }

    return {{}, {}, strat};
}

} // namespace lookup

} // namespace dir_index

} // namespace VirtFS
} // namespace Coffee
