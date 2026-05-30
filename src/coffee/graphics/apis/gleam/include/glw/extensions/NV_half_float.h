#pragma once

#ifdef GL_NV_half_float
#include "../enums/PixelType.h"

namespace gl::nv::half_float {
using gl::group::pixel_type;

namespace values {
} // namespace values

template<class vec_3_GLhalfNV>
requires(concepts::vector<vec_3_GLhalfNV, GLhalfNV, 3>)
/*!
 * \brief Part of GL_NV_half_float
 * \param red GLhalfNV
 * \param green GLhalfNV
 * \param blue GLhalfNV
 * \return void
 */
STATICINLINE void color3h(
    vec_3_GLhalfNV const& red, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Color3hNV)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glColor3hNV, red);
#endif
    glColor3hNV(red[0], red[1], red[2]);
    detail::error_check("Color3hNV"sv, check_errors);
}

template<class span_const_GLhalfNV>
requires(
    concepts::span<span_const_GLhalfNV> &&
    std::is_same_v<
        std::decay_t<typename span_const_GLhalfNV::value_type>,
        std::decay_t<GLhalfNV>>)
/*!
 * \brief Part of GL_NV_half_float
 * \param v const GLhalfNV *
 * \return void
 */
STATICINLINE void color3hv(
    span_const_GLhalfNV const& v, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Color3hvNV)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE_DATA(
        glColor3hvNV,
        gsl::span<const char>(
            reinterpret_cast<const char*>(v.data()), v.size_bytes()));
#endif
    glColor3hvNV(
        v.size() ? reinterpret_cast<const GLhalfNV*>(v.data()) : nullptr);
    detail::error_check("Color3hvNV"sv, check_errors);
}

template<class vec_4_GLhalfNV>
requires(concepts::vector<vec_4_GLhalfNV, GLhalfNV, 4>)
/*!
 * \brief Part of GL_NV_half_float
 * \param red GLhalfNV
 * \param green GLhalfNV
 * \param blue GLhalfNV
 * \param alpha GLhalfNV
 * \return void
 */
STATICINLINE void color4h(
    vec_4_GLhalfNV const& red, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Color4hNV)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glColor4hNV, red);
#endif
    glColor4hNV(red[0], red[1], red[2], red[3]);
    detail::error_check("Color4hNV"sv, check_errors);
}

template<class span_const_GLhalfNV>
requires(
    concepts::span<span_const_GLhalfNV> &&
    std::is_same_v<
        std::decay_t<typename span_const_GLhalfNV::value_type>,
        std::decay_t<GLhalfNV>>)
/*!
 * \brief Part of GL_NV_half_float
 * \param v const GLhalfNV *
 * \return void
 */
STATICINLINE void color4hv(
    span_const_GLhalfNV const& v, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Color4hvNV)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE_DATA(
        glColor4hvNV,
        gsl::span<const char>(
            reinterpret_cast<const char*>(v.data()), v.size_bytes()));
#endif
    glColor4hvNV(
        v.size() ? reinterpret_cast<const GLhalfNV*>(v.data()) : nullptr);
    detail::error_check("Color4hvNV"sv, check_errors);
}

/*!
 * \brief Part of GL_NV_half_float
 * \param target GLenum
 * \param s GLhalfNV
 * \return void
 */
STATICINLINE void multi_tex_coord1h(
    group::texture_unit target,
    GLhalfNV            s,
    error_check         check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MultiTexCoord1hNV)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glMultiTexCoord1hNV, target, s);
#endif
    glMultiTexCoord1hNV(static_cast<GLenum>(target), s);
    detail::error_check("MultiTexCoord1hNV"sv, check_errors);
}

template<class span_const_GLhalfNV>
requires(
    concepts::span<span_const_GLhalfNV> &&
    std::is_same_v<
        std::decay_t<typename span_const_GLhalfNV::value_type>,
        std::decay_t<GLhalfNV>>)
/*!
 * \brief Part of GL_NV_half_float
 * \param target GLenum
 * \param v const GLhalfNV *
 * \return void
 */
STATICINLINE void multi_tex_coord1hv(
    group::texture_unit        target,
    span_const_GLhalfNV const& v,
    error_check                check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MultiTexCoord1hvNV)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE_DATA(
        glMultiTexCoord1hvNV,
        gsl::span<const char>(
            reinterpret_cast<const char*>(v.data()), v.size_bytes()),
        target);
