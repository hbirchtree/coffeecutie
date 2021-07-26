#include <coffee/graphics/apis/gleam/rhi.h>

#include <coffee/graphics/apis/gleam/rhi_texture.h>
#include <peripherals/stl/regex.h>
#include <peripherals/stl/string_casting.h>
#include <peripherals/stl/string_ops.h>

namespace gleam {
namespace detail {

template<class Version>
#if defined(GL_VERSION_4_5)
requires gl::MaximumVersion<Version, gl::Version<4, 4>>
#endif
inline void texture_alloc(textures::type, u32& hnd)
{
    cmd::gen_textures(semantic::SpanOne(hnd));
}

#if defined(GL_VERSION_4_5)
template<class Version>
requires gl::MinimumVersion<Version, gl::Version<4, 5>>
inline void texture_alloc(textures::type type, u32& hnd)
{
    cmd::create_textures(convert::to(type), semantic::SpanOne(hnd));
}
#endif

} // namespace detail

void texture_t::alloc(size_type const& size, bool create_storage)
{
    if(enum_helpers::feval(m_type & textures::type::multisample))
        Throw(unimplemented_path("multisampled textures"));

    m_tex_size = {size[0], size[1]};
    m_layers   = size[2];
    if(m_features.dsa)
        detail::texture_alloc<cmd::version>(m_type, m_handle);
    else
        detail::texture_alloc<lowest_version>(m_type, m_handle);

    if(create_storage)
    {
        auto sized_fmt =
            std::get<0>(convert::to<group::sized_internal_format>(m_format));
        auto glsize = size.convert<i32>();

        using textures::type;

#if GLEAM_MAX_VERSION >= 450 || defined(GL_EXT_direct_state_access)
        if(m_features.dsa)
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
#if GLEAM_MAX_VERSION >= 420 || GLEAM_MAX_VERSION_ES >= 0x300
            if(m_features.storage)
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
        } else
#endif
        {
            auto                  target = convert::to(m_type);
            Span<const std::byte> null_span;
            std::function<void(size_3d<i32> const&, i32)> create_level;

            switch(m_type)
            {
            case type::d2:
                create_level = [&](size_3d<i32> const& s, i32 i) {
                    auto [old_fmt, ptype, pfmt] =
                        convert::to<group::internal_format>(m_format);
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
#if GLEAM_MAX_VERSION >= 0x150 || GLEAM_MAX_VERSION_ES >= 0x300 || \
    defined(GL_OES_texture_3D)
            case type::d2_array:
            case type::d3:
                create_level = [&](size_3d<i32> const& s, i32 i) {
                    auto [old_fmt, ptype, pfmt] =
                        convert::to<group::internal_format>(m_format);
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
        }
    }
}

size_3d<u32> texture_t::size()
{
    using get    = group::get_texture_parameter;
    using target = group::texture_target;

    size_3d<u32> out;
    Span<i32>    p = mem_chunk<i32>::ofBytes(out);
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
        if(!m_features.es20)
    {
        cmd::get_tex_level_parameter(
            target::texture_2d, 0, get::texture_width, p);
        cmd::get_tex_level_parameter(
            target::texture_2d, 0, get::texture_height, p.subspan(1, 1));
        cmd::get_tex_level_parameter(
            target::texture_2d, 0, static_cast<get>(GL_TEXTURE_DEPTH), p.subspan(2, 1));
    } else
#else
    {
        p[0] = m_tex_size[0];
        p[1] = m_tex_size[1];
        p[2] = m_layers;
    }
#endif

    return out;
}

Tup<features, api_type_t, u32> api::query_native_api_features(
    api::extensions_set const& extensions, load_options_t options)
{
    features   out{};
    api_type_t api_type{api_type_t::none};
    u32        api_version{0x0};

    using namespace gl;

    api_type    = options.api_type.value_or(platform_api);
    api_version = options.api_version.value_or(
        version_tuple_to_u32(query_native_version()));

    if(api_type == api_type_t::core)
    {
        auto dsa_mode  = api_version >= 0x450;
        out.buffer.dsa = out.program.dsa = out.query.dsa =
            out.rendertarget.dsa = out.texture.dsa = out.vertex.dsa = dsa_mode;

        out.buffer.storage      = api_version >= 0x440;
//        out.vertex.format       = api_version >= 0x430;
        out.texture.storage     = api_version >= 0x420;
        out.buffer.mapping      = true;
        out.texture.tex.gl.etc2 = api_version >= 0x430;
        out.texture.tex.gl.bptc = api_version >= 0x420;
        out.texture.tex.gl.rgtc = api_version >= 0x300;
    } else if(api_type == api_type_t::es)
    {
        out.buffer.mapping =
            api_version >= 0x300 ||
            supports_extension(extensions, oes::mapbuffer::name);
//        out.vertex.format = api_version >= 0x310;
        out.texture.oes.texture_3d =
            supports_extension(extensions, oes::texture_3d::name);
        out.query.disjoint_timer_query =
            supports_extension(extensions, ext::disjoint_timer_query::name);
        out.es20                = api_version == 0x200;
        out.texture.tex.gl.etc2 = api_version >= 0x300;

        out.texture.es20 = out.vertex.es20 = out.es20;
    }
    out.texture.arb.texture_view =
        supports_extension(extensions, arb::texture_view::name);
    out.texture.ext.texture_view =
        supports_extension(extensions, ext::texture_view::name);
    out.texture.oes.texture_view =
        supports_extension(extensions, oes::texture_view::name);

    out.texture.tex.arb.bptc =
        supports_extension(extensions, arb::texture_compression_bptc::name);
    out.texture.tex.arb.rgtc =
        supports_extension(extensions, arb::texture_compression_rgtc::name);
    out.texture.tex.ext.bptc =
        supports_extension(extensions, ext::texture_compression_bptc::name);
    out.texture.tex.ext.rgtc =
        supports_extension(extensions, ext::texture_compression_rgtc::name);
    out.texture.tex.ext.s3tc =
        supports_extension(extensions, ext::texture_compression_s3tc::name);

    out.texture.tex.oes.etc1 =
        supports_extension(extensions, oes::compressed_etc1_rgb8_texture::name);

    out.texture.tex.khr.astc =
        supports_extension(extensions, khr::texture_compression_astc_ldr::name);
    out.texture.tex.khr.astc_hdr =
        api_type == api_type_t::es &&
        supports_extension(extensions, khr::texture_compression_astc_hdr::name);

    out.texture.tex.img.pvrtc =
        supports_extension(extensions, img::texture_compression_pvrtc::name);
    out.texture.tex.img.pvrtc2 =
        supports_extension(extensions, img::texture_compression_pvrtc2::name);

    return {out, api_type, api_version};
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

Tup<u32, u32> api::query_native_version()
{
    using stl_types::cast_string;
    auto version = cmd::get_string(group::string_name::version);
    stl_types::Vector<String> elements;
    if(!stl_types::regex::match(version_regex(), version, elements))
        return {0, 0};

    return {cast_string<u32>(elements[2]), cast_string<u32>(elements[3])};
}

stl_types::Set<String> api::query_native_extensions()
{
    stl_types::Set<String> out;

#if GLEAM_MAX_VERSION >= 0x300
    if(platform_api == api_type_t::core)
    {
        i32 num_extensions{0};
        cmd::get_integerv(
            group::get_prop::num_extensions, SpanOne(num_extensions));
        for(auto i : stl_types::Range<>(num_extensions))
            out.insert(cmd::get_stringi(group::string_name::extensions, i));
    } else
#endif
    {
        String extensions = cmd::get_string(group::string_name::extensions);
        for(auto ext : stl_types::str::split::str(extensions, ' '))
            out.insert(String(ext.begin(), ext.end()));
    }

#if defined(EGL_VERSION_1_0)
    {
        auto edisplay = eglGetCurrentDisplay();
        /* Get display extensions */
        auto extensions = eglQueryString(edisplay, EGL_EXTENSIONS);
        for(auto ext : stl_types::str::split::str<char>(extensions, ' '))
            out.insert(String(ext.begin(), ext.end()));
#if defined(EGL_VERSION_1_5)
        /* Get client API extensions */
        extensions = eglQueryString(EGL_NO_DISPLAY, EGL_EXTENSIONS);
        for(auto ext : stl_types::str::split::str<char>(extensions, ' '))
            out.insert(String(ext.begin(), ext.end()));
#endif
    }
#endif

    return out;
}

std::string_view api::api_name()
{
    if(m_api_type == api_type_t::core)
        return "OpenGL Core";
    else
        return "OpenGL ES";
}

api_type_t api::api_type()
{
    return m_api_type;
}

Tup<u32, u32> api::api_version()
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

stl_types::String api::shaderlang_name()
{
    auto version =
        cmd::get_string(group::string_name::shading_language_version);
    stl_types::Vector<String> elements;
    if(!stl_types::regex::match(shaderlang_regex(), version, elements))
        return "Unknown GLSL";

    return elements[1].empty() ? "OpenGL GLSL" : elements[1];
}

api_type_t api::shaderlang_type()
{
    return shaderlang_name() != "OpenGL GLSL" ? api_type_t::es
                                              : api_type_t::core;
}

Tup<u32, u32> api::shaderlang_version()
{
    using stl_types::cast_string;
    auto version =
        cmd::get_string(group::string_name::shading_language_version);
    stl_types::Vector<String> elements;
    if(!stl_types::regex::match(shaderlang_regex(), version, elements))
        return {0, 0};

    return {cast_string<u32>(elements[2]), cast_string<u32>(elements[3])};
}

Tup<stl_types::String, stl_types::String> api::device()
{
    auto vendor = cmd::get_string(group::string_name::vendor);
    auto device = cmd::get_string(group::string_name::renderer);
    return {vendor, device};
}

Optional<String> api::device_driver()
{
    auto version = cmd::get_string(group::string_name::version);
    stl_types::Vector<String> elements;
    if(!stl_types::regex::match(version_regex(), version, elements))
        return std::nullopt;
    return elements[5];
}

Optional<error> api::load(load_options_t options)
{
    if(options.api_extensions)
        m_extensions = *options.api_extensions;
    else
        m_extensions = query_native_extensions();

    m_features.khr_debug = supports_extension(gl::khr::debug::name);

    auto _ = debug().scope(__PRETTY_FUNCTION__);

    auto [native_features, native_api, native_version] =
        query_native_api_features(m_extensions, options);

    if(options.api_version)
        m_api_version = *options.api_version;
    else
        m_api_version = native_version;

    if(options.api_type)
        m_api_type = *options.api_type;
    else
        m_api_type = native_api;

    if(options.api_features)
        m_features = *options.api_features;
    else
        m_features = native_features;

    /* Let's skip ES 2.0 implementations that don't fulfill a minimum */
    if(m_api_type == api_type_t::es)
    {
        if(m_api_version == 0x200 && !m_features.buffer.oes.mapbuffer)
            return error::refuse_bad_es20_impl;
    }

    if(m_api_type == api_type_t::core)
    {
        if(m_api_version < 0x330)
            return error::refuse_version_too_low;
    }

//    cmd::enable(group::enable_cap::cull_face);
    cmd::cull_face(group::cull_face_mode::back);
    cmd::front_face(group::front_face_direction::ccw);

    return std::nullopt;
}

bool api::supports_extension(
    const stl_types::Set<stl_types::String>& extensions,
    const stl_types::String&                 ext)
{
    return extensions.contains(ext);
}

bool api::supports_extension(String const& ext)
{
    return m_extensions.contains(ext);
}

} // namespace gleam
