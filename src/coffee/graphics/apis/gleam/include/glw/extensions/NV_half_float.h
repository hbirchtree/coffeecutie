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
STATICINLINE void color3h(vec_3_GLhalfNV const& red)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Color3hNV)
    }
    glColor3hNV(red[0], red[1], red[2]);
    detail::error_check("Color3hNV"sv);
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
STATICINLINE void color3hv(span_const_GLhalfNV const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Color3hvNV)
    }
    glColor3hvNV(
        v.size() ? reinterpret_cast<const GLhalfNV*>(v.data()) : nullptr);
    detail::error_check("Color3hvNV"sv);
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
STATICINLINE void color4h(vec_4_GLhalfNV const& red)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Color4hNV)
    }
    glColor4hNV(red[0], red[1], red[2], red[3]);
    detail::error_check("Color4hNV"sv);
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
STATICINLINE void color4hv(span_const_GLhalfNV const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Color4hvNV)
    }
    glColor4hvNV(
        v.size() ? reinterpret_cast<const GLhalfNV*>(v.data()) : nullptr);
    detail::error_check("Color4hvNV"sv);
}

/*!
 * \brief Part of GL_NV_half_float
 * \param fog GLhalfNV
 * \return void
 */
STATICINLINE void fog_coordh(GLhalfNV fog)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(FogCoordhNV)
    }
    glFogCoordhNV(fog);
    detail::error_check("FogCoordhNV"sv);
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
STATICINLINE void fog_coordhv(span_const_GLhalfNV const& fog)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(FogCoordhvNV)
    }
    glFogCoordhvNV(
        fog.size() ? reinterpret_cast<const GLhalfNV*>(fog.data()) : nullptr);
    detail::error_check("FogCoordhvNV"sv);
}

/*!
 * \brief Part of GL_NV_half_float
 * \param target GLenum
 * \param s GLhalfNV
 * \return void
 */
STATICINLINE void multi_tex_coord1h(group::texture_unit target, GLhalfNV s)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MultiTexCoord1hNV)
    }
    glMultiTexCoord1hNV(static_cast<GLenum>(target), s);
    detail::error_check("MultiTexCoord1hNV"sv);
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
    group::texture_unit target, span_const_GLhalfNV const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MultiTexCoord1hvNV)
    }
    glMultiTexCoord1hvNV(
        static_cast<GLenum>(target),
        v.size() ? reinterpret_cast<const GLhalfNV*>(v.data()) : nullptr);
    detail::error_check("MultiTexCoord1hvNV"sv);
}

/*!
 * \brief Part of GL_NV_half_float
 * \param target GLenum
 * \param s GLhalfNV
 * \param t GLhalfNV
 * \return void
 */
STATICINLINE void multi_tex_coord2h(
    group::texture_unit target, GLhalfNV s, GLhalfNV t)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MultiTexCoord2hNV)
    }
    glMultiTexCoord2hNV(static_cast<GLenum>(target), s, t);
    detail::error_check("MultiTexCoord2hNV"sv);
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
    group::texture_unit target, span_const_GLhalfNV const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MultiTexCoord2hvNV)
    }
    glMultiTexCoord2hvNV(
        static_cast<GLenum>(target),
        v.size() ? reinterpret_cast<const GLhalfNV*>(v.data()) : nullptr);
    detail::error_check("MultiTexCoord2hvNV"sv);
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
    group::texture_unit target, GLhalfNV s, GLhalfNV t, GLhalfNV r)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MultiTexCoord3hNV)
    }
    glMultiTexCoord3hNV(static_cast<GLenum>(target), s, t, r);
    detail::error_check("MultiTexCoord3hNV"sv);
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
    group::texture_unit target, span_const_GLhalfNV const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MultiTexCoord3hvNV)
    }
    glMultiTexCoord3hvNV(
        static_cast<GLenum>(target),
        v.size() ? reinterpret_cast<const GLhalfNV*>(v.data()) : nullptr);
    detail::error_check("MultiTexCoord3hvNV"sv);
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
    group::texture_unit target, GLhalfNV s, GLhalfNV t, GLhalfNV r, GLhalfNV q)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MultiTexCoord4hNV)
    }
    glMultiTexCoord4hNV(static_cast<GLenum>(target), s, t, r, q);
    detail::error_check("MultiTexCoord4hNV"sv);
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
    group::texture_unit target, span_const_GLhalfNV const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MultiTexCoord4hvNV)
    }
    glMultiTexCoord4hvNV(
        static_cast<GLenum>(target),
        v.size() ? reinterpret_cast<const GLhalfNV*>(v.data()) : nullptr);
    detail::error_check("MultiTexCoord4hvNV"sv);
}

/*!
 * \brief Part of GL_NV_half_float
 * \param nx GLhalfNV
 * \param ny GLhalfNV
 * \param nz GLhalfNV
 * \return void
 */
