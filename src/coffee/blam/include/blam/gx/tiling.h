#pragma once

#include <peripherals/libc/types.h>

/*!
 * GameCube/Wii (GX) texture tiling, shared by gx-bsp, which tiles at load
 * time, and the map transcoder, which does it ahead of time. Destinations
 * hold the tile-padded size; padding texels are zero.
 */
namespace blam::gx {

using libc_types::u16;
using libc_types::u32;
using libc_types::u8;

/*! Linear RGB565 to GX 4x4 tiles. swap byte-swaps each texel, for
 * little-endian map data; without it texels keep host order. */
void tile_rgb565(u16 const* src, u16* dst, u16 w, u16 h, bool swap);

/*! Linear 8bpp intensity to GX_TF_I8 8x4 tiles */
void tile_i8(u8 const* src, u8* dst, u16 w, u16 h);

/*! Linear intensity+alpha (bytes [I,A]) to GX_TF_IA8 4x4 tiles. GX reads
 * the 16-bit texel big-endian as (I<<8)|A, so [I,A] is stored per texel. */
void tile_ia8(u8 const* src, u8* dst, u16 w, u16 h);

/*!
 * Re-tiles a linear S3TC image's colour blocks into GX_TF_CMPR (the same
 * 4bpp): 4x4 blocks grouped into 8x8 tiles (TL,TR,BL,BR), 16-bit endpoints
 * byte-swapped to big-endian, 2-bit index groups reversed per byte. BC1 is
 * block 8 with colour at 0; BC2/BC3 block 16 with colour at 8 (alpha
 * dropped).
 */
void dxt_to_cmpr(u8 const* src, u8* dst, u16 w, u16 h, u32 block_size, u32 color_off);

} // namespace blam::gx
