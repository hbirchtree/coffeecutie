#pragma once

#ifdef GL_NV_vertex_attrib_integer_64bit
#include "../enums/AttributeType.h"
#include "../enums/VertexAttribPointerType.h"

namespace gl::nv::vertex_attrib_integer_64bit {
using gl::group::attribute_type;
using gl::group::vertex_attrib_pointer_type;

namespace values {
} // namespace values

template<class span_GLint64EXT>
requires(
    concepts::span<span_GLint64EXT> &&
    std::is_same_v<
        std::decay_t<typename span_GLint64EXT::value_type>,
        std::decay_t<GLint64EXT>>)
/*!
 * \brief Part of GL_NV_vertex_attrib_integer_64bit
 * \param index GLuint
 * \param pname GLenum
 * \param params GLint64EXT *
 * \return void
 */
STATICINLINE void get_vertex_attrib_li64v(
    u32                       index,
    group::vertex_attrib_enum pname,
    span_GLint64EXT           params,
    error_check               check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetVertexAttribLi64vNV)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glGetVertexAttribLi64vNV,
        gsl::span<char>(
            reinterpret_cast<char*>(params.data()), params.size_bytes()),
        index,
        pname);
#endif
    glGetVertexAttribLi64vNV(
        index,
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLint64EXT*>(params.data()) : nullptr);
    detail::error_check("GetVertexAttribLi64vNV"sv, check_errors);
}

template<class span_GLuint64EXT>
requires(
    concepts::span<span_GLuint64EXT> &&
    std::is_same_v<
        std::decay_t<typename span_GLuint64EXT::value_type>,
        std::decay_t<GLuint64EXT>>)
/*!
 * \brief Part of GL_NV_vertex_attrib_integer_64bit
 * \param index GLuint
 * \param pname GLenum
 * \param params GLuint64EXT *
 * \return void
 */
STATICINLINE void get_vertex_attrib_lui64v(
    u32                       index,
    group::vertex_attrib_enum pname,
    span_GLuint64EXT          params,
    error_check               check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetVertexAttribLui64vNV)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glGetVertexAttribLui64vNV,
        gsl::span<char>(
            reinterpret_cast<char*>(params.data()), params.size_bytes()),
        index,
        pname);
#endif
    glGetVertexAttribLui64vNV(
        index,
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLuint64EXT*>(params.data())
                      : nullptr);
    detail::error_check("GetVertexAttribLui64vNV"sv, check_errors);
}

/*!
 * \brief Part of GL_NV_vertex_attrib_integer_64bit
 * \param index GLuint
 * \param x GLint64EXT
 * \return void
 */
STATICINLINE void vertex_attrib_l1i64(
    u32 index, GLint64EXT x, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribL1i64NV)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glVertexAttribL1i64NV, index, x);
#endif
    glVertexAttribL1i64NV(index, x);
    detail::error_check("VertexAttribL1i64NV"sv, check_errors);
}

template<class span_const_GLint64EXT>
requires(
    concepts::span<span_const_GLint64EXT> &&
    std::is_same_v<
        std::decay_t<typename span_const_GLint64EXT::value_type>,
        std::decay_t<GLint64EXT>>)
/*!
 * \brief Part of GL_NV_vertex_attrib_integer_64bit
 * \param index GLuint
 * \param v const GLint64EXT *
 * \return void
 */
STATICINLINE void vertex_attrib_l1i64v(
    u32                          index,
    span_const_GLint64EXT const& v,
    error_check                  check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribL1i64vNV)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glVertexAttribL1i64vNV,
        gsl::span<const char>(
            reinterpret_cast<const char*>(v.data()), v.size_bytes()),
        index);
#endif
    glVertexAttribL1i64vNV(
        index, reinterpret_cast<const GLint64EXT*>(v.data()));
    detail::error_check("VertexAttribL1i64vNV"sv, check_errors);
}

/*!
 * \brief Part of GL_NV_vertex_attrib_integer_64bit
 * \param index GLuint
 * \param x GLuint64EXT
 * \return void
 */
STATICINLINE void vertex_attrib_l1ui64(
    u32 index, GLuint64EXT x, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribL1ui64NV)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glVertexAttribL1ui64NV, index, x);
