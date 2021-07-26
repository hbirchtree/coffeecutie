#pragma once

#ifdef GL_EXT_direct_state_access
namespace gl::ext::direct_state_access {
namespace values {
constexpr libc_types::u32 program_matrix             = 0x8E2D;
constexpr libc_types::u32 transpose_program_matrix   = 0x8E2E;
constexpr libc_types::u32 program_matrix_stack_depth = 0x8E2F;
} // namespace values
STATICINLINE void matrix_frustum(
    group::matrix_mode mode,
    f64                left,
    f64                right,
    f64                bottom,
    f64                top,
    f64                zNear,
    f64                zFar)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MatrixFrustumEXT)
    }
    glMatrixFrustumEXT(
        static_cast<GLenum>(mode), left, right, bottom, top, zNear, zFar);
    detail::error_check("MatrixFrustumEXT"sv);
}

STATICINLINE void matrix_load_identity(group::matrix_mode mode)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MatrixLoadIdentityEXT)
    }
    glMatrixLoadIdentityEXT(static_cast<GLenum>(mode));
    detail::error_check("MatrixLoadIdentityEXT"sv);
}

template<class span_const_f64>
requires(semantic::concepts::Span<span_const_f64>&& std::is_same_v<
         std::decay_t<typename span_const_f64::value_type>,
         std::decay_t<f64>>) STATICINLINE
    void matrix_loadd(group::matrix_mode mode, span_const_f64 const& m)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MatrixLoaddEXT)
    }
    glMatrixLoaddEXT(
        static_cast<GLenum>(mode),
        m.size() ? reinterpret_cast<const GLdouble*>(m.data()) : nullptr);
    detail::error_check("MatrixLoaddEXT"sv);
}

template<class span_const_f32>
requires(semantic::concepts::Span<span_const_f32>&& std::is_same_v<
         std::decay_t<typename span_const_f32::value_type>,
         std::decay_t<f32>>) STATICINLINE
    void matrix_loadf(group::matrix_mode mode, span_const_f32 const& m)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MatrixLoadfEXT)
    }
    glMatrixLoadfEXT(
        static_cast<GLenum>(mode),
        m.size() ? reinterpret_cast<const GLfloat*>(m.data()) : nullptr);
    detail::error_check("MatrixLoadfEXT"sv);
}

template<class span_const_f64>
requires(semantic::concepts::Span<span_const_f64>&& std::is_same_v<
         std::decay_t<typename span_const_f64::value_type>,
         std::decay_t<f64>>) STATICINLINE
    void matrix_multd(group::matrix_mode mode, span_const_f64 const& m)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MatrixMultdEXT)
    }
    glMatrixMultdEXT(
        static_cast<GLenum>(mode),
        m.size() ? reinterpret_cast<const GLdouble*>(m.data()) : nullptr);
    detail::error_check("MatrixMultdEXT"sv);
}

template<class span_const_f32>
requires(semantic::concepts::Span<span_const_f32>&& std::is_same_v<
         std::decay_t<typename span_const_f32::value_type>,
         std::decay_t<f32>>) STATICINLINE
    void matrix_multf(group::matrix_mode mode, span_const_f32 const& m)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MatrixMultfEXT)
    }
    glMatrixMultfEXT(
        static_cast<GLenum>(mode),
        m.size() ? reinterpret_cast<const GLfloat*>(m.data()) : nullptr);
    detail::error_check("MatrixMultfEXT"sv);
}

STATICINLINE void matrix_ortho(
    group::matrix_mode mode,
    f64                left,
    f64                right,
    f64                bottom,
    f64                top,
    f64                zNear,
    f64                zFar)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MatrixOrthoEXT)
    }
    glMatrixOrthoEXT(
        static_cast<GLenum>(mode), left, right, bottom, top, zNear, zFar);
    detail::error_check("MatrixOrthoEXT"sv);
}

STATICINLINE void matrix_pop(group::matrix_mode mode)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MatrixPopEXT)
    }
    glMatrixPopEXT(static_cast<GLenum>(mode));
    detail::error_check("MatrixPopEXT"sv);
}

STATICINLINE void matrix_push(group::matrix_mode mode)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MatrixPushEXT)
    }
    glMatrixPushEXT(static_cast<GLenum>(mode));
    detail::error_check("MatrixPushEXT"sv);
}

template<class vec_3_f64>
requires(semantic::concepts::Vector<vec_3_f64, f64, 3>) STATICINLINE
    void matrix_rotated(group::matrix_mode mode, f64 angle, vec_3_f64 const& x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MatrixRotatedEXT)
    }
    glMatrixRotatedEXT(static_cast<GLenum>(mode), angle, x.x(), x.y(), x.z());
    detail::error_check("MatrixRotatedEXT"sv);
}

template<class vec_3_f32>
requires(semantic::concepts::Vector<vec_3_f32, f32, 3>) STATICINLINE
    void matrix_rotatef(group::matrix_mode mode, f32 angle, vec_3_f32 const& x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MatrixRotatefEXT)
    }
    glMatrixRotatefEXT(static_cast<GLenum>(mode), angle, x.x(), x.y(), x.z());
    detail::error_check("MatrixRotatefEXT"sv);
}

template<class vec_3_f64>
requires(semantic::concepts::Vector<vec_3_f64, f64, 3>) STATICINLINE
    void matrix_scaled(group::matrix_mode mode, vec_3_f64 const& x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MatrixScaledEXT)
    }
    glMatrixScaledEXT(static_cast<GLenum>(mode), x.x(), x.y(), x.z());
    detail::error_check("MatrixScaledEXT"sv);
}

template<class vec_3_f32>
requires(semantic::concepts::Vector<vec_3_f32, f32, 3>) STATICINLINE
    void matrix_scalef(group::matrix_mode mode, vec_3_f32 const& x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MatrixScalefEXT)
    }
    glMatrixScalefEXT(static_cast<GLenum>(mode), x.x(), x.y(), x.z());
    detail::error_check("MatrixScalefEXT"sv);
}

template<class vec_3_f64>
requires(semantic::concepts::Vector<vec_3_f64, f64, 3>) STATICINLINE
    void matrix_translated(group::matrix_mode mode, vec_3_f64 const& x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MatrixTranslatedEXT)
    }
    glMatrixTranslatedEXT(static_cast<GLenum>(mode), x.x(), x.y(), x.z());
    detail::error_check("MatrixTranslatedEXT"sv);
}

template<class vec_3_f32>
requires(semantic::concepts::Vector<vec_3_f32, f32, 3>) STATICINLINE
    void matrix_translatef(group::matrix_mode mode, vec_3_f32 const& x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MatrixTranslatefEXT)
    }
    glMatrixTranslatefEXT(static_cast<GLenum>(mode), x.x(), x.y(), x.z());
    detail::error_check("MatrixTranslatefEXT"sv);
}

STATICINLINE void client_attrib_default(group::client_attrib_mask mask)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ClientAttribDefaultEXT)
    }
    glClientAttribDefaultEXT(static_cast<GLenum>(mask));
    detail::error_check("ClientAttribDefaultEXT"sv);
}

STATICINLINE void push_client_attrib_default(group::client_attrib_mask mask)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(PushClientAttribDefaultEXT)
    }
    glPushClientAttribDefaultEXT(static_cast<GLenum>(mask));
    detail::error_check("PushClientAttribDefaultEXT"sv);
}

template<class vec_2_i32>
requires(semantic::concepts::Vector<vec_2_i32, i32, 2>) STATICINLINE
    void copy_texture_image_1d(
        u32                    texture,
        group::texture_target  target,
        i32                    level,
        group::internal_format internalformat,
        vec_2_i32 const&       x,
        i32                    width,
        i32                    border)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CopyTextureImage1DEXT)
        glIsTexture(texture);
    }
    glCopyTextureImage1DEXT(
        texture,
        static_cast<GLenum>(target),
        level,
        static_cast<GLenum>(internalformat),
        x.x(),
        x.y(),
        width,
        border);
    detail::error_check("CopyTextureImage1DEXT"sv);
}

template<class size_2_i32, class vec_2_i32>
requires(semantic::concepts::Vector<vec_2_i32, i32, 2>&&
             semantic::concepts::Size2D<size_2_i32, i32>) STATICINLINE
    void copy_texture_image_2d(
        u32                    texture,
        group::texture_target  target,
        i32                    level,
        group::internal_format internalformat,
        vec_2_i32 const&       x,
        size_2_i32 const&      width,
        i32                    border)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CopyTextureImage2DEXT)
        glIsTexture(texture);
    }
    glCopyTextureImage2DEXT(
        texture,
        static_cast<GLenum>(target),
        level,
        static_cast<GLenum>(internalformat),
        x.x(),
        x.y(),
        width[0],
        width[1],
        border);
    detail::error_check("CopyTextureImage2DEXT"sv);
}

template<class vec_2_i32>
requires(semantic::concepts::Vector<vec_2_i32, i32, 2>) STATICINLINE
    void copy_texture_sub_image_1d(
        u32                   texture,
        group::texture_target target,
        i32                   level,
        i32                   xoffset,
        vec_2_i32 const&      x,
        i32                   width)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CopyTextureSubImage1DEXT)
        glIsTexture(texture);
    }
    glCopyTextureSubImage1DEXT(
        texture,
        static_cast<GLenum>(target),
        level,
        xoffset,
        x.x(),
        x.y(),
        width);
    detail::error_check("CopyTextureSubImage1DEXT"sv);
}

template<class size_2_i32, class vec_2_i32>
requires(semantic::concepts::Vector<vec_2_i32, i32, 2>&&
             semantic::concepts::Size2D<size_2_i32, i32>) STATICINLINE
    void copy_texture_sub_image_2d(
        u32                   texture,
        group::texture_target target,
        i32                   level,
        vec_2_i32 const&      xoffset,
        vec_2_i32 const&      x,
        size_2_i32 const&     width)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CopyTextureSubImage2DEXT)
        glIsTexture(texture);
    }
    glCopyTextureSubImage2DEXT(
        texture,
        static_cast<GLenum>(target),
        level,
        xoffset.x(),
        xoffset.y(),
        x.x(),
        x.y(),
        width[0],
        width[1]);
    detail::error_check("CopyTextureSubImage2DEXT"sv);
}

template<class span_void>
requires(semantic::concepts::Span<span_void>) STATICINLINE
    void get_texture_image(
        u32                   texture,
        group::texture_target target,
        i32                   level,
        group::pixel_format   format,
        group::pixel_type     type,
        span_void             pixels)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetTextureImageEXT)
        glIsTexture(texture);
    }
    glGetTextureImageEXT(
        texture,
        static_cast<GLenum>(target),
        level,
        static_cast<GLenum>(format),
        static_cast<GLenum>(type),
        pixels.size() ? reinterpret_cast<void*>(pixels.data()) : nullptr);
    detail::error_check("GetTextureImageEXT"sv);
}

template<class span_f32>
requires(semantic::concepts::Span<span_f32>&& std::is_same_v<
         std::decay_t<typename span_f32::value_type>,
         std::decay_t<f32>>) STATICINLINE
    void get_texture_level_parameter(
        u32                          texture,
        group::texture_target        target,
        i32                          level,
        group::get_texture_parameter pname,
        span_f32                     params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetTextureLevelParameterfvEXT)
        glIsTexture(texture);
    }
    glGetTextureLevelParameterfvEXT(
        texture,
        static_cast<GLenum>(target),
        level,
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLfloat*>(params.data()) : nullptr);
    detail::error_check("GetTextureLevelParameterfvEXT"sv);
}

template<class span_i32>
requires(semantic::concepts::Span<span_i32>&& std::is_same_v<
         std::decay_t<typename span_i32::value_type>,
         std::decay_t<i32>>) STATICINLINE
    void get_texture_level_parameter(
        u32                          texture,
        group::texture_target        target,
        i32                          level,
        group::get_texture_parameter pname,
        span_i32                     params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetTextureLevelParameterivEXT)
        glIsTexture(texture);
    }
    glGetTextureLevelParameterivEXT(
        texture,
        static_cast<GLenum>(target),
        level,
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetTextureLevelParameterivEXT"sv);
}

template<class span_f32>
requires(semantic::concepts::Span<span_f32>&& std::is_same_v<
         std::decay_t<typename span_f32::value_type>,
         std::decay_t<f32>>) STATICINLINE
    void get_texture_parameter(
        u32                          texture,
        group::texture_target        target,
        group::get_texture_parameter pname,
        span_f32                     params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetTextureParameterfvEXT)
        glIsTexture(texture);
    }
    glGetTextureParameterfvEXT(
        texture,
        static_cast<GLenum>(target),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLfloat*>(params.data()) : nullptr);
    detail::error_check("GetTextureParameterfvEXT"sv);
}

template<class span_i32>
requires(semantic::concepts::Span<span_i32>&& std::is_same_v<
         std::decay_t<typename span_i32::value_type>,
         std::decay_t<i32>>) STATICINLINE
    void get_texture_parameter(
        u32                          texture,
        group::texture_target        target,
        group::get_texture_parameter pname,
        span_i32                     params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetTextureParameterivEXT)
        glIsTexture(texture);
    }
    glGetTextureParameterivEXT(
        texture,
        static_cast<GLenum>(target),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetTextureParameterivEXT"sv);
}

template<class span_const_void>
requires(semantic::concepts::Span<span_const_void>) STATICINLINE
    void texture_image_1d(
        u32                    texture,
        group::texture_target  target,
        i32                    level,
        i32                    internalformat,
        i32                    width,
        i32                    border,
        group::pixel_format    format,
        group::pixel_type      type,
        span_const_void const& pixels)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TextureImage1DEXT)
        glIsTexture(texture);
    }
    glTextureImage1DEXT(
        texture,
        static_cast<GLenum>(target),
        level,
        internalformat,
        width,
        border,
        static_cast<GLenum>(format),
        static_cast<GLenum>(type),
        pixels.size() ? reinterpret_cast<const void*>(pixels.data()) : nullptr);
    detail::error_check("TextureImage1DEXT"sv);
}

template<class size_2_i32, class span_const_void>
requires(semantic::concepts::Size2D<size_2_i32, i32>&&
             semantic::concepts::Span<span_const_void>) STATICINLINE
    void texture_image_2d(
        u32                    texture,
        group::texture_target  target,
        i32                    level,
        i32                    internalformat,
        size_2_i32 const&      width,
        i32                    border,
        group::pixel_format    format,
        group::pixel_type      type,
        span_const_void const& pixels)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TextureImage2DEXT)
        glIsTexture(texture);
    }
    glTextureImage2DEXT(
        texture,
        static_cast<GLenum>(target),
        level,
        internalformat,
        width[0],
        width[1],
        border,
        static_cast<GLenum>(format),
        static_cast<GLenum>(type),
        pixels.size() ? reinterpret_cast<const void*>(pixels.data()) : nullptr);
    detail::error_check("TextureImage2DEXT"sv);
}

STATICINLINE void texture_parameter(
    u32                           texture,
    group::texture_target         target,
    group::texture_parameter_name pname,
    f32                           param)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TextureParameterfEXT)
        glIsTexture(texture);
    }
    glTextureParameterfEXT(
        texture,
        static_cast<GLenum>(target),
        static_cast<GLenum>(pname),
        param);
    detail::error_check("TextureParameterfEXT"sv);
}

template<class span_const_f32>
requires(semantic::concepts::Span<span_const_f32>&& std::is_same_v<
         std::decay_t<typename span_const_f32::value_type>,
         std::decay_t<f32>>) STATICINLINE
    void texture_parameter(
        u32                           texture,
        group::texture_target         target,
        group::texture_parameter_name pname,
        span_const_f32 const&         params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TextureParameterfvEXT)
        glIsTexture(texture);
    }
    glTextureParameterfvEXT(
        texture,
        static_cast<GLenum>(target),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<const GLfloat*>(params.data())
                      : nullptr);
    detail::error_check("TextureParameterfvEXT"sv);
}

STATICINLINE void texture_parameter(
    u32                           texture,
    group::texture_target         target,
    group::texture_parameter_name pname,
    i32                           param)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TextureParameteriEXT)
        glIsTexture(texture);
    }
    glTextureParameteriEXT(
        texture,
        static_cast<GLenum>(target),
        static_cast<GLenum>(pname),
        param);
    detail::error_check("TextureParameteriEXT"sv);
}

template<class span_const_i32>
requires(semantic::concepts::Span<span_const_i32>&& std::is_same_v<
         std::decay_t<typename span_const_i32::value_type>,
         std::decay_t<i32>>) STATICINLINE
    void texture_parameter(
        u32                           texture,
        group::texture_target         target,
        group::texture_parameter_name pname,
        span_const_i32 const&         params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TextureParameterivEXT)
        glIsTexture(texture);
    }
    glTextureParameterivEXT(
        texture,
        static_cast<GLenum>(target),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<const GLint*>(params.data())
                      : nullptr);
    detail::error_check("TextureParameterivEXT"sv);
}

template<class span_const_void>
requires(semantic::concepts::Span<span_const_void>) STATICINLINE
    void texture_sub_image_1d(
        u32                    texture,
        group::texture_target  target,
        i32                    level,
        i32                    xoffset,
        i32                    width,
        group::pixel_format    format,
        group::pixel_type      type,
        span_const_void const& pixels)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TextureSubImage1DEXT)
        glIsTexture(texture);
    }
    glTextureSubImage1DEXT(
        texture,
        static_cast<GLenum>(target),
        level,
        xoffset,
        width,
        static_cast<GLenum>(format),
        static_cast<GLenum>(type),
        pixels.size() ? reinterpret_cast<const void*>(pixels.data()) : nullptr);
    detail::error_check("TextureSubImage1DEXT"sv);
}

template<class size_2_i32, class span_const_void, class vec_2_i32>
requires(semantic::concepts::Vector<vec_2_i32, i32, 2>&&
                 semantic::concepts::Size2D<size_2_i32, i32>&&
                 semantic::concepts::Span<span_const_void>) STATICINLINE
    void texture_sub_image_2d(
        u32                    texture,
        group::texture_target  target,
        i32                    level,
        vec_2_i32 const&       xoffset,
        size_2_i32 const&      width,
        group::pixel_format    format,
        group::pixel_type      type,
        span_const_void const& pixels)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TextureSubImage2DEXT)
        glIsTexture(texture);
    }
    glTextureSubImage2DEXT(
        texture,
        static_cast<GLenum>(target),
        level,
        xoffset.x(),
        xoffset.y(),
        width[0],
        width[1],
        static_cast<GLenum>(format),
        static_cast<GLenum>(type),
        pixels.size() ? reinterpret_cast<const void*>(pixels.data()) : nullptr);
    detail::error_check("TextureSubImage2DEXT"sv);
}

