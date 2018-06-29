#pragma once

#include <coffee/core/types/cdef/geometry.h>
#include <coffee/core/types/edef/pixenum.h>
#include <coffee/core/types/edef/resenum.h>

#include <coffee/core/type_safety.h>
#include <coffee/core/types/tdef/standard_exceptions.h>
#include <coffee/core/types/tdef/stltypes.h>

namespace Coffee {

FORCEDINLINE szptr GetPixSize(BitFormat fmt, PixCmp comp, szptr pixels)
{
    using B = BitFormat;

    szptr pxsz = 0;
    switch(fmt)
    {
    case B::Byte:
    case B::ByteR:
    case B::UByte:
    case B::UByteR:
    case B::UByte_332:
    case B::UByte_233R:
        pxsz = 1;
        break;
    case B::Short:
    case B::ShortR:
    case B::UShort:
    case B::UShortR:
    case B::UShort_4444:
    case B::UShort_4444R:
    case B::UShort_565:
    case B::UShort_565R:
    case B::UShort_5551:
    case B::UShort_1555R:
    case B::Scalar_16:
        pxsz = 2;
        break;
    case B::Int:
    case B::IntR:
    case B::UInt:
    case B::UIntR:
    case B::UInt_5999R:
    case B::UInt_1010102:
    case B::UInt_2101010R:
    case B::Scalar_32:
    case B::Scalar_11_11_10:
    case B::UInt24_8:
        pxsz = 4;
        break;
    case B::Scalar_64:
    case B::Scalar_32_Int_24_8:
        pxsz = 8;
        break;

    default:
        Throw(implementation_error("size calculation not implemented"));
    }
    switch(comp)
    {
    case PixCmp::R:
    case PixCmp::G:
    case PixCmp::B:
    case PixCmp::A:
    case PixCmp::Stencil:
    case PixCmp::Depth:
    case PixCmp::DepthStencil:
        pxsz *= 1;
        break;
    case PixCmp::RG:
        pxsz *= 2;
        break;
    case PixCmp::RGB:
    case PixCmp::BGR:
        pxsz *= 3;
        break;
    case PixCmp::RGBA:
    case PixCmp::BGRA:
        pxsz *= 4;
        break;

    default:
        Throw(implementation_error("size calculation not implemented"));
    }

    return pxsz * pixels;
}

FORCEDINLINE u32 GetPixBlockSize(Size const& size, Size const& blockSize)
{
    const f32 block_w = blockSize.w;
    const f32 block_h = blockSize.h;

    const f32 width_block  = CMath::ceil(size.w / block_w);
    const f32 height_block = CMath::ceil(size.h / block_h);

    return C_CAST<u32>(width_block) * C_CAST<u32>(height_block);
}

FORCEDINLINE Size GetPixCompressedBlockSize(CompFmt format)
{
    switch(format.base_fmt)
    {
    case PixFmt::ETC1:
    case PixFmt::ETC2:
    case PixFmt::S3TC:
    case PixFmt::ATC:
        return {4, 4};
    case PixFmt::ASTC:
    {
        static_assert(
            C_CAST<u8>(CompFlags::ASTC_4x4) == 1, "assumption broken");
        static_assert(
            C_CAST<u8>(CompFlags::ASTC_12x12) == 14, "assumption broken");

        static const constexpr Array<Size, 14> ASTC_Block_Sizes = {{
            {4, 4},

            {5, 4},
            {5, 5},

            {6, 5},
            {6, 6},

            {8, 5},
            {8, 6},
            {8, 8},

            {10, 5},
            {10, 6},
            {10, 8},
            {10, 10},

            {12, 10},
            {12, 12},
        }};
        const u8                               block_idx =
            C_CAST<u8>(format.c_flags) - C_CAST<u8>(CompFlags::ASTC_4x4);

        return ASTC_Block_Sizes.at(block_idx);
    }
    case PixFmt::PVRTC2:
    {
        Size block_size = {4, 4};

        if(feval(format.c_flags & CompFlags::bpp_2))
            block_size.w = 8;

        return block_size;
    }
    default:
        break;
    }

    return {1, 1};
}

FORCEDINLINE bool CompressedFormatSupportsSubTexture(CompFmt format)
{
    switch(format.base_fmt)
    {
    case PixFmt::PVRTC:
    case PixFmt::ATC:
    case PixFmt::ETC1:
        return false;

    default:
        return true;
    }
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
FORCEDINLINE szptr GetPixCompressedSize(CompFmt format, Size const& tex_size)
{
    auto fmt    = format.base_fmt;
    auto cflags = format.c_flags;
    auto pflags = format.p_flags;

    const auto block_size = GetPixCompressedBlockSize(format);

    switch(fmt)
    {
    case PixFmt::S3TC: /* Alias for BCn and DXTn */
    {
        if(cflags == CompFlags::S3TC_1 || cflags == CompFlags::S3TC_3 ||
           cflags == CompFlags::S3TC_5)
        {
            u32 block_bytes = 128 / 8;

            if(cflags == CompFlags::S3TC_1)
                block_bytes = 64 / 8;

            return GetPixBlockSize(tex_size, block_size) * block_bytes;

        } else if(cflags == CompFlags::BC4 || cflags == CompFlags::BC5)
        {
            u32 block_bytes = 0;

            if(feval(pflags & PixFlg::R))
                block_bytes = 64 / 8;
            else if(feval(pflags & PixFlg::RG))
                block_bytes = 128 / 8;

            if(block_bytes)
                return GetPixBlockSize(tex_size, block_size) * block_bytes;

        } else if(cflags == CompFlags::BC6H || cflags == CompFlags::BC7)
        {
            const u32 block_bytes = 128 / 8;

            return GetPixBlockSize(tex_size, block_size) * block_bytes;
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
        Size pvrtc_size = {CMath::max(tex_size.w, 8),
                           CMath::max(tex_size.h, 8)};
        u32  bpp        = 4;

        if(feval(cflags & CompFlags::bpp_2))
        {
            bpp          = 2;
            pvrtc_size.w = CMath::max(pvrtc_size.w, 16);
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

    Throw(implementation_error("size calculation not implemented"));
}

} // namespace Coffee
