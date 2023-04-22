#define MAGIC_ENUM_RANGE_MIN 0x8000
#define MAGIC_ENUM_RANGE_MAX 0x9FFF
#include <magic_enum.hpp>

#include <coffee/graphics/apis/gleam/rhi_texture.h>

#include <glw/texture_formats.h>

namespace gleam {

gl::tex::texture_format_t const& texture_t::format_description() const
{
    return gl::tex::format_of(m_format);
}

#if GLEAM_MAX_VERSION >= 0x420
inline bool is_sw(group::internal_format fmt)
{
    i32 preferred{0};
    cmd::get_internalformativ(
        group::texture_target::texture_2d,
        fmt,
        group::internal_format_prop::internalformat_preferred,
        SpanOne(preferred));
    return preferred != static_cast<i32>(fmt);
}
#endif

std::vector<std::string> enumerate_compressed_formats(
    features::textures& features)
{
    std::vector<std::string> format_strings;
#if GLEAM_MAX_VERSION >= 0x420
    if(features.internal_format_query)
    {
        using fmt_t = group::internal_format;

        std::array<fmt_t, 20> internal_formats = {{
            fmt_t::compressed_rgba_astc_4x4_khr,
            fmt_t::compressed_rgba_astc_8x8_khr,

            fmt_t::compressed_rgb_s3tc_dxt1_ext,
            fmt_t::compressed_rgba_s3tc_dxt1_ext,
            fmt_t::compressed_rgba_s3tc_dxt3_ext,
            fmt_t::compressed_rgba_s3tc_dxt5_ext,

            fmt_t::compressed_red_rgtc1,
            fmt_t::compressed_rg_rgtc2,

            fmt_t::compressed_rgb_bptc_signed_float,
            fmt_t::compressed_rgb_bptc_unsigned_float,
            fmt_t::compressed_rgba_bptc_unorm,
            fmt_t::compressed_srgb_alpha_bptc_unorm,

            fmt_t::compressed_r11_eac,
            fmt_t::compressed_rg11_eac,
            fmt_t::compressed_rgb8_etc2,
            fmt_t::compressed_rgba8_etc2_eac,
            fmt_t::compressed_rgb8_punchthrough_alpha1_etc2,
        }};
        for(auto const& fmt : internal_formats)
        {
            i32 supported{};
            cmd::get_internalformativ(
                group::texture_target::texture_2d,
                fmt,
                group::internal_format_prop::internalformat_supported,
                SpanOne(supported));
            if(!supported)
                continue;
            std::string_view name = magic_enum::enum_name(fmt);
            format_strings.push_back(std::string(name.begin(), name.end()));
        }

        features::textures::texture_support_t& sw = features.software_decoded;
        sw.gl.rgtc = sw.arb.rgtc = sw.ext.rgtc
            = is_sw(fmt_t::compressed_red_rgtc1);
        sw.gl.etc2 = is_sw(fmt_t::compressed_rgba8_etc2_eac);
        sw.gl.bptc = sw.arb.bptc = sw.ext.bptc
            = is_sw(fmt_t::compressed_rgba_bptc_unorm);
        sw.gl.astc = sw.khr.astc = is_sw(fmt_t::compressed_rgba_astc_4x4_khr);

        sw.ext.s3tc = is_sw(fmt_t::compressed_rgb_s3tc_dxt1_ext);

#if defined(GLEAM_ENABLE_SOFTWARE_BCN)
        if(!features.tex.ext.s3tc)
            sw.ext.s3tc = true;
#endif

#if GLEAM_MAX_VERSION_ES >= 0x200
        sw.oes.etc1 = is_sw(fmt_t::etc1_rgb8_oes);

        sw.img.pvrtc  = is_sw(fmt_t::compressed_rgba_pvrtc_2bppv1_img);
        sw.img.pvrtc2 = is_sw(fmt_t::compressed_rgba_pvrtc_2bppv2_img);
#endif
    } else
#endif
    {
        i32 num_formats{};
        cmd::get_integerv(
            group::get_prop::num_compressed_texture_formats,
            SpanOne(num_formats));
        std::vector<i32> formats;
        formats.resize(num_formats);
        format_strings.reserve(num_formats);

        cmd::get_integerv(
            group::get_prop::compressed_texture_formats,
            semantic::SpanOver<i32>(formats.begin(), formats.end()));
        for(auto const& fmt : formats)
        {
            auto             tex_fmt = static_cast<group::internal_format>(fmt);
            std::string_view name    = magic_enum::enum_name(tex_fmt);
            if(name.empty())
                continue;
            format_strings.push_back(std::string(name.begin(), name.end()));
        }

        features::textures::texture_support_t& sw = features.software_decoded;

#if defined(GLEAM_ENABLE_SOFTWARE_BCN)
        if(!features.tex.ext.s3tc)
            sw.ext.s3tc = true;
#endif
    }
    return format_strings;
}

} // namespace gleam