#endif
    glVertexAttribL1ui64NV(index, x);
    detail::error_check("VertexAttribL1ui64NV"sv, check_errors);
}

template<class span_const_GLuint64EXT>
requires(
    concepts::span<span_const_GLuint64EXT> &&
    std::is_same_v<
        std::decay_t<typename span_const_GLuint64EXT::value_type>,
        std::decay_t<GLuint64EXT>>)
/*!
 * \brief Part of GL_NV_vertex_attrib_integer_64bit
 * \param index GLuint
 * \param v const GLuint64EXT *
 * \return void
 */
STATICINLINE void vertex_attrib_l1ui64v(
    u32                           index,
    span_const_GLuint64EXT const& v,
    error_check                   check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribL1ui64vNV)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glVertexAttribL1ui64vNV,
        gsl::span<const char>(
            reinterpret_cast<const char*>(v.data()), v.size_bytes()),
        index);
#endif
    glVertexAttribL1ui64vNV(
        index, reinterpret_cast<const GLuint64EXT*>(v.data()));
    detail::error_check("VertexAttribL1ui64vNV"sv, check_errors);
}

template<class vec_2_GLint64EXT>
requires(concepts::vector<vec_2_GLint64EXT, GLint64EXT, 2>)
/*!
 * \brief Part of GL_NV_vertex_attrib_integer_64bit
 * \param index GLuint
 * \param x GLint64EXT
 * \param y GLint64EXT
 * \return void
 */
STATICINLINE void vertex_attrib_l2i64(
    u32                     index,
    vec_2_GLint64EXT const& x,
    error_check             check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribL2i64NV)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glVertexAttribL2i64NV, index, x);
#endif
    glVertexAttribL2i64NV(index, x[0], x[1]);
    detail::error_check("VertexAttribL2i64NV"sv, check_errors);
}

template<class span_const_vec_2_GLint64EXT>
requires(
    concepts::span<span_const_vec_2_GLint64EXT> &&
    concepts::
        vector<typename span_const_vec_2_GLint64EXT::value_type, GLint64EXT, 2>)
/*!
 * \brief Part of GL_NV_vertex_attrib_integer_64bit
 * \param index GLuint
 * \param v const GLint64EXT *
 * \return void
 */
STATICINLINE void vertex_attrib_l2i64v(
    u32                                index,
    span_const_vec_2_GLint64EXT const& v,
    error_check                        check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribL2i64vNV)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glVertexAttribL2i64vNV,
        gsl::span<const char>(
            reinterpret_cast<const char*>(v.data()), v.size_bytes()),
        index);
#endif
    glVertexAttribL2i64vNV(
        index, reinterpret_cast<const GLint64EXT*>(v.data()));
    detail::error_check("VertexAttribL2i64vNV"sv, check_errors);
}

template<class vec_2_GLuint64EXT>
requires(concepts::vector<vec_2_GLuint64EXT, GLuint64EXT, 2>)
/*!
 * \brief Part of GL_NV_vertex_attrib_integer_64bit
 * \param index GLuint
 * \param x GLuint64EXT
 * \param y GLuint64EXT
 * \return void
 */
STATICINLINE void vertex_attrib_l2ui64(
    u32                      index,
    vec_2_GLuint64EXT const& x,
    error_check              check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribL2ui64NV)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glVertexAttribL2ui64NV, index, x);
#endif
    glVertexAttribL2ui64NV(index, x[0], x[1]);
    detail::error_check("VertexAttribL2ui64NV"sv, check_errors);
}

template<class span_const_vec_2_GLuint64EXT>
requires(
    concepts::span<span_const_vec_2_GLuint64EXT> &&
    concepts::vector<
        typename span_const_vec_2_GLuint64EXT::value_type,
        GLuint64EXT,
        2>)
/*!
 * \brief Part of GL_NV_vertex_attrib_integer_64bit
 * \param index GLuint
 * \param v const GLuint64EXT *
 * \return void
 */
STATICINLINE void vertex_attrib_l2ui64v(
    u32                                 index,
    span_const_vec_2_GLuint64EXT const& v,
    error_check                         check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribL2ui64vNV)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glVertexAttribL2ui64vNV,
        gsl::span<const char>(
            reinterpret_cast<const char*>(v.data()), v.size_bytes()),
        index);
