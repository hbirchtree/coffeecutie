#pragma once

#include "structures.h"

#include <glm/gtc/quaternion.hpp>

#include <algorithm>
#include <array>
#include <optional>
#include <vector>

namespace blam::dimeter::anim {

/*!
 * jmad, the model animation graph. Layout from Assembly's Halo2Xbox
 * jmad.xml; the keyframes live outside the tag, in raw resource groups the
 * animations point into (16-byte aligned).
 *
 * An animation's data is, back to back with the sizes in data_sizes_t:
 * the static codec stream (default_data, one frame of the nodes that hold
 * still), the animated codec stream (compressed), static node flags,
 * animated node flags, then movement. Each flag group is three bit arrays
 * (rotation, translation, scale) of equal size, the nth set bit naming the
 * model node of the nth track. Same codecs and layout as Halo 3; verified
 * over every animation of the Xbox maps.
 */

struct alignas(4) skeleton_node_t
{
    string_id                    name;
    block_index<skeleton_node_t> next_sibling;
    block_index<skeleton_node_t> first_child;
    block_index<skeleton_node_t> parent;
    u8                           model_flags; /* TODO: flags */
    u8                           joint_flags; /* TODO: flags */
    Vecf3                        base_vector;
    f32                          vector_range;
    f32                          z_position;
};

static_assert(sizeof(skeleton_node_t) == 0x20);

enum class animation_type_t : u8
{
    base,
    overlay,
    replacement,
};

/*! Root motion stored with the frames: f32 per frame of each, so 8, 12
 * or 16 bytes a frame */
enum class frame_info_t : u8
{
    none,
    dx_dy,
    dx_dy_dyaw,
    dx_dy_dz_dyaw,
};

struct alignas(2) frame_event_t
{
    i16 type;
    i16 frame;
};

struct alignas(4) sound_event_t
{
    i16       sound;
    i16       frame;
    string_id marker;
};

struct alignas(2) effect_event_t
{
    i16 effect;
    i16 frame;
};

struct alignas(4) object_space_parent_t
{
    i16                 node;
    u16                 component_flags; /* TODO: flags */
    std::array<i16, 4>  rotation;
    Vecf3               translation;
    f32                 scale;
};

static_assert(sizeof(object_space_parent_t) == 0x1C);

/*! First byte of a codec stream */
enum class codec_t : u8
{
    none,
    uncompressed_static,      /*!< static stream: one frame */
    uncompressed_animated,    /*!< fullframe, f32 x4 rotations */
    quantized_rotation_only,  /*!< fullframe, i16 x4 rotations */
    byte_keyframe,            /*!< keyframes, u8 times */
    word_keyframe,            /*!< keyframes, u16 times */
    reverse_byte_keyframe,    /*!< same layout as byte_keyframe */
    reverse_word_keyframe,    /*!< same layout as word_keyframe */
    blend_screen,             /*!< fullframe, f32 x4 rotations */
};

/*!
 * Common head of a codec stream. Rotations follow the header, i16 x4
 * (/32767) unless noted; translations f32 x3 and scales f32 sit at the
 * offsets given, all relative to the stream start.
 */
struct alignas(4) codec_header_t
{
    codec_t type;
    u8      rotation_count;
    u8      translation_count;
    u8      scale_count;
    f32     error;
    f32     compression_rate;
    u32     translation_offset;
    u32     scale_offset;
};

static_assert(sizeof(codec_header_t) == 20);

/*! Static and fullframe codecs: per node, every frame */
struct alignas(4) fullframe_header_t
{
    codec_header_t base;
    u32            rotation_stride; /*!< Bytes per node; 0 in static streams */
    u32            translation_stride;
    u32            scale_stride;
};

static_assert(sizeof(fullframe_header_t) == 32);

/*!
 * Keyframe codecs: after the header one u32 per track (rotations, then
 * translations, then scales) of (first key << 12) | key count, indexing
 * that component's time table (u8 or u16 frames, from 0, ascending) and
 * value table.
 */
struct alignas(4) keyframe_header_t
{
    codec_header_t     base;
    std::array<u32, 3> time_offset;  /*!< rotation, translation, scale */
    std::array<u32, 3> value_offset; /*!< rotation, translation, scale */
    u32                unknown_0;
};

static_assert(sizeof(keyframe_header_t) == 48);

/*! Byte sizes of the sections of an animation's data; static streams with
 * translations or scales are padded to 16 bytes */
struct alignas(4) data_sizes_t
{
    u8  static_node_flags;
    u8  animated_node_flags;
    i16 movement;
    i16 pill_offset;
    i16 default_data; /*!< The static codec stream */
    i32 uncompressed;
    i32 compressed; /*!< The animated codec stream */
};

static_assert(sizeof(data_sizes_t) == 0x10);

struct alignas(4) animation_t
{
    string_id                         name;
    i32                               node_list_checksum;
    i32                               production_checksum;
    i32                               import_checksum;
    animation_type_t                  type;
    frame_info_t                      frame_info;
    i8                                blend_screen;
    u8                                node_count;
    i16                               frame_count_raw;
    u8                                internal_flags;   /* TODO: flags */
    u8                                production_flags; /* TODO: flags */
    u16                               playback_flags;   /* TODO: flags */
    u8                                desired_compression;
    u8                                current_compression;
    f32                               weight;
    i32                               unknown_0;
    i16                               resource_group_raw;
    i16                               unknown_1;
    i32                               resource_group_offset_raw;
    i16                               unknown_2;
    i16                               loop_frame;
    block_index<animation_t>          parent_animation;
    block_index<animation_t>          next_animation;
    std::array<u8, 8>                 unknown_3;
    data_sizes_t                      data_sizes;
    tag_block<frame_event_t>          frame_events;
    tag_block<sound_event_t>          sound_events;
    tag_block<effect_event_t>         effect_events;
    tag_block<object_space_parent_t>  object_space_parents;

