#pragma once

#ifdef GL_NV_non_square_matrices
#include "../enums/AttributeType.h"

namespace gl::nv::non_square_matrices {
using gl::group::attribute_type;

namespace values {
} // namespace values

template<class span_const_mat_2x3_f32>
requires(
    concepts::span<span_const_mat_2x3_f32> &&
    concepts::matrix<typename span_const_mat_2x3_f32::value_type, f32, 2, 3>)
/*!
 * \brief Part of GL_NV_non_square_matrices
 * \param location GLint
 * \param count GLsizei
 * \param transpose GLboolean
 * \param value const GLfloat *
 * \return void
 */
STATICINLINE void uniform(
    i32                           location,
    bool                          transpose,
    span_const_mat_2x3_f32 const& value,
    error_check                   check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(UniformMatrix2x3fvNV)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glUniformMatrix2x3fvNV,
        gsl::span<const char>(
            reinterpret_cast<const char*>(value.data()), value.size_bytes()),
        location,
        transpose);
#endif
    glUniformMatrix2x3fvNV(
        location,
        value.size(),
        transpose,
        reinterpret_cast<const GLfloat*>(value.data()));
    detail::error_check("UniformMatrix2x3fvNV"sv, check_errors);
}

template<class span_const_mat_2x4_f32>
requires(
    concepts::span<span_const_mat_2x4_f32> &&
    concepts::matrix<typename span_const_mat_2x4_f32::value_type, f32, 2, 4>)
/*!
 * \brief Part of GL_NV_non_square_matrices
 * \param location GLint
 * \param count GLsizei
 * \param transpose GLboolean
 * \param value const GLfloat *
 * \return void
 */
STATICINLINE void uniform(
    i32                           location,
    bool                          transpose,
    span_const_mat_2x4_f32 const& value,
    error_check                   check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(UniformMatrix2x4fvNV)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glUniformMatrix2x4fvNV,
        gsl::span<const char>(
            reinterpret_cast<const char*>(value.data()), value.size_bytes()),
        location,
        transpose);
#endif
    glUniformMatrix2x4fvNV(
        location,
        value.size(),
        transpose,
        reinterpret_cast<const GLfloat*>(value.data()));
    detail::error_check("UniformMatrix2x4fvNV"sv, check_errors);
}

template<class span_const_mat_3x2_f32>
requires(
    concepts::span<span_const_mat_3x2_f32> &&
    concepts::matrix<typename span_const_mat_3x2_f32::value_type, f32, 3, 2>)
/*!
 * \brief Part of GL_NV_non_square_matrices
 * \param location GLint
 * \param count GLsizei
 * \param transpose GLboolean
 * \param value const GLfloat *
 * \return void
 */
STATICINLINE void uniform(
    i32                           location,
    bool                          transpose,
    span_const_mat_3x2_f32 const& value,
    error_check                   check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(UniformMatrix3x2fvNV)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glUniformMatrix3x2fvNV,
        gsl::span<const char>(
            reinterpret_cast<const char*>(value.data()), value.size_bytes()),
        location,
        transpose);
#endif
    glUniformMatrix3x2fvNV(
        location,
        value.size(),
        transpose,
        reinterpret_cast<const GLfloat*>(value.data()));
    detail::error_check("UniformMatrix3x2fvNV"sv, check_errors);
}

template<class span_const_mat_3x4_f32>
requires(
    concepts::span<span_const_mat_3x4_f32> &&
    concepts::matrix<typename span_const_mat_3x4_f32::value_type, f32, 3, 4>)
/*!
 * \brief Part of GL_NV_non_square_matrices
 * \param location GLint
 * \param count GLsizei
 * \param transpose GLboolean
 * \param value const GLfloat *
 * \return void
 */
STATICINLINE void uniform(
    i32                           location,
    bool                          transpose,
    span_const_mat_3x4_f32 const& value,
    error_check                   check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(UniformMatrix3x4fvNV)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glUniformMatrix3x4fvNV,
        gsl::span<const char>(
            reinterpret_cast<const char*>(value.data()), value.size_bytes()),
        location,
        transpose);
#endif
    glUniformMatrix3x4fvNV(
        location,
        value.size(),
        transpose,
        reinterpret_cast<const GLfloat*>(value.data()));
    detail::error_check("UniformMatrix3x4fvNV"sv, check_errors);
}

template<class span_const_mat_4x2_f32>
requires(
    concepts::span<span_const_mat_4x2_f32> &&
    concepts::matrix<typename span_const_mat_4x2_f32::value_type, f32, 4, 2>)
/*!
 * \brief Part of GL_NV_non_square_matrices
 * \param location GLint
 * \param count GLsizei
 * \param transpose GLboolean
 * \param value const GLfloat *
 * \return void
 */
STATICINLINE void uniform(
    i32                           location,
    bool                          transpose,
    span_const_mat_4x2_f32 const& value,
    error_check                   check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(UniformMatrix4x2fvNV)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glUniformMatrix4x2fvNV,
        gsl::span<const char>(
            reinterpret_cast<const char*>(value.data()), value.size_bytes()),
        location,
        transpose);
#endif
    glUniformMatrix4x2fvNV(
        location,
        value.size(),
        transpose,
        reinterpret_cast<const GLfloat*>(value.data()));
    detail::error_check("UniformMatrix4x2fvNV"sv, check_errors);
}

template<class span_const_mat_4x3_f32>
requires(
    concepts::span<span_const_mat_4x3_f32> &&
    concepts::matrix<typename span_const_mat_4x3_f32::value_type, f32, 4, 3>)
/*!
 * \brief Part of GL_NV_non_square_matrices
 * \param location GLint
 * \param count GLsizei
 * \param transpose GLboolean
 * \param value const GLfloat *
 * \return void
 */
STATICINLINE void uniform(
    i32                           location,
    bool                          transpose,
    span_const_mat_4x3_f32 const& value,
    error_check                   check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(UniformMatrix4x3fvNV)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glUniformMatrix4x3fvNV,
        gsl::span<const char>(
            reinterpret_cast<const char*>(value.data()), value.size_bytes()),
        location,
        transpose);
#endif
    glUniformMatrix4x3fvNV(
        location,
        value.size(),
        transpose,
        reinterpret_cast<const GLfloat*>(value.data()));
    detail::error_check("UniformMatrix4x3fvNV"sv, check_errors);
}

} // namespace gl::nv::non_square_matrices
#endif // GL_NV_non_square_matrices
namespace gl::nv::non_square_matrices {
constexpr auto name = "GL_NV_non_square_matrices";
}