STATICINLINE void normal3h(GLhalfNV nx, GLhalfNV ny, GLhalfNV nz)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Normal3hNV)
    }
    glNormal3hNV(nx, ny, nz);
    detail::error_check("Normal3hNV"sv);
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
STATICINLINE void normal3hv(span_const_GLhalfNV const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Normal3hvNV)
    }
    glNormal3hvNV(
        v.size() ? reinterpret_cast<const GLhalfNV*>(v.data()) : nullptr);
    detail::error_check("Normal3hvNV"sv);
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
STATICINLINE void secondary_color3h(vec_3_GLhalfNV const& red)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(SecondaryColor3hNV)
    }
    glSecondaryColor3hNV(red[0], red[1], red[2]);
    detail::error_check("SecondaryColor3hNV"sv);
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
STATICINLINE void secondary_color3hv(span_const_GLhalfNV const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(SecondaryColor3hvNV)
    }
    glSecondaryColor3hvNV(
        v.size() ? reinterpret_cast<const GLhalfNV*>(v.data()) : nullptr);
    detail::error_check("SecondaryColor3hvNV"sv);
}

/*!
 * \brief Part of GL_NV_half_float
 * \param s GLhalfNV
 * \return void
 */
STATICINLINE void tex_coord1h(GLhalfNV s)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TexCoord1hNV)
    }
    glTexCoord1hNV(s);
    detail::error_check("TexCoord1hNV"sv);
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
STATICINLINE void tex_coord1hv(span_const_GLhalfNV const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TexCoord1hvNV)
    }
    glTexCoord1hvNV(
        v.size() ? reinterpret_cast<const GLhalfNV*>(v.data()) : nullptr);
    detail::error_check("TexCoord1hvNV"sv);
}

/*!
 * \brief Part of GL_NV_half_float
 * \param s GLhalfNV
 * \param t GLhalfNV
 * \return void
 */
STATICINLINE void tex_coord2h(GLhalfNV s, GLhalfNV t)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TexCoord2hNV)
    }
    glTexCoord2hNV(s, t);
    detail::error_check("TexCoord2hNV"sv);
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
STATICINLINE void tex_coord2hv(span_const_GLhalfNV const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TexCoord2hvNV)
    }
    glTexCoord2hvNV(
        v.size() ? reinterpret_cast<const GLhalfNV*>(v.data()) : nullptr);
    detail::error_check("TexCoord2hvNV"sv);
}

/*!
 * \brief Part of GL_NV_half_float
 * \param s GLhalfNV
 * \param t GLhalfNV
 * \param r GLhalfNV
 * \return void
 */
STATICINLINE void tex_coord3h(GLhalfNV s, GLhalfNV t, GLhalfNV r)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TexCoord3hNV)
    }
    glTexCoord3hNV(s, t, r);
    detail::error_check("TexCoord3hNV"sv);
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
STATICINLINE void tex_coord3hv(span_const_GLhalfNV const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TexCoord3hvNV)
    }
    glTexCoord3hvNV(
        v.size() ? reinterpret_cast<const GLhalfNV*>(v.data()) : nullptr);
    detail::error_check("TexCoord3hvNV"sv);
}

/*!
 * \brief Part of GL_NV_half_float
 * \param s GLhalfNV
 * \param t GLhalfNV
 * \param r GLhalfNV
 * \param q GLhalfNV
 * \return void
 */
STATICINLINE void tex_coord4h(GLhalfNV s, GLhalfNV t, GLhalfNV r, GLhalfNV q)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TexCoord4hNV)
    }
    glTexCoord4hNV(s, t, r, q);
    detail::error_check("TexCoord4hNV"sv);
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
STATICINLINE void tex_coord4hv(span_const_GLhalfNV const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TexCoord4hvNV)
    }
    glTexCoord4hvNV(
        v.size() ? reinterpret_cast<const GLhalfNV*>(v.data()) : nullptr);
    detail::error_check("TexCoord4hvNV"sv);
}

template<class vec_2_GLhalfNV>
requires(concepts::vector<vec_2_GLhalfNV, GLhalfNV, 2>)
/*!
 * \brief Part of GL_NV_half_float
 * \param x GLhalfNV
 * \param y GLhalfNV
 * \return void
 */
STATICINLINE void vertex2h(vec_2_GLhalfNV const& x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Vertex2hNV)
    }
    glVertex2hNV(x[0], x[1]);
    detail::error_check("Vertex2hNV"sv);
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
STATICINLINE void vertex2hv(span_const_GLhalfNV const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Vertex2hvNV)
    }
    glVertex2hvNV(
        v.size() ? reinterpret_cast<const GLhalfNV*>(v.data()) : nullptr);
    detail::error_check("Vertex2hvNV"sv);
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
STATICINLINE void vertex3h(vec_3_GLhalfNV const& x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Vertex3hNV)
    }
    glVertex3hNV(x[0], x[1], x[2]);
    detail::error_check("Vertex3hNV"sv);
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
STATICINLINE void vertex3hv(span_const_GLhalfNV const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Vertex3hvNV)
    }
    glVertex3hvNV(
        v.size() ? reinterpret_cast<const GLhalfNV*>(v.data()) : nullptr);
    detail::error_check("Vertex3hvNV"sv);
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
STATICINLINE void vertex4h(vec_4_GLhalfNV const& x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Vertex4hNV)
    }
    glVertex4hNV(x[0], x[1], x[2], x[3]);
    detail::error_check("Vertex4hNV"sv);
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
STATICINLINE void vertex4hv(span_const_GLhalfNV const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Vertex4hvNV)
    }
    glVertex4hvNV(
        v.size() ? reinterpret_cast<const GLhalfNV*>(v.data()) : nullptr);
    detail::error_check("Vertex4hvNV"sv);
}

