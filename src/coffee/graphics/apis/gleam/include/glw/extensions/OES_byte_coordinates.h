#pragma once

#ifdef GL_OES_byte_coordinates
#include "../enums/BinormalPointerTypeEXT.h"
#include "../enums/ColorPointerType.h"
#include "../enums/ListNameType.h"
#include "../enums/NormalPointerType.h"
#include "../enums/PixelType.h"
#include "../enums/TangentPointerTypeEXT.h"
#include "../enums/VertexAttribIType.h"
#include "../enums/VertexAttribPointerType.h"
#include "../enums/VertexAttribType.h"
#include "../enums/WeightPointerTypeARB.h"

namespace gl::oes::byte_coordinates {
using gl::group::binormal_pointer_type_ext;
using gl::group::color_pointer_type;
using gl::group::list_name_type;
using gl::group::normal_pointer_type;
using gl::group::pixel_type;
using gl::group::tangent_pointer_type_ext;
using gl::group::vertex_attrib_int;
using gl::group::vertex_attrib_pointer_type;
using gl::group::vertex_attrib_type;
using gl::group::weight_pointer_type_arb;

namespace values {
} // namespace values
#if defined(GL_VERSION_1_0)
/*!
 * \brief Part of GL_OES_byte_coordinates
 * \param texture GLenum
 * \param s GLbyte
 * \return void
 */
STATICINLINE void multi_tex_coord1b(
    group::texture_unit texture,
    i8                  s,
    error_check         check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(MultiTexCoord1bOES)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glMultiTexCoord1bOES, texture, s);
#endif
    glMultiTexCoord1bOES(static_cast<GLenum>(texture), s);
    detail::error_check("MultiTexCoord1bOES"sv, check_errors);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_const_i8>
requires(
    concepts::span<span_const_i8> &&
    std::is_same_v<
        std::decay_t<typename span_const_i8::value_type>,
        std::decay_t<i8>>)
/*!
 * \brief Part of GL_OES_byte_coordinates
 * \param texture GLenum
 * \param coords const GLbyte *
 * \return void
 */
STATICINLINE void multi_tex_coord1bv(
    group::texture_unit  texture,
    span_const_i8 const& coords,
    error_check          check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(MultiTexCoord1bvOES)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glMultiTexCoord1bvOES,
        gsl::span<const char>(
            reinterpret_cast<const char*>(coords.data()), coords.size_bytes()),
        texture);
#endif
    glMultiTexCoord1bvOES(
        static_cast<GLenum>(texture),
        coords.size() ? reinterpret_cast<const GLbyte*>(coords.data())
                      : nullptr);
    detail::error_check("MultiTexCoord1bvOES"sv, check_errors);
}

#endif
#if defined(GL_VERSION_1_0)
/*!
 * \brief Part of GL_OES_byte_coordinates
 * \param texture GLenum
 * \param s GLbyte
 * \param t GLbyte
 * \return void
 */
STATICINLINE void multi_tex_coord2b(
    group::texture_unit texture,
    i8                  s,
    i8                  t,
    error_check         check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(MultiTexCoord2bOES)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glMultiTexCoord2bOES, texture, s, t);
#endif
    glMultiTexCoord2bOES(static_cast<GLenum>(texture), s, t);
    detail::error_check("MultiTexCoord2bOES"sv, check_errors);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_const_i8>
requires(
    concepts::span<span_const_i8> &&
    std::is_same_v<
        std::decay_t<typename span_const_i8::value_type>,
        std::decay_t<i8>>)
/*!
 * \brief Part of GL_OES_byte_coordinates
 * \param texture GLenum
 * \param coords const GLbyte *
 * \return void
 */
STATICINLINE void multi_tex_coord2bv(
    group::texture_unit  texture,
    span_const_i8 const& coords,
    error_check          check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(MultiTexCoord2bvOES)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glMultiTexCoord2bvOES,
        gsl::span<const char>(
            reinterpret_cast<const char*>(coords.data()), coords.size_bytes()),
        texture);
#endif
    glMultiTexCoord2bvOES(
        static_cast<GLenum>(texture),
        coords.size() ? reinterpret_cast<const GLbyte*>(coords.data())
                      : nullptr);
    detail::error_check("MultiTexCoord2bvOES"sv, check_errors);
}

