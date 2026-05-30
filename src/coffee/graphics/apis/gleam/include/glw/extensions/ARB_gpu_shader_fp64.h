#pragma once

#ifdef GL_ARB_gpu_shader_fp64
#include "../enums/AttributeType.h"
#include "../enums/BinormalPointerTypeEXT.h"
#include "../enums/ColorPointerType.h"
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
using gl::group::color_pointer_type;
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
STATICINLINE void get_uniformdv(
    u32         program,
    i32         location,
    span_f64    params,
    error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetUniformdv)
#if (defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE_DATA(
        glGetUniformdv,
        gsl::span<char>(
            reinterpret_cast<char*>(params.data()), params.size_bytes()),
        program,
        location);
#endif
    glGetUniformdv(
        program,
        location,
        params.size() ? reinterpret_cast<GLdouble*>(params.data()) : nullptr);
    detail::error_check("GetUniformdv"sv, check_errors);
}

/*!
 * \brief Part of GL_ARB_gpu_shader_fp64
 * \param location GLint
 * \param x GLdouble
 * \return void
 */
STATICINLINE void uniform(
    i32 location, f64 x, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform1d)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glUniform1d, location, x);
#endif
    glUniform1d(location, x);
    detail::error_check("Uniform1d"sv, check_errors);
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
STATICINLINE void uniform(
    i32                   location,
    span_const_f64 const& value,
    error_check           check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform1dv)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE_DATA(
        glUniform1dv,
        gsl::span<const char>(
            reinterpret_cast<const char*>(value.data()), value.size_bytes()),
        location);
#endif
    glUniform1dv(
        location,
        value.size(),
        reinterpret_cast<const GLdouble*>(value.data()));
    detail::error_check("Uniform1dv"sv, check_errors);
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
STATICINLINE void uniform(
    i32              location,
    vec_2_f64 const& x,
    error_check      check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform2d)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glUniform2d, location, x);
#endif
    glUniform2d(location, x[0], x[1]);
    detail::error_check("Uniform2d"sv, check_errors);
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
STATICINLINE void uniform(
    i32                         location,
    span_const_vec_2_f64 const& value,
    error_check                 check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform2dv)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE_DATA(
        glUniform2dv,
        gsl::span<const char>(
            reinterpret_cast<const char*>(value.data()), value.size_bytes()),
        location);
#endif
    glUniform2dv(
        location,
        value.size(),
        reinterpret_cast<const GLdouble*>(value.data()));
    detail::error_check("Uniform2dv"sv, check_errors);
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
STATICINLINE void uniform(
    i32              location,
    vec_3_f64 const& x,
    error_check      check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform3d)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glUniform3d, location, x);
#endif
    glUniform3d(location, x[0], x[1], x[2]);
    detail::error_check("Uniform3d"sv, check_errors);
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
STATICINLINE void uniform(
    i32                         location,
    span_const_vec_3_f64 const& value,
    error_check                 check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform3dv)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE_DATA(
        glUniform3dv,
        gsl::span<const char>(
            reinterpret_cast<const char*>(value.data()), value.size_bytes()),
        location);
#endif
    glUniform3dv(
        location,
        value.size(),
        reinterpret_cast<const GLdouble*>(value.data()));
    detail::error_check("Uniform3dv"sv, check_errors);
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
STATICINLINE void uniform(
    i32              location,
    vec_4_f64 const& x,
    error_check      check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform4d)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glUniform4d, location, x);
#endif
    glUniform4d(location, x[0], x[1], x[2], x[3]);
    detail::error_check("Uniform4d"sv, check_errors);
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
STATICINLINE void uniform(
    i32                         location,
    span_const_vec_4_f64 const& value,
    error_check                 check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform4dv)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE_DATA(
        glUniform4dv,
        gsl::span<const char>(
            reinterpret_cast<const char*>(value.data()), value.size_bytes()),
        location);
#endif
    glUniform4dv(
        location,
        value.size(),
        reinterpret_cast<const GLdouble*>(value.data()));
    detail::error_check("Uniform4dv"sv, check_errors);
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
    i32                           location,
    bool                          transpose,
    span_const_mat_2x2_f64 const& value,
    error_check                   check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(UniformMatrix2dv)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE_DATA(
        glUniformMatrix2dv,
        gsl::span<const char>(
            reinterpret_cast<const char*>(value.data()), value.size_bytes()),
        location,
        transpose);
#endif
    glUniformMatrix2dv(
        location,
        value.size(),
        transpose,
        reinterpret_cast<const GLdouble*>(value.data()));
    detail::error_check("UniformMatrix2dv"sv, check_errors);
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
    i32                           location,
    bool                          transpose,
    span_const_mat_2x3_f64 const& value,
    error_check                   check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(UniformMatrix2x3dv)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE_DATA(
        glUniformMatrix2x3dv,
        gsl::span<const char>(
            reinterpret_cast<const char*>(value.data()), value.size_bytes()),
        location,
        transpose);