/*!
 * \brief Part of GL_NV_half_float
 * \param index GLuint
 * \param x GLhalfNV
 * \return void
 */
STATICINLINE void vertex_attrib1h(u32 index, GLhalfNV x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttrib1hNV)
    }
    glVertexAttrib1hNV(index, x);
    detail::error_check("VertexAttrib1hNV"sv);
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
STATICINLINE void vertex_attrib1hv(u32 index, span_const_GLhalfNV const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttrib1hvNV)
    }
    glVertexAttrib1hvNV(
        index,
        v.size() ? reinterpret_cast<const GLhalfNV*>(v.data()) : nullptr);
    detail::error_check("VertexAttrib1hvNV"sv);
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
STATICINLINE void vertex_attrib2h(u32 index, vec_2_GLhalfNV const& x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttrib2hNV)
    }
    glVertexAttrib2hNV(index, x[0], x[1]);
    detail::error_check("VertexAttrib2hNV"sv);
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
STATICINLINE void vertex_attrib2hv(u32 index, span_const_GLhalfNV const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttrib2hvNV)
    }
    glVertexAttrib2hvNV(
        index,
        v.size() ? reinterpret_cast<const GLhalfNV*>(v.data()) : nullptr);
    detail::error_check("VertexAttrib2hvNV"sv);
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
STATICINLINE void vertex_attrib3h(u32 index, vec_3_GLhalfNV const& x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttrib3hNV)
    }
    glVertexAttrib3hNV(index, x[0], x[1], x[2]);
    detail::error_check("VertexAttrib3hNV"sv);
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
STATICINLINE void vertex_attrib3hv(u32 index, span_const_GLhalfNV const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttrib3hvNV)
    }
    glVertexAttrib3hvNV(
        index,
        v.size() ? reinterpret_cast<const GLhalfNV*>(v.data()) : nullptr);
    detail::error_check("VertexAttrib3hvNV"sv);
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
STATICINLINE void vertex_attrib4h(u32 index, vec_4_GLhalfNV const& x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttrib4hNV)
    }
    glVertexAttrib4hNV(index, x[0], x[1], x[2], x[3]);
    detail::error_check("VertexAttrib4hNV"sv);
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
STATICINLINE void vertex_attrib4hv(u32 index, span_const_GLhalfNV const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttrib4hvNV)
    }
    glVertexAttrib4hvNV(
        index,
        v.size() ? reinterpret_cast<const GLhalfNV*>(v.data()) : nullptr);
    detail::error_check("VertexAttrib4hvNV"sv);
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
STATICINLINE void vertex_attribs1hv(u32 index, span_const_GLhalfNV const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribs1hvNV)
    }
    glVertexAttribs1hvNV(
        index,
        v.size(),
        v.size() ? reinterpret_cast<const GLhalfNV*>(v.data()) : nullptr);
    detail::error_check("VertexAttribs1hvNV"sv);
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
STATICINLINE void vertex_attribs2hv(u32 index, span_const_GLhalfNV const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribs2hvNV)
    }
    glVertexAttribs2hvNV(
        index,
        v.size(),
        v.size() ? reinterpret_cast<const GLhalfNV*>(v.data()) : nullptr);
    detail::error_check("VertexAttribs2hvNV"sv);
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
STATICINLINE void vertex_attribs3hv(u32 index, span_const_GLhalfNV const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribs3hvNV)
    }
    glVertexAttribs3hvNV(
        index,
        v.size(),
        v.size() ? reinterpret_cast<const GLhalfNV*>(v.data()) : nullptr);
    detail::error_check("VertexAttribs3hvNV"sv);
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
STATICINLINE void vertex_attribs4hv(u32 index, span_const_GLhalfNV const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribs4hvNV)
    }
    glVertexAttribs4hvNV(
        index,
        v.size(),
        v.size() ? reinterpret_cast<const GLhalfNV*>(v.data()) : nullptr);
    detail::error_check("VertexAttribs4hvNV"sv);
}

/*!
 * \brief Part of GL_NV_half_float
 * \param weight GLhalfNV
 * \return void
 */
STATICINLINE void vertex_weighth(GLhalfNV weight)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexWeighthNV)
    }
    glVertexWeighthNV(weight);
    detail::error_check("VertexWeighthNV"sv);
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
STATICINLINE void vertex_weighthv(span_const_GLhalfNV const& weight)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexWeighthvNV)
    }
    glVertexWeighthvNV(
        weight.size() ? reinterpret_cast<const GLhalfNV*>(weight.data())
                      : nullptr);
    detail::error_check("VertexWeighthvNV"sv);
}

} // namespace gl::nv::half_float
#endif // GL_NV_half_float
namespace gl::nv::half_float {
constexpr auto name = "GL_NV_half_float";
}
