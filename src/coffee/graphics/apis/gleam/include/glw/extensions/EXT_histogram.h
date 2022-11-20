#pragma once

#ifdef GL_EXT_histogram
#include "../enums/EnableCap.h"
#include "../enums/ErrorCode.h"
#include "../enums/GetHistogramParameterPNameEXT.h"
#include "../enums/GetMinmaxParameterPNameEXT.h"
#include "../enums/GetPName.h"
#include "../enums/HistogramTargetEXT.h"
#include "../enums/MinmaxTargetEXT.h"
namespace gl::ext::histogram {
using gl::group::enable_cap;
using gl::group::error_code;
using gl::group::get_histogram_parameter_prop_ext;
using gl::group::get_minmax_parameter_prop_ext;
using gl::group::get_prop;
using gl::group::histogram_target_ext;
using gl::group::minmax_target_ext;
namespace values {
} // namespace values
template<class span_void>
requires(semantic::concepts::Span<span_void>)
    /*!
     * \brief Part of GL_EXT_histogram
     * \param target GLenum
     * \param reset GLboolean
     * \param format GLenum
     * \param type GLenum
     * \param values void *
     * \return void
     */
    STATICINLINE void get_histogram(
        group::histogram_target_ext target,
        bool                        reset,
        group::pixel_format         format,
        group::pixel_type           type,
        span_void                   values)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetHistogramEXT)
    }
    glGetHistogramEXT(
        static_cast<GLenum>(target),
        reset,
        static_cast<GLenum>(format),
        static_cast<GLenum>(type),
        values.size() ? reinterpret_cast<void*>(values.data()) : nullptr);
    detail::error_check("GetHistogramEXT"sv);
}

template<class span_f32>
requires(semantic::concepts::Span<span_f32>&& std::is_same_v<
         std::decay_t<typename span_f32::value_type>,
         std::decay_t<f32>>)
    /*!
     * \brief Part of GL_EXT_histogram
     * \param target GLenum
     * \param pname GLenum
     * \param params GLfloat *
     * \return void
     */
    STATICINLINE void get_histogram_parameter(
        group::histogram_target_ext             target,
        group::get_histogram_parameter_prop_ext pname,
        span_f32                                params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetHistogramParameterfvEXT)
    }
    glGetHistogramParameterfvEXT(
        static_cast<GLenum>(target),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLfloat*>(params.data()) : nullptr);
    detail::error_check("GetHistogramParameterfvEXT"sv);
}

template<class span_i32>
requires(semantic::concepts::Span<span_i32>&& std::is_same_v<
         std::decay_t<typename span_i32::value_type>,
         std::decay_t<i32>>)
    /*!
     * \brief Part of GL_EXT_histogram
     * \param target GLenum
     * \param pname GLenum
     * \param params GLint *
     * \return void
     */
    STATICINLINE void get_histogram_parameter(
        group::histogram_target_ext             target,
        group::get_histogram_parameter_prop_ext pname,
        span_i32                                params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetHistogramParameterivEXT)
    }
    glGetHistogramParameterivEXT(
        static_cast<GLenum>(target),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetHistogramParameterivEXT"sv);
}

template<class span_void>
requires(semantic::concepts::Span<span_void>)
    /*!
     * \brief Part of GL_EXT_histogram
     * \param target GLenum
     * \param reset GLboolean
     * \param format GLenum
     * \param type GLenum
     * \param values void *
     * \return void
     */
    STATICINLINE void get_minmax(
        group::minmax_target_ext target,
        bool                     reset,
        group::pixel_format      format,
        group::pixel_type        type,
        span_void                values)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetMinmaxEXT)
    }
    glGetMinmaxEXT(
        static_cast<GLenum>(target),
        reset,
        static_cast<GLenum>(format),
        static_cast<GLenum>(type),
        values.size() ? reinterpret_cast<void*>(values.data()) : nullptr);
    detail::error_check("GetMinmaxEXT"sv);
}

template<class span_f32>
requires(semantic::concepts::Span<span_f32>&& std::is_same_v<
         std::decay_t<typename span_f32::value_type>,
         std::decay_t<f32>>)
    /*!
     * \brief Part of GL_EXT_histogram
     * \param target GLenum
     * \param pname GLenum
     * \param params GLfloat *
     * \return void
     */
    STATICINLINE void get_minmax_parameter(
        group::minmax_target_ext             target,
        group::get_minmax_parameter_prop_ext pname,
        span_f32                             params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetMinmaxParameterfvEXT)
    }
    glGetMinmaxParameterfvEXT(
        static_cast<GLenum>(target),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLfloat*>(params.data()) : nullptr);
    detail::error_check("GetMinmaxParameterfvEXT"sv);
}

template<class span_i32>
requires(semantic::concepts::Span<span_i32>&& std::is_same_v<
         std::decay_t<typename span_i32::value_type>,
         std::decay_t<i32>>)
    /*!
     * \brief Part of GL_EXT_histogram
     * \param target GLenum
     * \param pname GLenum
     * \param params GLint *
     * \return void
     */
    STATICINLINE void get_minmax_parameter(
        group::minmax_target_ext             target,
        group::get_minmax_parameter_prop_ext pname,
        span_i32                             params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetMinmaxParameterivEXT)
    }
    glGetMinmaxParameterivEXT(
        static_cast<GLenum>(target),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetMinmaxParameterivEXT"sv);
}

/*!
 * \brief Part of GL_EXT_histogram
 * \param target GLenum
 * \param width GLsizei
 * \param internalformat GLenum
 * \param sink GLboolean
 * \return void
 */
STATICINLINE void histogram(
    group::histogram_target_ext target,
    i32                         width,
    group::internal_format      internalformat,
    bool                        sink)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(HistogramEXT)
    }
    glHistogramEXT(
        static_cast<GLenum>(target),
        width,
        static_cast<GLenum>(internalformat),
        sink);
    detail::error_check("HistogramEXT"sv);
}

/*!
 * \brief Part of GL_EXT_histogram
 * \param target GLenum
 * \param internalformat GLenum
 * \param sink GLboolean
 * \return void
 */
STATICINLINE void minmax(
    group::minmax_target_ext target,
    group::internal_format   internalformat,
    bool                     sink)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MinmaxEXT)
    }
    glMinmaxEXT(
        static_cast<GLenum>(target), static_cast<GLenum>(internalformat), sink);
    detail::error_check("MinmaxEXT"sv);
}

/*!
 * \brief Part of GL_EXT_histogram
 * \param target GLenum
 * \return void
 */
STATICINLINE void reset_histogram(group::histogram_target_ext target)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ResetHistogramEXT)
    }
    glResetHistogramEXT(static_cast<GLenum>(target));
    detail::error_check("ResetHistogramEXT"sv);
}

/*!
 * \brief Part of GL_EXT_histogram
 * \param target GLenum
 * \return void
 */
STATICINLINE void reset_minmax(group::minmax_target_ext target)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ResetMinmaxEXT)
    }
    glResetMinmaxEXT(static_cast<GLenum>(target));
    detail::error_check("ResetMinmaxEXT"sv);
}

} // namespace gl::ext::histogram
#endif // GL_EXT_histogram
namespace gl::ext::histogram {
constexpr auto name = "GL_EXT_histogram";
}