template<class size_2_i32, class vec_2_i32, class vec_3_i32>
requires(semantic::concepts::Vector<vec_3_i32, i32, 3>&&
                 semantic::concepts::Vector<vec_2_i32, i32, 2>&&
                 semantic::concepts::Size2D<size_2_i32, i32>) STATICINLINE
    void copy_texture_sub_image_3d(
        u32                   texture,
        group::texture_target target,
        i32                   level,
        vec_3_i32 const&      xoffset,
        vec_2_i32 const&      x,
        size_2_i32 const&     width)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CopyTextureSubImage3DEXT)
        glIsTexture(texture);
    }
    glCopyTextureSubImage3DEXT(
        texture,
        static_cast<GLenum>(target),
        level,
        xoffset.x(),
        xoffset.y(),
        xoffset.z(),
        x.x(),
        x.y(),
        width[0],
        width[1]);
    detail::error_check("CopyTextureSubImage3DEXT"sv);
}

template<class size_3_i32, class span_const_void>
requires(semantic::concepts::Size2D<size_3_i32, i32>&&
             semantic::concepts::Span<span_const_void>) STATICINLINE
    void texture_image_3d(
        u32                    texture,
        group::texture_target  target,
        i32                    level,
        i32                    internalformat,
        size_3_i32 const&      width,
        i32                    border,
        group::pixel_format    format,
        group::pixel_type      type,
        span_const_void const& pixels)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TextureImage3DEXT)
        glIsTexture(texture);
    }
    glTextureImage3DEXT(
        texture,
        static_cast<GLenum>(target),
        level,
        internalformat,
        width[0],
        width[1],
        width[2],
        border,
        static_cast<GLenum>(format),
        static_cast<GLenum>(type),
        pixels.size() ? reinterpret_cast<const void*>(pixels.data()) : nullptr);
    detail::error_check("TextureImage3DEXT"sv);
}

template<class size_3_i32, class span_const_void, class vec_3_i32>
requires(semantic::concepts::Vector<vec_3_i32, i32, 3>&&
                 semantic::concepts::Size2D<size_3_i32, i32>&&
                 semantic::concepts::Span<span_const_void>) STATICINLINE
    void texture_sub_image_3d(
        u32                    texture,
        group::texture_target  target,
        i32                    level,
        vec_3_i32 const&       xoffset,
        size_3_i32 const&      width,
        group::pixel_format    format,
        group::pixel_type      type,
        span_const_void const& pixels)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TextureSubImage3DEXT)
        glIsTexture(texture);
    }
    glTextureSubImage3DEXT(
        texture,
        static_cast<GLenum>(target),
        level,
        xoffset.x(),
        xoffset.y(),
        xoffset.z(),
        width[0],
        width[1],
        width[2],
        static_cast<GLenum>(format),
        static_cast<GLenum>(type),
        pixels.size() ? reinterpret_cast<const void*>(pixels.data()) : nullptr);
    detail::error_check("TextureSubImage3DEXT"sv);
}

STATICINLINE void bind_multi_texture(
    group::texture_unit texunit, group::texture_target target, u32 texture)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BindMultiTextureEXT)
        glIsTexture(texture);
    }
    glBindMultiTextureEXT(
        static_cast<GLenum>(texunit), static_cast<GLenum>(target), texture);
    detail::error_check("BindMultiTextureEXT"sv);
}

template<class vec_2_i32>
requires(semantic::concepts::Vector<vec_2_i32, i32, 2>) STATICINLINE
    void copy_multi_tex_image_1d(
        group::texture_unit    texunit,
        group::texture_target  target,
        i32                    level,
        group::internal_format internalformat,
        vec_2_i32 const&       x,
        i32                    width,
        i32                    border)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CopyMultiTexImage1DEXT)
    }
    glCopyMultiTexImage1DEXT(
        static_cast<GLenum>(texunit),
        static_cast<GLenum>(target),
        level,
        static_cast<GLenum>(internalformat),
        x.x(),
        x.y(),
        width,
        border);
    detail::error_check("CopyMultiTexImage1DEXT"sv);
}

template<class size_2_i32, class vec_2_i32>
requires(semantic::concepts::Vector<vec_2_i32, i32, 2>&&
             semantic::concepts::Size2D<size_2_i32, i32>) STATICINLINE
    void copy_multi_tex_image_2d(
        group::texture_unit    texunit,
        group::texture_target  target,
        i32                    level,
        group::internal_format internalformat,
        vec_2_i32 const&       x,
        size_2_i32 const&      width,
        i32                    border)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CopyMultiTexImage2DEXT)
    }
    glCopyMultiTexImage2DEXT(
        static_cast<GLenum>(texunit),
        static_cast<GLenum>(target),
        level,
        static_cast<GLenum>(internalformat),
        x.x(),
        x.y(),
        width[0],
        width[1],
        border);
    detail::error_check("CopyMultiTexImage2DEXT"sv);
}

template<class vec_2_i32>
requires(semantic::concepts::Vector<vec_2_i32, i32, 2>) STATICINLINE
    void copy_multi_tex_sub_image_1d(
        group::texture_unit   texunit,
        group::texture_target target,
        i32                   level,
        i32                   xoffset,
        vec_2_i32 const&      x,
        i32                   width)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CopyMultiTexSubImage1DEXT)
    }
    glCopyMultiTexSubImage1DEXT(
        static_cast<GLenum>(texunit),
        static_cast<GLenum>(target),
        level,
        xoffset,
        x.x(),
        x.y(),
        width);
    detail::error_check("CopyMultiTexSubImage1DEXT"sv);
}

template<class size_2_i32, class vec_2_i32>
requires(semantic::concepts::Vector<vec_2_i32, i32, 2>&&
             semantic::concepts::Size2D<size_2_i32, i32>) STATICINLINE
    void copy_multi_tex_sub_image_2d(
        group::texture_unit   texunit,
        group::texture_target target,
        i32                   level,
        vec_2_i32 const&      xoffset,
        vec_2_i32 const&      x,
        size_2_i32 const&     width)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CopyMultiTexSubImage2DEXT)
    }
    glCopyMultiTexSubImage2DEXT(
        static_cast<GLenum>(texunit),
        static_cast<GLenum>(target),
        level,
        xoffset.x(),
        xoffset.y(),
        x.x(),
        x.y(),
        width[0],
        width[1]);
    detail::error_check("CopyMultiTexSubImage2DEXT"sv);
}

template<class size_2_i32, class vec_2_i32, class vec_3_i32>
requires(semantic::concepts::Vector<vec_3_i32, i32, 3>&&
                 semantic::concepts::Vector<vec_2_i32, i32, 2>&&
                 semantic::concepts::Size2D<size_2_i32, i32>) STATICINLINE
    void copy_multi_tex_sub_image_3d(
        group::texture_unit   texunit,
        group::texture_target target,
        i32                   level,
        vec_3_i32 const&      xoffset,
        vec_2_i32 const&      x,
        size_2_i32 const&     width)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CopyMultiTexSubImage3DEXT)
    }
    glCopyMultiTexSubImage3DEXT(
        static_cast<GLenum>(texunit),
        static_cast<GLenum>(target),
        level,
        xoffset.x(),
        xoffset.y(),
        xoffset.z(),
        x.x(),
        x.y(),
        width[0],
        width[1]);
    detail::error_check("CopyMultiTexSubImage3DEXT"sv);
}

template<class span_f32>
requires(semantic::concepts::Span<span_f32>&& std::is_same_v<
         std::decay_t<typename span_f32::value_type>,
         std::decay_t<f32>>) STATICINLINE
    void get_multi_tex_envfv(
        group::texture_unit          texunit,
        group::texture_env_target    target,
        group::texture_env_parameter pname,
        span_f32                     params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetMultiTexEnvfvEXT)
    }
    glGetMultiTexEnvfvEXT(
        static_cast<GLenum>(texunit),
        static_cast<GLenum>(target),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLfloat*>(params.data()) : nullptr);
    detail::error_check("GetMultiTexEnvfvEXT"sv);
}

template<class span_i32>
requires(semantic::concepts::Span<span_i32>&& std::is_same_v<
         std::decay_t<typename span_i32::value_type>,
         std::decay_t<i32>>) STATICINLINE
    void get_multi_tex_enviv(
        group::texture_unit          texunit,
        group::texture_env_target    target,
        group::texture_env_parameter pname,
        span_i32                     params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetMultiTexEnvivEXT)
    }
    glGetMultiTexEnvivEXT(
        static_cast<GLenum>(texunit),
        static_cast<GLenum>(target),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetMultiTexEnvivEXT"sv);
}

template<class span_f64>
requires(semantic::concepts::Span<span_f64>&& std::is_same_v<
         std::decay_t<typename span_f64::value_type>,
         std::decay_t<f64>>) STATICINLINE
    void get_multi_tex_gendv(
        group::texture_unit          texunit,
        group::texture_coord_name    coord,
        group::texture_gen_parameter pname,
        span_f64                     params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetMultiTexGendvEXT)
    }
    glGetMultiTexGendvEXT(
        static_cast<GLenum>(texunit),
        static_cast<GLenum>(coord),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLdouble*>(params.data()) : nullptr);
    detail::error_check("GetMultiTexGendvEXT"sv);
}

template<class span_f32>
requires(semantic::concepts::Span<span_f32>&& std::is_same_v<
         std::decay_t<typename span_f32::value_type>,
         std::decay_t<f32>>) STATICINLINE
    void get_multi_tex_genfv(
        group::texture_unit          texunit,
        group::texture_coord_name    coord,
        group::texture_gen_parameter pname,
        span_f32                     params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetMultiTexGenfvEXT)
    }
    glGetMultiTexGenfvEXT(
        static_cast<GLenum>(texunit),
        static_cast<GLenum>(coord),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLfloat*>(params.data()) : nullptr);
    detail::error_check("GetMultiTexGenfvEXT"sv);
}

template<class span_i32>
requires(semantic::concepts::Span<span_i32>&& std::is_same_v<
         std::decay_t<typename span_i32::value_type>,
         std::decay_t<i32>>) STATICINLINE
    void get_multi_tex_geniv(
        group::texture_unit          texunit,
        group::texture_coord_name    coord,
        group::texture_gen_parameter pname,
        span_i32                     params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetMultiTexGenivEXT)
    }
    glGetMultiTexGenivEXT(
        static_cast<GLenum>(texunit),
        static_cast<GLenum>(coord),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetMultiTexGenivEXT"sv);
}

template<class span_void>
requires(semantic::concepts::Span<span_void>) STATICINLINE
    void get_multi_tex_image(
        group::texture_unit   texunit,
        group::texture_target target,
        i32                   level,
        group::pixel_format   format,
        group::pixel_type     type,
        span_void             pixels)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetMultiTexImageEXT)
    }
    glGetMultiTexImageEXT(
        static_cast<GLenum>(texunit),
        static_cast<GLenum>(target),
        level,
        static_cast<GLenum>(format),
        static_cast<GLenum>(type),
        pixels.size() ? reinterpret_cast<void*>(pixels.data()) : nullptr);
    detail::error_check("GetMultiTexImageEXT"sv);
}

template<class span_f32>
requires(semantic::concepts::Span<span_f32>&& std::is_same_v<
         std::decay_t<typename span_f32::value_type>,
         std::decay_t<f32>>) STATICINLINE
    void get_multi_tex_level_parameter(
        group::texture_unit          texunit,
        group::texture_target        target,
        i32                          level,
        group::get_texture_parameter pname,
        span_f32                     params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetMultiTexLevelParameterfvEXT)
    }
    glGetMultiTexLevelParameterfvEXT(
        static_cast<GLenum>(texunit),
        static_cast<GLenum>(target),
        level,
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLfloat*>(params.data()) : nullptr);
    detail::error_check("GetMultiTexLevelParameterfvEXT"sv);
}

template<class span_i32>
requires(semantic::concepts::Span<span_i32>&& std::is_same_v<
         std::decay_t<typename span_i32::value_type>,
         std::decay_t<i32>>) STATICINLINE
    void get_multi_tex_level_parameter(
        group::texture_unit          texunit,
        group::texture_target        target,
        i32                          level,
        group::get_texture_parameter pname,
        span_i32                     params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetMultiTexLevelParameterivEXT)
    }
    glGetMultiTexLevelParameterivEXT(
        static_cast<GLenum>(texunit),
        static_cast<GLenum>(target),
        level,
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetMultiTexLevelParameterivEXT"sv);
}

template<class span_f32>
requires(semantic::concepts::Span<span_f32>&& std::is_same_v<
         std::decay_t<typename span_f32::value_type>,
         std::decay_t<f32>>) STATICINLINE
    void get_multi_tex_parameter(
        group::texture_unit          texunit,
        group::texture_target        target,
        group::get_texture_parameter pname,
        span_f32                     params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetMultiTexParameterfvEXT)
    }
    glGetMultiTexParameterfvEXT(
        static_cast<GLenum>(texunit),
        static_cast<GLenum>(target),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLfloat*>(params.data()) : nullptr);
    detail::error_check("GetMultiTexParameterfvEXT"sv);
}

template<class span_i32>
requires(semantic::concepts::Span<span_i32>&& std::is_same_v<
         std::decay_t<typename span_i32::value_type>,
         std::decay_t<i32>>) STATICINLINE
    void get_multi_tex_parameter(
        group::texture_unit          texunit,
        group::texture_target        target,
        group::get_texture_parameter pname,
        span_i32                     params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetMultiTexParameterivEXT)
    }
    glGetMultiTexParameterivEXT(
        static_cast<GLenum>(texunit),
        static_cast<GLenum>(target),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetMultiTexParameterivEXT"sv);
}

template<class span_const_void>
requires(semantic::concepts::Span<span_const_void>) STATICINLINE
    void multi_tex_coord_pointer(
        group::texture_unit           texunit,
        i32                           size,
        group::tex_coord_pointer_type type,
        i32                           stride,
        span_const_void const&        pointer)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MultiTexCoordPointerEXT)
    }
    glMultiTexCoordPointerEXT(
        static_cast<GLenum>(texunit),
        size,
        static_cast<GLenum>(type),
        stride,
        pointer.size() ? reinterpret_cast<const void*>(pointer.data())
                       : nullptr);
    detail::error_check("MultiTexCoordPointerEXT"sv);
}

STATICINLINE void multi_tex_envf(
    group::texture_unit          texunit,
    group::texture_env_target    target,
    group::texture_env_parameter pname,
    f32                          param)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MultiTexEnvfEXT)
    }
    glMultiTexEnvfEXT(
        static_cast<GLenum>(texunit),
        static_cast<GLenum>(target),
        static_cast<GLenum>(pname),
        param);
    detail::error_check("MultiTexEnvfEXT"sv);
}

template<class span_const_f32>
requires(semantic::concepts::Span<span_const_f32>&& std::is_same_v<
         std::decay_t<typename span_const_f32::value_type>,
         std::decay_t<f32>>) STATICINLINE
    void multi_tex_envfv(
        group::texture_unit          texunit,
        group::texture_env_target    target,
        group::texture_env_parameter pname,
        span_const_f32 const&        params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MultiTexEnvfvEXT)
    }
    glMultiTexEnvfvEXT(
        static_cast<GLenum>(texunit),
        static_cast<GLenum>(target),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<const GLfloat*>(params.data())
                      : nullptr);
    detail::error_check("MultiTexEnvfvEXT"sv);
}

STATICINLINE void multi_tex_envi(
    group::texture_unit          texunit,
    group::texture_env_target    target,
    group::texture_env_parameter pname,
    i32                          param)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MultiTexEnviEXT)
    }
    glMultiTexEnviEXT(
        static_cast<GLenum>(texunit),
        static_cast<GLenum>(target),
        static_cast<GLenum>(pname),
        param);
    detail::error_check("MultiTexEnviEXT"sv);
}

template<class span_const_i32>
requires(semantic::concepts::Span<span_const_i32>&& std::is_same_v<
         std::decay_t<typename span_const_i32::value_type>,
         std::decay_t<i32>>) STATICINLINE
    void multi_tex_enviv(
        group::texture_unit          texunit,
        group::texture_env_target    target,
        group::texture_env_parameter pname,
        span_const_i32 const&        params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MultiTexEnvivEXT)
    }
    glMultiTexEnvivEXT(
        static_cast<GLenum>(texunit),
        static_cast<GLenum>(target),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<const GLint*>(params.data())
                      : nullptr);
    detail::error_check("MultiTexEnvivEXT"sv);
}

STATICINLINE void multi_tex_gend(
    group::texture_unit          texunit,
    group::texture_coord_name    coord,
    group::texture_gen_parameter pname,
    f64                          param)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MultiTexGendEXT)
    }
    glMultiTexGendEXT(
        static_cast<GLenum>(texunit),
        static_cast<GLenum>(coord),
        static_cast<GLenum>(pname),
        param);
    detail::error_check("MultiTexGendEXT"sv);
}

template<class span_const_f64>
requires(semantic::concepts::Span<span_const_f64>&& std::is_same_v<
         std::decay_t<typename span_const_f64::value_type>,
         std::decay_t<f64>>) STATICINLINE
    void multi_tex_gendv(
        group::texture_unit          texunit,
        group::texture_coord_name    coord,
        group::texture_gen_parameter pname,
        span_const_f64 const&        params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MultiTexGendvEXT)
    }
    glMultiTexGendvEXT(
        static_cast<GLenum>(texunit),
        static_cast<GLenum>(coord),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<const GLdouble*>(params.data())
                      : nullptr);
    detail::error_check("MultiTexGendvEXT"sv);
}

STATICINLINE void multi_tex_genf(
    group::texture_unit          texunit,
    group::texture_coord_name    coord,
    group::texture_gen_parameter pname,
    f32                          param)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MultiTexGenfEXT)
    }
    glMultiTexGenfEXT(
        static_cast<GLenum>(texunit),
        static_cast<GLenum>(coord),
        static_cast<GLenum>(pname),
        param);
    detail::error_check("MultiTexGenfEXT"sv);
}

template<class span_const_f32>
requires(semantic::concepts::Span<span_const_f32>&& std::is_same_v<
         std::decay_t<typename span_const_f32::value_type>,
         std::decay_t<f32>>) STATICINLINE
    void multi_tex_genfv(
        group::texture_unit          texunit,
        group::texture_coord_name    coord,
        group::texture_gen_parameter pname,
        span_const_f32 const&        params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MultiTexGenfvEXT)
    }
    glMultiTexGenfvEXT(
        static_cast<GLenum>(texunit),
        static_cast<GLenum>(coord),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<const GLfloat*>(params.data())
                      : nullptr);
    detail::error_check("MultiTexGenfvEXT"sv);
}

