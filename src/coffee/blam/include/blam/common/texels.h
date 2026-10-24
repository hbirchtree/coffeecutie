#pragma once

#include <peripherals/libc/types.h>
#include <peripherals/semantic/chunk.h>

#include <array>
#include <vector>

namespace blam::texels {

using namespace libc_types;

/*! Where an output channel comes from */
enum class channel_t : u8
{
    red,
    green,
    zero,
    one,
};

using swizzle_t = std::array<channel_t, 4>;

/*!
 * One- and two-channel texels spread to RGBA8 as `swizzle` reads them, for
 * GPUs without texture swizzle (WebGL). `pixels` holds `count` texels of
 * `channels` bytes each.
 */
std::vector<u8> expand_rgba8(
    semantic::Span<const u8> pixels, size_t count, u32 channels, swizzle_t const& swizzle);

} // namespace blam::texels
