#pragma once

#ifdef GL_NV_evaluators
#include "../enums/EvalTargetNV.h"
#include "../enums/MapAttribParameterNV.h"
#include "../enums/MapParameterNV.h"

namespace gl::nv::evaluators {
using gl::group::eval_target_nv;
using gl::group::map_attrib_parameter_nv;
using gl::group::map_parameter_nv;

namespace values {
constexpr u32 eval_fractional_tessellation = 0x86C5;
constexpr u32 eval_vertex_attrib0          = 0x86C6;
constexpr u32 eval_vertex_attrib1          = 0x86C7;
constexpr u32 eval_vertex_attrib2          = 0x86C8;
constexpr u32 eval_vertex_attrib3          = 0x86C9;
constexpr u32 eval_vertex_attrib4          = 0x86CA;
constexpr u32 eval_vertex_attrib5          = 0x86CB;
constexpr u32 eval_vertex_attrib6          = 0x86CC;
constexpr u32 eval_vertex_attrib7          = 0x86CD;
constexpr u32 eval_vertex_attrib8          = 0x86CE;
constexpr u32 eval_vertex_attrib9          = 0x86CF;
constexpr u32 eval_vertex_attrib10         = 0x86D0;
constexpr u32 eval_vertex_attrib11         = 0x86D1;
constexpr u32 eval_vertex_attrib12         = 0x86D2;
constexpr u32 eval_vertex_attrib13         = 0x86D3;
constexpr u32 eval_vertex_attrib14         = 0x86D4;
constexpr u32 eval_vertex_attrib15         = 0x86D5;
constexpr u32 max_map_tessellation         = 0x86D6;
constexpr u32 max_rational_eval_order      = 0x86D7;
} // namespace values

/*!
 * \brief Part of GL_NV_evaluators
 * \param target GLenum
 * \param mode GLenum
 * \return void
 */
STATICINLINE void eval_maps(
    group::eval_target_nv    target,
    group::eval_maps_mode_nv mode,
    error_check              check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(EvalMapsNV)
    }
    glEvalMapsNV(static_cast<GLenum>(target), static_cast<GLenum>(mode));
    detail::error_check("EvalMapsNV"sv, check_errors);
}

template<class span_f32>
requires(
    concepts::span<span_f32> && std::is_same_v<
                                    std::decay_t<typename span_f32::value_type>,
                                    std::decay_t<f32>>)
/*!
 * \brief Part of GL_NV_evaluators
 * \param target GLenum
 * \param index GLuint
 * \param pname GLenum
 * \param params GLfloat *
 * \return void
 */
STATICINLINE void get_map_attrib_parameter(
    group::eval_target_nv          target,
    u32                            index,
    group::map_attrib_parameter_nv pname,
    span_f32                       params,
    error_check                    check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetMapAttribParameterfvNV)
    }
    glGetMapAttribParameterfvNV(
        static_cast<GLenum>(target),
        index,
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLfloat*>(params.data()) : nullptr);
    detail::error_check("GetMapAttribParameterfvNV"sv, check_errors);
}

template<class span_i32>
requires(
    concepts::span<span_i32> && std::is_same_v<
                                    std::decay_t<typename span_i32::value_type>,
                                    std::decay_t<i32>>)
/*!
 * \brief Part of GL_NV_evaluators
 * \param target GLenum
 * \param index GLuint
 * \param pname GLenum
 * \param params GLint *
 * \return void
 */
STATICINLINE void get_map_attrib_parameter(
    group::eval_target_nv          target,
    u32                            index,
    group::map_attrib_parameter_nv pname,
    span_i32                       params,
    error_check                    check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetMapAttribParameterivNV)
    }
    glGetMapAttribParameterivNV(
        static_cast<GLenum>(target),
        index,
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetMapAttribParameterivNV"sv, check_errors);
}

template<class span_void>
requires(concepts::span<span_void>)
/*!
 * \brief Part of GL_NV_evaluators
 * \param target GLenum
 * \param index GLuint
 * \param type GLenum
 * \param ustride GLsizei
 * \param vstride GLsizei
 * \param packed GLboolean
 * \param points void *
 * \return void
 */
STATICINLINE void get_map_control_points(
    group::eval_target_nv target,
    u32                   index,
    group::map_type_nv    type,
    i32                   ustride,
    i32                   vstride,
    bool                  packed,
    span_void             points,
    error_check           check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetMapControlPointsNV)
    }
    glGetMapControlPointsNV(
        static_cast<GLenum>(target),
        index,
        static_cast<GLenum>(type),
        ustride,
        vstride,
        packed,
        points.size() ? reinterpret_cast<void*>(points.data()) : nullptr);
    detail::error_check("GetMapControlPointsNV"sv, check_errors);
}