STATICINLINE void multi_tex_geni(
    group::texture_unit          texunit,
    group::texture_coord_name    coord,
    group::texture_gen_parameter pname,
    i32                          param)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MultiTexGeniEXT)
    }
    glMultiTexGeniEXT(
        static_cast<GLenum>(texunit),
        static_cast<GLenum>(coord),
        static_cast<GLenum>(pname),
        param);
    detail::error_check("MultiTexGeniEXT"sv);
}

template<class span_const_i32>
requires(semantic::concepts::Span<span_const_i32>&& std::is_same_v<
         std::decay_t<typename span_const_i32::value_type>,
         std::decay_t<i32>>) STATICINLINE
    void multi_tex_geniv(
        group::texture_unit          texunit,
        group::texture_coord_name    coord,
        group::texture_gen_parameter pname,
        span_const_i32 const&        params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MultiTexGenivEXT)
    }
    glMultiTexGenivEXT(
        static_cast<GLenum>(texunit),
        static_cast<GLenum>(coord),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<const GLint*>(params.data())
                      : nullptr);
    detail::error_check("MultiTexGenivEXT"sv);
}

template<class span_const_void>
requires(semantic::concepts::Span<span_const_void>) STATICINLINE
    void multi_tex_image_1d(
        group::texture_unit    texunit,
        group::texture_target  target,
        i32                    level,
        i32                    internalformat,
        i32                    width,
        i32                    border,
        group::pixel_format    format,
        group::pixel_type      type,
        span_const_void const& pixels)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MultiTexImage1DEXT)
    }
    glMultiTexImage1DEXT(
        static_cast<GLenum>(texunit),
        static_cast<GLenum>(target),
        level,
        internalformat,
        width,
        border,
        static_cast<GLenum>(format),
        static_cast<GLenum>(type),
        pixels.size() ? reinterpret_cast<const void*>(pixels.data()) : nullptr);
    detail::error_check("MultiTexImage1DEXT"sv);
}

template<class size_2_i32, class span_const_void>
requires(semantic::concepts::Size2D<size_2_i32, i32>&&
             semantic::concepts::Span<span_const_void>) STATICINLINE
    void multi_tex_image_2d(
        group::texture_unit    texunit,
        group::texture_target  target,
        i32                    level,
        i32                    internalformat,
        size_2_i32 const&      width,
        i32                    border,
        group::pixel_format    format,
        group::pixel_type      type,
        span_const_void const& pixels)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MultiTexImage2DEXT)
    }
    glMultiTexImage2DEXT(
        static_cast<GLenum>(texunit),
        static_cast<GLenum>(target),
        level,
        internalformat,
        width[0],
        width[1],
        border,
        static_cast<GLenum>(format),
        static_cast<GLenum>(type),
        pixels.size() ? reinterpret_cast<const void*>(pixels.data()) : nullptr);
    detail::error_check("MultiTexImage2DEXT"sv);
}

template<class size_3_i32, class span_const_void>
requires(semantic::concepts::Size2D<size_3_i32, i32>&&
             semantic::concepts::Span<span_const_void>) STATICINLINE
    void multi_tex_image_3d(
        group::texture_unit    texunit,
        group::texture_target  target,
        i32                    level,
        i32                    internalformat,
        size_3_i32 const&      width,
        i32                    border,
        group::pixel_format    format,
        group::pixel_type      type,
        span_const_void const& pixels)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MultiTexImage3DEXT)
    }
    glMultiTexImage3DEXT(
        static_cast<GLenum>(texunit),
        static_cast<GLenum>(target),
        level,
        internalformat,
        width[0],
        width[1],
        width[2],
        border,
        static_cast<GLenum>(format),
        static_cast<GLenum>(type),
        pixels.size() ? reinterpret_cast<const void*>(pixels.data()) : nullptr);
    detail::error_check("MultiTexImage3DEXT"sv);
}

STATICINLINE void multi_tex_parameter(
    group::texture_unit           texunit,
    group::texture_target         target,
    group::texture_parameter_name pname,
    f32                           param)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MultiTexParameterfEXT)
    }
    glMultiTexParameterfEXT(
        static_cast<GLenum>(texunit),
        static_cast<GLenum>(target),
        static_cast<GLenum>(pname),
        param);
    detail::error_check("MultiTexParameterfEXT"sv);
}

template<class span_const_f32>
requires(semantic::concepts::Span<span_const_f32>&& std::is_same_v<
         std::decay_t<typename span_const_f32::value_type>,
         std::decay_t<f32>>) STATICINLINE
    void multi_tex_parameter(
        group::texture_unit           texunit,
        group::texture_target         target,
        group::texture_parameter_name pname,
        span_const_f32 const&         params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MultiTexParameterfvEXT)
    }
    glMultiTexParameterfvEXT(
        static_cast<GLenum>(texunit),
        static_cast<GLenum>(target),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<const GLfloat*>(params.data())
                      : nullptr);
    detail::error_check("MultiTexParameterfvEXT"sv);
}

STATICINLINE void multi_tex_parameter(
    group::texture_unit           texunit,
    group::texture_target         target,
    group::texture_parameter_name pname,
    i32                           param)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MultiTexParameteriEXT)
    }
    glMultiTexParameteriEXT(
        static_cast<GLenum>(texunit),
        static_cast<GLenum>(target),
        static_cast<GLenum>(pname),
        param);
    detail::error_check("MultiTexParameteriEXT"sv);
}

template<class span_const_i32>
requires(semantic::concepts::Span<span_const_i32>&& std::is_same_v<
         std::decay_t<typename span_const_i32::value_type>,
         std::decay_t<i32>>) STATICINLINE
    void multi_tex_parameter(
        group::texture_unit           texunit,
        group::texture_target         target,
        group::texture_parameter_name pname,
        span_const_i32 const&         params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MultiTexParameterivEXT)
    }
    glMultiTexParameterivEXT(
        static_cast<GLenum>(texunit),
        static_cast<GLenum>(target),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<const GLint*>(params.data())
                      : nullptr);
    detail::error_check("MultiTexParameterivEXT"sv);
}

template<class span_const_void>
requires(semantic::concepts::Span<span_const_void>) STATICINLINE
    void multi_tex_sub_image_1d(
        group::texture_unit    texunit,
        group::texture_target  target,
        i32                    level,
        i32                    xoffset,
        i32                    width,
        group::pixel_format    format,
        group::pixel_type      type,
        span_const_void const& pixels)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MultiTexSubImage1DEXT)
    }
    glMultiTexSubImage1DEXT(
        static_cast<GLenum>(texunit),
        static_cast<GLenum>(target),
        level,
        xoffset,
        width,
        static_cast<GLenum>(format),
        static_cast<GLenum>(type),
        pixels.size() ? reinterpret_cast<const void*>(pixels.data()) : nullptr);
    detail::error_check("MultiTexSubImage1DEXT"sv);
}

template<class size_2_i32, class span_const_void, class vec_2_i32>
requires(semantic::concepts::Vector<vec_2_i32, i32, 2>&&
                 semantic::concepts::Size2D<size_2_i32, i32>&&
                 semantic::concepts::Span<span_const_void>) STATICINLINE
    void multi_tex_sub_image_2d(
        group::texture_unit    texunit,
        group::texture_target  target,
        i32                    level,
        vec_2_i32 const&       xoffset,
        size_2_i32 const&      width,
        group::pixel_format    format,
        group::pixel_type      type,
        span_const_void const& pixels)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MultiTexSubImage2DEXT)
    }
    glMultiTexSubImage2DEXT(
        static_cast<GLenum>(texunit),
        static_cast<GLenum>(target),
        level,
        xoffset.x(),
        xoffset.y(),
        width[0],
        width[1],
        static_cast<GLenum>(format),
        static_cast<GLenum>(type),
        pixels.size() ? reinterpret_cast<const void*>(pixels.data()) : nullptr);
    detail::error_check("MultiTexSubImage2DEXT"sv);
}

template<class size_3_i32, class span_const_void, class vec_3_i32>
requires(semantic::concepts::Vector<vec_3_i32, i32, 3>&&
                 semantic::concepts::Size2D<size_3_i32, i32>&&
                 semantic::concepts::Span<span_const_void>) STATICINLINE
    void multi_tex_sub_image_3d(
        group::texture_unit    texunit,
        group::texture_target  target,
        i32                    level,
        vec_3_i32 const&       xoffset,
        size_3_i32 const&      width,
        group::pixel_format    format,
        group::pixel_type      type,
        span_const_void const& pixels)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MultiTexSubImage3DEXT)
    }
    glMultiTexSubImage3DEXT(
        static_cast<GLenum>(texunit),
        static_cast<GLenum>(target),
        level,
        xoffset.x(),
        xoffset.y(),
        xoffset.z(),
        width[0],
        width[1],
        width[2],
        static_cast<GLenum>(format),
        static_cast<GLenum>(type),
        pixels.size() ? reinterpret_cast<const void*>(pixels.data()) : nullptr);
    detail::error_check("MultiTexSubImage3DEXT"sv);
}

STATICINLINE void disable_client_state_indexed(
    group::enable_cap array, u32 index)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DisableClientStateIndexedEXT)
    }
    glDisableClientStateIndexedEXT(static_cast<GLenum>(array), index);
    detail::error_check("DisableClientStateIndexedEXT"sv);
}

STATICINLINE void enable_client_state_indexed(
    group::enable_cap array, u32 index)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(EnableClientStateIndexedEXT)
    }
    glEnableClientStateIndexedEXT(static_cast<GLenum>(array), index);
    detail::error_check("EnableClientStateIndexedEXT"sv);
}

template<class span_f64>
requires(semantic::concepts::Span<span_f64>&& std::is_same_v<
         std::decay_t<typename span_f64::value_type>,
         std::decay_t<f64>>) STATICINLINE
    void get_double_indexedv(group::get_prop target, u32 index, span_f64 data)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetDoubleIndexedvEXT)
    }
    glGetDoubleIndexedvEXT(
        static_cast<GLenum>(target),
        index,
        data.size() ? reinterpret_cast<GLdouble*>(data.data()) : nullptr);
    detail::error_check("GetDoubleIndexedvEXT"sv);
}

template<class span_f32>
requires(semantic::concepts::Span<span_f32>&& std::is_same_v<
         std::decay_t<typename span_f32::value_type>,
         std::decay_t<f32>>) STATICINLINE
    void get_float_indexedv(group::get_prop target, u32 index, span_f32 data)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetFloatIndexedvEXT)
    }
    glGetFloatIndexedvEXT(
        static_cast<GLenum>(target),
        index,
        data.size() ? reinterpret_cast<GLfloat*>(data.data()) : nullptr);
    detail::error_check("GetFloatIndexedvEXT"sv);
}

template<class span_void>
requires(semantic::concepts::Span<span_void>) STATICINLINE
    void get_pointer_indexedv(GLenum target, u32 index, span_void data)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetPointerIndexedvEXT)
    }
    glGetPointerIndexedvEXT(
        target,
        index,
        data.size() ? reinterpret_cast<void**>(data.data()) : nullptr);
    detail::error_check("GetPointerIndexedvEXT"sv);
}

STATICINLINE void disable_indexed(group::enable_cap target, u32 index)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DisableIndexedEXT)
    }
    glDisableIndexedEXT(static_cast<GLenum>(target), index);
    detail::error_check("DisableIndexedEXT"sv);
}

STATICINLINE void enable_indexed(group::enable_cap target, u32 index)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(EnableIndexedEXT)
    }
    glEnableIndexedEXT(static_cast<GLenum>(target), index);
    detail::error_check("EnableIndexedEXT"sv);
}

template<class span_bool>
requires(semantic::concepts::Span<span_bool>&& std::is_same_v<
         std::decay_t<typename span_bool::value_type>,
         std::decay_t<bool>>) STATICINLINE
    void get_boolean_indexedv(
        group::buffer_target_arb target, u32 index, span_bool data)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetBooleanIndexedvEXT)
    }
    glGetBooleanIndexedvEXT(
        static_cast<GLenum>(target),
        index,
        data.size() ? reinterpret_cast<GLboolean*>(data.data()) : nullptr);
    detail::error_check("GetBooleanIndexedvEXT"sv);
}

template<class span_i32>
requires(semantic::concepts::Span<span_i32>&& std::is_same_v<
         std::decay_t<typename span_i32::value_type>,
         std::decay_t<i32>>) STATICINLINE
    void get_integer_indexedv(group::get_prop target, u32 index, span_i32 data)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetIntegerIndexedvEXT)
    }
    glGetIntegerIndexedvEXT(
        static_cast<GLenum>(target),
        index,
        data.size() ? reinterpret_cast<GLint*>(data.data()) : nullptr);
    detail::error_check("GetIntegerIndexedvEXT"sv);
}

STATICINLINE GLboolean is_enabled_indexed(group::enable_cap target, u32 index)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(IsEnabledIndexedEXT)
    }
    auto out = glIsEnabledIndexedEXT(static_cast<GLenum>(target), index);
    detail::error_check("IsEnabledIndexedEXT"sv);
    return out;
}

template<class span_const_void>
requires(semantic::concepts::Span<span_const_void>) STATICINLINE
    void compressed_texture_image_1d(
        u32                    texture,
        group::texture_target  target,
        i32                    level,
        group::internal_format internalformat,
        i32                    width,
        i32                    border,
        span_const_void const& bits)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CompressedTextureImage1DEXT)
        glIsTexture(texture);
    }
    glCompressedTextureImage1DEXT(
        texture,
        static_cast<GLenum>(target),
        level,
        static_cast<GLenum>(internalformat),
        width,
        border,
        bits.size() *
            sizeof(typename std::decay_t<span_const_void const&>::value_type),
        bits.size() ? reinterpret_cast<const void*>(bits.data()) : nullptr);
    detail::error_check("CompressedTextureImage1DEXT"sv);
}

template<class size_2_i32, class span_const_void>
requires(semantic::concepts::Size2D<size_2_i32, i32>&&
             semantic::concepts::Span<span_const_void>) STATICINLINE
    void compressed_texture_image_2d(
        u32                    texture,
        group::texture_target  target,
        i32                    level,
        group::internal_format internalformat,
        size_2_i32 const&      width,
        i32                    border,
        span_const_void const& bits)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CompressedTextureImage2DEXT)
        glIsTexture(texture);
    }
    glCompressedTextureImage2DEXT(
        texture,
        static_cast<GLenum>(target),
        level,
        static_cast<GLenum>(internalformat),
        width[0],
        width[1],
        border,
        bits.size() *
            sizeof(typename std::decay_t<span_const_void const&>::value_type),
        bits.size() ? reinterpret_cast<const void*>(bits.data()) : nullptr);
    detail::error_check("CompressedTextureImage2DEXT"sv);
}

template<class size_3_i32, class span_const_void>
requires(semantic::concepts::Size2D<size_3_i32, i32>&&
             semantic::concepts::Span<span_const_void>) STATICINLINE
    void compressed_texture_image_3d(
        u32                    texture,
        group::texture_target  target,
        i32                    level,
        group::internal_format internalformat,
        size_3_i32 const&      width,
        i32                    border,
        span_const_void const& bits)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CompressedTextureImage3DEXT)
        glIsTexture(texture);
    }
    glCompressedTextureImage3DEXT(
        texture,
        static_cast<GLenum>(target),
        level,
        static_cast<GLenum>(internalformat),
        width[0],
        width[1],
        width[2],
        border,
        bits.size() *
            sizeof(typename std::decay_t<span_const_void const&>::value_type),
        bits.size() ? reinterpret_cast<const void*>(bits.data()) : nullptr);
    detail::error_check("CompressedTextureImage3DEXT"sv);
}

template<class span_const_void>
requires(semantic::concepts::Span<span_const_void>) STATICINLINE
    void compressed_texture_sub_image_1d(
        u32                    texture,
        group::texture_target  target,
        i32                    level,
        i32                    xoffset,
        i32                    width,
        group::pixel_format    format,
        span_const_void const& bits)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CompressedTextureSubImage1DEXT)
        glIsTexture(texture);
    }
    glCompressedTextureSubImage1DEXT(
        texture,
        static_cast<GLenum>(target),
        level,
        xoffset,
        width,
        static_cast<GLenum>(format),
        bits.size() *
            sizeof(typename std::decay_t<span_const_void const&>::value_type),
        bits.size() ? reinterpret_cast<const void*>(bits.data()) : nullptr);
    detail::error_check("CompressedTextureSubImage1DEXT"sv);
}

template<class size_2_i32, class span_const_void, class vec_2_i32>
requires(semantic::concepts::Vector<vec_2_i32, i32, 2>&&
                 semantic::concepts::Size2D<size_2_i32, i32>&&
                 semantic::concepts::Span<span_const_void>) STATICINLINE
    void compressed_texture_sub_image_2d(
        u32                    texture,
        group::texture_target  target,
        i32                    level,
        vec_2_i32 const&       xoffset,
        size_2_i32 const&      width,
        group::pixel_format    format,
        span_const_void const& bits)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CompressedTextureSubImage2DEXT)
        glIsTexture(texture);
    }
    glCompressedTextureSubImage2DEXT(
        texture,
        static_cast<GLenum>(target),
        level,
        xoffset.x(),
        xoffset.y(),
        width[0],
        width[1],
        static_cast<GLenum>(format),
        bits.size() *
            sizeof(typename std::decay_t<span_const_void const&>::value_type),
        bits.size() ? reinterpret_cast<const void*>(bits.data()) : nullptr);
    detail::error_check("CompressedTextureSubImage2DEXT"sv);
}

template<class size_3_i32, class span_const_void, class vec_3_i32>
requires(semantic::concepts::Vector<vec_3_i32, i32, 3>&&
                 semantic::concepts::Size2D<size_3_i32, i32>&&
                 semantic::concepts::Span<span_const_void>) STATICINLINE
    void compressed_texture_sub_image_3d(
        u32                    texture,
        group::texture_target  target,
        i32                    level,
        vec_3_i32 const&       xoffset,
        size_3_i32 const&      width,
        group::pixel_format    format,
        span_const_void const& bits)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CompressedTextureSubImage3DEXT)
        glIsTexture(texture);
    }
    glCompressedTextureSubImage3DEXT(
        texture,
        static_cast<GLenum>(target),
        level,
        xoffset.x(),
        xoffset.y(),
        xoffset.z(),
        width[0],
        width[1],
        width[2],
        static_cast<GLenum>(format),
        bits.size() *
            sizeof(typename std::decay_t<span_const_void const&>::value_type),
        bits.size() ? reinterpret_cast<const void*>(bits.data()) : nullptr);
    detail::error_check("CompressedTextureSubImage3DEXT"sv);
}

