#pragma once

#include "rhi.h"
#include "rhi_texture.h"

#include <algorithm>
#include <cstring>
#include <vector>

namespace gleam {

using libc_types::u8;
using typing::vector_types::Veci2;

/*! Shelf-packs tiles into the layers of an array texture, reserving a wrap
 *  gutter around each one so that a filter tap at a tile edge lands on a copy
 *  of the opposite edge instead of on the neighbouring tile. Sizes are
 *  reserved up front, pixels arrive whenever they are decoded.
 *
 *  A tile spanning the layer on an axis gets no gutter there, because the
 *  sampler's own repeat wrap is already correct for it. */
struct texture_atlas_t
{
    struct slot_t
    {
        Veci2 offset{0, 0}; /*!< level-0 texel position of the tile */
        Veci2 size{0, 0};   /*!< level-0 tile size */
        Veci2 gutter{0, 0}; /*!< level-0 wrap padding, per axis */
        u32   layer{0};
    };

    struct reference_t
    {
        typing::vector_types::Vecf2 offset{0.f, 0.f};
        typing::vector_types::Vecf2 scale{1.f, 1.f};
        u32                         layer{0};
    };

    texture_atlas_t() = default;

    texture_atlas_t(Veci2 layer_size, u32 mipmaps)
        : m_layer_size(layer_size)
    {
        /* Four texels at the coarsest level: one compressed block, and enough
         * for the 4:1 anisotropic taps the samplers ask for. Halving per level
         * leaves every level block-aligned. */
        m_grain = 4 << (mipmaps > 0 ? mipmaps - 1 : 0);
        /* Two gutters and a tile share a layer, and the largest tile that gets
         * packed rather than given a layer of its own is half of one. A
         * smaller grain is no use: halving it costs the block alignment that
         * makes it work in the first place. */
        if(m_grain > std::min(layer_size[0], layer_size[1]) / 4)
            m_grain = 0;
    }

    slot_t reserve(Veci2 size)
    {
        slot_t slot{
            .size   = size,
            .gutter = {fits(size[0], m_layer_size[0]),
                       fits(size[1], m_layer_size[1])},
        };

        const i32 stride = align_up(size[0] + slot.gutter[0] * 2);
        if(m_shelf > 0 && m_shelf + stride > m_layer_size[0])
        {
            m_layer++;
            m_shelf = 0;
        }
        slot.offset = {m_shelf + slot.gutter[0], slot.gutter[1]};
        slot.layer  = m_layer;
        m_shelf += stride;
        return slot;
    }

    /*! A layer to itself, for a tile that cannot share one. It needs no
     *  gutter: nothing else is in reach of its taps. */
    slot_t reserve_layer(Veci2 size)
    {
        if(m_shelf > 0)
        {
            m_layer++;
            m_shelf = 0;
        }
        m_shelf = m_layer_size[0];
        return slot_t{.size = size, .layer = m_layer};
    }

    /*! The normalized rect the shader samples with. The gutter sits outside
     *  it and is only ever reached by filter taps. */
    reference_t reference_of(slot_t const& slot) const
    {
        return {
            .offset =
                {
                    static_cast<f32>(slot.offset[0]) / m_layer_size[0],
                    static_cast<f32>(slot.offset[1]) / m_layer_size[1],
                },
            .scale =
                {
                    static_cast<f32>(slot.size[0]) / m_layer_size[0],
                    static_cast<f32>(slot.size[1]) / m_layer_size[1],
                },
            .layer = slot.layer,
        };
    }

    inline u32 layers() const
    {
        return m_layer + 1;
    }

    inline Veci2 layer_size() const
    {
        return m_layer_size;
    }

  private:
    /*! The gutter for one axis: none when the tile spans the layer, and none
     *  when two of them plus the tile would not fit beside each other. */
    inline i32 fits(i32 size, i32 layer) const
    {
        return size < layer && size + m_grain * 2 <= layer ? m_grain : 0;
    }

    /*! Placements land on grain boundaries, so shifting one down a mip level
     *  stays exact and block-aligned the whole way. */
    inline i32 align_up(i32 v) const
    {
        return m_grain > 0 ? (v + m_grain - 1) / m_grain * m_grain : v;
    }

    Veci2 m_layer_size{0, 0};
    i32   m_grain{0};
    i32   m_shelf{0};
    u32   m_layer{0};
};

/*! A tile expanded with its wrap gutter, ready to upload. */
struct padded_tile_t
{
    Veci2                  offset{0, 0};
    Veci2                  size{0, 0};
    semantic::Span<const u8> data;
};

/*! Wraps `data` -- a tile of `size` at mip `level`, placed at level-0 texel
 *  `offset` -- into `scratch`, ringed by copies of its own opposite edges,
 *  and reports where the padded rect lands. Compressed formats copy whole
 *  blocks, so a gutter that no longer spans one at this level is dropped. */
inline padded_tile_t pad_tile(
    std::vector<u8>&                 scratch,
    gl::tex::texture_format_t const& fmt,
    Veci2                            offset,
    Veci2                            gutter,
    u32                              level,
    Veci2                            size,
    semantic::Span<const u8>         data)
{
    i32    block_w = 1, block_h = 1;
    size_t unit = 0;
    if(fmt.is_compressed())
    {
        block_w = fmt.compression->block_width;
        block_h = fmt.compression->block_height;
        unit    = fmt.compression->block_size;
    } else if(fmt.raw_format.has_value())
        unit = fmt.raw_format->pixel_size;

    padded_tile_t plain{
        Veci2{(offset[0] >> level) / block_w * block_w,
              (offset[1] >> level) / block_h * block_h},
        size,
        data};

    /* A gutter narrower than a block cannot be expressed, and one wider than
     * the tile would need more than a single wrap period. */
    const Veci2 pad{
        std::min<i32>((gutter[0] >> level) / block_w * block_w, size[0]),
        std::min<i32>((gutter[1] >> level) / block_h * block_h, size[1])};

    if(unit == 0 || size[0] < block_w || size[1] < block_h ||
       (pad[0] == 0 && pad[1] == 0))
        return plain;

    const i32 tile_w = size[0] / block_w, tile_h = size[1] / block_h;
    const i32 pad_w = pad[0] / block_w, pad_h = pad[1] / block_h;
    const i32 dst_w = tile_w + pad_w * 2, dst_h = tile_h + pad_h * 2;

    if(data.size_bytes() < static_cast<size_t>(tile_w) * tile_h * unit)
        return plain;

    scratch.resize(static_cast<size_t>(dst_w) * dst_h * unit);
    for(i32 y = 0; y < dst_h; y++)
    {
        const i32 src_y = ((y - pad_h) % tile_h + tile_h) % tile_h;
        const u8* src = data.data() + static_cast<size_t>(src_y) * tile_w * unit;
        u8*       dst = scratch.data() + static_cast<size_t>(y) * dst_w * unit;

        std::memcpy(dst, src + (tile_w - pad_w) * unit, pad_w * unit);
        std::memcpy(dst + pad_w * unit, src, tile_w * unit);
        std::memcpy(dst + (pad_w + tile_w) * unit, src, pad_w * unit);
    }

    return padded_tile_t{
        Veci2{plain.offset[0] - pad[0], plain.offset[1] - pad[1]},
        Veci2{dst_w * block_w, dst_h * block_h},
        semantic::Span<const u8>(scratch.data(), scratch.size()),
    };
}

} // namespace gleam