#endif
    glMultiTexCoord1hvNV(
        static_cast<GLenum>(target),
        v.size() ? reinterpret_cast<const GLhalfNV*>(v.data()) : nullptr);
    detail::error_check("MultiTexCoord1hvNV"sv, check_errors);
}

/*!
 * \brief Part of GL_NV_half_float
 * \param target GLenum
 * \param s GLhalfNV
 * \param t GLhalfNV
 * \return void
 */
STATICINLINE void multi_tex_coord2h(
    group::texture_unit target,
    GLhalfNV            s,
    GLhalfNV            t,
    error_check         check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MultiTexCoord2hNV)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glMultiTexCoord2hNV, target, s, t);
#endif
    glMultiTexCoord2hNV(static_cast<GLenum>(target), s, t);
    detail::error_check("MultiTexCoord2hNV"sv, check_errors);
}

template<class span_const_GLhalfNV>
requires(
    concepts::span<span_const_GLhalfNV> &&
    std::is_same_v<
        std::decay_t<typename span_const_GLhalfNV::value_type>,
        std::decay_t<GLhalfNV>>)
/*!
 * \brief Part of GL_NV_half_float
 * \param target GLenum
 * \param v const GLhalfNV *
 * \return void
 */
STATICINLINE void multi_tex_coord2hv(
    group::texture_unit        target,
    span_const_GLhalfNV const& v,
    error_check                check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MultiTexCoord2hvNV)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE_DATA(
        glMultiTexCoord2hvNV,
        gsl::span<const char>(
            reinterpret_cast<const char*>(v.data()), v.size_bytes()),
        target);
#endif
    glMultiTexCoord2hvNV(
        static_cast<GLenum>(target),
        v.size() ? reinterpret_cast<const GLhalfNV*>(v.data()) : nullptr);
    detail::error_check("MultiTexCoord2hvNV"sv, check_errors);
}

/*!
 * \brief Part of GL_NV_half_float
 * \param target GLenum
 * \param s GLhalfNV
 * \param t GLhalfNV
 * \param r GLhalfNV
 * \return void
 */
STATICINLINE void multi_tex_coord3h(
    group::texture_unit target,
    GLhalfNV            s,
    GLhalfNV            t,
    GLhalfNV            r,
    error_check         check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MultiTexCoord3hNV)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glMultiTexCoord3hNV, target, s, t, r);
#endif
    glMultiTexCoord3hNV(static_cast<GLenum>(target), s, t, r);
    detail::error_check("MultiTexCoord3hNV"sv, check_errors);
}

template<class span_const_GLhalfNV>
requires(
    concepts::span<span_const_GLhalfNV> &&
    std::is_same_v<
        std::decay_t<typename span_const_GLhalfNV::value_type>,
        std::decay_t<GLhalfNV>>)
/*!
 * \brief Part of GL_NV_half_float
 * \param target GLenum
 * \param v const GLhalfNV *
 * \return void
 */
STATICINLINE void multi_tex_coord3hv(
    group::texture_unit        target,
    span_const_GLhalfNV const& v,
    error_check                check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MultiTexCoord3hvNV)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE_DATA(
        glMultiTexCoord3hvNV,
        gsl::span<const char>(
            reinterpret_cast<const char*>(v.data()), v.size_bytes()),
        target);
#endif
    glMultiTexCoord3hvNV(
        static_cast<GLenum>(target),
        v.size() ? reinterpret_cast<const GLhalfNV*>(v.data()) : nullptr);
    detail::error_check("MultiTexCoord3hvNV"sv, check_errors);
}

/*!
 * \brief Part of GL_NV_half_float
 * \param target GLenum
 * \param s GLhalfNV
 * \param t GLhalfNV
 * \param r GLhalfNV
 * \param q GLhalfNV
 * \return void
 */
STATICINLINE void multi_tex_coord4h(
    group::texture_unit target,
    GLhalfNV            s,
    GLhalfNV            t,
    GLhalfNV            r,
    GLhalfNV            q,
    error_check         check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MultiTexCoord4hNV)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glMultiTexCoord4hNV, target, s, t, r, q);
#endif
    glMultiTexCoord4hNV(static_cast<GLenum>(target), s, t, r, q);
    detail::error_check("MultiTexCoord4hNV"sv, check_errors);
}

template<class span_const_GLhalfNV>
requires(
    concepts::span<span_const_GLhalfNV> &&
    std::is_same_v<
        std::decay_t<typename span_const_GLhalfNV::value_type>,
        std::decay_t<GLhalfNV>>)