#endif
    glUniformMatrix2x3dv(
        location,
        value.size(),
        transpose,
        reinterpret_cast<const GLdouble*>(value.data()));
    detail::error_check("UniformMatrix2x3dv"sv, check_errors);
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
    i32                           location,
    bool                          transpose,
    span_const_mat_2x4_f64 const& value,
    error_check                   check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(UniformMatrix2x4dv)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE_DATA(
        glUniformMatrix2x4dv,
        gsl::span<const char>(
            reinterpret_cast<const char*>(value.data()), value.size_bytes()),
        location,
        transpose);
#endif
    glUniformMatrix2x4dv(
        location,
        value.size(),
        transpose,
        reinterpret_cast<const GLdouble*>(value.data()));
    detail::error_check("UniformMatrix2x4dv"sv, check_errors);
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
    i32                           location,
    bool                          transpose,
    span_const_mat_3x3_f64 const& value,
    error_check                   check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(UniformMatrix3dv)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE_DATA(
        glUniformMatrix3dv,
        gsl::span<const char>(
            reinterpret_cast<const char*>(value.data()), value.size_bytes()),
        location,
        transpose);
#endif
    glUniformMatrix3dv(
        location,
        value.size(),
        transpose,
        reinterpret_cast<const GLdouble*>(value.data()));
    detail::error_check("UniformMatrix3dv"sv, check_errors);
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
    i32                           location,
    bool                          transpose,
    span_const_mat_3x2_f64 const& value,
    error_check                   check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(UniformMatrix3x2dv)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE_DATA(
        glUniformMatrix3x2dv,
        gsl::span<const char>(
            reinterpret_cast<const char*>(value.data()), value.size_bytes()),
        location,
        transpose);
#endif
    glUniformMatrix3x2dv(
        location,
        value.size(),
        transpose,
        reinterpret_cast<const GLdouble*>(value.data()));
    detail::error_check("UniformMatrix3x2dv"sv, check_errors);
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
    i32                           location,
    bool                          transpose,
    span_const_mat_3x4_f64 const& value,
    error_check                   check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(UniformMatrix3x4dv)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE_DATA(
        glUniformMatrix3x4dv,
        gsl::span<const char>(
            reinterpret_cast<const char*>(value.data()), value.size_bytes()),
        location,
        transpose);
#endif
    glUniformMatrix3x4dv(
        location,
        value.size(),
        transpose,
        reinterpret_cast<const GLdouble*>(value.data()));
    detail::error_check("UniformMatrix3x4dv"sv, check_errors);
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
    i32                           location,
    bool                          transpose,
    span_const_mat_4x4_f64 const& value,
    error_check                   check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(UniformMatrix4dv)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE_DATA(
        glUniformMatrix4dv,
        gsl::span<const char>(
            reinterpret_cast<const char*>(value.data()), value.size_bytes()),
        location,
        transpose);
#endif
    glUniformMatrix4dv(
        location,
        value.size(),
        transpose,
        reinterpret_cast<const GLdouble*>(value.data()));
    detail::error_check("UniformMatrix4dv"sv, check_errors);
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
    i32                           location,
    bool                          transpose,
    span_const_mat_4x2_f64 const& value,
    error_check                   check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(UniformMatrix4x2dv)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE_DATA(
        glUniformMatrix4x2dv,
        gsl::span<const char>(
            reinterpret_cast<const char*>(value.data()), value.size_bytes()),
        location,
        transpose);
#endif
    glUniformMatrix4x2dv(
        location,
        value.size(),
        transpose,
        reinterpret_cast<const GLdouble*>(value.data()));
    detail::error_check("UniformMatrix4x2dv"sv, check_errors);
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
    i32                           location,
    bool                          transpose,
    span_const_mat_4x3_f64 const& value,
    error_check                   check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(UniformMatrix4x3dv)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE_DATA(
        glUniformMatrix4x3dv,
        gsl::span<const char>(
            reinterpret_cast<const char*>(value.data()), value.size_bytes()),
        location,
        transpose);
#endif
    glUniformMatrix4x3dv(
        location,
        value.size(),
        transpose,
        reinterpret_cast<const GLdouble*>(value.data()));
    detail::error_check("UniformMatrix4x3dv"sv, check_errors);
}

} // namespace gl::arb::gpu_shader_fp64
#endif // GL_ARB_gpu_shader_fp64
namespace gl::arb::gpu_shader_fp64 {
constexpr auto name = "GL_ARB_gpu_shader_fp64";
}
