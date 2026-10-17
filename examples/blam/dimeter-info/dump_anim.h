#pragma once

#include "dump.h"

#include <blam/dimeter/animation.h>

#include <algorithm>

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

/* Decodes every animation; per codec of the animated stream, how many
 * decode and how smooth they are (wrong offsets read as noise) */
struct anim_check_t
{
    struct codec_stats_t
    {
        u32              decoded{0};
        u32              failed{0};
        f32              max_translation{0.f};
        std::vector<f32> steps; /*!< Largest frame-to-frame turn, degrees */
    };
    std::array<codec_stats_t, 9> codecs{};
    std::vector<std::string>     failures;
};

/* jmad's own default pose, one per skeleton node */
inline std::vector<anim::node_pose_t> jmad_defaults(
    auto const& map, anim::header_t const& graph, size_t nodes)
{
    std::vector<anim::node_pose_t> out(nodes);
    if(auto data = graph.node_data.data(map.magic()); !data.has_error())
        for(size_t i = 0; i < nodes && i < data.value().size(); i++)
        {
            auto const& d = data.value()[i];
            out[i]        = {
                       .rotation = glm::normalize(typing::vector_types::Quatf(
                    from_le(d.rotation.w),
                    from_le(d.rotation.x),
                    from_le(d.rotation.y),
                    from_le(d.rotation.z))),
                       .translation = typing::vector_types::Vecf3(
                    from_le(d.translation.x),
                    from_le(d.translation.y),
                    from_le(d.translation.z)),
                       .scale = from_le(d.scale),
            };
        }
    return out;
}

template<typename V>
inline void check_jmad(
    h2::map_container<V> const& map,
    h2::raw_pool const&         pool,
    h2::tag_t const&            tag,
    anim_check_t&               out)
{
    auto const* graph = tag.template data<anim::header_t>(map.magic());
    if(!graph)
        return;
    auto skeleton = graph->skeleton.data(map.magic());
    auto groups   = graph->resource_groups.data(map.magic());
    auto anims    = graph->animations.data(map.magic());
    if(skeleton.has_error() || groups.has_error() || anims.has_error())
        return;
    auto const defaults = jmad_defaults(map, *graph, skeleton.value().size());
    for(auto const& a : anims.value())
    {
        i16 gi = a.resource_group();
        if(gi < 0 || static_cast<size_t>(gi) >= groups.value().size())
            continue;
        auto const& g   = groups.value()[gi];
        auto        raw = pool.resolve(
            map.magic(), &g, g.data, static_cast<u32>(std::max(g.size(), 0)));
        auto at = static_cast<size_t>(std::max(a.resource_group_offset(), 0));
        if(!raw || at >= raw->size())
            continue;
        auto bytes = semantic::Span<const u8>(
            reinterpret_cast<u8 const*>(raw->data()), raw->size()).subspan(at);
        auto stat  = static_cast<size_t>(std::max<i16>(from_le(a.data_sizes.default_data), 0));
        u8   codec = stat < bytes.size() ? bytes[stat] : 0;
        auto& c    = out.codecs[std::min<size_t>(codec, 8)];
        auto  dec  = anim::decode(a, bytes, defaults);
        if(!dec)
        {
            c.failed++;
            if(out.failures.size() < 20)
                out.failures.push_back(fmt::format(
                    "{} / {} (codec {})", map.name_of(tag), map.string(a.name), codec));
            continue;
        }
        c.decoded++;
        f32 step = 0.f;
        for(u32 n = 0; n < dec->nodes; n++)
        {
            for(u32 f = 0; f < dec->frames; f++)
                c.max_translation =
                    std::max(c.max_translation, glm::length(dec->at(f, n).translation));
            if(!(dec->tracked[n] & static_cast<u8>(anim::track_t::rotation)))
                continue;
            for(u32 f = 1; f < dec->frames; f++)
            {
                f32 d = std::abs(glm::dot(dec->at(f, n).rotation, dec->at(f - 1, n).rotation));
                step  = std::max(step, glm::degrees(2.f * std::acos(std::min(d, 1.f))));
            }
        }
        c.steps.push_back(step);
    }
}

inline void print_anim_check(anim_check_t& check)
{
    for(auto const& [i, c] : stl_types::enumerate(check.codecs))
    {
        if(c.decoded + c.failed == 0)
            continue;
        std::sort(c.steps.begin(), c.steps.end());
        auto pct = [&](f32 p) {
            return c.steps.empty() ? 0.f : c.steps[static_cast<size_t>(p * (c.steps.size() - 1))];
        };
        fmt::print(
            "codec {}: {} decoded, {} failed, max translation {:.2f}, "
            "frame step deg p50 {:.1f} p95 {:.1f} max {:.1f}\n",
            i,
            c.decoded,
            c.failed,
            c.max_translation,
            pct(.5f),
            pct(.95f),
            pct(1.f));
    }
    for(auto const& f : check.failures)
        fmt::print("  failed: {}\n", f);
}

} // namespace dimeter_info
