#pragma once

#ifdef GL_EXT_direct_state_access
namespace gl::ext::direct_state_access {
namespace values {
constexpr u32 program_matrix             = 0x8E2D;
constexpr u32 transpose_program_matrix   = 0x8E2E;
constexpr u32 program_matrix_stack_depth = 0x8E2F;
} // namespace values

/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param mode GLenum
 * \param left GLdouble
 * \param right GLdouble
 * \param bottom GLdouble
 * \param top GLdouble
 * \param zNear GLdouble
 * \param zFar GLdouble
 * \return void
 */
STATICINLINE void matrix_frustum(
    group::matrix_mode mode,
    f64                left,
    f64                right,
    f64                bottom,
    f64                top,
    f64                zNear,
    f64                zFar,
    error_check        check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MatrixFrustumEXT)
    }
    glMatrixFrustumEXT(
        static_cast<GLenum>(mode), left, right, bottom, top, zNear, zFar);
    detail::error_check("MatrixFrustumEXT"sv, check_errors);
}

/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param mode GLenum
 * \return void
 */
STATICINLINE void matrix_load_identity(
    group::matrix_mode mode, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MatrixLoadIdentityEXT)
    }
    glMatrixLoadIdentityEXT(static_cast<GLenum>(mode));
    detail::error_check("MatrixLoadIdentityEXT"sv, check_errors);
}

template<class span_const_f64>
requires(
    concepts::span<span_const_f64> &&
    std::is_same_v<
        std::decay_t<typename span_const_f64::value_type>,
        std::decay_t<f64>>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param mode GLenum
 * \param m const GLdouble *
 * \return void
 */
STATICINLINE void matrix_loadd(
    group::matrix_mode    mode,
    span_const_f64 const& m,
    error_check           check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MatrixLoaddEXT)
    }
    glMatrixLoaddEXT(
        static_cast<GLenum>(mode),
        m.size() ? reinterpret_cast<const GLdouble*>(m.data()) : nullptr);
    detail::error_check("MatrixLoaddEXT"sv, check_errors);
}

template<class span_const_f32>
requires(
    concepts::span<span_const_f32> &&
    std::is_same_v<
        std::decay_t<typename span_const_f32::value_type>,
        std::decay_t<f32>>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param mode GLenum
 * \param m const GLfloat *
 * \return void
 */
STATICINLINE void matrix_loadf(
    group::matrix_mode    mode,
    span_const_f32 const& m,
    error_check           check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MatrixLoadfEXT)
    }
    glMatrixLoadfEXT(
        static_cast<GLenum>(mode),
        m.size() ? reinterpret_cast<const GLfloat*>(m.data()) : nullptr);
    detail::error_check("MatrixLoadfEXT"sv, check_errors);
}

template<class span_const_f64>
requires(
    concepts::span<span_const_f64> &&
    std::is_same_v<
        std::decay_t<typename span_const_f64::value_type>,
        std::decay_t<f64>>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param mode GLenum
 * \param m const GLdouble *
 * \return void
 */
STATICINLINE void matrix_multd(
    group::matrix_mode    mode,
    span_const_f64 const& m,
    error_check           check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MatrixMultdEXT)
    }
    glMatrixMultdEXT(
        static_cast<GLenum>(mode),
        m.size() ? reinterpret_cast<const GLdouble*>(m.data()) : nullptr);
    detail::error_check("MatrixMultdEXT"sv, check_errors);
}

template<class span_const_f32>
requires(
    concepts::span<span_const_f32> &&
    std::is_same_v<
        std::decay_t<typename span_const_f32::value_type>,
        std::decay_t<f32>>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param mode GLenum
 * \param m const GLfloat *
 * \return void
 */
STATICINLINE void matrix_multf(
    group::matrix_mode    mode,
    span_const_f32 const& m,
    error_check           check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MatrixMultfEXT)
    }
    glMatrixMultfEXT(
        static_cast<GLenum>(mode),
        m.size() ? reinterpret_cast<const GLfloat*>(m.data()) : nullptr);
    detail::error_check("MatrixMultfEXT"sv, check_errors);
}

/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param mode GLenum
 * \param left GLdouble
 * \param right GLdouble
 * \param bottom GLdouble
 * \param top GLdouble
 * \param zNear GLdouble
 * \param zFar GLdouble
 * \return void
 */
STATICINLINE void matrix_ortho(
    group::matrix_mode mode,
    f64                left,
    f64                right,
    f64                bottom,
    f64                top,
    f64                zNear,
    f64                zFar,
    error_check        check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MatrixOrthoEXT)
    }
    glMatrixOrthoEXT(
        static_cast<GLenum>(mode), left, right, bottom, top, zNear, zFar);
    detail::error_check("MatrixOrthoEXT"sv, check_errors);
}

/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param mode GLenum
 * \return void
 */
STATICINLINE void matrix_pop(
    group::matrix_mode mode, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MatrixPopEXT)
    }
    glMatrixPopEXT(static_cast<GLenum>(mode));
    detail::error_check("MatrixPopEXT"sv, check_errors);
}

/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param mode GLenum
 * \return void
 */
STATICINLINE void matrix_push(
    group::matrix_mode mode, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MatrixPushEXT)
    }
    glMatrixPushEXT(static_cast<GLenum>(mode));
    detail::error_check("MatrixPushEXT"sv, check_errors);
}

template<class vec_3_f64>
requires(concepts::vector<vec_3_f64, f64, 3>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param mode GLenum
 * \param angle GLdouble
 * \param x GLdouble
 * \param y GLdouble
 * \param z GLdouble
 * \return void
 */
STATICINLINE void matrix_rotated(
    group::matrix_mode mode,
    f64                angle,
    vec_3_f64 const&   x,
    error_check        check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MatrixRotatedEXT)
    }
    glMatrixRotatedEXT(static_cast<GLenum>(mode), angle, x[0], x[1], x[2]);
    detail::error_check("MatrixRotatedEXT"sv, check_errors);
}

template<class vec_3_f32>
requires(concepts::vector<vec_3_f32, f32, 3>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param mode GLenum
 * \param angle GLfloat
 * \param x GLfloat
 * \param y GLfloat
 * \param z GLfloat
 * \return void
 */
STATICINLINE void matrix_rotatef(
    group::matrix_mode mode,
    f32                angle,
    vec_3_f32 const&   x,
    error_check        check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MatrixRotatefEXT)
    }
    glMatrixRotatefEXT(static_cast<GLenum>(mode), angle, x[0], x[1], x[2]);
    detail::error_check("MatrixRotatefEXT"sv, check_errors);
}

template<class vec_3_f64>
requires(concepts::vector<vec_3_f64, f64, 3>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param mode GLenum
 * \param x GLdouble
 * \param y GLdouble
 * \param z GLdouble
 * \return void
 */
STATICINLINE void matrix_scaled(
    group::matrix_mode mode,
    vec_3_f64 const&   x,
    error_check        check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MatrixScaledEXT)
    }
    glMatrixScaledEXT(static_cast<GLenum>(mode), x[0], x[1], x[2]);
    detail::error_check("MatrixScaledEXT"sv, check_errors);
}

template<class vec_3_f32>
requires(concepts::vector<vec_3_f32, f32, 3>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param mode GLenum
 * \param x GLfloat
 * \param y GLfloat
 * \param z GLfloat
 * \return void
 */
STATICINLINE void matrix_scalef(
    group::matrix_mode mode,
    vec_3_f32 const&   x,
    error_check        check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MatrixScalefEXT)
    }
    glMatrixScalefEXT(static_cast<GLenum>(mode), x[0], x[1], x[2]);
    detail::error_check("MatrixScalefEXT"sv, check_errors);
}

template<class vec_3_f64>
requires(concepts::vector<vec_3_f64, f64, 3>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param mode GLenum
 * \param x GLdouble
 * \param y GLdouble
 * \param z GLdouble
 * \return void
 */
STATICINLINE void matrix_translated(
    group::matrix_mode mode,
    vec_3_f64 const&   x,
    error_check        check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MatrixTranslatedEXT)
    }
    glMatrixTranslatedEXT(static_cast<GLenum>(mode), x[0], x[1], x[2]);
    detail::error_check("MatrixTranslatedEXT"sv, check_errors);
}

template<class vec_3_f32>
requires(concepts::vector<vec_3_f32, f32, 3>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param mode GLenum
 * \param x GLfloat
 * \param y GLfloat
 * \param z GLfloat
 * \return void
 */
STATICINLINE void matrix_translatef(
    group::matrix_mode mode,
    vec_3_f32 const&   x,
    error_check        check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MatrixTranslatefEXT)
    }
    glMatrixTranslatefEXT(static_cast<GLenum>(mode), x[0], x[1], x[2]);
    detail::error_check("MatrixTranslatefEXT"sv, check_errors);
}

/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param mask GLbitfield
 * \return void
 */
STATICINLINE void client_attrib_default(
    group::client_attrib_mask mask, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ClientAttribDefaultEXT)
    }
    glClientAttribDefaultEXT(static_cast<GLenum>(mask));
    detail::error_check("ClientAttribDefaultEXT"sv, check_errors);
}

/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param mask GLbitfield
 * \return void
 */
STATICINLINE void push_client_attrib_default(
    group::client_attrib_mask mask, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(PushClientAttribDefaultEXT)
    }
    glPushClientAttribDefaultEXT(static_cast<GLenum>(mask));
    detail::error_check("PushClientAttribDefaultEXT"sv, check_errors);
}

template<class vec_2_i32>
requires(concepts::vector<vec_2_i32, i32, 2>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param texture GLuint
 * \param target GLenum
 * \param level GLint
 * \param internalformat GLenum
 * \param x GLint
 * \param y GLint
 * \param width GLsizei
 * \param border GLint
 * \return void
 */
STATICINLINE void copy_texture_image_1d(
    u32                    texture,
    group::texture_target  target,
    i32                    level,
    group::internal_format internalformat,
    vec_2_i32 const&       x,
    i32                    width,
    i32                    border,
    error_check            check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CopyTextureImage1DEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsTexture)
            glIsTexture(texture);
#endif
    }
    glCopyTextureImage1DEXT(
        texture,
        static_cast<GLenum>(target),
        level,
        static_cast<GLenum>(internalformat),
        x[0],
        x[1],
        width,
        border);
    detail::error_check("CopyTextureImage1DEXT"sv, check_errors);
}

template<class size_2_i32, class vec_2_i32>
requires(
    concepts::vector<vec_2_i32, i32, 2> && concepts::size_2d<size_2_i32, i32>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param texture GLuint
 * \param target GLenum
 * \param level GLint
 * \param internalformat GLenum
 * \param x GLint
 * \param y GLint
 * \param width GLsizei
 * \param height GLsizei
 * \param border GLint
 * \return void
 */
STATICINLINE void copy_texture_image_2d(
    u32                    texture,
    group::texture_target  target,
    i32                    level,
    group::internal_format internalformat,
    vec_2_i32 const&       x,
    size_2_i32 const&      width,
    i32                    border,
    error_check            check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CopyTextureImage2DEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsTexture)
            glIsTexture(texture);
#endif
    }
    glCopyTextureImage2DEXT(
        texture,
        static_cast<GLenum>(target),
        level,
        static_cast<GLenum>(internalformat),
        x[0],
        x[1],
        width[0],
        width[1],
        border);
    detail::error_check("CopyTextureImage2DEXT"sv, check_errors);
}

template<class vec_2_i32>
requires(concepts::vector<vec_2_i32, i32, 2>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param texture GLuint
 * \param target GLenum
 * \param level GLint
 * \param xoffset GLint
 * \param x GLint
 * \param y GLint
 * \param width GLsizei
 * \return void
 */
STATICINLINE void copy_texture_sub_image_1d(
    u32                   texture,
    group::texture_target target,
    i32                   level,
    i32                   xoffset,
    vec_2_i32 const&      x,
    i32                   width,
    error_check           check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CopyTextureSubImage1DEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsTexture)
            glIsTexture(texture);
#endif
    }
    glCopyTextureSubImage1DEXT(
        texture,
        static_cast<GLenum>(target),
        level,
        xoffset,
        x[0],
        x[1],
        width);
    detail::error_check("CopyTextureSubImage1DEXT"sv, check_errors);
}

template<class size_2_i32, class vec_2_i32>
requires(
    concepts::vector<vec_2_i32, i32, 2> && concepts::size_2d<size_2_i32, i32>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param texture GLuint
 * \param target GLenum
 * \param level GLint
 * \param xoffset GLint
 * \param yoffset GLint
 * \param x GLint
 * \param y GLint
 * \param width GLsizei
 * \param height GLsizei
 * \return void
 */
STATICINLINE void copy_texture_sub_image_2d(
    u32                   texture,
    group::texture_target target,
    i32                   level,
    vec_2_i32 const&      xoffset,
    vec_2_i32 const&      x,
    size_2_i32 const&     width,
    error_check           check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CopyTextureSubImage2DEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsTexture)
            glIsTexture(texture);
#endif
    }
    glCopyTextureSubImage2DEXT(
        texture,
        static_cast<GLenum>(target),
        level,
        xoffset[0],
        xoffset[1],
        x[0],
        x[1],
        width[0],
        width[1]);
    detail::error_check("CopyTextureSubImage2DEXT"sv, check_errors);
}

template<class span_void>
requires(concepts::span<span_void>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param texture GLuint
 * \param target GLenum
 * \param level GLint
 * \param format GLenum
 * \param type GLenum
 * \param pixels void *
 * \return void
 */
STATICINLINE void get_texture_image(
    u32                   texture,
    group::texture_target target,
    i32                   level,
    group::pixel_format   format,
    group::pixel_type     type,
    span_void             pixels,
    error_check           check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetTextureImageEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsTexture)
            glIsTexture(texture);
#endif
    }
    glGetTextureImageEXT(
        texture,
        static_cast<GLenum>(target),
        level,
        static_cast<GLenum>(format),
        static_cast<GLenum>(type),
        pixels.size() ? reinterpret_cast<void*>(pixels.data()) : nullptr);
    detail::error_check("GetTextureImageEXT"sv, check_errors);
}

template<class span_f32>
requires(
    concepts::span<span_f32> && std::is_same_v<
                                    std::decay_t<typename span_f32::value_type>,
                                    std::decay_t<f32>>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param texture GLuint
 * \param target GLenum
 * \param level GLint
 * \param pname GLenum
 * \param params GLfloat *
 * \return void
 */
STATICINLINE void get_texture_level_parameter(
    u32                          texture,
    group::texture_target        target,
    i32                          level,
    group::get_texture_parameter pname,
    span_f32                     params,
    error_check                  check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetTextureLevelParameterfvEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsTexture)
            glIsTexture(texture);
#endif
    }
    glGetTextureLevelParameterfvEXT(
        texture,
        static_cast<GLenum>(target),
        level,
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLfloat*>(params.data()) : nullptr);
    detail::error_check("GetTextureLevelParameterfvEXT"sv, check_errors);
}

template<class span_i32>
requires(
    concepts::span<span_i32> && std::is_same_v<
                                    std::decay_t<typename span_i32::value_type>,
                                    std::decay_t<i32>>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param texture GLuint
 * \param target GLenum
 * \param level GLint
 * \param pname GLenum
 * \param params GLint *
 * \return void
 */
STATICINLINE void get_texture_level_parameter(
    u32                          texture,
    group::texture_target        target,
    i32                          level,
    group::get_texture_parameter pname,
    span_i32                     params,
    error_check                  check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetTextureLevelParameterivEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsTexture)
            glIsTexture(texture);
#endif
    }
    glGetTextureLevelParameterivEXT(
        texture,
        static_cast<GLenum>(target),
        level,
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetTextureLevelParameterivEXT"sv, check_errors);
}

template<class span_f32>
requires(
    concepts::span<span_f32> && std::is_same_v<
                                    std::decay_t<typename span_f32::value_type>,
                                    std::decay_t<f32>>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param texture GLuint
 * \param target GLenum
 * \param pname GLenum
 * \param params GLfloat *
 * \return void
 */
STATICINLINE void get_texture_parameter(
    u32                          texture,
    group::texture_target        target,
    group::get_texture_parameter pname,
    span_f32                     params,
    error_check                  check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetTextureParameterfvEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsTexture)
            glIsTexture(texture);
#endif
    }
    glGetTextureParameterfvEXT(
        texture,
        static_cast<GLenum>(target),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLfloat*>(params.data()) : nullptr);
    detail::error_check("GetTextureParameterfvEXT"sv, check_errors);
}

template<class span_i32>
requires(
    concepts::span<span_i32> && std::is_same_v<
                                    std::decay_t<typename span_i32::value_type>,
                                    std::decay_t<i32>>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param texture GLuint
 * \param target GLenum
 * \param pname GLenum
 * \param params GLint *
 * \return void
 */
STATICINLINE void get_texture_parameter(
    u32                          texture,
    group::texture_target        target,
    group::get_texture_parameter pname,
    span_i32                     params,
    error_check                  check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetTextureParameterivEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsTexture)
            glIsTexture(texture);
#endif
    }
    glGetTextureParameterivEXT(
        texture,
        static_cast<GLenum>(target),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetTextureParameterivEXT"sv, check_errors);
}

template<class span_const_void>
requires(concepts::span<span_const_void>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param texture GLuint
 * \param target GLenum
 * \param level GLint
 * \param internalformat GLint
 * \param width GLsizei
 * \param border GLint
 * \param format GLenum
 * \param type GLenum
 * \param pixels const void *
 * \return void
 */
STATICINLINE void texture_image_1d(
    u32                    texture,
    group::texture_target  target,
    i32                    level,
    i32                    internalformat,
    i32                    width,
    i32                    border,
    group::pixel_format    format,
    group::pixel_type      type,
    span_const_void const& pixels,
    error_check            check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TextureImage1DEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsTexture)
            glIsTexture(texture);
#endif
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
    detail::error_check("TextureImage1DEXT"sv, check_errors);
}

template<class size_2_i32, class span_const_void>
requires(concepts::size_2d<size_2_i32, i32> && concepts::span<span_const_void>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param texture GLuint
 * \param target GLenum
 * \param level GLint
 * \param internalformat GLint
 * \param width GLsizei
 * \param height GLsizei
 * \param border GLint
 * \param format GLenum
 * \param type GLenum
 * \param pixels const void *
 * \return void
 */
STATICINLINE void texture_image_2d(
    u32                    texture,
    group::texture_target  target,
    i32                    level,
    i32                    internalformat,
    size_2_i32 const&      width,
    i32                    border,
    group::pixel_format    format,
    group::pixel_type      type,
    span_const_void const& pixels,
    error_check            check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TextureImage2DEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsTexture)
            glIsTexture(texture);
#endif
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
    detail::error_check("TextureImage2DEXT"sv, check_errors);
}

/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param texture GLuint
 * \param target GLenum
 * \param pname GLenum
 * \param param GLfloat
 * \return void
 */
STATICINLINE void texture_parameter(
    u32                           texture,
    group::texture_target         target,
    group::texture_parameter_name pname,
    f32                           param,
    error_check                   check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TextureParameterfEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsTexture)
            glIsTexture(texture);
#endif
    }
    glTextureParameterfEXT(
        texture,
        static_cast<GLenum>(target),
        static_cast<GLenum>(pname),
        param);
    detail::error_check("TextureParameterfEXT"sv, check_errors);
}

template<class span_const_f32>
requires(
    concepts::span<span_const_f32> &&
    std::is_same_v<
        std::decay_t<typename span_const_f32::value_type>,
        std::decay_t<f32>>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param texture GLuint
 * \param target GLenum
 * \param pname GLenum
 * \param params const GLfloat *
 * \return void
 */
STATICINLINE void texture_parameter(
    u32                           texture,
    group::texture_target         target,
    group::texture_parameter_name pname,
    span_const_f32 const&         params,
    error_check                   check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TextureParameterfvEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsTexture)
            glIsTexture(texture);
#endif
    }
    glTextureParameterfvEXT(
        texture,
        static_cast<GLenum>(target),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<const GLfloat*>(params.data())
                      : nullptr);
    detail::error_check("TextureParameterfvEXT"sv, check_errors);
}

/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param texture GLuint
 * \param target GLenum
 * \param pname GLenum
 * \param param GLint
 * \return void
 */
STATICINLINE void texture_parameter(
    u32                           texture,
    group::texture_target         target,
    group::texture_parameter_name pname,
    i32                           param,
    error_check                   check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TextureParameteriEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsTexture)
            glIsTexture(texture);
#endif
    }
    glTextureParameteriEXT(
        texture,
        static_cast<GLenum>(target),
        static_cast<GLenum>(pname),
        param);
    detail::error_check("TextureParameteriEXT"sv, check_errors);
}

template<class span_const_i32>
requires(
    concepts::span<span_const_i32> &&
    std::is_same_v<
        std::decay_t<typename span_const_i32::value_type>,
        std::decay_t<i32>>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param texture GLuint
 * \param target GLenum
 * \param pname GLenum
 * \param params const GLint *
 * \return void
 */
STATICINLINE void texture_parameter(
    u32                           texture,
    group::texture_target         target,
    group::texture_parameter_name pname,
    span_const_i32 const&         params,
    error_check                   check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TextureParameterivEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsTexture)
            glIsTexture(texture);
#endif
    }
    glTextureParameterivEXT(
        texture,
        static_cast<GLenum>(target),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<const GLint*>(params.data())
                      : nullptr);
    detail::error_check("TextureParameterivEXT"sv, check_errors);
}

template<class span_const_void>
requires(concepts::span<span_const_void>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param texture GLuint
 * \param target GLenum
 * \param level GLint
 * \param xoffset GLint
 * \param width GLsizei
 * \param format GLenum
 * \param type GLenum
 * \param pixels const void *
 * \return void
 */
STATICINLINE void texture_sub_image_1d(
    u32                    texture,
    group::texture_target  target,
    i32                    level,
    i32                    xoffset,
    i32                    width,
    group::pixel_format    format,
    group::pixel_type      type,
    span_const_void const& pixels,
    error_check            check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TextureSubImage1DEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsTexture)
            glIsTexture(texture);
#endif
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
    detail::error_check("TextureSubImage1DEXT"sv, check_errors);
}

