#pragma once

#include <peripherals/libc/types.h>

/*!
 * What the animation system asks of a clip, in any game. Each game's clip
 * type gets free functions in its own namespace (frame_count, frame_rate,
 * loop_frame, next_clip, blend, animates, foot_frames, sound_key), so code
 * calling them unqualified works for whichever clip it holds.
 */
namespace blam::anim {

/*! How a clip's nodes combine with what is under them */
enum class blend_t
{
    base,        /*!< The whole pose */
    replacement, /*!< Its animated nodes replace the pose's */
    overlay,     /*!< Its animated nodes add to the pose's */
};

enum class channel_t
{
    rotation,
    translation,
    scale,
};

/*! A sound a clip plays as it passes a frame */
struct sound_key_t
{
    libc_types::i32 frame;
    libc_types::i32 sound; /*!< Into the graph's sounds */
};

} // namespace blam::anim