/*!
 * \brief Part of GL_NV_half_float
 * \param target GLenum
 * \param v const GLhalfNV *
 * \return void
 */
STATICINLINE void multi_tex_coord4hv(
    group::texture_unit        target,
    span_const_GLhalfNV const& v,
    error_check                check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MultiTexCoord4hvNV)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE_DATA(
        glMultiTexCoord4hvNV,
        gsl::span<const char>(
            reinterpret_cast<const char*>(v.data()), v.size_bytes()),
        target);
#endif
    glMultiTexCoord4hvNV(
        static_cast<GLenum>(target),
        v.size() ? reinterpret_cast<const GLhalfNV*>(v.data()) : nullptr);
    detail::error_check("MultiTexCoord4hvNV"sv, check_errors);
}

/*!
 * \brief Part of GL_NV_half_float
 * \param nx GLhalfNV
 * \param ny GLhalfNV
 * \param nz GLhalfNV
 * \return void
 */
STATICINLINE void normal3h(
    GLhalfNV    nx,
    GLhalfNV    ny,
    GLhalfNV    nz,
    error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Normal3hNV)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glNormal3hNV, nx, ny, nz);
#endif
    glNormal3hNV(nx, ny, nz);
    detail::error_check("Normal3hNV"sv, check_errors);
}

template<class span_const_GLhalfNV>
requires(
    concepts::span<span_const_GLhalfNV> &&
    std::is_same_v<
        std::decay_t<typename span_const_GLhalfNV::value_type>,
        std::decay_t<GLhalfNV>>)
/*!
 * \brief Part of GL_NV_half_float
 * \param v const GLhalfNV *
 * \return void
 */
STATICINLINE void normal3hv(
    span_const_GLhalfNV const& v, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Normal3hvNV)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE_DATA(
        glNormal3hvNV,
        gsl::span<const char>(
            reinterpret_cast<const char*>(v.data()), v.size_bytes()));
#endif
    glNormal3hvNV(
        v.size() ? reinterpret_cast<const GLhalfNV*>(v.data()) : nullptr);
    detail::error_check("Normal3hvNV"sv, check_errors);
}

/*!
 * \brief Part of GL_NV_half_float
 * \param s GLhalfNV
 * \return void
 */
STATICINLINE void tex_coord1h(
    GLhalfNV s, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TexCoord1hNV)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glTexCoord1hNV, s);
#endif
    glTexCoord1hNV(s);
    detail::error_check("TexCoord1hNV"sv, check_errors);
}

template<class span_const_GLhalfNV>
requires(
    concepts::span<span_const_GLhalfNV> &&
    std::is_same_v<
        std::decay_t<typename span_const_GLhalfNV::value_type>,
        std::decay_t<GLhalfNV>>)
/*!
 * \brief Part of GL_NV_half_float
 * \param v const GLhalfNV *
 * \return void
 */
STATICINLINE void tex_coord1hv(
    span_const_GLhalfNV const& v, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TexCoord1hvNV)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE_DATA(
        glTexCoord1hvNV,
        gsl::span<const char>(
            reinterpret_cast<const char*>(v.data()), v.size_bytes()));
#endif
    glTexCoord1hvNV(
        v.size() ? reinterpret_cast<const GLhalfNV*>(v.data()) : nullptr);
    detail::error_check("TexCoord1hvNV"sv, check_errors);
}

/*!
 * \brief Part of GL_NV_half_float
 * \param s GLhalfNV
 * \param t GLhalfNV
 * \return void
 */
STATICINLINE void tex_coord2h(
    GLhalfNV s, GLhalfNV t, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TexCoord2hNV)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glTexCoord2hNV, s, t);
#endif
    glTexCoord2hNV(s, t);
    detail::error_check("TexCoord2hNV"sv, check_errors);
}

template<class span_const_GLhalfNV>
requires(
    concepts::span<span_const_GLhalfNV> &&
    std::is_same_v<
        std::decay_t<typename span_const_GLhalfNV::value_type>,
        std::decay_t<GLhalfNV>>)
/*!
 * \brief Part of GL_NV_half_float
 * \param v const GLhalfNV *
 * \return void
 */
STATICINLINE void tex_coord2hv(
    span_const_GLhalfNV const& v, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TexCoord2hvNV)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE_DATA(
        glTexCoord2hvNV,
        gsl::span<const char>(
            reinterpret_cast<const char*>(v.data()), v.size_bytes()));
