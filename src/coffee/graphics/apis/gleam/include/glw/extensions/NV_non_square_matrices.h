#pragma once

#ifdef GL_NV_non_square_matrices
#include "../enums/AttributeType.h"
namespace gl::nv::non_square_matrices {
using gl::group::attribute_type;
namespace values {
} // namespace values
template<class span_const_mat_2x3_f32>
requires(
    semantic::concepts::Span<span_const_mat_2x3_f32> &&
    semantic::concepts::
        Matrix<typename span_const_mat_2x3_f32::value_type, f32, 2, 3>)
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
    i32                           count,
    bool                          transpose,
    span_const_mat_2x3_f32 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(UniformMatrix2x3fvNV)
    }
    glUniformMatrix2x3fvNV(
        location,
        count,
        transpose,
        reinterpret_cast<const GLfloat*>(value.data()));
    detail::error_check("UniformMatrix2x3fvNV"sv);
}

template<class span_const_mat_2x4_f32>
requires(
    semantic::concepts::Span<span_const_mat_2x4_f32> &&
    semantic::concepts::
        Matrix<typename span_const_mat_2x4_f32::value_type, f32, 2, 4>)
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
    i32                           count,
    bool                          transpose,
    span_const_mat_2x4_f32 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(UniformMatrix2x4fvNV)
    }
    glUniformMatrix2x4fvNV(
        location,
        count,
        transpose,
        reinterpret_cast<const GLfloat*>(value.data()));
    detail::error_check("UniformMatrix2x4fvNV"sv);
}

template<class span_const_mat_3x2_f32>
requires(
    semantic::concepts::Span<span_const_mat_3x2_f32> &&
    semantic::concepts::
        Matrix<typename span_const_mat_3x2_f32::value_type, f32, 3, 2>)
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
    i32                           count,
    bool                          transpose,
    span_const_mat_3x2_f32 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(UniformMatrix3x2fvNV)
    }
    glUniformMatrix3x2fvNV(
        location,
        count,
        transpose,
        reinterpret_cast<const GLfloat*>(value.data()));
    detail::error_check("UniformMatrix3x2fvNV"sv);
}

template<class span_const_mat_3x4_f32>
requires(
    semantic::concepts::Span<span_const_mat_3x4_f32> &&
    semantic::concepts::
        Matrix<typename span_const_mat_3x4_f32::value_type, f32, 3, 4>)
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
    i32                           count,
    bool                          transpose,
    span_const_mat_3x4_f32 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(UniformMatrix3x4fvNV)
    }
    glUniformMatrix3x4fvNV(
        location,
        count,
        transpose,
        reinterpret_cast<const GLfloat*>(value.data()));
    detail::error_check("UniformMatrix3x4fvNV"sv);
}

template<class span_const_mat_4x2_f32>
requires(
    semantic::concepts::Span<span_const_mat_4x2_f32> &&
    semantic::concepts::
        Matrix<typename span_const_mat_4x2_f32::value_type, f32, 4, 2>)
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
    i32                           count,
    bool                          transpose,
    span_const_mat_4x2_f32 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(UniformMatrix4x2fvNV)
    }
    glUniformMatrix4x2fvNV(
        location,
        count,
        transpose,
        reinterpret_cast<const GLfloat*>(value.data()));
    detail::error_check("UniformMatrix4x2fvNV"sv);
}

template<class span_const_mat_4x3_f32>
requires(
    semantic::concepts::Span<span_const_mat_4x3_f32> &&
    semantic::concepts::
        Matrix<typename span_const_mat_4x3_f32::value_type, f32, 4, 3>)
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
    i32                           count,
    bool                          transpose,
    span_const_mat_4x3_f32 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(UniformMatrix4x3fvNV)
    }
    glUniformMatrix4x3fvNV(
        location,
        count,
        transpose,
        reinterpret_cast<const GLfloat*>(value.data()));
    detail::error_check("UniformMatrix4x3fvNV"sv);
}

} // namespace gl::nv::non_square_matrices
#endif // GL_NV_non_square_matrices
namespace gl::nv::non_square_matrices {
constexpr auto name = "GL_NV_non_square_matrices";
}
