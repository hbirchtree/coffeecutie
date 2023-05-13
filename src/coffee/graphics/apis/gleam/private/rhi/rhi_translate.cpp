#include <coffee/graphics/apis/gleam/rhi_translate.h>

#define MAGIC_ENUM_RANGE_MIN 0
#define MAGIC_ENUM_RANGE_MAX 1024
#include <magic_enum.hpp>

namespace gleam::convert {

template<typename T>
requires std::is_same_v<T, group::sized_internal_format> || std::
    is_same_v<T, group::internal_format>
        std::tuple<T, group::pixel_type, group::pixel_format> to(
            PixDesc const&                             fmt,
            [[maybe_unused]] features::textures const& features)
{
    using ::enum_helpers::feval;

    using f = T;
    using b = group::pixel_type;
    using p = group::pixel_format;

    using P = typing::pixels::PixFmt;
    using F = typing::pixels::PixFlg;
    using C = typing::PixCmp;
    using M = typing::pixels::CompFlags;

    if(auto fmt_ = detail::to_internal<T>(fmt);
       static_cast<u32>(std::get<0>(fmt_)) != 0)
    {
        return fmt_;
    }

    constexpr std::array<std::pair<P, std::tuple<f, b, p>>, 23> direct_mapping
        = {{
#if GLEAM_MAX_VERSION >= 0x300 || GLEAM_MAX_VERSION_ES >= 0x300
            {P::R8, {f::r8, b::unsigned_byte, p::red}},
            {P::R16F, {f::r16f, b::half_float, p::red}},
            {P::R32F, {f::r32f, b::float_, p::red}},
            {P::RG8, {f::rg8, b::unsigned_byte, p::rg}},
            {P::RG16F, {f::rg16f, b::half_float, p::rg}},
            {P::RG32F, {f::rg32f, b::float_, p::rg}},
#endif

    /* RGB */
#if defined(GL_RGB565) && defined(GL_UNSIGNED_SHORT_5_6_5)
            {P::RGB565, {f::rgb565, b::unsigned_short_5_6_5, p::rgb}},
#endif
#if GLEAM_MAX_VERSION >= 0x300 || GLEAM_MAX_VERSION_ES >= 0x300
            {P::RGB8, {f::rgb8, b::unsigned_byte, p::rgb}},
            {P::RGB16F, {f::rgb16f, b::half_float, p::rgb}},
            {P::RGB32F, {f::rgb32f, b::half_float, p::rgb}},
#endif
#if defined(GL_UNSIGNED_INT_5_9_9_9_REV)
            {P::RGB9E5, {f::rgb9_e5, b::unsigned_int_5_9_9_9_rev, p::rgba}},
#endif
#if defined(GL_UNSIGNED_INT_10F_11F_11F_REV)
            {P::R11G11B10F,
             {f::r11f_g11f_b10f, b::unsigned_int_10f_11f_11f_rev, p::rgb}},
#endif
#if defined(GL_ETC1_RGB8_OES)
            {P::ETC1, {f::etc1_rgb8_oes, b::unsigned_byte, p::rgb}},
#endif

            /* RGBA */
            {P::RGBA4, {f::rgba4, b::unsigned_short_4_4_4_4, p::rgba}},
            {P::RGB5A1, {f::rgb5_a1, b::unsigned_short_5_5_5_1, p::rgba}},
#if GLEAM_MAX_VERSION >= 0x300 || GLEAM_MAX_VERSION_ES >= 0x300
            {P::RGBA8, {f::rgba8, b::unsigned_byte, p::rgba}},
            {P::RGBA16F, {f::rgba16f, b::float_, p::rgba}},
            {P::RGBA32F, {f::rgba32f, b::float_, p::rgba}},
#endif

            /* Special formats */
            {P::Depth16,
             {f::depth_component16, b::unsigned_short, p::depth_component}},
//        {P::Depth16F, {f::depth_component, b::half_float,
//        p::depth_component}},
#if GLEAM_MAX_VERSION >= 0x300 || GLEAM_MAX_VERSION_ES >= 0x300
            {P::Depth32F,
             {f::depth_component32f, b::float_, p::depth_component}},
            {P::Depth24Stencil8,
             {f::depth24_stencil8, b::unsigned_byte, p::depth_stencil}},
            {P::Depth32FStencil8,
             {f::depth32f_stencil8, b::unsigned_byte, p::depth_stencil}},
#endif
        }};

    auto it = std::find_if(
        direct_mapping.begin(),
        direct_mapping.end(),
        [pix = fmt.pixfmt](auto const& v) { return v.first == pix; });
    if(it != direct_mapping.end())
        return it->second;

#if GLEAM_MAX_VERSION >= 0x300
    if(fmt.pixfmt == P::BCn && features.tex.gl.rgtc)
        switch(fmt.cmpflg)
        {
        case M::BC4:
            return {f::compressed_red_rgtc1, b::unsigned_byte, p::red};
        case M::BC5:
            return {f::compressed_rg_rgtc2, b::unsigned_byte, p::rg};
        default:
            break;
        }
#endif
#if GLEAM_MAX_VERSION >= 0x420
    if(fmt.pixfmt == P::BCn && features.tex.gl.bptc)
        switch(fmt.cmpflg)
        {
        case M::BC6H:
            return {
                f::compressed_rgb_bptc_unsigned_float,
                b::unsigned_byte,
                p::rgb};
        case M::BC7:
            return {f::compressed_rgba_bptc_unorm, b::unsigned_byte, p::rgba};
        default:
            break;
        }
#endif
#if GLEAM_MAX_VERSION >= 0x430 || GLEAM_MAX_VERSION_ES >= 0x300
    if(fmt.pixfmt == P::ETC2 && features.tex.gl.etc2)
    {
        if(feval(fmt.pixflg & F::RGB))
            return {f::compressed_rgb8_etc2, b::unsigned_byte, p::rgb};
        if(feval(fmt.pixflg & F::RGBA_Punchthrough))
            return {
                f::compressed_rgb8_punchthrough_alpha1_etc2,
                b::unsigned_byte,
                p::rgb};
        else
            return {f::compressed_rgba8_etc2_eac, b::unsigned_byte, p::rgb};
    }
#endif
#if GLEAM_MAX_VERSION_ES >= 0x320
    if(fmt.pixfmt == P::ASTC && features.tex.gl.astc)
        switch(fmt.cmpflg)
        {
        case M::ASTC_4x4:
            return {f::compressed_rgba_astc_4x4, b::unsigned_byte, p::rgba};
        case M::ASTC_8x8:
            return {f::compressed_rgba_astc_8x8, b::unsigned_byte, p::rgba};
        case M::ASTC_10x10:
            return {f::compressed_rgba_astc_10x10, b::unsigned_byte, p::rgba};
        case M::ASTC_12x12:
            return {f::compressed_rgba_astc_12x12, b::unsigned_byte, p::rgba};
        default:
            break;
        }
#endif

        /* Formats behind extensions */
#if defined(GL_EXT_texture_compression_s3tc)
    if(fmt.pixfmt == P::BCn && features.tex.ext.s3tc)
        switch(fmt.cmpflg)
        {
        case M::BC1: {
//            if(feval(fmt.pixflg & F::sRGB))
//            {
//                return {
//                    f::compressed_srgb_s3tc_dxt1_ext, b::unsigned_byte, p::rgb};
//            }
            switch(fmt.comp)
            {
            case C::RGB:
                return {
                    f::compressed_rgb_s3tc_dxt1_ext, b::unsigned_byte, p::rgb};
            case C::RGBA:
                return {
                    f::compressed_rgba_s3tc_dxt1_ext,
                    b::unsigned_byte,
                    p::rgba};
            default:
                break;
            }
            break;
        }
        case M::BC2:
            return {
                f::compressed_rgba_s3tc_dxt3_ext, b::unsigned_byte, p::rgba};
        case M::BC3:
            return {
                f::compressed_rgba_s3tc_dxt5_ext, b::unsigned_byte, p::rgba};
        default:
            break;
        };
#endif
#if defined(GL_ARB_texture_compression_rgtc)
    if(fmt.pixfmt == P::BCn && features.tex.arb.rgtc)
        switch(fmt.cmpflg)
        {
        case M::BC4:
            return {f::compressed_red_rgtc1, b::unsigned_byte, p::red};
        case M::BC5:
            return {f::compressed_rg_rgtc2, b::unsigned_byte, p::rg};
        default:
            break;
        }
#endif
#if defined(GL_ARB_texture_compression_bptc)
    if(fmt.pixfmt == P::BCn && features.tex.arb.bptc)
        switch(fmt.cmpflg)
        {
        case M::BC6H:
            return {
                f::compressed_rgb_bptc_unsigned_float,
                b::unsigned_byte,
                p::rgb};
        case M::BC7:
            return {f::compressed_rgba_bptc_unorm, b::unsigned_byte, p::rgba};
        default:
            break;
        }
#endif
#if defined(GL_KHR_texture_compression_astc_ldr)
    if(fmt.pixfmt == P::ASTC && features.tex.khr.astc)
        switch(fmt.cmpflg)
        {
        case M::ASTC_4x4:
            return {f::compressed_rgba_astc_4x4_khr, b::unsigned_byte, p::rgba};
        case M::ASTC_8x8:
            return {f::compressed_rgba_astc_8x8_khr, b::unsigned_byte, p::rgba};
        case M::ASTC_10x10:
            return {
                f::compressed_rgba_astc_10x10_khr, b::unsigned_byte, p::rgba};
        case M::ASTC_12x12:
            return {
                f::compressed_rgba_astc_12x12_khr, b::unsigned_byte, p::rgba};
        default:
            break;
        }
#endif

#if defined(GL_OES_rgb8_rgba8)
    if(fmt.pixfmt == P::RGBA8 && features.tex.oes.rgba8)
    {
        return {f::rgba8_oes, b::unsigned_byte, p::rgba};
    } else if(fmt.pixfmt == P::RGB8 && features.tex.oes.rgba8)
    {
        return {f::rgb8_oes, b::unsigned_byte, p::rgb};
    }
#endif
    auto fmt_name = compile_info::debug_mode ? magic_enum::enum_name(fmt.pixfmt)
                                             : std::string_view();
    Throw(undefined_behavior(
        "unhandled pixel format: "
        + std::string(fmt_name.begin(), fmt_name.end())));
}

template std::
    tuple<group::sized_internal_format, group::pixel_type, group::pixel_format>
    to<group::sized_internal_format>(
        PixDesc const&                             fmt,
        [[maybe_unused]] features::textures const& features);

template std::
    tuple<group::internal_format, group::pixel_type, group::pixel_format>
    to<group::internal_format>(
        PixDesc const&                             fmt,
        [[maybe_unused]] features::textures const& features);

} // namespace gleam::convert
