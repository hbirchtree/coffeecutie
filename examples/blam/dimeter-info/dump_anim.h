#pragma once

#include "dump.h"

#include <blam/dimeter/animation.h>

namespace dimeter_info {

namespace anim = blam::dimeter::anim;

/* jmad: skeleton, animations and where their raw data lives */
template<typename V>
inline void dump_jmad(
    h2::map_container<V> const& map,
    h2::raw_pool const&         pool,
    h2::tag_t const&            tag)
{
    auto const* graph = tag.template data<anim::header_t>(map.magic());
    if(!graph)
        return;
    fmt::print(
        "  parent {} codec pack {}\n",
        tag_label(map, graph->parent),
        from_le(graph->codec_pack));
    if(auto nodes = graph->skeleton.data(map.magic()); !nodes.has_error())
        for(auto const& [i, n] : stl_types::enumerate(nodes.value()))
            fmt::print(
                "  node {} {} parent={} child={} sibling={}\n",
                i,
                map.string(n.name),
                n.parent.value(),
                n.first_child.value(),
                n.next_sibling.value());
    auto groups = graph->resource_groups.data(map.magic());
    if(!groups.has_error())
        for(auto const& [i, g] : stl_types::enumerate(groups.value()))
            fmt::print(
                "  resource group {}: {}+{:#x} {} bytes\n",
                i,
                raw_file_name(g.data),
                g.data.offset(),
                g.size());
    auto anims = graph->animations.data(map.magic());
    if(anims.has_error())
        return;
    for(auto const& [i, a] : stl_types::enumerate(anims.value()))
    {
        fmt::print(
            "  animation {} {}: type={} frame_info={} nodes={} frames={} "
            "compression={}/{} group={} offset={:#x} loop={}\n"
            "    sizes: static flags {} animated flags {} movement {} pill {} "
            "default {} uncompressed {} compressed {}\n",
            i,
            map.string(a.name),
            static_cast<u8>(a.type),
            static_cast<u8>(a.frame_info),
            a.node_count,
            a.frame_count(),
            a.desired_compression,
            a.current_compression,
            a.resource_group(),
            a.resource_group_offset(),
            from_le(a.loop_frame),
            a.data_sizes.static_node_flags,
            a.data_sizes.animated_node_flags,
            from_le(a.data_sizes.movement),
            from_le(a.data_sizes.pill_offset),
            from_le(a.data_sizes.default_data),
            from_le(a.data_sizes.uncompressed),
            from_le(a.data_sizes.compressed));
        i16 gi = a.resource_group();
        if(groups.has_error() || gi < 0 ||
           static_cast<size_t>(gi) >= groups.value().size())
            continue;
        auto const& g = groups.value()[gi];
        auto raw = pool.resolve(
            map.magic(), &g, g.data, static_cast<u32>(std::max(g.size(), 0)));
        if(!raw)
            continue;
        auto at = static_cast<size_t>(std::max(a.resource_group_offset(), 0));
        if(at >= raw->size())
            continue;
        auto bytes = semantic::Span<const u8>(
            reinterpret_cast<u8 const*>(raw->data()), raw->size()).subspan(at);
        bytes      = bytes.subspan(0, std::min<size_t>(bytes.size(), 64));
        fmt::print("    data {:02x}\n", fmt::join(bytes, " "));
    }
}

} // namespace dimeter_info
