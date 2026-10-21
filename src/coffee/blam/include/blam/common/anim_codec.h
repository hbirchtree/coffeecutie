#pragma once

#include "endian.h"

#include <glm/gtc/quaternion.hpp>
#include <peripherals/semantic/chunk.h>
#include <peripherals/typing/vectors/glm_vector_types.h>

#include <algorithm>
#include <array>
#include <bit>
#include <cstring>
#include <optional>
#include <vector>

/*!
 * The Halo 2 and 3 animation codecs (jmad). An animation's data is, back to
 * back: the static codec stream (one frame of the nodes that hold still),
 * the animated codec stream, static node flags, animated node flags. Each
 * flag group is three bit arrays (rotation, translation, scale) of equal
 * size, the nth set bit naming the node of the nth track. Halo 2 stores it
 * little-endian, Halo 3 big-endian; nothing else differs.
 */
namespace blam::anim {

using libc_types::f32;
using libc_types::i16;
using libc_types::u32;
using libc_types::u8;
using semantic::Span;
using typing::vector_types::Quatf;
using typing::vector_types::Vecf3;

/*! First byte of a codec stream */
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

/*!
 * Common head of a codec stream. Rotations follow the header, i16 x4
 * (/32767) unless noted; translations f32 x3 and scales f32 sit at the
 * offsets given, all relative to the stream start.
 */
template<std::endian E>
struct codec_header_t
{
    codec_t               type;
    u8                    rotation_count;
    u8                    translation_count;
    u8                    scale_count;
    endian_value<f32, E>  error;
    endian_value<f32, E>  compression_rate;
    endian_value<u32, E>  translation_offset;
    endian_value<u32, E>  scale_offset;
};

/*! Static and fullframe codecs: per node, every frame */
template<std::endian E>
struct fullframe_header_t
{
    codec_header_t<E>    base;
    endian_value<u32, E> rotation_stride; /*!< Bytes per node; 0 in static streams */
    endian_value<u32, E> translation_stride;
    endian_value<u32, E> scale_stride;
};

/*!
 * Keyframe codecs: after the header one u32 per track (rotations, then
 * translations, then scales) of (first key << 12) | key count, indexing
 * that component's time table (u8 or u16 frames, from 0, ascending) and
 * value table.
 */
template<std::endian E>
struct keyframe_header_t
{
    codec_header_t<E>                   base;
    std::array<endian_value<u32, E>, 3> time_offset;  /*!< rotation, translation, scale */
    std::array<endian_value<u32, E>, 3> value_offset; /*!< rotation, translation, scale */
    endian_value<u32, E>                unknown_0;
};

static_assert(sizeof(codec_header_t<std::endian::little>) == 20);
static_assert(sizeof(fullframe_header_t<std::endian::little>) == 32);
static_assert(sizeof(keyframe_header_t<std::endian::little>) == 48);

/*! A node's local transform, relative to its parent */
struct node_pose_t
{
    Quatf rotation{1.f, 0.f, 0.f, 0.f};
    Vecf3 translation{0.f};
    f32   scale{1.f};
};

enum class track_t : u8
{
    rotation    = 0x1,
    translation = 0x2,
    scale       = 0x4,
};

/*! Every frame of an animation, frame-major */
struct decoded_t
{
    u32                      frames{0};
    u32                      nodes{0};
    std::vector<node_pose_t> poses;
    std::vector<u8>          tracked; /*!< Per node, the track_t it animates */

    inline node_pose_t const& at(u32 frame, u32 node) const
    {
        return poses[frame * nodes + node];
    }

    inline Span<node_pose_t const> frame(u32 index) const
    {
        return Span<node_pose_t const>(poses).subspan(std::min(index, frames - 1) * nodes, nodes);
    }
};

/*! Where an animation's sections end, from its data_sizes */
struct data_layout_t
{
    u32    frames;
    size_t static_size;   /*!< The static codec stream */
    size_t animated_size; /*!< The animated codec stream */
    u8     static_flags;
    u8     animated_flags;
};

namespace detail {

template<std::endian E>
struct reader_t
{
    Span<const u8> data;
    bool           ok{true};