template<class span_void>
requires(semantic::concepts::Span<span_void>) STATICINLINE
    void get_compressed_texture_image(
        u32 texture, group::texture_target target, i32 lod, span_void img)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetCompressedTextureImageEXT)
        glIsTexture(texture);
    }
    glGetCompressedTextureImageEXT(
        texture,
        static_cast<GLenum>(target),
        lod,
        img.size() ? reinterpret_cast<void*>(img.data()) : nullptr);
    detail::error_check("GetCompressedTextureImageEXT"sv);
}

template<class span_const_void>
requires(semantic::concepts::Span<span_const_void>) STATICINLINE
    void compressed_multi_tex_image_1d(
        group::texture_unit    texunit,
        group::texture_target  target,
        i32                    level,
        group::internal_format internalformat,
        i32                    width,
        i32                    border,
        span_const_void const& bits)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CompressedMultiTexImage1DEXT)
    }
    glCompressedMultiTexImage1DEXT(
        static_cast<GLenum>(texunit),
        static_cast<GLenum>(target),
        level,
        static_cast<GLenum>(internalformat),
        width,
        border,
        bits.size() *
            sizeof(typename std::decay_t<span_const_void const&>::value_type),
        bits.size() ? reinterpret_cast<const void*>(bits.data()) : nullptr);
    detail::error_check("CompressedMultiTexImage1DEXT"sv);
}

template<class size_2_i32, class span_const_void>
requires(semantic::concepts::Size2D<size_2_i32, i32>&&
             semantic::concepts::Span<span_const_void>) STATICINLINE
    void compressed_multi_tex_image_2d(
        group::texture_unit    texunit,
        group::texture_target  target,
        i32                    level,
        group::internal_format internalformat,
        size_2_i32 const&      width,
        i32                    border,
        span_const_void const& bits)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CompressedMultiTexImage2DEXT)
    }
    glCompressedMultiTexImage2DEXT(
        static_cast<GLenum>(texunit),
        static_cast<GLenum>(target),
        level,
        static_cast<GLenum>(internalformat),
        width[0],
        width[1],
        border,
        bits.size() *
            sizeof(typename std::decay_t<span_const_void const&>::value_type),
        bits.size() ? reinterpret_cast<const void*>(bits.data()) : nullptr);
    detail::error_check("CompressedMultiTexImage2DEXT"sv);
}

template<class size_3_i32, class span_const_void>
requires(semantic::concepts::Size2D<size_3_i32, i32>&&
             semantic::concepts::Span<span_const_void>) STATICINLINE
    void compressed_multi_tex_image_3d(
        group::texture_unit    texunit,
        group::texture_target  target,
        i32                    level,
        group::internal_format internalformat,
        size_3_i32 const&      width,
        i32                    border,
        span_const_void const& bits)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CompressedMultiTexImage3DEXT)
    }
    glCompressedMultiTexImage3DEXT(
        static_cast<GLenum>(texunit),
        static_cast<GLenum>(target),
        level,
        static_cast<GLenum>(internalformat),
        width[0],
        width[1],
        width[2],
        border,
        bits.size() *
            sizeof(typename std::decay_t<span_const_void const&>::value_type),
        bits.size() ? reinterpret_cast<const void*>(bits.data()) : nullptr);
    detail::error_check("CompressedMultiTexImage3DEXT"sv);
}

template<class span_const_void>
requires(semantic::concepts::Span<span_const_void>) STATICINLINE
    void compressed_multi_tex_sub_image_1d(
        group::texture_unit    texunit,
        group::texture_target  target,
        i32                    level,
        i32                    xoffset,
        i32                    width,
        group::pixel_format    format,
        span_const_void const& bits)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CompressedMultiTexSubImage1DEXT)
    }
    glCompressedMultiTexSubImage1DEXT(
        static_cast<GLenum>(texunit),
        static_cast<GLenum>(target),
        level,
        xoffset,
        width,
        static_cast<GLenum>(format),
        bits.size() *
            sizeof(typename std::decay_t<span_const_void const&>::value_type),
        bits.size() ? reinterpret_cast<const void*>(bits.data()) : nullptr);
    detail::error_check("CompressedMultiTexSubImage1DEXT"sv);
}

template<class size_2_i32, class span_const_void, class vec_2_i32>
requires(semantic::concepts::Vector<vec_2_i32, i32, 2>&&
                 semantic::concepts::Size2D<size_2_i32, i32>&&
                 semantic::concepts::Span<span_const_void>) STATICINLINE
    void compressed_multi_tex_sub_image_2d(
        group::texture_unit    texunit,
        group::texture_target  target,
        i32                    level,
        vec_2_i32 const&       xoffset,
        size_2_i32 const&      width,
        group::pixel_format    format,
        span_const_void const& bits)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CompressedMultiTexSubImage2DEXT)
    }
    glCompressedMultiTexSubImage2DEXT(
        static_cast<GLenum>(texunit),
        static_cast<GLenum>(target),
        level,
        xoffset.x(),
        xoffset.y(),
        width[0],
        width[1],
        static_cast<GLenum>(format),
        bits.size() *
            sizeof(typename std::decay_t<span_const_void const&>::value_type),
        bits.size() ? reinterpret_cast<const void*>(bits.data()) : nullptr);
    detail::error_check("CompressedMultiTexSubImage2DEXT"sv);
}

template<class size_3_i32, class span_const_void, class vec_3_i32>
requires(semantic::concepts::Vector<vec_3_i32, i32, 3>&&
                 semantic::concepts::Size2D<size_3_i32, i32>&&
                 semantic::concepts::Span<span_const_void>) STATICINLINE
    void compressed_multi_tex_sub_image_3d(
        group::texture_unit    texunit,
        group::texture_target  target,
        i32                    level,
        vec_3_i32 const&       xoffset,
        size_3_i32 const&      width,
        group::pixel_format    format,
        span_const_void const& bits)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CompressedMultiTexSubImage3DEXT)
    }
    glCompressedMultiTexSubImage3DEXT(
        static_cast<GLenum>(texunit),
        static_cast<GLenum>(target),
        level,
        xoffset.x(),
        xoffset.y(),
        xoffset.z(),
        width[0],
        width[1],
        width[2],
        static_cast<GLenum>(format),
        bits.size() *
            sizeof(typename std::decay_t<span_const_void const&>::value_type),
        bits.size() ? reinterpret_cast<const void*>(bits.data()) : nullptr);
    detail::error_check("CompressedMultiTexSubImage3DEXT"sv);
}

template<class span_void>
requires(semantic::concepts::Span<span_void>) STATICINLINE
    void get_compressed_multi_tex_image(
        group::texture_unit   texunit,
        group::texture_target target,
        i32                   lod,
        span_void             img)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetCompressedMultiTexImageEXT)
    }
    glGetCompressedMultiTexImageEXT(
        static_cast<GLenum>(texunit),
        static_cast<GLenum>(target),
        lod,
        img.size() ? reinterpret_cast<void*>(img.data()) : nullptr);
    detail::error_check("GetCompressedMultiTexImageEXT"sv);
}

template<class span_const_f64>
requires(semantic::concepts::Span<span_const_f64>&& std::is_same_v<
         std::decay_t<typename span_const_f64::value_type>,
         std::decay_t<f64>>) STATICINLINE
    void matrix_load_transposed(
        group::matrix_mode mode, span_const_f64 const& m)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MatrixLoadTransposedEXT)
    }
    glMatrixLoadTransposedEXT(
        static_cast<GLenum>(mode),
        m.size() ? reinterpret_cast<const GLdouble*>(m.data()) : nullptr);
    detail::error_check("MatrixLoadTransposedEXT"sv);
}

template<class span_const_f32>
requires(semantic::concepts::Span<span_const_f32>&& std::is_same_v<
         std::decay_t<typename span_const_f32::value_type>,
         std::decay_t<f32>>) STATICINLINE
    void matrix_load_transposef(
        group::matrix_mode mode, span_const_f32 const& m)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MatrixLoadTransposefEXT)
    }
    glMatrixLoadTransposefEXT(
        static_cast<GLenum>(mode),
        m.size() ? reinterpret_cast<const GLfloat*>(m.data()) : nullptr);
    detail::error_check("MatrixLoadTransposefEXT"sv);
}

template<class span_const_f64>
requires(semantic::concepts::Span<span_const_f64>&& std::is_same_v<
         std::decay_t<typename span_const_f64::value_type>,
         std::decay_t<f64>>) STATICINLINE
    void matrix_mult_transposed(
        group::matrix_mode mode, span_const_f64 const& m)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MatrixMultTransposedEXT)
    }
    glMatrixMultTransposedEXT(
        static_cast<GLenum>(mode),
        m.size() ? reinterpret_cast<const GLdouble*>(m.data()) : nullptr);
    detail::error_check("MatrixMultTransposedEXT"sv);
}

template<class span_const_f32>
requires(semantic::concepts::Span<span_const_f32>&& std::is_same_v<
         std::decay_t<typename span_const_f32::value_type>,
         std::decay_t<f32>>) STATICINLINE
    void matrix_mult_transposef(
        group::matrix_mode mode, span_const_f32 const& m)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MatrixMultTransposefEXT)
    }
    glMatrixMultTransposefEXT(
        static_cast<GLenum>(mode),
        m.size() ? reinterpret_cast<const GLfloat*>(m.data()) : nullptr);
    detail::error_check("MatrixMultTransposefEXT"sv);
}

template<class span_i32>
requires(semantic::concepts::Span<span_i32>&& std::is_same_v<
         std::decay_t<typename span_i32::value_type>,
         std::decay_t<i32>>) STATICINLINE
    void get_named_buffer_parameter(
        u32 buffer, group::buffer_prop_arb pname, span_i32 params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetNamedBufferParameterivEXT)
        glIsBuffer(buffer);
    }
    glGetNamedBufferParameterivEXT(
        buffer,
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetNamedBufferParameterivEXT"sv);
}

template<class span_void>
requires(semantic::concepts::Span<span_void>) STATICINLINE
    void get_named_buffer_pointerv(
        u32 buffer, group::buffer_pointer_name_arb pname, span_void params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetNamedBufferPointervEXT)
        glIsBuffer(buffer);
    }
    glGetNamedBufferPointervEXT(
        buffer,
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<void**>(params.data()) : nullptr);
    detail::error_check("GetNamedBufferPointervEXT"sv);
}

template<class span_void>
requires(semantic::concepts::Span<span_void>) STATICINLINE
    void get_named_buffer_sub_data(
        u32 buffer, GLintptr offset, GLsizeiptr size, span_void data)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetNamedBufferSubDataEXT)
        glIsBuffer(buffer);
    }
    glGetNamedBufferSubDataEXT(
        buffer,
        offset,
        size,
        data.size() ? reinterpret_cast<void*>(data.data()) : nullptr);
    detail::error_check("GetNamedBufferSubDataEXT"sv);
}

STATICINLINE void* map_named_buffer(u32 buffer, group::buffer_access_arb access)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MapNamedBufferEXT)
        glIsBuffer(buffer);
    }
    auto out = glMapNamedBufferEXT(buffer, static_cast<GLenum>(access));
    detail::error_check("MapNamedBufferEXT"sv);
    return out;
}

template<class span_const_void>
requires(semantic::concepts::Span<span_const_void>) STATICINLINE
    void named_buffer_data(
        u32                               buffer,
        GLsizeiptr                        size,
        span_const_void const&            data,
        group::vertex_buffer_object_usage usage)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(NamedBufferDataEXT)
        glIsBuffer(buffer);
    }
    glNamedBufferDataEXT(
        buffer,
        size,
        data.size() ? reinterpret_cast<const void*>(data.data()) : nullptr,
        static_cast<GLenum>(usage));
    detail::error_check("NamedBufferDataEXT"sv);
}

template<class span_const_void>
requires(semantic::concepts::Span<span_const_void>) STATICINLINE
    void named_buffer_sub_data(
        u32                    buffer,
        GLintptr               offset,
        GLsizeiptr             size,
        span_const_void const& data)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(NamedBufferSubDataEXT)
        glIsBuffer(buffer);
    }
    glNamedBufferSubDataEXT(
        buffer,
        offset,
        size,
        data.size() ? reinterpret_cast<const void*>(data.data()) : nullptr);
    detail::error_check("NamedBufferSubDataEXT"sv);
}

STATICINLINE GLboolean unmap_named_buffer(u32 buffer)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(UnmapNamedBufferEXT)
        glIsBuffer(buffer);
    }
    auto out = glUnmapNamedBufferEXT(buffer);
    detail::error_check("UnmapNamedBufferEXT"sv);
    return out;
}

STATICINLINE void program_uniform(u32 program, i32 location, f32 v0)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform1fEXT)
        glIsProgram(program);
    }
    glProgramUniform1fEXT(program, location, v0);
    detail::error_check("ProgramUniform1fEXT"sv);
}

template<class span_const_f32>
requires(semantic::concepts::Span<span_const_f32>&& std::is_same_v<
         std::decay_t<typename span_const_f32::value_type>,
         std::decay_t<f32>>) STATICINLINE
    void program_uniform(u32 program, i32 location, span_const_f32 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform1fvEXT)
        glIsProgram(program);
    }
    glProgramUniform1fvEXT(
        program,
        location,
        value.size(),
        reinterpret_cast<const GLfloat*>(value.data()));
    detail::error_check("ProgramUniform1fvEXT"sv);
}

STATICINLINE void program_uniform(u32 program, i32 location, i32 v0)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform1iEXT)
        glIsProgram(program);
    }
    glProgramUniform1iEXT(program, location, v0);
    detail::error_check("ProgramUniform1iEXT"sv);
}

template<class span_const_i32>
requires(semantic::concepts::Span<span_const_i32>&& std::is_same_v<
         std::decay_t<typename span_const_i32::value_type>,
         std::decay_t<i32>>) STATICINLINE
    void program_uniform(u32 program, i32 location, span_const_i32 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform1ivEXT)
        glIsProgram(program);
    }
    glProgramUniform1ivEXT(
        program,
        location,
        value.size(),
        reinterpret_cast<const GLint*>(value.data()));
    detail::error_check("ProgramUniform1ivEXT"sv);
}

template<class vec_2_f32>
requires(semantic::concepts::Vector<vec_2_f32, f32, 2>) STATICINLINE
    void program_uniform(u32 program, i32 location, vec_2_f32 const& v0)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform2fEXT)
        glIsProgram(program);
    }
    glProgramUniform2fEXT(program, location, v0.x(), v0.y());
    detail::error_check("ProgramUniform2fEXT"sv);
}

template<class span_const_vec_2_f32>
requires(semantic::concepts::Span<span_const_vec_2_f32>&& semantic::concepts::
             Vector<typename span_const_vec_2_f32::value_type, f32, 2>)
    STATICINLINE void program_uniform(
        u32 program, i32 location, i32 count, span_const_vec_2_f32 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform2fvEXT)
        glIsProgram(program);
    }
    glProgramUniform2fvEXT(
        program,
        location,
        count,
        reinterpret_cast<const GLfloat*>(value.data()));
    detail::error_check("ProgramUniform2fvEXT"sv);
}

template<class vec_2_i32>
requires(semantic::concepts::Vector<vec_2_i32, i32, 2>) STATICINLINE
    void program_uniform(u32 program, i32 location, vec_2_i32 const& v0)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform2iEXT)
        glIsProgram(program);
    }
    glProgramUniform2iEXT(program, location, v0.x(), v0.y());
    detail::error_check("ProgramUniform2iEXT"sv);
}

template<class span_const_vec_2_i32>
requires(semantic::concepts::Span<span_const_vec_2_i32>&& semantic::concepts::
             Vector<typename span_const_vec_2_i32::value_type, i32, 2>)
    STATICINLINE void program_uniform(
        u32 program, i32 location, i32 count, span_const_vec_2_i32 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform2ivEXT)
        glIsProgram(program);
    }
    glProgramUniform2ivEXT(
        program, location, count, reinterpret_cast<const GLint*>(value.data()));
    detail::error_check("ProgramUniform2ivEXT"sv);
}

template<class vec_3_f32>
requires(semantic::concepts::Vector<vec_3_f32, f32, 3>) STATICINLINE
    void program_uniform(u32 program, i32 location, vec_3_f32 const& v0)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform3fEXT)
        glIsProgram(program);
    }
    glProgramUniform3fEXT(program, location, v0.x(), v0.y(), v0.z());
    detail::error_check("ProgramUniform3fEXT"sv);
}

template<class span_const_vec_3_f32>
requires(semantic::concepts::Span<span_const_vec_3_f32>&& semantic::concepts::
             Vector<typename span_const_vec_3_f32::value_type, f32, 3>)
    STATICINLINE void program_uniform(
        u32 program, i32 location, i32 count, span_const_vec_3_f32 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform3fvEXT)
        glIsProgram(program);
    }
    glProgramUniform3fvEXT(
        program,
        location,
        count,
        reinterpret_cast<const GLfloat*>(value.data()));
    detail::error_check("ProgramUniform3fvEXT"sv);
}

template<class vec_3_i32>
requires(semantic::concepts::Vector<vec_3_i32, i32, 3>) STATICINLINE
    void program_uniform(u32 program, i32 location, vec_3_i32 const& v0)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform3iEXT)
        glIsProgram(program);
    }
    glProgramUniform3iEXT(program, location, v0.x(), v0.y(), v0.z());
    detail::error_check("ProgramUniform3iEXT"sv);
}

template<class span_const_vec_3_i32>
requires(semantic::concepts::Span<span_const_vec_3_i32>&& semantic::concepts::
             Vector<typename span_const_vec_3_i32::value_type, i32, 3>)
    STATICINLINE void program_uniform(
        u32 program, i32 location, i32 count, span_const_vec_3_i32 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform3ivEXT)
        glIsProgram(program);
    }
    glProgramUniform3ivEXT(
        program, location, count, reinterpret_cast<const GLint*>(value.data()));
    detail::error_check("ProgramUniform3ivEXT"sv);
}

template<class vec_4_f32>
requires(semantic::concepts::Vector<vec_4_f32, f32, 4>) STATICINLINE
    void program_uniform(u32 program, i32 location, vec_4_f32 const& v0)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform4fEXT)
        glIsProgram(program);
    }
    glProgramUniform4fEXT(program, location, v0.x(), v0.y(), v0.z(), v0.w());
    detail::error_check("ProgramUniform4fEXT"sv);
}