#endif
    glTexCoord2hvNV(
        v.size() ? reinterpret_cast<const GLhalfNV*>(v.data()) : nullptr);
    detail::error_check("TexCoord2hvNV"sv, check_errors);
}

/*!
 * \brief Part of GL_NV_half_float
 * \param s GLhalfNV
 * \param t GLhalfNV
 * \param r GLhalfNV
 * \return void
 */
STATICINLINE void tex_coord3h(
    GLhalfNV    s,
    GLhalfNV    t,
    GLhalfNV    r,
    error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TexCoord3hNV)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glTexCoord3hNV, s, t, r);
#endif
    glTexCoord3hNV(s, t, r);
    detail::error_check("TexCoord3hNV"sv, check_errors);
}

template<class span_const_GLhalfNV>
requires(
    concepts::span<span_const_GLhalfNV> &&
    std::is_same_v<
        std::decay_t<typename span_const_GLhalfNV::value_type>,
        std::decay_t<GLhalfNV>>)
/*!
 * \brief Part of GL_NV_half_float
 * \param v const GLhalfNV *
 * \return void
 */
STATICINLINE void tex_coord3hv(
    span_const_GLhalfNV const& v, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TexCoord3hvNV)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE_DATA(
        glTexCoord3hvNV,
        gsl::span<const char>(
            reinterpret_cast<const char*>(v.data()), v.size_bytes()));
#endif
    glTexCoord3hvNV(
        v.size() ? reinterpret_cast<const GLhalfNV*>(v.data()) : nullptr);
    detail::error_check("TexCoord3hvNV"sv, check_errors);
}

/*!
 * \brief Part of GL_NV_half_float
 * \param s GLhalfNV
 * \param t GLhalfNV
 * \param r GLhalfNV
 * \param q GLhalfNV
 * \return void
 */
STATICINLINE void tex_coord4h(
    GLhalfNV    s,
    GLhalfNV    t,
    GLhalfNV    r,
    GLhalfNV    q,
    error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TexCoord4hNV)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glTexCoord4hNV, s, t, r, q);
#endif
    glTexCoord4hNV(s, t, r, q);
    detail::error_check("TexCoord4hNV"sv, check_errors);
}

template<class span_const_GLhalfNV>
requires(
    concepts::span<span_const_GLhalfNV> &&
    std::is_same_v<
        std::decay_t<typename span_const_GLhalfNV::value_type>,
        std::decay_t<GLhalfNV>>)
/*!
 * \brief Part of GL_NV_half_float
 * \param v const GLhalfNV *
 * \return void
 */
STATICINLINE void tex_coord4hv(
    span_const_GLhalfNV const& v, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TexCoord4hvNV)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE_DATA(
        glTexCoord4hvNV,
        gsl::span<const char>(
            reinterpret_cast<const char*>(v.data()), v.size_bytes()));
#endif
    glTexCoord4hvNV(
        v.size() ? reinterpret_cast<const GLhalfNV*>(v.data()) : nullptr);
    detail::error_check("TexCoord4hvNV"sv, check_errors);
}

template<class vec_2_GLhalfNV>
requires(concepts::vector<vec_2_GLhalfNV, GLhalfNV, 2>)
/*!
 * \brief Part of GL_NV_half_float
 * \param x GLhalfNV
 * \param y GLhalfNV
 * \return void
 */
STATICINLINE void vertex2h(
    vec_2_GLhalfNV const& x, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Vertex2hNV)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glVertex2hNV, x);
#endif
    glVertex2hNV(x[0], x[1]);
    detail::error_check("Vertex2hNV"sv, check_errors);
}

template<class span_const_GLhalfNV>
requires(
    concepts::span<span_const_GLhalfNV> &&
    std::is_same_v<
        std::decay_t<typename span_const_GLhalfNV::value_type>,
        std::decay_t<GLhalfNV>>)
/*!
 * \brief Part of GL_NV_half_float
 * \param v const GLhalfNV *
 * \return void
 */
STATICINLINE void vertex2hv(
    span_const_GLhalfNV const& v, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Vertex2hvNV)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE_DATA(
        glVertex2hvNV,
        gsl::span<const char>(
            reinterpret_cast<const char*>(v.data()), v.size_bytes()));
