#pragma once

#include <peripherals/enum/helpers.h>
#include <stdexcept>

#include "texture_formats.h"

// #define MAGIC_ENUM_RANGE_MIN 0x8000
// #define MAGIC_ENUM_RANGE_MAX 0x9FFF
// #include <magic_enum.hpp>

namespace gl::tex {

using typing::pixels::PixDesc;

inline texture_format_t const& format_of(PixDesc const& desc)
{
    using enum_helpers::feval;
    using C = typing::pixels::comp_flags;
    using P = typing::pixels::pix_fmt;
    using F = typing::pixels::pix_flags;
    switch(desc.pixfmt)
    {
    case P::BCn:
        switch(desc.cmpflg)
        {
#if defined(GL_COMPRESSED_RGB_S3TC_DXT1_EXT)
        case C::BC1:
            return format_of(format_t::compressed_rgb_s3tc_dxt1_ext);
#endif
#if defined(GL_COMPRESSED_RGBA_S3TC_DXT3_EXT)
        case C::BC2:
            return format_of(format_t::compressed_rgba_s3tc_dxt3_ext);
#endif
#if defined(GL_COMPRESSED_RGBA_S3TC_DXT5_EXT)
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
#if defined(GL_IMG_texture_compression_pvrtc)
    case P::PVRTC:
        return format_of(
            feval(desc.cmpflg, C::PVRTC_BPP2)
                ? feval(desc.cmpflg, C::PVRTC_RGB)
                      ? format_t::compressed_rgb_pvrtc_2bppv1_img
                      : format_t::compressed_rgba_pvrtc_2bppv1_img
            : feval(desc.cmpflg, C::PVRTC_RGB)
                ? format_t::compressed_rgb_pvrtc_4bppv1_img
                : format_t::compressed_rgba_pvrtc_4bppv1_img);
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
#if defined(GL_RGB9_E5)
    case P::RGB9E5:
        return format_of(format_t::rgb9_e5);
#endif

#if defined(GL_R8)
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
#endif

#if defined(GL_SRGB8)
    case P::SRGB8:
        return format_of(format_t::srgb8);
    case P::SRGB8A8:
        return format_of(format_t::srgb8_alpha8);
#endif

    case P::Depth16:
        return format_of(format_t::depth_component16);
#if defined(GL_DEPTH_COMPONENT24)
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
#endif
#if defined(GL_DEPTH24_STENCIL8)
    case P::Depth24Stencil8:
        return format_of(format_t::depth24_stencil8);
#endif
#if defined(GL_DEPTH_COMPONENT32)
    case P::Depth32:
        return format_of(format_t::depth_component32);
#endif
#if defined(GL_DEPTH_COMPONENT32F)
    case P::Depth32F:
        return format_of(format_t::depth_component32f);
#endif
#if defined(GL_DEPTH32F_STENCIL8)
    case P::Depth32FStencil8:
        return format_of(format_t::depth32f_stencil8);
#endif
    default:
        break;
    }
    throw std::out_of_range(
        "format not found: " +
        std::to_string(static_cast<uint32_t>(desc.pixfmt)));
}

inline PixDesc desc_of(texture_format_t const& fmt)
{
    using typing::pix_components;
    using typing::pixels::comp_flags;
    using typing::pixels::CompFmt;
    using typing::pixels::pix_flags;
    using typing::pixels::pix_fmt;
    using typing::pixels::PixDesc;
    switch(fmt.type)
    {
        /* ETC1 */
#if defined(GL_ETC1_RGB8_OES)
    case format_t::etc1_rgb8_oes:
        return CompFmt(pix_fmt::ETC1);
#endif

        /* ETC2 */
#if defined(GL_COMPRESSED_R11_EAC)
    case format_t::compressed_r11_eac:
        return CompFmt(pix_fmt::ETC2, pix_flags::R);
    case format_t::compressed_rg11_eac:
        return CompFmt(pix_fmt::ETC2, pix_flags::RG);
    case format_t::compressed_rgb8_etc2:
        return CompFmt(pix_fmt::ETC2, pix_flags::RGBA);
    case format_t::compressed_rgba8_etc2_eac:
        return CompFmt(pix_fmt::ETC2, pix_flags::RGBA);
    case format_t::compressed_rgb8_punchthrough_alpha1_etc2:
        return CompFmt(pix_fmt::ETC2, pix_flags::RGBA_Punchthrough);
#endif

        /* BCn */
#if defined(GL_COMPRESSED_RGB_S3TC_DXT1_EXT)
    case format_t::compressed_rgb_s3tc_dxt1_ext:
        return CompFmt(pix_fmt::BCn, comp_flags::BC1);
    case format_t::compressed_rgba_s3tc_dxt3_ext:
        return CompFmt(pix_fmt::BCn, comp_flags::BC2);
    case format_t::compressed_rgba_s3tc_dxt5_ext:
        return CompFmt(pix_fmt::BCn, comp_flags::BC3);
#endif

#if defined(GL_COMPRESSED_RED_RGTC1)
    case format_t::compressed_red_rgtc1:
        return CompFmt(pix_fmt::BCn, comp_flags::BC4);
#endif
#if defined(GL_COMPRESSED_RG_RGTC2)
    case format_t::compressed_rg_rgtc2:
        return CompFmt(pix_fmt::BCn, comp_flags::BC5);
#endif

#if defined(GL_COMPRESSED_RGBA_BPTC_UNORM) || \
    defined(GL_COMPRESSED_RGBA_BPTC_UNORM_ARB)
    case format_t::compressed_rgb_bptc_signed_float:
        return CompFmt(
            pix_fmt::BCn,
            pix_flags::Signed | pix_flags::FloatingPoint,
            comp_flags::BC6H);
    case format_t::compressed_rgb_bptc_unsigned_float:
        return CompFmt(
            pix_fmt::BCn,
            pix_flags::Unsigned | pix_flags::FloatingPoint,
            comp_flags::BC6H);
    case format_t::compressed_rgba_bptc_unorm:
        return CompFmt(pix_fmt::BCn, comp_flags::BC7);
#endif

#if defined(GL_R16)
    case format_t::r16:
        return PixDesc(pix_fmt::R16);
    case format_t::rg16:
        return PixDesc(pix_fmt::RG16);
    case format_t::rgb16:
        return PixDesc(pix_fmt::RGB16);
    case format_t::rgba16:
        return PixDesc(pix_fmt::RGBA16);
#endif

    case format_t::rgb565:
        return PixDesc(pix_fmt::RGB565);
    case format_t::rgb5_a1:
        return PixDesc(pix_fmt::RGB5A1);
#if defined(GL_RGB9_E5)
    case format_t::rgb9_e5:
        return PixDesc(pix_fmt::RGB9E5);
#endif

#if defined(GL_R8)
    case format_t::r8:
        return PixDesc(pix_fmt::R8);
    case format_t::r8i:
        return PixDesc(pix_fmt::R8I);
    case format_t::r8ui:
        return PixDesc(pix_fmt::R8UI);
    case format_t::r16f:
        return PixDesc(pix_fmt::R16F);
    case format_t::r16i:
        return PixDesc(pix_fmt::R16I);
    case format_t::r16ui:
        return PixDesc(pix_fmt::R16UI);
    case format_t::r32f:
        return PixDesc(pix_fmt::R32F);
    case format_t::r32i:
        return PixDesc(pix_fmt::R32I);
    case format_t::r32ui:
        return PixDesc(pix_fmt::R32UI);

    case format_t::rg8:
        return PixDesc(pix_fmt::RG8);
    case format_t::rg8i:
        return PixDesc(pix_fmt::RG8I);
    case format_t::rg8ui:
        return PixDesc(pix_fmt::RG8UI);
    case format_t::rg16f:
        return PixDesc(pix_fmt::RG16F);
    case format_t::rg16i:
        return PixDesc(pix_fmt::RG16I);
    case format_t::rg16ui:
        return PixDesc(pix_fmt::RG16UI);
    case format_t::rg32f:
        return PixDesc(pix_fmt::RG32F);
    case format_t::rg32i:
        return PixDesc(pix_fmt::RG32I);
    case format_t::rg32ui:
        return PixDesc(pix_fmt::RG32UI);

    case format_t::rgb8:
        return PixDesc(pix_fmt::RGB8);
    case format_t::rgb8i:
        return PixDesc(pix_fmt::RGB8I);
    case format_t::rgb8ui:
        return PixDesc(pix_fmt::RGB8UI);
    case format_t::rgb16f:
        return PixDesc(pix_fmt::RGB16F);
    case format_t::rgb16i:
        return PixDesc(pix_fmt::RGB16I);
    case format_t::rgb16ui:
        return PixDesc(pix_fmt::RGB16UI);
    case format_t::rgb32f:
        return PixDesc(pix_fmt::RGB32F);
    case format_t::rgb32i:
        return PixDesc(pix_fmt::RGB32I);
    case format_t::rgb32ui:
        return PixDesc(pix_fmt::RGB32UI);

    case format_t::rgba8:
        return PixDesc(pix_fmt::RGBA8);
    case format_t::rgba8i:
        return PixDesc(pix_fmt::RGBA8I);
    case format_t::rgba8ui:
        return PixDesc(pix_fmt::RGBA8UI);
    case format_t::rgba16f:
        return PixDesc(pix_fmt::RGBA16F);
    case format_t::rgba16i:
        return PixDesc(pix_fmt::RGBA16I);
    case format_t::rgba16ui:
        return PixDesc(pix_fmt::RGBA16UI);
    case format_t::rgba32f:
        return PixDesc(pix_fmt::RGBA32F);
    case format_t::rgba32i:
        return PixDesc(pix_fmt::RGBA32I);
    case format_t::rgba32ui:
        return PixDesc(pix_fmt::RGBA32UI);
#endif
    default:
        break;
    }
    throw std::out_of_range(
        "description not found: " +
        std::to_string(static_cast<uint32_t>(fmt.type)));
}

} // namespace gl::tex