#endif
    glVertexAttribL2ui64vNV(
        index, reinterpret_cast<const GLuint64EXT*>(v.data()));
    detail::error_check("VertexAttribL2ui64vNV"sv, check_errors);
}

template<class vec_3_GLint64EXT>
requires(concepts::vector<vec_3_GLint64EXT, GLint64EXT, 3>)
/*!
 * \brief Part of GL_NV_vertex_attrib_integer_64bit
 * \param index GLuint
 * \param x GLint64EXT
 * \param y GLint64EXT
 * \param z GLint64EXT
 * \return void
 */
STATICINLINE void vertex_attrib_l3i64(
    u32                     index,
    vec_3_GLint64EXT const& x,
    error_check             check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribL3i64NV)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glVertexAttribL3i64NV, index, x);
#endif
    glVertexAttribL3i64NV(index, x[0], x[1], x[2]);
    detail::error_check("VertexAttribL3i64NV"sv, check_errors);
}

template<class span_const_vec_3_GLint64EXT>
requires(
    concepts::span<span_const_vec_3_GLint64EXT> &&
    concepts::
        vector<typename span_const_vec_3_GLint64EXT::value_type, GLint64EXT, 3>)
/*!
 * \brief Part of GL_NV_vertex_attrib_integer_64bit
 * \param index GLuint
 * \param v const GLint64EXT *
 * \return void
 */
STATICINLINE void vertex_attrib_l3i64v(
    u32                                index,
    span_const_vec_3_GLint64EXT const& v,
    error_check                        check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribL3i64vNV)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glVertexAttribL3i64vNV,
        gsl::span<const char>(
            reinterpret_cast<const char*>(v.data()), v.size_bytes()),
        index);
#endif
    glVertexAttribL3i64vNV(
        index, reinterpret_cast<const GLint64EXT*>(v.data()));
    detail::error_check("VertexAttribL3i64vNV"sv, check_errors);
}

template<class vec_3_GLuint64EXT>
requires(concepts::vector<vec_3_GLuint64EXT, GLuint64EXT, 3>)
/*!
 * \brief Part of GL_NV_vertex_attrib_integer_64bit
 * \param index GLuint
 * \param x GLuint64EXT
 * \param y GLuint64EXT
 * \param z GLuint64EXT
 * \return void
 */
STATICINLINE void vertex_attrib_l3ui64(
    u32                      index,
    vec_3_GLuint64EXT const& x,
    error_check              check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribL3ui64NV)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glVertexAttribL3ui64NV, index, x);
#endif
    glVertexAttribL3ui64NV(index, x[0], x[1], x[2]);
    detail::error_check("VertexAttribL3ui64NV"sv, check_errors);
}

template<class span_const_vec_3_GLuint64EXT>
requires(
    concepts::span<span_const_vec_3_GLuint64EXT> &&
    concepts::vector<
        typename span_const_vec_3_GLuint64EXT::value_type,
        GLuint64EXT,
        3>)
/*!
 * \brief Part of GL_NV_vertex_attrib_integer_64bit
 * \param index GLuint
 * \param v const GLuint64EXT *
 * \return void
 */
STATICINLINE void vertex_attrib_l3ui64v(
    u32                                 index,
    span_const_vec_3_GLuint64EXT const& v,
    error_check                         check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribL3ui64vNV)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glVertexAttribL3ui64vNV,
        gsl::span<const char>(
            reinterpret_cast<const char*>(v.data()), v.size_bytes()),
        index);
#endif
    glVertexAttribL3ui64vNV(
        index, reinterpret_cast<const GLuint64EXT*>(v.data()));
    detail::error_check("VertexAttribL3ui64vNV"sv, check_errors);
}

template<class vec_4_GLint64EXT>
requires(concepts::vector<vec_4_GLint64EXT, GLint64EXT, 4>)
/*!
 * \brief Part of GL_NV_vertex_attrib_integer_64bit
 * \param index GLuint
 * \param x GLint64EXT
 * \param y GLint64EXT
 * \param z GLint64EXT
 * \param w GLint64EXT
 * \return void
 */