template<class span_const_vec_4_f32>
requires(semantic::concepts::Span<span_const_vec_4_f32>&& semantic::concepts::
             Vector<typename span_const_vec_4_f32::value_type, f32, 4>)
    STATICINLINE void program_uniform(
        u32 program, i32 location, i32 count, span_const_vec_4_f32 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform4fvEXT)
        glIsProgram(program);
    }
    glProgramUniform4fvEXT(
        program,
        location,
        count,
        reinterpret_cast<const GLfloat*>(value.data()));
    detail::error_check("ProgramUniform4fvEXT"sv);
}

template<class vec_4_i32>
requires(semantic::concepts::Vector<vec_4_i32, i32, 4>) STATICINLINE
    void program_uniform(u32 program, i32 location, vec_4_i32 const& v0)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform4iEXT)
        glIsProgram(program);
    }
    glProgramUniform4iEXT(program, location, v0.x(), v0.y(), v0.z(), v0.w());
    detail::error_check("ProgramUniform4iEXT"sv);
}

template<class span_const_vec_4_i32>
requires(semantic::concepts::Span<span_const_vec_4_i32>&& semantic::concepts::
             Vector<typename span_const_vec_4_i32::value_type, i32, 4>)
    STATICINLINE void program_uniform(
        u32 program, i32 location, i32 count, span_const_vec_4_i32 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform4ivEXT)
        glIsProgram(program);
    }
    glProgramUniform4ivEXT(
        program, location, count, reinterpret_cast<const GLint*>(value.data()));
    detail::error_check("ProgramUniform4ivEXT"sv);
}

template<class span_const_mat_2x2_f32>
requires(semantic::concepts::Span<span_const_mat_2x2_f32>&& semantic::concepts::
             Matrix<typename span_const_mat_2x2_f32::value_type, f32, 2, 2>)
    STATICINLINE void program_uniform(
        u32                           program,
        i32                           location,
        i32                           count,
        bool                          transpose,
        span_const_mat_2x2_f32 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniformMatrix2fvEXT)
        glIsProgram(program);
    }
    glProgramUniformMatrix2fvEXT(
        program,
        location,
        count,
        transpose,
        reinterpret_cast<const GLfloat*>(value.data()));
    detail::error_check("ProgramUniformMatrix2fvEXT"sv);
}

template<class span_const_mat_3x3_f32>
requires(semantic::concepts::Span<span_const_mat_3x3_f32>&& semantic::concepts::
             Matrix<typename span_const_mat_3x3_f32::value_type, f32, 3, 3>)
    STATICINLINE void program_uniform(
        u32                           program,
        i32                           location,
        i32                           count,
        bool                          transpose,
        span_const_mat_3x3_f32 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniformMatrix3fvEXT)
        glIsProgram(program);
    }
    glProgramUniformMatrix3fvEXT(
        program,
        location,
        count,
        transpose,
        reinterpret_cast<const GLfloat*>(value.data()));
    detail::error_check("ProgramUniformMatrix3fvEXT"sv);
}

template<class span_const_mat_4x4_f32>
requires(semantic::concepts::Span<span_const_mat_4x4_f32>&& semantic::concepts::
             Matrix<typename span_const_mat_4x4_f32::value_type, f32, 4, 4>)
    STATICINLINE void program_uniform(
        u32                           program,
        i32                           location,
        i32                           count,
        bool                          transpose,
        span_const_mat_4x4_f32 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniformMatrix4fvEXT)
        glIsProgram(program);
    }
    glProgramUniformMatrix4fvEXT(
        program,
        location,
        count,
        transpose,
        reinterpret_cast<const GLfloat*>(value.data()));
    detail::error_check("ProgramUniformMatrix4fvEXT"sv);
}

template<class span_const_mat_2x3_f32>
requires(semantic::concepts::Span<span_const_mat_2x3_f32>&& semantic::concepts::
             Matrix<typename span_const_mat_2x3_f32::value_type, f32, 2, 3>)
    STATICINLINE void program_uniform(
        u32                           program,
        i32                           location,
        i32                           count,
        bool                          transpose,
        span_const_mat_2x3_f32 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniformMatrix2x3fvEXT)
        glIsProgram(program);
    }
    glProgramUniformMatrix2x3fvEXT(
        program,
        location,
        count,
        transpose,
        reinterpret_cast<const GLfloat*>(value.data()));
    detail::error_check("ProgramUniformMatrix2x3fvEXT"sv);
}

template<class span_const_mat_2x4_f32>
requires(semantic::concepts::Span<span_const_mat_2x4_f32>&& semantic::concepts::
             Matrix<typename span_const_mat_2x4_f32::value_type, f32, 2, 4>)
    STATICINLINE void program_uniform(
        u32                           program,
        i32                           location,
        i32                           count,
        bool                          transpose,
        span_const_mat_2x4_f32 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniformMatrix2x4fvEXT)
        glIsProgram(program);
    }
    glProgramUniformMatrix2x4fvEXT(
        program,
        location,
        count,
        transpose,
        reinterpret_cast<const GLfloat*>(value.data()));
    detail::error_check("ProgramUniformMatrix2x4fvEXT"sv);
}

template<class span_const_mat_3x2_f32>
requires(semantic::concepts::Span<span_const_mat_3x2_f32>&& semantic::concepts::
             Matrix<typename span_const_mat_3x2_f32::value_type, f32, 3, 2>)
    STATICINLINE void program_uniform(
        u32                           program,
        i32                           location,
        i32                           count,
        bool                          transpose,
        span_const_mat_3x2_f32 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniformMatrix3x2fvEXT)
        glIsProgram(program);
    }
    glProgramUniformMatrix3x2fvEXT(
        program,
        location,
        count,
        transpose,
        reinterpret_cast<const GLfloat*>(value.data()));
    detail::error_check("ProgramUniformMatrix3x2fvEXT"sv);
}

template<class span_const_mat_3x4_f32>
requires(semantic::concepts::Span<span_const_mat_3x4_f32>&& semantic::concepts::
             Matrix<typename span_const_mat_3x4_f32::value_type, f32, 3, 4>)
    STATICINLINE void program_uniform(
        u32                           program,
        i32                           location,
        i32                           count,
        bool                          transpose,
        span_const_mat_3x4_f32 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniformMatrix3x4fvEXT)
        glIsProgram(program);
    }
    glProgramUniformMatrix3x4fvEXT(
        program,
        location,
        count,
        transpose,
        reinterpret_cast<const GLfloat*>(value.data()));
    detail::error_check("ProgramUniformMatrix3x4fvEXT"sv);
}

template<class span_const_mat_4x2_f32>
requires(semantic::concepts::Span<span_const_mat_4x2_f32>&& semantic::concepts::
             Matrix<typename span_const_mat_4x2_f32::value_type, f32, 4, 2>)
    STATICINLINE void program_uniform(
        u32                           program,
        i32                           location,
        i32                           count,
        bool                          transpose,
        span_const_mat_4x2_f32 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniformMatrix4x2fvEXT)
        glIsProgram(program);
    }
    glProgramUniformMatrix4x2fvEXT(
        program,
        location,
        count,
        transpose,
        reinterpret_cast<const GLfloat*>(value.data()));
    detail::error_check("ProgramUniformMatrix4x2fvEXT"sv);
}

template<class span_const_mat_4x3_f32>
requires(semantic::concepts::Span<span_const_mat_4x3_f32>&& semantic::concepts::
             Matrix<typename span_const_mat_4x3_f32::value_type, f32, 4, 3>)
    STATICINLINE void program_uniform(
        u32                           program,
        i32                           location,
        i32                           count,
        bool                          transpose,
        span_const_mat_4x3_f32 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniformMatrix4x3fvEXT)
        glIsProgram(program);
    }
    glProgramUniformMatrix4x3fvEXT(
        program,
        location,
        count,
        transpose,
        reinterpret_cast<const GLfloat*>(value.data()));
    detail::error_check("ProgramUniformMatrix4x3fvEXT"sv);
}

STATICINLINE void multi_tex_buffer(
    group::texture_unit    texunit,
    group::texture_target  target,
    group::internal_format internalformat,
    u32                    buffer)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MultiTexBufferEXT)
        glIsBuffer(buffer);
    }
    glMultiTexBufferEXT(
        static_cast<GLenum>(texunit),
        static_cast<GLenum>(target),
        static_cast<GLenum>(internalformat),
        buffer);
    detail::error_check("MultiTexBufferEXT"sv);
}

STATICINLINE void texture_buffer(
    u32                          texture,
    group::texture_target        target,
    group::sized_internal_format internalformat,
    u32                          buffer)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TextureBufferEXT)
        glIsTexture(texture);
        glIsBuffer(buffer);
    }
    glTextureBufferEXT(
        texture,
        static_cast<GLenum>(target),
        static_cast<GLenum>(internalformat),
        buffer);
    detail::error_check("TextureBufferEXT"sv);
}

template<class span_i32>
requires(semantic::concepts::Span<span_i32>&& std::is_same_v<
         std::decay_t<typename span_i32::value_type>,
         std::decay_t<i32>>) STATICINLINE
    void get_multi_tex_parameter_iiv(
        group::texture_unit          texunit,
        group::texture_target        target,
        group::get_texture_parameter pname,
        span_i32                     params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetMultiTexParameterIivEXT)
    }
    glGetMultiTexParameterIivEXT(
        static_cast<GLenum>(texunit),
        static_cast<GLenum>(target),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetMultiTexParameterIivEXT"sv);
}

template<class span_u32>
requires(semantic::concepts::Span<span_u32>&& std::is_same_v<
         std::decay_t<typename span_u32::value_type>,
         std::decay_t<u32>>) STATICINLINE
    void get_multi_tex_parameter_iuiv(
        group::texture_unit          texunit,
        group::texture_target        target,
        group::get_texture_parameter pname,
        span_u32                     params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetMultiTexParameterIuivEXT)
    }
    glGetMultiTexParameterIuivEXT(
        static_cast<GLenum>(texunit),
        static_cast<GLenum>(target),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLuint*>(params.data()) : nullptr);
    detail::error_check("GetMultiTexParameterIuivEXT"sv);
}

template<class span_i32>
requires(semantic::concepts::Span<span_i32>&& std::is_same_v<
         std::decay_t<typename span_i32::value_type>,
         std::decay_t<i32>>) STATICINLINE
    void get_texture_parameter_iiv(
        u32                          texture,
        group::texture_target        target,
        group::get_texture_parameter pname,
        span_i32                     params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetTextureParameterIivEXT)
        glIsTexture(texture);
    }
    glGetTextureParameterIivEXT(
        texture,
        static_cast<GLenum>(target),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetTextureParameterIivEXT"sv);
}

template<class span_u32>
requires(semantic::concepts::Span<span_u32>&& std::is_same_v<
         std::decay_t<typename span_u32::value_type>,
         std::decay_t<u32>>) STATICINLINE
    void get_texture_parameter_iuiv(
        u32                          texture,
        group::texture_target        target,
        group::get_texture_parameter pname,
        span_u32                     params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetTextureParameterIuivEXT)
        glIsTexture(texture);
    }
    glGetTextureParameterIuivEXT(
        texture,
        static_cast<GLenum>(target),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLuint*>(params.data()) : nullptr);
    detail::error_check("GetTextureParameterIuivEXT"sv);
}

template<class span_const_i32>
requires(semantic::concepts::Span<span_const_i32>&& std::is_same_v<
         std::decay_t<typename span_const_i32::value_type>,
         std::decay_t<i32>>) STATICINLINE
    void multi_tex_parameter_iiv(
        group::texture_unit           texunit,
        group::texture_target         target,
        group::texture_parameter_name pname,
        span_const_i32 const&         params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MultiTexParameterIivEXT)
    }
    glMultiTexParameterIivEXT(
        static_cast<GLenum>(texunit),
        static_cast<GLenum>(target),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<const GLint*>(params.data())
                      : nullptr);
    detail::error_check("MultiTexParameterIivEXT"sv);
}

template<class span_const_u32>
requires(semantic::concepts::Span<span_const_u32>&& std::is_same_v<
         std::decay_t<typename span_const_u32::value_type>,
         std::decay_t<u32>>) STATICINLINE
    void multi_tex_parameter_iuiv(
        group::texture_unit           texunit,
        group::texture_target         target,
        group::texture_parameter_name pname,
        span_const_u32 const&         params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MultiTexParameterIuivEXT)
    }
    glMultiTexParameterIuivEXT(
        static_cast<GLenum>(texunit),
        static_cast<GLenum>(target),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<const GLuint*>(params.data())
                      : nullptr);
    detail::error_check("MultiTexParameterIuivEXT"sv);
}

template<class span_const_i32>
requires(semantic::concepts::Span<span_const_i32>&& std::is_same_v<
         std::decay_t<typename span_const_i32::value_type>,
         std::decay_t<i32>>) STATICINLINE
    void texture_parameter_iiv(
        u32                           texture,
        group::texture_target         target,
        group::texture_parameter_name pname,
        span_const_i32 const&         params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TextureParameterIivEXT)
        glIsTexture(texture);
    }
    glTextureParameterIivEXT(
        texture,
        static_cast<GLenum>(target),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<const GLint*>(params.data())
                      : nullptr);
    detail::error_check("TextureParameterIivEXT"sv);
}

template<class span_const_u32>
requires(semantic::concepts::Span<span_const_u32>&& std::is_same_v<
         std::decay_t<typename span_const_u32::value_type>,
         std::decay_t<u32>>) STATICINLINE
    void texture_parameter_iuiv(
        u32                           texture,
        group::texture_target         target,
        group::texture_parameter_name pname,
        span_const_u32 const&         params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TextureParameterIuivEXT)
        glIsTexture(texture);
    }
    glTextureParameterIuivEXT(
        texture,
        static_cast<GLenum>(target),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<const GLuint*>(params.data())
                      : nullptr);
    detail::error_check("TextureParameterIuivEXT"sv);
}

STATICINLINE void program_uniform(u32 program, i32 location, u32 v0)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform1uiEXT)
        glIsProgram(program);
    }
    glProgramUniform1uiEXT(program, location, v0);
    detail::error_check("ProgramUniform1uiEXT"sv);
}

template<class span_const_u32>
requires(semantic::concepts::Span<span_const_u32>&& std::is_same_v<
         std::decay_t<typename span_const_u32::value_type>,
         std::decay_t<u32>>) STATICINLINE
    void program_uniform(u32 program, i32 location, span_const_u32 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform1uivEXT)
        glIsProgram(program);
    }
    glProgramUniform1uivEXT(
        program,
        location,
        value.size(),
        reinterpret_cast<const GLuint*>(value.data()));
    detail::error_check("ProgramUniform1uivEXT"sv);
}

template<class vec_2_u32>
requires(semantic::concepts::Vector<vec_2_u32, u32, 2>) STATICINLINE
    void program_uniform(u32 program, i32 location, vec_2_u32 const& v0)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform2uiEXT)
        glIsProgram(program);
    }
    glProgramUniform2uiEXT(program, location, v0.x(), v0.y());
    detail::error_check("ProgramUniform2uiEXT"sv);
}

template<class span_const_vec_2_u32>
requires(semantic::concepts::Span<span_const_vec_2_u32>&& semantic::concepts::
             Vector<typename span_const_vec_2_u32::value_type, u32, 2>)
    STATICINLINE void program_uniform(
        u32 program, i32 location, i32 count, span_const_vec_2_u32 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform2uivEXT)
        glIsProgram(program);
    }
    glProgramUniform2uivEXT(
        program,
        location,
        count,
        reinterpret_cast<const GLuint*>(value.data()));
    detail::error_check("ProgramUniform2uivEXT"sv);
}

template<class vec_3_u32>
requires(semantic::concepts::Vector<vec_3_u32, u32, 3>) STATICINLINE
    void program_uniform(u32 program, i32 location, vec_3_u32 const& v0)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform3uiEXT)
        glIsProgram(program);
    }
    glProgramUniform3uiEXT(program, location, v0.x(), v0.y(), v0.z());
    detail::error_check("ProgramUniform3uiEXT"sv);
}

template<class span_const_vec_3_u32>
requires(semantic::concepts::Span<span_const_vec_3_u32>&& semantic::concepts::
             Vector<typename span_const_vec_3_u32::value_type, u32, 3>)
    STATICINLINE void program_uniform(
        u32 program, i32 location, i32 count, span_const_vec_3_u32 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform3uivEXT)
        glIsProgram(program);
    }
    glProgramUniform3uivEXT(
        program,
        location,
        count,
        reinterpret_cast<const GLuint*>(value.data()));
    detail::error_check("ProgramUniform3uivEXT"sv);
}

template<class vec_4_u32>
requires(semantic::concepts::Vector<vec_4_u32, u32, 4>) STATICINLINE
    void program_uniform(u32 program, i32 location, vec_4_u32 const& v0)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform4uiEXT)
        glIsProgram(program);
    }
    glProgramUniform4uiEXT(program, location, v0.x(), v0.y(), v0.z(), v0.w());
    detail::error_check("ProgramUniform4uiEXT"sv);
}

template<class span_const_vec_4_u32>
requires(semantic::concepts::Span<span_const_vec_4_u32>&& semantic::concepts::
             Vector<typename span_const_vec_4_u32::value_type, u32, 4>)
    STATICINLINE void program_uniform(
        u32 program, i32 location, i32 count, span_const_vec_4_u32 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform4uivEXT)
        glIsProgram(program);
    }
    glProgramUniform4uivEXT(
        program,
        location,
        count,
        reinterpret_cast<const GLuint*>(value.data()));
    detail::error_check("ProgramUniform4uivEXT"sv);
}

template<class span_const_vec_4_f32>
requires(semantic::concepts::Span<span_const_vec_4_f32>&& semantic::concepts::
             Vector<typename span_const_vec_4_f32::value_type, f32, 4>)
    STATICINLINE void named_program_local_parameters4fv(
        u32                         program,
        group::program_target       target,
        u32                         index,
        i32                         count,
        span_const_vec_4_f32 const& params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(NamedProgramLocalParameters4fvEXT)
        glIsProgram(program);
    }
    glNamedProgramLocalParameters4fvEXT(
        program,
        static_cast<GLenum>(target),
        index,
        count,
        reinterpret_cast<const GLfloat*>(params.data()));
    detail::error_check("NamedProgramLocalParameters4fvEXT"sv);
}

template<class span_i32>
requires(semantic::concepts::Span<span_i32>&& std::is_same_v<
         std::decay_t<typename span_i32::value_type>,
         std::decay_t<i32>>) STATICINLINE
    void get_named_program_local_parameter_iiv(
        u32 program, group::program_target target, u32 index, span_i32 params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetNamedProgramLocalParameterIivEXT)
        glIsProgram(program);
    }
    glGetNamedProgramLocalParameterIivEXT(
        program,
        static_cast<GLenum>(target),
        index,
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetNamedProgramLocalParameterIivEXT"sv);
}

