#pragma once

/* Host-side pixel transcoders for the offline map bitmap transcoder. Each takes
 * a linear Halo source image and produces a GameCube GX-native *tiled* buffer,
 * ready to upload with no runtime decode. Multi-byte GX texels are big-endian.
 *
 * These mirror the runtime re-tilers in examples/blam/gx-bsp/main.cpp
 * (tile_rgb565, dxt_to_cmpr) but run on the host so the console never decodes. */

#include "peripherals/identify/compiler/unreachable.h"
#include "peripherals/stl/range.h"
#include <blam/volta/blam_bitm.h>
#include <coffee/graphics/apis/gexxo/native_format.h>

#include <pvrtc/Common.h>
#include <pvrtc/Encode.h>
#include <pvrtc/Morton.h>

#include <algorithm>
#include <cstring>
#include <functional>
#include <optional>
#include <string>
#include <vector>

#define BCDEC_IMPLEMENTATION
#define BCDEC_STATIC
#include <bcdec.h>

namespace mtx {

using libc_types::u16;
using libc_types::u32;
using libc_types::u8;
using blam::bitm::format_t;

inline bool is_pow2(u16 v)
{
    return v != 0 && (v & (v - 1)) == 0;
}

inline void decode_565_rgba(gsl::span<u16 const> px, std::vector<u8>& rgba)
{
    for(size_t i = 0; i<px.size(); i++)
    {
        u16 const v = px[i];
        int       r = (v >> 11) & 31, g = (v >> 5) & 63, b = v & 31;
        rgba[i * 4 + 0] = static_cast<u8>((r << 3) | (r >> 2));
        rgba[i * 4 + 1] = static_cast<u8>((g << 2) | (g >> 4));
        rgba[i * 4 + 2] = static_cast<u8>((b << 3) | (b >> 2));
        rgba[i * 4 + 3] = 255;
    }
}

inline std::vector<u8> decode_uncompressed_rgba(
    format_t src, gsl::span<u8 const> px, u16 w, u16 h)
{
    std::vector<u8> rgba;
    rgba.reserve(static_cast<size_t>(w) * h * 4);
    switch(src)
    {
    case format_t::R5G6B5:
        // Element count for gsl::span is w*h (texels), not w*h*sizeof(u16)
        // (that's the BYTE count) -- the latter double-counts and made
        // decode_565_rgba's unguarded write loop run off the end of `rgba`.
        rgba.resize(w * h * 4);
        decode_565_rgba(
            gsl::span<u16 const>(reinterpret_cast<u16 const*>(px.data()), w * h),
            rgba);
        break;
    case format_t::XRGB8: // bytes R,G,B,X -> force alpha opaque
        rgba.resize(w * h * 4);
        for(size_t i = 0; i < static_cast<size_t>(w) * h; i++)
        {
            rgba[i * 4 + 0] = px[i * 4 + 0];
            rgba[i * 4 + 1] = px[i * 4 + 1];
            rgba[i * 4 + 2] = px[i * 4 + 2];
            rgba[i * 4 + 3] = 255;
        }
        break;
    case format_t::ARGB8: // already tightly-packed R,G,B,A
        rgba.insert(rgba.begin(), px.begin(), px.end());
        break;
    case format_t::BC1:
    case format_t::BC2:
    case format_t::BC3:
    {
        u32 blk_size{};
        decltype(bcdec_bc1)* transform{};
        switch(src)
        {
        case format_t::BC1:
            blk_size = BCDEC_BC1_BLOCK_SIZE;
            transform = bcdec_bc1;
            break;
        case format_t::BC2:
            blk_size = BCDEC_BC2_BLOCK_SIZE;
            transform = bcdec_bc2;
            break;
        case format_t::BC3:
            blk_size = BCDEC_BC3_BLOCK_SIZE;
            transform = bcdec_bc3;
            break;
        default:
            unreachable();
        }
        rgba.resize(w * h * 4);

        u32 blk_pitch = w / 4;
        for(auto y : stl_types::range<int>(h / 4))
            for(auto x : stl_types::range<int>(blk_pitch))
            {
                auto blk_idx = (y * blk_pitch + x) * blk_size;
                transform(
                    px.subspan(blk_idx, blk_size).data(),
                    &rgba[(y * w + x) * 16],
                    w * 4);
            }
        break;
    }
    default:
        rgba.clear();
        break;
    }
    return rgba;
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

/* --- transcode kernel interface --------------------------------------------
 * A "kernel" is one target's whole encode step, in a shape that's the same
 * for every target: given a source image's format/pixels/dims/mip count,
 * either produce the bytes to write in its place (+ the on-disk format code
 * to patch into image_t::format, + GX maxlod / mip levels for targets that
 * have mips) or return nullopt to leave the source untouched. Adding a new
 * target (another game console, another GPU) means writing one function
 * matching `kernel_fn` -- main.cpp's per-image loop and file I/O don't
 * change. `format` is the RAW on-disk value already computed for the target
 * (e.g. gexxo::native::to_blam(gx_format) for Gekko) -- kernels own their own
 * format-marker scheme; the caller just writes whatever value comes back. */
struct transcode_result
{
    u16             format; // patched into image_t::format verbatim
    u16             maxlod; // patched into image_t::mipmaps verbatim (0 if n/a)
    std::vector<u8> data;
};

using kernel_fn = std::function<std::optional<transcode_result>(
    format_t             src_fmt,
    gsl::span<u8 const>  src_px,
    u32                  src_size,
    u16                  w,
    u16                  h,
    u16                  src_mip_count)>;

namespace gekko {
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
inline void tile_rgb565(gsl::span<u16 const> src, gsl::span<u16> dst, u16 w, u16 h, bool swap)
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
inline void tile_i8(gsl::span<u8 const> src, gsl::span<u8>& dst, u16 w, u16 h)
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
inline void tile_ia8(gsl::span<u8 const> src, gsl::span<u8>& dst, u16 w, u16 h)
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
    gsl::span<u8 const> src,
    gsl::span<u8>& dst,
    u16 w,
    u16 h,
    u32 block_size,
    u32 color_off)
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
                            b, &src[(by * bw + bx) * block_size + color_off], 8);
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
inline void encode_bc1(gsl::span<u8 const> rgba, std::vector<u8>& blocks, u16 w, u16 h)
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
            u8* blk = &blocks[(by * bw + bx) * 8];
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
inline void rgba_to_cmpr(gsl::span<u8 const> rgba, gsl::span<u8>& dst, u16 w, u16 h)
{
    u32 const nblocks = ((w + 3u) / 4u) * ((h + 3u) / 4u);
    std::vector<u8> bc1(static_cast<size_t>(nblocks) * 8);
    encode_bc1(rgba, bc1, w, h);
    dxt_to_cmpr(
        gsl::span<u8 const>(bc1.data(), bc1.size()),
        dst, 
        w, 
        h, 
        8, 
        0);
}

/* --- dispatch ------------------------------------------------------------- */

/* Transcode one mip level (w x h) of source pixels to GX-native `tgt` into the
 * caller-sized `dst` (tiled_size(tgt, w, h) bytes). Returns false on short
 * input or an unsupported (src, tgt) pair. */
inline bool transcode_level(
    format_t src,
    gx_format tgt,
    gsl::span<u8 const> px,
    u32 px_size,
    u16 w,
    u16 h,
    gsl::span<u8> dst)
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
            // Element count is w*h texels, not w*h*sizeof(u16) bytes -- see
            // decode_uncompressed_rgba's R5G6B5 case for why that matters.
            std::vector<u8> rgba(static_cast<size_t>(w) * h * 4);
            decode_565_rgba(
                gsl::span<u16 const>(
                    reinterpret_cast<u16 const*>(px.data()), w * h),
                rgba);
            rgba_to_cmpr(rgba, dst, w, h);
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
            rgba_to_cmpr(rgba, dst, w, h);
            return true;
        }
        default:
            return false;
        }
    }

    switch(src)
    {
    case format_t::R5G6B5:
        // Map data is little-endian -> swap to big-endian for GX. dst's
        // element count comes from dst.size() (== tiled_size(rgb565,w,h),
        // already 4x4-tile-padded by the caller) -- NOT w*h, which
        // undershoots whenever w or h isn't a multiple of 4 (tile_rgb565's
        // write loop is unconditional and always writes the padded count).
        tile_rgb565(
            gsl::span<u16 const>(
                reinterpret_cast<u16 const*>(px.data()), w * h),
            gsl::span<u16>(
                reinterpret_cast<u16*>(dst.data()), dst.size() / sizeof(u16)),
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
        // See the R5G6B5 case above: dst sized from dst.size(), not w*h.
        tile_rgb565(
            gsl::span(tmp.data(), tmp.size()),
            gsl::span<u16>(
                reinterpret_cast<u16*>(dst.data()), dst.size() / sizeof(u16)),
            w,
            h,
            true);
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
    gsl::span<u8 const>        px,
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
               src, out,
               px.subspan(src_off, ssz),
               ssz,
               wl, hl,
               gsl::span<u8>(dst.data() + at, dst.size() - at)))
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

