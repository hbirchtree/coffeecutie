#include <blam/elegy/animation.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#include <cstring>

namespace blam::elegy {
namespace {

/*! First byte of a codec stream; the Halo 2 set, values big-endian */
enum class codec_t : u8
{
    none,
    uncompressed_static,     /*!< static stream: one frame */
    uncompressed_animated,   /*!< fullframe, f32 x4 rotations */
    quantized_rotation_only, /*!< fullframe, i16 x4 rotations */
    byte_keyframe,           /*!< keyframes, u8 times */
    word_keyframe,           /*!< keyframes, u16 times */
    reverse_byte_keyframe,   /*!< same layout as byte_keyframe */
    reverse_word_keyframe,   /*!< same layout as word_keyframe */
    blend_screen,            /*!< fullframe, f32 x4 rotations */
};

/* Rotations follow the header, i16 x4 (/32767) unless noted; translations
 * f32 x3 and scales f32 at the offsets given, from the stream start */
struct codec_header_t
{
    codec_t type;
    u8      rotation_count;
    u8      translation_count;
    u8      scale_count;
    be<f32> error;
    be<f32> compression_rate;
    be<u32> translation_offset;
    be<u32> scale_offset;
};

static_assert(sizeof(codec_header_t) == 20);

/* Static and fullframe codecs: per node, every frame */
struct fullframe_header_t
{
    codec_header_t base;
    be<u32>        rotation_stride; /*!< Bytes per node; 0 in static streams */
    be<u32>        translation_stride;
    be<u32>        scale_stride;
};

static_assert(sizeof(fullframe_header_t) == 32);

/* Keyframe codecs: after the header one u32 per track (rotations, then
 * translations, then scales) of (first key << 12) | key count, indexing
 * that component's time table (u8 or u16 frames) and value table */
struct keyframe_header_t
{
    codec_header_t         base;
    std::array<be<u32>, 3> time_offset;  /*!< rotation, translation, scale */
    std::array<be<u32>, 3> value_offset; /*!< rotation, translation, scale */
    be<u32>                unknown_0;
};

static_assert(sizeof(keyframe_header_t) == 48);

struct reader_t
{
    Span<u8 const> data;
    bool               ok{true};

    template<typename T>
    T get(size_t at)
    {
        T out{};
        if(at + sizeof(T) > data.size())
        {
            ok = false;
            return out;
        }
        std::memcpy(&out, data.data() + at, sizeof(T));
        return out;
    }

    Quatf quantized(size_t at)
    {
        auto q = get<std::array<be<i16>, 4>>(at);
        return glm::normalize(Quatf(q[3].value() / 32767.f, q[0].value() / 32767.f, q[1].value() / 32767.f,
                                    q[2].value() / 32767.f));
    }

    Quatf full(size_t at)
    {
        auto q = get<std::array<be<f32>, 4>>(at);
        return glm::normalize(Quatf(q[3].value(), q[0].value(), q[1].value(), q[2].value()));
    }

    Vecf3 vec3(size_t at)
    {
        return get<be<Vecf3>>(at).value();
    }

