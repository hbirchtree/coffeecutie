#include "peripherals/stl/magic_enum.hpp"
#include <coffee/graphics/apis/gleam/rhi.h>

#include <glw/enums/FrontFaceDirection.h>
#include <glw/enums/TriangleFace.h>

#include <coffee/graphics/apis/gleam/rhi_texture.h>
#include <coffee/interfaces/cgraphics_pixops.h>

#include <peripherals/stl/regex.h>
#include <peripherals/stl/string_casting.h>
#include <peripherals/stl/string_ops.h>

#include <coffee/strings/libc_types.h>

#include <coffee/core/debug/formatting.h>

#include <glw/extensions/ARB_shader_draw_parameters.h>
#include <glw/extensions/EXT_discard_framebuffer.h>
#include <glw/extensions/KHR_debug.h>
#include <glw/extensions/NV_shading_rate_image.h>
#include <glw/extensions/OES_rgb8_rgba8.h>
#include <glw/extensions/OES_vertex_array_object.h>

#include <glw/texture_formats.h>

#if defined(FEATURE_ENABLE_CEAGL)
#include <CEAGL/eagl.h>
#elif defined(FEATURE_ENABLE_EGLComponent)
// #include <EGL/egl.h>
#endif

namespace gleam {

const api::load_options_t api::default_options = api::load_options_t{
    .api_version = std::nullopt,
    .api_type    = std::nullopt,
};

namespace detail {

template<class Version>
requires gl::MaximumVersion<Version, gl::Version<4, 4>>
inline void texture_alloc(textures::type type, u32& hnd)
{
    cmd::gen_textures(SpanOne(hnd));
    cmd::bind_texture(convert::to(type), hnd);
    cmd::bind_texture(convert::to(type), 0);
}

#if GLEAM_MAX_VERSION >= 0x450
template<class Version>
requires gl::MinimumVersion<Version, gl::Version<4, 5>>
inline void texture_alloc(textures::type type, u32& hnd)
{
    cmd::create_textures(convert::to(type), SpanOne(hnd));
}
#endif

} // namespace detail

void texture_t::alloc(size_type const& size, bool create_storage)
{
    using enum_helpers::feval;

    if(feval(m_type & textures::type::multisample))
        Throw(unimplemented_path("multisampled textures"));

    [[maybe_unused]] auto _ = m_debug.scope();

    m_tex_size = {size[0], size[1]};
    m_layers   = size[2];
    if(m_handle == 0u)
    {
        if(m_features.dsa)
            detail::texture_alloc<cmd::version>(m_type, m_handle);
        else
            detail::texture_alloc<lowest_version>(m_type, m_handle);
    }

    if constexpr(compile_info::debug_mode)
    {
        using P = typing::pixels::PixFmt;
        using C = typing::pixels::CompFlags;

        bool sw_decoded = requires_software_decode();
        if(!sw_decoded)
        {
            auto const& sw = m_features.software_decoded;
            if(m_format.pixfmt == P::BCn
               && stl_types::any_of(m_format.cmpflg, C::BC1, C::BC2, C::BC3))
                sw_decoded = sw.ext.s3tc;
            if(m_format.pixfmt == P::ASTC)
                sw_decoded = sw.gl.astc || sw.khr.astc;
            if(m_format.pixfmt == P::ETC2)
                sw_decoded = sw.gl.etc2;
        }
        if(sw_decoded)
            Coffee::cWarning(
                "Format {0} is software decoded",
                magic_enum::enum_name(m_format.pixfmt));
    }

    if(create_storage)
    {
        [[maybe_unused]] auto glsize = size.convert<i32>();
        auto is_compressed           = format_description().is_compressed()
                             && !requires_software_decode();
        [[maybe_unused]] auto is_immutable
            = feval(m_flags, textures::property::immutable);

        auto alloc_format = software_decode_format().value_or(m_format);
        [[maybe_unused]] auto [sized_fmt, _, __]
            = convert::to<group::sized_internal_format>(
                alloc_format, m_features);

        using textures::type;

#if GLEAM_MAX_VERSION >= 0x450
        if(is_immutable && m_features.dsa && !is_compressed)
        {
            switch(m_type)
            {
            case type::d2:
                cmd::texture_storage_2d(m_handle, m_mipmaps, sized_fmt, glsize);
                break;
            case type::d2_array:
            case type::d3:
                cmd::texture_storage_3d(m_handle, m_mipmaps, sized_fmt, glsize);
                break;
            default:
                Throw(unimplemented_path("texture type (dsa storage)"));
            }
        } else
#endif
#if GLEAM_MAX_VERSION >= 0x420 || GLEAM_MAX_VERSION_ES >= 0x300
            if(is_immutable && m_features.storage && !is_compressed)
        {
            auto target = convert::to(m_type);
            cmd::bind_texture(target, m_handle);
            switch(m_type)
            {
            case type::d2:
                cmd::tex_storage_2d(target, m_mipmaps, sized_fmt, glsize);
                break;
            case type::d2_array:
            case type::d3:
                cmd::tex_storage_3d(target, m_mipmaps, sized_fmt, glsize);
                break;
            default:
                Throw(unimplemented_path("texture type (tex storage)"));
            }
            cmd::bind_texture(target, 0);
        } else
#endif
            if(is_compressed)
        {
            auto [ifmt, type, layout]
                = convert::to<group::internal_format>(m_format, m_features);
            auto signed_size = size.convert<i32>();
            if(m_type == type::cube_array)
                signed_size.d *= 6;
            cmd::bind_texture(convert::to(m_type), m_handle);
            for(auto i : stl_types::Range<>(m_mipmaps))
            {
                auto alloc_size
                    = gl::tex::format_of(m_format).data_size(
                          size_2d<i32>{signed_size.w, signed_size.h})
                      * signed_size.d;
#if defined(COFFEE_EMSCRIPTEN)
                std::vector<libc_types::u8> black(alloc_size, 0);
#endif
                switch(m_type)
                {
                case type::d2:
                    cmd::compressed_tex_image_2d(
                        convert::to(m_type),
                        i,
                        ifmt,
                        size_2d<i32>{signed_size.w, signed_size.h},
                        0,
#if defined(COFFEE_EMSCRIPTEN)
                        semantic::SpanOver<libc_types::u8>(
                            black.begin(), black.end())
#else
                        null_span<> { alloc_size }
#endif
                    );
                    break;
#if GLEAM_MAX_VERSION >= 0x120 || GLEAM_MAX_VERSION_ES >= 0x300
                case type::d3:
                case type::d2_array:
                    cmd::compressed_tex_image_3d(
                        convert::to(m_type),
                        i,
                        ifmt,
                        signed_size,
                        0,
#if defined(COFFEE_EMSCRIPTEN)
                        semantic::SpanOver<libc_types::u8>(
                            black.begin(), black.end())
#else
                        null_span<> { alloc_size }
#endif
                    );
                    break;
#endif
#if GLEAM_MAX_VERSION >= 0x120 || GLEAM_MAX_VERSION_ES >= 0x300
                case type::cube_array:
                    cmd::compressed_tex_image_3d(
                        convert::to(m_type),
                        i,
                        ifmt,
                        signed_size,
                        0,
                        null_span<>{alloc_size});
                    break;
#endif
                default:
                    break;
                }
                signed_size.w >>= 1;
                signed_size.h >>= 1;
                if(m_type == type::d3)
                    signed_size.d >>= 1;
            }
#if GLEAM_MAX_VERSION >= 0x120 || GLEAM_MAX_VERSION_ES >= 0x300
            if(m_features.max_level)
                cmd::tex_parameter(
                    convert::to(m_type),
                    group::texture_parameter_name::texture_max_level,
                    static_cast<i32>(m_mipmaps - 1u));
#endif
            if(m_type == textures::type::cube_array)
            {
                cmd::tex_parameter(
                    convert::to(m_type),
                    group::texture_parameter_name::texture_wrap_r,
                    static_cast<i32>(group::texture_wrap_mode::clamp_to_edge));
                cmd::tex_parameter(
                    convert::to(m_type),
                    group::texture_parameter_name::texture_wrap_s,
                    static_cast<i32>(group::texture_wrap_mode::clamp_to_edge));
                cmd::tex_parameter(
                    convert::to(m_type),
                    group::texture_parameter_name::texture_wrap_t,
                    static_cast<i32>(group::texture_wrap_mode::clamp_to_edge));
            }
            cmd::bind_texture(convert::to(m_type), 0);
        } else
        {
            auto                  target = convert::to(m_type);
            Span<const std::byte> null_span;
            std::function<void(size_3d<i32> const&, i32)> create_level;

            switch(m_type)
            {
            case type::d2:
                create_level = [&](size_3d<i32> const& s, i32 i) {
                    auto [old_fmt, ptype, pfmt]
                        = convert::to<group::internal_format>(
                            alloc_format, m_features);
                    cmd::tex_image_2d(
                        target,
                        i,
                        static_cast<u32>(old_fmt),
                        s,
                        0,
                        pfmt,
                        ptype,
                        null_span);
                };
                break;
#if GLEAM_MAX_VERSION >= 0x150 || GLEAM_MAX_VERSION_ES >= 0x300 \
    || defined(GL_OES_texture_3D)
            case type::d2_array:
            case type::d3:
                create_level = [&](size_3d<i32> const& s, i32 i) {
                    auto [old_fmt, ptype, pfmt]
                        = convert::to<group::internal_format>(
                            alloc_format, m_features);
#if GLEAM_MAX_VERSION >= 0x150 || GLEAM_MAX_VERSION_ES >= 0x300
                    using internal_format_t = i32;
#else
                    using internal_format_t = group::internal_format;
#endif
                    using size_type  = std::decay_t<decltype(s)>;
                    using span_type  = std::decay_t<decltype(null_span)>;
                    using image_3d_t = std::function<
#if GLEAM_MAX_VERSION >= 0x150 || GLEAM_MAX_VERSION_ES >= 0x300
                        decltype(cmd::tex_image_3d<size_type, span_type>)
#elif defined(GL_OES_texture_3D) && GLEAM_MAX_VERSION_ES == 0x200
                        decltype(gl::oes::texture_3d::
                                     tex_image_3d<size_type, span_type>)
#endif
                        >;
                    image_3d_t tex_image_3d_impl =
#if GLEAM_MAX_VERSION >= 0x150 || GLEAM_MAX_VERSION_ES >= 0x300
                        cmd::tex_image_3d<size_type, span_type>;
#else
                        nullptr;
#endif
#if GLEAM_MAX_VERSION_ES == 0x200 && defined(GL_OES_texture_3D)
                    if(m_features.oes.texture_3d)
                        tex_image_3d_impl = gl::oes::texture_3d::
                            tex_image_3d<size_type, span_type>;
#endif
                    tex_image_3d_impl(
                        target,
                        i,
                        static_cast<internal_format_t>(old_fmt),
                        s,
                        0,
                        pfmt,
                        ptype,
                        null_span);
                };
                break;
#endif
#if GLEAM_MAX_VERSION >= 0x400 || GLEAM_MAX_VERSION_ES >= 0x320
            case type::cube_array:
                create_level = [&](size_3d<i32> const& s, i32 i) {
                    auto [old_fmt, ptype, pfmt]
                        = convert::to<group::internal_format>(
                            alloc_format, m_features);
                    cmd::tex_image_3d(
                        target,
                        i,
                        static_cast<i32>(old_fmt),
                        size_3d<i32>{s.w, s.h, s.d * 6},
                        0,
                        pfmt,
                        ptype,
                        null_span);
                };
                break;
#endif
            default:
                Throw(unimplemented_path("texture type (tex image)"));
            }

            bool mipmapped_depth = m_type == textures::type::d3;
            cmd::bind_texture(target, m_handle);
            for(auto i : stl_types::Range<i32>(m_mipmaps))
            {
                u32 mipfactor = std::pow<u32>(2, i);
                create_level(
                    size_3d<u32>{
                        size.w / mipfactor,
                        size.h / mipfactor,
                        size.d / (mipmapped_depth ? mipfactor : 1)}
                        .convert<i32>(),
                    i);
            }
#if GLEAM_MAX_VERSION >= 0x150 || GLEAM_MAX_VERSION_ES >= 0x300
            if(m_features.max_level)
                cmd::tex_parameter(
                    convert::to(m_type),
                    group::texture_parameter_name::texture_max_level,
                    static_cast<i32>(m_mipmaps - 1u));
#endif
            cmd::bind_texture(convert::to(m_type), 0);
        }
    }
}

size_3d<u32> texture_t::size()
{
#if GLEAM_MAX_VERSION >= 0x200 || GLEAM_MAX_VERSION_ES >= 0x310
    using get    = group::get_texture_parameter;
    using target = group::texture_target;
#endif

    size_3d<u32> out;
    Span<i32>    p = mem_chunk<i32>::ofBytes(out).view;
#if GLEAM_MAX_VERSION >= 0x450
    if(m_features.dsa)
    {
        cmd::get_texture_level_parameter(m_handle, 0, get::texture_width, p);
        cmd::get_texture_level_parameter(
            m_handle, 0, get::texture_height, p.subspan(1, 1));
        cmd::get_texture_level_parameter(
            m_handle, 0, static_cast<get>(GL_TEXTURE_DEPTH), p.subspan(2, 1));
    } else
#endif
#if GLEAM_MAX_VERSION >= 0x200 || GLEAM_MAX_VERSION_ES >= 0x310
        if(!m_features.tex_layer_query)
    {
        cmd::get_tex_level_parameter(
            target::texture_2d, 0, get::texture_width, p);
        cmd::get_tex_level_parameter(
            target::texture_2d, 0, get::texture_height, p.subspan(1, 1));
        cmd::get_tex_level_parameter(
            target::texture_2d, 0, get::texture_depth, p.subspan(2, 1));
    }
#else
    {
        p[0] = m_tex_size[0];
        p[1] = m_tex_size[1];
        p[2] = m_layers;
    }
#endif

    return out;
}

api::api() noexcept = default;
api::api(api&&)     = default;

tuple<features, api_type_t, u32> api::query_native_api_features(
    api::extensions_set const& extensions, load_options_t options)
{
    features   out{};
    api_type_t api_type{api_type_t::none};
    u32        api_version{0x0};

    using namespace gl;

    api_type    = options.api_type.value_or(compiled_api);
    api_version = options.api_version.value_or(
        version_tuple_to_u32(query_native_version()));

    /* Verify we're not trying to use features above what's compiled in */
    {
        auto compiled_ver = version_tuple_to_u32(query_compiled_version());
        if(compiled_api == api_type && api_version > compiled_ver)
            api_version = compiled_ver;
    }

    if(api_type == api_type_t::core)
    {
        out.buffer.mapping      = true;
        out.buffer.pbo          = true;
        out.buffer.ubo          = true;
        out.draw.instancing     = true;
        out.draw.vertex_offset  = true;
        out.texture.max_level   = true;
        out.texture.tex.gl.rgtc = true;

        out.texture.cube_array = api_version >= 0x400;

        out.draw.indirect                 = api_version >= 0x420;
        out.draw.base_instance            = api_version >= 0x420;
        out.texture.image_texture         = api_version >= 0x420;
        out.texture.internal_format_query = api_version >= 0x420;
        out.texture.storage               = api_version >= 0x420;
        out.texture.tex.gl.bptc           = api_version >= 0x420;
        out.vertex.layout_binding         = api_version >= 0x420;

        out.buffer.ssbo                        = api_version >= 0x430;
        out.debug.debug                        = api_version >= 0x430;
        out.draw.multi_indirect                = api_version >= 0x430;
        out.program.compute                    = api_version >= 0x430;
        out.program.uniform_location           = api_version >= 0x430;
        out.rendertarget.framebuffer_parameter = api_version >= 0x430;
        out.texture.image_copy                 = api_version >= 0x430;
        out.texture.sampler_binding            = api_version >= 0x430;
        out.texture.tex.gl.etc2                = api_version >= 0x430;
        out.texture.views                      = api_version >= 0x430;
        out.vertex.format                      = api_version >= 0x430;

        out.buffer.persistence = api_version >= 0x440;
        out.buffer.storage     = api_version >= 0x440;
        out.texture.multibind  = api_version >= 0x440;

        auto dsa_mode        = api_version >= 0x450;
        out.buffer.dsa       = dsa_mode;
        out.program.dsa      = dsa_mode;
        out.query.dsa        = dsa_mode;
        out.rendertarget.dsa = dsa_mode;
        out.texture.dsa      = dsa_mode;
        out.vertex.dsa       = dsa_mode;

        out.draw.shader_base_instance = api_version >= 0x460;
        out.program.spirv             = api_version >= 0x460;

        out.draw.arb.shader_draw_parameters
            = supports_extension(extensions, arb::shader_draw_parameters::name);
    } else if(api_type == api_type_t::es)
    {
        out.buffer.mapping                = api_version >= 0x300;
        out.buffer.pbo                    = api_version >= 0x300;
        out.buffer.ubo                    = api_version >= 0x300;
        out.draw.instancing               = api_version >= 0x300;
        out.rendertarget.clearbuffer      = api_version >= 0x300;
        out.rendertarget.readdraw_buffers = api_version >= 0x300;
        out.texture.internal_format_query = api_version >= 0x300;
        out.texture.max_level             = api_version >= 0x300;
        out.texture.samplers              = api_version >= 0x300;
        out.texture.texture_3d            = api_version >= 0x300;
        out.texture.tex_layer_query       = api_version >= 0x300;
        out.texture.tex.gl.etc2           = api_version >= 0x300;
        out.vertex.attribute_binding      = api_version >= 0x300;
        out.vertex.vertex_arrays          = api_version >= 0x300;

        out.buffer.ssbo                        = api_version >= 0x310;
        out.draw.indirect                      = api_version >= 0x310;
        out.program.compute                    = api_version >= 0x310;
        out.program.uniform_location           = api_version >= 0x310;
        out.rendertarget.framebuffer_parameter = api_version >= 0x310;
        out.texture.image_texture              = api_version >= 0x310;
        out.vertex.format                      = api_version >= 0x310;
        out.vertex.layout_binding              = api_version >= 0x310;

        out.debug.debug                      = api_version >= 0x320;
        out.draw.vertex_offset               = api_version >= 0x320;
        out.rendertarget.framebuffer_texture = api_version >= 0x320;
        out.texture.image_copy               = api_version >= 0x320;
        out.texture.sampler_binding          = api_version >= 0x320;
        out.texture.tex.gl.astc              = api_version >= 0x320;
        out.texture.cube_array               = api_version >= 0x320;

        out.vertex.vertex_offset = out.draw.vertex_offset;

#if defined(GL_OES_mapbuffer)
        out.buffer.oes.mapbuffer
            = supports_extension(extensions, oes::mapbuffer::name);
#endif
        out.query.disjoint_timer_query
            = supports_extension(extensions, ext::disjoint_timer_query::name);
        out.texture.oes.texture_3d
            = supports_extension(extensions, oes::texture_3d::name);
        out.vertex.oes.vertex_arrays
            = supports_extension(extensions, oes::vertex_array_object::name);
        out.rendertarget.ext.discard_framebuffer
            = supports_extension(extensions, ext::discard_framebuffer::name);
    }
    out.debug.khr.debug = supports_extension(extensions, khr::debug::name);

    out.rendertarget.nv.shading_rate_image
        = supports_extension(extensions, nv::shading_rate_image::name);

    out.texture.arb.texture_view
        = supports_extension(extensions, arb::texture_view::name);
    out.texture.ext.texture_view
        = supports_extension(extensions, ext::texture_view::name);
    out.texture.oes.texture_view
        = supports_extension(extensions, oes::texture_view::name);

    out.texture.ext.texture_anisotropic
        = supports_extension(extensions, ext::texture_filter_anisotropic::name);

    out.texture.tex.arb.bptc
        = supports_extension(extensions, arb::texture_compression_bptc::name);
    out.texture.tex.arb.rgtc
        = supports_extension(extensions, arb::texture_compression_rgtc::name);
    out.texture.tex.ext.bptc
        = supports_extension(extensions, ext::texture_compression_bptc::name);
    out.texture.tex.ext.rgtc
        = supports_extension(extensions, ext::texture_compression_rgtc::name);
    out.texture.tex.ext.s3tc
        = supports_extension(extensions, ext::texture_compression_s3tc::name)
          || supports_extension(extensions, "WEBGL_compressed_texture_s3tc");

    out.texture.tex.oes.etc1
        = supports_extension(
              extensions, oes::compressed_etc1_rgb8_texture::name)
          || supports_extension(extensions, "WEBGL_compressed_texture_etc");
    out.texture.tex.oes.rgba8
        = supports_extension(extensions, oes::rgb8_rgba8::name);

    out.texture.tex.khr.astc = supports_extension(
        extensions, khr::texture_compression_astc_ldr::name);
    out.texture.tex.khr.astc_hdr = supports_extension(
        extensions, khr::texture_compression_astc_hdr::name);

    out.texture.tex.img.pvrtc
        = supports_extension(extensions, img::texture_compression_pvrtc::name);
    out.texture.tex.img.pvrtc2
        = supports_extension(extensions, img::texture_compression_pvrtc2::name);

    out.texture.bindless_handles
        = supports_extension(extensions, arb::bindless_texture::name);

    return {out, api_type, api_version};
}

api_type_t api::query_native_api()
{
    auto version = cmd::get_string(group::string_name::version);
    if(version.empty())
        return api_type_t::none;
    return version.find("OpenGL ES") != std::string::npos ? api_type_t::es
                                                          : api_type_t::core;
}

static auto version_regex()
{
    static bool                      initialized{false};
    static stl_types::regex::Pattern pattern;

    if(!initialized)
    {
        stl_types::regex::regex_error_code ec;
        pattern = stl_types::regex::compile_pattern(
            "^(OpenGL ES )?([0-9]).([0-9])(.[0-9])? ?(.*)$", ec);
        initialized = true;
    }
    return pattern;
}

tuple<u32, u32> api::query_native_version()
{
    using stl_types::cast_string;
    auto                version = cmd::get_string(group::string_name::version);
    std::vector<string> elements;
    if(!stl_types::regex::match(version_regex(), version, elements))
        return {0, 0};

    return {cast_string<u32>(elements[2]), cast_string<u32>(elements[3])};
}

tuple<u32, u32> api::query_compiled_version()
{
    constexpr u32 version_basis =
#if GLEAM_MAX_VERSION_ES > 0x0
        GLEAM_MAX_VERSION_ES;
#else
        GLEAM_MAX_VERSION;
#endif
    u32 major = (version_basis & 0xF00) >> 8;
    u32 minor = (version_basis & 0x0F0) >> 4;
    return {major, minor};
}

std::set<string> api::query_native_extensions()
{
    std::set<string> out;

#if GLEAM_MAX_VERSION >= 0x300
    if(compiled_api == api_type_t::core)
    {
        i32 num_extensions{0};
        cmd::get_integerv(
            group::get_prop::num_extensions, SpanOne(num_extensions));
        for(auto i : stl_types::Range<>(num_extensions))
            out.insert(cmd::get_stringi(group::string_name::extensions, i));
    } else
#endif
        if(platform_api == api_type_t::webgl)
    {
        string extensions = cmd::get_string(group::string_name::extensions);
        size_t prev_idx   = 0;
        size_t idx        = extensions.find(' ');
        while(idx != string::npos)
        {
            out.insert(extensions.substr(prev_idx, idx - prev_idx));
            prev_idx = idx + 1;
            idx      = extensions.find(' ', prev_idx);
        }
    } else
    {
        string extensions = cmd::get_string(group::string_name::extensions);
        for(auto ext : stl_types::str::split::str<char>(extensions, ' '))
        {
            out.insert(string(ext.begin(), ext.end()));
        }
    }

#if defined(EGL_VERSION_1_0)
    {
        auto edisplay = eglGetCurrentDisplay();
        /* Get display extensions */
        auto extensions = eglQuerystring(edisplay, EGL_EXTENSIONS);
        for(auto ext : stl_types::str::split::str<char>(extensions, ' '))
            out.insert(string(ext.begin(), ext.end()));
#if defined(EGL_VERSION_1_5)
        /* Get client API extensions */
        extensions = eglQuerystring(EGL_NO_DISPLAY, EGL_EXTENSIONS);
        for(auto ext : stl_types::str::split::str<char>(extensions, ' '))
            out.insert(string(ext.begin(), ext.end()));
#endif
    }
#endif

    return out;
}

std::string_view api::api_name()
{
    using namespace std::string_view_literals;
    if(m_api_type == api_type_t::core)
        return "OpenGL Core"sv;
    else
        return "OpenGL ES"sv;
}

api_type_t api::api_type()
{
    return m_api_type;
}

tuple<u32, u32> api::api_version()
{
    return {(m_api_version & 0xF00) >> 8, (m_api_version & 0x0F0) >> 4};
}

static auto shaderlang_regex()
{
    static bool                      initialized{false};
    static stl_types::regex::Pattern pattern;

    if(!initialized)
    {
        stl_types::regex::regex_error_code ec;
        pattern = stl_types::regex::compile_pattern(
            "^(OpenGL ES GLSL ES )?([0-9]).([0-9]{2}) ?(.*)$", ec);
        initialized = true;
    }
    return pattern;
}

string api::shaderlang_name()
{
    auto version
        = cmd::get_string(group::string_name::shading_language_version);
    std::vector<std::string> elements;
    if(!stl_types::regex::match(shaderlang_regex(), version, elements))
        return "Unknown GLSL";

    return elements[1].empty() ? "OpenGL GLSL" : elements[1];
}

api_type_t api::shaderlang_type()
{
    return shaderlang_name() != "OpenGL GLSL" ? api_type_t::es
                                              : api_type_t::core;
}

tuple<u32, u32> api::shaderlang_version()
{
    using stl_types::cast_string;
    auto version
        = cmd::get_string(group::string_name::shading_language_version);
    std::vector<std::string> elements;
    if(!stl_types::regex::match(shaderlang_regex(), version, elements))
        return {0, 0};

    return {cast_string<u32>(elements[2]), cast_string<u32>(elements[3])};
}

tuple<string, string> api::device()
{
    auto vendor = cmd::get_string(group::string_name::vendor);
    auto device = cmd::get_string(group::string_name::renderer);
    return {vendor, device};
}

optional<string> api::device_driver()
{
    auto version = cmd::get_string(group::string_name::version);
    std::vector<std::string> elements;
    if(!stl_types::regex::match(version_regex(), version, elements))
        return std::nullopt;
    return elements[5];
}

api::extensions_set api::extensions()
{
    return m_extensions;
}

std::vector<std::string> enumerate_compressed_formats(
    features::textures& features);

void api::collect_info(comp_app::interfaces::AppInfo& appInfo)
{
    using group::string_name;

    auto api = api_name();
    appInfo.add("gl:api", std::string(api.begin(), api.end()));
    {
        auto [vendor, device_] = device();
        appInfo.add("gl:vendor", vendor);
        appInfo.add("gl:device", device_);
    }
    {
        auto [sl_major, sl_minor] = shaderlang_version();
        appInfo.add("gl:glslMajor", std::to_string(sl_major));
        appInfo.add("gl:glslMinor", std::to_string(sl_minor));
        appInfo.add(
            "gl:shaderLanguage",
            cmd::get_string(string_name::shading_language_version));
    }
    {
        auto [major, minor] = api_version();
        appInfo.add("gl:major", std::to_string(major));
        appInfo.add("gl:minor", std::to_string(minor));
    }

    if(auto driver = device_driver())
        appInfo.add("gl:driver", driver.value());

    auto        exts = query_native_extensions();
    std::string exts_list;
    for(auto const& ext : exts)
    {
        if(exts_list.size() > 0)
            exts_list.push_back(' ');
        exts_list.insert(exts_list.end(), ext.begin(), ext.end());
    }
    exts = {};
    appInfo.add("gl:extensions", exts_list);
    exts_list.clear();
    auto        fmts = enumerate_compressed_formats(m_features.texture);
    std::string formats_list;
    for(auto const& fmt : fmts)
    {
        if(!formats_list.empty())
            formats_list.push_back(' ');
        formats_list.insert(formats_list.end(), fmt.begin(), fmt.end());
    }
    fmts = {};
    appInfo.add("gl:compressedFormats", formats_list);
}

optional<error> api::load(load_options_t options)
{
    m_main_queue = rq::runtime_queue::GetCurrentQueue().value();

    if(options.api_extensions)
    {
        auto supported = query_native_extensions();
        auto requested = *options.api_extensions;
        for(auto const& ext : requested)
            if(supported.contains(ext))
                m_extensions.insert(ext);
    } else
        m_extensions = query_native_extensions();

    m_features.debug.khr.debug = supports_extension(gl::khr::debug::name);
    [[maybe_unused]] auto _    = debug().scope(__PRETTY_FUNCTION__);

    auto [native_features, native_api, native_version]
        = query_native_api_features(m_extensions, options);

    if(options.api_version)
        m_api_version = *options.api_version;
    else
        m_api_version = native_version;

    if(options.api_type)
        m_api_type = *options.api_type;
    else
        m_api_type = native_api;

    workarounds default_workarounds = {
    // clang-format off
        .draw = {
            .emulated_instance_id   = true,
            .emulated_base_instance = true,
            .emulated_vertex_offset = true,
            .force_vertex_attrib_names = m_api_type == api_type_t::es,
            .advance_ubos_by_baseinstance = true,
        },
        .buffer = {
            .emulated_mapbuffer = true,
        },
    // clang-format on
    };

    /* TODO: Implement shader preprocessing to make InstanceID and BaseInstance
     * work
     */
    m_workarounds = options.api_workarounds.value_or(default_workarounds);

    if(options.api_features)
        m_features = *options.api_features;
    else
        m_features = native_features;

    m_limits.set_limits(m_features);

    if(m_features.draw.instancing)
        m_workarounds.draw.emulated_instance_id = false;
    //    if(m_features.draw.shader_base_instance)
    //        m_workarounds.draw.emulated_base_instance = false;
    if(m_features.draw.vertex_offset)
        m_workarounds.draw.emulated_vertex_offset = false;
    if(m_features.draw.shader_base_instance)
        m_workarounds.draw.emulated_base_instance = false;
    //    if(m_features.buffer.ssbo)
    //        m_workarounds.draw.advance_ubos_by_baseinstance = false;
    if(m_features.buffer.mapping || m_features.buffer.oes.mapbuffer)
        m_workarounds.buffer.emulated_mapbuffer = false;

    /* Let's skip ES 2.0 implementations that don't fulfill a minimum */
    if(m_api_type == api_type_t::es)
    {
        if(m_api_version == 0x200)
        {
            //            if(!m_features.buffer.oes.mapbuffer
            //               || !m_features.vertex.oes.vertex_arrays)
            //                return error::refuse_bad_es20_impl;
            i32 shader_compiler_present{};
            cmd::get_integerv(
                group::get_prop::shader_compiler,
                SpanOne(shader_compiler_present));
            if(!shader_compiler_present)
                return error::refuse_bad_es20_impl;
        }

#if GLEAM_MAX_VERSION_ES != 0x200
        if(compiled_api == api_type_t::core
           || (compiled_api == api_type_t::es && m_api_version >= 0x300))
        {
            /* Some modern implementations might not like use of VAO 0 */
            u32 hnd{};
            cmd::gen_vertex_arrays(SpanOne(hnd));
            cmd::bind_vertex_array(hnd);
        }
#endif
    }

    if(m_api_type == api_type_t::core)
    {
        if(m_api_version < 0x330)
            return error::refuse_version_too_low;
    }
    if(compiled_api == api_type_t::core && m_api_type == api_type_t::es)
    {
        auto compiled_ver = version_tuple_to_u32(query_compiled_version());
        auto native_ver   = version_tuple_to_u32(query_native_version());
        //        auto [native_features, _, __]
        //            = query_native_api_features(query_native_extensions());

        //        Coffee::Logging::cDebug(
        //            "\nNative extensions:\n{0}\nEmulated extensions:\n{1}",
        //            query_native_extensions(),
        //            m_extensions);

        /* Emulating OpenGL ES on desktop requires specific versions */
        if(compiled_ver < 0x410 && m_api_version >= 0x200)
            return error::no_emulation_compiled_version_low;
        if(compiled_ver < 0x430 && m_api_version >= 0x300)
            return error::no_emulation_compiled_version_low;
        if(compiled_ver < 0x450 && m_api_version >= 0x310)
            return error::no_emulation_compiled_version_low;

        if(native_ver < 0x410 && m_api_version >= 0x200)
            return error::no_emulation_native_version_low;
        if(native_ver < 0x430 && m_api_version >= 0x300)
            return error::no_emulation_native_version_low;
        if(native_ver < 0x450 && m_api_version >= 0x310)
            return error::no_emulation_native_version_low;

        /* OpenGL ES 3.2 requires some extensions,
         * we leak a little bit of the abstraction here */
        while(m_api_version >= 0x320)
        {
            /* ASTC has some problems on GL core, might need software decode */
            //            if(!native_features.texture.tex.khr.astc)
            //                return error::no_emulation_native_version_low;
            //            else
            //                m_features.texture.tex.khr.astc = true;
            break;
        }
    }

    //    cmd::enable(group::enable_cap::cull_face);
    cmd::cull_face(group::triangle_face::back);
    cmd::front_face(group::front_face_direction::ccw);

#if GLEAM_MAX_VERSION >= 0x320
    cmd::enable(group::enable_cap::texture_cube_map_seamless);
#endif

    debug().message("gleam::api::load");

#if GLEAM_MAX_VERSION_ES != 0x200
    if(m_features.draw.indirect)
    {
        using semantic::RSCA;
        auto buffer
            = alloc_buffer(buffers::draw, RSCA::ReadWrite | RSCA::Streaming);
        buffer->alloc();
        buffer->commit(options.indirect_buffer_size.value_or(16 * 1024));
        m_indirect_buffer = std::make_unique<circular_buffer_t>(
            std::ref(debug()), std::move(buffer));
    }
#endif

    return std::nullopt;
}

void api::unload()
{
    debug().message("gleam::api::unload");
    // TODO: Maybe invalidate some state?
}

bool api::supports_extension(
    const std::set<string>& extensions, const string& ext)
{
    return extensions.contains(ext);
}

bool api::supports_extension(string const& ext)
{
    return m_extensions.contains(ext);
}

} // namespace gleam