/* Kernel factory: the Gekko target's only knob is whether uncompressed
 * colour (lightmaps) should go through CMPR instead of native RGB565 (see
 * transcode_mipped's rgb565_to_cmpr). `lightmap_format` is the raw
 * --lightmap-format value (unset/empty = native); interpreting it is Gekko's
 * own business -- other targets can give the same flag a different meaning. */
inline mtx::kernel_fn make_kernel(std::optional<std::string> const& lightmap_format)
{
    bool const lightmap_cmpr =
        lightmap_format &&
        (*lightmap_format == "cmpr" || *lightmap_format == "compressed");
    return [lightmap_cmpr](
               format_t             src_fmt,
               gsl::span<u8 const>  src_px,
               u32                  src_size,
               u16                  w,
               u16                  h,
               u16                  src_mip_count)
               -> std::optional<mtx::transcode_result> {
        std::vector<u8> tiled;
        gx_format       out_fmt;
        u32             levels = 0;
        if(!transcode_mipped(
               src_fmt,
               src_px,
               src_size,
               w,
               h,
               src_mip_count,
               lightmap_cmpr,
               tiled,
               out_fmt,
               levels))
            return std::nullopt;
        return mtx::transcode_result{
            static_cast<u16>(gexxo::native::to_blam(out_fmt)),
            static_cast<u16>(levels > 0 ? levels - 1 : 0),
            std::move(tiled)};
    };
}

} // namespace gekko