template<class span_u32>
requires(semantic::concepts::Span<span_u32>&& std::is_same_v<
         std::decay_t<typename span_u32::value_type>,
         std::decay_t<u32>>) STATICINLINE
    void get_named_program_local_parameter_iuiv(
        u32 program, group::program_target target, u32 index, span_u32 params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetNamedProgramLocalParameterIuivEXT)
        glIsProgram(program);
    }
    glGetNamedProgramLocalParameterIuivEXT(
        program,
        static_cast<GLenum>(target),
        index,
        params.size() ? reinterpret_cast<GLuint*>(params.data()) : nullptr);
    detail::error_check("GetNamedProgramLocalParameterIuivEXT"sv);
}

template<class vec_4_i32>
requires(semantic::concepts::Vector<vec_4_i32, i32, 4>) STATICINLINE
    void named_program_local_parameter_i4i(
        u32                   program,
        group::program_target target,
        u32                   index,
        vec_4_i32 const&      x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(NamedProgramLocalParameterI4iEXT)
        glIsProgram(program);
    }
    glNamedProgramLocalParameterI4iEXT(
        program,
        static_cast<GLenum>(target),
        index,
        x.x(),
        x.y(),
        x.z(),
        x.w());
    detail::error_check("NamedProgramLocalParameterI4iEXT"sv);
}

template<class span_const_vec_4_i32>
requires(semantic::concepts::Span<span_const_vec_4_i32>&& semantic::concepts::
             Vector<typename span_const_vec_4_i32::value_type, i32, 4>)
    STATICINLINE void named_program_local_parameter_i4iv(
        u32                         program,
        group::program_target       target,
        u32                         index,
        span_const_vec_4_i32 const& params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(NamedProgramLocalParameterI4ivEXT)
        glIsProgram(program);
    }
    glNamedProgramLocalParameterI4ivEXT(
        program,
        static_cast<GLenum>(target),
        index,
        reinterpret_cast<const GLint*>(params.data()));
    detail::error_check("NamedProgramLocalParameterI4ivEXT"sv);
}

template<class vec_4_u32>
requires(semantic::concepts::Vector<vec_4_u32, u32, 4>) STATICINLINE
    void named_program_local_parameter_i4ui(
        u32                   program,
        group::program_target target,
        u32                   index,
        vec_4_u32 const&      x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(NamedProgramLocalParameterI4uiEXT)
        glIsProgram(program);
    }
    glNamedProgramLocalParameterI4uiEXT(
        program,
        static_cast<GLenum>(target),
        index,
        x.x(),
        x.y(),
        x.z(),
        x.w());
    detail::error_check("NamedProgramLocalParameterI4uiEXT"sv);
}

template<class span_const_vec_4_u32>
requires(semantic::concepts::Span<span_const_vec_4_u32>&& semantic::concepts::
             Vector<typename span_const_vec_4_u32::value_type, u32, 4>)
    STATICINLINE void named_program_local_parameter_i4uiv(
        u32                         program,
        group::program_target       target,
        u32                         index,
        span_const_vec_4_u32 const& params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(NamedProgramLocalParameterI4uivEXT)
        glIsProgram(program);
    }
    glNamedProgramLocalParameterI4uivEXT(
        program,
        static_cast<GLenum>(target),
        index,
        reinterpret_cast<const GLuint*>(params.data()));
    detail::error_check("NamedProgramLocalParameterI4uivEXT"sv);
}

template<class span_const_vec_4_i32>
requires(semantic::concepts::Span<span_const_vec_4_i32>&& semantic::concepts::
             Vector<typename span_const_vec_4_i32::value_type, i32, 4>)
    STATICINLINE void named_program_local_parameters_i4iv(
        u32                         program,
        group::program_target       target,
        u32                         index,
        i32                         count,
        span_const_vec_4_i32 const& params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(NamedProgramLocalParametersI4ivEXT)
        glIsProgram(program);
    }
    glNamedProgramLocalParametersI4ivEXT(
        program,
        static_cast<GLenum>(target),
        index,
        count,
        reinterpret_cast<const GLint*>(params.data()));
    detail::error_check("NamedProgramLocalParametersI4ivEXT"sv);
}

template<class span_const_vec_4_u32>
requires(semantic::concepts::Span<span_const_vec_4_u32>&& semantic::concepts::
             Vector<typename span_const_vec_4_u32::value_type, u32, 4>)
    STATICINLINE void named_program_local_parameters_i4uiv(
        u32                         program,
        group::program_target       target,
        u32                         index,
        i32                         count,
        span_const_vec_4_u32 const& params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(NamedProgramLocalParametersI4uivEXT)
        glIsProgram(program);
    }
    glNamedProgramLocalParametersI4uivEXT(
        program,
        static_cast<GLenum>(target),
        index,
        count,
        reinterpret_cast<const GLuint*>(params.data()));
    detail::error_check("NamedProgramLocalParametersI4uivEXT"sv);
}

STATICINLINE void disable_client_statei(group::enable_cap array, u32 index)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DisableClientStateiEXT)
    }
    glDisableClientStateiEXT(static_cast<GLenum>(array), index);
    detail::error_check("DisableClientStateiEXT"sv);
}

STATICINLINE void enable_client_statei(group::enable_cap array, u32 index)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(EnableClientStateiEXT)
    }
    glEnableClientStateiEXT(static_cast<GLenum>(array), index);
    detail::error_check("EnableClientStateiEXT"sv);
}

template<class span_f64>
requires(semantic::concepts::Span<span_f64>&& std::is_same_v<
         std::decay_t<typename span_f64::value_type>,
         std::decay_t<f64>>) STATICINLINE
    void get_doublei_v(group::get_prop pname, u32 index, span_f64 params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetDoublei_vEXT)
    }
    glGetDoublei_vEXT(
        static_cast<GLenum>(pname),
        index,
        params.size() ? reinterpret_cast<GLdouble*>(params.data()) : nullptr);
    detail::error_check("GetDoublei_vEXT"sv);
}

template<class span_f32>
requires(semantic::concepts::Span<span_f32>&& std::is_same_v<
         std::decay_t<typename span_f32::value_type>,
         std::decay_t<f32>>) STATICINLINE
    void get_floati_v(group::get_prop pname, u32 index, span_f32 params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetFloati_vEXT)
    }
    glGetFloati_vEXT(
        static_cast<GLenum>(pname),
        index,
        params.size() ? reinterpret_cast<GLfloat*>(params.data()) : nullptr);
    detail::error_check("GetFloati_vEXT"sv);
}

template<class span_void>
requires(semantic::concepts::Span<span_void>) STATICINLINE
    void get_pointeri_v(GLenum pname, u32 index, span_void params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetPointeri_vEXT)
    }
    glGetPointeri_vEXT(
        pname,
        index,
        params.size() ? reinterpret_cast<void**>(params.data()) : nullptr);
    detail::error_check("GetPointeri_vEXT"sv);
}

template<class span_f64>
requires(semantic::concepts::Span<span_f64>&& std::is_same_v<
         std::decay_t<typename span_f64::value_type>,
         std::decay_t<f64>>) STATICINLINE
    void get_named_program_local_parameter(
        u32 program, group::program_target target, u32 index, span_f64 params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetNamedProgramLocalParameterdvEXT)
        glIsProgram(program);
    }
    glGetNamedProgramLocalParameterdvEXT(
        program,
        static_cast<GLenum>(target),
        index,
        params.size() ? reinterpret_cast<GLdouble*>(params.data()) : nullptr);
    detail::error_check("GetNamedProgramLocalParameterdvEXT"sv);
}

template<class span_f32>
requires(semantic::concepts::Span<span_f32>&& std::is_same_v<
         std::decay_t<typename span_f32::value_type>,
         std::decay_t<f32>>) STATICINLINE
    void get_named_program_local_parameter(
        u32 program, group::program_target target, u32 index, span_f32 params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetNamedProgramLocalParameterfvEXT)
        glIsProgram(program);
    }
    glGetNamedProgramLocalParameterfvEXT(
        program,
        static_cast<GLenum>(target),
        index,
        params.size() ? reinterpret_cast<GLfloat*>(params.data()) : nullptr);
    detail::error_check("GetNamedProgramLocalParameterfvEXT"sv);
}

template<class span_void>
requires(semantic::concepts::Span<span_void>) STATICINLINE
    void get_named_program_string(
        u32                            program,
        group::program_target          target,
        group::program_string_property pname,
        span_void                      string)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetNamedProgramStringEXT)
        glIsProgram(program);
    }
    glGetNamedProgramStringEXT(
        program,
        static_cast<GLenum>(target),
        static_cast<GLenum>(pname),
        string.size() ? reinterpret_cast<void*>(string.data()) : nullptr);
    detail::error_check("GetNamedProgramStringEXT"sv);
}

STATICINLINE void get_named_programiv(
    u32                         program,
    group::program_target       target,
    group::program_property_arb pname,
    i32&                        params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetNamedProgramivEXT)
        glIsProgram(program);
    }
    glGetNamedProgramivEXT(
        program,
        static_cast<GLenum>(target),
        static_cast<GLenum>(pname),
        &params);
    detail::error_check("GetNamedProgramivEXT"sv);
}

template<class vec_4_f64>
requires(semantic::concepts::Vector<vec_4_f64, f64, 4>) STATICINLINE
    void named_program_local_parameter4d(
        u32                   program,
        group::program_target target,
        u32                   index,
        vec_4_f64 const&      x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(NamedProgramLocalParameter4dEXT)
        glIsProgram(program);
    }
    glNamedProgramLocalParameter4dEXT(
        program,
        static_cast<GLenum>(target),
        index,
        x.x(),
        x.y(),
        x.z(),
        x.w());
    detail::error_check("NamedProgramLocalParameter4dEXT"sv);
}

template<class span_const_vec_4_f64>
requires(semantic::concepts::Span<span_const_vec_4_f64>&& semantic::concepts::
             Vector<typename span_const_vec_4_f64::value_type, f64, 4>)
    STATICINLINE void named_program_local_parameter4dv(
        u32                         program,
        group::program_target       target,
        u32                         index,
        span_const_vec_4_f64 const& params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(NamedProgramLocalParameter4dvEXT)
        glIsProgram(program);
    }
    glNamedProgramLocalParameter4dvEXT(
        program,
        static_cast<GLenum>(target),
        index,
        reinterpret_cast<const GLdouble*>(params.data()));
    detail::error_check("NamedProgramLocalParameter4dvEXT"sv);
}

template<class vec_4_f32>
requires(semantic::concepts::Vector<vec_4_f32, f32, 4>) STATICINLINE
    void named_program_local_parameter4f(
        u32                   program,
        group::program_target target,
        u32                   index,
        vec_4_f32 const&      x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(NamedProgramLocalParameter4fEXT)
        glIsProgram(program);
    }
    glNamedProgramLocalParameter4fEXT(
        program,
        static_cast<GLenum>(target),
        index,
        x.x(),
        x.y(),
        x.z(),
        x.w());
    detail::error_check("NamedProgramLocalParameter4fEXT"sv);
}

template<class span_const_vec_4_f32>
requires(semantic::concepts::Span<span_const_vec_4_f32>&& semantic::concepts::
             Vector<typename span_const_vec_4_f32::value_type, f32, 4>)
    STATICINLINE void named_program_local_parameter4fv(
        u32                         program,
        group::program_target       target,
        u32                         index,
        span_const_vec_4_f32 const& params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(NamedProgramLocalParameter4fvEXT)
        glIsProgram(program);
    }
    glNamedProgramLocalParameter4fvEXT(
        program,
        static_cast<GLenum>(target),
        index,
        reinterpret_cast<const GLfloat*>(params.data()));
    detail::error_check("NamedProgramLocalParameter4fvEXT"sv);
}

template<class span_const_void>
requires(semantic::concepts::Span<span_const_void>) STATICINLINE
    void named_program_string(
        u32                    program,
        group::program_target  target,
        group::program_format  format,
        span_const_void const& string)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(NamedProgramStringEXT)
        glIsProgram(program);
    }
    glNamedProgramStringEXT(
        program,
        static_cast<GLenum>(target),
        static_cast<GLenum>(format),
        string.size() *
            sizeof(typename std::decay_t<span_const_void const&>::value_type),
        string.size() ? reinterpret_cast<const void*>(string.data()) : nullptr);
    detail::error_check("NamedProgramStringEXT"sv);
}

template<class span_i32>
requires(semantic::concepts::Span<span_i32>&& std::is_same_v<
         std::decay_t<typename span_i32::value_type>,
         std::decay_t<i32>>) STATICINLINE
    void get_named_renderbuffer_parameter(
        u32                                renderbuffer,
        group::renderbuffer_parameter_name pname,
        span_i32                           params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetNamedRenderbufferParameterivEXT)
        glIsRenderbuffer(renderbuffer);
    }
    glGetNamedRenderbufferParameterivEXT(
        renderbuffer,
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetNamedRenderbufferParameterivEXT"sv);
}

template<class size_2_i32>
requires(semantic::concepts::Size2D<size_2_i32, i32>) STATICINLINE
    void named_renderbuffer_storage(
        u32                    renderbuffer,
        group::internal_format internalformat,
        size_2_i32 const&      width)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(NamedRenderbufferStorageEXT)
        glIsRenderbuffer(renderbuffer);
    }
    glNamedRenderbufferStorageEXT(
        renderbuffer, static_cast<GLenum>(internalformat), width[0], width[1]);
    detail::error_check("NamedRenderbufferStorageEXT"sv);
}

template<class size_2_i32>
requires(semantic::concepts::Size2D<size_2_i32, i32>) STATICINLINE
    void named_renderbuffer_storage_multisample(
        u32                    renderbuffer,
        i32                    samples,
        group::internal_format internalformat,
        size_2_i32 const&      width)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(NamedRenderbufferStorageMultisampleEXT)
        glIsRenderbuffer(renderbuffer);
    }
    glNamedRenderbufferStorageMultisampleEXT(
        renderbuffer,
        samples,
        static_cast<GLenum>(internalformat),
        width[0],
        width[1]);
    detail::error_check("NamedRenderbufferStorageMultisampleEXT"sv);
}

template<class size_2_i32>
requires(semantic::concepts::Size2D<size_2_i32, i32>) STATICINLINE
    void named_renderbuffer_storage_multisample_coverage(
        u32                    renderbuffer,
        i32                    coverageSamples,
        i32                    colorSamples,
        group::internal_format internalformat,
        size_2_i32 const&      width)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(NamedRenderbufferStorageMultisampleCoverageEXT)
        glIsRenderbuffer(renderbuffer);
    }
    glNamedRenderbufferStorageMultisampleCoverageEXT(
        renderbuffer,
        coverageSamples,
        colorSamples,
        static_cast<GLenum>(internalformat),
        width[0],
        width[1]);
    detail::error_check("NamedRenderbufferStorageMultisampleCoverageEXT"sv);
}

STATICINLINE GLenum check_named_framebuffer_status(
    u32 framebuffer, group::framebuffer_target target)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CheckNamedFramebufferStatusEXT)
        glIsFramebuffer(framebuffer);
    }
    auto out = glCheckNamedFramebufferStatusEXT(
        framebuffer, static_cast<GLenum>(target));
    detail::error_check("CheckNamedFramebufferStatusEXT"sv);
    return out;
}

template<class span_i32>
requires(semantic::concepts::Span<span_i32>&& std::is_same_v<
         std::decay_t<typename span_i32::value_type>,
         std::decay_t<i32>>) STATICINLINE
    void get_named_framebuffer_attachment_parameter(
        u32                                          framebuffer,
        group::framebuffer_attachment                attachment,
        group::framebuffer_attachment_parameter_name pname,
        span_i32                                     params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetNamedFramebufferAttachmentParameterivEXT)
        glIsFramebuffer(framebuffer);
    }
    glGetNamedFramebufferAttachmentParameterivEXT(
        framebuffer,
        static_cast<GLenum>(attachment),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetNamedFramebufferAttachmentParameterivEXT"sv);
}

STATICINLINE void named_framebuffer_renderbuffer(
    u32                           framebuffer,
    group::framebuffer_attachment attachment,
    group::renderbuffer_target    renderbuffertarget,
    u32                           renderbuffer)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(NamedFramebufferRenderbufferEXT)
        glIsFramebuffer(framebuffer);
        glIsRenderbuffer(renderbuffer);
    }
    glNamedFramebufferRenderbufferEXT(
        framebuffer,
        static_cast<GLenum>(attachment),
        static_cast<GLenum>(renderbuffertarget),
        renderbuffer);
    detail::error_check("NamedFramebufferRenderbufferEXT"sv);
}

STATICINLINE void named_framebuffer_texture_1d(
    u32                           framebuffer,
    group::framebuffer_attachment attachment,
    group::texture_target         textarget,
    u32                           texture,
    i32                           level)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(NamedFramebufferTexture1DEXT)
        glIsFramebuffer(framebuffer);
        glIsTexture(texture);
    }
    glNamedFramebufferTexture1DEXT(
        framebuffer,
        static_cast<GLenum>(attachment),
        static_cast<GLenum>(textarget),
        texture,
        level);
    detail::error_check("NamedFramebufferTexture1DEXT"sv);
}

STATICINLINE void named_framebuffer_texture_2d(
    u32                           framebuffer,
    group::framebuffer_attachment attachment,
    group::texture_target         textarget,
    u32                           texture,
    i32                           level)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(NamedFramebufferTexture2DEXT)
        glIsFramebuffer(framebuffer);
        glIsTexture(texture);
    }
    glNamedFramebufferTexture2DEXT(
        framebuffer,
        static_cast<GLenum>(attachment),
        static_cast<GLenum>(textarget),
        texture,
        level);
    detail::error_check("NamedFramebufferTexture2DEXT"sv);
}

STATICINLINE void named_framebuffer_texture_3d(
    u32                           framebuffer,
    group::framebuffer_attachment attachment,
    group::texture_target         textarget,
    u32                           texture,
    i32                           level,
    i32                           zoffset)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(NamedFramebufferTexture3DEXT)
        glIsFramebuffer(framebuffer);
        glIsTexture(texture);
    }
    glNamedFramebufferTexture3DEXT(
        framebuffer,
        static_cast<GLenum>(attachment),
        static_cast<GLenum>(textarget),
        texture,
        level,
        zoffset);
    detail::error_check("NamedFramebufferTexture3DEXT"sv);
}

STATICINLINE void generate_multi_tex_mipmap(
    group::texture_unit texunit, group::texture_target target)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GenerateMultiTexMipmapEXT)
    }
    glGenerateMultiTexMipmapEXT(
        static_cast<GLenum>(texunit), static_cast<GLenum>(target));
    detail::error_check("GenerateMultiTexMipmapEXT"sv);
}

