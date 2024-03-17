#pragma once

#ifdef GL_ARB_viewport_array
#include "../enums/EnableCap.h"
#include "../enums/GetPName.h"
#include "../enums/VertexProvokingMode.h"

namespace gl::arb::viewport_array {
using gl::group::enable_cap;
using gl::group::get_prop;
using gl::group::vertex_provoking_mode;

namespace values {
constexpr u32 undefined_vertex = 0x8260;
} // namespace values

template<class span_const_f64>
requires(
    concepts::span<span_const_f64> &&
    std::is_same_v<
        std::decay_t<typename span_const_f64::value_type>,
        std::decay_t<f64>>)
/*!
 * \brief Part of GL_ARB_viewport_array
 * \param first GLuint
 * \param count GLsizei
 * \param v const GLdouble *
 * \return void
 */
STATICINLINE void depth_range_arrayv(
    u32                   first,
    i32                   count,
    span_const_f64 const& v,
    error_check           check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DepthRangeArrayv)
    }
    glDepthRangeArrayv(
        first,
        count,
        v.size() ? reinterpret_cast<const GLdouble*>(v.data()) : nullptr);
    detail::error_check("DepthRangeArrayv"sv, check_errors);
}

/*!
 * \brief Part of GL_ARB_viewport_array
 * \param index GLuint
 * \param n GLdouble
 * \param f GLdouble
 * \return void
 */
STATICINLINE void depth_range_indexed(
    u32 index, f64 n, f64 f, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DepthRangeIndexed)
    }
    glDepthRangeIndexed(index, n, f);
    detail::error_check("DepthRangeIndexed"sv, check_errors);
}

template<class span_f64>
requires(
    concepts::span<span_f64> && std::is_same_v<
                                    std::decay_t<typename span_f64::value_type>,
                                    std::decay_t<f64>>)
/*!
 * \brief Part of GL_ARB_viewport_array
 * \param target GLenum
 * \param index GLuint
 * \param data GLdouble *
 * \return void
 */
STATICINLINE void get_doublei_v(
    group::get_prop target,
    u32             index,
    span_f64        data,
    error_check     check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetDoublei_v)
    }
    glGetDoublei_v(
        static_cast<GLenum>(target),
        index,
        data.size() ? reinterpret_cast<GLdouble*>(data.data()) : nullptr);
    detail::error_check("GetDoublei_v"sv, check_errors);
}

template<class span_f32>
requires(
    concepts::span<span_f32> && std::is_same_v<
                                    std::decay_t<typename span_f32::value_type>,
                                    std::decay_t<f32>>)
/*!
 * \brief Part of GL_ARB_viewport_array
 * \param target GLenum
 * \param index GLuint
 * \param data GLfloat *
 * \return void
 */
STATICINLINE void get_floati_v(
    group::get_prop target,
    u32             index,
    span_f32        data,
    error_check     check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetFloati_v)
    }
    glGetFloati_v(
        static_cast<GLenum>(target),
        index,
        data.size() ? reinterpret_cast<GLfloat*>(data.data()) : nullptr);
    detail::error_check("GetFloati_v"sv, check_errors);
}

template<class span_const_i32>
requires(
    concepts::span<span_const_i32> &&
    std::is_same_v<
        std::decay_t<typename span_const_i32::value_type>,
        std::decay_t<i32>>)
/*!
 * \brief Part of GL_ARB_viewport_array
 * \param first GLuint
 * \param count GLsizei
 * \param v const GLint *
 * \return void
 */
STATICINLINE void scissor_arrayv(
    u32                   first,
    i32                   count,
    span_const_i32 const& v,
    error_check           check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ScissorArrayv)
    }
    glScissorArrayv(
        first,
        count,
        v.size() ? reinterpret_cast<const GLint*>(v.data()) : nullptr);
    detail::error_check("ScissorArrayv"sv, check_errors);
}

template<class size_2_i32>
requires(concepts::size_2d<size_2_i32, i32>)
/*!
 * \brief Part of GL_ARB_viewport_array
 * \param index GLuint
 * \param left GLint
 * \param bottom GLint
 * \param width GLsizei
 * \param height GLsizei
 * \return void
 */
STATICINLINE void scissor_indexed(
    u32               index,
    i32               left,
    i32               bottom,
    size_2_i32 const& width,
    error_check       check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ScissorIndexed)
    }
    glScissorIndexed(index, left, bottom, width[0], width[1]);
    detail::error_check("ScissorIndexed"sv, check_errors);
}

