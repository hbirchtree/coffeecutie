#pragma once

#include <blam/volta/blam_bitm.h>
#include <peripherals/libc/types.h>

/* GameCube GX-native tiled pixel layouts, produced offline by the map
 * transcoder (examples/blam/map-transcode) and uploaded to the GPU with no
 * runtime decode/re-tile.
 *
 * These are the "auxiliary formats" reserved by blam::bitm::format_t::
 * platform_specific: a bitmap whose `format` has the 0xFF00 high byte set is
 * not a Blam engine format -- the low byte is one of the codes below. Keeping
 * the GX-specific codes here (not in the Blam source tree) confines platform
 * texture knowledge to the GX backend. Only the reader needs to know these
 * exist; the mapping to GX_TF_* constants lives in the GX consumer (which links
 * libogc), so this header stays libogc-free and host-includable. */
namespace gexxo::native {

using libc_types::u16;

enum class format : u16
{
    cmpr   = 0, /*!< GX_TF_CMPR   4bpp   <- BC1/BC2/BC3 (alpha dropped) */
    rgb565 = 1, /*!< GX_TF_RGB565 16bpp  <- R5G6B5/XRGB8 */
    i8     = 2, /*!< GX_TF_I8     8bpp   <- Y8/A8/AY8/P8 */
    ia8    = 3, /*!< GX_TF_IA8    16bpp  <- A8Y8 */
};

/* The blam mask marking auxiliary (platform-specific) formats. */
constexpr u16 mask = static_cast<u16>(blam::bitm::format_t::platform_specific);

/* Pack a GX-native code into a blam bitmap `format` value. */
constexpr blam::bitm::format_t to_blam(format f)
{
    return static_cast<blam::bitm::format_t>(mask | static_cast<u16>(f));
}

/* True if a blam `format` is a GX-native auxiliary format. */
constexpr bool is_native(blam::bitm::format_t f)
{
    return (static_cast<u16>(f) & 0xFF00) == mask;
}

/* Extract the GX-native code from a blam `format` (only valid if is_native). */
constexpr format code_of(blam::bitm::format_t f)
{
    return static_cast<format>(static_cast<u16>(f) & 0x00FF);
}

} // namespace gexxo::native