template<class span_f32>
requires(
    concepts::span<span_f32> && std::is_same_v<
                                    std::decay_t<typename span_f32::value_type>,
                                    std::decay_t<f32>>)
/*!
 * \brief Part of GL_NV_evaluators
 * \param target GLenum
 * \param pname GLenum
 * \param params GLfloat *
 * \return void
 */
STATICINLINE void get_map_parameter(
    group::eval_target_nv   target,
    group::map_parameter_nv pname,
    span_f32                params,
    error_check             check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetMapParameterfvNV)
    }
    glGetMapParameterfvNV(
        static_cast<GLenum>(target),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLfloat*>(params.data()) : nullptr);
    detail::error_check("GetMapParameterfvNV"sv, check_errors);
}

template<class span_i32>
requires(
    concepts::span<span_i32> && std::is_same_v<
                                    std::decay_t<typename span_i32::value_type>,
                                    std::decay_t<i32>>)
/*!
 * \brief Part of GL_NV_evaluators
 * \param target GLenum
 * \param pname GLenum
 * \param params GLint *
 * \return void
 */
STATICINLINE void get_map_parameter(
    group::eval_target_nv   target,
    group::map_parameter_nv pname,
    span_i32                params,
    error_check             check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetMapParameterivNV)
    }
    glGetMapParameterivNV(
        static_cast<GLenum>(target),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetMapParameterivNV"sv, check_errors);
}

template<class span_const_void>
requires(concepts::span<span_const_void>)
/*!
 * \brief Part of GL_NV_evaluators
 * \param target GLenum
 * \param index GLuint
 * \param type GLenum
 * \param ustride GLsizei
 * \param vstride GLsizei
 * \param uorder GLint
 * \param vorder GLint
 * \param packed GLboolean
 * \param points const void *
 * \return void
 */
STATICINLINE void map_control_points(
    group::eval_target_nv  target,
    u32                    index,
    group::map_type_nv     type,
    i32                    ustride,
    i32                    vstride,
    i32                    uorder,
    i32                    vorder,
    bool                   packed,
    span_const_void const& points,
    error_check            check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MapControlPointsNV)
    }
    glMapControlPointsNV(
        static_cast<GLenum>(target),
        index,
        static_cast<GLenum>(type),
        ustride,
        vstride,
        uorder,
        vorder,
        packed,
        points.size() ? reinterpret_cast<const void*>(points.data()) : nullptr);
    detail::error_check("MapControlPointsNV"sv, check_errors);
}

template<class span_const_f32>
requires(
    concepts::span<span_const_f32> &&
    std::is_same_v<
        std::decay_t<typename span_const_f32::value_type>,
        std::decay_t<f32>>)
/*!
 * \brief Part of GL_NV_evaluators
 * \param target GLenum
 * \param pname GLenum
 * \param params const GLfloat *
 * \return void
 */
STATICINLINE void map_parameter(
    group::eval_target_nv   target,
    group::map_parameter_nv pname,
    span_const_f32 const&   params,
    error_check             check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MapParameterfvNV)
    }
    glMapParameterfvNV(
        static_cast<GLenum>(target),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<const GLfloat*>(params.data())
                      : nullptr);
    detail::error_check("MapParameterfvNV"sv, check_errors);
}

template<class span_const_i32>
requires(
    concepts::span<span_const_i32> &&
    std::is_same_v<
        std::decay_t<typename span_const_i32::value_type>,
        std::decay_t<i32>>)
/*!
 * \brief Part of GL_NV_evaluators
 * \param target GLenum
 * \param pname GLenum
 * \param params const GLint *
 * \return void
 */
STATICINLINE void map_parameter(
    group::eval_target_nv   target,
    group::map_parameter_nv pname,
    span_const_i32 const&   params,
    error_check             check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MapParameterivNV)
    }
    glMapParameterivNV(
        static_cast<GLenum>(target),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<const GLint*>(params.data())
                      : nullptr);
    detail::error_check("MapParameterivNV"sv, check_errors);
}

} // namespace gl::nv::evaluators
#endif // GL_NV_evaluators
namespace gl::nv::evaluators {
constexpr auto name = "GL_NV_evaluators";
}