template<class size_2_i32, class span_const_void, class vec_2_i32>
requires(
    concepts::vector<vec_2_i32, i32, 2> && concepts::size_2d<size_2_i32, i32> &&
    concepts::span<span_const_void>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param texture GLuint
 * \param target GLenum
 * \param level GLint
 * \param xoffset GLint
 * \param yoffset GLint
 * \param width GLsizei
 * \param height GLsizei
 * \param format GLenum
 * \param type GLenum
 * \param pixels const void *
 * \return void
 */
STATICINLINE void texture_sub_image_2d(
    u32                    texture,
    group::texture_target  target,
    i32                    level,
    vec_2_i32 const&       xoffset,
    size_2_i32 const&      width,
    group::pixel_format    format,
    group::pixel_type      type,
    span_const_void const& pixels,
    error_check            check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TextureSubImage2DEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsTexture)
            glIsTexture(texture);
#endif
    }
    glTextureSubImage2DEXT(
        texture,
        static_cast<GLenum>(target),
        level,
        xoffset[0],
        xoffset[1],
        width[0],
        width[1],
        static_cast<GLenum>(format),
        static_cast<GLenum>(type),
        pixels.size() ? reinterpret_cast<const void*>(pixels.data()) : nullptr);
    detail::error_check("TextureSubImage2DEXT"sv, check_errors);
}

template<class size_2_i32, class vec_2_i32, class vec_3_i32>
requires(
    concepts::vector<vec_3_i32, i32, 3> &&
    concepts::vector<vec_2_i32, i32, 2> && concepts::size_2d<size_2_i32, i32>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param texture GLuint
 * \param target GLenum
 * \param level GLint
 * \param xoffset GLint
 * \param yoffset GLint
 * \param zoffset GLint
 * \param x GLint
 * \param y GLint
 * \param width GLsizei
 * \param height GLsizei
 * \return void
 */
STATICINLINE void copy_texture_sub_image_3d(
    u32                   texture,
    group::texture_target target,
    i32                   level,
    vec_3_i32 const&      xoffset,
    vec_2_i32 const&      x,
    size_2_i32 const&     width,
    error_check           check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CopyTextureSubImage3DEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsTexture)
            glIsTexture(texture);
#endif
    }
    glCopyTextureSubImage3DEXT(
        texture,
        static_cast<GLenum>(target),
        level,
        xoffset[0],
        xoffset[1],
        xoffset[2],
        x[0],
        x[1],
        width[0],
        width[1]);
    detail::error_check("CopyTextureSubImage3DEXT"sv, check_errors);
}

template<class size_3_i32, class span_const_void>
requires(concepts::size_2d<size_3_i32, i32> && concepts::span<span_const_void>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param texture GLuint
 * \param target GLenum
 * \param level GLint
 * \param internalformat GLint
 * \param width GLsizei
 * \param height GLsizei
 * \param depth GLsizei
 * \param border GLint
 * \param format GLenum
 * \param type GLenum
 * \param pixels const void *
 * \return void
 */
STATICINLINE void texture_image_3d(
    u32                    texture,
    group::texture_target  target,
    i32                    level,
    i32                    internalformat,
    size_3_i32 const&      width,
    i32                    border,
    group::pixel_format    format,
    group::pixel_type      type,
    span_const_void const& pixels,
    error_check            check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TextureImage3DEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsTexture)
            glIsTexture(texture);
#endif
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
    detail::error_check("TextureImage3DEXT"sv, check_errors);
}

template<class size_3_i32, class span_const_void, class vec_3_i32>
requires(
    concepts::vector<vec_3_i32, i32, 3> && concepts::size_2d<size_3_i32, i32> &&
    concepts::span<span_const_void>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param texture GLuint
 * \param target GLenum
 * \param level GLint
 * \param xoffset GLint
 * \param yoffset GLint
 * \param zoffset GLint
 * \param width GLsizei
 * \param height GLsizei
 * \param depth GLsizei
 * \param format GLenum
 * \param type GLenum
 * \param pixels const void *
 * \return void
 */
STATICINLINE void texture_sub_image_3d(
    u32                    texture,
    group::texture_target  target,
    i32                    level,
    vec_3_i32 const&       xoffset,
    size_3_i32 const&      width,
    group::pixel_format    format,
    group::pixel_type      type,
    span_const_void const& pixels,
    error_check            check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TextureSubImage3DEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsTexture)
            glIsTexture(texture);
#endif
    }
    glTextureSubImage3DEXT(
        texture,
        static_cast<GLenum>(target),
        level,
        xoffset[0],
        xoffset[1],
        xoffset[2],
        width[0],
        width[1],
        width[2],
        static_cast<GLenum>(format),
        static_cast<GLenum>(type),
        pixels.size() ? reinterpret_cast<const void*>(pixels.data()) : nullptr);
    detail::error_check("TextureSubImage3DEXT"sv, check_errors);
}

/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param texunit GLenum
 * \param target GLenum
 * \param texture GLuint
 * \return void
 */
STATICINLINE void bind_multi_texture(
    group::texture_unit   texunit,
    group::texture_target target,
    u32                   texture,
    error_check           check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BindMultiTextureEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsTexture)
            glIsTexture(texture);
#endif
    }
    glBindMultiTextureEXT(
        static_cast<GLenum>(texunit), static_cast<GLenum>(target), texture);
    detail::error_check("BindMultiTextureEXT"sv, check_errors);
}

template<class vec_2_i32>
requires(concepts::vector<vec_2_i32, i32, 2>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param texunit GLenum
 * \param target GLenum
 * \param level GLint
 * \param internalformat GLenum
 * \param x GLint
 * \param y GLint
 * \param width GLsizei
 * \param border GLint
 * \return void
 */
STATICINLINE void copy_multi_tex_image_1d(
    group::texture_unit    texunit,
    group::texture_target  target,
    i32                    level,
    group::internal_format internalformat,
    vec_2_i32 const&       x,
    i32                    width,
    i32                    border,
    error_check            check_errors = error_check::on)
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
        x[0],
        x[1],
        width,
        border);
    detail::error_check("CopyMultiTexImage1DEXT"sv, check_errors);
}

template<class size_2_i32, class vec_2_i32>
requires(
    concepts::vector<vec_2_i32, i32, 2> && concepts::size_2d<size_2_i32, i32>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param texunit GLenum
 * \param target GLenum
 * \param level GLint
 * \param internalformat GLenum
 * \param x GLint
 * \param y GLint
 * \param width GLsizei
 * \param height GLsizei
 * \param border GLint
 * \return void
 */
STATICINLINE void copy_multi_tex_image_2d(
    group::texture_unit    texunit,
    group::texture_target  target,
    i32                    level,
    group::internal_format internalformat,
    vec_2_i32 const&       x,
    size_2_i32 const&      width,
    i32                    border,
    error_check            check_errors = error_check::on)
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
        x[0],
        x[1],
        width[0],
        width[1],
        border);
    detail::error_check("CopyMultiTexImage2DEXT"sv, check_errors);
}

template<class vec_2_i32>
requires(concepts::vector<vec_2_i32, i32, 2>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param texunit GLenum
 * \param target GLenum
 * \param level GLint
 * \param xoffset GLint
 * \param x GLint
 * \param y GLint
 * \param width GLsizei
 * \return void
 */
STATICINLINE void copy_multi_tex_sub_image_1d(
    group::texture_unit   texunit,
    group::texture_target target,
    i32                   level,
    i32                   xoffset,
    vec_2_i32 const&      x,
    i32                   width,
    error_check           check_errors = error_check::on)
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
        x[0],
        x[1],
        width);
    detail::error_check("CopyMultiTexSubImage1DEXT"sv, check_errors);
}

template<class size_2_i32, class vec_2_i32>
requires(
    concepts::vector<vec_2_i32, i32, 2> && concepts::size_2d<size_2_i32, i32>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param texunit GLenum
 * \param target GLenum
 * \param level GLint
 * \param xoffset GLint
 * \param yoffset GLint
 * \param x GLint
 * \param y GLint
 * \param width GLsizei
 * \param height GLsizei
 * \return void
 */
STATICINLINE void copy_multi_tex_sub_image_2d(
    group::texture_unit   texunit,
    group::texture_target target,
    i32                   level,
    vec_2_i32 const&      xoffset,
    vec_2_i32 const&      x,
    size_2_i32 const&     width,
    error_check           check_errors = error_check::on)
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
        xoffset[0],
        xoffset[1],
        x[0],
        x[1],
        width[0],
        width[1]);
    detail::error_check("CopyMultiTexSubImage2DEXT"sv, check_errors);
}

template<class size_2_i32, class vec_2_i32, class vec_3_i32>
requires(
    concepts::vector<vec_3_i32, i32, 3> &&
    concepts::vector<vec_2_i32, i32, 2> && concepts::size_2d<size_2_i32, i32>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param texunit GLenum
 * \param target GLenum
 * \param level GLint
 * \param xoffset GLint
 * \param yoffset GLint
 * \param zoffset GLint
 * \param x GLint
 * \param y GLint
 * \param width GLsizei
 * \param height GLsizei
 * \return void
 */
STATICINLINE void copy_multi_tex_sub_image_3d(
    group::texture_unit   texunit,
    group::texture_target target,
    i32                   level,
    vec_3_i32 const&      xoffset,
    vec_2_i32 const&      x,
    size_2_i32 const&     width,
    error_check           check_errors = error_check::on)
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
        xoffset[0],
        xoffset[1],
        xoffset[2],
        x[0],
        x[1],
        width[0],
        width[1]);
    detail::error_check("CopyMultiTexSubImage3DEXT"sv, check_errors);
}

template<class span_f32>
requires(
    concepts::span<span_f32> && std::is_same_v<
                                    std::decay_t<typename span_f32::value_type>,
                                    std::decay_t<f32>>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param texunit GLenum
 * \param target GLenum
 * \param pname GLenum
 * \param params GLfloat *
 * \return void
 */
STATICINLINE void get_multi_tex_envfv(
    group::texture_unit          texunit,
    group::texture_env_target    target,
    group::texture_env_parameter pname,
    span_f32                     params,
    error_check                  check_errors = error_check::on)
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
    detail::error_check("GetMultiTexEnvfvEXT"sv, check_errors);
}

template<class span_i32>
requires(
    concepts::span<span_i32> && std::is_same_v<
                                    std::decay_t<typename span_i32::value_type>,
                                    std::decay_t<i32>>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param texunit GLenum
 * \param target GLenum
 * \param pname GLenum
 * \param params GLint *
 * \return void
 */
STATICINLINE void get_multi_tex_enviv(
    group::texture_unit          texunit,
    group::texture_env_target    target,
    group::texture_env_parameter pname,
    span_i32                     params,
    error_check                  check_errors = error_check::on)
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
    detail::error_check("GetMultiTexEnvivEXT"sv, check_errors);
}

template<class span_f64>
requires(
    concepts::span<span_f64> && std::is_same_v<
                                    std::decay_t<typename span_f64::value_type>,
                                    std::decay_t<f64>>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param texunit GLenum
 * \param coord GLenum
 * \param pname GLenum
 * \param params GLdouble *
 * \return void
 */
STATICINLINE void get_multi_tex_gendv(
    group::texture_unit          texunit,
    group::texture_coord_name    coord,
    group::texture_gen_parameter pname,
    span_f64                     params,
    error_check                  check_errors = error_check::on)
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
    detail::error_check("GetMultiTexGendvEXT"sv, check_errors);
}

template<class span_f32>
requires(
    concepts::span<span_f32> && std::is_same_v<
                                    std::decay_t<typename span_f32::value_type>,
                                    std::decay_t<f32>>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param texunit GLenum
 * \param coord GLenum
 * \param pname GLenum
 * \param params GLfloat *
 * \return void
 */
STATICINLINE void get_multi_tex_genfv(
    group::texture_unit          texunit,
    group::texture_coord_name    coord,
    group::texture_gen_parameter pname,
    span_f32                     params,
    error_check                  check_errors = error_check::on)
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
    detail::error_check("GetMultiTexGenfvEXT"sv, check_errors);
}

template<class span_i32>
requires(
    concepts::span<span_i32> && std::is_same_v<
                                    std::decay_t<typename span_i32::value_type>,
                                    std::decay_t<i32>>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param texunit GLenum
 * \param coord GLenum
 * \param pname GLenum
 * \param params GLint *
 * \return void
 */
STATICINLINE void get_multi_tex_geniv(
    group::texture_unit          texunit,
    group::texture_coord_name    coord,
    group::texture_gen_parameter pname,
    span_i32                     params,
    error_check                  check_errors = error_check::on)
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
    detail::error_check("GetMultiTexGenivEXT"sv, check_errors);
}

template<class span_void>
requires(concepts::span<span_void>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param texunit GLenum
 * \param target GLenum
 * \param level GLint
 * \param format GLenum
 * \param type GLenum
 * \param pixels void *
 * \return void
 */
STATICINLINE void get_multi_tex_image(
    group::texture_unit   texunit,
    group::texture_target target,
    i32                   level,
    group::pixel_format   format,
    group::pixel_type     type,
    span_void             pixels,
    error_check           check_errors = error_check::on)
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
    detail::error_check("GetMultiTexImageEXT"sv, check_errors);
}

template<class span_f32>
requires(
    concepts::span<span_f32> && std::is_same_v<
                                    std::decay_t<typename span_f32::value_type>,
                                    std::decay_t<f32>>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param texunit GLenum
 * \param target GLenum
 * \param level GLint
 * \param pname GLenum
 * \param params GLfloat *
 * \return void
 */
STATICINLINE void get_multi_tex_level_parameter(
    group::texture_unit          texunit,
    group::texture_target        target,
    i32                          level,
    group::get_texture_parameter pname,
    span_f32                     params,
    error_check                  check_errors = error_check::on)
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
    detail::error_check("GetMultiTexLevelParameterfvEXT"sv, check_errors);
}

template<class span_i32>
requires(
    concepts::span<span_i32> && std::is_same_v<
                                    std::decay_t<typename span_i32::value_type>,
                                    std::decay_t<i32>>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param texunit GLenum
 * \param target GLenum
 * \param level GLint
 * \param pname GLenum
 * \param params GLint *
 * \return void
 */
STATICINLINE void get_multi_tex_level_parameter(
    group::texture_unit          texunit,
    group::texture_target        target,
    i32                          level,
    group::get_texture_parameter pname,
    span_i32                     params,
    error_check                  check_errors = error_check::on)
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
    detail::error_check("GetMultiTexLevelParameterivEXT"sv, check_errors);
}

template<class span_f32>
requires(
    concepts::span<span_f32> && std::is_same_v<
                                    std::decay_t<typename span_f32::value_type>,
                                    std::decay_t<f32>>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param texunit GLenum
 * \param target GLenum
 * \param pname GLenum
 * \param params GLfloat *
 * \return void
 */
STATICINLINE void get_multi_tex_parameter(
    group::texture_unit          texunit,
    group::texture_target        target,
    group::get_texture_parameter pname,
    span_f32                     params,
    error_check                  check_errors = error_check::on)
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
    detail::error_check("GetMultiTexParameterfvEXT"sv, check_errors);
}

template<class span_i32>
requires(
    concepts::span<span_i32> && std::is_same_v<
                                    std::decay_t<typename span_i32::value_type>,
                                    std::decay_t<i32>>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param texunit GLenum
 * \param target GLenum
 * \param pname GLenum
 * \param params GLint *
 * \return void
 */
STATICINLINE void get_multi_tex_parameter(
    group::texture_unit          texunit,
    group::texture_target        target,
    group::get_texture_parameter pname,
    span_i32                     params,
    error_check                  check_errors = error_check::on)
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
    detail::error_check("GetMultiTexParameterivEXT"sv, check_errors);
}

template<class span_const_void>
requires(concepts::span<span_const_void>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param texunit GLenum
 * \param size GLint
 * \param type GLenum
 * \param stride GLsizei
 * \param pointer const void *
 * \return void
 */
STATICINLINE void multi_tex_coord_pointer(
    group::texture_unit           texunit,
    i32                           size,
    group::tex_coord_pointer_type type,
    i32                           stride,
    span_const_void const&        pointer,
    error_check                   check_errors = error_check::on)
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
    detail::error_check("MultiTexCoordPointerEXT"sv, check_errors);
}

/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param texunit GLenum
 * \param target GLenum
 * \param pname GLenum
 * \param param GLfloat
 * \return void
 */
STATICINLINE void multi_tex_envf(
    group::texture_unit          texunit,
    group::texture_env_target    target,
    group::texture_env_parameter pname,
    f32                          param,
    error_check                  check_errors = error_check::on)
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
    detail::error_check("MultiTexEnvfEXT"sv, check_errors);
}

template<class span_const_f32>
requires(
    concepts::span<span_const_f32> &&
    std::is_same_v<
        std::decay_t<typename span_const_f32::value_type>,
        std::decay_t<f32>>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param texunit GLenum
 * \param target GLenum
 * \param pname GLenum
 * \param params const GLfloat *
 * \return void
 */
STATICINLINE void multi_tex_envfv(
    group::texture_unit          texunit,
    group::texture_env_target    target,
    group::texture_env_parameter pname,
    span_const_f32 const&        params,
    error_check                  check_errors = error_check::on)
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
    detail::error_check("MultiTexEnvfvEXT"sv, check_errors);
}

/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param texunit GLenum
 * \param target GLenum
 * \param pname GLenum
 * \param param GLint
 * \return void
 */
STATICINLINE void multi_tex_envi(
    group::texture_unit          texunit,
    group::texture_env_target    target,
    group::texture_env_parameter pname,
    i32                          param,
    error_check                  check_errors = error_check::on)
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
    detail::error_check("MultiTexEnviEXT"sv, check_errors);
}

template<class span_const_i32>
requires(
    concepts::span<span_const_i32> &&
    std::is_same_v<
        std::decay_t<typename span_const_i32::value_type>,
        std::decay_t<i32>>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param texunit GLenum
 * \param target GLenum
 * \param pname GLenum
 * \param params const GLint *
 * \return void
 */
STATICINLINE void multi_tex_enviv(
    group::texture_unit          texunit,
    group::texture_env_target    target,
    group::texture_env_parameter pname,
    span_const_i32 const&        params,
    error_check                  check_errors = error_check::on)
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
    detail::error_check("MultiTexEnvivEXT"sv, check_errors);
}

/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param texunit GLenum
 * \param coord GLenum
 * \param pname GLenum
 * \param param GLdouble
 * \return void
 */
STATICINLINE void multi_tex_gend(
    group::texture_unit          texunit,
    group::texture_coord_name    coord,
    group::texture_gen_parameter pname,
    f64                          param,
    error_check                  check_errors = error_check::on)
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
    detail::error_check("MultiTexGendEXT"sv, check_errors);
}

template<class span_const_f64>
requires(
    concepts::span<span_const_f64> &&
    std::is_same_v<
        std::decay_t<typename span_const_f64::value_type>,
        std::decay_t<f64>>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param texunit GLenum
 * \param coord GLenum
 * \param pname GLenum
 * \param params const GLdouble *
 * \return void
 */
STATICINLINE void multi_tex_gendv(
    group::texture_unit          texunit,
    group::texture_coord_name    coord,
    group::texture_gen_parameter pname,
    span_const_f64 const&        params,
    error_check                  check_errors = error_check::on)
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
    detail::error_check("MultiTexGendvEXT"sv, check_errors);
}

/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param texunit GLenum
 * \param coord GLenum
 * \param pname GLenum
 * \param param GLfloat
 * \return void
 */
STATICINLINE void multi_tex_genf(
    group::texture_unit          texunit,
    group::texture_coord_name    coord,
    group::texture_gen_parameter pname,
    f32                          param,
    error_check                  check_errors = error_check::on)
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
    detail::error_check("MultiTexGenfEXT"sv, check_errors);
}

template<class span_const_f32>
requires(
    concepts::span<span_const_f32> &&
    std::is_same_v<
        std::decay_t<typename span_const_f32::value_type>,
        std::decay_t<f32>>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param texunit GLenum
 * \param coord GLenum
 * \param pname GLenum
 * \param params const GLfloat *
 * \return void
 */
STATICINLINE void multi_tex_genfv(
    group::texture_unit          texunit,
    group::texture_coord_name    coord,
    group::texture_gen_parameter pname,
    span_const_f32 const&        params,
    error_check                  check_errors = error_check::on)
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
    detail::error_check("MultiTexGenfvEXT"sv, check_errors);
}

/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param texunit GLenum
 * \param coord GLenum
 * \param pname GLenum
 * \param param GLint
 * \return void
 */
STATICINLINE void multi_tex_geni(
    group::texture_unit          texunit,
    group::texture_coord_name    coord,
    group::texture_gen_parameter pname,
    i32                          param,
    error_check                  check_errors = error_check::on)
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
    detail::error_check("MultiTexGeniEXT"sv, check_errors);
}

template<class span_const_i32>
requires(
    concepts::span<span_const_i32> &&
    std::is_same_v<
        std::decay_t<typename span_const_i32::value_type>,
        std::decay_t<i32>>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param texunit GLenum
 * \param coord GLenum
 * \param pname GLenum
 * \param params const GLint *
 * \return void
 */
STATICINLINE void multi_tex_geniv(
    group::texture_unit          texunit,
    group::texture_coord_name    coord,
    group::texture_gen_parameter pname,
    span_const_i32 const&        params,
    error_check                  check_errors = error_check::on)
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
    detail::error_check("MultiTexGenivEXT"sv, check_errors);
}

template<class span_const_void>
requires(concepts::span<span_const_void>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param texunit GLenum
 * \param target GLenum
 * \param level GLint
 * \param internalformat GLint
 * \param width GLsizei
 * \param border GLint
 * \param format GLenum
 * \param type GLenum
 * \param pixels const void *
 * \return void
 */
STATICINLINE void multi_tex_image_1d(
    group::texture_unit    texunit,
    group::texture_target  target,
    i32                    level,
    i32                    internalformat,
    i32                    width,
    i32                    border,
    group::pixel_format    format,
    group::pixel_type      type,
    span_const_void const& pixels,
    error_check            check_errors = error_check::on)
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
    detail::error_check("MultiTexImage1DEXT"sv, check_errors);
}