#endif
    glVertex2hvNV(
        v.size() ? reinterpret_cast<const GLhalfNV*>(v.data()) : nullptr);
    detail::error_check("Vertex2hvNV"sv, check_errors);
}

template<class vec_3_GLhalfNV>
requires(concepts::vector<vec_3_GLhalfNV, GLhalfNV, 3>)
/*!
 * \brief Part of GL_NV_half_float
 * \param x GLhalfNV
 * \param y GLhalfNV
 * \param z GLhalfNV
 * \return void
 */
STATICINLINE void vertex3h(
    vec_3_GLhalfNV const& x, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Vertex3hNV)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glVertex3hNV, x);
#endif
    glVertex3hNV(x[0], x[1], x[2]);
    detail::error_check("Vertex3hNV"sv, check_errors);
}

template<class span_const_GLhalfNV>
requires(
    concepts::span<span_const_GLhalfNV> &&
    std::is_same_v<
        std::decay_t<typename span_const_GLhalfNV::value_type>,
        std::decay_t<GLhalfNV>>)
/*!
 * \brief Part of GL_NV_half_float
 * \param v const GLhalfNV *
 * \return void
 */
STATICINLINE void vertex3hv(
    span_const_GLhalfNV const& v, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Vertex3hvNV)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE_DATA(
        glVertex3hvNV,
        gsl::span<const char>(
            reinterpret_cast<const char*>(v.data()), v.size_bytes()));
#endif
    glVertex3hvNV(
        v.size() ? reinterpret_cast<const GLhalfNV*>(v.data()) : nullptr);
    detail::error_check("Vertex3hvNV"sv, check_errors);
}

template<class vec_4_GLhalfNV>
requires(concepts::vector<vec_4_GLhalfNV, GLhalfNV, 4>)
/*!
 * \brief Part of GL_NV_half_float
 * \param x GLhalfNV
 * \param y GLhalfNV
 * \param z GLhalfNV
 * \param w GLhalfNV
 * \return void
 */
STATICINLINE void vertex4h(
    vec_4_GLhalfNV const& x, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Vertex4hNV)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glVertex4hNV, x);
#endif
    glVertex4hNV(x[0], x[1], x[2], x[3]);
    detail::error_check("Vertex4hNV"sv, check_errors);
}

template<class span_const_GLhalfNV>
requires(
    concepts::span<span_const_GLhalfNV> &&
    std::is_same_v<
        std::decay_t<typename span_const_GLhalfNV::value_type>,
        std::decay_t<GLhalfNV>>)
/*!
 * \brief Part of GL_NV_half_float
 * \param v const GLhalfNV *
 * \return void
 */
STATICINLINE void vertex4hv(
    span_const_GLhalfNV const& v, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Vertex4hvNV)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE_DATA(
        glVertex4hvNV,
        gsl::span<const char>(
            reinterpret_cast<const char*>(v.data()), v.size_bytes()));
#endif
    glVertex4hvNV(
        v.size() ? reinterpret_cast<const GLhalfNV*>(v.data()) : nullptr);
    detail::error_check("Vertex4hvNV"sv, check_errors);
}

/*!
 * \brief Part of GL_NV_half_float
 * \param index GLuint
 * \param x GLhalfNV
 * \return void
 */
STATICINLINE void vertex_attrib1h(
    u32 index, GLhalfNV x, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttrib1hNV)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glVertexAttrib1hNV, index, x);
#endif
    glVertexAttrib1hNV(index, x);
    detail::error_check("VertexAttrib1hNV"sv, check_errors);
}

template<class span_const_GLhalfNV>
requires(
    concepts::span<span_const_GLhalfNV> &&
    std::is_same_v<
        std::decay_t<typename span_const_GLhalfNV::value_type>,
        std::decay_t<GLhalfNV>>)
/*!
 * \brief Part of GL_NV_half_float
 * \param index GLuint
 * \param v const GLhalfNV *
 * \return void
 */
STATICINLINE void vertex_attrib1hv(
    u32                        index,
    span_const_GLhalfNV const& v,
    error_check                check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttrib1hvNV)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE_DATA(
        glVertexAttrib1hvNV,
        gsl::span<const char>(
            reinterpret_cast<const char*>(v.data()), v.size_bytes()),
        index);
#endif
    glVertexAttrib1hvNV(
        index,
        v.size() ? reinterpret_cast<const GLhalfNV*>(v.data()) : nullptr);
    detail::error_check("VertexAttrib1hvNV"sv, check_errors);
}

