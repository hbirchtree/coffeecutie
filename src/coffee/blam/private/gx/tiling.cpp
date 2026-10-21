#include <blam/gx/tiling.h>

#include <cstring>

namespace blam::gx {

void tile_rgb565(u16 const* src, u16* dst, u16 w, u16 h, bool swap)
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

void tile_i8(u8 const* src, u8* dst, u16 w, u16 h)
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

void tile_ia8(u8 const* src, u8* dst, u16 w, u16 h)
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

void dxt_to_cmpr(u8 const* src, u8* dst, u16 w, u16 h, u32 block_size, u32 color_off)
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