#endif
#if defined(GL_VERSION_1_0)
/*!
 * \brief Part of GL_OES_byte_coordinates
 * \param texture GLenum
 * \param s GLbyte
 * \param t GLbyte
 * \param r GLbyte
 * \return void
 */
STATICINLINE void multi_tex_coord3b(
    group::texture_unit texture,
    i8                  s,
    i8                  t,
    i8                  r,
    error_check         check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(MultiTexCoord3bOES)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glMultiTexCoord3bOES, texture, s, t, r);
#endif
    glMultiTexCoord3bOES(static_cast<GLenum>(texture), s, t, r);
    detail::error_check("MultiTexCoord3bOES"sv, check_errors);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_const_i8>
requires(
    concepts::span<span_const_i8> &&
    std::is_same_v<
        std::decay_t<typename span_const_i8::value_type>,
        std::decay_t<i8>>)
/*!
 * \brief Part of GL_OES_byte_coordinates
 * \param texture GLenum
 * \param coords const GLbyte *
 * \return void
 */
STATICINLINE void multi_tex_coord3bv(
    group::texture_unit  texture,
    span_const_i8 const& coords,
    error_check          check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(MultiTexCoord3bvOES)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glMultiTexCoord3bvOES,
        gsl::span<const char>(
            reinterpret_cast<const char*>(coords.data()), coords.size_bytes()),
        texture);
#endif
    glMultiTexCoord3bvOES(
        static_cast<GLenum>(texture),
        coords.size() ? reinterpret_cast<const GLbyte*>(coords.data())
                      : nullptr);
    detail::error_check("MultiTexCoord3bvOES"sv, check_errors);
}

#endif
#if defined(GL_VERSION_1_0)
/*!
 * \brief Part of GL_OES_byte_coordinates
 * \param texture GLenum
 * \param s GLbyte
 * \param t GLbyte
 * \param r GLbyte
 * \param q GLbyte
 * \return void
 */
STATICINLINE void multi_tex_coord4b(
    group::texture_unit texture,
    i8                  s,
    i8                  t,
    i8                  r,
    i8                  q,
    error_check         check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(MultiTexCoord4bOES)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glMultiTexCoord4bOES, texture, s, t, r, q);
#endif
    glMultiTexCoord4bOES(static_cast<GLenum>(texture), s, t, r, q);
    detail::error_check("MultiTexCoord4bOES"sv, check_errors);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_const_i8>
requires(
    concepts::span<span_const_i8> &&
    std::is_same_v<
        std::decay_t<typename span_const_i8::value_type>,
        std::decay_t<i8>>)
/*!
 * \brief Part of GL_OES_byte_coordinates
 * \param texture GLenum
 * \param coords const GLbyte *
 * \return void
 */
STATICINLINE void multi_tex_coord4bv(
    group::texture_unit  texture,
    span_const_i8 const& coords,
    error_check          check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(MultiTexCoord4bvOES)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glMultiTexCoord4bvOES,
        gsl::span<const char>(
            reinterpret_cast<const char*>(coords.data()), coords.size_bytes()),
        texture);
#endif
    glMultiTexCoord4bvOES(
        static_cast<GLenum>(texture),
        coords.size() ? reinterpret_cast<const GLbyte*>(coords.data())
                      : nullptr);
    detail::error_check("MultiTexCoord4bvOES"sv, check_errors);
}

#endif
#if defined(GL_VERSION_1_0)
/*!
 * \brief Part of GL_OES_byte_coordinates
 * \param s GLbyte
 * \return void
 */
STATICINLINE void tex_coord1b(i8 s, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(TexCoord1bOES)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glTexCoord1bOES, s);
#endif
    glTexCoord1bOES(s);
    detail::error_check("TexCoord1bOES"sv, check_errors);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_const_i8>
requires(
    concepts::span<span_const_i8> &&
    std::is_same_v<
        std::decay_t<typename span_const_i8::value_type>,
        std::decay_t<i8>>)
/*!
 * \brief Part of GL_OES_byte_coordinates
 * \param coords const GLbyte *
 * \return void
 */
STATICINLINE void tex_coord1bv(
    span_const_i8 const& coords, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(TexCoord1bvOES)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glTexCoord1bvOES,
        gsl::span<const char>(
            reinterpret_cast<const char*>(coords.data()), coords.size_bytes()));