template<class vec_2_GLhalfNV>
requires(concepts::vector<vec_2_GLhalfNV, GLhalfNV, 2>)
/*!
 * \brief Part of GL_NV_half_float
 * \param index GLuint
 * \param x GLhalfNV
 * \param y GLhalfNV
 * \return void
 */
STATICINLINE void vertex_attrib2h(
    u32                   index,
    vec_2_GLhalfNV const& x,
    error_check           check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttrib2hNV)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glVertexAttrib2hNV, index, x);
#endif
    glVertexAttrib2hNV(index, x[0], x[1]);
    detail::error_check("VertexAttrib2hNV"sv, check_errors);
}

template<class span_const_GLhalfNV>
requires(
    concepts::span<span_const_GLhalfNV> &&
    std::is_same_v<
        std::decay_t<typename span_const_GLhalfNV::value_type>,
        std::decay_t<GLhalfNV>>)
/*!
 * \brief Part of GL_NV_half_float
 * \param index GLuint
 * \param v const GLhalfNV *
 * \return void
 */
STATICINLINE void vertex_attrib2hv(
    u32                        index,
    span_const_GLhalfNV const& v,
    error_check                check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttrib2hvNV)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE_DATA(
        glVertexAttrib2hvNV,
        gsl::span<const char>(
            reinterpret_cast<const char*>(v.data()), v.size_bytes()),
        index);
#endif
    glVertexAttrib2hvNV(
        index,
        v.size() ? reinterpret_cast<const GLhalfNV*>(v.data()) : nullptr);
    detail::error_check("VertexAttrib2hvNV"sv, check_errors);
}

template<class vec_3_GLhalfNV>
requires(concepts::vector<vec_3_GLhalfNV, GLhalfNV, 3>)
/*!
 * \brief Part of GL_NV_half_float
 * \param index GLuint
 * \param x GLhalfNV
 * \param y GLhalfNV
 * \param z GLhalfNV
 * \return void
 */
STATICINLINE void vertex_attrib3h(
    u32                   index,
    vec_3_GLhalfNV const& x,
    error_check           check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttrib3hNV)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glVertexAttrib3hNV, index, x);
#endif
    glVertexAttrib3hNV(index, x[0], x[1], x[2]);
    detail::error_check("VertexAttrib3hNV"sv, check_errors);
}

template<class span_const_GLhalfNV>
requires(
    concepts::span<span_const_GLhalfNV> &&
    std::is_same_v<
        std::decay_t<typename span_const_GLhalfNV::value_type>,
        std::decay_t<GLhalfNV>>)
/*!
 * \brief Part of GL_NV_half_float
 * \param index GLuint
 * \param v const GLhalfNV *
 * \return void
 */
STATICINLINE void vertex_attrib3hv(
    u32                        index,
    span_const_GLhalfNV const& v,
    error_check                check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttrib3hvNV)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE_DATA(
        glVertexAttrib3hvNV,
        gsl::span<const char>(
            reinterpret_cast<const char*>(v.data()), v.size_bytes()),
        index);
#endif
    glVertexAttrib3hvNV(
        index,
        v.size() ? reinterpret_cast<const GLhalfNV*>(v.data()) : nullptr);
    detail::error_check("VertexAttrib3hvNV"sv, check_errors);
}

template<class vec_4_GLhalfNV>
requires(concepts::vector<vec_4_GLhalfNV, GLhalfNV, 4>)
/*!
 * \brief Part of GL_NV_half_float
 * \param index GLuint
 * \param x GLhalfNV
 * \param y GLhalfNV
 * \param z GLhalfNV
 * \param w GLhalfNV
 * \return void
 */
STATICINLINE void vertex_attrib4h(
    u32                   index,
    vec_4_GLhalfNV const& x,
    error_check           check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttrib4hNV)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glVertexAttrib4hNV, index, x);
#endif
    glVertexAttrib4hNV(index, x[0], x[1], x[2], x[3]);
    detail::error_check("VertexAttrib4hNV"sv, check_errors);
}

template<class span_const_GLhalfNV>
requires(
    concepts::span<span_const_GLhalfNV> &&
    std::is_same_v<
        std::decay_t<typename span_const_GLhalfNV::value_type>,
        std::decay_t<GLhalfNV>>)
/*!
 * \brief Part of GL_NV_half_float
 * \param index GLuint
 * \param v const GLhalfNV *
 * \return void
 */
