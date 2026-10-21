#pragma once

#include <peripherals/libc/types.h>

#include <cstring>

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
inline void tile_rgb565(u16 const* src, u16* dst, u16 w, u16 h, bool swap)
{
    u32 o = 0;
    for(u16 ty = 0; ty < h; ty += 4)
        for(u16 tx = 0; tx < w; tx += 4)
            for(u16 ry = 0; ry < 4; ry++)
                for(u16 rx = 0; rx < 4; rx++)
                {
                    u16 const x = tx + rx, y = ty + ry;
                    u16 const t = (x < w && y < h) ? src[y * w + x] : 0;
                    dst[o++]    = swap ? __builtin_bswap16(t) : t;
                }
}

/*! Linear 8bpp intensity to GX_TF_I8 8x4 tiles */
inline void tile_i8(u8 const* src, u8* dst, u16 w, u16 h)
{
    u32 o = 0;
    for(u16 ty = 0; ty < h; ty += 4)
        for(u16 tx = 0; tx < w; tx += 8)
            for(u16 ry = 0; ry < 4; ry++)
                for(u16 rx = 0; rx < 8; rx++)
                {
                    u16 const x = tx + rx, y = ty + ry;
                    dst[o++]    = (x < w && y < h) ? src[y * w + x] : 0;
                }
}

/*! Linear intensity+alpha (bytes [I,A]) to GX_TF_IA8 4x4 tiles. GX reads
 * the 16-bit texel big-endian as (I<<8)|A, so [I,A] is stored per texel. */
inline void tile_ia8(u8 const* src, u8* dst, u16 w, u16 h)
{
    u32 o = 0;
    for(u16 ty = 0; ty < h; ty += 4)
        for(u16 tx = 0; tx < w; tx += 4)
            for(u16 ry = 0; ry < 4; ry++)
                for(u16 rx = 0; rx < 4; rx++)
                {
                    u16 const  x  = tx + rx, y = ty + ry;
                    bool const in = (x < w && y < h);
                    dst[o++]      = in ? src[(y * w + x) * 2 + 0] : 0; // I
                    dst[o++]      = in ? src[(y * w + x) * 2 + 1] : 0; // A
                }
}

/*!
 * Re-tiles a linear S3TC image's colour blocks into GX_TF_CMPR (the same
 * 4bpp): 4x4 blocks grouped into 8x8 tiles (TL,TR,BL,BR), 16-bit endpoints
 * byte-swapped to big-endian, 2-bit index groups reversed per byte. BC1 is
 * block 8 with colour at 0; BC2/BC3 block 16 with colour at 8 (alpha
 * dropped).
 */
inline void dxt_to_cmpr(u8 const* src, u8* dst, u16 w, u16 h, u32 block_size, u32 color_off)
{
    u32 const bw = (w + 3) / 4, bh = (h + 3) / 4;
    u32       o   = 0;
    auto      rev = [](u8 v) -> u8 {
        return ((v & 0x03) << 6) | ((v & 0x0C) << 2) | ((v & 0x30) >> 2) | ((v & 0xC0) >> 6);
    };
    for(u32 ty = 0; ty < bh; ty += 2)
        for(u32 tx = 0; tx < bw; tx += 2)
            for(u32 sy = 0; sy < 2; sy++)
                for(u32 sx = 0; sx < 2; sx++)
                {
                    u32 const bx = tx + sx, by = ty + sy;
                    u8        b[8] = {0};
                    if(bx < bw && by < bh)
                        std::memcpy(b, src + (by * bw + bx) * block_size + color_off, 8);
                    dst[o++] = b[1]; // endpoint0 -> BE
                    dst[o++] = b[0];
                    dst[o++] = b[3]; // endpoint1 -> BE
                    dst[o++] = b[2];
                    dst[o++] = rev(b[4]); // index rows, columns reversed
                    dst[o++] = rev(b[5]);
                    dst[o++] = rev(b[6]);
                    dst[o++] = rev(b[7]);
                }
}

} // namespace blam::gx
