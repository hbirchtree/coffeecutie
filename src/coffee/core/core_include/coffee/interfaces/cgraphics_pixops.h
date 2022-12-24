#pragma once

#include <coffee/core/CMath>

#include <coffee/core/types/pixel_components.h>
#include <coffee/core/types/pixel_transform.h>
#include <coffee/core/types/size.h>

#include <peripherals/stl/any_of.h>

namespace Coffee {

FORCEDINLINE szptr GetPixSize(BitFmt fmt, PixCmp comp, szptr pixels)
{
    return typing::pixels::properties::get<
        typing::pixels::properties::pixel_size>(fmt, comp, pixels);
}

FORCEDINLINE u32 GetPixBlockSize(Size const& size, Size const& blockSize)
{
    const f32 block_w = blockSize.w;
    const f32 block_h = blockSize.h;

    const f32 width_block  = CMath::ceil(size.w / block_w);
    const f32 height_block = CMath::ceil(size.h / block_h);

    return static_cast<u32>(width_block) * static_cast<u32>(height_block);
}

FORCEDINLINE constexpr Size GetPixCompressedBlockSize(CompFmt format)
{
    auto block_dim =
        typing::pixels::properties::get<typing::pixels::properties::block_size>(
            format);

    return {static_cast<u32>(block_dim.w), static_cast<u32>(block_dim.h)};
}

FORCEDINLINE bool CompressedFormatSupportsSubTexture(CompFmt format)
{
    return typing::pixels::properties::get<
        typing::pixels::properties::supports_subtextures>(format);
}

/*!
 * \brief Size calculations from:
 *
 * S3TC/DXT1/DXT3/DXT5
 * https://www.khronos.org/registry/OpenGL/extensions/EXT/EXT_texture_compression_s3tc.txt
 * BC4/BC5
 * https://www.khronos.org/registry/OpenGL/extensions/EXT/EXT_texture_compression_rgtc.txt
 * BC6H/BC7
 * https://www.khronos.org/registry/OpenGL/extensions/ARB/ARB_texture_compression_bptc.txt
 *
 * ETC1
 * https://www.khronos.org/registry/OpenGL/extensions/OES/OES_compressed_ETC1_RGB8_texture.txt
 * ETC2
 * https://www.khronos.org/registry/OpenGL-Refpages/es3.0/html/glCompressedTexImage2D.xhtml
 *
 * ASTC
 * https://www.khronos.org/registry/OpenGL/extensions/KHR/KHR_texture_compression_astc_hdr.txt
 *
 * ATC
 * https://www.khronos.org/registry/OpenGL/extensions/AMD/AMD_compressed_ATC_texture.txt
 *
 * PVRTC
 * https://www.khronos.org/registry/OpenGL/extensions/IMG/IMG_texture_compression_pvrtc.txt
 * PVRTC2
 * https://www.khronos.org/registry/OpenGL/extensions/IMG/IMG_texture_compression_pvrtc2.txt
 *
 * \param format
 * \param tex_size
 * \return
 */
FORCEDINLINE constexpr szptr GetPixCompressedSize(
    CompFmt format, Size const& tex_size)
{
    using namespace ::enum_helpers;

    auto fmt    = format.base_fmt;
    auto cflags = format.c_flags;
    auto pflags = format.p_flags;

    const auto block_size = GetPixCompressedBlockSize(format);

    switch(fmt)
    {
    case PixFmt::BCn: /* Alias for BCn and DXTn */
    {
        if(one_of(cflags, CompFlags::BC1, CompFlags::BC2, CompFlags::BC3))
        {
            const u32 block_bytes = (cflags == CompFlags::BC1 ? 64 : 128) / 8;
            return GetPixBlockSize(tex_size, block_size) * block_bytes;

        } else if(one_of(cflags, CompFlags::BC4, CompFlags::BC5))
        {
            const u32 block_bytes = (feval(pflags & PixFlg::R) ? 64 : 128) / 8;
            return GetPixBlockSize(tex_size, block_size) * block_bytes;
        } else if(one_of(cflags, CompFlags::BC6H, CompFlags::BC7))
        {
            return GetPixBlockSize(tex_size, block_size) * (128 / 8);
        }
        break;
    }
    case PixFmt::ETC1:
    case PixFmt::ETC2:
    {
        u32 block_bytes = 0;

        if(fmt == PixFmt::ETC1)
            block_bytes = 64 / 8;
        else
        {
            switch(C_CAST<u32>(pflags))
            {
            case C_CAST<u32>(PixFlg::R):
            case C_CAST<u32>(PixFlg::R | PixFlg::Signed):
            case C_CAST<u32>(PixFlg::RGB):
            case C_CAST<u32>(PixFlg::RGB | PixFlg::sRGB):
            case C_CAST<u32>(PixFlg::RGBA | PixFlg::RGBA_Punchthrough):
            case C_CAST<u32>(
                PixFlg::RGBA | PixFlg::RGBA_Punchthrough | PixFlg::sRGB):
                block_bytes = 64 / 8;
                break;
            case C_CAST<u32>(PixFlg::RG):
            case C_CAST<u32>(PixFlg::RG | PixFlg::Signed):
            case C_CAST<u32>(PixFlg::RGBA):
            case C_CAST<u32>(PixFlg::RGBA | PixFlg::sRGB):
                block_bytes = 128 / 8;
                break;
            }
        }

        if(block_bytes)
            return GetPixBlockSize(tex_size, block_size) * block_bytes;

        break;
    }
    case PixFmt::ASTC:
    {
        constexpr u32 block_bytes = 128 / 8; /* Every ASTC block is 128 bits */

        return GetPixBlockSize(tex_size, block_size) * block_bytes;
    }
    case PixFmt::ATC:
    {
        u32 block_bytes = 8;

        if(feval(pflags & PixFlg::RGBA))
            block_bytes = 16;

        Size atc_size = {tex_size.w + 3, tex_size.h + 3};

        return GetPixBlockSize(atc_size, block_size) * block_bytes;
    }
    case PixFmt::PVRTC:
    {
        Size pvrtc_size = {CMath::max<u32>(tex_size.w, 8),
                           CMath::max<u32>(tex_size.h, 8)};
        u32  bpp        = 4;

        if(cflags == CompFlags::bpp_2)
        {
            bpp          = 2;
            pvrtc_size.w = CMath::max<u32>(pvrtc_size.w, 16);
        }

        return (C_FCAST<u32>(pvrtc_size.area()) * bpp + 7) / 8;
    }
    case PixFmt::PVRTC2:
    {
        return GetPixBlockSize(tex_size, block_size) * 8;
    }
    default:
        break;
    }

    // static_assert(false, "size calculation not implemented");
    // Throw(implementation_error("size calculation not implemented"));
    return 0;
}

FORCEDINLINE cstring GetPixCompressedExtension(CompFmt fmt)
{
    switch(fmt.base_fmt)
    {
    case PixFmt::S3TC: /* Also applies to BCn */
    {
        switch(fmt.c_flags)
        {
        case CompFlags::BC1:
            return "dxt1";
        case CompFlags::BC3:
            return "dxt5";
        case CompFlags::BC5:
            return "rgtc";
        case CompFlags::BC6H:
            return "bptch";
        case CompFlags::BC7:
            return "bptc";
        }
        break;
    }
    case PixFmt::ASTC:
        return "astc";
    case PixFmt::ETC1:
        return "etc1";
    case PixFmt::ETC2:
        return "etc2";
    default:
        return "";
    }

    return "";
}

FORCEDINLINE constexpr szptr PixDescSize(
    PixDesc const& desc, Size const& pix)
{
    using namespace typing::pixels::properties;

    if(get<is_compressed>(desc.pixfmt))
        return GetPixCompressedSize(desc.c, pix);
    else
        return GetPixSize(desc.bfmt, desc.comp, pix.area());
}

} // namespace Coffee