#endif
    glTexCoord1bvOES(
        coords.size() ? reinterpret_cast<const GLbyte*>(coords.data())
                      : nullptr);
    detail::error_check("TexCoord1bvOES"sv, check_errors);
}

#endif
#if defined(GL_VERSION_1_0)
/*!
 * \brief Part of GL_OES_byte_coordinates
 * \param s GLbyte
 * \param t GLbyte
 * \return void
 */
STATICINLINE void tex_coord2b(
    i8 s, i8 t, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(TexCoord2bOES)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glTexCoord2bOES, s, t);
#endif
    glTexCoord2bOES(s, t);
    detail::error_check("TexCoord2bOES"sv, check_errors);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_const_i8>
requires(
    concepts::span<span_const_i8> &&
    std::is_same_v<
        std::decay_t<typename span_const_i8::value_type>,
        std::decay_t<i8>>)
/*!
 * \brief Part of GL_OES_byte_coordinates
 * \param coords const GLbyte *
 * \return void
 */
STATICINLINE void tex_coord2bv(
    span_const_i8 const& coords, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(TexCoord2bvOES)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glTexCoord2bvOES,
        gsl::span<const char>(
            reinterpret_cast<const char*>(coords.data()), coords.size_bytes()));
#endif
    glTexCoord2bvOES(
        coords.size() ? reinterpret_cast<const GLbyte*>(coords.data())
                      : nullptr);
    detail::error_check("TexCoord2bvOES"sv, check_errors);
}

#endif
#if defined(GL_VERSION_1_0)
/*!
 * \brief Part of GL_OES_byte_coordinates
 * \param s GLbyte
 * \param t GLbyte
 * \param r GLbyte
 * \return void
 */
STATICINLINE void tex_coord3b(
    i8 s, i8 t, i8 r, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(TexCoord3bOES)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glTexCoord3bOES, s, t, r);
#endif
    glTexCoord3bOES(s, t, r);
    detail::error_check("TexCoord3bOES"sv, check_errors);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_const_i8>
requires(
    concepts::span<span_const_i8> &&
    std::is_same_v<
        std::decay_t<typename span_const_i8::value_type>,
        std::decay_t<i8>>)
/*!
 * \brief Part of GL_OES_byte_coordinates
 * \param coords const GLbyte *
 * \return void
 */
STATICINLINE void tex_coord3bv(
    span_const_i8 const& coords, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(TexCoord3bvOES)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glTexCoord3bvOES,
        gsl::span<const char>(
            reinterpret_cast<const char*>(coords.data()), coords.size_bytes()));
#endif
    glTexCoord3bvOES(
        coords.size() ? reinterpret_cast<const GLbyte*>(coords.data())
                      : nullptr);
    detail::error_check("TexCoord3bvOES"sv, check_errors);
}

#endif
#if defined(GL_VERSION_1_0)
/*!
 * \brief Part of GL_OES_byte_coordinates
 * \param s GLbyte
 * \param t GLbyte
 * \param r GLbyte
 * \param q GLbyte
 * \return void
 */
STATICINLINE void tex_coord4b(
    i8 s, i8 t, i8 r, i8 q, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(TexCoord4bOES)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glTexCoord4bOES, s, t, r, q);
#endif
    glTexCoord4bOES(s, t, r, q);
    detail::error_check("TexCoord4bOES"sv, check_errors);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_const_i8>
requires(
    concepts::span<span_const_i8> &&
    std::is_same_v<
        std::decay_t<typename span_const_i8::value_type>,
        std::decay_t<i8>>)
/*!
 * \brief Part of GL_OES_byte_coordinates
 * \param coords const GLbyte *
 * \return void
 */
STATICINLINE void tex_coord4bv(
    span_const_i8 const& coords, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(TexCoord4bvOES)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glTexCoord4bvOES,
        gsl::span<const char>(
            reinterpret_cast<const char*>(coords.data()), coords.size_bytes()));
#endif
    glTexCoord4bvOES(
        coords.size() ? reinterpret_cast<const GLbyte*>(coords.data())
                      : nullptr);
    detail::error_check("TexCoord4bvOES"sv, check_errors);
}

