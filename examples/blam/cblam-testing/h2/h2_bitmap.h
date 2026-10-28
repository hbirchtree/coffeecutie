#pragma once

#include "h2_scene.h"
#include "../world/texels.h"

#include <blam/dimeter/bitm.h>
#include <blam/dimeter/bump_palette.h>
#include <blam/dimeter/map.h>
#include <blam/dimeter/views.h>
#include <blam/dimeter/vista.h>
#include <blam/common/swizzle.h>
#include <coffee/core/CDebug>
#include <coffee/graphics/apis/gleam/rhi.h>
#include <coffee/graphics/apis/gleam/rhi_texture.h>
#include <peripherals/typing/enum/pixels/format.h>
#include <peripherals/typing/enum/pixels/format_transform.h>

#include <cstring>
#include <optional>
#include <vector>

/* Halo 2 bitmaps onto the GPU, for the world's materials and the shell */
namespace h2 {

using blam::from_le;

/* Bytes of one mip level; 0 = unhandled format */
inline u32 level_bytes(blam::bitm::format_t fmt, u32 w, u32 h)
{
    using blam::bitm::format_t;
    switch(fmt)
    {
    case format_t::BC1:
        return std::max(w / 4, 1u) * std::max(h / 4, 1u) * 8;
    case format_t::BC2:
    case format_t::BC3:
        return std::max(w / 4, 1u) * std::max(h / 4, 1u) * 16;
    case format_t::A8:
    case format_t::Y8:
    case format_t::AY8:
    case format_t::P8:
        return w * h;
    case format_t::A8Y8:
    case format_t::V8U8:
    case format_t::R5G6B5:
    case format_t::A1RGB5:
    case format_t::ARGB4:
        return w * h * 2;
    case format_t::ARGB8:
    case format_t::XRGB8:
        return w * h * 4;
    default:
        return 0;
    }
}

inline bool is_compressed(blam::bitm::format_t fmt)
{
    using blam::bitm::format_t;
    return fmt == format_t::BC1 || fmt == format_t::BC2 || fmt == format_t::BC3;
}


/* Pixels in a side cache are the same for every map: the slot and
 * offset of the first lod identify them across loads */
template<typename V>
inline std::optional<u64> shared_key(
    dim::map_container<V> const& map,
    dim::raw_pool const&         pool,
    dim::tag_t const&            bitm_tag,
    size_t                       image = 0)
{
    auto const* image_at = dim::image_of(map, bitm_tag, image);
    if(!image_at)
        return std::nullopt;
    dim::bitm::image_t const& img = *image_at;
    for(int lod = 0; lod < 3; lod++)
    {
        if(!from_le(img.lod_size[lod]))
            continue;
        auto ref = pool.rebase(map.magic(), &img, img.lod_offset[lod]);
        if(!ref.valid() || ref.file() == dim::raw_file_t::local)
            return std::nullopt;
        return (static_cast<u64>(ref.file()) << 32) | ref.offset();
    }
    return std::nullopt;
}

/* One image of a bitm tag with its mip chain, from whichever LOD is
 * present in the files we have */
template<typename V>
inline std::optional<std::shared_ptr<gfx::texture_2d_t>> upload_bitmap(
    gfx::api&                    api,
    dim::map_container<V> const& map,
    dim::raw_pool const&         pool,
    dim::tag_t const&            bitm_tag,
    size_t                       image = 0)
{
    auto const* image_at = dim::image_of(map, bitm_tag, image);
    if(!image_at)
        return std::nullopt;
    dim::bitm::image_t const& img = *image_at;
    if(from_le(img.type) != blam::bitm::type_t::tex_2d)
        return std::nullopt;
    auto format = from_le(img.format);
    bool p8     = format == blam::bitm::format_t::P8;
    /* P8 bumps are expanded to RGBA8 normals at load */
    auto fmt = blam::bitm::to_pixdesc(p8 ? blam::bitm::format_t::ARGB8 : format);
    if(fmt.pixfmt == typing::pixels::pix_fmt::None)
        return std::nullopt;

    u32 width  = static_cast<u32>(from_le(img.width));
    u32 height = static_cast<u32>(from_le(img.height));
    u32 base   = level_bytes(format, width, height);
    if(!width || !height || !base)
        return std::nullopt;
    bool compressed = is_compressed(format);
    /* Without texture swizzle (WebGL) one- and two-channel formats go up
     * expanded to RGBA8 */
    auto const swizzle = world::channel_swizzle(format);
    bool const expand  = swizzle && !api.feature_info().texture.swizzle;
    if(expand)
        fmt = typing::pixels::PixDesc(typing::pixels::pix_fmt::RGBA8);
    u32  bpp        = compressed ? 0 : base / (width * height);
    u32  mip_count  = static_cast<u32>(std::max<i16>(from_le(img.mipmaps), 0)) + 1;

    for(int lod = 0; lod < 3; lod++)
    {
        u32 lod_size = from_le(img.lod_size[lod]);
        if(!lod_size)
            continue;
        auto raw = pool.resolve(map.magic(), &img, img.lod_offset[lod], lod_size);
        if(!raw)
            continue;
        auto const* src = reinterpret_cast<u8 const*>(raw->data());
        /* Vista: the lod is a zlib stream of the same layout */
        std::optional<std::vector<u8>> inflated;
        if(dim::vista::is_zlib(*raw))
        {
            inflated = dim::vista::inflate(*raw);
            if(!inflated)
                continue;
            src      = inflated->data();
            lod_size = static_cast<u32>(inflated->size());
        }
        if(lod_size < base)
            continue;

        /* Mips follow the base level back to back */
        struct level_t
        {
            u32             w, h;
            std::vector<u8> pixels;
        };
        std::vector<level_t> levels;
        u32                  offset = 0, lw = width, lh = height;
        while(levels.size() < mip_count)
        {
            u32 bytes = level_bytes(format, lw, lh);
            /* Linear rows are padded as stored */
            u32 const row    = compressed ? 0 : lw * bpp;
            u32 const pitch  = compressed ? 0 : img.row_pitch(row);
            u32 const stored = pitch != row ? pitch * lh : bytes;
            if(!bytes || offset + stored > lod_size)
                break;
            if(compressed && (lw < 4 || lh < 4))
                break;
            level_t level{.w = lw, .h = lh};
            level.pixels.resize(bytes);
            Span<const u8> in(src + offset, bytes);
            Span<u8>       out(level.pixels.data(), bytes);
            if(!compressed && img.swizzled())
            {
                if(!blam::swizzle::deswizzle_bytes(in, out, lw, lh, bpp))
                    std::memcpy(out.data(), in.data(), bytes);
            } else if(pitch != row)
                for(u32 y = 0; y < lh; y++)
                    std::memcpy(out.data() + y * row, src + offset + y * pitch, row);
            else
                std::memcpy(out.data(), in.data(), bytes);
            if(p8)
            {
                std::vector<u8> rgba(bytes * 4);
                dim::bitm::expand_p8(
                    level.pixels,
                    Span<dim::bitm::vecb4>(
                        reinterpret_cast<dim::bitm::vecb4*>(rgba.data()), bytes));
                level.pixels = std::move(rgba);
            }
            world::fix_channels(format, level.pixels);
            if(expand)
                level.pixels = world::expand_rgba8(format, *swizzle, level.pixels, lw, lh);
            levels.push_back(std::move(level));
            offset += stored;
            if(lw == 1 && lh == 1)
                break;
            lw = std::max(lw / 2, 1u);
            lh = std::max(lh / 2, 1u);
        }
        if(levels.empty())
            break;

        cDebug(
            "H2: bitm {}: {}x{} format {} lod{} ({} bytes) -> {} of {} "
            "mip levels{}",
            map.name_of(bitm_tag),
            width,
            height,
            static_cast<u16>(format),
            lod + 1,
            lod_size,
            levels.size(),
            mip_count,
            img.swizzled() && !compressed ? ", deswizzled" : "");
        auto tex = api.alloc_texture(
            gfx::textures::d2, fmt, static_cast<u32>(levels.size()));
        tex->alloc(size_3d<u32>{width, height, 1});
        if(swizzle && !expand)
            world::set_swizzle(*tex, *swizzle);
        for(size_t i = 0; i < levels.size(); i++)
        {
            auto& level = levels[i];
            Veci2 size{static_cast<i32>(level.w), static_cast<i32>(level.h)};
            tex->upload(
                std::move(level.pixels),
                Veci2{0, 0},
                size,
                static_cast<i32>(i));
        }
        return tex;
    }
    return std::nullopt;
}


/* A cube map. Xbox stores it face by face, each face with its whole mip
 * chain and padded to 128 bytes; faces go in the order Halo 1's do */
template<typename V>
inline std::optional<std::shared_ptr<gfx::texture_cube_t>> upload_cubemap(
    gfx::api&                    api,
    dim::map_container<V> const& map,
    dim::raw_pool const&         pool,
    dim::tag_t const&            bitm_tag)
{
    auto const* image_at = dim::image_of(map, bitm_tag);
    if(!image_at)
        return std::nullopt;
    dim::bitm::image_t const& img = *image_at;
    if(from_le(img.type) != blam::bitm::type_t::tex_cube)
        return std::nullopt;
    auto format = from_le(img.format);
    auto fmt    = blam::bitm::to_pixdesc(format);
    u32  width  = static_cast<u32>(from_le(img.width));
    u32  height = static_cast<u32>(from_le(img.height));
    u32  base   = level_bytes(format, width, height);
    if(fmt.pixfmt == typing::pixels::pix_fmt::None || !base)
        return std::nullopt;
    bool compressed = is_compressed(format);
    u32  bpp        = compressed ? 0 : base / (width * height);
    u32  mip_count  = static_cast<u32>(std::max<i16>(from_le(img.mipmaps), 0)) + 1;

    u32 face_bytes = 0, levels = 0;
    for(u32 i = 0, lw = width, lh = height; i < mip_count; i++)
    {
        face_bytes += level_bytes(format, lw, lh);
        if(!compressed || (lw >= 4 && lh >= 4))
            levels = i + 1;
        lw = std::max(lw / 2, 1u);
        lh = std::max(lh / 2, 1u);
    }
    u32 const stride = (face_bytes + 127u) & ~127u;

    u32 lod_size = from_le(img.lod_size[0]);
    auto raw = lod_size ? pool.resolve(map.magic(), &img, img.lod_offset[0], lod_size)
                        : std::nullopt;
    if(!raw || dim::vista::is_zlib(*raw) || lod_size < stride * 6)
        return std::nullopt;
    auto const* src = reinterpret_cast<u8 const*>(raw->data());

    auto tex = api.alloc_texture(gfx::textures::cube, fmt, levels);
    tex->alloc(size_3d<u32>{width, height, 1});
    /* TODO: unverified. Halo 1 swaps faces 1/2 only on PC (bitm cube_face),
     * this swaps them on Xbox data; check against a reference capture */
    constexpr std::array<u32, 6> face_order{0, 2, 1, 3, 4, 5};
    u32                          offset = 0;
    for(u32 level = 0, lw = width, lh = height; level < levels; level++)
    {
        u32 const                       bytes = level_bytes(format, lw, lh);
        std::array<std::vector<u8>, 6>  faces;
        std::array<Span<const u8>, 6>   spans;
        for(u32 f = 0; f < 6; f++)
        {
            Span<const u8> in(src + face_order[f] * stride + offset, bytes);
            faces[f].resize(bytes);
            if(!compressed && img.swizzled())
            {
                if(!blam::swizzle::deswizzle_bytes(
                       in, Span<u8>(faces[f].data(), bytes), lw, lh, bpp))
                    std::memcpy(faces[f].data(), in.data(), bytes);
            } else
                std::memcpy(faces[f].data(), in.data(), bytes);
            if(format == blam::bitm::format_t::ARGB8 ||
               format == blam::bitm::format_t::XRGB8)
                for(u32 i = 0; i + 3 < bytes; i += 4)
                {
                    std::swap(faces[f][i], faces[f][i + 2]);
                    if(format == blam::bitm::format_t::XRGB8)
                        faces[f][i + 3] = 0xFF;
                }
            spans[f] = Span<const u8>(faces[f].data(), bytes);
        }
        tex->upload(
            spans,
            Veci2{0, 0},
            Veci2{static_cast<i32>(lw), static_cast<i32>(lh)},
            static_cast<i32>(level));
        offset += bytes;
        lw = std::max(lw / 2, 1u);
        lh = std::max(lh / 2, 1u);
    }
    return tex;
}

} // namespace h2