    template<typename T>
    inline T get(size_t at)
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

    template<typename T>
    inline T value(size_t at)
    {
        return from_endian<E>(get<T>(at));
    }

    inline Quatf quantized(size_t at)
    {
        auto q = get<std::array<i16, 4>>(at);
        return glm::normalize(Quatf(
            from_endian<E>(q[3]) / 32767.f,
            from_endian<E>(q[0]) / 32767.f,
            from_endian<E>(q[1]) / 32767.f,
            from_endian<E>(q[2]) / 32767.f));
    }

    inline Quatf full(size_t at)
    {
        auto q = get<std::array<f32, 4>>(at);
        return glm::normalize(Quatf(
            from_endian<E>(q[3]), from_endian<E>(q[0]), from_endian<E>(q[1]), from_endian<E>(q[2])));
    }

    inline Vecf3 vec3(size_t at)
    {
        auto v = get<std::array<f32, 3>>(at);
        return Vecf3(from_endian<E>(v[0]), from_endian<E>(v[1]), from_endian<E>(v[2]));
    }

    inline f32 scalar(size_t at)
    {
        return value<f32>(at);
    }
};

/*! One codec stream as tracks of values, each `frames` long */
struct tracks_t
{
    std::vector<std::vector<Quatf>> rotation;
    std::vector<std::vector<Vecf3>> translation;
    std::vector<std::vector<f32>>   scale;
};

inline Quatf same_hemisphere(Quatf const& q, Quatf const& ref)
{
    return glm::dot(q, ref) < 0.f ? -q : q;
}

template<std::endian E, typename T, typename Read, typename Lerp>
inline void keyframe_track(
    reader_t<E>&    in,
    u32             word,
    size_t          times,
    size_t          values,
    size_t          value_size,
    bool            wide,
    u32             frames,
    std::vector<T>& out,
    Read&&          read,
    Lerp&&          lerp)
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
        return wide ? in.template value<libc_types::u16>(at) : in.template get<u8>(at);
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

template<std::endian E>
inline std::optional<tracks_t> decode_stream(Span<const u8> data, u32 frames)
{
    reader_t<E> in{data};
    tracks_t    out;
    auto        head = in.template get<codec_header_t<E>>(0);
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
        auto       full   = in.template get<fullframe_header_t<E>>(0);
        bool const quant  = head.type == codec_t::uncompressed_static ||
                           head.type == codec_t::quantized_rotation_only;
        u32 const  rframe = quant ? 8 : 16;
        u32 const  count  = head.type == codec_t::uncompressed_static ? 1 : frames;
        auto       stride = [&](u32 s, u32 elem) { return s ? s : elem * count; };
        u32 const  rs     = stride(full.rotation_stride.value(), rframe);
        u32 const  ts     = stride(full.translation_stride.value(), 12);
        u32 const  ss     = stride(full.scale_stride.value(), 4);
        for(u32 i = 0; i < head.rotation_count; i++)
            for(u32 f = 0; f < count; f++)
            {
                size_t at = sizeof(fullframe_header_t<E>) + i * rs + f * rframe;
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
        auto       key     = in.template get<keyframe_header_t<E>>(0);
        bool const wide    = head.type == codec_t::word_keyframe ||
                          head.type == codec_t::reverse_word_keyframe;
        size_t     word_at = sizeof(keyframe_header_t<E>);
        auto       next    = [&] {
            u32 w = in.template value<u32>(word_at);
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
                in, next(), times(1), values(1), 12, wide, frames, track,
                [&](size_t at) { return in.vec3(at); }, mix3);
        for(auto& track : out.scale)
            keyframe_track(
                in, next(), times(2), values(2), 4, wide, frames, track,
                [&](size_t at) { return in.scalar(at); }, mix1);
        break;
    }
    default:
        return std::nullopt;
    }
    if(!in.ok)
        return std::nullopt;
    return out;
}

/*! Nodes of the set bits in one third of a flag group, in order; the bits
 * are in u32 words of byte order E */
template<std::endian E>
inline std::vector<u32> flagged_nodes(Span<const u8> group, u32 part)
{
    std::vector<u32> nodes;
    size_t const     third = group.size() / 3;
    for(size_t i = 0; i < third * 8; i++)
    {
        size_t const in_word = (i % 32) / 8;
        size_t const byte = (i / 32) * 4 + (E == std::endian::big ? 3 - in_word : in_word);
        if(group[part * third + byte] & (1u << (i % 8)))
            nodes.push_back(static_cast<u32>(i));
    }
    return nodes;
}

} // namespace detail