template<class span_const_i32>
requires(
    concepts::span<span_const_i32> &&
    std::is_same_v<
        std::decay_t<typename span_const_i32::value_type>,
        std::decay_t<i32>>)
/*!
 * \brief Part of GL_ARB_viewport_array
 * \param index GLuint
 * \param v const GLint *
 * \return void
 */
STATICINLINE void scissor_indexedv(
    u32                   index,
    span_const_i32 const& v,
    error_check           check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ScissorIndexedv)
    }
    glScissorIndexedv(
        index, v.size() ? reinterpret_cast<const GLint*>(v.data()) : nullptr);
    detail::error_check("ScissorIndexedv"sv, check_errors);
}

template<class span_const_f32>
requires(
    concepts::span<span_const_f32> &&
    std::is_same_v<
        std::decay_t<typename span_const_f32::value_type>,
        std::decay_t<f32>>)
/*!
 * \brief Part of GL_ARB_viewport_array
 * \param first GLuint
 * \param count GLsizei
 * \param v const GLfloat *
 * \return void
 */
STATICINLINE void viewport_arrayv(
    u32                   first,
    i32                   count,
    span_const_f32 const& v,
    error_check           check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ViewportArrayv)
    }
    glViewportArrayv(
        first,
        count,
        v.size() ? reinterpret_cast<const GLfloat*>(v.data()) : nullptr);
    detail::error_check("ViewportArrayv"sv, check_errors);
}

template<class size_2_f32, class vec_2_f32>
requires(
    concepts::vector<vec_2_f32, f32, 2> && concepts::size_2d<size_2_f32, f32>)
/*!
 * \brief Part of GL_ARB_viewport_array
 * \param index GLuint
 * \param x GLfloat
 * \param y GLfloat
 * \param w GLfloat
 * \param h GLfloat
 * \return void
 */
STATICINLINE void viewport_indexedf(
    u32               index,
    vec_2_f32 const&  x,
    size_2_f32 const& w,
    error_check       check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ViewportIndexedf)
    }
    glViewportIndexedf(index, x[0], x[1], w[0], w[1]);
    detail::error_check("ViewportIndexedf"sv, check_errors);
}

template<class span_const_f32>
requires(
    concepts::span<span_const_f32> &&
    std::is_same_v<
        std::decay_t<typename span_const_f32::value_type>,
        std::decay_t<f32>>)
/*!
 * \brief Part of GL_ARB_viewport_array
 * \param index GLuint
 * \param v const GLfloat *
 * \return void
 */
STATICINLINE void viewport_indexedfv(
    u32                   index,
    span_const_f32 const& v,
    error_check           check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ViewportIndexedfv)
    }
    glViewportIndexedfv(
        index, v.size() ? reinterpret_cast<const GLfloat*>(v.data()) : nullptr);
    detail::error_check("ViewportIndexedfv"sv, check_errors);
}

template<class span_const_f64>
requires(
    concepts::span<span_const_f64> &&
    std::is_same_v<
        std::decay_t<typename span_const_f64::value_type>,
        std::decay_t<f64>>)
/*!
 * \brief Part of GL_ARB_viewport_array
 * \param first GLuint
 * \param count GLsizei
 * \param v const GLdouble *
 * \return void
 */
STATICINLINE void depth_range_arraydv_nv(
    u32                   first,
    i32                   count,
    span_const_f64 const& v,
    error_check           check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DepthRangeArraydvNV)
    }
    glDepthRangeArraydvNV(
        first,
        count,
        v.size() ? reinterpret_cast<const GLdouble*>(v.data()) : nullptr);
    detail::error_check("DepthRangeArraydvNV"sv, check_errors);
}

/*!
 * \brief Part of GL_ARB_viewport_array
 * \param index GLuint
 * \param n GLdouble
 * \param f GLdouble
 * \return void
 */
STATICINLINE void depth_range_indexedd_nv(
    u32 index, f64 n, f64 f, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DepthRangeIndexeddNV)
    }
    glDepthRangeIndexeddNV(index, n, f);
    detail::error_check("DepthRangeIndexeddNV"sv, check_errors);
}

} // namespace gl::arb::viewport_array
#endif // GL_ARB_viewport_array
namespace gl::arb::viewport_array {
constexpr auto name = "GL_ARB_viewport_array";
}
