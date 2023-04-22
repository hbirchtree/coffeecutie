#pragma once

#include <optional>
#include <string_view>

#include <peripherals/libc/types.h>
#include <peripherals/typing/enum/pixels/format.h>
#include <peripherals/typing/enum/pixels/format_transform.h>

#include "texture_format.h"

namespace gl::tex {

namespace detail {

using namespace std::string_view_literals;

#define COMPRESSED_FORMAT_TYPE texture_format_t::compression_t
#define RAW_FORMAT_TYPE texture_format_t::raw_format_t

constexpr texture_format_t texture_formats[] = {
#include "texture_formats.inl"
};
constexpr size_t num_texture_formats
    = sizeof(texture_formats) / sizeof(texture_format_t);

#undef COMPRESSED_FORMAT_TYPE
#undef RAW_FORMAT_TYPE

} // namespace detail

using texture_format_span
    = gsl::span<const texture_format_t, detail::num_texture_formats>;

inline constexpr texture_format_span texture_formats_view()
{
    return texture_format_span(detail::texture_formats);
}

inline constexpr bool is_compiled(format_t fmt)
{
    for(auto const& def : texture_formats_view())
        if(def.type == fmt)
            return true;
    return false;
}

inline constexpr texture_format_t const& format_of(format_t fmt)
{
    for(auto const& def : texture_formats_view())
        if(def.type == fmt)
            return def;
    throw std::out_of_range("format not found");
}

inline constexpr texture_format_t const& format_of(vk_format_t fmt)
{
    for(auto const& def : texture_formats_view())
        if(def.vk_type == fmt)
            return def;
    throw std::out_of_range("format not found");
}

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
            if(desc.pixflg == F::R)
                return format_of(format_t::compressed_red_rgtc1);
            if(desc.pixflg == (F::R | F::Signed))
                return format_of(format_t::compressed_signed_red_rgtc1);
            break;
#endif
#if defined(GL_COMPRESSED_RG_RGTC2)
        case C::BC5:
            if(desc.pixflg == F::RG)
                return format_of(format_t::compressed_rg_rgtc2);
            if(desc.pixflg == (F::RG | F::Signed))
                return format_of(format_t::compressed_signed_rg_rgtc2);
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

    case P::SRGB8:
        return format_of(format_t::srgb8);
    case P::SRGB8A8:
        return format_of(format_t::srgb8_alpha8);

    case P::Depth16:
        return format_of(format_t::depth_component16);
    case P::Depth24Stencil8:
        return format_of(format_t::depth24_stencil8);
    case P::Depth32F:
        return format_of(format_t::depth_component32f);
    case P::Depth32FStencil8:
        return format_of(format_t::depth32f_stencil8);
    default:
        break;
    }
    throw std::out_of_range("format not found");
}

} // namespace gl::tex