/*!
 * Decodes an animation from `data`, its bytes from its own start on. Nodes
 * no track names keep `defaults` (one per node).
 */
template<std::endian E>
inline std::optional<decoded_t> decode_clip(
    Span<const u8> data, data_layout_t const& layout, Span<node_pose_t const> defaults)
{
    auto const   frames   = layout.frames;
    auto const   nodes    = static_cast<u32>(defaults.size());
    size_t const flags_at = layout.static_size + layout.animated_size;
    if(flags_at + layout.static_flags + layout.animated_flags > data.size())
        return std::nullopt;

    decoded_t out{.frames = frames, .nodes = nodes};
    out.poses.resize(static_cast<size_t>(frames) * nodes);
    out.tracked.assign(nodes, 0);
    for(u32 f = 0; f < frames; f++)
        std::copy(defaults.begin(), defaults.end(), out.poses.begin() + f * nodes);

    /* The static stream holds one frame; the animated one every frame */
    auto apply = [&](Span<const u8> stream, Span<const u8> flags, bool animated) {
        if(stream.empty())
            return true;
        auto tracks = detail::decode_stream<E>(stream, animated ? frames : 1);
        if(!tracks)
            return false;
        auto rot   = detail::flagged_nodes<E>(flags, 0);
        auto trans = detail::flagged_nodes<E>(flags, 1);
        auto scale = detail::flagged_nodes<E>(flags, 2);
        if(rot.size() < tracks->rotation.size() ||
           trans.size() < tracks->translation.size() ||
           scale.size() < tracks->scale.size())
            return false;
        auto value = [&](auto const& track, u32 f) {
            return track[std::min<size_t>(animated ? f : 0, track.size() - 1)];
        };
        for(size_t i = 0; i < tracks->rotation.size(); i++)
            if(rot[i] < nodes)
            {
                out.tracked[rot[i]] |= static_cast<u8>(track_t::rotation);
                for(u32 f = 0; f < frames; f++)
                    out.poses[f * nodes + rot[i]].rotation = value(tracks->rotation[i], f);
            }
        for(size_t i = 0; i < tracks->translation.size(); i++)
            if(trans[i] < nodes)
            {
                out.tracked[trans[i]] |= static_cast<u8>(track_t::translation);
                for(u32 f = 0; f < frames; f++)
                    out.poses[f * nodes + trans[i]].translation = value(tracks->translation[i], f);
            }
        for(size_t i = 0; i < tracks->scale.size(); i++)
            if(scale[i] < nodes)
            {
                out.tracked[scale[i]] |= static_cast<u8>(track_t::scale);
                for(u32 f = 0; f < frames; f++)
                    out.poses[f * nodes + scale[i]].scale = value(tracks->scale[i], f);
            }
        return true;
    };
    auto const static_flags   = data.subspan(flags_at, layout.static_flags);
    auto const animated_flags = data.subspan(flags_at + layout.static_flags, layout.animated_flags);
    if(!apply(data.subspan(0, layout.static_size), static_flags, false) ||
       !apply(data.subspan(layout.static_size, layout.animated_size), animated_flags, true))
        return std::nullopt;
    return out;
}

} // namespace blam::anim
