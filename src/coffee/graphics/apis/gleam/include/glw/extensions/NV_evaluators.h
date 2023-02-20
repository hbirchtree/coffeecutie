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
constexpr libc_types::u32 eval_fractional_tessellation = 0x86C5;
constexpr libc_types::u32 eval_vertex_attrib0          = 0x86C6;
constexpr libc_types::u32 eval_vertex_attrib1          = 0x86C7;
constexpr libc_types::u32 eval_vertex_attrib2          = 0x86C8;
constexpr libc_types::u32 eval_vertex_attrib3          = 0x86C9;
constexpr libc_types::u32 eval_vertex_attrib4          = 0x86CA;
constexpr libc_types::u32 eval_vertex_attrib5          = 0x86CB;
constexpr libc_types::u32 eval_vertex_attrib6          = 0x86CC;
constexpr libc_types::u32 eval_vertex_attrib7          = 0x86CD;
constexpr libc_types::u32 eval_vertex_attrib8          = 0x86CE;
constexpr libc_types::u32 eval_vertex_attrib9          = 0x86CF;
constexpr libc_types::u32 eval_vertex_attrib10         = 0x86D0;
constexpr libc_types::u32 eval_vertex_attrib11         = 0x86D1;
constexpr libc_types::u32 eval_vertex_attrib12         = 0x86D2;
constexpr libc_types::u32 eval_vertex_attrib13         = 0x86D3;
constexpr libc_types::u32 eval_vertex_attrib14         = 0x86D4;
constexpr libc_types::u32 eval_vertex_attrib15         = 0x86D5;
constexpr libc_types::u32 max_map_tessellation         = 0x86D6;
constexpr libc_types::u32 max_rational_eval_order      = 0x86D7;
} // namespace values
/*!
 * \brief Part of GL_NV_evaluators
 * \param target GLenum
 * \param mode GLenum
 * \return void
 */
STATICINLINE void eval_maps(
    group::eval_target_nv target, group::eval_maps_mode_nv mode)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(EvalMapsNV)
    }
    glEvalMapsNV(static_cast<GLenum>(target), static_cast<GLenum>(mode));
    detail::error_check("EvalMapsNV"sv);
}

template<class span_f32>
requires(semantic::concepts::Span<span_f32>&& std::is_same_v<
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
        span_f32                       params)
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
    detail::error_check("GetMapAttribParameterfvNV"sv);
}

template<class span_i32>
requires(semantic::concepts::Span<span_i32>&& std::is_same_v<
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
        span_i32                       params)
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
    detail::error_check("GetMapAttribParameterivNV"sv);
}

template<class span_void>
requires(semantic::concepts::Span<span_void>)
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
        span_void             points)
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
    detail::error_check("GetMapControlPointsNV"sv);
}

template<class span_f32>
requires(semantic::concepts::Span<span_f32>&& std::is_same_v<
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
        span_f32                params)
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
    detail::error_check("GetMapParameterfvNV"sv);
}

template<class span_i32>
requires(semantic::concepts::Span<span_i32>&& std::is_same_v<
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
        span_i32                params)
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
    detail::error_check("GetMapParameterivNV"sv);
}

template<class span_const_void>
requires(semantic::concepts::Span<span_const_void>)
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
        span_const_void const& points)
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
    detail::error_check("MapControlPointsNV"sv);
}

template<class span_const_f32>
requires(semantic::concepts::Span<span_const_f32>&& std::is_same_v<
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
        span_const_f32 const&   params)
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
    detail::error_check("MapParameterfvNV"sv);
}

template<class span_const_i32>
requires(semantic::concepts::Span<span_const_i32>&& std::is_same_v<
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
        span_const_i32 const&   params)
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
    detail::error_check("MapParameterivNV"sv);
}

} // namespace gl::nv::evaluators
#endif // GL_NV_evaluators
namespace gl::nv::evaluators {
constexpr auto name = "GL_NV_evaluators";
}