template<class size_2_i32, class span_const_void>
requires(concepts::size_2d<size_2_i32, i32> && concepts::span<span_const_void>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param texunit GLenum
 * \param target GLenum
 * \param level GLint
 * \param internalformat GLint
 * \param width GLsizei
 * \param height GLsizei
 * \param border GLint
 * \param format GLenum
 * \param type GLenum
 * \param pixels const void *
 * \return void
 */
STATICINLINE void multi_tex_image_2d(
    group::texture_unit    texunit,
    group::texture_target  target,
    i32                    level,
    i32                    internalformat,
    size_2_i32 const&      width,
    i32                    border,
    group::pixel_format    format,
    group::pixel_type      type,
    span_const_void const& pixels,
    error_check            check_errors = error_check::on)
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
    detail::error_check("MultiTexImage2DEXT"sv, check_errors);
}

template<class size_3_i32, class span_const_void>
requires(concepts::size_2d<size_3_i32, i32> && concepts::span<span_const_void>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param texunit GLenum
 * \param target GLenum
 * \param level GLint
 * \param internalformat GLint
 * \param width GLsizei
 * \param height GLsizei
 * \param depth GLsizei
 * \param border GLint
 * \param format GLenum
 * \param type GLenum
 * \param pixels const void *
 * \return void
 */
STATICINLINE void multi_tex_image_3d(
    group::texture_unit    texunit,
    group::texture_target  target,
    i32                    level,
    i32                    internalformat,
    size_3_i32 const&      width,
    i32                    border,
    group::pixel_format    format,
    group::pixel_type      type,
    span_const_void const& pixels,
    error_check            check_errors = error_check::on)
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
    detail::error_check("MultiTexImage3DEXT"sv, check_errors);
}

/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param texunit GLenum
 * \param target GLenum
 * \param pname GLenum
 * \param param GLfloat
 * \return void
 */
STATICINLINE void multi_tex_parameter(
    group::texture_unit           texunit,
    group::texture_target         target,
    group::texture_parameter_name pname,
    f32                           param,
    error_check                   check_errors = error_check::on)
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
    detail::error_check("MultiTexParameterfEXT"sv, check_errors);
}

template<class span_const_f32>
requires(
    concepts::span<span_const_f32> &&
    std::is_same_v<
        std::decay_t<typename span_const_f32::value_type>,
        std::decay_t<f32>>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param texunit GLenum
 * \param target GLenum
 * \param pname GLenum
 * \param params const GLfloat *
 * \return void
 */
STATICINLINE void multi_tex_parameter(
    group::texture_unit           texunit,
    group::texture_target         target,
    group::texture_parameter_name pname,
    span_const_f32 const&         params,
    error_check                   check_errors = error_check::on)
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
    detail::error_check("MultiTexParameterfvEXT"sv, check_errors);
}

/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param texunit GLenum
 * \param target GLenum
 * \param pname GLenum
 * \param param GLint
 * \return void
 */
STATICINLINE void multi_tex_parameter(
    group::texture_unit           texunit,
    group::texture_target         target,
    group::texture_parameter_name pname,
    i32                           param,
    error_check                   check_errors = error_check::on)
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
    detail::error_check("MultiTexParameteriEXT"sv, check_errors);
}

template<class span_const_i32>
requires(
    concepts::span<span_const_i32> &&
    std::is_same_v<
        std::decay_t<typename span_const_i32::value_type>,
        std::decay_t<i32>>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param texunit GLenum
 * \param target GLenum
 * \param pname GLenum
 * \param params const GLint *
 * \return void
 */
STATICINLINE void multi_tex_parameter(
    group::texture_unit           texunit,
    group::texture_target         target,
    group::texture_parameter_name pname,
    span_const_i32 const&         params,
    error_check                   check_errors = error_check::on)
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
    detail::error_check("MultiTexParameterivEXT"sv, check_errors);
}

template<class span_const_void>
requires(concepts::span<span_const_void>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param texunit GLenum
 * \param target GLenum
 * \param level GLint
 * \param xoffset GLint
 * \param width GLsizei
 * \param format GLenum
 * \param type GLenum
 * \param pixels const void *
 * \return void
 */
STATICINLINE void multi_tex_sub_image_1d(
    group::texture_unit    texunit,
    group::texture_target  target,
    i32                    level,
    i32                    xoffset,
    i32                    width,
    group::pixel_format    format,
    group::pixel_type      type,
    span_const_void const& pixels,
    error_check            check_errors = error_check::on)
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
    detail::error_check("MultiTexSubImage1DEXT"sv, check_errors);
}

template<class size_2_i32, class span_const_void, class vec_2_i32>
requires(
    concepts::vector<vec_2_i32, i32, 2> && concepts::size_2d<size_2_i32, i32> &&
    concepts::span<span_const_void>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param texunit GLenum
 * \param target GLenum
 * \param level GLint
 * \param xoffset GLint
 * \param yoffset GLint
 * \param width GLsizei
 * \param height GLsizei
 * \param format GLenum
 * \param type GLenum
 * \param pixels const void *
 * \return void
 */
STATICINLINE void multi_tex_sub_image_2d(
    group::texture_unit    texunit,
    group::texture_target  target,
    i32                    level,
    vec_2_i32 const&       xoffset,
    size_2_i32 const&      width,
    group::pixel_format    format,
    group::pixel_type      type,
    span_const_void const& pixels,
    error_check            check_errors = error_check::on)
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
        xoffset[0],
        xoffset[1],
        width[0],
        width[1],
        static_cast<GLenum>(format),
        static_cast<GLenum>(type),
        pixels.size() ? reinterpret_cast<const void*>(pixels.data()) : nullptr);
    detail::error_check("MultiTexSubImage2DEXT"sv, check_errors);
}

template<class size_3_i32, class span_const_void, class vec_3_i32>
requires(
    concepts::vector<vec_3_i32, i32, 3> && concepts::size_2d<size_3_i32, i32> &&
    concepts::span<span_const_void>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param texunit GLenum
 * \param target GLenum
 * \param level GLint
 * \param xoffset GLint
 * \param yoffset GLint
 * \param zoffset GLint
 * \param width GLsizei
 * \param height GLsizei
 * \param depth GLsizei
 * \param format GLenum
 * \param type GLenum
 * \param pixels const void *
 * \return void
 */
STATICINLINE void multi_tex_sub_image_3d(
    group::texture_unit    texunit,
    group::texture_target  target,
    i32                    level,
    vec_3_i32 const&       xoffset,
    size_3_i32 const&      width,
    group::pixel_format    format,
    group::pixel_type      type,
    span_const_void const& pixels,
    error_check            check_errors = error_check::on)
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
        xoffset[0],
        xoffset[1],
        xoffset[2],
        width[0],
        width[1],
        width[2],
        static_cast<GLenum>(format),
        static_cast<GLenum>(type),
        pixels.size() ? reinterpret_cast<const void*>(pixels.data()) : nullptr);
    detail::error_check("MultiTexSubImage3DEXT"sv, check_errors);
}

/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param array GLenum
 * \param index GLuint
 * \return void
 */
STATICINLINE void disable_client_state_indexed(
    group::enable_cap array,
    u32               index,
    error_check       check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DisableClientStateIndexedEXT)
    }
    glDisableClientStateIndexedEXT(static_cast<GLenum>(array), index);
    detail::error_check("DisableClientStateIndexedEXT"sv, check_errors);
}

/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param array GLenum
 * \param index GLuint
 * \return void
 */
STATICINLINE void enable_client_state_indexed(
    group::enable_cap array,
    u32               index,
    error_check       check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(EnableClientStateIndexedEXT)
    }
    glEnableClientStateIndexedEXT(static_cast<GLenum>(array), index);
    detail::error_check("EnableClientStateIndexedEXT"sv, check_errors);
}

template<class span_f64>
requires(
    concepts::span<span_f64> && std::is_same_v<
                                    std::decay_t<typename span_f64::value_type>,
                                    std::decay_t<f64>>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param target GLenum
 * \param index GLuint
 * \param data GLdouble *
 * \return void
 */
STATICINLINE void get_double_indexedv(
    group::get_prop target,
    u32             index,
    span_f64        data,
    error_check     check_errors = error_check::on)
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
    detail::error_check("GetDoubleIndexedvEXT"sv, check_errors);
}

template<class span_f32>
requires(
    concepts::span<span_f32> && std::is_same_v<
                                    std::decay_t<typename span_f32::value_type>,
                                    std::decay_t<f32>>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param target GLenum
 * \param index GLuint
 * \param data GLfloat *
 * \return void
 */
STATICINLINE void get_float_indexedv(
    group::get_prop target,
    u32             index,
    span_f32        data,
    error_check     check_errors = error_check::on)
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
    detail::error_check("GetFloatIndexedvEXT"sv, check_errors);
}

template<class span_void>
requires(concepts::span<span_void>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param target GLenum
 * \param index GLuint
 * \param data void **
 * \return void
 */
STATICINLINE void get_pointer_indexedv(
    GLenum      target,
    u32         index,
    span_void   data,
    error_check check_errors = error_check::on)
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
    detail::error_check("GetPointerIndexedvEXT"sv, check_errors);
}

/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param target GLenum
 * \param index GLuint
 * \return void
 */
STATICINLINE void disable_indexed(
    group::enable_cap target,
    u32               index,
    error_check       check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DisableIndexedEXT)
    }
    glDisableIndexedEXT(static_cast<GLenum>(target), index);
    detail::error_check("DisableIndexedEXT"sv, check_errors);
}

/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param target GLenum
 * \param index GLuint
 * \return void
 */
STATICINLINE void enable_indexed(
    group::enable_cap target,
    u32               index,
    error_check       check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(EnableIndexedEXT)
    }
    glEnableIndexedEXT(static_cast<GLenum>(target), index);
    detail::error_check("EnableIndexedEXT"sv, check_errors);
}

template<class span_bool>
requires(
    concepts::span<span_bool> &&
    std::is_same_v<
        std::decay_t<typename span_bool::value_type>,
        std::decay_t<bool>>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param target GLenum
 * \param index GLuint
 * \param data GLboolean *
 * \return void
 */
STATICINLINE void get_boolean_indexedv(
    group::buffer_target_arb target,
    u32                      index,
    span_bool                data,
    error_check              check_errors = error_check::on)
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
    detail::error_check("GetBooleanIndexedvEXT"sv, check_errors);
}

template<class span_i32>
requires(
    concepts::span<span_i32> && std::is_same_v<
                                    std::decay_t<typename span_i32::value_type>,
                                    std::decay_t<i32>>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param target GLenum
 * \param index GLuint
 * \param data GLint *
 * \return void
 */
STATICINLINE void get_integer_indexedv(
    group::get_prop target,
    u32             index,
    span_i32        data,
    error_check     check_errors = error_check::on)
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
    detail::error_check("GetIntegerIndexedvEXT"sv, check_errors);
}

/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param target GLenum
 * \param index GLuint
 * \return Boolean
 */
STATICINLINE bool is_enabled_indexed(
    group::enable_cap target,
    u32               index,
    error_check       check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(IsEnabledIndexedEXT)
    }
    auto out = glIsEnabledIndexedEXT(static_cast<GLenum>(target), index);
    detail::error_check("IsEnabledIndexedEXT"sv, check_errors);
    return out == GL_TRUE ? true : false;
}

template<class span_const_void>
requires(concepts::span<span_const_void>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param texture GLuint
 * \param target GLenum
 * \param level GLint
 * \param internalformat GLenum
 * \param width GLsizei
 * \param border GLint
 * \param imageSize GLsizei
 * \param bits const void *
 * \return void
 */
STATICINLINE void compressed_texture_image_1d(
    u32                    texture,
    group::texture_target  target,
    i32                    level,
    group::internal_format internalformat,
    i32                    width,
    i32                    border,
    span_const_void const& bits,
    error_check            check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CompressedTextureImage1DEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsTexture)
            glIsTexture(texture);
#endif
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
    detail::error_check("CompressedTextureImage1DEXT"sv, check_errors);
}

template<class size_2_i32, class span_const_void>
requires(concepts::size_2d<size_2_i32, i32> && concepts::span<span_const_void>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param texture GLuint
 * \param target GLenum
 * \param level GLint
 * \param internalformat GLenum
 * \param width GLsizei
 * \param height GLsizei
 * \param border GLint
 * \param imageSize GLsizei
 * \param bits const void *
 * \return void
 */
STATICINLINE void compressed_texture_image_2d(
    u32                    texture,
    group::texture_target  target,
    i32                    level,
    group::internal_format internalformat,
    size_2_i32 const&      width,
    i32                    border,
    span_const_void const& bits,
    error_check            check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CompressedTextureImage2DEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsTexture)
            glIsTexture(texture);
#endif
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
    detail::error_check("CompressedTextureImage2DEXT"sv, check_errors);
}

template<class size_3_i32, class span_const_void>
requires(concepts::size_2d<size_3_i32, i32> && concepts::span<span_const_void>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param texture GLuint
 * \param target GLenum
 * \param level GLint
 * \param internalformat GLenum
 * \param width GLsizei
 * \param height GLsizei
 * \param depth GLsizei
 * \param border GLint
 * \param imageSize GLsizei
 * \param bits const void *
 * \return void
 */
STATICINLINE void compressed_texture_image_3d(
    u32                    texture,
    group::texture_target  target,
    i32                    level,
    group::internal_format internalformat,
    size_3_i32 const&      width,
    i32                    border,
    span_const_void const& bits,
    error_check            check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CompressedTextureImage3DEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsTexture)
            glIsTexture(texture);
#endif
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
    detail::error_check("CompressedTextureImage3DEXT"sv, check_errors);
}

template<class span_const_void>
requires(concepts::span<span_const_void>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param texture GLuint
 * \param target GLenum
 * \param level GLint
 * \param xoffset GLint
 * \param width GLsizei
 * \param format GLenum
 * \param imageSize GLsizei
 * \param bits const void *
 * \return void
 */
STATICINLINE void compressed_texture_sub_image_1d(
    u32                    texture,
    group::texture_target  target,
    i32                    level,
    i32                    xoffset,
    i32                    width,
    group::internal_format format,
    span_const_void const& bits,
    error_check            check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CompressedTextureSubImage1DEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsTexture)
            glIsTexture(texture);
#endif
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
    detail::error_check("CompressedTextureSubImage1DEXT"sv, check_errors);
}

template<class size_2_i32, class span_const_void, class vec_2_i32>
requires(
    concepts::vector<vec_2_i32, i32, 2> && concepts::size_2d<size_2_i32, i32> &&
    concepts::span<span_const_void>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param texture GLuint
 * \param target GLenum
 * \param level GLint
 * \param xoffset GLint
 * \param yoffset GLint
 * \param width GLsizei
 * \param height GLsizei
 * \param format GLenum
 * \param imageSize GLsizei
 * \param bits const void *
 * \return void
 */
STATICINLINE void compressed_texture_sub_image_2d(
    u32                    texture,
    group::texture_target  target,
    i32                    level,
    vec_2_i32 const&       xoffset,
    size_2_i32 const&      width,
    group::internal_format format,
    span_const_void const& bits,
    error_check            check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CompressedTextureSubImage2DEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsTexture)
            glIsTexture(texture);
#endif
    }
    glCompressedTextureSubImage2DEXT(
        texture,
        static_cast<GLenum>(target),
        level,
        xoffset[0],
        xoffset[1],
        width[0],
        width[1],
        static_cast<GLenum>(format),
        bits.size() *
            sizeof(typename std::decay_t<span_const_void const&>::value_type),
        bits.size() ? reinterpret_cast<const void*>(bits.data()) : nullptr);
    detail::error_check("CompressedTextureSubImage2DEXT"sv, check_errors);
}

template<class size_3_i32, class span_const_void, class vec_3_i32>
requires(
    concepts::vector<vec_3_i32, i32, 3> && concepts::size_2d<size_3_i32, i32> &&
    concepts::span<span_const_void>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param texture GLuint
 * \param target GLenum
 * \param level GLint
 * \param xoffset GLint
 * \param yoffset GLint
 * \param zoffset GLint
 * \param width GLsizei
 * \param height GLsizei
 * \param depth GLsizei
 * \param format GLenum
 * \param imageSize GLsizei
 * \param bits const void *
 * \return void
 */
STATICINLINE void compressed_texture_sub_image_3d(
    u32                    texture,
    group::texture_target  target,
    i32                    level,
    vec_3_i32 const&       xoffset,
    size_3_i32 const&      width,
    group::internal_format format,
    span_const_void const& bits,
    error_check            check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CompressedTextureSubImage3DEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsTexture)
            glIsTexture(texture);
#endif
    }
    glCompressedTextureSubImage3DEXT(
        texture,
        static_cast<GLenum>(target),
        level,
        xoffset[0],
        xoffset[1],
        xoffset[2],
        width[0],
        width[1],
        width[2],
        static_cast<GLenum>(format),
        bits.size() *
            sizeof(typename std::decay_t<span_const_void const&>::value_type),
        bits.size() ? reinterpret_cast<const void*>(bits.data()) : nullptr);
    detail::error_check("CompressedTextureSubImage3DEXT"sv, check_errors);
}

template<class span_void>
requires(concepts::span<span_void>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param texture GLuint
 * \param target GLenum
 * \param lod GLint
 * \param img void *
 * \return void
 */
STATICINLINE void get_compressed_texture_image(
    u32                   texture,
    group::texture_target target,
    i32                   lod,
    span_void             img,
    error_check           check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetCompressedTextureImageEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsTexture)
            glIsTexture(texture);
#endif
    }
    glGetCompressedTextureImageEXT(
        texture,
        static_cast<GLenum>(target),
        lod,
        img.size() ? reinterpret_cast<void*>(img.data()) : nullptr);
    detail::error_check("GetCompressedTextureImageEXT"sv, check_errors);
}

template<class span_const_void>
requires(concepts::span<span_const_void>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param texunit GLenum
 * \param target GLenum
 * \param level GLint
 * \param internalformat GLenum
 * \param width GLsizei
 * \param border GLint
 * \param imageSize GLsizei
 * \param bits const void *
 * \return void
 */
STATICINLINE void compressed_multi_tex_image_1d(
    group::texture_unit    texunit,
    group::texture_target  target,
    i32                    level,
    group::internal_format internalformat,
    i32                    width,
    i32                    border,
    span_const_void const& bits,
    error_check            check_errors = error_check::on)
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
    detail::error_check("CompressedMultiTexImage1DEXT"sv, check_errors);
}

template<class size_2_i32, class span_const_void>
requires(concepts::size_2d<size_2_i32, i32> && concepts::span<span_const_void>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param texunit GLenum
 * \param target GLenum
 * \param level GLint
 * \param internalformat GLenum
 * \param width GLsizei
 * \param height GLsizei
 * \param border GLint
 * \param imageSize GLsizei
 * \param bits const void *
 * \return void
 */
STATICINLINE void compressed_multi_tex_image_2d(
    group::texture_unit    texunit,
    group::texture_target  target,
    i32                    level,
    group::internal_format internalformat,
    size_2_i32 const&      width,
    i32                    border,
    span_const_void const& bits,
    error_check            check_errors = error_check::on)
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
    detail::error_check("CompressedMultiTexImage2DEXT"sv, check_errors);
}

template<class size_3_i32, class span_const_void>
requires(concepts::size_2d<size_3_i32, i32> && concepts::span<span_const_void>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param texunit GLenum
 * \param target GLenum
 * \param level GLint
 * \param internalformat GLenum
 * \param width GLsizei
 * \param height GLsizei
 * \param depth GLsizei
 * \param border GLint
 * \param imageSize GLsizei
 * \param bits const void *
 * \return void
 */
STATICINLINE void compressed_multi_tex_image_3d(
    group::texture_unit    texunit,
    group::texture_target  target,
    i32                    level,
    group::internal_format internalformat,
    size_3_i32 const&      width,
    i32                    border,
    span_const_void const& bits,
    error_check            check_errors = error_check::on)
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
    detail::error_check("CompressedMultiTexImage3DEXT"sv, check_errors);
}

template<class span_const_void>
requires(concepts::span<span_const_void>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param texunit GLenum
 * \param target GLenum
 * \param level GLint
 * \param xoffset GLint
 * \param width GLsizei
 * \param format GLenum
 * \param imageSize GLsizei
 * \param bits const void *
 * \return void
 */
STATICINLINE void compressed_multi_tex_sub_image_1d(
    group::texture_unit    texunit,
    group::texture_target  target,
    i32                    level,
    i32                    xoffset,
    i32                    width,
    group::internal_format format,
    span_const_void const& bits,
    error_check            check_errors = error_check::on)
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
    detail::error_check("CompressedMultiTexSubImage1DEXT"sv, check_errors);
}

template<class size_2_i32, class span_const_void, class vec_2_i32>
requires(
    concepts::vector<vec_2_i32, i32, 2> && concepts::size_2d<size_2_i32, i32> &&
    concepts::span<span_const_void>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param texunit GLenum
 * \param target GLenum
 * \param level GLint
 * \param xoffset GLint
 * \param yoffset GLint
 * \param width GLsizei
 * \param height GLsizei
 * \param format GLenum
 * \param imageSize GLsizei
 * \param bits const void *
 * \return void
 */
STATICINLINE void compressed_multi_tex_sub_image_2d(
    group::texture_unit    texunit,
    group::texture_target  target,
    i32                    level,
    vec_2_i32 const&       xoffset,
    size_2_i32 const&      width,
    group::internal_format format,
    span_const_void const& bits,
    error_check            check_errors = error_check::on)
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
        xoffset[0],
        xoffset[1],
        width[0],
        width[1],
        static_cast<GLenum>(format),
        bits.size() *
            sizeof(typename std::decay_t<span_const_void const&>::value_type),
        bits.size() ? reinterpret_cast<const void*>(bits.data()) : nullptr);
    detail::error_check("CompressedMultiTexSubImage2DEXT"sv, check_errors);
}