    f32 scalar(size_t at)
    {
        return get<be<f32>>(at).value();
    }
};

/* One codec stream as tracks of values, each `frames` long */
struct tracks_t
{
    std::vector<std::vector<Quatf>> rotation;
    std::vector<std::vector<Vecf3>> translation;
    std::vector<std::vector<f32>>   scale;
};

Quatf same_hemisphere(Quatf const& q, Quatf const& ref)
{
    return glm::dot(q, ref) < 0.f ? -q : q;
}

template<typename T, typename Read, typename Lerp>
void keyframe_track(
    reader_t& in, u32 word, size_t times, size_t values, size_t value_size, bool wide, u32 frames,
    std::vector<T>& out, Read&& read, Lerp&& lerp)
{
    u32 const first = word >> 12, count = word & 0xFFF;
    out.resize(frames);
    if(count == 0)
    {
        in.ok = false;
        return;
    }
    auto time_of = [&](u32 k) -> u32 {
        size_t at = times + (first + k) * (wide ? 2 : 1);
        return wide ? in.get<be<u16>>(at).value() : in.get<u8>(at);
    };
    auto value_of = [&](u32 k) { return read(values + (first + k) * value_size); };
    u32  key      = 0;
    for(u32 f = 0; f < frames && in.ok; f++)
    {
        while(key + 1 < count && time_of(key + 1) <= f)
            key++;
        u32 const t0 = time_of(key);
        if(key + 1 >= count || f <= t0)
        {
            out[f] = value_of(key);
            continue;
        }
        u32 const t1 = time_of(key + 1);
        f32 const t  = static_cast<f32>(f - t0) / static_cast<f32>(std::max(t1 - t0, 1u));
        out[f]       = lerp(value_of(key), value_of(key + 1), t);
    }
}

std::optional<tracks_t> decode_stream(Span<u8 const> data, u32 frames)
{
    reader_t in{data};
    tracks_t out;
    auto     head = in.get<codec_header_t>(0);
    if(!in.ok)
        return std::nullopt;
    out.rotation.resize(head.rotation_count);
    out.translation.resize(head.translation_count);
    out.scale.resize(head.scale_count);
    auto const trans_at = static_cast<size_t>(head.translation_offset.value());
    auto const scale_at = static_cast<size_t>(head.scale_offset.value());

    auto slerp = [](Quatf const& a, Quatf const& b, f32 t) {
        return glm::normalize(glm::slerp(a, same_hemisphere(b, a), t));
    };
    auto mix3 = [](Vecf3 const& a, Vecf3 const& b, f32 t) { return glm::mix(a, b, t); };
    auto mix1 = [](f32 a, f32 b, f32 t) { return a + (b - a) * t; };

    switch(head.type)
    {
    case codec_t::none:
        return out;
    case codec_t::uncompressed_static:
    case codec_t::uncompressed_animated:
    case codec_t::quantized_rotation_only:
    case codec_t::blend_screen: {
        /* Node-major: every frame of a track, then the next track */
        auto       full   = in.get<fullframe_header_t>(0);
        bool const quant  = head.type == codec_t::uncompressed_static || head.type == codec_t::quantized_rotation_only;
        u32 const  rframe = quant ? 8 : 16;
        u32 const  count  = head.type == codec_t::uncompressed_static ? 1 : frames;
        auto       stride = [&](u32 s, u32 elem) { return s ? s : elem * count; };
        u32 const  rs     = stride(full.rotation_stride.value(), rframe);
        u32 const  ts     = stride(full.translation_stride.value(), 12);
        u32 const  ss     = stride(full.scale_stride.value(), 4);
        for(u32 i = 0; i < head.rotation_count; i++)
            for(u32 f = 0; f < count; f++)
            {
                size_t at = sizeof(fullframe_header_t) + i * rs + f * rframe;
                out.rotation[i].push_back(quant ? in.quantized(at) : in.full(at));
            }
        for(u32 i = 0; i < head.translation_count; i++)
            for(u32 f = 0; f < count; f++)
                out.translation[i].push_back(in.vec3(trans_at + i * ts + f * 12));
        for(u32 i = 0; i < head.scale_count; i++)
            for(u32 f = 0; f < count; f++)
                out.scale[i].push_back(in.scalar(scale_at + i * ss + f * 4));
        break;
    }
    case codec_t::byte_keyframe:
    case codec_t::word_keyframe:
    case codec_t::reverse_byte_keyframe:
    case codec_t::reverse_word_keyframe: {
        auto       key     = in.get<keyframe_header_t>(0);
        bool const wide    = head.type == codec_t::word_keyframe || head.type == codec_t::reverse_word_keyframe;
        size_t     word_at = sizeof(keyframe_header_t);
        auto       next    = [&] {
            u32 w = in.get<be<u32>>(word_at).value();
            word_at += 4;
            return w;
        };
        auto times  = [&](u32 c) { return static_cast<size_t>(key.time_offset[c].value()); };
        auto values = [&](u32 c) { return static_cast<size_t>(key.value_offset[c].value()); };
        for(auto& track : out.rotation)
            keyframe_track(
                in, next(), times(0), values(0), 8, wide, frames, track,
                [&](size_t at) { return in.quantized(at); }, slerp);
        for(auto& track : out.translation)
            keyframe_track(
                in, next(), times(1), values(1), 12, wide, frames, track, [&](size_t at) { return in.vec3(at); },
                mix3);
        for(auto& track : out.scale)
            keyframe_track(
                in, next(), times(2), values(2), 4, wide, frames, track, [&](size_t at) { return in.scalar(at); },
                mix1);
        break;
    }
    default:
        return std::nullopt;
    }
    if(!in.ok)
        return std::nullopt;
    return out;
}

/* Nodes of the set bits in one third of a flag group, in order; the bits
 * are in big-endian u32 words */
std::vector<u32> flagged_nodes(Span<u8 const> group, u32 part)
{
    std::vector<u32> nodes;
    size_t const     third = group.size() / 3;
    for(size_t i = 0; i < third * 8; i++)
        if(group[part * third + (i / 32) * 4 + 3 - (i % 32) / 8] & (1u << (i % 8)))
            nodes.push_back(static_cast<u32>(i));
    return nodes;
}

} // namespace

animation_t const* animation_graph_t::find(map_container const& map, std::string_view name) const
{
    for(auto const& animation : map.span(animations))
        if(map.string(animation.name) == name)
            return &animation;
    return nullptr;
}

std::vector<node_pose_t> animation_graph_t::default_pose(map_container const& map) const
{
    auto const               defaults = map.span(node_defaults);
    std::vector<node_pose_t> out(map.span(nodes).size());
    for(size_t i = 0; i < out.size() && i < defaults.size(); i++)
    {
        Vecf4 const r = defaults[i].rotation.value();
        out[i]        = {
                   .rotation    = glm::normalize(Quatf(r.w, r.x, r.y, r.z)),
                   .translation = defaults[i].translation,
                   .scale       = defaults[i].scale,
        };
    }
    return out;
}

std::vector<Matf4> animation_graph_t::node_matrices(map_container const& map, std::vector<node_pose_t> const& pose) const
{
    auto const         list = map.span(nodes);
    std::vector<Matf4> world(list.size(), Matf4(1.f));
    for(size_t i = 0; i < list.size() && i < pose.size(); i++)
    {
        Matf4 const local = glm::translate(Matf4(1.f), pose[i].translation) * glm::mat4_cast(pose[i].rotation) *
                            glm::scale(Matf4(1.f), Vecf3(pose[i].scale));
        i16 const parent = list[i].parent.value();
        world[i] = parent >= 0 && static_cast<size_t>(parent) < i ? world[static_cast<size_t>(parent)] * local : local;
    }
    return world;
}

std::optional<decoded_animation_t> decode_animation(
    resource_view const& view, animation_member_t const& member, Span<node_pose_t const> defaults)
{
    auto const data   = view.bytes(member.data);
    auto const& sizes = member.sizes;
    auto const frames = static_cast<u32>(std::max<i16>(member.frame_count, 1));
    auto const nodes  = static_cast<u32>(defaults.size());
    auto const stat   = static_cast<size_t>(std::max<i16>(sizes.default_data, 0));
    auto const anim_s = static_cast<size_t>(std::max<i32>(sizes.compressed, 0));
    size_t const flags_at = stat + anim_s;
    if(flags_at + sizes.static_node_flags + sizes.animated_node_flags > data.size())
        return std::nullopt;

    decoded_animation_t out{.frames = frames, .nodes = nodes};
    out.poses.resize(static_cast<size_t>(frames) * nodes);
    for(u32 f = 0; f < frames; f++)
        std::copy(defaults.begin(), defaults.end(), out.poses.begin() + f * nodes);

    /* The static stream holds one frame; the animated one every frame */
    auto apply = [&](Span<u8 const> stream, Span<u8 const> flags, bool animated) {
        if(stream.empty())
            return true;
        auto tracks = decode_stream(stream, animated ? frames : 1);
        if(!tracks)
            return false;
        auto rot   = flagged_nodes(flags, 0);
        auto trans = flagged_nodes(flags, 1);
        auto scale = flagged_nodes(flags, 2);
        if(rot.size() < tracks->rotation.size() || trans.size() < tracks->translation.size() ||
           scale.size() < tracks->scale.size())
            return false;
        auto value = [&](auto const& track, u32 f) { return track[std::min<size_t>(animated ? f : 0, track.size() - 1)]; };
        for(size_t i = 0; i < tracks->rotation.size(); i++)
            if(rot[i] < nodes)
                for(u32 f = 0; f < frames; f++)
                    out.poses[f * nodes + rot[i]].rotation = value(tracks->rotation[i], f);
        for(size_t i = 0; i < tracks->translation.size(); i++)
            if(trans[i] < nodes)
                for(u32 f = 0; f < frames; f++)
                    out.poses[f * nodes + trans[i]].translation = value(tracks->translation[i], f);
        for(size_t i = 0; i < tracks->scale.size(); i++)
            if(scale[i] < nodes)
                for(u32 f = 0; f < frames; f++)
                    out.poses[f * nodes + scale[i]].scale = value(tracks->scale[i], f);
        return true;
    };
    auto const static_flags   = data.subspan(flags_at, sizes.static_node_flags);
    auto const animated_flags = data.subspan(flags_at + sizes.static_node_flags, sizes.animated_node_flags);
    if(!apply(data.subspan(0, stat), static_flags, false) || !apply(data.subspan(stat, anim_s), animated_flags, true))
        return std::nullopt;
    return out;
}

std::optional<decoded_animation_t> decode_animation(
    map_container const&     map,
    resource_loader&         loader,
    animation_graph_t const& graph,
    animation_t const&       animation,
    Span<node_pose_t const>  defaults)
{
    auto const groups = map.span(graph.resource_groups);
    i16 const  group  = animation.resource_group;
    if(group < 0 || static_cast<size_t>(group) >= groups.size())
        return std::nullopt;
    auto view = loader.load(groups[static_cast<size_t>(group)].resource);
    if(!view.has_value())
        return std::nullopt;
    auto const* root = view->root_as<animation_resource_t>();
    if(!root)
        return std::nullopt;
    auto const members = view->block(root->members);
    i16 const  member  = animation.resource_group_member;
    if(member < 0 || static_cast<size_t>(member) >= members.size())
        return std::nullopt;
    auto const own = defaults.empty() ? graph.default_pose(map) : std::vector<node_pose_t>();
    return decode_animation(view.value(), members[static_cast<size_t>(member)], defaults.empty() ? Span<node_pose_t const>(own) : defaults);
}

std::vector<node_pose_t> default_pose(
    map_container const& map, animation_graph_t const& graph, Span<render_model_t const* const> models)
{
    auto       out   = graph.default_pose(map);
    auto const nodes = map.span(graph.nodes);
    std::vector<bool> posed(out.size());
    for(auto const* model : models)
    {
        auto const pose = elegy::default_pose(map, *model);
        auto const own  = map.span(model->nodes);
        for(size_t i = 0; i < nodes.size(); i++)
            for(size_t k = 0; k < own.size() && !posed[i]; k++)
                if(own[k].name.raw.value() == nodes[i].name.raw.value())
                {
                    out[i]   = pose[k];
                    posed[i] = true;
                }
    }
    return out;
}

std::vector<Matf4> skinning_matrices(
    map_container const& map, render_model_t const& model, Span<string_id const> names, Span<Matf4 const> frames)
{
    auto const nodes = map.span(model.nodes);
    auto       out   = node_matrices(map, model, default_pose(map, model));
    for(size_t i = 0; i < nodes.size(); i++)
    {
        auto const& node = nodes[i];
        for(size_t k = 0; k < names.size() && k < frames.size(); k++)
            if(names[k].raw.value() == node.name.raw.value())
            {
                out[i] = frames[k];
                break;
            }
        Matf4 inverse(1.f);
        inverse[0] = Vecf4(node.inverse_forward.value(), 0.f);
        inverse[1] = Vecf4(node.inverse_left.value(), 0.f);
        inverse[2] = Vecf4(node.inverse_up.value(), 0.f);
        inverse[3] = Vecf4(node.inverse_position.value(), 1.f);
        out[i]     = out[i] * inverse;
    }
    return out;
}

} // namespace blam::elegy