    inline i16 frame_count() const
    {
        return from_le(frame_count_raw);
    }

    inline i16 resource_group() const
    {
        return from_le(resource_group_raw);
    }

    /*! Where this animation's data starts inside its resource group */
    inline i32 resource_group_offset() const
    {
        return from_le(resource_group_offset_raw);
    }
};

static_assert(offsetof(animation_t, resource_group_raw) == 0x24);
static_assert(offsetof(animation_t, frame_events) == 0x4C);
static_assert(sizeof(animation_t) == 0x6C);

struct alignas(4) node_data_t
{
    string_id name;
    Vecf4     rotation;
    Vecf3     translation;
    f32       scale;
    Vecf3     min_bounds;
    Vecf3     max_bounds;
};

static_assert(sizeof(node_data_t) == 0x3C);

/*! A raw blob holding the data of several animations */
struct alignas(4) resource_group_t
{
    datum_index owner;
    i32         size_raw;
    raw_ref     data;
    i32         unknown_0;
    i32         unknown_1;

    inline i32 size() const
    {
        return from_le(size_raw);
    }
};

static_assert(sizeof(resource_group_t) == 0x14);

struct alignas(4) header_t
{
    tag_reference<tag_class_t::jmad> parent;
    u8                               inheritance_flags; /* TODO: flags */
    u8                               private_flags;     /* TODO: flags */
    i16                              codec_pack;
    tag_block<skeleton_node_t>       skeleton;
    tag_block<u8>                    sounds;        /*!< elem 0xC */
    tag_block<u8>                    effects;       /*!< elem 0xC */
    tag_block<u8>                    blend_screens; /*!< elem 0x1C */
    tag_block<animation_t>           animations;
    tag_block<u8>                    modes;             /*!< elem 0x14 */
    tag_block<u8>                    vehicle_suspension; /*!< elem 0x28 */
    tag_block<u8>                    object_overlays;   /*!< elem 0x14 */
    tag_block<u8>                    inheritance;       /*!< elem 0x20 */
    tag_block<u8>                    weapons;           /*!< elem 0x8 */
    std::array<u32, 8>               left_arm_nodes;
    std::array<u32, 8>               right_arm_nodes;
    data_reference<>                 last_import_results;
    tag_block<node_data_t>           node_data;
    tag_block<resource_group_t>      resource_groups;
    tag_block<u8>                    unknown_0; /*!< elem 0x18 */
};

static_assert(offsetof(header_t, animations) == 0x2C);
static_assert(offsetof(header_t, left_arm_nodes) == 0x5C);
static_assert(offsetof(header_t, node_data) == 0xA4);
static_assert(sizeof(header_t) == 0xBC);

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
};

namespace detail {

struct reader_t
{
    Span<const u8> data;
    bool                     ok{true};

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

    inline Quatf quantized(size_t at)
    {
        auto  q = get<std::array<i16, 4>>(at);
        Quatf r(
            from_le(q[3]) / 32767.f,
            from_le(q[0]) / 32767.f,
            from_le(q[1]) / 32767.f,
            from_le(q[2]) / 32767.f);
        return glm::normalize(r);
    }

    inline Quatf full(size_t at)
    {
        auto q = get<std::array<f32, 4>>(at);
        return glm::normalize(
            Quatf(from_le(q[3]), from_le(q[0]), from_le(q[1]), from_le(q[2])));
    }

    inline Vecf3 vec3(size_t at)
    {
        auto v = get<std::array<f32, 3>>(at);
        return Vecf3(from_le(v[0]), from_le(v[1]), from_le(v[2]));
    }