template<class size_3_i32, class span_const_void, class vec_3_i32>
requires(
    concepts::vector<vec_3_i32, i32, 3> && concepts::size_2d<size_3_i32, i32> &&
    concepts::span<span_const_void>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param texunit GLenum
 * \param target GLenum
 * \param level GLint
 * \param xoffset GLint
 * \param yoffset GLint
 * \param zoffset GLint
 * \param width GLsizei
 * \param height GLsizei
 * \param depth GLsizei
 * \param format GLenum
 * \param imageSize GLsizei
 * \param bits const void *
 * \return void
 */
STATICINLINE void compressed_multi_tex_sub_image_3d(
    group::texture_unit    texunit,
    group::texture_target  target,
    i32                    level,
    vec_3_i32 const&       xoffset,
    size_3_i32 const&      width,
    group::internal_format format,
    span_const_void const& bits,
    error_check            check_errors = error_check::on)
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
        xoffset[0],
        xoffset[1],
        xoffset[2],
        width[0],
        width[1],
        width[2],
        static_cast<GLenum>(format),
        bits.size() *
            sizeof(typename std::decay_t<span_const_void const&>::value_type),
        bits.size() ? reinterpret_cast<const void*>(bits.data()) : nullptr);
    detail::error_check("CompressedMultiTexSubImage3DEXT"sv, check_errors);
}

template<class span_void>
requires(concepts::span<span_void>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param texunit GLenum
 * \param target GLenum
 * \param lod GLint
 * \param img void *
 * \return void
 */
STATICINLINE void get_compressed_multi_tex_image(
    group::texture_unit   texunit,
    group::texture_target target,
    i32                   lod,
    span_void             img,
    error_check           check_errors = error_check::on)
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
    detail::error_check("GetCompressedMultiTexImageEXT"sv, check_errors);
}

template<class span_const_f64>
requires(
    concepts::span<span_const_f64> &&
    std::is_same_v<
        std::decay_t<typename span_const_f64::value_type>,
        std::decay_t<f64>>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param mode GLenum
 * \param m const GLdouble *
 * \return void
 */
STATICINLINE void matrix_load_transposed(
    group::matrix_mode    mode,
    span_const_f64 const& m,
    error_check           check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MatrixLoadTransposedEXT)
    }
    glMatrixLoadTransposedEXT(
        static_cast<GLenum>(mode),
        m.size() ? reinterpret_cast<const GLdouble*>(m.data()) : nullptr);
    detail::error_check("MatrixLoadTransposedEXT"sv, check_errors);
}

template<class span_const_f32>
requires(
    concepts::span<span_const_f32> &&
    std::is_same_v<
        std::decay_t<typename span_const_f32::value_type>,
        std::decay_t<f32>>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param mode GLenum
 * \param m const GLfloat *
 * \return void
 */
STATICINLINE void matrix_load_transposef(
    group::matrix_mode    mode,
    span_const_f32 const& m,
    error_check           check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MatrixLoadTransposefEXT)
    }
    glMatrixLoadTransposefEXT(
        static_cast<GLenum>(mode),
        m.size() ? reinterpret_cast<const GLfloat*>(m.data()) : nullptr);
    detail::error_check("MatrixLoadTransposefEXT"sv, check_errors);
}

template<class span_const_f64>
requires(
    concepts::span<span_const_f64> &&
    std::is_same_v<
        std::decay_t<typename span_const_f64::value_type>,
        std::decay_t<f64>>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param mode GLenum
 * \param m const GLdouble *
 * \return void
 */
STATICINLINE void matrix_mult_transposed(
    group::matrix_mode    mode,
    span_const_f64 const& m,
    error_check           check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MatrixMultTransposedEXT)
    }
    glMatrixMultTransposedEXT(
        static_cast<GLenum>(mode),
        m.size() ? reinterpret_cast<const GLdouble*>(m.data()) : nullptr);
    detail::error_check("MatrixMultTransposedEXT"sv, check_errors);
}

template<class span_const_f32>
requires(
    concepts::span<span_const_f32> &&
    std::is_same_v<
        std::decay_t<typename span_const_f32::value_type>,
        std::decay_t<f32>>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param mode GLenum
 * \param m const GLfloat *
 * \return void
 */
STATICINLINE void matrix_mult_transposef(
    group::matrix_mode    mode,
    span_const_f32 const& m,
    error_check           check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MatrixMultTransposefEXT)
    }
    glMatrixMultTransposefEXT(
        static_cast<GLenum>(mode),
        m.size() ? reinterpret_cast<const GLfloat*>(m.data()) : nullptr);
    detail::error_check("MatrixMultTransposefEXT"sv, check_errors);
}

template<class span_i32>
requires(
    concepts::span<span_i32> && std::is_same_v<
                                    std::decay_t<typename span_i32::value_type>,
                                    std::decay_t<i32>>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param buffer GLuint
 * \param pname GLenum
 * \param params GLint *
 * \return void
 */
STATICINLINE void get_named_buffer_parameter(
    u32                    buffer,
    group::buffer_prop_arb pname,
    span_i32               params,
    error_check            check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetNamedBufferParameterivEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsBuffer)
            glIsBuffer(buffer);
#endif
    }
    glGetNamedBufferParameterivEXT(
        buffer,
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetNamedBufferParameterivEXT"sv, check_errors);
}

template<class span_void>
requires(concepts::span<span_void>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param buffer GLuint
 * \param pname GLenum
 * \param params void **
 * \return void
 */
STATICINLINE void get_named_buffer_pointerv(
    u32                            buffer,
    group::buffer_pointer_name_arb pname,
    span_void                      params,
    error_check                    check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetNamedBufferPointervEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsBuffer)
            glIsBuffer(buffer);
#endif
    }
    glGetNamedBufferPointervEXT(
        buffer,
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<void**>(params.data()) : nullptr);
    detail::error_check("GetNamedBufferPointervEXT"sv, check_errors);
}

template<class span_void>
requires(concepts::span<span_void>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param buffer GLuint
 * \param offset GLintptr
 * \param size GLsizeiptr
 * \param data void *
 * \return void
 */
STATICINLINE void get_named_buffer_sub_data(
    u32         buffer,
    GLintptr    offset,
    GLsizeiptr  size,
    span_void   data,
    error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetNamedBufferSubDataEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsBuffer)
            glIsBuffer(buffer);
#endif
    }
    glGetNamedBufferSubDataEXT(
        buffer,
        offset,
        size,
        data.size() ? reinterpret_cast<void*>(data.data()) : nullptr);
    detail::error_check("GetNamedBufferSubDataEXT"sv, check_errors);
}

/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param buffer GLuint
 * \param access GLenum
 * \return void *
 */
STATICINLINE void* map_named_buffer(
    u32                      buffer,
    group::buffer_access_arb access,
    error_check              check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MapNamedBufferEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsBuffer)
            glIsBuffer(buffer);
#endif
    }
    auto out = glMapNamedBufferEXT(buffer, static_cast<GLenum>(access));
    detail::error_check("MapNamedBufferEXT"sv, check_errors);
    return out;
}

template<class span_const_void>
requires(concepts::span<span_const_void>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param buffer GLuint
 * \param size GLsizeiptr
 * \param data const void *
 * \param usage GLenum
 * \return void
 */
STATICINLINE void named_buffer_data(
    u32                               buffer,
    GLsizeiptr                        size,
    span_const_void const&            data,
    group::vertex_buffer_object_usage usage,
    error_check                       check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(NamedBufferDataEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsBuffer)
            glIsBuffer(buffer);
#endif
    }
    glNamedBufferDataEXT(
        buffer,
        size,
        data.size() ? reinterpret_cast<const void*>(data.data()) : nullptr,
        static_cast<GLenum>(usage));
    detail::error_check("NamedBufferDataEXT"sv, check_errors);
}

template<class span_const_void>
requires(concepts::span<span_const_void>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param buffer GLuint
 * \param offset GLintptr
 * \param size GLsizeiptr
 * \param data const void *
 * \return void
 */
STATICINLINE void named_buffer_sub_data(
    u32                    buffer,
    GLintptr               offset,
    GLsizeiptr             size,
    span_const_void const& data,
    error_check            check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(NamedBufferSubDataEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsBuffer)
            glIsBuffer(buffer);
#endif
    }
    glNamedBufferSubDataEXT(
        buffer,
        offset,
        size,
        data.size() ? reinterpret_cast<const void*>(data.data()) : nullptr);
    detail::error_check("NamedBufferSubDataEXT"sv, check_errors);
}

/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param buffer GLuint
 * \return Boolean
 */
STATICINLINE bool unmap_named_buffer(
    u32 buffer, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(UnmapNamedBufferEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsBuffer)
            glIsBuffer(buffer);
#endif
    }
    auto out = glUnmapNamedBufferEXT(buffer);
    detail::error_check("UnmapNamedBufferEXT"sv, check_errors);
    return out == GL_TRUE ? true : false;
}

/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param program GLuint
 * \param location GLint
 * \param v0 GLfloat
 * \return void
 */
STATICINLINE void program_uniform(
    u32         program,
    i32         location,
    f32         v0,
    error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform1fEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glProgramUniform1fEXT(program, location, v0);
    detail::error_check("ProgramUniform1fEXT"sv, check_errors);
}

