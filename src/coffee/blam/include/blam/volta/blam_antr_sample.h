#pragma once

#include "blam_antr.h"

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

} // namespace blam::antr
