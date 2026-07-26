#pragma once

#include "blam_base_types.h"

/* Original Xbox (NV2A) stores uncompressed/linear textures in a Morton/Z-order
 * swizzle (the bitm flag flags_t::swizzled marks these). Compressed (DXT)
 * textures are stored linearly and must NOT be deswizzled.
 *
 * The deswizzle works on caller-provided spans so the destination can be any
 * buffer (a temporary, or in future a mapped GPU upload buffer). It operates on
 * whole pixels: pick the element type T to match the bytes-per-pixel of the
 * format (u8 for 8-bit, u16 for 16-bit, u32 for 32-bit). */

namespace blam::swizzle {

namespace detail {

/* Distribute the bits of `value` into the set-bit positions of `pattern`. */
inline u32 fill_pattern(u32 pattern, u32 value)
{
    u32 result = 0;
    u32 bit    = 1;
    while(value)
    {
        if(pattern & bit)
        {
            if(value & 1)
                result |= bit;
            value >>= 1;
        }
        bit <<= 1;
    }
    return result;
}

/* Build the per-axis bit masks describing how x/y/z interleave in the swizzled
 * address. Handles non-square and non-power-of-two sizes: once an axis is fully
 * covered its remaining bits fall through to the larger axes. */
inline void generate_masks(
    u32 width, u32 height, u32 depth, u32& mask_x, u32& mask_y, u32& mask_z)
{
    mask_x = mask_y = mask_z = 0;
    u32  bit                 = 1;
    u32  mask_bit            = 1;
    bool done;
    do
    {
        done = true;
        if(bit < width)
        {
            mask_x |= mask_bit;
            mask_bit <<= 1;
            done = false;
        }
        if(bit < height)
        {
            mask_y |= mask_bit;
            mask_bit <<= 1;
            done = false;
        }
        if(bit < depth)
        {
            mask_z |= mask_bit;
            mask_bit <<= 1;
            done = false;
        }
        bit <<= 1;
    } while(!done);
}

} // namespace detail

/* Flat swizzled pixel index for texel (x, y, z) given precomputed axis masks.
 */
inline u32 swizzled_offset(
    u32 x, u32 y, u32 z, u32 mask_x, u32 mask_y, u32 mask_z)
{
    return detail::fill_pattern(mask_x, x) | detail::fill_pattern(mask_y, y) |
           detail::fill_pattern(mask_z, z);
}

/*!
 * \brief Deswizzle an Xbox Morton-swizzled image into a linear layout.
 * \param src swizzled source, one element (T) per pixel
 * \param dst linear destination, must hold width*height*depth elements
 * \return true on success, false if a span is too small
 *
 * T must match the bytes-per-pixel of the format (e.g. u16 for R5G6B5,
 * u32 for ARGB8, u8 for P8/A8).
 */
template<typename T>
inline bool deswizzle(
    Span<const T> src, Span<T> dst, u32 width, u32 height, u32 depth = 1)
{
    size_t count = static_cast<size_t>(width) * height * depth;
    if(dst.size() < count || src.size() < count)
        return false;

    u32 mask_x, mask_y, mask_z;
    detail::generate_masks(width, height, depth, mask_x, mask_y, mask_z);

    for(u32 z = 0; z < depth; z++)
        for(u32 y = 0; y < height; y++)
            for(u32 x = 0; x < width; x++)
            {
                u32 src_idx = swizzled_offset(x, y, z, mask_x, mask_y, mask_z);
                size_t dst_idx =
                    (static_cast<size_t>(z) * height + y) * width + x;
                dst[dst_idx] = src[src_idx];
            }
    return true;
}

/* Convenience: deswizzle raw bytes given bytes-per-pixel, dispatching to the
 * matching element width. Returns false for unsupported bpp (e.g. compressed,
 * which is never swizzled anyway). */
inline bool deswizzle_bytes(
    Span<const u8> src,
    Span<u8>       dst,
    u32            width,
    u32            height,
    u32            bytes_per_pixel,
    u32            depth = 1)
{
    auto as = [&](auto tag) {
        using T = decltype(tag);
        return deswizzle<T>(
            Span<const T>(
                reinterpret_cast<T const*>(src.data()), src.size() / sizeof(T)),
            Span<T>(reinterpret_cast<T*>(dst.data()), dst.size() / sizeof(T)),
            width,
            height,
            depth);
    };
    switch(bytes_per_pixel)
    {
    case 1:
        return as(u8{});
    case 2:
        return as(u16{});
    case 4:
        return as(u32{});
    default:
        return false;
    }
}

} // namespace blam::swizzle