STATICINLINE void vertex_attrib4hv(
    u32                        index,
    span_const_GLhalfNV const& v,
    error_check                check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttrib4hvNV)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE_DATA(
        glVertexAttrib4hvNV,
        gsl::span<const char>(
            reinterpret_cast<const char*>(v.data()), v.size_bytes()),
        index);
#endif
    glVertexAttrib4hvNV(
        index,
        v.size() ? reinterpret_cast<const GLhalfNV*>(v.data()) : nullptr);
    detail::error_check("VertexAttrib4hvNV"sv, check_errors);
}

template<class span_const_GLhalfNV>
requires(
    concepts::span<span_const_GLhalfNV> &&
    std::is_same_v<
        std::decay_t<typename span_const_GLhalfNV::value_type>,
        std::decay_t<GLhalfNV>>)
/*!
 * \brief Part of GL_NV_half_float
 * \param index GLuint
 * \param n GLsizei
 * \param v const GLhalfNV *
 * \return void
 */
STATICINLINE void vertex_attribs1hv(
    u32                        index,
    span_const_GLhalfNV const& v,
    error_check                check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribs1hvNV)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE_DATA(
        glVertexAttribs1hvNV,
        gsl::span<const char>(
            reinterpret_cast<const char*>(v.data()), v.size_bytes()),
        index);
#endif
    glVertexAttribs1hvNV(
        index,
        v.size(),
        v.size() ? reinterpret_cast<const GLhalfNV*>(v.data()) : nullptr);
    detail::error_check("VertexAttribs1hvNV"sv, check_errors);
}

template<class span_const_GLhalfNV>
requires(
    concepts::span<span_const_GLhalfNV> &&
    std::is_same_v<
        std::decay_t<typename span_const_GLhalfNV::value_type>,
        std::decay_t<GLhalfNV>>)
/*!
 * \brief Part of GL_NV_half_float
 * \param index GLuint
 * \param n GLsizei
 * \param v const GLhalfNV *
 * \return void
 */
STATICINLINE void vertex_attribs2hv(
    u32                        index,
    span_const_GLhalfNV const& v,
    error_check                check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribs2hvNV)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE_DATA(
        glVertexAttribs2hvNV,
        gsl::span<const char>(
            reinterpret_cast<const char*>(v.data()), v.size_bytes()),
        index);
#endif
    glVertexAttribs2hvNV(
        index,
        v.size(),
        v.size() ? reinterpret_cast<const GLhalfNV*>(v.data()) : nullptr);
    detail::error_check("VertexAttribs2hvNV"sv, check_errors);
}

template<class span_const_GLhalfNV>
requires(
    concepts::span<span_const_GLhalfNV> &&
    std::is_same_v<
        std::decay_t<typename span_const_GLhalfNV::value_type>,
        std::decay_t<GLhalfNV>>)
/*!
 * \brief Part of GL_NV_half_float
 * \param index GLuint
 * \param n GLsizei
 * \param v const GLhalfNV *
 * \return void
 */
STATICINLINE void vertex_attribs3hv(
    u32                        index,
    span_const_GLhalfNV const& v,
    error_check                check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribs3hvNV)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE_DATA(
        glVertexAttribs3hvNV,
        gsl::span<const char>(
            reinterpret_cast<const char*>(v.data()), v.size_bytes()),
        index);
#endif
    glVertexAttribs3hvNV(
        index,
        v.size(),
        v.size() ? reinterpret_cast<const GLhalfNV*>(v.data()) : nullptr);
    detail::error_check("VertexAttribs3hvNV"sv, check_errors);
}

template<class span_const_GLhalfNV>
requires(
    concepts::span<span_const_GLhalfNV> &&
    std::is_same_v<
        std::decay_t<typename span_const_GLhalfNV::value_type>,
        std::decay_t<GLhalfNV>>)
/*!
 * \brief Part of GL_NV_half_float
 * \param index GLuint
 * \param n GLsizei
 * \param v const GLhalfNV *
 * \return void
 */
STATICINLINE void vertex_attribs4hv(
    u32                        index,
    span_const_GLhalfNV const& v,
    error_check                check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribs4hvNV)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE_DATA(
        glVertexAttribs4hvNV,
        gsl::span<const char>(
            reinterpret_cast<const char*>(v.data()), v.size_bytes()),
        index);
