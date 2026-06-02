#include "coffee/graphics/apis/gleam/rhi_versioning.h"
#include "glw/enums/InternalFormat.h"
#include "glw/enums/PixelFormat.h"
#include "glw/enums/PixelType.h"
#include "peripherals/typing/enum/pixels/format_transform.h"
#include <array>
#include <coffee/graphics/apis/gleam/rhi_translate.h>
#include <tuple>

#define MAGIC_ENUM_RANGE_MIN 0
#define MAGIC_ENUM_RANGE_MAX 1024
#include <peripherals/stl/magic_enum.hpp>

namespace gleam::convert {

template<typename T>
std::array<
    std::pair<
        typing::pixels::pix_fmt,
        std::tuple<T, group::pixel_type, group::pixel_format>>,
    26>
uncompressed_formats()
{
    using ::enum_helpers::feval;

    using f = T;
    using b = group::pixel_type;
    using p = group::pixel_format;

    using P = typing::pixels::pix_fmt;
    using F = typing::pixels::pix_flags;
    using C = typing::pix_components;
    using M = typing::pixels::comp_flags;

    constexpr std::array<std::pair<P, std::tuple<f, b, p>>, 26> direct_mapping =
        {{
#if GLEAM_MAX_VERSION >= 0x300 || GLEAM_MAX_VERSION_ES >= 0x300
            {P::R8, {f::r8, b::unsigned_byte, p::red}},
            {P::R16F, {f::r16f, b::half_float, p::red}},
            {P::R32F, {f::r32f, b::float_, p::red}},
            {P::RG8, {f::rg8, b::unsigned_byte, p::rg}},
            {P::RG16F, {f::rg16f, b::half_float, p::rg}},
            {P::RG32F, {f::rg32f, b::float_, p::rg}},
#else
            {P::R8,
             {static_cast<f>(0x1909),
              b::unsigned_byte,
              static_cast<p>(0x1909)}},
#endif

    /* RGB */
#if defined(GL_R3_G3_B2) && defined(GL_UNSIGNED_BYTE)
            {P::R3G3B2, {f::r3_g3_b2, b::unsigned_byte, p::rgb}},
#endif
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

            /* RGBA */
            {P::RGBA4, {f::rgba4, b::unsigned_short_4_4_4_4, p::rgba}},
            {P::RGB5A1, {f::rgb5_a1, b::unsigned_short_5_5_5_1, p::rgba}},
#if GLEAM_MAX_VERSION >= 0x300 || GLEAM_MAX_VERSION_ES >= 0x300
            {P::RGBA8, {f::rgba8, b::unsigned_byte, p::rgba}},
            {P::RGBA16F, {f::rgba16f, b::float_, p::rgba}},
            {P::RGBA32F, {f::rgba32f, b::float_, p::rgba}},
            {P::RGB10A2,
             {f::rgb10_a2, b::unsigned_int_2_10_10_10_rev, p::rgba}},
            {P::RGB10A2UI,
             {f::rgb10_a2ui, b::unsigned_int_2_10_10_10_rev, p::rgba}},
#endif

            /* Special formats */
            {P::Depth16,
             {f::depth_component16, b::unsigned_short, p::depth_component}},
#if GLEAM_MAX_VERSION >= 0x300 || GLEAM_MAX_VERSION_ES >= 0x300
            {P::Depth24,
             {f::depth_component24, b::unsigned_int, p::depth_component}},
            {P::Depth32,
             {f::depth_component24, b::unsigned_int, p::depth_component}},
            {P::Depth32F,
             {f::depth_component32f, b::float_, p::depth_component}},
            {P::Depth24Stencil8,
             {f::depth24_stencil8, b::unsigned_int_24_8, p::depth_stencil}},
            {P::Depth32FStencil8,
             {f::depth32f_stencil8,
              b::float_32_unsigned_int_24_8_rev,
              p::depth_stencil}},
#endif
        }};
    return direct_mapping;
}

template std::array<
    std::pair<
        typing::pixels::pix_fmt,
        std::tuple<
            group::internal_format,
            group::pixel_type,
            group::pixel_format>>,
    26>
uncompressed_formats();

template<typename T>
std::array<compressed_format_t<T>, 27> compressed_formats(
    features::textures const& features)
{
    using ::enum_helpers::feval;

    using f = T;
    using b = group::pixel_type;
    using p = group::pixel_format;

    using P = typing::pixels::pix_fmt;
    using F = typing::pixels::pix_flags;
    using C = typing::pix_components;
    using M = typing::pixels::comp_flags;

    using CF = typing::pixels::CompFmt;

    const bool rgtc =
        features.tex.gl.rgtc || features.tex.ext.rgtc || features.tex.arb.rgtc;
    const bool bptc =
        features.tex.gl.bptc || features.tex.ext.bptc || features.tex.arb.bptc;
    const bool etc2 = features.tex.gl.etc2;
    const bool astc = features.tex.gl.astc || features.tex.khr.astc;
    const bool s3tc = features.tex.ext.s3tc || features.tex.angle.s3tc;

    std::array<compressed_format_t<T>, 27> compressed_formats = {{
    // clang-format off
#if defined(GL_EXT_texture_compression_s3tc)
        {CF(P::BCn, F::RGB,  M::BC1), {f::compressed_rgb_s3tc_dxt1_ext,  b::unsigned_byte, p::rgb},  s3tc},
        {CF(P::BCn, F::RGBA, M::BC1), {f::compressed_rgba_s3tc_dxt1_ext, b::unsigned_byte, p::rgba}, s3tc},
        {CF(P::BCn, M::BC2),          {f::compressed_rgba_s3tc_dxt3_ext, b::unsigned_byte, p::rgba}, s3tc},
        {CF(P::BCn, M::BC3),          {f::compressed_rgba_s3tc_dxt5_ext, b::unsigned_byte, p::rgba}, s3tc},
#endif
#if GLEAM_MAX_VERSION >= 0x300
        {CF(P::BCn, M::BC4),  {f::compressed_red_rgtc1,               b::unsigned_byte, p::red},  rgtc},
        {CF(P::BCn, M::BC5),  {f::compressed_rg_rgtc2,                b::unsigned_byte, p::rg},   rgtc},
#endif
#if GLEAM_MAX_VERSION >= 0x420
        {CF(P::BCn, M::BC6H), {f::compressed_rgb_bptc_unsigned_float,  b::unsigned_byte, p::rgb},  bptc},
        {CF(P::BCn, M::BC7),  {f::compressed_rgba_bptc_unorm,          b::unsigned_byte, p::rgba}, bptc},
#endif
#if defined(GL_ETC1_RGB8_OES)
        {CF(P::ETC1), {f::etc1_rgb8_oes, b::unsigned_byte, p::rgb}, features.tex.oes.etc1},
#endif
#if GLEAM_MAX_VERSION >= 0x430 || GLEAM_MAX_VERSION_ES >= 0x300
        {CF(P::ETC2, F::R),                 {f::compressed_r11_eac,                       b::unsigned_byte, p::red},  etc2},
        {CF(P::ETC2, F::RG),                {f::compressed_rg11_eac,                      b::unsigned_byte, p::rg},   etc2},
        {CF(P::ETC2, F::RGB),               {f::compressed_rgb8_etc2,                     b::unsigned_byte, p::rgb},  etc2},
        {CF(P::ETC2, F::RGBA_Punchthrough), {f::compressed_rgb8_punchthrough_alpha1_etc2, b::unsigned_byte, p::rgb},  etc2},
        {CF(P::ETC2, F::RGBA),              {f::compressed_rgba8_etc2_eac,                b::unsigned_byte, p::rgba}, etc2},
#endif
#if defined(GL_KHR_texture_compression_astc_ldr) || GLEAM_MAX_VERSION_ES >= 0x320
        {CF(P::ASTC, M::ASTC_4x4),   {f::compressed_rgba_astc_4x4_khr,   b::unsigned_byte, p::rgba}, astc},
        {CF(P::ASTC, M::ASTC_8x8),   {f::compressed_rgba_astc_8x8_khr,   b::unsigned_byte, p::rgba}, astc},
        {CF(P::ASTC, M::ASTC_10x10), {f::compressed_rgba_astc_10x10_khr, b::unsigned_byte, p::rgba}, astc},
        {CF(P::ASTC, M::ASTC_12x12), {f::compressed_rgba_astc_12x12_khr, b::unsigned_byte, p::rgba}, astc},
#endif
#if defined(GL_OES_rgb8_rgba8)
        {P::RGBA8, {f::rgba8_oes, b::unsigned_byte, p::rgba}, features.tex.oes.rgba8},
        {P::RGB8,  {f::rgb8_oes,  b::unsigned_byte, p::rgb},  features.tex.oes.rgba8},
#endif
        // clang-format on
    }};
    return compressed_formats;
}

template std::array<compressed_format_t<group::internal_format>, 27>
compressed_formats(features::textures const& features);

template<typename T>
requires std::is_same_v<T, group::sized_internal_format> ||
         std::is_same_v<T, group::internal_format>
std::tuple<T, group::pixel_type, group::pixel_format> to(
    PixDesc const& fmt, [[maybe_unused]] features::textures const& features)
{
    using ::enum_helpers::feval;

    using f = T;
    using b = group::pixel_type;
    using p = group::pixel_format;

    using P = typing::pixels::pix_fmt;
    using F = typing::pixels::pix_flags;
    using C = typing::pix_components;
    using M = typing::pixels::comp_flags;

    if(auto fmt_ = detail::to_internal<T>(fmt);
       static_cast<u32>(std::get<0>(fmt_)) != 0)
    {
        return fmt_;
    }

    auto direct_mapping = uncompressed_formats<T>();

    auto it = std::find_if(
        direct_mapping.begin(),
        direct_mapping.end(),
        [pix = fmt.pixfmt](auto const& v) { return v.first == pix; });
    if(it != direct_mapping.end())
        return it->second;

    auto fmt_name = compile_info::debug_mode ? magic_enum::enum_name(fmt.pixfmt)
                                             : std::string_view();

    for(auto const& format : compressed_formats<T>(features))
    {
        const bool matching = fmt.pixfmt == format.desc.pixfmt &&
                              fmt.comp == format.desc.comp &&
                              fmt.cmpflg == format.desc.cmpflg;
        if(matching && format.condition)
            return format.out;
    }

    Throw(undefined_behavior(
        "unhandled pixel format: " +
        std::string(fmt_name.begin(), fmt_name.end())));
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