STATICINLINE void generate_texture_mipmap(
    u32 texture, group::texture_target target)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GenerateTextureMipmapEXT)
        glIsTexture(texture);
    }
    glGenerateTextureMipmapEXT(texture, static_cast<GLenum>(target));
    detail::error_check("GenerateTextureMipmapEXT"sv);
}

STATICINLINE void framebuffer_draw_buffer(
    u32 framebuffer, group::draw_buffer_mode mode)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(FramebufferDrawBufferEXT)
        glIsFramebuffer(framebuffer);
    }
    glFramebufferDrawBufferEXT(framebuffer, static_cast<GLenum>(mode));
    detail::error_check("FramebufferDrawBufferEXT"sv);
}

template<class span_const_draw_buffer_mode>
requires(semantic::concepts::Span<span_const_draw_buffer_mode>&& std::is_same_v<
         std::decay_t<typename span_const_draw_buffer_mode::value_type>,
         std::decay_t<group::draw_buffer_mode>>) STATICINLINE
    void framebuffer_draw_buffers(
        u32 framebuffer, span_const_draw_buffer_mode const& bufs)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(FramebufferDrawBuffersEXT)
        glIsFramebuffer(framebuffer);
    }
    glFramebufferDrawBuffersEXT(
        framebuffer,
        bufs.size(),
        bufs.size() ? reinterpret_cast<const GLenum*>(bufs.data()) : nullptr);
    detail::error_check("FramebufferDrawBuffersEXT"sv);
}

STATICINLINE void framebuffer_read_buffer(
    u32 framebuffer, group::read_buffer_mode mode)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(FramebufferReadBufferEXT)
        glIsFramebuffer(framebuffer);
    }
    glFramebufferReadBufferEXT(framebuffer, static_cast<GLenum>(mode));
    detail::error_check("FramebufferReadBufferEXT"sv);
}

template<class span_i32>
requires(semantic::concepts::Span<span_i32>&& std::is_same_v<
         std::decay_t<typename span_i32::value_type>,
         std::decay_t<i32>>) STATICINLINE
    void get_framebuffer_parameter(
        u32                              framebuffer,
        group::get_framebuffer_parameter pname,
        span_i32                         params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetFramebufferParameterivEXT)
        glIsFramebuffer(framebuffer);
    }
    glGetFramebufferParameterivEXT(
        framebuffer,
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetFramebufferParameterivEXT"sv);
}

STATICINLINE void named_copy_buffer_sub_data(
    u32        readBuffer,
    u32        writeBuffer,
    GLintptr   readOffset,
    GLintptr   writeOffset,
    GLsizeiptr size)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(NamedCopyBufferSubDataEXT)
    }
    glNamedCopyBufferSubDataEXT(
        readBuffer, writeBuffer, readOffset, writeOffset, size);
    detail::error_check("NamedCopyBufferSubDataEXT"sv);
}

STATICINLINE void named_framebuffer_texture(
    u32                           framebuffer,
    group::framebuffer_attachment attachment,
    u32                           texture,
    i32                           level)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(NamedFramebufferTextureEXT)
        glIsFramebuffer(framebuffer);
        glIsTexture(texture);
    }
    glNamedFramebufferTextureEXT(
        framebuffer, static_cast<GLenum>(attachment), texture, level);
    detail::error_check("NamedFramebufferTextureEXT"sv);
}

STATICINLINE void named_framebuffer_texture_face(
    u32                           framebuffer,
    group::framebuffer_attachment attachment,
    u32                           texture,
    i32                           level,
    group::texture_target         face)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(NamedFramebufferTextureFaceEXT)
        glIsFramebuffer(framebuffer);
        glIsTexture(texture);
    }
    glNamedFramebufferTextureFaceEXT(
        framebuffer,
        static_cast<GLenum>(attachment),
        texture,
        level,
        static_cast<GLenum>(face));
    detail::error_check("NamedFramebufferTextureFaceEXT"sv);
}

STATICINLINE void named_framebuffer_texture_layer(
    u32                           framebuffer,
    group::framebuffer_attachment attachment,
    u32                           texture,
    i32                           level,
    i32                           layer)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(NamedFramebufferTextureLayerEXT)
        glIsFramebuffer(framebuffer);
        glIsTexture(texture);
    }
    glNamedFramebufferTextureLayerEXT(
        framebuffer, static_cast<GLenum>(attachment), texture, level, layer);
    detail::error_check("NamedFramebufferTextureLayerEXT"sv);
}

STATICINLINE void multi_tex_renderbuffer(
    group::texture_unit texunit, group::texture_target target, u32 renderbuffer)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MultiTexRenderbufferEXT)
        glIsRenderbuffer(renderbuffer);
    }
    glMultiTexRenderbufferEXT(
        static_cast<GLenum>(texunit),
        static_cast<GLenum>(target),
        renderbuffer);
    detail::error_check("MultiTexRenderbufferEXT"sv);
}

STATICINLINE void texture_renderbuffer(
    u32 texture, group::texture_target target, u32 renderbuffer)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TextureRenderbufferEXT)
        glIsTexture(texture);
        glIsRenderbuffer(renderbuffer);
    }
    glTextureRenderbufferEXT(
        texture, static_cast<GLenum>(target), renderbuffer);
    detail::error_check("TextureRenderbufferEXT"sv);
}

STATICINLINE void vertex_array_color_offset(
    u32                       vaobj,
    u32                       buffer,
    i32                       size,
    group::color_pointer_type type,
    i32                       stride,
    GLintptr                  offset)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexArrayColorOffsetEXT)
        glIsVertexArray(vaobj);
        glIsBuffer(buffer);
    }
    glVertexArrayColorOffsetEXT(
        vaobj, buffer, size, static_cast<GLenum>(type), stride, offset);
    detail::error_check("VertexArrayColorOffsetEXT"sv);
}

STATICINLINE void vertex_array_edge_flag_offset(
    u32 vaobj, u32 buffer, i32 stride, GLintptr offset)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexArrayEdgeFlagOffsetEXT)
        glIsVertexArray(vaobj);
        glIsBuffer(buffer);
    }
    glVertexArrayEdgeFlagOffsetEXT(vaobj, buffer, stride, offset);
    detail::error_check("VertexArrayEdgeFlagOffsetEXT"sv);
}

STATICINLINE void vertex_array_fog_coord_offset(
    u32                                vaobj,
    u32                                buffer,
    group::fog_coordinate_pointer_type type,
    i32                                stride,
    GLintptr                           offset)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexArrayFogCoordOffsetEXT)
        glIsVertexArray(vaobj);
        glIsBuffer(buffer);
    }
    glVertexArrayFogCoordOffsetEXT(
        vaobj, buffer, static_cast<GLenum>(type), stride, offset);
    detail::error_check("VertexArrayFogCoordOffsetEXT"sv);
}

STATICINLINE void vertex_array_index_offset(
    u32                       vaobj,
    u32                       buffer,
    group::index_pointer_type type,
    i32                       stride,
    GLintptr                  offset)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexArrayIndexOffsetEXT)
        glIsVertexArray(vaobj);
        glIsBuffer(buffer);
    }
    glVertexArrayIndexOffsetEXT(
        vaobj, buffer, static_cast<GLenum>(type), stride, offset);
    detail::error_check("VertexArrayIndexOffsetEXT"sv);
}

STATICINLINE void vertex_array_multi_tex_coord_offset(
    u32                           vaobj,
    u32                           buffer,
    GLenum                        texunit,
    i32                           size,
    group::tex_coord_pointer_type type,
    i32                           stride,
    GLintptr                      offset)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexArrayMultiTexCoordOffsetEXT)
        glIsVertexArray(vaobj);
        glIsBuffer(buffer);
    }
    glVertexArrayMultiTexCoordOffsetEXT(
        vaobj,
        buffer,
        texunit,
        size,
        static_cast<GLenum>(type),
        stride,
        offset);
    detail::error_check("VertexArrayMultiTexCoordOffsetEXT"sv);
}

STATICINLINE void vertex_array_normal_offset(
    u32                        vaobj,
    u32                        buffer,
    group::normal_pointer_type type,
    i32                        stride,
    GLintptr                   offset)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexArrayNormalOffsetEXT)
        glIsVertexArray(vaobj);
        glIsBuffer(buffer);
    }
    glVertexArrayNormalOffsetEXT(
        vaobj, buffer, static_cast<GLenum>(type), stride, offset);
    detail::error_check("VertexArrayNormalOffsetEXT"sv);
}

STATICINLINE void vertex_array_secondary_color_offset(
    u32                       vaobj,
    u32                       buffer,
    i32                       size,
    group::color_pointer_type type,
    i32                       stride,
    GLintptr                  offset)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexArraySecondaryColorOffsetEXT)
        glIsVertexArray(vaobj);
        glIsBuffer(buffer);
    }
    glVertexArraySecondaryColorOffsetEXT(
        vaobj, buffer, size, static_cast<GLenum>(type), stride, offset);
    detail::error_check("VertexArraySecondaryColorOffsetEXT"sv);
}

STATICINLINE void vertex_array_tex_coord_offset(
    u32                           vaobj,
    u32                           buffer,
    i32                           size,
    group::tex_coord_pointer_type type,
    i32                           stride,
    GLintptr                      offset)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexArrayTexCoordOffsetEXT)
        glIsVertexArray(vaobj);
        glIsBuffer(buffer);
    }
    glVertexArrayTexCoordOffsetEXT(
        vaobj, buffer, size, static_cast<GLenum>(type), stride, offset);
    detail::error_check("VertexArrayTexCoordOffsetEXT"sv);
}

STATICINLINE void vertex_array_vertex_attrib_i_offset(
    u32                       vaobj,
    u32                       buffer,
    u32                       index,
    i32                       size,
    group::vertex_attrib_type type,
    i32                       stride,
    GLintptr                  offset)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexArrayVertexAttribIOffsetEXT)
        glIsVertexArray(vaobj);
        glIsBuffer(buffer);
    }
    glVertexArrayVertexAttribIOffsetEXT(
        vaobj, buffer, index, size, static_cast<GLenum>(type), stride, offset);
    detail::error_check("VertexArrayVertexAttribIOffsetEXT"sv);
}

STATICINLINE void vertex_array_vertex_attrib_offset(
    u32                               vaobj,
    u32                               buffer,
    u32                               index,
    i32                               size,
    group::vertex_attrib_pointer_type type,
    bool                              normalized,
    i32                               stride,
    GLintptr                          offset)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexArrayVertexAttribOffsetEXT)
        glIsVertexArray(vaobj);
        glIsBuffer(buffer);
    }
    glVertexArrayVertexAttribOffsetEXT(
        vaobj,
        buffer,
        index,
        size,
        static_cast<GLenum>(type),
        normalized,
        stride,
        offset);
    detail::error_check("VertexArrayVertexAttribOffsetEXT"sv);
}

STATICINLINE void vertex_array_vertex_offset(
    u32                        vaobj,
    u32                        buffer,
    i32                        size,
    group::vertex_pointer_type type,
    i32                        stride,
    GLintptr                   offset)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexArrayVertexOffsetEXT)
        glIsVertexArray(vaobj);
        glIsBuffer(buffer);
    }
    glVertexArrayVertexOffsetEXT(
        vaobj, buffer, size, static_cast<GLenum>(type), stride, offset);
    detail::error_check("VertexArrayVertexOffsetEXT"sv);
}

STATICINLINE void disable_vertex_array(u32 vaobj, group::enable_cap array)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DisableVertexArrayEXT)
        glIsVertexArray(vaobj);
    }
    glDisableVertexArrayEXT(vaobj, static_cast<GLenum>(array));
    detail::error_check("DisableVertexArrayEXT"sv);
}

STATICINLINE void enable_vertex_array(u32 vaobj, group::enable_cap array)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(EnableVertexArrayEXT)
        glIsVertexArray(vaobj);
    }
    glEnableVertexArrayEXT(vaobj, static_cast<GLenum>(array));
    detail::error_check("EnableVertexArrayEXT"sv);
}

STATICINLINE void disable_vertex_array_attrib(u32 vaobj, u32 index)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DisableVertexArrayAttribEXT)
        glIsVertexArray(vaobj);
    }
    glDisableVertexArrayAttribEXT(vaobj, index);
    detail::error_check("DisableVertexArrayAttribEXT"sv);
}

STATICINLINE void enable_vertex_array_attrib(u32 vaobj, u32 index)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(EnableVertexArrayAttribEXT)
        glIsVertexArray(vaobj);
    }
    glEnableVertexArrayAttribEXT(vaobj, index);
    detail::error_check("EnableVertexArrayAttribEXT"sv);
}

template<class span_i32>
requires(semantic::concepts::Span<span_i32>&& std::is_same_v<
         std::decay_t<typename span_i32::value_type>,
         std::decay_t<i32>>) STATICINLINE
    void get_vertex_array_integeri_v(
        u32 vaobj, u32 index, group::vertex_array_prop pname, span_i32 param)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetVertexArrayIntegeri_vEXT)
        glIsVertexArray(vaobj);
    }
    glGetVertexArrayIntegeri_vEXT(
        vaobj,
        index,
        static_cast<GLenum>(pname),
        param.size() ? reinterpret_cast<GLint*>(param.data()) : nullptr);
    detail::error_check("GetVertexArrayIntegeri_vEXT"sv);
}

template<class span_i32>
requires(semantic::concepts::Span<span_i32>&& std::is_same_v<
         std::decay_t<typename span_i32::value_type>,
         std::decay_t<i32>>) STATICINLINE
    void get_vertex_array_integerv(
        u32 vaobj, group::vertex_array_prop pname, span_i32 param)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetVertexArrayIntegervEXT)
        glIsVertexArray(vaobj);
    }
    glGetVertexArrayIntegervEXT(
        vaobj,
        static_cast<GLenum>(pname),
        param.size() ? reinterpret_cast<GLint*>(param.data()) : nullptr);
    detail::error_check("GetVertexArrayIntegervEXT"sv);
}

template<class span_void>
requires(semantic::concepts::Span<span_void>) STATICINLINE
    void get_vertex_array_pointeri_v(
        u32 vaobj, u32 index, group::vertex_array_prop pname, span_void param)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetVertexArrayPointeri_vEXT)
        glIsVertexArray(vaobj);
    }
    glGetVertexArrayPointeri_vEXT(
        vaobj,
        index,
        static_cast<GLenum>(pname),
        param.size() ? reinterpret_cast<void**>(param.data()) : nullptr);
    detail::error_check("GetVertexArrayPointeri_vEXT"sv);
}

template<class span_void>
requires(semantic::concepts::Span<span_void>) STATICINLINE
    void get_vertex_array_pointerv(
        u32 vaobj, group::vertex_array_prop pname, span_void param)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetVertexArrayPointervEXT)
        glIsVertexArray(vaobj);
    }
    glGetVertexArrayPointervEXT(
        vaobj,
        static_cast<GLenum>(pname),
        param.size() ? reinterpret_cast<void**>(param.data()) : nullptr);
    detail::error_check("GetVertexArrayPointervEXT"sv);
}

STATICINLINE void flush_mapped_named_buffer_range(
    u32 buffer, GLintptr offset, GLsizeiptr length)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(FlushMappedNamedBufferRangeEXT)
        glIsBuffer(buffer);
    }
    glFlushMappedNamedBufferRangeEXT(buffer, offset, length);
    detail::error_check("FlushMappedNamedBufferRangeEXT"sv);
}

STATICINLINE void* map_named_buffer_range(
    u32                           buffer,
    GLintptr                      offset,
    GLsizeiptr                    length,
    group::map_buffer_access_mask access)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MapNamedBufferRangeEXT)
        glIsBuffer(buffer);
    }
    auto out = glMapNamedBufferRangeEXT(
        buffer, offset, length, static_cast<GLenum>(access));
    detail::error_check("MapNamedBufferRangeEXT"sv);
    return out;
}

template<class span_const_void>
requires(semantic::concepts::Span<span_const_void>) STATICINLINE
    void named_buffer_storage(
        u32                        buffer,
        span_const_void const&     data,
        group::buffer_storage_mask flags)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(NamedBufferStorageEXT)
        glIsBuffer(buffer);
    }
    glNamedBufferStorageEXT(
        buffer,
        data.size() *
            sizeof(typename std::decay_t<span_const_void const&>::value_type),
        data.size() ? reinterpret_cast<const void*>(data.data()) : nullptr,
        static_cast<GLenum>(flags));
    detail::error_check("NamedBufferStorageEXT"sv);
}

template<class span_const_void>
requires(semantic::concepts::Span<span_const_void>) STATICINLINE
    void clear_named_buffer_data(
        u32                          buffer,
        group::sized_internal_format internalformat,
        group::pixel_format          format,
        group::pixel_type            type,
        span_const_void const&       data)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ClearNamedBufferDataEXT)
        glIsBuffer(buffer);
    }
    glClearNamedBufferDataEXT(
        buffer,
        static_cast<GLenum>(internalformat),
        static_cast<GLenum>(format),
        static_cast<GLenum>(type),
        data.size() ? reinterpret_cast<const void*>(data.data()) : nullptr);
    detail::error_check("ClearNamedBufferDataEXT"sv);
}

template<class span_const_void>
requires(semantic::concepts::Span<span_const_void>) STATICINLINE
    void clear_named_buffer_sub_data(
        u32                          buffer,
        group::sized_internal_format internalformat,
        GLsizeiptr                   offset,
        GLsizeiptr                   size,
        group::pixel_format          format,
        group::pixel_type            type,
        span_const_void const&       data)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ClearNamedBufferSubDataEXT)
        glIsBuffer(buffer);
    }
    glClearNamedBufferSubDataEXT(
        buffer,
        static_cast<GLenum>(internalformat),
        offset,
        size,
        static_cast<GLenum>(format),
        static_cast<GLenum>(type),
        data.size() ? reinterpret_cast<const void*>(data.data()) : nullptr);
    detail::error_check("ClearNamedBufferSubDataEXT"sv);
}

template<class span_i32>
requires(semantic::concepts::Span<span_i32>&& std::is_same_v<
         std::decay_t<typename span_i32::value_type>,
         std::decay_t<i32>>) STATICINLINE
    void get_named_framebuffer_parameter(
        u32                              framebuffer,
        group::get_framebuffer_parameter pname,
        span_i32                         params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetNamedFramebufferParameterivEXT)
        glIsFramebuffer(framebuffer);
    }
    glGetNamedFramebufferParameterivEXT(
        framebuffer,
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetNamedFramebufferParameterivEXT"sv);
}