template<class span_const_f32>
requires(
    concepts::span<span_const_f32> &&
    std::is_same_v<
        std::decay_t<typename span_const_f32::value_type>,
        std::decay_t<f32>>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param program GLuint
 * \param location GLint
 * \param count GLsizei
 * \param value const GLfloat *
 * \return void
 */
STATICINLINE void program_uniform(
    u32                   program,
    i32                   location,
    span_const_f32 const& value,
    error_check           check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform1fvEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glProgramUniform1fvEXT(
        program,
        location,
        value.size(),
        reinterpret_cast<const GLfloat*>(value.data()));
    detail::error_check("ProgramUniform1fvEXT"sv, check_errors);
}

/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param program GLuint
 * \param location GLint
 * \param v0 GLint
 * \return void
 */
STATICINLINE void program_uniform(
    u32         program,
    i32         location,
    i32         v0,
    error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform1iEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glProgramUniform1iEXT(program, location, v0);
    detail::error_check("ProgramUniform1iEXT"sv, check_errors);
}

template<class span_const_i32>
requires(
    concepts::span<span_const_i32> &&
    std::is_same_v<
        std::decay_t<typename span_const_i32::value_type>,
        std::decay_t<i32>>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param program GLuint
 * \param location GLint
 * \param count GLsizei
 * \param value const GLint *
 * \return void
 */
STATICINLINE void program_uniform(
    u32                   program,
    i32                   location,
    span_const_i32 const& value,
    error_check           check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform1ivEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glProgramUniform1ivEXT(
        program,
        location,
        value.size(),
        reinterpret_cast<const GLint*>(value.data()));
    detail::error_check("ProgramUniform1ivEXT"sv, check_errors);
}

template<class vec_2_f32>
requires(concepts::vector<vec_2_f32, f32, 2>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param program GLuint
 * \param location GLint
 * \param v0 GLfloat
 * \param v1 GLfloat
 * \return void
 */
STATICINLINE void program_uniform(
    u32              program,
    i32              location,
    vec_2_f32 const& v0,
    error_check      check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform2fEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glProgramUniform2fEXT(program, location, v0[0], v0[1]);
    detail::error_check("ProgramUniform2fEXT"sv, check_errors);
}

template<class span_const_vec_2_f32>
requires(
    concepts::span<span_const_vec_2_f32> &&
    concepts::vector<typename span_const_vec_2_f32::value_type, f32, 2>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param program GLuint
 * \param location GLint
 * \param count GLsizei
 * \param value const GLfloat *
 * \return void
 */
STATICINLINE void program_uniform(
    u32                         program,
    i32                         location,
    span_const_vec_2_f32 const& value,
    error_check                 check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform2fvEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glProgramUniform2fvEXT(
        program,
        location,
        value.size(),
        reinterpret_cast<const GLfloat*>(value.data()));
    detail::error_check("ProgramUniform2fvEXT"sv, check_errors);
}

template<class vec_2_i32>
requires(concepts::vector<vec_2_i32, i32, 2>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param program GLuint
 * \param location GLint
 * \param v0 GLint
 * \param v1 GLint
 * \return void
 */
STATICINLINE void program_uniform(
    u32              program,
    i32              location,
    vec_2_i32 const& v0,
    error_check      check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform2iEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glProgramUniform2iEXT(program, location, v0[0], v0[1]);
    detail::error_check("ProgramUniform2iEXT"sv, check_errors);
}

template<class span_const_vec_2_i32>
requires(
    concepts::span<span_const_vec_2_i32> &&
    concepts::vector<typename span_const_vec_2_i32::value_type, i32, 2>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param program GLuint
 * \param location GLint
 * \param count GLsizei
 * \param value const GLint *
 * \return void
 */
STATICINLINE void program_uniform(
    u32                         program,
    i32                         location,
    span_const_vec_2_i32 const& value,
    error_check                 check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform2ivEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glProgramUniform2ivEXT(
        program,
        location,
        value.size(),
        reinterpret_cast<const GLint*>(value.data()));
    detail::error_check("ProgramUniform2ivEXT"sv, check_errors);
}

template<class vec_3_f32>
requires(concepts::vector<vec_3_f32, f32, 3>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param program GLuint
 * \param location GLint
 * \param v0 GLfloat
 * \param v1 GLfloat
 * \param v2 GLfloat
 * \return void
 */
STATICINLINE void program_uniform(
    u32              program,
    i32              location,
    vec_3_f32 const& v0,
    error_check      check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform3fEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glProgramUniform3fEXT(program, location, v0[0], v0[1], v0[2]);
    detail::error_check("ProgramUniform3fEXT"sv, check_errors);
}

template<class span_const_vec_3_f32>
requires(
    concepts::span<span_const_vec_3_f32> &&
    concepts::vector<typename span_const_vec_3_f32::value_type, f32, 3>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param program GLuint
 * \param location GLint
 * \param count GLsizei
 * \param value const GLfloat *
 * \return void
 */
STATICINLINE void program_uniform(
    u32                         program,
    i32                         location,
    span_const_vec_3_f32 const& value,
    error_check                 check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform3fvEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glProgramUniform3fvEXT(
        program,
        location,
        value.size(),
        reinterpret_cast<const GLfloat*>(value.data()));
    detail::error_check("ProgramUniform3fvEXT"sv, check_errors);
}

template<class vec_3_i32>
requires(concepts::vector<vec_3_i32, i32, 3>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param program GLuint
 * \param location GLint
 * \param v0 GLint
 * \param v1 GLint
 * \param v2 GLint
 * \return void
 */
STATICINLINE void program_uniform(
    u32              program,
    i32              location,
    vec_3_i32 const& v0,
    error_check      check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform3iEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glProgramUniform3iEXT(program, location, v0[0], v0[1], v0[2]);
    detail::error_check("ProgramUniform3iEXT"sv, check_errors);
}

template<class span_const_vec_3_i32>
requires(
    concepts::span<span_const_vec_3_i32> &&
    concepts::vector<typename span_const_vec_3_i32::value_type, i32, 3>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param program GLuint
 * \param location GLint
 * \param count GLsizei
 * \param value const GLint *
 * \return void
 */
STATICINLINE void program_uniform(
    u32                         program,
    i32                         location,
    span_const_vec_3_i32 const& value,
    error_check                 check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform3ivEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glProgramUniform3ivEXT(
        program,
        location,
        value.size(),
        reinterpret_cast<const GLint*>(value.data()));
    detail::error_check("ProgramUniform3ivEXT"sv, check_errors);
}

template<class vec_4_f32>
requires(concepts::vector<vec_4_f32, f32, 4>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param program GLuint
 * \param location GLint
 * \param v0 GLfloat
 * \param v1 GLfloat
 * \param v2 GLfloat
 * \param v3 GLfloat
 * \return void
 */
STATICINLINE void program_uniform(
    u32              program,
    i32              location,
    vec_4_f32 const& v0,
    error_check      check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform4fEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glProgramUniform4fEXT(program, location, v0[0], v0[1], v0[2], v0[3]);
    detail::error_check("ProgramUniform4fEXT"sv, check_errors);
}

template<class span_const_vec_4_f32>
requires(
    concepts::span<span_const_vec_4_f32> &&
    concepts::vector<typename span_const_vec_4_f32::value_type, f32, 4>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param program GLuint
 * \param location GLint
 * \param count GLsizei
 * \param value const GLfloat *
 * \return void
 */
STATICINLINE void program_uniform(
    u32                         program,
    i32                         location,
    span_const_vec_4_f32 const& value,
    error_check                 check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform4fvEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glProgramUniform4fvEXT(
        program,
        location,
        value.size(),
        reinterpret_cast<const GLfloat*>(value.data()));
    detail::error_check("ProgramUniform4fvEXT"sv, check_errors);
}

template<class vec_4_i32>
requires(concepts::vector<vec_4_i32, i32, 4>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param program GLuint
 * \param location GLint
 * \param v0 GLint
 * \param v1 GLint
 * \param v2 GLint
 * \param v3 GLint
 * \return void
 */
STATICINLINE void program_uniform(
    u32              program,
    i32              location,
    vec_4_i32 const& v0,
    error_check      check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform4iEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glProgramUniform4iEXT(program, location, v0[0], v0[1], v0[2], v0[3]);
    detail::error_check("ProgramUniform4iEXT"sv, check_errors);
}

template<class span_const_vec_4_i32>
requires(
    concepts::span<span_const_vec_4_i32> &&
    concepts::vector<typename span_const_vec_4_i32::value_type, i32, 4>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param program GLuint
 * \param location GLint
 * \param count GLsizei
 * \param value const GLint *
 * \return void
 */
STATICINLINE void program_uniform(
    u32                         program,
    i32                         location,
    span_const_vec_4_i32 const& value,
    error_check                 check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform4ivEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glProgramUniform4ivEXT(
        program,
        location,
        value.size(),
        reinterpret_cast<const GLint*>(value.data()));
    detail::error_check("ProgramUniform4ivEXT"sv, check_errors);
}

template<class span_const_mat_2x2_f32>
requires(
    concepts::span<span_const_mat_2x2_f32> &&
    concepts::matrix<typename span_const_mat_2x2_f32::value_type, f32, 2, 2>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param program GLuint
 * \param location GLint
 * \param count GLsizei
 * \param transpose GLboolean
 * \param value const GLfloat *
 * \return void
 */
STATICINLINE void program_uniform(
    u32                           program,
    i32                           location,
    bool                          transpose,
    span_const_mat_2x2_f32 const& value,
    error_check                   check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniformMatrix2fvEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glProgramUniformMatrix2fvEXT(
        program,
        location,
        value.size(),
        transpose,
        reinterpret_cast<const GLfloat*>(value.data()));
    detail::error_check("ProgramUniformMatrix2fvEXT"sv, check_errors);
}

template<class span_const_mat_3x3_f32>
requires(
    concepts::span<span_const_mat_3x3_f32> &&
    concepts::matrix<typename span_const_mat_3x3_f32::value_type, f32, 3, 3>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param program GLuint
 * \param location GLint
 * \param count GLsizei
 * \param transpose GLboolean
 * \param value const GLfloat *
 * \return void
 */
STATICINLINE void program_uniform(
    u32                           program,
    i32                           location,
    bool                          transpose,
    span_const_mat_3x3_f32 const& value,
    error_check                   check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniformMatrix3fvEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glProgramUniformMatrix3fvEXT(
        program,
        location,
        value.size(),
        transpose,
        reinterpret_cast<const GLfloat*>(value.data()));
    detail::error_check("ProgramUniformMatrix3fvEXT"sv, check_errors);
}

template<class span_const_mat_4x4_f32>
requires(
    concepts::span<span_const_mat_4x4_f32> &&
    concepts::matrix<typename span_const_mat_4x4_f32::value_type, f32, 4, 4>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param program GLuint
 * \param location GLint
 * \param count GLsizei
 * \param transpose GLboolean
 * \param value const GLfloat *
 * \return void
 */
STATICINLINE void program_uniform(
    u32                           program,
    i32                           location,
    bool                          transpose,
    span_const_mat_4x4_f32 const& value,
    error_check                   check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniformMatrix4fvEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glProgramUniformMatrix4fvEXT(
        program,
        location,
        value.size(),
        transpose,
        reinterpret_cast<const GLfloat*>(value.data()));
    detail::error_check("ProgramUniformMatrix4fvEXT"sv, check_errors);
}

template<class span_const_mat_2x3_f32>
requires(
    concepts::span<span_const_mat_2x3_f32> &&
    concepts::matrix<typename span_const_mat_2x3_f32::value_type, f32, 2, 3>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param program GLuint
 * \param location GLint
 * \param count GLsizei
 * \param transpose GLboolean
 * \param value const GLfloat *
 * \return void
 */
STATICINLINE void program_uniform(
    u32                           program,
    i32                           location,
    bool                          transpose,
    span_const_mat_2x3_f32 const& value,
    error_check                   check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniformMatrix2x3fvEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glProgramUniformMatrix2x3fvEXT(
        program,
        location,
        value.size(),
        transpose,
        reinterpret_cast<const GLfloat*>(value.data()));
    detail::error_check("ProgramUniformMatrix2x3fvEXT"sv, check_errors);
}

template<class span_const_mat_2x4_f32>
requires(
    concepts::span<span_const_mat_2x4_f32> &&
    concepts::matrix<typename span_const_mat_2x4_f32::value_type, f32, 2, 4>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param program GLuint
 * \param location GLint
 * \param count GLsizei
 * \param transpose GLboolean
 * \param value const GLfloat *
 * \return void
 */
STATICINLINE void program_uniform(
    u32                           program,
    i32                           location,
    bool                          transpose,
    span_const_mat_2x4_f32 const& value,
    error_check                   check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniformMatrix2x4fvEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glProgramUniformMatrix2x4fvEXT(
        program,
        location,
        value.size(),
        transpose,
        reinterpret_cast<const GLfloat*>(value.data()));
    detail::error_check("ProgramUniformMatrix2x4fvEXT"sv, check_errors);
}

template<class span_const_mat_3x2_f32>
requires(
    concepts::span<span_const_mat_3x2_f32> &&
    concepts::matrix<typename span_const_mat_3x2_f32::value_type, f32, 3, 2>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param program GLuint
 * \param location GLint
 * \param count GLsizei
 * \param transpose GLboolean
 * \param value const GLfloat *
 * \return void
 */
STATICINLINE void program_uniform(
    u32                           program,
    i32                           location,
    bool                          transpose,
    span_const_mat_3x2_f32 const& value,
    error_check                   check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniformMatrix3x2fvEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glProgramUniformMatrix3x2fvEXT(
        program,
        location,
        value.size(),
        transpose,
        reinterpret_cast<const GLfloat*>(value.data()));
    detail::error_check("ProgramUniformMatrix3x2fvEXT"sv, check_errors);
}

template<class span_const_mat_3x4_f32>
requires(
    concepts::span<span_const_mat_3x4_f32> &&
    concepts::matrix<typename span_const_mat_3x4_f32::value_type, f32, 3, 4>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param program GLuint
 * \param location GLint
 * \param count GLsizei
 * \param transpose GLboolean
 * \param value const GLfloat *
 * \return void
 */
STATICINLINE void program_uniform(
    u32                           program,
    i32                           location,
    bool                          transpose,
    span_const_mat_3x4_f32 const& value,
    error_check                   check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniformMatrix3x4fvEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glProgramUniformMatrix3x4fvEXT(
        program,
        location,
        value.size(),
        transpose,
        reinterpret_cast<const GLfloat*>(value.data()));
    detail::error_check("ProgramUniformMatrix3x4fvEXT"sv, check_errors);
}

template<class span_const_mat_4x2_f32>
requires(
    concepts::span<span_const_mat_4x2_f32> &&
    concepts::matrix<typename span_const_mat_4x2_f32::value_type, f32, 4, 2>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param program GLuint
 * \param location GLint
 * \param count GLsizei
 * \param transpose GLboolean
 * \param value const GLfloat *
 * \return void
 */
STATICINLINE void program_uniform(
    u32                           program,
    i32                           location,
    bool                          transpose,
    span_const_mat_4x2_f32 const& value,
    error_check                   check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniformMatrix4x2fvEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glProgramUniformMatrix4x2fvEXT(
        program,
        location,
        value.size(),
        transpose,
        reinterpret_cast<const GLfloat*>(value.data()));
    detail::error_check("ProgramUniformMatrix4x2fvEXT"sv, check_errors);
}

template<class span_const_mat_4x3_f32>
requires(
    concepts::span<span_const_mat_4x3_f32> &&
    concepts::matrix<typename span_const_mat_4x3_f32::value_type, f32, 4, 3>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param program GLuint
 * \param location GLint
 * \param count GLsizei
 * \param transpose GLboolean
 * \param value const GLfloat *
 * \return void
 */
STATICINLINE void program_uniform(
    u32                           program,
    i32                           location,
    bool                          transpose,
    span_const_mat_4x3_f32 const& value,
    error_check                   check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniformMatrix4x3fvEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glProgramUniformMatrix4x3fvEXT(
        program,
        location,
        value.size(),
        transpose,
        reinterpret_cast<const GLfloat*>(value.data()));
    detail::error_check("ProgramUniformMatrix4x3fvEXT"sv, check_errors);
}

/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param texunit GLenum
 * \param target GLenum
 * \param internalformat GLenum
 * \param buffer GLuint
 * \return void
 */
STATICINLINE void multi_tex_buffer(
    group::texture_unit    texunit,
    group::texture_target  target,
    group::internal_format internalformat,
    u32                    buffer,
    error_check            check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MultiTexBufferEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsBuffer)
            glIsBuffer(buffer);
#endif
    }
    glMultiTexBufferEXT(
        static_cast<GLenum>(texunit),
        static_cast<GLenum>(target),
        static_cast<GLenum>(internalformat),
        buffer);
    detail::error_check("MultiTexBufferEXT"sv, check_errors);
}

/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param texture GLuint
 * \param target GLenum
 * \param internalformat GLenum
 * \param buffer GLuint
 * \return void
 */
STATICINLINE void texture_buffer(
    u32                          texture,
    group::texture_target        target,
    group::sized_internal_format internalformat,
    u32                          buffer,
    error_check                  check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TextureBufferEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsTexture)
            glIsTexture(texture);
#endif
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsBuffer)
            glIsBuffer(buffer);
#endif
    }
    glTextureBufferEXT(
        texture,
        static_cast<GLenum>(target),
        static_cast<GLenum>(internalformat),
        buffer);
    detail::error_check("TextureBufferEXT"sv, check_errors);
}

template<class span_i32>
requires(
    concepts::span<span_i32> && std::is_same_v<
                                    std::decay_t<typename span_i32::value_type>,
                                    std::decay_t<i32>>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param texunit GLenum
 * \param target GLenum
 * \param pname GLenum
 * \param params GLint *
 * \return void
 */
STATICINLINE void get_multi_tex_parameter_iiv(
    group::texture_unit          texunit,
    group::texture_target        target,
    group::get_texture_parameter pname,
    span_i32                     params,
    error_check                  check_errors = error_check::on)
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
    detail::error_check("GetMultiTexParameterIivEXT"sv, check_errors);
}

template<class span_u32>
requires(
    concepts::span<span_u32> && std::is_same_v<
                                    std::decay_t<typename span_u32::value_type>,
                                    std::decay_t<u32>>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param texunit GLenum
 * \param target GLenum
 * \param pname GLenum
 * \param params GLuint *
 * \return void
 */
STATICINLINE void get_multi_tex_parameter_iuiv(
    group::texture_unit          texunit,
    group::texture_target        target,
    group::get_texture_parameter pname,
    span_u32                     params,
    error_check                  check_errors = error_check::on)
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
    detail::error_check("GetMultiTexParameterIuivEXT"sv, check_errors);
}

template<class span_i32>
requires(
    concepts::span<span_i32> && std::is_same_v<
                                    std::decay_t<typename span_i32::value_type>,
                                    std::decay_t<i32>>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param texture GLuint
 * \param target GLenum
 * \param pname GLenum
 * \param params GLint *
 * \return void
 */
STATICINLINE void get_texture_parameter_iiv(
    u32                          texture,
    group::texture_target        target,
    group::get_texture_parameter pname,
    span_i32                     params,
    error_check                  check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetTextureParameterIivEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsTexture)
            glIsTexture(texture);
#endif
    }
    glGetTextureParameterIivEXT(
        texture,
        static_cast<GLenum>(target),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetTextureParameterIivEXT"sv, check_errors);
}

template<class span_u32>
requires(
    concepts::span<span_u32> && std::is_same_v<
                                    std::decay_t<typename span_u32::value_type>,
                                    std::decay_t<u32>>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param texture GLuint
 * \param target GLenum
 * \param pname GLenum
 * \param params GLuint *
 * \return void
 */
STATICINLINE void get_texture_parameter_iuiv(
    u32                          texture,
    group::texture_target        target,
    group::get_texture_parameter pname,
    span_u32                     params,
    error_check                  check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetTextureParameterIuivEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsTexture)
            glIsTexture(texture);
#endif
    }
    glGetTextureParameterIuivEXT(
        texture,
        static_cast<GLenum>(target),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLuint*>(params.data()) : nullptr);
    detail::error_check("GetTextureParameterIuivEXT"sv, check_errors);
}

template<class span_const_i32>
requires(
    concepts::span<span_const_i32> &&
    std::is_same_v<
        std::decay_t<typename span_const_i32::value_type>,
        std::decay_t<i32>>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param texunit GLenum
 * \param target GLenum
 * \param pname GLenum
 * \param params const GLint *
 * \return void
 */
STATICINLINE void multi_tex_parameter_iiv(
    group::texture_unit           texunit,
    group::texture_target         target,
    group::texture_parameter_name pname,
    span_const_i32 const&         params,
    error_check                   check_errors = error_check::on)
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
    detail::error_check("MultiTexParameterIivEXT"sv, check_errors);
}

template<class span_const_u32>
requires(
    concepts::span<span_const_u32> &&
    std::is_same_v<
        std::decay_t<typename span_const_u32::value_type>,
        std::decay_t<u32>>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param texunit GLenum
 * \param target GLenum
 * \param pname GLenum
 * \param params const GLuint *
 * \return void
 */
STATICINLINE void multi_tex_parameter_iuiv(
    group::texture_unit           texunit,
    group::texture_target         target,
    group::texture_parameter_name pname,
    span_const_u32 const&         params,
    error_check                   check_errors = error_check::on)
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
    detail::error_check("MultiTexParameterIuivEXT"sv, check_errors);
}

template<class span_const_i32>
requires(
    concepts::span<span_const_i32> &&
    std::is_same_v<
        std::decay_t<typename span_const_i32::value_type>,
        std::decay_t<i32>>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param texture GLuint
 * \param target GLenum
 * \param pname GLenum
 * \param params const GLint *
 * \return void
 */
STATICINLINE void texture_parameter_iiv(
    u32                           texture,
    group::texture_target         target,
    group::texture_parameter_name pname,
    span_const_i32 const&         params,
    error_check                   check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TextureParameterIivEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsTexture)
            glIsTexture(texture);
#endif
    }
    glTextureParameterIivEXT(
        texture,
        static_cast<GLenum>(target),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<const GLint*>(params.data())
                      : nullptr);
    detail::error_check("TextureParameterIivEXT"sv, check_errors);
}

template<class span_const_u32>
requires(
    concepts::span<span_const_u32> &&
    std::is_same_v<
        std::decay_t<typename span_const_u32::value_type>,
        std::decay_t<u32>>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param texture GLuint
 * \param target GLenum
 * \param pname GLenum
 * \param params const GLuint *
 * \return void
 */
STATICINLINE void texture_parameter_iuiv(
    u32                           texture,
    group::texture_target         target,
    group::texture_parameter_name pname,
    span_const_u32 const&         params,
    error_check                   check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TextureParameterIuivEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsTexture)
            glIsTexture(texture);
#endif
    }
    glTextureParameterIuivEXT(
        texture,
        static_cast<GLenum>(target),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<const GLuint*>(params.data())
                      : nullptr);
    detail::error_check("TextureParameterIuivEXT"sv, check_errors);
}

/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param program GLuint
 * \param location GLint
 * \param v0 GLuint
 * \return void
 */
STATICINLINE void program_uniform(
    u32         program,
    i32         location,
    u32         v0,
    error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform1uiEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glProgramUniform1uiEXT(program, location, v0);
    detail::error_check("ProgramUniform1uiEXT"sv, check_errors);
}

template<class span_const_u32>
requires(
    concepts::span<span_const_u32> &&
    std::is_same_v<
        std::decay_t<typename span_const_u32::value_type>,
        std::decay_t<u32>>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param program GLuint
 * \param location GLint
 * \param count GLsizei
 * \param value const GLuint *
 * \return void
 */
STATICINLINE void program_uniform(
    u32                   program,
    i32                   location,
    span_const_u32 const& value,
    error_check           check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform1uivEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glProgramUniform1uivEXT(
        program,
        location,
        value.size(),
        reinterpret_cast<const GLuint*>(value.data()));
    detail::error_check("ProgramUniform1uivEXT"sv, check_errors);
}

template<class vec_2_u32>
requires(concepts::vector<vec_2_u32, u32, 2>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param program GLuint
 * \param location GLint
 * \param v0 GLuint
 * \param v1 GLuint
 * \return void
 */
STATICINLINE void program_uniform(
    u32              program,
    i32              location,
    vec_2_u32 const& v0,
    error_check      check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform2uiEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glProgramUniform2uiEXT(program, location, v0[0], v0[1]);
    detail::error_check("ProgramUniform2uiEXT"sv, check_errors);
}

template<class span_const_vec_2_u32>
requires(
    concepts::span<span_const_vec_2_u32> &&
    concepts::vector<typename span_const_vec_2_u32::value_type, u32, 2>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param program GLuint
 * \param location GLint
 * \param count GLsizei
 * \param value const GLuint *
 * \return void
 */
STATICINLINE void program_uniform(
    u32                         program,
    i32                         location,
    span_const_vec_2_u32 const& value,
    error_check                 check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform2uivEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glProgramUniform2uivEXT(
        program,
        location,
        value.size(),
        reinterpret_cast<const GLuint*>(value.data()));
    detail::error_check("ProgramUniform2uivEXT"sv, check_errors);
}

template<class vec_3_u32>
requires(concepts::vector<vec_3_u32, u32, 3>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param program GLuint
 * \param location GLint
 * \param v0 GLuint
 * \param v1 GLuint
 * \param v2 GLuint
 * \return void
 */
STATICINLINE void program_uniform(
    u32              program,
    i32              location,
    vec_3_u32 const& v0,
    error_check      check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform3uiEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glProgramUniform3uiEXT(program, location, v0[0], v0[1], v0[2]);
    detail::error_check("ProgramUniform3uiEXT"sv, check_errors);
}

template<class span_const_vec_3_u32>
requires(
    concepts::span<span_const_vec_3_u32> &&
    concepts::vector<typename span_const_vec_3_u32::value_type, u32, 3>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param program GLuint
 * \param location GLint
 * \param count GLsizei
 * \param value const GLuint *
 * \return void
 */
STATICINLINE void program_uniform(
    u32                         program,
    i32                         location,
    span_const_vec_3_u32 const& value,
    error_check                 check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform3uivEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glProgramUniform3uivEXT(
        program,
        location,
        value.size(),
        reinterpret_cast<const GLuint*>(value.data()));
    detail::error_check("ProgramUniform3uivEXT"sv, check_errors);
}

template<class vec_4_u32>
requires(concepts::vector<vec_4_u32, u32, 4>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param program GLuint
 * \param location GLint
 * \param v0 GLuint
 * \param v1 GLuint
 * \param v2 GLuint
 * \param v3 GLuint
 * \return void
 */
STATICINLINE void program_uniform(
    u32              program,
    i32              location,
    vec_4_u32 const& v0,
    error_check      check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform4uiEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glProgramUniform4uiEXT(program, location, v0[0], v0[1], v0[2], v0[3]);
    detail::error_check("ProgramUniform4uiEXT"sv, check_errors);
}

template<class span_const_vec_4_u32>
requires(
    concepts::span<span_const_vec_4_u32> &&
    concepts::vector<typename span_const_vec_4_u32::value_type, u32, 4>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param program GLuint
 * \param location GLint
 * \param count GLsizei
 * \param value const GLuint *
 * \return void
 */
STATICINLINE void program_uniform(
    u32                         program,
    i32                         location,
    span_const_vec_4_u32 const& value,
    error_check                 check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform4uivEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glProgramUniform4uivEXT(
        program,
        location,
        value.size(),
        reinterpret_cast<const GLuint*>(value.data()));
    detail::error_check("ProgramUniform4uivEXT"sv, check_errors);
}

template<class span_const_vec_4_f32>
requires(
    concepts::span<span_const_vec_4_f32> &&
    concepts::vector<typename span_const_vec_4_f32::value_type, f32, 4>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param program GLuint
 * \param target GLenum
 * \param index GLuint
 * \param count GLsizei
 * \param params const GLfloat *
 * \return void
 */
STATICINLINE void named_program_local_parameters4fv(
    u32                         program,
    group::program_target       target,
    u32                         index,
    span_const_vec_4_f32 const& params,
    error_check                 check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(NamedProgramLocalParameters4fvEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glNamedProgramLocalParameters4fvEXT(
        program,
        static_cast<GLenum>(target),
        index,
        params.size(),
        reinterpret_cast<const GLfloat*>(params.data()));
    detail::error_check("NamedProgramLocalParameters4fvEXT"sv, check_errors);
}

template<class span_i32>
requires(
    concepts::span<span_i32> && std::is_same_v<
                                    std::decay_t<typename span_i32::value_type>,
                                    std::decay_t<i32>>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param program GLuint
 * \param target GLenum
 * \param index GLuint
 * \param params GLint *
 * \return void
 */
STATICINLINE void get_named_program_local_parameter_iiv(
    u32                   program,
    group::program_target target,
    u32                   index,
    span_i32              params,
    error_check           check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetNamedProgramLocalParameterIivEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glGetNamedProgramLocalParameterIivEXT(
        program,
        static_cast<GLenum>(target),
        index,
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetNamedProgramLocalParameterIivEXT"sv, check_errors);
}

template<class span_u32>
requires(
    concepts::span<span_u32> && std::is_same_v<
                                    std::decay_t<typename span_u32::value_type>,
                                    std::decay_t<u32>>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param program GLuint
 * \param target GLenum
 * \param index GLuint
 * \param params GLuint *
 * \return void
 */
STATICINLINE void get_named_program_local_parameter_iuiv(
    u32                   program,
    group::program_target target,
    u32                   index,
    span_u32              params,
    error_check           check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetNamedProgramLocalParameterIuivEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glGetNamedProgramLocalParameterIuivEXT(
        program,
        static_cast<GLenum>(target),
        index,
        params.size() ? reinterpret_cast<GLuint*>(params.data()) : nullptr);
    detail::error_check("GetNamedProgramLocalParameterIuivEXT"sv, check_errors);
}

template<class vec_4_i32>
requires(concepts::vector<vec_4_i32, i32, 4>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param program GLuint
 * \param target GLenum
 * \param index GLuint
 * \param x GLint
 * \param y GLint
 * \param z GLint
 * \param w GLint
 * \return void
 */
STATICINLINE void named_program_local_parameter_i4i(
    u32                   program,
    group::program_target target,
    u32                   index,
    vec_4_i32 const&      x,
    error_check           check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(NamedProgramLocalParameterI4iEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glNamedProgramLocalParameterI4iEXT(
        program, static_cast<GLenum>(target), index, x[0], x[1], x[2], x[3]);
    detail::error_check("NamedProgramLocalParameterI4iEXT"sv, check_errors);
}

template<class span_const_vec_4_i32>
requires(
    concepts::span<span_const_vec_4_i32> &&
    concepts::vector<typename span_const_vec_4_i32::value_type, i32, 4>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param program GLuint
 * \param target GLenum
 * \param index GLuint
 * \param params const GLint *
 * \return void
 */
STATICINLINE void named_program_local_parameter_i4iv(
    u32                         program,
    group::program_target       target,
    u32                         index,
    span_const_vec_4_i32 const& params,
    error_check                 check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(NamedProgramLocalParameterI4ivEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glNamedProgramLocalParameterI4ivEXT(
        program,
        static_cast<GLenum>(target),
        index,
        reinterpret_cast<const GLint*>(params.data()));
    detail::error_check("NamedProgramLocalParameterI4ivEXT"sv, check_errors);
}

template<class vec_4_u32>
requires(concepts::vector<vec_4_u32, u32, 4>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param program GLuint
 * \param target GLenum
 * \param index GLuint
 * \param x GLuint
 * \param y GLuint
 * \param z GLuint
 * \param w GLuint
 * \return void
 */
STATICINLINE void named_program_local_parameter_i4ui(
    u32                   program,
    group::program_target target,
    u32                   index,
    vec_4_u32 const&      x,
    error_check           check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(NamedProgramLocalParameterI4uiEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glNamedProgramLocalParameterI4uiEXT(
        program, static_cast<GLenum>(target), index, x[0], x[1], x[2], x[3]);
    detail::error_check("NamedProgramLocalParameterI4uiEXT"sv, check_errors);
}

template<class span_const_vec_4_u32>
requires(
    concepts::span<span_const_vec_4_u32> &&
    concepts::vector<typename span_const_vec_4_u32::value_type, u32, 4>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param program GLuint
 * \param target GLenum
 * \param index GLuint
 * \param params const GLuint *
 * \return void
 */
STATICINLINE void named_program_local_parameter_i4uiv(
    u32                         program,
    group::program_target       target,
    u32                         index,
    span_const_vec_4_u32 const& params,
    error_check                 check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(NamedProgramLocalParameterI4uivEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glNamedProgramLocalParameterI4uivEXT(
        program,
        static_cast<GLenum>(target),
        index,
        reinterpret_cast<const GLuint*>(params.data()));
    detail::error_check("NamedProgramLocalParameterI4uivEXT"sv, check_errors);
}

template<class span_const_vec_4_i32>
requires(
    concepts::span<span_const_vec_4_i32> &&
    concepts::vector<typename span_const_vec_4_i32::value_type, i32, 4>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param program GLuint
 * \param target GLenum
 * \param index GLuint
 * \param count GLsizei
 * \param params const GLint *
 * \return void
 */
STATICINLINE void named_program_local_parameters_i4iv(
    u32                         program,
    group::program_target       target,
    u32                         index,
    span_const_vec_4_i32 const& params,
    error_check                 check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(NamedProgramLocalParametersI4ivEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glNamedProgramLocalParametersI4ivEXT(
        program,
        static_cast<GLenum>(target),
        index,
        params.size(),
        reinterpret_cast<const GLint*>(params.data()));
    detail::error_check("NamedProgramLocalParametersI4ivEXT"sv, check_errors);
}

template<class span_const_vec_4_u32>
requires(
    concepts::span<span_const_vec_4_u32> &&
    concepts::vector<typename span_const_vec_4_u32::value_type, u32, 4>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param program GLuint
 * \param target GLenum
 * \param index GLuint
 * \param count GLsizei
 * \param params const GLuint *
 * \return void
 */
STATICINLINE void named_program_local_parameters_i4uiv(
    u32                         program,
    group::program_target       target,
    u32                         index,
    span_const_vec_4_u32 const& params,
    error_check                 check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(NamedProgramLocalParametersI4uivEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glNamedProgramLocalParametersI4uivEXT(
        program,
        static_cast<GLenum>(target),
        index,
        params.size(),
        reinterpret_cast<const GLuint*>(params.data()));
    detail::error_check("NamedProgramLocalParametersI4uivEXT"sv, check_errors);
}

/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param array GLenum
 * \param index GLuint
 * \return void
 */
STATICINLINE void disable_client_statei(
    group::enable_cap array,
    u32               index,
    error_check       check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DisableClientStateiEXT)
    }
    glDisableClientStateiEXT(static_cast<GLenum>(array), index);
    detail::error_check("DisableClientStateiEXT"sv, check_errors);
}

/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param array GLenum
 * \param index GLuint
 * \return void
 */
STATICINLINE void enable_client_statei(
    group::enable_cap array,
    u32               index,
    error_check       check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(EnableClientStateiEXT)
    }
    glEnableClientStateiEXT(static_cast<GLenum>(array), index);
    detail::error_check("EnableClientStateiEXT"sv, check_errors);
}

template<class span_f64>
requires(
    concepts::span<span_f64> && std::is_same_v<
                                    std::decay_t<typename span_f64::value_type>,
                                    std::decay_t<f64>>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param pname GLenum
 * \param index GLuint
 * \param params GLdouble *
 * \return void
 */
STATICINLINE void get_doublei_v(
    group::get_prop pname,
    u32             index,
    span_f64        params,
    error_check     check_errors = error_check::on)
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
    detail::error_check("GetDoublei_vEXT"sv, check_errors);
}

template<class span_f32>
requires(
    concepts::span<span_f32> && std::is_same_v<
                                    std::decay_t<typename span_f32::value_type>,
                                    std::decay_t<f32>>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param pname GLenum
 * \param index GLuint
 * \param params GLfloat *
 * \return void
 */
STATICINLINE void get_floati_v(
    group::get_prop pname,
    u32             index,
    span_f32        params,
    error_check     check_errors = error_check::on)
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
    detail::error_check("GetFloati_vEXT"sv, check_errors);
}

template<class span_void>
requires(concepts::span<span_void>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param pname GLenum
 * \param index GLuint
 * \param params void **
 * \return void
 */
STATICINLINE void get_pointeri_v(
    GLenum      pname,
    u32         index,
    span_void   params,
    error_check check_errors = error_check::on)
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
    detail::error_check("GetPointeri_vEXT"sv, check_errors);
}

template<class span_f64>
requires(
    concepts::span<span_f64> && std::is_same_v<
                                    std::decay_t<typename span_f64::value_type>,
                                    std::decay_t<f64>>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param program GLuint
 * \param target GLenum
 * \param index GLuint
 * \param params GLdouble *
 * \return void
 */
STATICINLINE void get_named_program_local_parameter(
    u32                   program,
    group::program_target target,
    u32                   index,
    span_f64              params,
    error_check           check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetNamedProgramLocalParameterdvEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glGetNamedProgramLocalParameterdvEXT(
        program,
        static_cast<GLenum>(target),
        index,
        params.size() ? reinterpret_cast<GLdouble*>(params.data()) : nullptr);
    detail::error_check("GetNamedProgramLocalParameterdvEXT"sv, check_errors);
}

template<class span_f32>
requires(
    concepts::span<span_f32> && std::is_same_v<
                                    std::decay_t<typename span_f32::value_type>,
                                    std::decay_t<f32>>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param program GLuint
 * \param target GLenum
 * \param index GLuint
 * \param params GLfloat *
 * \return void
 */
STATICINLINE void get_named_program_local_parameter(
    u32                   program,
    group::program_target target,
    u32                   index,
    span_f32              params,
    error_check           check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetNamedProgramLocalParameterfvEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glGetNamedProgramLocalParameterfvEXT(
        program,
        static_cast<GLenum>(target),
        index,
        params.size() ? reinterpret_cast<GLfloat*>(params.data()) : nullptr);
    detail::error_check("GetNamedProgramLocalParameterfvEXT"sv, check_errors);
}

template<class span_void>
requires(concepts::span<span_void>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param program GLuint
 * \param target GLenum
 * \param pname GLenum
 * \param string void *
 * \return void
 */
STATICINLINE void get_named_program_string(
    u32                            program,
    group::program_target          target,
    group::program_string_property pname,
    span_void                      string,
    error_check                    check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetNamedProgramStringEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glGetNamedProgramStringEXT(
        program,
        static_cast<GLenum>(target),
        static_cast<GLenum>(pname),
        string.size() ? reinterpret_cast<void*>(string.data()) : nullptr);
    detail::error_check("GetNamedProgramStringEXT"sv, check_errors);
}

/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param program GLuint
 * \param target GLenum
 * \param pname GLenum
 * \param params GLint *
 * \return void
 */
STATICINLINE void get_named_programiv(
    u32                         program,
    group::program_target       target,
    group::program_property_arb pname,
    i32&                        params,
    error_check                 check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetNamedProgramivEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glGetNamedProgramivEXT(
        program,
        static_cast<GLenum>(target),
        static_cast<GLenum>(pname),
        &params);
    detail::error_check("GetNamedProgramivEXT"sv, check_errors);
}

template<class vec_4_f64>
requires(concepts::vector<vec_4_f64, f64, 4>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param program GLuint
 * \param target GLenum
 * \param index GLuint
 * \param x GLdouble
 * \param y GLdouble
 * \param z GLdouble
 * \param w GLdouble
 * \return void
 */
STATICINLINE void named_program_local_parameter4d(
    u32                   program,
    group::program_target target,
    u32                   index,
    vec_4_f64 const&      x,
    error_check           check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(NamedProgramLocalParameter4dEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glNamedProgramLocalParameter4dEXT(
        program, static_cast<GLenum>(target), index, x[0], x[1], x[2], x[3]);
    detail::error_check("NamedProgramLocalParameter4dEXT"sv, check_errors);
}

template<class span_const_vec_4_f64>
requires(
    concepts::span<span_const_vec_4_f64> &&
    concepts::vector<typename span_const_vec_4_f64::value_type, f64, 4>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param program GLuint
 * \param target GLenum
 * \param index GLuint
 * \param params const GLdouble *
 * \return void
 */
STATICINLINE void named_program_local_parameter4dv(
    u32                         program,
    group::program_target       target,
    u32                         index,
    span_const_vec_4_f64 const& params,
    error_check                 check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(NamedProgramLocalParameter4dvEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glNamedProgramLocalParameter4dvEXT(
        program,
        static_cast<GLenum>(target),
        index,
        reinterpret_cast<const GLdouble*>(params.data()));
    detail::error_check("NamedProgramLocalParameter4dvEXT"sv, check_errors);
}

template<class vec_4_f32>
requires(concepts::vector<vec_4_f32, f32, 4>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param program GLuint
 * \param target GLenum
 * \param index GLuint
 * \param x GLfloat
 * \param y GLfloat
 * \param z GLfloat
 * \param w GLfloat
 * \return void
 */
STATICINLINE void named_program_local_parameter4f(
    u32                   program,
    group::program_target target,
    u32                   index,
    vec_4_f32 const&      x,
    error_check           check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(NamedProgramLocalParameter4fEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glNamedProgramLocalParameter4fEXT(
        program, static_cast<GLenum>(target), index, x[0], x[1], x[2], x[3]);
    detail::error_check("NamedProgramLocalParameter4fEXT"sv, check_errors);
}

template<class span_const_vec_4_f32>
requires(
    concepts::span<span_const_vec_4_f32> &&
    concepts::vector<typename span_const_vec_4_f32::value_type, f32, 4>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param program GLuint
 * \param target GLenum
 * \param index GLuint
 * \param params const GLfloat *
 * \return void
 */
STATICINLINE void named_program_local_parameter4fv(
    u32                         program,
    group::program_target       target,
    u32                         index,
    span_const_vec_4_f32 const& params,
    error_check                 check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(NamedProgramLocalParameter4fvEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glNamedProgramLocalParameter4fvEXT(
        program,
        static_cast<GLenum>(target),
        index,
        reinterpret_cast<const GLfloat*>(params.data()));
    detail::error_check("NamedProgramLocalParameter4fvEXT"sv, check_errors);
}

template<class span_const_void>
requires(concepts::span<span_const_void>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param program GLuint
 * \param target GLenum
 * \param format GLenum
 * \param len GLsizei
 * \param string const void *
 * \return void
 */
STATICINLINE void named_program_string(
    u32                    program,
    group::program_target  target,
    group::program_format  format,
    span_const_void const& string,
    error_check            check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(NamedProgramStringEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glNamedProgramStringEXT(
        program,
        static_cast<GLenum>(target),
        static_cast<GLenum>(format),
        string.size() *
            sizeof(typename std::decay_t<span_const_void const&>::value_type),
        string.size() ? reinterpret_cast<const void*>(string.data()) : nullptr);
    detail::error_check("NamedProgramStringEXT"sv, check_errors);
}

template<class span_i32>
requires(
    concepts::span<span_i32> && std::is_same_v<
                                    std::decay_t<typename span_i32::value_type>,
                                    std::decay_t<i32>>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param renderbuffer GLuint
 * \param pname GLenum
 * \param params GLint *
 * \return void
 */
STATICINLINE void get_named_renderbuffer_parameter(
    u32                                renderbuffer,
    group::renderbuffer_parameter_name pname,
    span_i32                           params,
    error_check                        check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetNamedRenderbufferParameterivEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsRenderbuffer)
            glIsRenderbuffer(renderbuffer);
#endif
    }
    glGetNamedRenderbufferParameterivEXT(
        renderbuffer,
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetNamedRenderbufferParameterivEXT"sv, check_errors);
}

template<class size_2_i32>
requires(concepts::size_2d<size_2_i32, i32>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param renderbuffer GLuint
 * \param internalformat GLenum
 * \param width GLsizei
 * \param height GLsizei
 * \return void
 */
STATICINLINE void named_renderbuffer_storage(
    u32                    renderbuffer,
    group::internal_format internalformat,
    size_2_i32 const&      width,
    error_check            check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(NamedRenderbufferStorageEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsRenderbuffer)
            glIsRenderbuffer(renderbuffer);
#endif
    }
    glNamedRenderbufferStorageEXT(
        renderbuffer, static_cast<GLenum>(internalformat), width[0], width[1]);
    detail::error_check("NamedRenderbufferStorageEXT"sv, check_errors);
}

template<class size_2_i32>
requires(concepts::size_2d<size_2_i32, i32>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param renderbuffer GLuint
 * \param samples GLsizei
 * \param internalformat GLenum
 * \param width GLsizei
 * \param height GLsizei
 * \return void
 */
STATICINLINE void named_renderbuffer_storage_multisample(
    u32                    renderbuffer,
    i32                    samples,
    group::internal_format internalformat,
    size_2_i32 const&      width,
    error_check            check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(NamedRenderbufferStorageMultisampleEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsRenderbuffer)
            glIsRenderbuffer(renderbuffer);
#endif
    }
    glNamedRenderbufferStorageMultisampleEXT(
        renderbuffer,
        samples,
        static_cast<GLenum>(internalformat),
        width[0],
        width[1]);
    detail::error_check(
        "NamedRenderbufferStorageMultisampleEXT"sv, check_errors);
}

template<class size_2_i32>
requires(concepts::size_2d<size_2_i32, i32>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param renderbuffer GLuint
 * \param coverageSamples GLsizei
 * \param colorSamples GLsizei
 * \param internalformat GLenum
 * \param width GLsizei
 * \param height GLsizei
 * \return void
 */
STATICINLINE void named_renderbuffer_storage_multisample_coverage(
    u32                    renderbuffer,
    i32                    coverageSamples,
    i32                    colorSamples,
    group::internal_format internalformat,
    size_2_i32 const&      width,
    error_check            check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(NamedRenderbufferStorageMultisampleCoverageEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsRenderbuffer)
            glIsRenderbuffer(renderbuffer);
#endif
    }
    glNamedRenderbufferStorageMultisampleCoverageEXT(
        renderbuffer,
        coverageSamples,
        colorSamples,
        static_cast<GLenum>(internalformat),
        width[0],
        width[1]);
    detail::error_check(
        "NamedRenderbufferStorageMultisampleCoverageEXT"sv, check_errors);
}

/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param framebuffer GLuint
 * \param target GLenum
 * \return FramebufferStatus
 */
STATICINLINE group::framebuffer_status check_named_framebuffer_status(
    u32                       framebuffer,
    group::framebuffer_target target,
    error_check               check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CheckNamedFramebufferStatusEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsFramebuffer)
            glIsFramebuffer(framebuffer);
#endif
    }
    auto out = glCheckNamedFramebufferStatusEXT(
        framebuffer, static_cast<GLenum>(target));
    detail::error_check("CheckNamedFramebufferStatusEXT"sv, check_errors);
    return static_cast<group::framebuffer_status>(out);
}

template<class span_i32>
requires(
    concepts::span<span_i32> && std::is_same_v<
                                    std::decay_t<typename span_i32::value_type>,
                                    std::decay_t<i32>>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param framebuffer GLuint
 * \param attachment GLenum
 * \param pname GLenum
 * \param params GLint *
 * \return void
 */
STATICINLINE void get_named_framebuffer_attachment_parameter(
    u32                                          framebuffer,
    group::framebuffer_attachment                attachment,
    group::framebuffer_attachment_parameter_name pname,
    span_i32                                     params,
    error_check                                  check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetNamedFramebufferAttachmentParameterivEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsFramebuffer)
            glIsFramebuffer(framebuffer);
#endif
    }
    glGetNamedFramebufferAttachmentParameterivEXT(
        framebuffer,
        static_cast<GLenum>(attachment),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check(
        "GetNamedFramebufferAttachmentParameterivEXT"sv, check_errors);
}

/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param framebuffer GLuint
 * \param attachment GLenum
 * \param renderbuffertarget GLenum
 * \param renderbuffer GLuint
 * \return void
 */
STATICINLINE void named_framebuffer_renderbuffer(
    u32                           framebuffer,
    group::framebuffer_attachment attachment,
    group::renderbuffer_target    renderbuffertarget,
    u32                           renderbuffer,
    error_check                   check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(NamedFramebufferRenderbufferEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsFramebuffer)
            glIsFramebuffer(framebuffer);
#endif
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsRenderbuffer)
            glIsRenderbuffer(renderbuffer);
#endif
    }
    glNamedFramebufferRenderbufferEXT(
        framebuffer,
        static_cast<GLenum>(attachment),
        static_cast<GLenum>(renderbuffertarget),
        renderbuffer);
    detail::error_check("NamedFramebufferRenderbufferEXT"sv, check_errors);
}

/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param framebuffer GLuint
 * \param attachment GLenum
 * \param textarget GLenum
 * \param texture GLuint
 * \param level GLint
 * \return void
 */
STATICINLINE void named_framebuffer_texture_1d(
    u32                           framebuffer,
    group::framebuffer_attachment attachment,
    group::texture_target         textarget,
    u32                           texture,
    i32                           level,
    error_check                   check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(NamedFramebufferTexture1DEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsFramebuffer)
            glIsFramebuffer(framebuffer);
#endif
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsTexture)
            glIsTexture(texture);
#endif
    }
    glNamedFramebufferTexture1DEXT(
        framebuffer,
        static_cast<GLenum>(attachment),
        static_cast<GLenum>(textarget),
        texture,
        level);
    detail::error_check("NamedFramebufferTexture1DEXT"sv, check_errors);
}

/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param framebuffer GLuint
 * \param attachment GLenum
 * \param textarget GLenum
 * \param texture GLuint
 * \param level GLint
 * \return void
 */
STATICINLINE void named_framebuffer_texture_2d(
    u32                           framebuffer,
    group::framebuffer_attachment attachment,
    group::texture_target         textarget,
    u32                           texture,
    i32                           level,
    error_check                   check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(NamedFramebufferTexture2DEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsFramebuffer)
            glIsFramebuffer(framebuffer);
#endif
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsTexture)
            glIsTexture(texture);
#endif
    }
    glNamedFramebufferTexture2DEXT(
        framebuffer,
        static_cast<GLenum>(attachment),
        static_cast<GLenum>(textarget),
        texture,
        level);
    detail::error_check("NamedFramebufferTexture2DEXT"sv, check_errors);
}

/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param framebuffer GLuint
 * \param attachment GLenum
 * \param textarget GLenum
 * \param texture GLuint
 * \param level GLint
 * \param zoffset GLint
 * \return void
 */
STATICINLINE void named_framebuffer_texture_3d(
    u32                           framebuffer,
    group::framebuffer_attachment attachment,
    group::texture_target         textarget,
    u32                           texture,
    i32                           level,
    i32                           zoffset,
    error_check                   check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(NamedFramebufferTexture3DEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsFramebuffer)
            glIsFramebuffer(framebuffer);
#endif
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsTexture)
            glIsTexture(texture);
#endif
    }
    glNamedFramebufferTexture3DEXT(
        framebuffer,
        static_cast<GLenum>(attachment),
        static_cast<GLenum>(textarget),
        texture,
        level,
        zoffset);
    detail::error_check("NamedFramebufferTexture3DEXT"sv, check_errors);
}

/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param texunit GLenum
 * \param target GLenum
 * \return void
 */
STATICINLINE void generate_multi_tex_mipmap(
    group::texture_unit   texunit,
    group::texture_target target,
    error_check           check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GenerateMultiTexMipmapEXT)
    }
    glGenerateMultiTexMipmapEXT(
        static_cast<GLenum>(texunit), static_cast<GLenum>(target));
    detail::error_check("GenerateMultiTexMipmapEXT"sv, check_errors);
}

/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param texture GLuint
 * \param target GLenum
 * \return void
 */
STATICINLINE void generate_texture_mipmap(
    u32                   texture,
    group::texture_target target,
    error_check           check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GenerateTextureMipmapEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsTexture)
            glIsTexture(texture);
#endif
    }
    glGenerateTextureMipmapEXT(texture, static_cast<GLenum>(target));
    detail::error_check("GenerateTextureMipmapEXT"sv, check_errors);
}

/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param framebuffer GLuint
 * \param mode GLenum
 * \return void
 */
STATICINLINE void framebuffer_draw_buffer(
    u32                     framebuffer,
    group::draw_buffer_mode mode,
    error_check             check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(FramebufferDrawBufferEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsFramebuffer)
            glIsFramebuffer(framebuffer);
#endif
    }
    glFramebufferDrawBufferEXT(framebuffer, static_cast<GLenum>(mode));
    detail::error_check("FramebufferDrawBufferEXT"sv, check_errors);
}

template<class span_const_draw_buffer_mode>
requires(
    concepts::span<span_const_draw_buffer_mode> &&
    std::is_same_v<
        std::decay_t<typename span_const_draw_buffer_mode::value_type>,
        std::decay_t<group::draw_buffer_mode>>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param framebuffer GLuint
 * \param n GLsizei
 * \param bufs const GLenum *
 * \return void
 */
STATICINLINE void framebuffer_draw_buffers(
    u32                                framebuffer,
    span_const_draw_buffer_mode const& bufs,
    error_check                        check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(FramebufferDrawBuffersEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsFramebuffer)
            glIsFramebuffer(framebuffer);
#endif
    }
    glFramebufferDrawBuffersEXT(
        framebuffer,
        bufs.size(),
        bufs.size() ? reinterpret_cast<const GLenum*>(bufs.data()) : nullptr);
    detail::error_check("FramebufferDrawBuffersEXT"sv, check_errors);
}

/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param framebuffer GLuint
 * \param mode GLenum
 * \return void
 */
STATICINLINE void framebuffer_read_buffer(
    u32                     framebuffer,
    group::read_buffer_mode mode,
    error_check             check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(FramebufferReadBufferEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsFramebuffer)
            glIsFramebuffer(framebuffer);
#endif
    }
    glFramebufferReadBufferEXT(framebuffer, static_cast<GLenum>(mode));
    detail::error_check("FramebufferReadBufferEXT"sv, check_errors);
}

template<class span_i32>
requires(
    concepts::span<span_i32> && std::is_same_v<
                                    std::decay_t<typename span_i32::value_type>,
                                    std::decay_t<i32>>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param framebuffer GLuint
 * \param pname GLenum
 * \param params GLint *
 * \return void
 */
STATICINLINE void get_framebuffer_parameter(
    u32                              framebuffer,
    group::get_framebuffer_parameter pname,
    span_i32                         params,
    error_check                      check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetFramebufferParameterivEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsFramebuffer)
            glIsFramebuffer(framebuffer);
#endif
    }
    glGetFramebufferParameterivEXT(
        framebuffer,
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetFramebufferParameterivEXT"sv, check_errors);
}

/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param readBuffer GLuint
 * \param writeBuffer GLuint
 * \param readOffset GLintptr
 * \param writeOffset GLintptr
 * \param size GLsizeiptr
 * \return void
 */
STATICINLINE void named_copy_buffer_sub_data(
    u32         readBuffer,
    u32         writeBuffer,
    GLintptr    readOffset,
    GLintptr    writeOffset,
    GLsizeiptr  size,
    error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(NamedCopyBufferSubDataEXT)
    }
    glNamedCopyBufferSubDataEXT(
        readBuffer, writeBuffer, readOffset, writeOffset, size);
    detail::error_check("NamedCopyBufferSubDataEXT"sv, check_errors);
}

/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param framebuffer GLuint
 * \param attachment GLenum
 * \param texture GLuint
 * \param level GLint
 * \return void
 */
STATICINLINE void named_framebuffer_texture(
    u32                           framebuffer,
    group::framebuffer_attachment attachment,
    u32                           texture,
    i32                           level,
    error_check                   check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(NamedFramebufferTextureEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsFramebuffer)
            glIsFramebuffer(framebuffer);
#endif
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsTexture)
            glIsTexture(texture);
#endif
    }
    glNamedFramebufferTextureEXT(
        framebuffer, static_cast<GLenum>(attachment), texture, level);
    detail::error_check("NamedFramebufferTextureEXT"sv, check_errors);
}

/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param framebuffer GLuint
 * \param attachment GLenum
 * \param texture GLuint
 * \param level GLint
 * \param face GLenum
 * \return void
 */
STATICINLINE void named_framebuffer_texture_face(
    u32                           framebuffer,
    group::framebuffer_attachment attachment,
    u32                           texture,
    i32                           level,
    group::texture_target         face,
    error_check                   check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(NamedFramebufferTextureFaceEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsFramebuffer)
            glIsFramebuffer(framebuffer);
#endif
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsTexture)
            glIsTexture(texture);
#endif
    }
    glNamedFramebufferTextureFaceEXT(
        framebuffer,
        static_cast<GLenum>(attachment),
        texture,
        level,
        static_cast<GLenum>(face));
    detail::error_check("NamedFramebufferTextureFaceEXT"sv, check_errors);
}

/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param framebuffer GLuint
 * \param attachment GLenum
 * \param texture GLuint
 * \param level GLint
 * \param layer GLint
 * \return void
 */
STATICINLINE void named_framebuffer_texture_layer(
    u32                           framebuffer,
    group::framebuffer_attachment attachment,
    u32                           texture,
    i32                           level,
    i32                           layer,
    error_check                   check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(NamedFramebufferTextureLayerEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsFramebuffer)
            glIsFramebuffer(framebuffer);
#endif
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsTexture)
            glIsTexture(texture);
#endif
    }
    glNamedFramebufferTextureLayerEXT(
        framebuffer, static_cast<GLenum>(attachment), texture, level, layer);
    detail::error_check("NamedFramebufferTextureLayerEXT"sv, check_errors);
}

/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param texunit GLenum
 * \param target GLenum
 * \param renderbuffer GLuint
 * \return void
 */
STATICINLINE void multi_tex_renderbuffer(
    group::texture_unit   texunit,
    group::texture_target target,
    u32                   renderbuffer,
    error_check           check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MultiTexRenderbufferEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsRenderbuffer)
            glIsRenderbuffer(renderbuffer);
#endif
    }
    glMultiTexRenderbufferEXT(
        static_cast<GLenum>(texunit),
        static_cast<GLenum>(target),
        renderbuffer);
    detail::error_check("MultiTexRenderbufferEXT"sv, check_errors);
}

/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param texture GLuint
 * \param target GLenum
 * \param renderbuffer GLuint
 * \return void
 */
STATICINLINE void texture_renderbuffer(
    u32                   texture,
    group::texture_target target,
    u32                   renderbuffer,
    error_check           check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TextureRenderbufferEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsTexture)
            glIsTexture(texture);
#endif
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsRenderbuffer)
            glIsRenderbuffer(renderbuffer);
#endif
    }
    glTextureRenderbufferEXT(
        texture, static_cast<GLenum>(target), renderbuffer);
    detail::error_check("TextureRenderbufferEXT"sv, check_errors);
}

/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param vaobj GLuint
 * \param buffer GLuint
 * \param size GLint
 * \param type GLenum
 * \param stride GLsizei
 * \param offset GLintptr
 * \return void
 */
STATICINLINE void vertex_array_color_offset(
    u32                       vaobj,
    u32                       buffer,
    i32                       size,
    group::color_pointer_type type,
    i32                       stride,
    GLintptr                  offset,
    error_check               check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexArrayColorOffsetEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsVertexArray)
            glIsVertexArray(vaobj);
#endif
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsBuffer)
            glIsBuffer(buffer);
#endif
    }
    glVertexArrayColorOffsetEXT(
        vaobj, buffer, size, static_cast<GLenum>(type), stride, offset);
    detail::error_check("VertexArrayColorOffsetEXT"sv, check_errors);
}

/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param vaobj GLuint
 * \param buffer GLuint
 * \param stride GLsizei
 * \param offset GLintptr
 * \return void
 */
STATICINLINE void vertex_array_edge_flag_offset(
    u32         vaobj,
    u32         buffer,
    i32         stride,
    GLintptr    offset,
    error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexArrayEdgeFlagOffsetEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsVertexArray)
            glIsVertexArray(vaobj);
#endif
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsBuffer)
            glIsBuffer(buffer);
#endif
    }
    glVertexArrayEdgeFlagOffsetEXT(vaobj, buffer, stride, offset);
    detail::error_check("VertexArrayEdgeFlagOffsetEXT"sv, check_errors);
}

/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param vaobj GLuint
 * \param buffer GLuint
 * \param type GLenum
 * \param stride GLsizei
 * \param offset GLintptr
 * \return void
 */
STATICINLINE void vertex_array_fog_coord_offset(
    u32                                vaobj,
    u32                                buffer,
    group::fog_coordinate_pointer_type type,
    i32                                stride,
    GLintptr                           offset,
    error_check                        check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexArrayFogCoordOffsetEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsVertexArray)
            glIsVertexArray(vaobj);
#endif
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsBuffer)
            glIsBuffer(buffer);
#endif
    }
    glVertexArrayFogCoordOffsetEXT(
        vaobj, buffer, static_cast<GLenum>(type), stride, offset);
    detail::error_check("VertexArrayFogCoordOffsetEXT"sv, check_errors);
}

/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param vaobj GLuint
 * \param buffer GLuint
 * \param type GLenum
 * \param stride GLsizei
 * \param offset GLintptr
 * \return void
 */
STATICINLINE void vertex_array_index_offset(
    u32                       vaobj,
    u32                       buffer,
    group::index_pointer_type type,
    i32                       stride,
    GLintptr                  offset,
    error_check               check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexArrayIndexOffsetEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsVertexArray)
            glIsVertexArray(vaobj);
#endif
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsBuffer)
            glIsBuffer(buffer);
#endif
    }
    glVertexArrayIndexOffsetEXT(
        vaobj, buffer, static_cast<GLenum>(type), stride, offset);
    detail::error_check("VertexArrayIndexOffsetEXT"sv, check_errors);
}

/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param vaobj GLuint
 * \param buffer GLuint
 * \param texunit GLenum
 * \param size GLint
 * \param type GLenum
 * \param stride GLsizei
 * \param offset GLintptr
 * \return void
 */
STATICINLINE void vertex_array_multi_tex_coord_offset(
    u32                           vaobj,
    u32                           buffer,
    GLenum                        texunit,
    i32                           size,
    group::tex_coord_pointer_type type,
    i32                           stride,
    GLintptr                      offset,
    error_check                   check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexArrayMultiTexCoordOffsetEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsVertexArray)
            glIsVertexArray(vaobj);
#endif
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsBuffer)
            glIsBuffer(buffer);
#endif
    }
    glVertexArrayMultiTexCoordOffsetEXT(
        vaobj,
        buffer,
        texunit,
        size,
        static_cast<GLenum>(type),
        stride,
        offset);
    detail::error_check("VertexArrayMultiTexCoordOffsetEXT"sv, check_errors);
}

/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param vaobj GLuint
 * \param buffer GLuint
 * \param type GLenum
 * \param stride GLsizei
 * \param offset GLintptr
 * \return void
 */
STATICINLINE void vertex_array_normal_offset(
    u32                        vaobj,
    u32                        buffer,
    group::normal_pointer_type type,
    i32                        stride,
    GLintptr                   offset,
    error_check                check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexArrayNormalOffsetEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsVertexArray)
            glIsVertexArray(vaobj);
#endif
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsBuffer)
            glIsBuffer(buffer);
#endif
    }
    glVertexArrayNormalOffsetEXT(
        vaobj, buffer, static_cast<GLenum>(type), stride, offset);
    detail::error_check("VertexArrayNormalOffsetEXT"sv, check_errors);
}

/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param vaobj GLuint
 * \param buffer GLuint
 * \param size GLint
 * \param type GLenum
 * \param stride GLsizei
 * \param offset GLintptr
 * \return void
 */
STATICINLINE void vertex_array_secondary_color_offset(
    u32                       vaobj,
    u32                       buffer,
    i32                       size,
    group::color_pointer_type type,
    i32                       stride,
    GLintptr                  offset,
    error_check               check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexArraySecondaryColorOffsetEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsVertexArray)
            glIsVertexArray(vaobj);
#endif
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsBuffer)
            glIsBuffer(buffer);
#endif
    }
    glVertexArraySecondaryColorOffsetEXT(
        vaobj, buffer, size, static_cast<GLenum>(type), stride, offset);
    detail::error_check("VertexArraySecondaryColorOffsetEXT"sv, check_errors);
}

