#pragma once

#include "sound.h"

#include <vector>

namespace blam::elegy {

/*! Decoded audio, interleaved signed 16-bit */
struct pcm_t
{
    u32              channels{0};
    u32              rate{0};
    std::vector<i16> samples;
};

/*!
 * \brief Decodes a permutation's XMA2 data (see permutation_data) through
 * libavcodec, which is linked as a shared library. Only built with
 * BUILD_OPENAL_XMA2 (BLAM_ELEGY_XMA2 defined).
 */
bool decode_xma2(xma2_format_t const& format, Span<byte_t const> data, pcm_t& out);

} // namespace blam::elegy
