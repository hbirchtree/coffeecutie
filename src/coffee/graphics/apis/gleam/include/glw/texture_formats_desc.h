#pragma once

#include "texture_formats.h"

// #define MAGIC_ENUM_RANGE_MIN 0x8000
// #define MAGIC_ENUM_RANGE_MAX 0x9FFF
// #include <magic_enum.hpp>

namespace gl::tex {

using typing::pixels::PixDesc;

inline texture_format_t const& format_of(PixDesc const& desc)
{
    using C = typing::pixels::CompFlags;
    using P = typing::pixels::PixFmt;
    using F = typing::pixels::PixFlg;
    switch(desc.pixfmt)
    {
    case P::BCn:
        switch(desc.cmpflg)
        {
#if defined(GL_COMPRESSED_RGB_S3TC_DXT1_EXT)
        case C::BC1:
            return format_of(format_t::compressed_rgb_s3tc_dxt1_ext);
        case C::BC2:
            return format_of(format_t::compressed_rgba_s3tc_dxt3_ext);
        case C::BC3:
            return format_of(format_t::compressed_rgba_s3tc_dxt5_ext);
#endif
#if defined(GL_COMPRESSED_RED_RGTC1)
        case C::BC4:
            if(desc.pixflg == F::Signed)
                return format_of(format_t::compressed_signed_red_rgtc1);
            return format_of(format_t::compressed_red_rgtc1);
            break;
#endif
#if defined(GL_COMPRESSED_RG_RGTC2)
        case C::BC5:
            if(desc.pixflg == F::Signed)
                return format_of(format_t::compressed_signed_rg_rgtc2);
            return format_of(format_t::compressed_rg_rgtc2);
            break;
#endif
#if defined(GL_COMPRESSED_RGBA_BPTC_UNORM)
        case C::BC6H:
            return format_of(format_t::compressed_rgb_bptc_signed_float);
        case C::BC7:
            return format_of(format_t::compressed_rgba_bptc_unorm);
#endif
        default:
            break;
        }
        break;

#if defined(GL_ETC1_RGB8_OES)
    case P::ETC1:
        return format_of(format_t::etc1_rgb8_oes);
#endif
#if defined(GL_COMPRESSED_RGB8_ETC2)
    case P::ETC2:
        if(desc.pixflg == (F::R | F::Signed))
            return format_of(format_t::compressed_signed_r11_eac);
        if(desc.pixflg == (F::RG | F::Signed))
            return format_of(format_t::compressed_signed_rg11_eac);
        if(desc.pixflg == (F::sRGB | F::RGBA))
            return format_of(format_t::compressed_srgb8_alpha8_etc2_eac);
        if(desc.pixflg == F::sRGB)
            return format_of(format_t::compressed_srgb8_etc2);
        switch(desc.pixflg)
        {
        case F::R:
            return format_of(format_t::compressed_r11_eac);
        case F::RG:
            return format_of(format_t::compressed_rg11_eac);
        case F::RGB:
            return format_of(format_t::compressed_rgb8_etc2);
        case F::RGBA:
            return format_of(format_t::compressed_rgba8_etc2_eac);
        case F::RGBA_Punchthrough:
            return format_of(
                format_t::compressed_rgb8_punchthrough_alpha1_etc2);
        default:
            break;
        }
        break;
#endif
#if defined(GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG)
    case P::PVRTC:
        return format_of(
            desc.cmpflg == C::bpp_2
                ? desc.pixflg == F::RGB
                      ? format_t::compressed_rgb_pvrtc_2bppv1_img
                      : format_t::compressed_rgba_pvrtc_2bppv1_img
            : desc.pixflg == F::RGB
                ? format_t::compressed_rgb_pvrtc_4bppv1_img
                : format_t::compressed_rgba_pvrtc_4bppv1_img);
        break;
#endif
#if defined(GL_R16)
    /* For some reason, ES does not have these 16-bit formats */
    case P::R16:
        return format_of(format_t::r16);
    case P::RG16:
        return format_of(format_t::rg16);
    case P::RGB16:
        return format_of(format_t::rgb16);
    case P::RGBA16:
        return format_of(format_t::rgba16);
#endif
#if defined(GL_RGBA4)
    case P::RGBA4:
        return format_of(format_t::rgba4);
#endif

    case P::RGB565:
        return format_of(format_t::rgb565);
    case P::RGB5A1:
        return format_of(format_t::rgb5_a1);
    case P::RGB9E5:
        return format_of(format_t::rgb9_e5);

    case P::R8:
        return format_of(format_t::r8);
    case P::R8UI:
        return format_of(format_t::r8ui);
    case P::R8I:
        return format_of(format_t::r8i);
    case P::R16F:
        return format_of(format_t::r16f);
    case P::R16UI:
        return format_of(format_t::r16ui);
    case P::R16I:
        return format_of(format_t::r16i);
    case P::R32F:
        return format_of(format_t::r32f);
    case P::R32UI:
        return format_of(format_t::r32ui);
    case P::R32I:
        return format_of(format_t::r32i);

    case P::RG8:
        return format_of(format_t::rg8);
    case P::RG8UI:
        return format_of(format_t::rg8ui);
    case P::RG8I:
        return format_of(format_t::rg8i);
    case P::RG16F:
        return format_of(format_t::rg16f);
    case P::RG16UI:
        return format_of(format_t::rg16ui);
    case P::RG16I:
        return format_of(format_t::rg16i);
    case P::RG32F:
        return format_of(format_t::rg32f);
    case P::RG32UI:
        return format_of(format_t::rg32ui);
    case P::RG32I:
        return format_of(format_t::rg32i);

    case P::RGB8:
        return format_of(format_t::rgb8);
    case P::RGB8UI:
        return format_of(format_t::rgb8ui);
    case P::RGB8I:
        return format_of(format_t::rgb8i);
    case P::RGB16F:
        return format_of(format_t::rgb16f);
    case P::RGB16UI:
        return format_of(format_t::rgb16ui);
    case P::RGB16I:
        return format_of(format_t::rgb16i);
    case P::RGB32F:
        return format_of(format_t::rgb32f);
    case P::RGB32UI:
        return format_of(format_t::rgb32ui);
    case P::RGB32I:
        return format_of(format_t::rgb32i);

    case P::RGBA8:
        return format_of(format_t::rgba8);
    case P::RGBA8UI:
        return format_of(format_t::rgba8ui);
    case P::RGBA8I:
        return format_of(format_t::rgba8i);
    case P::RGBA16F:
        return format_of(format_t::rgba16f);
    case P::RGBA16UI:
        return format_of(format_t::rgba16ui);
    case P::RGBA16I:
        return format_of(format_t::rgba16i);
    case P::RGBA32F:
        return format_of(format_t::rgba32f);
    case P::RGBA32UI:
        return format_of(format_t::rgba32ui);
    case P::RGBA32I:
        return format_of(format_t::rgba32i);

    case P::RGB10A2:
        return format_of(format_t::rgb10_a2);
    case P::RGB10A2UI:
        return format_of(format_t::rgb10_a2ui);

    case P::SRGB8:
        return format_of(format_t::srgb8);
    case P::SRGB8A8:
        return format_of(format_t::srgb8_alpha8);

    case P::Depth16:
        return format_of(format_t::depth_component16);
    case P::Depth24: {
        static texture_format_t d24 = {
            .type = gl::group::internal_format::depth_component24,
            .raw_format =
                texture_format_t::raw_format_t{
                    .bit_layout =
                        {
                            .depth = 24,
                        },
                    .type            = texture_type_t::unsigned_int,
                    .format          = texture_layout_t::depth_component,
                    .pixel_size      = 3,
                    .component_count = 1,
                    .component_size  = 3,
                    .srgb            = false,
                    .floating_point  = false,
                },
            .version =
                {
                    .core = 0x140,
                    .es   = 0x300,
                    .web  = 0x200,
                },
        };
        return d24;
    }
    case P::Depth24Stencil8:
        return format_of(format_t::depth24_stencil8);
#if defined(GL_DEPTH_COMPONENT32)
    case P::Depth32:
        return format_of(format_t::depth_component32);
#endif
    case P::Depth32F:
        return format_of(format_t::depth_component32f);
    case P::Depth32FStencil8:
        return format_of(format_t::depth32f_stencil8);
    default:
        break;
    }
    throw std::out_of_range(
        "format not found: " +
        std::to_string(static_cast<uint32_t>(desc.pixfmt)));
}

inline PixDesc desc_of(texture_format_t const& fmt)
{
    using typing::PixCmp;
    using typing::pixels::CompFlags;
    using typing::pixels::CompFmt;
    using typing::pixels::PixDesc;
    using typing::pixels::PixFlg;
    using typing::pixels::PixFmt;
    switch(fmt.type)
    {
        /* ETC1 */
#if defined(GL_ETC1_RGB8_OES)
    case format_t::etc1_rgb8_oes:
        return CompFmt(PixFmt::ETC1);
#endif

        /* ETC2 */
#if defined(GL_COMPRESSED_R11_EAC)
    case format_t::compressed_r11_eac:
        return CompFmt(PixFmt::ETC2, PixFlg::R);
    case format_t::compressed_rg11_eac:
        return CompFmt(PixFmt::ETC2, PixFlg::RG);
    case format_t::compressed_rgb8_etc2:
        return CompFmt(PixFmt::ETC2, PixFlg::RGBA);
    case format_t::compressed_rgba8_etc2_eac:
        return CompFmt(PixFmt::ETC2, PixFlg::RGBA);
    case format_t::compressed_rgb8_punchthrough_alpha1_etc2:
        return CompFmt(PixFmt::ETC2, PixFlg::RGBA_Punchthrough);
#endif

        /* BCn */
#if defined(GL_COMPRESSED_RGB_S3TC_DXT1_EXT)
    case format_t::compressed_rgb_s3tc_dxt1_ext:
        return CompFmt(PixFmt::BCn, CompFlags::BC1);
    case format_t::compressed_rgba_s3tc_dxt3_ext:
        return CompFmt(PixFmt::BCn, CompFlags::BC2);
    case format_t::compressed_rgba_s3tc_dxt5_ext:
        return CompFmt(PixFmt::BCn, CompFlags::BC3);
#endif

#if defined(GL_COMPRESSED_RED_RGTC1)
    case format_t::compressed_red_rgtc1:
        return CompFmt(PixFmt::BCn, CompFlags::BC4);
#endif
#if defined(GL_COMPRESSED_RG_RGTC2)
    case format_t::compressed_rg_rgtc2:
        return CompFmt(PixFmt::BCn, CompFlags::BC5);
#endif

#if defined(GL_COMPRESSED_RGBA_BPTC_UNORM) || \
    defined(GL_COMPRESSED_RGBA_BPTC_UNORM_ARB)
    case format_t::compressed_rgb_bptc_signed_float:
        return CompFmt(
            PixFmt::BCn,
            PixFlg::Signed | PixFlg::FloatingPoint,
            CompFlags::BC6H);
    case format_t::compressed_rgb_bptc_unsigned_float:
        return CompFmt(
            PixFmt::BCn,
            PixFlg::Unsigned | PixFlg::FloatingPoint,
            CompFlags::BC6H);
    case format_t::compressed_rgba_bptc_unorm:
        return CompFmt(PixFmt::BCn, CompFlags::BC7);
#endif

#if defined(GL_R16)
    case format_t::r16:
        return PixDesc(PixFmt::R16);
    case format_t::rg16:
        return PixDesc(PixFmt::RG16);
    case format_t::rgb16:
        return PixDesc(PixFmt::RGB16);
    case format_t::rgba16:
        return PixDesc(PixFmt::RGBA16);
#endif

    case format_t::rgb565:
        return PixDesc(PixFmt::RGB565);
    case format_t::rgb5_a1:
        return PixDesc(PixFmt::RGB5A1);
    case format_t::rgb9_e5:
        return PixDesc(PixFmt::RGB9E5);

    case format_t::r8:
        return PixDesc(PixFmt::R8);
    case format_t::r8i:
        return PixDesc(PixFmt::R8I);
    case format_t::r8ui:
        return PixDesc(PixFmt::R8UI);
    case format_t::r16f:
        return PixDesc(PixFmt::R16F);
    case format_t::r16i:
        return PixDesc(PixFmt::R16I);
    case format_t::r16ui:
        return PixDesc(PixFmt::R16UI);
    case format_t::r32f:
        return PixDesc(PixFmt::R32F);
    case format_t::r32i:
        return PixDesc(PixFmt::R32I);
    case format_t::r32ui:
        return PixDesc(PixFmt::R32UI);

    case format_t::rg8:
        return PixDesc(PixFmt::RG8);
    case format_t::rg8i:
        return PixDesc(PixFmt::RG8I);
    case format_t::rg8ui:
        return PixDesc(PixFmt::RG8UI);
    case format_t::rg16f:
        return PixDesc(PixFmt::RG16F);
    case format_t::rg16i:
        return PixDesc(PixFmt::RG16I);
    case format_t::rg16ui:
        return PixDesc(PixFmt::RG16UI);
    case format_t::rg32f:
        return PixDesc(PixFmt::RG32F);
    case format_t::rg32i:
        return PixDesc(PixFmt::RG32I);
    case format_t::rg32ui:
        return PixDesc(PixFmt::RG32UI);

    case format_t::rgb8:
        return PixDesc(PixFmt::RGB8);
    case format_t::rgb8i:
        return PixDesc(PixFmt::RGB8I);
    case format_t::rgb8ui:
        return PixDesc(PixFmt::RGB8UI);
    case format_t::rgb16f:
        return PixDesc(PixFmt::RGB16F);
    case format_t::rgb16i:
        return PixDesc(PixFmt::RGB16I);
    case format_t::rgb16ui:
        return PixDesc(PixFmt::RGB16UI);
    case format_t::rgb32f:
        return PixDesc(PixFmt::RGB32F);
    case format_t::rgb32i:
        return PixDesc(PixFmt::RGB32I);
    case format_t::rgb32ui:
        return PixDesc(PixFmt::RGB32UI);

    case format_t::rgba8:
        return PixDesc(PixFmt::RGBA8);
    case format_t::rgba8i:
        return PixDesc(PixFmt::RGBA8I);
    case format_t::rgba8ui:
        return PixDesc(PixFmt::RGBA8UI);
    case format_t::rgba16f:
        return PixDesc(PixFmt::RGBA16F);
    case format_t::rgba16i:
        return PixDesc(PixFmt::RGBA16I);
    case format_t::rgba16ui:
        return PixDesc(PixFmt::RGBA16UI);
    case format_t::rgba32f:
        return PixDesc(PixFmt::RGBA32F);
    case format_t::rgba32i:
        return PixDesc(PixFmt::RGBA32I);
    case format_t::rgba32ui:
        return PixDesc(PixFmt::RGBA32UI);
    default:
        break;
    }
    throw std::out_of_range(
        "description not found: " +
        std::to_string(static_cast<uint32_t>(fmt.type)));
}

} // namespace gl::tex