/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param vaobj GLuint
 * \param buffer GLuint
 * \param size GLint
 * \param type GLenum
 * \param stride GLsizei
 * \param offset GLintptr
 * \return void
 */
STATICINLINE void vertex_array_tex_coord_offset(
    u32                           vaobj,
    u32                           buffer,
    i32                           size,
    group::tex_coord_pointer_type type,
    i32                           stride,
    GLintptr                      offset,
    error_check                   check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexArrayTexCoordOffsetEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsVertexArray)
            glIsVertexArray(vaobj);
#endif
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsBuffer)
            glIsBuffer(buffer);
#endif
    }
    glVertexArrayTexCoordOffsetEXT(
        vaobj, buffer, size, static_cast<GLenum>(type), stride, offset);
    detail::error_check("VertexArrayTexCoordOffsetEXT"sv, check_errors);
}

/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param vaobj GLuint
 * \param buffer GLuint
 * \param index GLuint
 * \param size GLint
 * \param type GLenum
 * \param stride GLsizei
 * \param offset GLintptr
 * \return void
 */
STATICINLINE void vertex_array_vertex_attrib_i_offset(
    u32                       vaobj,
    u32                       buffer,
    u32                       index,
    i32                       size,
    group::vertex_attrib_type type,
    i32                       stride,
    GLintptr                  offset,
    error_check               check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexArrayVertexAttribIOffsetEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsVertexArray)
            glIsVertexArray(vaobj);