#endif
    glVertexAttribs4hvNV(
        index,
        v.size(),
        v.size() ? reinterpret_cast<const GLhalfNV*>(v.data()) : nullptr);
    detail::error_check("VertexAttribs4hvNV"sv, check_errors);
}

/*!
 * \brief Part of GL_NV_half_float
 * \param fog GLhalfNV
 * \return void
 */
STATICINLINE void fog_coordh(
    GLhalfNV fog, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(FogCoordhNV)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glFogCoordhNV, fog);
#endif
    glFogCoordhNV(fog);
    detail::error_check("FogCoordhNV"sv, check_errors);
}

template<class span_const_GLhalfNV>
requires(
    concepts::span<span_const_GLhalfNV> &&
    std::is_same_v<
        std::decay_t<typename span_const_GLhalfNV::value_type>,
        std::decay_t<GLhalfNV>>)
/*!
 * \brief Part of GL_NV_half_float
 * \param fog const GLhalfNV *
 * \return void
 */
STATICINLINE void fog_coordhv(
    span_const_GLhalfNV const& fog, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(FogCoordhvNV)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE_DATA(
        glFogCoordhvNV,
        gsl::span<const char>(
            reinterpret_cast<const char*>(fog.data()), fog.size_bytes()));
#endif
    glFogCoordhvNV(
        fog.size() ? reinterpret_cast<const GLhalfNV*>(fog.data()) : nullptr);
    detail::error_check("FogCoordhvNV"sv, check_errors);
}

template<class vec_3_GLhalfNV>
requires(concepts::vector<vec_3_GLhalfNV, GLhalfNV, 3>)
/*!
 * \brief Part of GL_NV_half_float
 * \param red GLhalfNV
 * \param green GLhalfNV
 * \param blue GLhalfNV
 * \return void
 */
STATICINLINE void secondary_color3h(
    vec_3_GLhalfNV const& red, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(SecondaryColor3hNV)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glSecondaryColor3hNV, red);
#endif
    glSecondaryColor3hNV(red[0], red[1], red[2]);
    detail::error_check("SecondaryColor3hNV"sv, check_errors);
}

template<class span_const_GLhalfNV>
requires(
    concepts::span<span_const_GLhalfNV> &&
    std::is_same_v<
        std::decay_t<typename span_const_GLhalfNV::value_type>,
        std::decay_t<GLhalfNV>>)
/*!
 * \brief Part of GL_NV_half_float
 * \param v const GLhalfNV *
 * \return void
 */
STATICINLINE void secondary_color3hv(
    span_const_GLhalfNV const& v, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(SecondaryColor3hvNV)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE_DATA(
        glSecondaryColor3hvNV,
        gsl::span<const char>(
            reinterpret_cast<const char*>(v.data()), v.size_bytes()));
#endif
    glSecondaryColor3hvNV(
        v.size() ? reinterpret_cast<const GLhalfNV*>(v.data()) : nullptr);
    detail::error_check("SecondaryColor3hvNV"sv, check_errors);
}

/*!
 * \brief Part of GL_NV_half_float
 * \param weight GLhalfNV
 * \return void
 */
STATICINLINE void vertex_weighth(
    GLhalfNV weight, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexWeighthNV)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glVertexWeighthNV, weight);
#endif
    glVertexWeighthNV(weight);
    detail::error_check("VertexWeighthNV"sv, check_errors);
}

template<class span_const_GLhalfNV>
requires(
    concepts::span<span_const_GLhalfNV> &&
    std::is_same_v<
        std::decay_t<typename span_const_GLhalfNV::value_type>,
        std::decay_t<GLhalfNV>>)
/*!
 * \brief Part of GL_NV_half_float
 * \param weight const GLhalfNV *
 * \return void
 */
STATICINLINE void vertex_weighthv(
    span_const_GLhalfNV const& weight,
    error_check                check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexWeighthvNV)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE_DATA(
        glVertexWeighthvNV,
        gsl::span<const char>(
            reinterpret_cast<const char*>(weight.data()), weight.size_bytes()));
#endif
    glVertexWeighthvNV(
        weight.size() ? reinterpret_cast<const GLhalfNV*>(weight.data())
                      : nullptr);
    detail::error_check("VertexWeighthvNV"sv, check_errors);
}

} // namespace gl::nv::half_float
#endif // GL_NV_half_float
namespace gl::nv::half_float {
constexpr auto name = "GL_NV_half_float";
}