#endif
#if defined(GL_VERSION_1_0)
template<class vec_2_i8>
requires(concepts::vector<vec_2_i8, i8, 2>)
/*!
 * \brief Part of GL_OES_byte_coordinates
 * \param x GLbyte
 * \param y GLbyte
 * \return void
 */
STATICINLINE void vertex2b(
    vec_2_i8 const& x, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(Vertex2bOES)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glVertex2bOES, x);
#endif
    glVertex2bOES(x[0], x[1]);
    detail::error_check("Vertex2bOES"sv, check_errors);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_const_i8>
requires(
    concepts::span<span_const_i8> &&
    std::is_same_v<
        std::decay_t<typename span_const_i8::value_type>,
        std::decay_t<i8>>)
/*!
 * \brief Part of GL_OES_byte_coordinates
 * \param coords const GLbyte *
 * \return void
 */
STATICINLINE void vertex2bv(
    span_const_i8 const& coords, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(Vertex2bvOES)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glVertex2bvOES,
        gsl::span<const char>(
            reinterpret_cast<const char*>(coords.data()), coords.size_bytes()));
#endif
    glVertex2bvOES(
        coords.size() ? reinterpret_cast<const GLbyte*>(coords.data())
                      : nullptr);
    detail::error_check("Vertex2bvOES"sv, check_errors);
}

#endif
#if defined(GL_VERSION_1_0)
template<class vec_3_i8>
requires(concepts::vector<vec_3_i8, i8, 3>)
/*!
 * \brief Part of GL_OES_byte_coordinates
 * \param x GLbyte
 * \param y GLbyte
 * \param z GLbyte
 * \return void
 */
STATICINLINE void vertex3b(
    vec_3_i8 const& x, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(Vertex3bOES)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glVertex3bOES, x);
#endif
    glVertex3bOES(x[0], x[1], x[2]);
    detail::error_check("Vertex3bOES"sv, check_errors);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_const_i8>
requires(
    concepts::span<span_const_i8> &&
    std::is_same_v<
        std::decay_t<typename span_const_i8::value_type>,
        std::decay_t<i8>>)
/*!
 * \brief Part of GL_OES_byte_coordinates
 * \param coords const GLbyte *
 * \return void
 */
STATICINLINE void vertex3bv(
    span_const_i8 const& coords, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(Vertex3bvOES)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glVertex3bvOES,
        gsl::span<const char>(
            reinterpret_cast<const char*>(coords.data()), coords.size_bytes()));
#endif
    glVertex3bvOES(
        coords.size() ? reinterpret_cast<const GLbyte*>(coords.data())
                      : nullptr);
    detail::error_check("Vertex3bvOES"sv, check_errors);
}

#endif
#if defined(GL_VERSION_1_0)
template<class vec_4_i8>
requires(concepts::vector<vec_4_i8, i8, 4>)
/*!
 * \brief Part of GL_OES_byte_coordinates
 * \param x GLbyte
 * \param y GLbyte
 * \param z GLbyte
 * \param w GLbyte
 * \return void
 */
STATICINLINE void vertex4b(
    vec_4_i8 const& x, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(Vertex4bOES)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glVertex4bOES, x);
#endif
    glVertex4bOES(x[0], x[1], x[2], x[3]);
    detail::error_check("Vertex4bOES"sv, check_errors);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_const_i8>
requires(
    concepts::span<span_const_i8> &&
    std::is_same_v<
        std::decay_t<typename span_const_i8::value_type>,
        std::decay_t<i8>>)
/*!
 * \brief Part of GL_OES_byte_coordinates
 * \param coords const GLbyte *
 * \return void
 */
STATICINLINE void vertex4bv(
    span_const_i8 const& coords, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(Vertex4bvOES)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glVertex4bvOES,
        gsl::span<const char>(
            reinterpret_cast<const char*>(coords.data()), coords.size_bytes()));
#endif
    glVertex4bvOES(
        coords.size() ? reinterpret_cast<const GLbyte*>(coords.data())
                      : nullptr);
    detail::error_check("Vertex4bvOES"sv, check_errors);
}

#endif
} // namespace gl::oes::byte_coordinates
#endif // GL_OES_byte_coordinates
namespace gl::oes::byte_coordinates {
constexpr auto name = "GL_OES_byte_coordinates";
}
