#pragma once

/* Host-side pixel transcoders for the offline map bitmap transcoder. Each takes
 * a linear Halo source image and produces a GameCube GX-native *tiled* buffer,
 * ready to upload with no runtime decode. Multi-byte GX texels are big-endian.
 *
 * These mirror the runtime re-tilers in examples/blam/gx-bsp/main.cpp
 * (tile_rgb565, dxt_to_cmpr) but run on the host so the console never decodes. */

#include <blam/volta/blam_bitm.h>
#include <coffee/graphics/apis/gexxo/native_format.h>

#include <algorithm>
#include <cstring>
#include <optional>
#include <vector>

namespace mtx {

using libc_types::u16;
using libc_types::u32;
using libc_types::u8;
using blam::bitm::format_t;
using gx_format = gexxo::native::format;

/* --- GX tile geometry (block dimensions, bits/texel) ---------------------- */

inline u16 align_up(u16 v, u16 a)
{
    return static_cast<u16>((v + (a - 1)) & ~(a - 1));
}

/* Padded byte size of a GX-native image at w x h. */
inline u32 tiled_size(gx_format t, u16 w, u16 h)
{
    switch(t)
    {
    case gx_format::cmpr: // 8x8 tile of 4x4 blocks, 4bpp
        return static_cast<u32>(align_up(w, 8)) * align_up(h, 8) / 2;
    case gx_format::rgb565: // 4x4 tile, 16bpp
    case gx_format::ia8:    // 4x4 tile, 16bpp
        return static_cast<u32>(align_up(w, 4)) * align_up(h, 4) * 2;
    case gx_format::i8: // 8x4 tile, 8bpp
        return static_cast<u32>(align_up(w, 8)) * align_up(h, 4);
    }
    return 0;
}

/* GX-native target for a Halo source format, or nullopt if unsupported. */
inline std::optional<gx_format> target_for(format_t src)
{
    switch(src)
    {
    case format_t::BC1:
    case format_t::BC2:
    case format_t::BC3:
        return gx_format::cmpr;
    case format_t::R5G6B5:
    case format_t::XRGB8:
        return gx_format::rgb565;
    case format_t::Y8:
    case format_t::A8:
    case format_t::AY8:
    case format_t::P8:
        return gx_format::i8;
    case format_t::A8Y8:
        return gx_format::ia8;
    default:
        return std::nullopt;
    }
}

/* --- tilers --------------------------------------------------------------- */

/* Linear RGB565 -> GX 4x4 tiles. swap byte-swaps each texel to big-endian. */
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

/* Linear 8bpp intensity -> GX_TF_I8 8x4 tiles. */
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

/* Linear intensity+alpha (source byte order [I,A]) -> GX_TF_IA8 4x4 tiles.
 * GX reads the 16-bit texel big-endian as (I<<8)|A, so store [I,A] per texel. */
inline void tile_ia8(u8 const* src, u8* dst, u16 w, u16 h)
{
    u32 o = 0;
    for(u16 ty = 0; ty < h; ty += 4)
        for(u16 tx = 0; tx < w; tx += 4)
            for(u16 ry = 0; ry < 4; ry++)
                for(u16 rx = 0; rx < 4; rx++)
                {
                    u16 const x = tx + rx, y = ty + ry;
                    bool const in = (x < w && y < h);
                    dst[o++]      = in ? src[(y * w + x) * 2 + 0] : 0; // I
                    dst[o++]      = in ? src[(y * w + x) * 2 + 1] : 0; // A
                }
}

/* Re-tile an S3TC colour block into GX_TF_CMPR (same 4bpp size). 4x4 blocks
 * grouped into 8x8 tiles (TL,TR,BL,BR); 16-bit endpoints byte-swapped to
 * big-endian; 2-bit index groups reversed per byte. BC1: block 8, colour@0.
 * BC2/BC3: block 16, colour@8 (alpha dropped). Mirrors gx-bsp dxt_to_cmpr. */
inline void dxt_to_cmpr(
    u8 const* src, u8* dst, u16 w, u16 h, u32 block_size, u32 color_off)
{
    u32 const bw = (w + 3) / 4, bh = (h + 3) / 4;
    u32       o  = 0;
    auto      rev = [](u8 v) -> u8 {
        return ((v & 0x03) << 6) | ((v & 0x0C) << 2) | ((v & 0x30) >> 2) |
               ((v & 0xC0) >> 6);
    };
    for(u32 ty = 0; ty < bh; ty += 2)
        for(u32 tx = 0; tx < bw; tx += 2)
            for(u32 sy = 0; sy < 2; sy++)
                for(u32 sx = 0; sx < 2; sx++)
                {
                    u32 const bx = tx + sx, by = ty + sy;
                    u8        b[8] = {0};
                    if(bx < bw && by < bh)
                        std::memcpy(
                            b, src + (by * bw + bx) * block_size + color_off, 8);
                    dst[o++] = b[1];
                    dst[o++] = b[0];
                    dst[o++] = b[3];
                    dst[o++] = b[2];
                    dst[o++] = rev(b[4]);
                    dst[o++] = rev(b[5]);
                    dst[o++] = rev(b[6]);
                    dst[o++] = rev(b[7]);
                }
}

/* --- BC1 encode (for uncompressed sources targeting CMPR, e.g. lightmaps) --- */

/* Minimal BC1/DXT1 block encoder: per 4x4 block use the per-channel min/max as
 * the two 565 endpoints (opaque 4-colour mode), assign each texel the nearest of
 * the 4 ramp colours. Adequate for smooth low-frequency data (lightmaps).
 * `rgba` = tightly packed w*h*4 (RGBA8); emits 8-byte blocks in linear order. */
inline void encode_bc1(u8 const* rgba, u8* blocks, u16 w, u16 h)
{
    u32 const bw = (w + 3u) / 4u, bh = (h + 3u) / 4u;
    auto to565 = [](int r, int g, int b) -> u16 {
        return static_cast<u16>(((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3));
    };
    auto exp565 = [](u16 c, int& r, int& g, int& b) {
        r = (c >> 11) & 31;
        r = (r << 3) | (r >> 2);
        g = (c >> 5) & 63;
        g = (g << 2) | (g >> 4);
        b = c & 31;
        b = (b << 3) | (b >> 2);
    };
    for(u32 by = 0; by < bh; by++)
        for(u32 bx = 0; bx < bw; bx++)
        {
            int mnr = 255, mng = 255, mnb = 255, mxr = 0, mxg = 0, mxb = 0;
            for(int y = 0; y < 4; y++)
                for(int x = 0; x < 4; x++)
                {
                    u32 const px = bx * 4 + x, py = by * 4 + y;
                    if(px >= w || py >= h)
                        continue;
                    u32 const i = (py * w + px) * 4;
                    int const r = rgba[i], g = rgba[i + 1], b = rgba[i + 2];
                    mnr = std::min(mnr, r);
                    mng = std::min(mng, g);
                    mnb = std::min(mnb, b);
                    mxr = std::max(mxr, r);
                    mxg = std::max(mxg, g);
                    mxb = std::max(mxb, b);
                }
            u16 c0 = to565(mxr, mxg, mxb), c1 = to565(mnr, mng, mnb);
            if(c0 < c1)
                std::swap(c0, c1); // c0 > c1 -> opaque 4-colour mode
            int pal[4][3];
            exp565(c0, pal[0][0], pal[0][1], pal[0][2]);
            exp565(c1, pal[1][0], pal[1][1], pal[1][2]);
            for(int k = 0; k < 3; k++)
            {
                pal[2][k] = (2 * pal[0][k] + pal[1][k]) / 3;
                pal[3][k] = (pal[0][k] + 2 * pal[1][k]) / 3;
            }
            u32 idx = 0;
            for(int t = 0; t < 16; t++)
            {
                u32 const px = bx * 4 + (t & 3), py = by * 4 + (t >> 2);
                int       r = pal[0][0], g = pal[0][1], b = pal[0][2];
                if(px < w && py < h)
                {
                    u32 const i = (py * w + px) * 4;
                    r = rgba[i];
                    g = rgba[i + 1];
                    b = rgba[i + 2];
                }
                int best = 0, bd = 1 << 30;
                for(int c = 0; c < 4; c++)
                {
                    int const dr = r - pal[c][0], dg = g - pal[c][1],
                              db = b - pal[c][2];
                    int const d = dr * dr + dg * dg + db * db;
                    if(d < bd)
                    {
                        bd   = d;
                        best = c;
                    }
                }
                idx |= static_cast<u32>(best) << (t * 2);
            }
            u8* blk = blocks + (by * bw + bx) * 8;
            blk[0]  = c0 & 0xFF;
            blk[1]  = c0 >> 8;
            blk[2]  = c1 & 0xFF;
            blk[3]  = c1 >> 8;
            blk[4]  = idx & 0xFF;
            blk[5]  = (idx >> 8) & 0xFF;
            blk[6]  = (idx >> 16) & 0xFF;
            blk[7]  = (idx >> 24) & 0xFF;
        }
}

/* Encode an RGBA8 image straight to GX_TF_CMPR (BC1 encode + GX re-tile). */
inline void rgba_to_cmpr(u8 const* rgba, u8* dst, u16 w, u16 h)
{
    u32 const nblocks = ((w + 3u) / 4u) * ((h + 3u) / 4u);
    std::vector<u8> bc1(static_cast<size_t>(nblocks) * 8);
    encode_bc1(rgba, bc1.data(), w, h);
    dxt_to_cmpr(bc1.data(), dst, w, h, 8, 0);
}

/* Bytes of one source mip level at w x h (matches image_t::data() layout:
 * BC = ceil/4 blocks x block bytes; uncompressed = w*h*bpp). */
inline u32 src_level_bytes(format_t src, u16 w, u16 h)
{
    u32 const bw = (w + 3u) / 4u, bh = (h + 3u) / 4u;
    switch(src)
    {
    case format_t::BC1:
        return bw * bh * 8u;
    case format_t::BC2:
    case format_t::BC3:
        return bw * bh * 16u;
    case format_t::R5G6B5:
    case format_t::A8Y8:
        return static_cast<u32>(w) * h * 2u;
    case format_t::XRGB8:
        return static_cast<u32>(w) * h * 4u;
    case format_t::Y8:
    case format_t::A8:
    case format_t::AY8:
    case format_t::P8:
        return static_cast<u32>(w) * h;
    default:
        return 0;
    }
}

/* --- dispatch ------------------------------------------------------------- */

/* Decode a linear R5G6B5 (LE) image to tightly-packed RGBA8. */
inline void decode_565_rgba(u16 const* px, u8* rgba, u16 w, u16 h)
{
    for(size_t i = 0; i < static_cast<size_t>(w) * h; i++)
    {
        u16 const v = px[i];
        int       r = (v >> 11) & 31, g = (v >> 5) & 63, b = v & 31;
        rgba[i * 4 + 0] = static_cast<u8>((r << 3) | (r >> 2));
        rgba[i * 4 + 1] = static_cast<u8>((g << 2) | (g >> 4));
        rgba[i * 4 + 2] = static_cast<u8>((b << 3) | (b >> 2));
        rgba[i * 4 + 3] = 255;
    }
}

/* Transcode one mip level (w x h) of source pixels to GX-native `tgt` into the
 * caller-sized `dst` (tiled_size(tgt, w, h) bytes). Returns false on short
 * input or an unsupported (src, tgt) pair. */
inline bool transcode_level(
    format_t src, gx_format tgt, u8 const* px, u32 px_size, u16 w, u16 h,
    u8* dst)
{
    if(px_size < src_level_bytes(src, w, h))
        return false;

    if(tgt == gx_format::cmpr)
    {
        switch(src)
        {
        case format_t::BC1:
            dxt_to_cmpr(px, dst, w, h, 8, 0);
            return true;
        case format_t::BC2:
        case format_t::BC3:
            dxt_to_cmpr(px, dst, w, h, 16, 8);
            return true;
        case format_t::R5G6B5:
        {
            // Encode 565 -> BC1 -> CMPR (e.g. lightmaps at 4bpp vs 16bpp).
            std::vector<u8> rgba(static_cast<size_t>(w) * h * 4);
            decode_565_rgba(reinterpret_cast<u16 const*>(px), rgba.data(), w, h);
            rgba_to_cmpr(rgba.data(), dst, w, h);
            return true;
        }
        case format_t::XRGB8:
        {
            std::vector<u8> rgba(static_cast<size_t>(w) * h * 4);
            for(size_t i = 0; i < static_cast<size_t>(w) * h; i++)
            {
                rgba[i * 4 + 0] = px[i * 4 + 0];
                rgba[i * 4 + 1] = px[i * 4 + 1];
                rgba[i * 4 + 2] = px[i * 4 + 2];
                rgba[i * 4 + 3] = 255;
            }
            rgba_to_cmpr(rgba.data(), dst, w, h);
            return true;
        }
        default:
            return false;
        }
    }

    switch(src)
    {
    case format_t::R5G6B5:
        // Map data is little-endian -> swap to big-endian for GX.
        tile_rgb565(
            reinterpret_cast<u16 const*>(px),
            reinterpret_cast<u16*>(dst),
            w,
            h,
            true);
        return true;
    case format_t::XRGB8:
    {
        // Downconvert RGBX8 (bytes R,G,B,X) -> linear 565, then tile+swap.
        std::vector<u16> tmp(static_cast<size_t>(w) * h);
        for(size_t i = 0; i < tmp.size(); i++)
        {
            u8 const r = px[i * 4 + 0], g = px[i * 4 + 1], b = px[i * 4 + 2];
            tmp[i] = static_cast<u16>(
                ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3));
        }
        tile_rgb565(tmp.data(), reinterpret_cast<u16*>(dst), w, h, true);
        return true;
    }
    case format_t::Y8:
    case format_t::A8:
    case format_t::AY8:
    case format_t::P8:
        tile_i8(px, dst, w, h);
        return true;
    case format_t::A8Y8:
        tile_ia8(px, dst, w, h);
        return true;
    default:
        return false;
    }
}

/* Transcode an image's full mip chain into GX-native tiled bytes, packed level
 * consecutively (the layout GX_InitTexObj expects). Levels are emitted while
 * they fit the source slot (px_size); GX pads each CMPR level to 8x8, so the
 * smallest source levels can overflow -> the chain is truncated there and
 * `out_levels` reports how many were emitted (maxlod = out_levels - 1). Returns
 * false if the base level is unsupported or doesn't fit. */
inline bool transcode_mipped(
    format_t         src,
    u8 const*        px,
    u32              px_size,
    u16              base_w,
    u16              base_h,
    u16              src_mip_count,
    bool             rgb565_to_cmpr,
    std::vector<u8>& dst,
    gx_format&       out,
    u32&             out_levels)
{
    auto target = target_for(src);
    if(!target)
        return false;
    out = *target;
    // Optionally compress uncompressed colour (e.g. lightmaps) to CMPR (4bpp
    // vs 16bpp) instead of the native RGB565 path.
    if(rgb565_to_cmpr &&
       (src == format_t::R5G6B5 || src == format_t::XRGB8))
        out = gx_format::cmpr;
    out_levels = 0;
    dst.clear();

    u16 const levels = src_mip_count < 1 ? 1 : src_mip_count;
    u32       src_off = 0;
    for(u16 k = 0; k < levels; k++)
    {
        u16 const wl  = static_cast<u16>(base_w >> k ? base_w >> k : 1);
        u16 const hl  = static_cast<u16>(base_h >> k ? base_h >> k : 1);
        u32 const ssz = src_level_bytes(src, wl, hl);
        u32 const gsz = tiled_size(out, wl, hl);
        if(ssz == 0 || src_off + ssz > px_size)
            break; // ran out of source data
        if(dst.size() + gsz > px_size)
            break; // GX-padded chain would overflow the in-place slot

        size_t const at = dst.size();
        dst.resize(at + gsz, 0);
        if(!transcode_level(
               src, out, px + src_off, ssz, wl, hl, dst.data() + at))
        {
            dst.resize(at);
            break;
        }
        out_levels++;
        src_off += ssz;
        if(wl == 1 && hl == 1)
            break;
    }
    return out_levels > 0;
}

} // namespace mtx