namespace powervr {

// --- PVRTC (PowerVR GPUs) ---------------------------------------------

/* Twiddle (Morton-tile) a block coordinate for a possibly-rectangular
 * power-of-two grid: bit-exact port of Imagination's reference `TwiddleUV`
 * (PVRCore/texture/PVRTDecompress.cpp) -- interleaves bits up to the smaller
 * dimension (Y at even bit positions, X at odd, matching harumazzz/pvrtc's
 * own MORTON_TABLE convention for the square case, verified bit-identical),
 * then appends the excess bits of the larger dimension as a contiguous "tile
 * index" in the high bits. This is the real format's addressing for wide/tall
 * textures (square tiles laid out in raster order), not a guess -- it's what
 * a real PowerVR decoder (and this repo's own `pvrtcdec` dependency, same
 * Imagination PVRTDecompress code) expects. ysize/xsize are BLOCK counts. */
inline u32 twiddle_uv(u32 ysize, u32 xsize, u32 ypos, u32 xpos)
{
    u32 const min_dim  = ysize < xsize ? ysize : xsize;
    u32       max_val  = ysize < xsize ? xpos : ypos;
    u32       src_bit   = 1, dst_bit = 1, twiddled = 0;
    int       shift     = 0;
    while(src_bit < min_dim)
    {
        if(ypos & src_bit)
            twiddled |= dst_bit;
        if(xpos & src_bit)
            twiddled |= dst_bit << 1;
        src_bit <<= 1;
        dst_bit <<= 2;
        shift++;
    }
    max_val >>= shift;
    twiddled |= max_val << (2 * shift);
    return twiddled;
}

/* Encode tightly-packed RGBA8 (w, h independently power-of-two -- NOT
 * required to be equal; each dimension >= 4) to raw PVRTC 4bpp bytes: 8
 * bytes/block (64-bit word, native/little-endian -- matches how a PowerVR
 * driver's glCompressedTexImage2D expects the blob), real twiddled block
 * order (via twiddle_uv above; NOT the GX CMPR tiling -- a different GPU's
 * native layout). has_alpha selects the RGBA vs RGB (opaque) encode.
 *
 * Generalizes harumazzz/pvrtc's encode_rgba_4bpp/encode_rgb_4bpp (which
 * hardcode width==height, both for the output block count and for reusing
 * one width-derived wraparound mask on both axes) to rectangular POT, reusing
 * the port's primitives (Packet, ColorRGB/RGBA, calculate_bounding_box --
 * calculate_bounding_box only uses `width` as a row stride, so it's already
 * shape-agnostic) with a real twiddled index instead of the square-only one. */
inline std::vector<u8> encode_pvrtc_rect(
    gsl::span<u8 const> rgba, u16 w, u16 h, bool has_alpha)
{
    u32 const bw = w >> 2, bh = h >> 2;
    if(bw == 0 || bh == 0)
        return {};
    u32 const bmask_x = bw - 1, bmask_y = bh - 1;

    std::vector<u8> color(rgba.begin(), rgba.end());
    std::vector<PVRTC::Packet> result(static_cast<size_t>(bw) * bh);

    Rgba32 min_color, max_color;
    for(u32 y = 0; y < bh; y++)
        for(u32 x = 0; x < bw; x++)
        {
            PVRTC::calculate_bounding_box(color, w, x, y, min_color, max_color);
            PVRTC::Packet packet;
            packet.set_use_punch_through_alpha(false);
            if(has_alpha)
            {
                packet.set_color_alpha_color_rgba(min_color);
                packet.set_color_blue_color_rgba(max_color);
            } else
            {
                packet.set_color_alpha_color_rgb(min_color);
                packet.set_color_blue_color_rgb(max_color);
            }
            result[twiddle_uv(bh, bw, y, x)] = packet;
        }

    for(u32 y = 0; y < bh; y++)
        for(u32 x = 0; x < bw; x++)
        {
            auto const factors      = PVRTC::Packet::BILINEAR_FACTORS;
            int        factor_index = 0;
            auto const data_index   = (y << 2) * w + (x << 2);
            u32        modulation   = 0;

            for(int py = 0; py < 4; py++)
            {
                int const  y_off = (py < 2) ? -1 : 0;
                u32 const  y0    = (y + static_cast<u32>(y_off + bh)) & bmask_y;
                u32 const  y1    = (y0 + 1) & bmask_y;

                for(int px = 0; px < 4; px++)
                {
                    auto const factor = factors[factor_index];
                    int const  x_off  = (px < 2) ? -1 : 0;
                    u32 const  x0 = (x + static_cast<u32>(x_off + bw)) & bmask_x;
                    u32 const  x1 = (x0 + 1) & bmask_x;

                    auto const& p0 = result[twiddle_uv(bh, bw, y0, x0)];
                    auto const& p1 = result[twiddle_uv(bh, bw, y0, x1)];
                    auto const& p2 = result[twiddle_uv(bh, bw, y1, x0)];
                    auto const& p3 = result[twiddle_uv(bh, bw, y1, x1)];

                    auto const pixel_index =
                        static_cast<size_t>(data_index + py * w + px) * 4;

                    int projection, length_squared;
                    if(has_alpha)
                    {
                        auto const ca = p0.get_color_alpha_color_rgba() * factor[0] +
                                        p1.get_color_alpha_color_rgba() * factor[1] +
                                        p2.get_color_alpha_color_rgba() * factor[2] +
                                        p3.get_color_alpha_color_rgba() * factor[3];
                        auto const cb = p0.get_color_blue_color_rgba() * factor[0] +
                                        p1.get_color_blue_color_rgba() * factor[1] +
                                        p2.get_color_blue_color_rgba() * factor[2] +
                                        p3.get_color_blue_color_rgba() * factor[3];
                        auto const d = cb - ca;
                        auto const p = ColorRGBA(
                            color[pixel_index] << 4,
                            color[pixel_index + 1] << 4,
                            color[pixel_index + 2] << 4,
                            color[pixel_index + 3] << 4);
                        auto const v = p - ca;
                        projection      = (v % d) << 4;
                        length_squared  = d % d;
                    } else
                    {
                        auto const ca = p0.get_color_alpha_color_rgb() * factor[0] +
                                        p1.get_color_alpha_color_rgb() * factor[1] +
                                        p2.get_color_alpha_color_rgb() * factor[2] +
                                        p3.get_color_alpha_color_rgb() * factor[3];
                        auto const cb = p0.get_color_blue_color_rgb() * factor[0] +
                                        p1.get_color_blue_color_rgb() * factor[1] +
                                        p2.get_color_blue_color_rgb() * factor[2] +
                                        p3.get_color_blue_color_rgb() * factor[3];
                        auto const d = cb - ca;
                        auto const p = ColorRGB(
                            color[pixel_index] << 4,
                            color[pixel_index + 1] << 4,
                            color[pixel_index + 2] << 4);
                        auto const v = p - ca;
                        projection      = (v % d) << 4;
                        length_squared  = d % d;
                    }

                    if(projection > 3 * length_squared)
                        modulation++;
                    if(projection > 8 * length_squared)
                        modulation++;
                    if(projection > 13 * length_squared)
                        modulation++;
                    modulation = PVRTC::rotate_right(modulation, 2);
                    factor_index++;
                }
            }
            result[twiddle_uv(bh, bw, y, x)].set_modulation_data(modulation);
        }

    std::vector<u8> out(result.size() * 8);
    for(size_t i = 0; i < result.size(); i++)
        std::memcpy(out.data() + i * 8, &result[i].PvrTcWord, 8);
    return out;
}

/* Square convenience wrapper (bit-identical to encode_pvrtc_rect(rgba, size,
 * size, ...), verified: twiddle_uv degenerates to the port's own Morton code
 * when w==h). */
inline std::vector<u8> encode_pvrtc(gsl::span<u8 const> rgba, u16 size, bool has_alpha)
{
    return encode_pvrtc_rect(rgba, size, size, has_alpha);
}

/* No PowerVR-consuming runtime exists yet (unlike Gekko, nothing reads a
 * "this bitmap is already PVRTC" marker back out of a patched map), so this
 * target is diagnostic-only: real-encode eligible images via the port and
 * report what the size would be, without producing patchable output. Doesn't
 * fit `kernel_fn` (there's no format code / bytes to write back) -- kept as
 * its own small interface instead of forcing a shape that doesn't apply. */
struct probe_stats
{
    u32 encoded{0}, skipped_shape{0}, skipped_fmt{0};
    u32 src_bytes{0}, pvrtc_bytes{0};
};

inline void probe(
    format_t             src_fmt,
    gsl::span<u8 const>  src_px,
    u32                  src_size,
    u16                  w,
    u16                  h,
    probe_stats&         pv)
{
    // Each dimension must independently be power-of-two, >= 4px (one block).
    // Real PVRTC handles rectangular POT via twiddle_uv's tiled addressing;
    // width == height is not required.
    if(w < 4 || h < 4 || !mtx::is_pow2(w) || !mtx::is_pow2(h))
    {
        pv.skipped_shape++;
        return;
    }
    auto const rgba = mtx::decode_uncompressed_rgba(src_fmt, src_px, w, h);
    if(rgba.empty())
    {
        pv.skipped_fmt++;
        return;
    }
    // decode_uncompressed_rgba only unpacks R5G6B5/XRGB8/ARGB8 -- ARGB8 is the
    // only one of those with real alpha.
    bool const has_alpha = src_fmt == format_t::ARGB8;
    auto const enc       = encode_pvrtc_rect(rgba, w, h, has_alpha);
    pv.encoded++;
    pv.src_bytes += src_size;
    pv.pvrtc_bytes += static_cast<u32>(enc.size());
}

/*!
 * PVRTC supports RGB + RGBA (punchthrough alpha)
 * This covers most use-cases except the R/RG formats.
 * We'll passthrough the R/RG formats since they map nicely enough
 * We'll transcode lightmaps to PVRTC RGB
 */
inline mtx::kernel_fn make_kernel()
{
    return [](
               format_t             src_fmt,
               gsl::span<u8 const>  src_px,
               u32                  src_size,
               u16                  w,
               u16                  h,
               u16                  src_mip_count)
               -> std::optional<mtx::transcode_result> {
        std::vector<u8> tiled;
        u16       out_fmt = [&src_fmt] {
            switch(src_fmt)
            {
            case format_t::BC1:
            case format_t::R5G6B5:
                return 0x1;
            case format_t::ARGB8:
            case format_t::BC2:
            case format_t::BC3:
            case format_t::XRGB8:
                return 0x2;
            // These are widely supported on GL ES 2.0+
            case format_t::A1RGB5:
            case format_t::ARGB4:
            case format_t::A8Y8:
            case format_t::A8:
            case format_t::AY8:
            default:
                return 0x0;
            }
        }();
        u32             levels = 0;
        if(out_fmt == 0x0)
            return std::nullopt;

        levels            = src_mip_count < 1 ? 1 : src_mip_count;
        u32       src_off = 0;
        u16    out_levels = 0;
        for(u16 k = 0; k < levels; k++)
        {
            u16 const wl = static_cast<u16>(w >> k ? w >> k : 1);
            u16 const hl = static_cast<u16>(h >> k ? h >> k : 1);
            // encode_pvrtc_rect needs >= 1 block per axis (4px); below that
            // it silently returns no bytes, so stop the chain here instead
            // of wasting a decode+encode pass on nothing.
            if(wl < 4 || hl < 4)
                break;
            u32 const ssz = src_level_bytes(src_fmt, wl, hl);
            if(ssz == 0 || src_off + ssz > src_size)
                break; // ran out of source data

            // Decode just this level's slice (not the whole image -- every
            // level after the base one otherwise reads the wrong bytes at
            // the wrong length). decode_uncompressed_rgba already sizes its
            // output correctly and is reused as-is (BC1/BC2/BC3 aren't
            // decoded yet -- it returns empty for those; TODO wire a BCn
            // decoder in here once one exists).
            auto const rgba = decode_uncompressed_rgba(
                src_fmt, src_px.subspan(src_off, ssz), wl, hl);
            if(rgba.empty())
                break; // this source format isn't decodable yet

            auto out_data = encode_pvrtc_rect(rgba, wl, hl, out_fmt == 0x2);
            tiled.insert(tiled.end(), out_data.begin(), out_data.end());

            out_levels++;
            src_off += ssz;
        }
        if(out_levels == 0)
            return std::nullopt;
        return mtx::transcode_result{
            out_fmt,
            static_cast<u16>(out_levels > 0 ? out_levels - 1 : 0),
            std::move(tiled)};
    };
}

} // namespace powervr

} // namespace mtx
