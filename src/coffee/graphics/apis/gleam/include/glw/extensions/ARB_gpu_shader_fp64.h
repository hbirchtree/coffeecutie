#pragma once

#ifdef GL_ARB_gpu_shader_fp64
#include "../enums/AttributeType.h"
#include "../enums/BinormalPointerTypeEXT.h"
#include "../enums/FogCoordinatePointerType.h"
#include "../enums/FogPointerTypeEXT.h"
#include "../enums/FogPointerTypeIBM.h"
#include "../enums/IndexPointerType.h"
#include "../enums/MapTypeNV.h"
#include "../enums/NormalPointerType.h"
#include "../enums/SecondaryColorPointerTypeIBM.h"
#include "../enums/TangentPointerTypeEXT.h"
#include "../enums/TexCoordPointerType.h"
#include "../enums/UniformType.h"
#include "../enums/VertexAttribLType.h"
#include "../enums/VertexAttribPointerType.h"
#include "../enums/VertexAttribType.h"
#include "../enums/VertexPointerType.h"
#include "../enums/WeightPointerTypeARB.h"

namespace gl::arb::gpu_shader_fp64 {
using gl::group::attribute_type;
using gl::group::binormal_pointer_type_ext;
using gl::group::fog_coordinate_pointer_type;
using gl::group::fog_pointer_type_ext;
using gl::group::fog_pointer_type_ibm;
using gl::group::index_pointer_type;
using gl::group::map_type_nv;
using gl::group::normal_pointer_type;
using gl::group::secondary_color_pointer_type_ibm;
using gl::group::tangent_pointer_type_ext;
using gl::group::tex_coord_pointer_type;
using gl::group::uniform_type;
using gl::group::vertex_attrib_long;
using gl::group::vertex_attrib_pointer_type;
using gl::group::vertex_attrib_type;
using gl::group::vertex_pointer_type;
using gl::group::weight_pointer_type_arb;

namespace values {
} // namespace values

template<class span_f64>
requires(
    concepts::span<span_f64> && std::is_same_v<
                                    std::decay_t<typename span_f64::value_type>,
                                    std::decay_t<f64>>)
/*!
 * \brief Part of GL_ARB_gpu_shader_fp64
 * \param program GLuint
 * \param location GLint
 * \param params GLdouble *
 * \return void
 */
STATICINLINE void get_uniformdv(u32 program, i32 location, span_f64 params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetUniformdv)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glGetUniformdv(
        program,
        location,
        params.size() ? reinterpret_cast<GLdouble*>(params.data()) : nullptr);
    detail::error_check("GetUniformdv"sv);
}

/*!
 * \brief Part of GL_ARB_gpu_shader_fp64
 * \param location GLint
 * \param x GLdouble
 * \return void
 */
STATICINLINE void uniform(i32 location, f64 x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform1d)
    }
    glUniform1d(location, x);
    detail::error_check("Uniform1d"sv);
}

template<class span_const_f64>
requires(
    concepts::span<span_const_f64> &&
    std::is_same_v<
        std::decay_t<typename span_const_f64::value_type>,
        std::decay_t<f64>>)
/*!
 * \brief Part of GL_ARB_gpu_shader_fp64
 * \param location GLint
 * \param count GLsizei
 * \param value const GLdouble *
 * \return void
 */
STATICINLINE void uniform(i32 location, span_const_f64 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform1dv)
    }
    glUniform1dv(
        location,
        value.size(),
        reinterpret_cast<const GLdouble*>(value.data()));
    detail::error_check("Uniform1dv"sv);
}

template<class vec_2_f64>
requires(concepts::vector<vec_2_f64, f64, 2>)
/*!
 * \brief Part of GL_ARB_gpu_shader_fp64
 * \param location GLint
 * \param x GLdouble
 * \param y GLdouble
 * \return void
 */
STATICINLINE void uniform(i32 location, vec_2_f64 const& x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform2d)
    }
    glUniform2d(location, x[0], x[1]);
    detail::error_check("Uniform2d"sv);
}

template<class span_const_vec_2_f64>
requires(
    concepts::span<span_const_vec_2_f64> &&
    concepts::vector<typename span_const_vec_2_f64::value_type, f64, 2>)
/*!
 * \brief Part of GL_ARB_gpu_shader_fp64
 * \param location GLint
 * \param count GLsizei
 * \param value const GLdouble *
 * \return void
 */
STATICINLINE void uniform(i32 location, span_const_vec_2_f64 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform2dv)
    }
    glUniform2dv(
        location,
        value.size(),
        reinterpret_cast<const GLdouble*>(value.data()));
    detail::error_check("Uniform2dv"sv);
}

