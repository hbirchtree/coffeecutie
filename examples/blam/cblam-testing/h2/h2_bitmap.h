#pragma once

#include "h2_scene.h"

#include <blam/dimeter/bitm.h>
#include <blam/dimeter/bump_palette.h>
#include <blam/dimeter/map.h>
#include <blam/dimeter/vista.h>
#include <blam/volta/blam_swizzle.h>
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
    dim::tag_t const&            bitm_tag)
{
    auto const* bitm =
        bitm_tag.template data<dim::bitm::header_t>(map.magic());
    if(!bitm)
        return std::nullopt;
    auto images = bitm->images.data(map.magic());
    if(images.has_error() || images.value().empty())
        return std::nullopt;
    dim::bitm::image_t const& img = images.value()[0];
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

/* First image of a bitm tag with its mip chain, from whichever LOD is
 * present in the files we have */
template<typename V>
inline std::optional<std::shared_ptr<gfx::texture_2d_t>> upload_bitmap(
    gfx::api&                    api,
    dim::map_container<V> const& map,
    dim::raw_pool const&         pool,
    dim::tag_t const&            bitm_tag)
{
    auto const* bitm =
        bitm_tag.template data<dim::bitm::header_t>(map.magic());
    if(!bitm)
        return std::nullopt;
    auto images = bitm->images.data(map.magic());
    if(images.has_error() || images.value().empty())
        return std::nullopt;
    dim::bitm::image_t const& img = images.value()[0];
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
            if(!bytes || offset + bytes > lod_size)
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
            } else
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
            if(format == blam::bitm::format_t::V8U8)
                for(auto& b : level.pixels)
                    b = static_cast<u8>(b + 0x80);
            /* A8R8G8B8 is stored B,G,R,A */
            if(format == blam::bitm::format_t::ARGB8 ||
               format == blam::bitm::format_t::XRGB8)
                for(u32 i = 0; i + 3 < bytes; i += 4)
                {
                    std::swap(level.pixels[i], level.pixels[i + 2]);
                    if(format == blam::bitm::format_t::XRGB8)
                        level.pixels[i + 3] = 0xFF;
                }
            levels.push_back(std::move(level));
            offset += bytes;
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
        /* One- and two-channel formats land in R/RG; spread them */
        using gfx::textures::swizzle_t;
        switch(format)
        {
        case blam::bitm::format_t::Y8:
            tex->set_swizzle(
                swizzle_t::red, swizzle_t::red, swizzle_t::red, swizzle_t::one);
            break;
        case blam::bitm::format_t::AY8:
            tex->set_swizzle(
                swizzle_t::red, swizzle_t::red, swizzle_t::red, swizzle_t::red);
            break;
        case blam::bitm::format_t::A8:
            tex->set_swizzle(
                swizzle_t::one, swizzle_t::one, swizzle_t::one, swizzle_t::red);
            break;
        case blam::bitm::format_t::A8Y8:
            tex->set_swizzle(
                swizzle_t::red, swizzle_t::red, swizzle_t::red, swizzle_t::green);
            break;
        default:
            break;
        }
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


} // namespace h2