STATICINLINE void named_framebuffer_parameter(
    u32 framebuffer, group::framebuffer_parameter_name pname, i32 param)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(NamedFramebufferParameteriEXT)
        glIsFramebuffer(framebuffer);
    }
    glNamedFramebufferParameteriEXT(
        framebuffer, static_cast<GLenum>(pname), param);
    detail::error_check("NamedFramebufferParameteriEXT"sv);
}

STATICINLINE void program_uniform(u32 program, i32 location, f64 x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform1dEXT)
        glIsProgram(program);
    }
    glProgramUniform1dEXT(program, location, x);
    detail::error_check("ProgramUniform1dEXT"sv);
}

template<class span_const_f64>
requires(semantic::concepts::Span<span_const_f64>&& std::is_same_v<
         std::decay_t<typename span_const_f64::value_type>,
         std::decay_t<f64>>) STATICINLINE
    void program_uniform(u32 program, i32 location, span_const_f64 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform1dvEXT)
        glIsProgram(program);
    }
    glProgramUniform1dvEXT(
        program,
        location,
        value.size(),
        reinterpret_cast<const GLdouble*>(value.data()));
    detail::error_check("ProgramUniform1dvEXT"sv);
}

template<class vec_2_f64>
requires(semantic::concepts::Vector<vec_2_f64, f64, 2>) STATICINLINE
    void program_uniform(u32 program, i32 location, vec_2_f64 const& x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform2dEXT)
        glIsProgram(program);
    }
    glProgramUniform2dEXT(program, location, x.x(), x.y());
    detail::error_check("ProgramUniform2dEXT"sv);
}

template<class span_const_vec_2_f64>
requires(semantic::concepts::Span<span_const_vec_2_f64>&& semantic::concepts::
             Vector<typename span_const_vec_2_f64::value_type, f64, 2>)
    STATICINLINE void program_uniform(
        u32 program, i32 location, i32 count, span_const_vec_2_f64 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform2dvEXT)
        glIsProgram(program);
    }
    glProgramUniform2dvEXT(
        program,
        location,
        count,
        reinterpret_cast<const GLdouble*>(value.data()));
    detail::error_check("ProgramUniform2dvEXT"sv);
}

template<class vec_3_f64>
requires(semantic::concepts::Vector<vec_3_f64, f64, 3>) STATICINLINE
    void program_uniform(u32 program, i32 location, vec_3_f64 const& x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform3dEXT)
        glIsProgram(program);
    }
    glProgramUniform3dEXT(program, location, x.x(), x.y(), x.z());
    detail::error_check("ProgramUniform3dEXT"sv);
}

template<class span_const_vec_3_f64>
requires(semantic::concepts::Span<span_const_vec_3_f64>&& semantic::concepts::
             Vector<typename span_const_vec_3_f64::value_type, f64, 3>)
    STATICINLINE void program_uniform(
        u32 program, i32 location, i32 count, span_const_vec_3_f64 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform3dvEXT)
        glIsProgram(program);
    }
    glProgramUniform3dvEXT(
        program,
        location,
        count,
        reinterpret_cast<const GLdouble*>(value.data()));
    detail::error_check("ProgramUniform3dvEXT"sv);
}

template<class vec_4_f64>
requires(semantic::concepts::Vector<vec_4_f64, f64, 4>) STATICINLINE
    void program_uniform(u32 program, i32 location, vec_4_f64 const& x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform4dEXT)
        glIsProgram(program);
    }
    glProgramUniform4dEXT(program, location, x.x(), x.y(), x.z(), x.w());
    detail::error_check("ProgramUniform4dEXT"sv);
}

template<class span_const_vec_4_f64>
requires(semantic::concepts::Span<span_const_vec_4_f64>&& semantic::concepts::
             Vector<typename span_const_vec_4_f64::value_type, f64, 4>)
    STATICINLINE void program_uniform(
        u32 program, i32 location, i32 count, span_const_vec_4_f64 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform4dvEXT)
        glIsProgram(program);
    }
    glProgramUniform4dvEXT(
        program,
        location,
        count,
        reinterpret_cast<const GLdouble*>(value.data()));
    detail::error_check("ProgramUniform4dvEXT"sv);
}

template<class span_const_mat_2x2_f64>
requires(semantic::concepts::Span<span_const_mat_2x2_f64>&& semantic::concepts::
             Matrix<typename span_const_mat_2x2_f64::value_type, f64, 2, 2>)
    STATICINLINE void program_uniform(
        u32                           program,
        i32                           location,
        i32                           count,
        bool                          transpose,
        span_const_mat_2x2_f64 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniformMatrix2dvEXT)
        glIsProgram(program);
    }
    glProgramUniformMatrix2dvEXT(
        program,
        location,
        count,
        transpose,
        reinterpret_cast<const GLdouble*>(value.data()));
    detail::error_check("ProgramUniformMatrix2dvEXT"sv);
}

template<class span_const_mat_2x3_f64>
requires(semantic::concepts::Span<span_const_mat_2x3_f64>&& semantic::concepts::
             Matrix<typename span_const_mat_2x3_f64::value_type, f64, 2, 3>)
    STATICINLINE void program_uniform(
        u32                           program,
        i32                           location,
        i32                           count,
        bool                          transpose,
        span_const_mat_2x3_f64 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniformMatrix2x3dvEXT)
        glIsProgram(program);
    }
    glProgramUniformMatrix2x3dvEXT(
        program,
        location,
        count,
        transpose,
        reinterpret_cast<const GLdouble*>(value.data()));
    detail::error_check("ProgramUniformMatrix2x3dvEXT"sv);
}

template<class span_const_mat_2x4_f64>
requires(semantic::concepts::Span<span_const_mat_2x4_f64>&& semantic::concepts::
             Matrix<typename span_const_mat_2x4_f64::value_type, f64, 2, 4>)
    STATICINLINE void program_uniform(
        u32                           program,
        i32                           location,
        i32                           count,
        bool                          transpose,
        span_const_mat_2x4_f64 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniformMatrix2x4dvEXT)
        glIsProgram(program);
    }
    glProgramUniformMatrix2x4dvEXT(
        program,
        location,
        count,
        transpose,
        reinterpret_cast<const GLdouble*>(value.data()));
    detail::error_check("ProgramUniformMatrix2x4dvEXT"sv);
}

template<class span_const_mat_3x3_f64>
requires(semantic::concepts::Span<span_const_mat_3x3_f64>&& semantic::concepts::
             Matrix<typename span_const_mat_3x3_f64::value_type, f64, 3, 3>)
    STATICINLINE void program_uniform(
        u32                           program,
        i32                           location,
        i32                           count,
        bool                          transpose,
        span_const_mat_3x3_f64 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniformMatrix3dvEXT)
        glIsProgram(program);
    }
    glProgramUniformMatrix3dvEXT(
        program,
        location,
        count,
        transpose,
        reinterpret_cast<const GLdouble*>(value.data()));
    detail::error_check("ProgramUniformMatrix3dvEXT"sv);
}

template<class span_const_mat_3x2_f64>
requires(semantic::concepts::Span<span_const_mat_3x2_f64>&& semantic::concepts::
             Matrix<typename span_const_mat_3x2_f64::value_type, f64, 3, 2>)
    STATICINLINE void program_uniform(
        u32                           program,
        i32                           location,
        i32                           count,
        bool                          transpose,
        span_const_mat_3x2_f64 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniformMatrix3x2dvEXT)
        glIsProgram(program);
    }
    glProgramUniformMatrix3x2dvEXT(
        program,
        location,
        count,
        transpose,
        reinterpret_cast<const GLdouble*>(value.data()));
    detail::error_check("ProgramUniformMatrix3x2dvEXT"sv);
}

template<class span_const_mat_3x4_f64>
requires(semantic::concepts::Span<span_const_mat_3x4_f64>&& semantic::concepts::
             Matrix<typename span_const_mat_3x4_f64::value_type, f64, 3, 4>)
    STATICINLINE void program_uniform(
        u32                           program,
        i32                           location,
        i32                           count,
        bool                          transpose,
        span_const_mat_3x4_f64 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniformMatrix3x4dvEXT)
        glIsProgram(program);
    }
    glProgramUniformMatrix3x4dvEXT(
        program,
        location,
        count,
        transpose,
        reinterpret_cast<const GLdouble*>(value.data()));
    detail::error_check("ProgramUniformMatrix3x4dvEXT"sv);
}

template<class span_const_mat_4x4_f64>
requires(semantic::concepts::Span<span_const_mat_4x4_f64>&& semantic::concepts::
             Matrix<typename span_const_mat_4x4_f64::value_type, f64, 4, 4>)
    STATICINLINE void program_uniform(
        u32                           program,
        i32                           location,
        i32                           count,
        bool                          transpose,
        span_const_mat_4x4_f64 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniformMatrix4dvEXT)
        glIsProgram(program);
    }
    glProgramUniformMatrix4dvEXT(
        program,
        location,
        count,
        transpose,
        reinterpret_cast<const GLdouble*>(value.data()));
    detail::error_check("ProgramUniformMatrix4dvEXT"sv);
}

template<class span_const_mat_4x2_f64>
requires(semantic::concepts::Span<span_const_mat_4x2_f64>&& semantic::concepts::
             Matrix<typename span_const_mat_4x2_f64::value_type, f64, 4, 2>)
    STATICINLINE void program_uniform(
        u32                           program,
        i32                           location,
        i32                           count,
        bool                          transpose,
        span_const_mat_4x2_f64 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniformMatrix4x2dvEXT)
        glIsProgram(program);
    }
    glProgramUniformMatrix4x2dvEXT(
        program,
        location,
        count,
        transpose,
        reinterpret_cast<const GLdouble*>(value.data()));
    detail::error_check("ProgramUniformMatrix4x2dvEXT"sv);
}

template<class span_const_mat_4x3_f64>
requires(semantic::concepts::Span<span_const_mat_4x3_f64>&& semantic::concepts::
             Matrix<typename span_const_mat_4x3_f64::value_type, f64, 4, 3>)
    STATICINLINE void program_uniform(
        u32                           program,
        i32                           location,
        i32                           count,
        bool                          transpose,
        span_const_mat_4x3_f64 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniformMatrix4x3dvEXT)
        glIsProgram(program);
    }
    glProgramUniformMatrix4x3dvEXT(
        program,
        location,
        count,
        transpose,
        reinterpret_cast<const GLdouble*>(value.data()));
    detail::error_check("ProgramUniformMatrix4x3dvEXT"sv);
}

STATICINLINE void texture_buffer_range(
    u32                          texture,
    group::texture_target        target,
    group::sized_internal_format internalformat,
    u32                          buffer,
    GLintptr                     offset,
    GLsizeiptr                   size)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TextureBufferRangeEXT)
        glIsTexture(texture);
        glIsBuffer(buffer);
    }
    glTextureBufferRangeEXT(
        texture,
        static_cast<GLenum>(target),
        static_cast<GLenum>(internalformat),
        buffer,
        offset,
        size);
    detail::error_check("TextureBufferRangeEXT"sv);
}

STATICINLINE void texture_storage_1d(
    u32                          texture,
    GLenum                       target,
    i32                          levels,
    group::sized_internal_format internalformat,
    i32                          width)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TextureStorage1DEXT)
        glIsTexture(texture);
    }
    glTextureStorage1DEXT(
        texture, target, levels, static_cast<GLenum>(internalformat), width);
    detail::error_check("TextureStorage1DEXT"sv);
}

template<class size_2_i32>
requires(semantic::concepts::Size2D<size_2_i32, i32>) STATICINLINE
    void texture_storage_2d(
        u32                          texture,
        GLenum                       target,
        i32                          levels,
        group::sized_internal_format internalformat,
        size_2_i32 const&            width)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TextureStorage2DEXT)
        glIsTexture(texture);
    }
    glTextureStorage2DEXT(
        texture,
        target,
        levels,
        static_cast<GLenum>(internalformat),
        width[0],
        width[1]);
    detail::error_check("TextureStorage2DEXT"sv);
}

template<class size_3_i32>
requires(semantic::concepts::Size2D<size_3_i32, i32>) STATICINLINE
    void texture_storage_3d(
        u32                          texture,
        GLenum                       target,
        i32                          levels,
        group::sized_internal_format internalformat,
        size_3_i32 const&            width)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TextureStorage3DEXT)
        glIsTexture(texture);
    }
    glTextureStorage3DEXT(
        texture,
        target,
        levels,
        static_cast<GLenum>(internalformat),
        width[0],
        width[1],
        width[2]);
    detail::error_check("TextureStorage3DEXT"sv);
}

template<class size_2_i32>
requires(semantic::concepts::Size2D<size_2_i32, i32>) STATICINLINE
    void texture_storage_2d_multisample(
        u32                          texture,
        group::texture_target        target,
        i32                          samples,
        group::sized_internal_format internalformat,
        size_2_i32 const&            width,
        bool                         fixedsamplelocations)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TextureStorage2DMultisampleEXT)
        glIsTexture(texture);
    }
    glTextureStorage2DMultisampleEXT(
        texture,
        static_cast<GLenum>(target),
        samples,
        static_cast<GLenum>(internalformat),
        width[0],
        width[1],
        fixedsamplelocations);
    detail::error_check("TextureStorage2DMultisampleEXT"sv);
}

template<class size_3_i32>
requires(semantic::concepts::Size2D<size_3_i32, i32>) STATICINLINE
    void texture_storage_3d_multisample(
        u32                          texture,
        GLenum                       target,
        i32                          samples,
        group::sized_internal_format internalformat,
        size_3_i32 const&            width,
        bool                         fixedsamplelocations)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TextureStorage3DMultisampleEXT)
        glIsTexture(texture);
    }
    glTextureStorage3DMultisampleEXT(
        texture,
        target,
        samples,
        static_cast<GLenum>(internalformat),
        width[0],
        width[1],
        width[2],
        fixedsamplelocations);
    detail::error_check("TextureStorage3DMultisampleEXT"sv);
}

STATICINLINE void vertex_array_bind_vertex_buffer(
    u32 vaobj, u32 bindingindex, u32 buffer, GLintptr offset, i32 stride)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexArrayBindVertexBufferEXT)
        glIsVertexArray(vaobj);
        glIsBuffer(buffer);
    }
    glVertexArrayBindVertexBufferEXT(
        vaobj, bindingindex, buffer, offset, stride);
    detail::error_check("VertexArrayBindVertexBufferEXT"sv);
}

STATICINLINE void vertex_array_vertex_attrib_binding(
    u32 vaobj, u32 attribindex, u32 bindingindex)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexArrayVertexAttribBindingEXT)
        glIsVertexArray(vaobj);
    }
    glVertexArrayVertexAttribBindingEXT(vaobj, attribindex, bindingindex);
    detail::error_check("VertexArrayVertexAttribBindingEXT"sv);
}

STATICINLINE void vertex_array_vertex_attrib_format(
    u32                       vaobj,
    u32                       attribindex,
    i32                       size,
    group::vertex_attrib_type type,
    bool                      normalized,
    u32                       relativeoffset)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexArrayVertexAttribFormatEXT)
        glIsVertexArray(vaobj);
    }
    glVertexArrayVertexAttribFormatEXT(
        vaobj,
        attribindex,
        size,
        static_cast<GLenum>(type),
        normalized,
        relativeoffset);
    detail::error_check("VertexArrayVertexAttribFormatEXT"sv);
}

STATICINLINE void vertex_array_vertex_attrib_i_format(
    u32                      vaobj,
    u32                      attribindex,
    i32                      size,
    group::vertex_attrib_int type,
    u32                      relativeoffset)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexArrayVertexAttribIFormatEXT)
        glIsVertexArray(vaobj);
    }
    glVertexArrayVertexAttribIFormatEXT(
        vaobj, attribindex, size, static_cast<GLenum>(type), relativeoffset);
    detail::error_check("VertexArrayVertexAttribIFormatEXT"sv);
}

STATICINLINE void vertex_array_vertex_attrib_l_format(
    u32                       vaobj,
    u32                       attribindex,
    i32                       size,
    group::vertex_attrib_long type,
    u32                       relativeoffset)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexArrayVertexAttribLFormatEXT)
        glIsVertexArray(vaobj);
    }
    glVertexArrayVertexAttribLFormatEXT(
        vaobj, attribindex, size, static_cast<GLenum>(type), relativeoffset);
    detail::error_check("VertexArrayVertexAttribLFormatEXT"sv);
}

STATICINLINE void vertex_array_vertex_binding_divisor(
    u32 vaobj, u32 bindingindex, u32 divisor)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexArrayVertexBindingDivisorEXT)
        glIsVertexArray(vaobj);
    }
    glVertexArrayVertexBindingDivisorEXT(vaobj, bindingindex, divisor);
    detail::error_check("VertexArrayVertexBindingDivisorEXT"sv);
}

STATICINLINE void vertex_array_vertex_attrib_l_offset(
    u32                       vaobj,
    u32                       buffer,
    u32                       index,
    i32                       size,
    group::vertex_attrib_long type,
    i32                       stride,
    GLintptr                  offset)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexArrayVertexAttribLOffsetEXT)
        glIsVertexArray(vaobj);
        glIsBuffer(buffer);
    }
    glVertexArrayVertexAttribLOffsetEXT(
        vaobj, buffer, index, size, static_cast<GLenum>(type), stride, offset);
    detail::error_check("VertexArrayVertexAttribLOffsetEXT"sv);
}

template<class size_3_i32, class vec_3_i32>
requires(semantic::concepts::Vector<vec_3_i32, i32, 3>&&
             semantic::concepts::Size2D<size_3_i32, i32>) STATICINLINE
    void texture_page_commitment(
        u32               texture,
        i32               level,
        vec_3_i32 const&  xoffset,
        size_3_i32 const& width,
        bool              commit)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TexturePageCommitmentEXT)
        glIsTexture(texture);
    }
    glTexturePageCommitmentEXT(
        texture,
        level,
        xoffset.x(),
        xoffset.y(),
        xoffset.z(),
        width[0],
        width[1],
        width[2],
        commit);
    detail::error_check("TexturePageCommitmentEXT"sv);
}

STATICINLINE void vertex_array_vertex_attrib_divisor(
    u32 vaobj, u32 index, u32 divisor)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexArrayVertexAttribDivisorEXT)
        glIsVertexArray(vaobj);
    }
    glVertexArrayVertexAttribDivisorEXT(vaobj, index, divisor);
    detail::error_check("VertexArrayVertexAttribDivisorEXT"sv);
}

} // namespace gl::ext::direct_state_access
#endif // GL_EXT_direct_state_access
namespace gl::ext::direct_state_access {
constexpr auto name = "GL_EXT_direct_state_access";
}