template<class vec_3_f64>
requires(concepts::vector<vec_3_f64, f64, 3>)
/*!
 * \brief Part of GL_ARB_gpu_shader_fp64
 * \param location GLint
 * \param x GLdouble
 * \param y GLdouble
 * \param z GLdouble
 * \return void
 */
STATICINLINE void uniform(i32 location, vec_3_f64 const& x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform3d)
    }
    glUniform3d(location, x[0], x[1], x[2]);
    detail::error_check("Uniform3d"sv);
}

template<class span_const_vec_3_f64>
requires(
    concepts::span<span_const_vec_3_f64> &&
    concepts::vector<typename span_const_vec_3_f64::value_type, f64, 3>)
/*!
 * \brief Part of GL_ARB_gpu_shader_fp64
 * \param location GLint
 * \param count GLsizei
 * \param value const GLdouble *
 * \return void
 */
STATICINLINE void uniform(i32 location, span_const_vec_3_f64 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform3dv)
    }
    glUniform3dv(
        location,
        value.size(),
        reinterpret_cast<const GLdouble*>(value.data()));
    detail::error_check("Uniform3dv"sv);
}

template<class vec_4_f64>
requires(concepts::vector<vec_4_f64, f64, 4>)
/*!
 * \brief Part of GL_ARB_gpu_shader_fp64
 * \param location GLint
 * \param x GLdouble
 * \param y GLdouble
 * \param z GLdouble
 * \param w GLdouble
 * \return void
 */
STATICINLINE void uniform(i32 location, vec_4_f64 const& x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform4d)
    }
    glUniform4d(location, x[0], x[1], x[2], x[3]);
    detail::error_check("Uniform4d"sv);
}

template<class span_const_vec_4_f64>
requires(
    concepts::span<span_const_vec_4_f64> &&
    concepts::vector<typename span_const_vec_4_f64::value_type, f64, 4>)
/*!
 * \brief Part of GL_ARB_gpu_shader_fp64
 * \param location GLint
 * \param count GLsizei
 * \param value const GLdouble *
 * \return void
 */
STATICINLINE void uniform(i32 location, span_const_vec_4_f64 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform4dv)
    }
    glUniform4dv(
        location,
        value.size(),
        reinterpret_cast<const GLdouble*>(value.data()));
    detail::error_check("Uniform4dv"sv);
}

template<class span_const_mat_2x2_f64>
requires(
    concepts::span<span_const_mat_2x2_f64> &&
    concepts::matrix<typename span_const_mat_2x2_f64::value_type, f64, 2, 2>)
/*!
 * \brief Part of GL_ARB_gpu_shader_fp64
 * \param location GLint
 * \param count GLsizei
 * \param transpose GLboolean
 * \param value const GLdouble *
 * \return void
 */
STATICINLINE void uniform(
    i32 location, bool transpose, span_const_mat_2x2_f64 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(UniformMatrix2dv)
    }
    glUniformMatrix2dv(
        location,
        value.size(),
        transpose,
        reinterpret_cast<const GLdouble*>(value.data()));
    detail::error_check("UniformMatrix2dv"sv);
}

template<class span_const_mat_2x3_f64>
requires(
    concepts::span<span_const_mat_2x3_f64> &&
    concepts::matrix<typename span_const_mat_2x3_f64::value_type, f64, 2, 3>)
/*!
 * \brief Part of GL_ARB_gpu_shader_fp64
 * \param location GLint
 * \param count GLsizei
 * \param transpose GLboolean
 * \param value const GLdouble *
 * \return void
 */
STATICINLINE void uniform(
    i32 location, bool transpose, span_const_mat_2x3_f64 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(UniformMatrix2x3dv)
    }
    glUniformMatrix2x3dv(
        location,
        value.size(),
        transpose,
        reinterpret_cast<const GLdouble*>(value.data()));
    detail::error_check("UniformMatrix2x3dv"sv);
}

template<class span_const_mat_2x4_f64>
requires(
    concepts::span<span_const_mat_2x4_f64> &&
    concepts::matrix<typename span_const_mat_2x4_f64::value_type, f64, 2, 4>)
/*!
 * \brief Part of GL_ARB_gpu_shader_fp64
 * \param location GLint
 * \param count GLsizei
 * \param transpose GLboolean
 * \param value const GLdouble *
 * \return void
 */
STATICINLINE void uniform(
    i32 location, bool transpose, span_const_mat_2x4_f64 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(UniformMatrix2x4dv)
    }
    glUniformMatrix2x4dv(
        location,
        value.size(),
        transpose,
        reinterpret_cast<const GLdouble*>(value.data()));
    detail::error_check("UniformMatrix2x4dv"sv);
}

template<class span_const_mat_3x3_f64>
requires(
    concepts::span<span_const_mat_3x3_f64> &&
    concepts::matrix<typename span_const_mat_3x3_f64::value_type, f64, 3, 3>)