#endif
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsBuffer)
            glIsBuffer(buffer);
#endif
    }
    glVertexArrayVertexAttribIOffsetEXT(
        vaobj, buffer, index, size, static_cast<GLenum>(type), stride, offset);
    detail::error_check("VertexArrayVertexAttribIOffsetEXT"sv, check_errors);
}

/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param vaobj GLuint
 * \param buffer GLuint
 * \param index GLuint
 * \param size GLint
 * \param type GLenum
 * \param normalized GLboolean
 * \param stride GLsizei
 * \param offset GLintptr
 * \return void
 */
STATICINLINE void vertex_array_vertex_attrib_offset(
    u32                               vaobj,
    u32                               buffer,
    u32                               index,
    i32                               size,
    group::vertex_attrib_pointer_type type,
    bool                              normalized,
    i32                               stride,
    GLintptr                          offset,
    error_check                       check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexArrayVertexAttribOffsetEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsVertexArray)
            glIsVertexArray(vaobj);
#endif
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsBuffer)
            glIsBuffer(buffer);
#endif
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
    detail::error_check("VertexArrayVertexAttribOffsetEXT"sv, check_errors);
}

/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param vaobj GLuint
 * \param buffer GLuint
 * \param size GLint
 * \param type GLenum
 * \param stride GLsizei
 * \param offset GLintptr
 * \return void
 */
STATICINLINE void vertex_array_vertex_offset(
    u32                        vaobj,
    u32                        buffer,
    i32                        size,
    group::vertex_pointer_type type,
    i32                        stride,
    GLintptr                   offset,
    error_check                check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexArrayVertexOffsetEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsVertexArray)
            glIsVertexArray(vaobj);
#endif
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsBuffer)
            glIsBuffer(buffer);
#endif
    }
    glVertexArrayVertexOffsetEXT(
        vaobj, buffer, size, static_cast<GLenum>(type), stride, offset);
    detail::error_check("VertexArrayVertexOffsetEXT"sv, check_errors);
}

/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param vaobj GLuint
 * \param array GLenum
 * \return void
 */
STATICINLINE void disable_vertex_array(
    u32               vaobj,
    group::enable_cap array,
    error_check       check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DisableVertexArrayEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsVertexArray)
            glIsVertexArray(vaobj);
#endif
    }
    glDisableVertexArrayEXT(vaobj, static_cast<GLenum>(array));
    detail::error_check("DisableVertexArrayEXT"sv, check_errors);
}

/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param vaobj GLuint
 * \param array GLenum
 * \return void
 */
STATICINLINE void enable_vertex_array(
    u32               vaobj,
    group::enable_cap array,
    error_check       check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(EnableVertexArrayEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsVertexArray)
            glIsVertexArray(vaobj);
#endif
    }
    glEnableVertexArrayEXT(vaobj, static_cast<GLenum>(array));
    detail::error_check("EnableVertexArrayEXT"sv, check_errors);
}

/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param vaobj GLuint
 * \param index GLuint
 * \return void
 */
STATICINLINE void disable_vertex_array_attrib(
    u32 vaobj, u32 index, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DisableVertexArrayAttribEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsVertexArray)
            glIsVertexArray(vaobj);
#endif
    }
    glDisableVertexArrayAttribEXT(vaobj, index);
    detail::error_check("DisableVertexArrayAttribEXT"sv, check_errors);
}

/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param vaobj GLuint
 * \param index GLuint
 * \return void
 */
STATICINLINE void enable_vertex_array_attrib(
    u32 vaobj, u32 index, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(EnableVertexArrayAttribEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsVertexArray)
            glIsVertexArray(vaobj);
#endif
    }
    glEnableVertexArrayAttribEXT(vaobj, index);
    detail::error_check("EnableVertexArrayAttribEXT"sv, check_errors);
}

template<class span_i32>
requires(
    concepts::span<span_i32> && std::is_same_v<
                                    std::decay_t<typename span_i32::value_type>,
                                    std::decay_t<i32>>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param vaobj GLuint
 * \param index GLuint
 * \param pname GLenum
 * \param param GLint *
 * \return void
 */
STATICINLINE void get_vertex_array_integeri_v(
    u32                      vaobj,
    u32                      index,
    group::vertex_array_prop pname,
    span_i32                 param,
    error_check              check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetVertexArrayIntegeri_vEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsVertexArray)
            glIsVertexArray(vaobj);
#endif
    }
    glGetVertexArrayIntegeri_vEXT(
        vaobj,
        index,
        static_cast<GLenum>(pname),
        param.size() ? reinterpret_cast<GLint*>(param.data()) : nullptr);
    detail::error_check("GetVertexArrayIntegeri_vEXT"sv, check_errors);
}

template<class span_i32>
requires(
    concepts::span<span_i32> && std::is_same_v<
                                    std::decay_t<typename span_i32::value_type>,
                                    std::decay_t<i32>>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param vaobj GLuint
 * \param pname GLenum
 * \param param GLint *
 * \return void
 */
STATICINLINE void get_vertex_array_integerv(
    u32                      vaobj,
    group::vertex_array_prop pname,
    span_i32                 param,
    error_check              check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetVertexArrayIntegervEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsVertexArray)
            glIsVertexArray(vaobj);
#endif
    }
    glGetVertexArrayIntegervEXT(
        vaobj,
        static_cast<GLenum>(pname),
        param.size() ? reinterpret_cast<GLint*>(param.data()) : nullptr);
    detail::error_check("GetVertexArrayIntegervEXT"sv, check_errors);
}

template<class span_void>
requires(concepts::span<span_void>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param vaobj GLuint
 * \param index GLuint
 * \param pname GLenum
 * \param param void **
 * \return void
 */
STATICINLINE void get_vertex_array_pointeri_v(
    u32                      vaobj,
    u32                      index,
    group::vertex_array_prop pname,
    span_void                param,
    error_check              check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetVertexArrayPointeri_vEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsVertexArray)
            glIsVertexArray(vaobj);
#endif
    }
    glGetVertexArrayPointeri_vEXT(
        vaobj,
        index,
        static_cast<GLenum>(pname),
        param.size() ? reinterpret_cast<void**>(param.data()) : nullptr);
    detail::error_check("GetVertexArrayPointeri_vEXT"sv, check_errors);
}

template<class span_void>
requires(concepts::span<span_void>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param vaobj GLuint
 * \param pname GLenum
 * \param param void **
 * \return void
 */
STATICINLINE void get_vertex_array_pointerv(
    u32                      vaobj,
    group::vertex_array_prop pname,
    span_void                param,
    error_check              check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetVertexArrayPointervEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsVertexArray)
            glIsVertexArray(vaobj);
#endif
    }
    glGetVertexArrayPointervEXT(
        vaobj,
        static_cast<GLenum>(pname),
        param.size() ? reinterpret_cast<void**>(param.data()) : nullptr);
    detail::error_check("GetVertexArrayPointervEXT"sv, check_errors);
}

/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param buffer GLuint
 * \param offset GLintptr
 * \param length GLsizeiptr
 * \return void
 */
STATICINLINE void flush_mapped_named_buffer_range(
    u32         buffer,
    GLintptr    offset,
    GLsizeiptr  length,
    error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(FlushMappedNamedBufferRangeEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsBuffer)
            glIsBuffer(buffer);
#endif
    }
    glFlushMappedNamedBufferRangeEXT(buffer, offset, length);
    detail::error_check("FlushMappedNamedBufferRangeEXT"sv, check_errors);
}

/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param buffer GLuint
 * \param offset GLintptr
 * \param length GLsizeiptr
 * \param access GLbitfield
 * \return void *
 */
STATICINLINE void* map_named_buffer_range(
    u32                           buffer,
    GLintptr                      offset,
    GLsizeiptr                    length,
    group::map_buffer_access_mask access,
    error_check                   check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MapNamedBufferRangeEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsBuffer)
            glIsBuffer(buffer);
#endif
    }
    auto out = glMapNamedBufferRangeEXT(
        buffer, offset, length, static_cast<GLenum>(access));
    detail::error_check("MapNamedBufferRangeEXT"sv, check_errors);
    return out;
}

template<class span_const_void>
requires(concepts::span<span_const_void>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param buffer GLuint
 * \param size GLsizeiptr
 * \param data const void *
 * \param flags GLbitfield
 * \return void
 */
STATICINLINE void named_buffer_storage(
    u32                        buffer,
    span_const_void const&     data,
    group::buffer_storage_mask flags,
    error_check                check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(NamedBufferStorageEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsBuffer)
            glIsBuffer(buffer);
#endif
    }
    glNamedBufferStorageEXT(
        buffer,
        data.size() *
            sizeof(typename std::decay_t<span_const_void const&>::value_type),
        data.size() ? reinterpret_cast<const void*>(data.data()) : nullptr,
        static_cast<GLenum>(flags));
    detail::error_check("NamedBufferStorageEXT"sv, check_errors);
}

template<class span_const_void>
requires(concepts::span<span_const_void>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param buffer GLuint
 * \param internalformat GLenum
 * \param format GLenum
 * \param type GLenum
 * \param data const void *
 * \return void
 */
STATICINLINE void clear_named_buffer_data(
    u32                          buffer,
    group::sized_internal_format internalformat,
    group::pixel_format          format,
    group::pixel_type            type,
    span_const_void const&       data,
    error_check                  check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ClearNamedBufferDataEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsBuffer)
            glIsBuffer(buffer);
#endif
    }
    glClearNamedBufferDataEXT(
        buffer,
        static_cast<GLenum>(internalformat),
        static_cast<GLenum>(format),
        static_cast<GLenum>(type),
        data.size() ? reinterpret_cast<const void*>(data.data()) : nullptr);
    detail::error_check("ClearNamedBufferDataEXT"sv, check_errors);
}

template<class span_const_void>
requires(concepts::span<span_const_void>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param buffer GLuint
 * \param internalformat GLenum
 * \param offset GLsizeiptr
 * \param size GLsizeiptr
 * \param format GLenum
 * \param type GLenum
 * \param data const void *
 * \return void
 */
STATICINLINE void clear_named_buffer_sub_data(
    u32                          buffer,
    group::sized_internal_format internalformat,
    GLsizeiptr                   offset,
    GLsizeiptr                   size,
    group::pixel_format          format,
    group::pixel_type            type,
    span_const_void const&       data,
    error_check                  check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ClearNamedBufferSubDataEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsBuffer)
            glIsBuffer(buffer);
#endif
    }
    glClearNamedBufferSubDataEXT(
        buffer,
        static_cast<GLenum>(internalformat),
        offset,
        size,
        static_cast<GLenum>(format),
        static_cast<GLenum>(type),
        data.size() ? reinterpret_cast<const void*>(data.data()) : nullptr);
    detail::error_check("ClearNamedBufferSubDataEXT"sv, check_errors);
}

template<class span_i32>
requires(
    concepts::span<span_i32> && std::is_same_v<
                                    std::decay_t<typename span_i32::value_type>,
                                    std::decay_t<i32>>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param framebuffer GLuint
 * \param pname GLenum
 * \param params GLint *
 * \return void
 */
STATICINLINE void get_named_framebuffer_parameter(
    u32                              framebuffer,
    group::get_framebuffer_parameter pname,
    span_i32                         params,
    error_check                      check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetNamedFramebufferParameterivEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsFramebuffer)
            glIsFramebuffer(framebuffer);
#endif
    }
    glGetNamedFramebufferParameterivEXT(
        framebuffer,
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetNamedFramebufferParameterivEXT"sv, check_errors);
}

/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param framebuffer GLuint
 * \param pname GLenum
 * \param param GLint
 * \return void
 */
STATICINLINE void named_framebuffer_parameter(
    u32                               framebuffer,
    group::framebuffer_parameter_name pname,
    i32                               param,
    error_check                       check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(NamedFramebufferParameteriEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsFramebuffer)
            glIsFramebuffer(framebuffer);
#endif
    }
    glNamedFramebufferParameteriEXT(
        framebuffer, static_cast<GLenum>(pname), param);
    detail::error_check("NamedFramebufferParameteriEXT"sv, check_errors);
}

/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param program GLuint
 * \param location GLint
 * \param x GLdouble
 * \return void
 */
STATICINLINE void program_uniform(
    u32         program,
    i32         location,
    f64         x,
    error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform1dEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glProgramUniform1dEXT(program, location, x);
    detail::error_check("ProgramUniform1dEXT"sv, check_errors);
}