STATICINLINE void vertex_attrib_l4i64(
    u32                     index,
    vec_4_GLint64EXT const& x,
    error_check             check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribL4i64NV)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glVertexAttribL4i64NV, index, x);
#endif
    glVertexAttribL4i64NV(index, x[0], x[1], x[2], x[3]);
    detail::error_check("VertexAttribL4i64NV"sv, check_errors);
}

template<class span_const_vec_4_GLint64EXT>
requires(
    concepts::span<span_const_vec_4_GLint64EXT> &&
    concepts::
        vector<typename span_const_vec_4_GLint64EXT::value_type, GLint64EXT, 4>)
/*!
 * \brief Part of GL_NV_vertex_attrib_integer_64bit
 * \param index GLuint
 * \param v const GLint64EXT *
 * \return void
 */
STATICINLINE void vertex_attrib_l4i64v(
    u32                                index,
    span_const_vec_4_GLint64EXT const& v,
    error_check                        check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribL4i64vNV)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glVertexAttribL4i64vNV,
        gsl::span<const char>(
            reinterpret_cast<const char*>(v.data()), v.size_bytes()),
        index);
#endif
    glVertexAttribL4i64vNV(
        index, reinterpret_cast<const GLint64EXT*>(v.data()));
    detail::error_check("VertexAttribL4i64vNV"sv, check_errors);
}

template<class vec_4_GLuint64EXT>
requires(concepts::vector<vec_4_GLuint64EXT, GLuint64EXT, 4>)
/*!
 * \brief Part of GL_NV_vertex_attrib_integer_64bit
 * \param index GLuint
 * \param x GLuint64EXT
 * \param y GLuint64EXT
 * \param z GLuint64EXT
 * \param w GLuint64EXT
 * \return void
 */
STATICINLINE void vertex_attrib_l4ui64(
    u32                      index,
    vec_4_GLuint64EXT const& x,
    error_check              check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribL4ui64NV)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glVertexAttribL4ui64NV, index, x);
#endif
    glVertexAttribL4ui64NV(index, x[0], x[1], x[2], x[3]);
    detail::error_check("VertexAttribL4ui64NV"sv, check_errors);
}

template<class span_const_vec_4_GLuint64EXT>
requires(
    concepts::span<span_const_vec_4_GLuint64EXT> &&
    concepts::vector<
        typename span_const_vec_4_GLuint64EXT::value_type,
        GLuint64EXT,
        4>)
/*!
 * \brief Part of GL_NV_vertex_attrib_integer_64bit
 * \param index GLuint
 * \param v const GLuint64EXT *
 * \return void
 */
STATICINLINE void vertex_attrib_l4ui64v(
    u32                                 index,
    span_const_vec_4_GLuint64EXT const& v,
    error_check                         check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribL4ui64vNV)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glVertexAttribL4ui64vNV,
        gsl::span<const char>(
            reinterpret_cast<const char*>(v.data()), v.size_bytes()),
        index);
#endif
    glVertexAttribL4ui64vNV(
        index, reinterpret_cast<const GLuint64EXT*>(v.data()));
    detail::error_check("VertexAttribL4ui64vNV"sv, check_errors);
}

/*!
 * \brief Part of GL_NV_vertex_attrib_integer_64bit
 * \param index GLuint
 * \param size GLint
 * \param type GLenum
 * \param stride GLsizei
 * \return void
 */
STATICINLINE void vertex_attrib_l_format(
    u32                       index,
    i32                       size,
    group::vertex_attrib_long type,
    i32                       stride,
    error_check               check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribLFormatNV)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glVertexAttribLFormatNV, index, size, type, stride);
#endif
    glVertexAttribLFormatNV(index, size, static_cast<GLenum>(type), stride);
    detail::error_check("VertexAttribLFormatNV"sv, check_errors);
}

} // namespace gl::nv::vertex_attrib_integer_64bit
#endif // GL_NV_vertex_attrib_integer_64bit
namespace gl::nv::vertex_attrib_integer_64bit {
constexpr auto name = "GL_NV_vertex_attrib_integer_64bit";
}