/*!
 * \brief Part of GL_ARB_gpu_shader_fp64
 * \param location GLint
 * \param count GLsizei
 * \param transpose GLboolean
 * \param value const GLdouble *
 * \return void
 */
STATICINLINE void uniform(
    i32 location, bool transpose, span_const_mat_3x3_f64 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(UniformMatrix3dv)
    }
    glUniformMatrix3dv(
        location,
        value.size(),
        transpose,
        reinterpret_cast<const GLdouble*>(value.data()));
    detail::error_check("UniformMatrix3dv"sv);
}

template<class span_const_mat_3x2_f64>
requires(
    concepts::span<span_const_mat_3x2_f64> &&
    concepts::matrix<typename span_const_mat_3x2_f64::value_type, f64, 3, 2>)
/*!
 * \brief Part of GL_ARB_gpu_shader_fp64
 * \param location GLint
 * \param count GLsizei
 * \param transpose GLboolean
 * \param value const GLdouble *
 * \return void
 */
STATICINLINE void uniform(
    i32 location, bool transpose, span_const_mat_3x2_f64 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(UniformMatrix3x2dv)
    }
    glUniformMatrix3x2dv(
        location,
        value.size(),
        transpose,
        reinterpret_cast<const GLdouble*>(value.data()));
    detail::error_check("UniformMatrix3x2dv"sv);
}

template<class span_const_mat_3x4_f64>
requires(
    concepts::span<span_const_mat_3x4_f64> &&
    concepts::matrix<typename span_const_mat_3x4_f64::value_type, f64, 3, 4>)
/*!
 * \brief Part of GL_ARB_gpu_shader_fp64
 * \param location GLint
 * \param count GLsizei
 * \param transpose GLboolean
 * \param value const GLdouble *
 * \return void
 */
STATICINLINE void uniform(
    i32 location, bool transpose, span_const_mat_3x4_f64 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(UniformMatrix3x4dv)
    }
    glUniformMatrix3x4dv(
        location,
        value.size(),
        transpose,
        reinterpret_cast<const GLdouble*>(value.data()));
    detail::error_check("UniformMatrix3x4dv"sv);
}

template<class span_const_mat_4x4_f64>
requires(
    concepts::span<span_const_mat_4x4_f64> &&
    concepts::matrix<typename span_const_mat_4x4_f64::value_type, f64, 4, 4>)
/*!
 * \brief Part of GL_ARB_gpu_shader_fp64
 * \param location GLint
 * \param count GLsizei
 * \param transpose GLboolean
 * \param value const GLdouble *
 * \return void
 */
STATICINLINE void uniform(
    i32 location, bool transpose, span_const_mat_4x4_f64 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(UniformMatrix4dv)
    }
    glUniformMatrix4dv(
        location,
        value.size(),
        transpose,
        reinterpret_cast<const GLdouble*>(value.data()));
    detail::error_check("UniformMatrix4dv"sv);
}

template<class span_const_mat_4x2_f64>
requires(
    concepts::span<span_const_mat_4x2_f64> &&
    concepts::matrix<typename span_const_mat_4x2_f64::value_type, f64, 4, 2>)
/*!
 * \brief Part of GL_ARB_gpu_shader_fp64
 * \param location GLint
 * \param count GLsizei
 * \param transpose GLboolean
 * \param value const GLdouble *
 * \return void
 */
STATICINLINE void uniform(
    i32 location, bool transpose, span_const_mat_4x2_f64 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(UniformMatrix4x2dv)
    }
    glUniformMatrix4x2dv(
        location,
        value.size(),
        transpose,
        reinterpret_cast<const GLdouble*>(value.data()));
    detail::error_check("UniformMatrix4x2dv"sv);
}

template<class span_const_mat_4x3_f64>
requires(
    concepts::span<span_const_mat_4x3_f64> &&
    concepts::matrix<typename span_const_mat_4x3_f64::value_type, f64, 4, 3>)
/*!
 * \brief Part of GL_ARB_gpu_shader_fp64
 * \param location GLint
 * \param count GLsizei
 * \param transpose GLboolean
 * \param value const GLdouble *
 * \return void
 */
STATICINLINE void uniform(
    i32 location, bool transpose, span_const_mat_4x3_f64 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(UniformMatrix4x3dv)
    }
    glUniformMatrix4x3dv(
        location,
        value.size(),
        transpose,
        reinterpret_cast<const GLdouble*>(value.data()));
    detail::error_check("UniformMatrix4x3dv"sv);
}

} // namespace gl::arb::gpu_shader_fp64
#endif // GL_ARB_gpu_shader_fp64
namespace gl::arb::gpu_shader_fp64 {
constexpr auto name = "GL_ARB_gpu_shader_fp64";
}
