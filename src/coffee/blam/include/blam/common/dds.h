#pragma once

#include <peripherals/libc/types.h>
#include <peripherals/semantic/chunk.h>

#include <vector>

/*! Minimal DDS files for extracted bitmaps: the base level, optionally a
 * 6-face cube map, enough for image viewers and texconv */
namespace blam::dds {

using libc_types::u32;
using libc_types::u8;

/*! Pixel format flags for the uncompressed case */
constexpr u32 rgb       = 0x40;
constexpr u32 rgba      = 0x41;
constexpr u32 luminance = 0x20000;
constexpr u32 lum_alpha = 0x20001;
constexpr u32 alpha     = 0x2;
constexpr u32 bump_dudv = 0x80000; /*!< Signed channels */

struct info_t
{
    u32 fourcc{0}; /*!< 0 = uncompressed, described by the mask fields */
    u32 block_bytes{0};
    u32 bpp{0};
    u32 pf_flags{0};
    u32 mask_r{0};
    u32 mask_g{0};
    u32 mask_b{0};
    u32 mask_a{0};
};

/*! A DDS file of `pixels`, one base level (six faces for a cube map) */
std::vector<char> make(
    info_t const& info, u32 width, u32 height, semantic::Span<const u8> pixels, bool cube_map = false);

} // namespace blam::dds