template<class span_const_f64>
requires(
    concepts::span<span_const_f64> &&
    std::is_same_v<
        std::decay_t<typename span_const_f64::value_type>,
        std::decay_t<f64>>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param program GLuint
 * \param location GLint
 * \param count GLsizei
 * \param value const GLdouble *
 * \return void
 */
STATICINLINE void program_uniform(
    u32                   program,
    i32                   location,
    span_const_f64 const& value,
    error_check           check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform1dvEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glProgramUniform1dvEXT(
        program,
        location,
        value.size(),
        reinterpret_cast<const GLdouble*>(value.data()));
    detail::error_check("ProgramUniform1dvEXT"sv, check_errors);
}

template<class vec_2_f64>
requires(concepts::vector<vec_2_f64, f64, 2>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param program GLuint
 * \param location GLint
 * \param x GLdouble
 * \param y GLdouble
 * \return void
 */
STATICINLINE void program_uniform(
    u32              program,
    i32              location,
    vec_2_f64 const& x,
    error_check      check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform2dEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glProgramUniform2dEXT(program, location, x[0], x[1]);
    detail::error_check("ProgramUniform2dEXT"sv, check_errors);
}

template<class span_const_vec_2_f64>
requires(
    concepts::span<span_const_vec_2_f64> &&
    concepts::vector<typename span_const_vec_2_f64::value_type, f64, 2>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param program GLuint
 * \param location GLint
 * \param count GLsizei
 * \param value const GLdouble *
 * \return void
 */
STATICINLINE void program_uniform(
    u32                         program,
    i32                         location,
    span_const_vec_2_f64 const& value,
    error_check                 check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform2dvEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glProgramUniform2dvEXT(
        program,
        location,
        value.size(),
        reinterpret_cast<const GLdouble*>(value.data()));
    detail::error_check("ProgramUniform2dvEXT"sv, check_errors);
}

template<class vec_3_f64>
requires(concepts::vector<vec_3_f64, f64, 3>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param program GLuint
 * \param location GLint
 * \param x GLdouble
 * \param y GLdouble
 * \param z GLdouble
 * \return void
 */
STATICINLINE void program_uniform(
    u32              program,
    i32              location,
    vec_3_f64 const& x,
    error_check      check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform3dEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glProgramUniform3dEXT(program, location, x[0], x[1], x[2]);
    detail::error_check("ProgramUniform3dEXT"sv, check_errors);
}

template<class span_const_vec_3_f64>
requires(
    concepts::span<span_const_vec_3_f64> &&
    concepts::vector<typename span_const_vec_3_f64::value_type, f64, 3>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param program GLuint
 * \param location GLint
 * \param count GLsizei
 * \param value const GLdouble *
 * \return void
 */
STATICINLINE void program_uniform(
    u32                         program,
    i32                         location,
    span_const_vec_3_f64 const& value,
    error_check                 check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform3dvEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glProgramUniform3dvEXT(
        program,
        location,
        value.size(),
        reinterpret_cast<const GLdouble*>(value.data()));
    detail::error_check("ProgramUniform3dvEXT"sv, check_errors);
}

template<class vec_4_f64>
requires(concepts::vector<vec_4_f64, f64, 4>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param program GLuint
 * \param location GLint
 * \param x GLdouble
 * \param y GLdouble
 * \param z GLdouble
 * \param w GLdouble
 * \return void
 */
STATICINLINE void program_uniform(
    u32              program,
    i32              location,
    vec_4_f64 const& x,
    error_check      check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform4dEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glProgramUniform4dEXT(program, location, x[0], x[1], x[2], x[3]);
    detail::error_check("ProgramUniform4dEXT"sv, check_errors);
}

template<class span_const_vec_4_f64>
requires(
    concepts::span<span_const_vec_4_f64> &&
    concepts::vector<typename span_const_vec_4_f64::value_type, f64, 4>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param program GLuint
 * \param location GLint
 * \param count GLsizei
 * \param value const GLdouble *
 * \return void
 */
STATICINLINE void program_uniform(
    u32                         program,
    i32                         location,
    span_const_vec_4_f64 const& value,
    error_check                 check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform4dvEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glProgramUniform4dvEXT(
        program,
        location,
        value.size(),
        reinterpret_cast<const GLdouble*>(value.data()));
    detail::error_check("ProgramUniform4dvEXT"sv, check_errors);
}

template<class span_const_mat_2x2_f64>
requires(
    concepts::span<span_const_mat_2x2_f64> &&
    concepts::matrix<typename span_const_mat_2x2_f64::value_type, f64, 2, 2>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param program GLuint
 * \param location GLint
 * \param count GLsizei
 * \param transpose GLboolean
 * \param value const GLdouble *
 * \return void
 */
STATICINLINE void program_uniform(
    u32                           program,
    i32                           location,
    bool                          transpose,
    span_const_mat_2x2_f64 const& value,
    error_check                   check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniformMatrix2dvEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glProgramUniformMatrix2dvEXT(
        program,
        location,
        value.size(),
        transpose,
        reinterpret_cast<const GLdouble*>(value.data()));
    detail::error_check("ProgramUniformMatrix2dvEXT"sv, check_errors);
}

template<class span_const_mat_2x3_f64>
requires(
    concepts::span<span_const_mat_2x3_f64> &&
    concepts::matrix<typename span_const_mat_2x3_f64::value_type, f64, 2, 3>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param program GLuint
 * \param location GLint
 * \param count GLsizei
 * \param transpose GLboolean
 * \param value const GLdouble *
 * \return void
 */
STATICINLINE void program_uniform(
    u32                           program,
    i32                           location,
    bool                          transpose,
    span_const_mat_2x3_f64 const& value,
    error_check                   check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniformMatrix2x3dvEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glProgramUniformMatrix2x3dvEXT(
        program,
        location,
        value.size(),
        transpose,
        reinterpret_cast<const GLdouble*>(value.data()));
    detail::error_check("ProgramUniformMatrix2x3dvEXT"sv, check_errors);
}

template<class span_const_mat_2x4_f64>
requires(
    concepts::span<span_const_mat_2x4_f64> &&
    concepts::matrix<typename span_const_mat_2x4_f64::value_type, f64, 2, 4>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param program GLuint
 * \param location GLint
 * \param count GLsizei
 * \param transpose GLboolean
 * \param value const GLdouble *
 * \return void
 */
STATICINLINE void program_uniform(
    u32                           program,
    i32                           location,
    bool                          transpose,
    span_const_mat_2x4_f64 const& value,
    error_check                   check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniformMatrix2x4dvEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glProgramUniformMatrix2x4dvEXT(
        program,
        location,
        value.size(),
        transpose,
        reinterpret_cast<const GLdouble*>(value.data()));
    detail::error_check("ProgramUniformMatrix2x4dvEXT"sv, check_errors);
}

template<class span_const_mat_3x3_f64>
requires(
    concepts::span<span_const_mat_3x3_f64> &&
    concepts::matrix<typename span_const_mat_3x3_f64::value_type, f64, 3, 3>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param program GLuint
 * \param location GLint
 * \param count GLsizei
 * \param transpose GLboolean
 * \param value const GLdouble *
 * \return void
 */
STATICINLINE void program_uniform(
    u32                           program,
    i32                           location,
    bool                          transpose,
    span_const_mat_3x3_f64 const& value,
    error_check                   check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniformMatrix3dvEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glProgramUniformMatrix3dvEXT(
        program,
        location,
        value.size(),
        transpose,
        reinterpret_cast<const GLdouble*>(value.data()));
    detail::error_check("ProgramUniformMatrix3dvEXT"sv, check_errors);
}

template<class span_const_mat_3x2_f64>
requires(
    concepts::span<span_const_mat_3x2_f64> &&
    concepts::matrix<typename span_const_mat_3x2_f64::value_type, f64, 3, 2>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param program GLuint
 * \param location GLint
 * \param count GLsizei
 * \param transpose GLboolean
 * \param value const GLdouble *
 * \return void
 */
STATICINLINE void program_uniform(
    u32                           program,
    i32                           location,
    bool                          transpose,
    span_const_mat_3x2_f64 const& value,
    error_check                   check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniformMatrix3x2dvEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glProgramUniformMatrix3x2dvEXT(
        program,
        location,
        value.size(),
        transpose,
        reinterpret_cast<const GLdouble*>(value.data()));
    detail::error_check("ProgramUniformMatrix3x2dvEXT"sv, check_errors);
}

template<class span_const_mat_3x4_f64>
requires(
    concepts::span<span_const_mat_3x4_f64> &&
    concepts::matrix<typename span_const_mat_3x4_f64::value_type, f64, 3, 4>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param program GLuint
 * \param location GLint
 * \param count GLsizei
 * \param transpose GLboolean
 * \param value const GLdouble *
 * \return void
 */
STATICINLINE void program_uniform(
    u32                           program,
    i32                           location,
    bool                          transpose,
    span_const_mat_3x4_f64 const& value,
    error_check                   check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniformMatrix3x4dvEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glProgramUniformMatrix3x4dvEXT(
        program,
        location,
        value.size(),
        transpose,
        reinterpret_cast<const GLdouble*>(value.data()));
    detail::error_check("ProgramUniformMatrix3x4dvEXT"sv, check_errors);
}

template<class span_const_mat_4x4_f64>
requires(
    concepts::span<span_const_mat_4x4_f64> &&
    concepts::matrix<typename span_const_mat_4x4_f64::value_type, f64, 4, 4>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param program GLuint
 * \param location GLint
 * \param count GLsizei
 * \param transpose GLboolean
 * \param value const GLdouble *
 * \return void
 */
STATICINLINE void program_uniform(
    u32                           program,
    i32                           location,
    bool                          transpose,
    span_const_mat_4x4_f64 const& value,
    error_check                   check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniformMatrix4dvEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glProgramUniformMatrix4dvEXT(
        program,
        location,
        value.size(),
        transpose,
        reinterpret_cast<const GLdouble*>(value.data()));
    detail::error_check("ProgramUniformMatrix4dvEXT"sv, check_errors);
}

template<class span_const_mat_4x2_f64>
requires(
    concepts::span<span_const_mat_4x2_f64> &&
    concepts::matrix<typename span_const_mat_4x2_f64::value_type, f64, 4, 2>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param program GLuint
 * \param location GLint
 * \param count GLsizei
 * \param transpose GLboolean
 * \param value const GLdouble *
 * \return void
 */
STATICINLINE void program_uniform(
    u32                           program,
    i32                           location,
    bool                          transpose,
    span_const_mat_4x2_f64 const& value,
    error_check                   check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniformMatrix4x2dvEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glProgramUniformMatrix4x2dvEXT(
        program,
        location,
        value.size(),
        transpose,
        reinterpret_cast<const GLdouble*>(value.data()));
    detail::error_check("ProgramUniformMatrix4x2dvEXT"sv, check_errors);
}

template<class span_const_mat_4x3_f64>
requires(
    concepts::span<span_const_mat_4x3_f64> &&
    concepts::matrix<typename span_const_mat_4x3_f64::value_type, f64, 4, 3>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param program GLuint
 * \param location GLint
 * \param count GLsizei
 * \param transpose GLboolean
 * \param value const GLdouble *
 * \return void
 */
STATICINLINE void program_uniform(
    u32                           program,
    i32                           location,
    bool                          transpose,
    span_const_mat_4x3_f64 const& value,
    error_check                   check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniformMatrix4x3dvEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glProgramUniformMatrix4x3dvEXT(
        program,
        location,
        value.size(),
        transpose,
        reinterpret_cast<const GLdouble*>(value.data()));
    detail::error_check("ProgramUniformMatrix4x3dvEXT"sv, check_errors);
}

/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param texture GLuint
 * \param target GLenum
 * \param internalformat GLenum
 * \param buffer GLuint
 * \param offset GLintptr
 * \param size GLsizeiptr
 * \return void
 */
STATICINLINE void texture_buffer_range(
    u32                          texture,
    group::texture_target        target,
    group::sized_internal_format internalformat,
    u32                          buffer,
    GLintptr                     offset,
    GLsizeiptr                   size,
    error_check                  check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TextureBufferRangeEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsTexture)
            glIsTexture(texture);
#endif
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsBuffer)
            glIsBuffer(buffer);
#endif
    }
    glTextureBufferRangeEXT(
        texture,
        static_cast<GLenum>(target),
        static_cast<GLenum>(internalformat),
        buffer,
        offset,
        size);
    detail::error_check("TextureBufferRangeEXT"sv, check_errors);
}

/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param texture GLuint
 * \param target GLenum
 * \param levels GLsizei
 * \param internalformat GLenum
 * \param width GLsizei
 * \return void
 */
STATICINLINE void texture_storage_1d(
    u32                          texture,
    GLenum                       target,
    i32                          levels,
    group::sized_internal_format internalformat,
    i32                          width,
    error_check                  check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TextureStorage1DEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsTexture)
            glIsTexture(texture);
#endif
    }
    glTextureStorage1DEXT(
        texture, target, levels, static_cast<GLenum>(internalformat), width);
    detail::error_check("TextureStorage1DEXT"sv, check_errors);
}

template<class size_2_i32>
requires(concepts::size_2d<size_2_i32, i32>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param texture GLuint
 * \param target GLenum
 * \param levels GLsizei
 * \param internalformat GLenum
 * \param width GLsizei
 * \param height GLsizei
 * \return void
 */
STATICINLINE void texture_storage_2d(
    u32                          texture,
    GLenum                       target,
    i32                          levels,
    group::sized_internal_format internalformat,
    size_2_i32 const&            width,
    error_check                  check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TextureStorage2DEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsTexture)
            glIsTexture(texture);
#endif
    }
    glTextureStorage2DEXT(
        texture,
        target,
        levels,
        static_cast<GLenum>(internalformat),
        width[0],
        width[1]);
    detail::error_check("TextureStorage2DEXT"sv, check_errors);
}

template<class size_3_i32>
requires(concepts::size_2d<size_3_i32, i32>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param texture GLuint
 * \param target GLenum
 * \param levels GLsizei
 * \param internalformat GLenum
 * \param width GLsizei
 * \param height GLsizei
 * \param depth GLsizei
 * \return void
 */
STATICINLINE void texture_storage_3d(
    u32                          texture,
    GLenum                       target,
    i32                          levels,
    group::sized_internal_format internalformat,
    size_3_i32 const&            width,
    error_check                  check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TextureStorage3DEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsTexture)
            glIsTexture(texture);
#endif
    }
    glTextureStorage3DEXT(
        texture,
        target,
        levels,
        static_cast<GLenum>(internalformat),
        width[0],
        width[1],
        width[2]);
    detail::error_check("TextureStorage3DEXT"sv, check_errors);
}

template<class size_2_i32>
requires(concepts::size_2d<size_2_i32, i32>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param texture GLuint
 * \param target GLenum
 * \param samples GLsizei
 * \param internalformat GLenum
 * \param width GLsizei
 * \param height GLsizei
 * \param fixedsamplelocations GLboolean
 * \return void
 */
STATICINLINE void texture_storage_2d_multisample(
    u32                          texture,
    group::texture_target        target,
    i32                          samples,
    group::sized_internal_format internalformat,
    size_2_i32 const&            width,
    bool                         fixedsamplelocations,
    error_check                  check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TextureStorage2DMultisampleEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsTexture)
            glIsTexture(texture);
#endif
    }
    glTextureStorage2DMultisampleEXT(
        texture,
        static_cast<GLenum>(target),
        samples,
        static_cast<GLenum>(internalformat),
        width[0],
        width[1],
        fixedsamplelocations);
    detail::error_check("TextureStorage2DMultisampleEXT"sv, check_errors);
}

template<class size_3_i32>
requires(concepts::size_2d<size_3_i32, i32>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param texture GLuint
 * \param target GLenum
 * \param samples GLsizei
 * \param internalformat GLenum
 * \param width GLsizei
 * \param height GLsizei
 * \param depth GLsizei
 * \param fixedsamplelocations GLboolean
 * \return void
 */
STATICINLINE void texture_storage_3d_multisample(
    u32                          texture,
    GLenum                       target,
    i32                          samples,
    group::sized_internal_format internalformat,
    size_3_i32 const&            width,
    bool                         fixedsamplelocations,
    error_check                  check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TextureStorage3DMultisampleEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsTexture)
            glIsTexture(texture);
#endif
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
    detail::error_check("TextureStorage3DMultisampleEXT"sv, check_errors);
}

/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param vaobj GLuint
 * \param bindingindex GLuint
 * \param buffer GLuint
 * \param offset GLintptr
 * \param stride GLsizei
 * \return void
 */
STATICINLINE void vertex_array_bind_vertex_buffer(
    u32         vaobj,
    u32         bindingindex,
    u32         buffer,
    GLintptr    offset,
    i32         stride,
    error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexArrayBindVertexBufferEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsVertexArray)
            glIsVertexArray(vaobj);
#endif
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsBuffer)
            glIsBuffer(buffer);
#endif
    }
    glVertexArrayBindVertexBufferEXT(
        vaobj, bindingindex, buffer, offset, stride);
    detail::error_check("VertexArrayBindVertexBufferEXT"sv, check_errors);
}

/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param vaobj GLuint
 * \param attribindex GLuint
 * \param bindingindex GLuint
 * \return void
 */
STATICINLINE void vertex_array_vertex_attrib_binding(
    u32         vaobj,
    u32         attribindex,
    u32         bindingindex,
    error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexArrayVertexAttribBindingEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsVertexArray)
            glIsVertexArray(vaobj);
#endif
    }
    glVertexArrayVertexAttribBindingEXT(vaobj, attribindex, bindingindex);
    detail::error_check("VertexArrayVertexAttribBindingEXT"sv, check_errors);
}

/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param vaobj GLuint
 * \param attribindex GLuint
 * \param size GLint
 * \param type GLenum
 * \param normalized GLboolean
 * \param relativeoffset GLuint
 * \return void
 */
STATICINLINE void vertex_array_vertex_attrib_format(
    u32                       vaobj,
    u32                       attribindex,
    i32                       size,
    group::vertex_attrib_type type,
    bool                      normalized,
    u32                       relativeoffset,
    error_check               check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexArrayVertexAttribFormatEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsVertexArray)
            glIsVertexArray(vaobj);
#endif
    }
    glVertexArrayVertexAttribFormatEXT(
        vaobj,
        attribindex,
        size,
        static_cast<GLenum>(type),
        normalized,
        relativeoffset);
    detail::error_check("VertexArrayVertexAttribFormatEXT"sv, check_errors);
}

/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param vaobj GLuint
 * \param attribindex GLuint
 * \param size GLint
 * \param type GLenum
 * \param relativeoffset GLuint
 * \return void
 */
STATICINLINE void vertex_array_vertex_attrib_i_format(
    u32                      vaobj,
    u32                      attribindex,
    i32                      size,
    group::vertex_attrib_int type,
    u32                      relativeoffset,
    error_check              check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexArrayVertexAttribIFormatEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsVertexArray)
            glIsVertexArray(vaobj);
#endif
    }
    glVertexArrayVertexAttribIFormatEXT(
        vaobj, attribindex, size, static_cast<GLenum>(type), relativeoffset);
    detail::error_check("VertexArrayVertexAttribIFormatEXT"sv, check_errors);
}

/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param vaobj GLuint
 * \param attribindex GLuint
 * \param size GLint
 * \param type GLenum
 * \param relativeoffset GLuint
 * \return void
 */
STATICINLINE void vertex_array_vertex_attrib_l_format(
    u32                       vaobj,
    u32                       attribindex,
    i32                       size,
    group::vertex_attrib_long type,
    u32                       relativeoffset,
    error_check               check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexArrayVertexAttribLFormatEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsVertexArray)
            glIsVertexArray(vaobj);
#endif
    }
    glVertexArrayVertexAttribLFormatEXT(
        vaobj, attribindex, size, static_cast<GLenum>(type), relativeoffset);
    detail::error_check("VertexArrayVertexAttribLFormatEXT"sv, check_errors);
}

/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param vaobj GLuint
 * \param bindingindex GLuint
 * \param divisor GLuint
 * \return void
 */
STATICINLINE void vertex_array_vertex_binding_divisor(
    u32         vaobj,
    u32         bindingindex,
    u32         divisor,
    error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexArrayVertexBindingDivisorEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsVertexArray)
            glIsVertexArray(vaobj);
#endif
    }
    glVertexArrayVertexBindingDivisorEXT(vaobj, bindingindex, divisor);
    detail::error_check("VertexArrayVertexBindingDivisorEXT"sv, check_errors);
}

/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param vaobj GLuint
 * \param buffer GLuint
 * \param index GLuint
 * \param size GLint
 * \param type GLenum
 * \param stride GLsizei
 * \param offset GLintptr
 * \return void
 */
STATICINLINE void vertex_array_vertex_attrib_l_offset(
    u32                       vaobj,
    u32                       buffer,
    u32                       index,
    i32                       size,
    group::vertex_attrib_long type,
    i32                       stride,
    GLintptr                  offset,
    error_check               check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexArrayVertexAttribLOffsetEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsVertexArray)
            glIsVertexArray(vaobj);
#endif
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsBuffer)
            glIsBuffer(buffer);
#endif
    }
    glVertexArrayVertexAttribLOffsetEXT(
        vaobj, buffer, index, size, static_cast<GLenum>(type), stride, offset);
    detail::error_check("VertexArrayVertexAttribLOffsetEXT"sv, check_errors);
}

template<class size_3_i32, class vec_3_i32>
requires(
    concepts::vector<vec_3_i32, i32, 3> && concepts::size_2d<size_3_i32, i32>)
/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param texture GLuint
 * \param level GLint
 * \param xoffset GLint
 * \param yoffset GLint
 * \param zoffset GLint
 * \param width GLsizei
 * \param height GLsizei
 * \param depth GLsizei
 * \param commit GLboolean
 * \return void
 */
STATICINLINE void texture_page_commitment(
    u32               texture,
    i32               level,
    vec_3_i32 const&  xoffset,
    size_3_i32 const& width,
    bool              commit,
    error_check       check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TexturePageCommitmentEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsTexture)
            glIsTexture(texture);
#endif
    }
    glTexturePageCommitmentEXT(
        texture,
        level,
        xoffset[0],
        xoffset[1],
        xoffset[2],
        width[0],
        width[1],
        width[2],
        commit);
    detail::error_check("TexturePageCommitmentEXT"sv, check_errors);
}

/*!
 * \brief Part of GL_EXT_direct_state_access
 * \param vaobj GLuint
 * \param index GLuint
 * \param divisor GLuint
 * \return void
 */
STATICINLINE void vertex_array_vertex_attrib_divisor(
    u32         vaobj,
    u32         index,
    u32         divisor,
    error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexArrayVertexAttribDivisorEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsVertexArray)
            glIsVertexArray(vaobj);
#endif
    }
    glVertexArrayVertexAttribDivisorEXT(vaobj, index, divisor);
    detail::error_check("VertexArrayVertexAttribDivisorEXT"sv, check_errors);
}

} // namespace gl::ext::direct_state_access
#endif // GL_EXT_direct_state_access
namespace gl::ext::direct_state_access {
constexpr auto name = "GL_EXT_direct_state_access";
}