    inline f32 scalar(size_t at)
    {
        return from_le(get<f32>(at));
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

template<typename T, typename Read, typename Lerp>
inline void keyframe_track(
    reader_t&          in,
    u32                word,
    size_t             times,
    size_t             values,
    size_t             value_size,
    bool               wide,
    u32                frames,
    std::vector<T>&    out,
    Read&&             read,
    Lerp&&             lerp)
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
        return wide ? from_le(in.get<u16>(at)) : in.get<u8>(at);
    };
    auto value_of = [&](u32 k) { return read(values + (first + k) * value_size); };
    u32 key = 0;
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

inline std::optional<tracks_t> decode_stream(Span<const u8> data, u32 frames)
{
    reader_t in{data};
    tracks_t out;
    auto     head = in.get<codec_header_t>(0);
    if(!in.ok)
        return std::nullopt;
    out.rotation.resize(head.rotation_count);
    out.translation.resize(head.translation_count);
    out.scale.resize(head.scale_count);
    auto const trans_at = static_cast<size_t>(from_le(head.translation_offset));
    auto const scale_at = static_cast<size_t>(from_le(head.scale_offset));

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
        bool const quant  = head.type == codec_t::uncompressed_static ||
                           head.type == codec_t::quantized_rotation_only;
        u32 const  rframe = quant ? 8 : 16;
        u32 const  count  = head.type == codec_t::uncompressed_static ? 1 : frames;
        auto       stride = [&](u32 s, u32 elem) { return s ? s : elem * count; };
        u32 const  rs     = stride(from_le(full.rotation_stride), rframe);
        u32 const  ts     = stride(from_le(full.translation_stride), 12);
        u32 const  ss     = stride(from_le(full.scale_stride), 4);
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
        auto       key  = in.get<keyframe_header_t>(0);
        bool const wide = head.type == codec_t::word_keyframe ||
                          head.type == codec_t::reverse_word_keyframe;
        size_t word_at  = sizeof(keyframe_header_t);
        auto   next     = [&] {
            u32 w = from_le(in.get<u32>(word_at));
            word_at += 4;
            return w;
        };
        auto offset = [&](std::array<u32, 3> const& o, u32 c) {
            return static_cast<size_t>(from_le(o[c]));
        };
        for(auto& track : out.rotation)
            keyframe_track(
                in, next(), offset(key.time_offset, 0), offset(key.value_offset, 0),
                8, wide, frames, track,
                [&](size_t at) { return in.quantized(at); }, slerp);
        for(auto& track : out.translation)
            keyframe_track(
                in, next(), offset(key.time_offset, 1), offset(key.value_offset, 1),
                12, wide, frames, track,
                [&](size_t at) { return in.vec3(at); }, mix3);
        for(auto& track : out.scale)
            keyframe_track(
                in, next(), offset(key.time_offset, 2), offset(key.value_offset, 2),
                4, wide, frames, track,
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

/*! Nodes of the set bits in one third of a flag group, in order */
inline std::vector<u32> flagged_nodes(Span<const u8> group, u32 part)
{
    std::vector<u32> nodes;
    size_t const     third = group.size() / 3;
    for(size_t i = 0; i < third * 8; i++)
        if(group[part * third + i / 8] & (1u << (i % 8)))
            nodes.push_back(static_cast<u32>(i));
    return nodes;
}

} // namespace detail

/*!
 * Decodes an animation from `data`, its bytes in the resource group from
 * its offset on. Nodes no track names keep `defaults` (one per node).
 */
inline std::optional<decoded_t> decode(
    animation_t const&                 anim,
    Span<const u8>           data,
    Span<node_pose_t const> defaults)
{
    auto const& sizes  = anim.data_sizes;
    auto const  frames = static_cast<u32>(std::max<i16>(anim.frame_count(), 1));
    auto const  nodes  = static_cast<u32>(defaults.size());
    auto const  stat   = static_cast<size_t>(std::max<i16>(from_le(sizes.default_data), 0));
    auto const  anim_s = static_cast<size_t>(std::max<i32>(from_le(sizes.compressed), 0));
    size_t const flags_at = stat + anim_s;
    size_t const flags_end =
        flags_at + sizes.static_node_flags + sizes.animated_node_flags;
    if(flags_end > data.size())
        return std::nullopt;

    decoded_t out{.frames = frames, .nodes = nodes};
    out.poses.resize(static_cast<size_t>(frames) * nodes);
    out.tracked.assign(nodes, 0);
    for(u32 f = 0; f < frames; f++)
        for(u32 n = 0; n < nodes; n++)
            out.poses[f * nodes + n] = defaults[n];

    /* The static stream holds one frame; the animated one every frame */
    auto apply = [&](Span<const u8> stream,
                     Span<const u8> flags,
                     bool                     animated) {
        if(stream.empty())
            return true;
        auto tracks = detail::decode_stream(stream, animated ? frames : 1);
        if(!tracks)
            return false;
        auto rot   = detail::flagged_nodes(flags, 0);
        auto trans = detail::flagged_nodes(flags, 1);
        auto scale = detail::flagged_nodes(flags, 2);
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
                    out.poses[f * nodes + trans[i]].translation =
                        value(tracks->translation[i], f);
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
    auto const static_flags   = data.subspan(flags_at, sizes.static_node_flags);
    auto const animated_flags = data.subspan(
        flags_at + sizes.static_node_flags, sizes.animated_node_flags);
    if(!apply(data.subspan(0, stat), static_flags, false) ||
       !apply(data.subspan(stat, anim_s), animated_flags, true))
        return std::nullopt;
    return out;
}

} // namespace blam::dimeter::anim
