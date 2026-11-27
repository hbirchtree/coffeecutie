#pragma once

#include "../common/anim_clip.h"
#include "blam_antr.h"

#include <algorithm>
#include <array>
#include <optional>

#include <utility>
#include <vector>

/*!
 * Halo 1 antr sampling: one frame of a clip's node channels as node-local
 * rotations and translations. Rotations come out conjugated, the same
 * convention as the mod2 bind quats. `scale`, when given, gets each node's
 * uniform scale.
 */
namespace blam::antr {

using semantic::Span;

/* Cutscene clips: keyframed channels in a block of frame_data at
 * compressed_data_offset, led by the offsets of their streams */
bool sample_compressed(
    animation const&    anim,
    Span<const u8>      frame_bytes,
    u32                 frame,
    u32                 node_count,
    std::vector<Quatf>& rot,
    std::vector<Vecf3>& trans,
    std::vector<f32>*   scale = nullptr);

/*!
 * An uncompressed clip's nodes at `frame`. Interleaved by node: each emits
 * [rotation 8B][translation 12B][scale 4B], animated channels from
 * frame_data and the rest from default_data.
 */
bool sample_uncompressed(
    animation const&    anim,
    Span<const u8>      default_bytes,
    Span<const u8>      frame_bytes,
    u32                 frame,
    u32                 node_count,
    std::vector<Quatf>& rot,
    std::vector<Vecf3>& trans,
    std::vector<f32>*   scale = nullptr);

/*! A frame of the clip's root motion: a move in the unit's space, and a turn */
std::pair<Vecf3, f32> root_motion(animation const& clip, Span<const u8> frame_info, i32 frame);

/* A clip as the animation system reads it (common/anim_clip.h) */

inline u32 frame_count(animation const& clip)
{
    return static_cast<u32>(std::max<i16>(from_le(clip.frame_count), 0));
}

inline f32 frame_rate(animation const& clip)
{
    return clip.frame_rate();
}

inline u32 loop_frame(animation const& clip)
{
    return clip.loop_start_frame();
}

/*! The clip to chain into after this one ends */
inline std::optional<u32> next_clip(animation const& clip)
{
    i16 const next = from_le(clip.next_animation);
    return next >= 0 ? std::optional<u32>(static_cast<u32>(next)) : std::nullopt;
}

inline anim::blend_t blend(animation const& clip)
{
    switch(clip.type)
    {
    case anim_type::overlay:
        return anim::blend_t::overlay;
    case anim_type::replacement:
        return anim::blend_t::replacement;
    default:
        return anim::blend_t::base;
    }
}

inline bool animates(animation const& clip, u32 node, anim::channel_t channel)
{
    switch(channel)
    {
    case anim::channel_t::rotation:
        return clip.has_rotation(node);
    case anim::channel_t::translation:
        return clip.has_translation(node);
    default:
        return clip.has_scale(node);
    }
}

/*! The frames each foot comes down on, left then right; none when the
 * clip marks no steps */
inline std::optional<std::array<i32, 2>> foot_frames(animation const& clip)
{
    i32 const left = clip.left_foot_frame_index, right = clip.right_foot_frame_index;
    if(left == 0 && right == 0)
        return std::nullopt;
    return std::array<i32, 2>{left, right};
}

inline std::optional<anim::sound_key_t> sound_key(animation const& clip)
{
    i16 const sound = from_le(clip.sound_index);
    if(sound < 0)
        return std::nullopt;
    return anim::sound_key_t{.frame = from_le(clip.sound_frame_index), .sound = sound};
}

} // namespace blam::antr
