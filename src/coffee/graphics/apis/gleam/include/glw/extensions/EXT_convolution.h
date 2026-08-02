#pragma once

#ifdef GL_EXT_convolution
#include "../enums/ConvolutionBorderModeEXT.h"
#include "../enums/ConvolutionParameter.h"
#include "../enums/ConvolutionTargetEXT.h"
#include "../enums/EnableCap.h"
#include "../enums/GetPName.h"
#include "../enums/PixelTransferParameter.h"
#include "../enums/SeparableTargetEXT.h"

namespace gl::ext::convolution {
using gl::group::convolution_border_mode_ext;
using gl::group::convolution_parameter;
using gl::group::convolution_target_ext;
using gl::group::enable_cap;
using gl::group::get_prop;
using gl::group::pixel_transfer_parameter;
using gl::group::separable_target_ext;

namespace values {
} // namespace values

template<class span_const_void>
requires(concepts::span<span_const_void>)
/*!
 * \brief Part of GL_EXT_convolution
 * \param target GLenum
 * \param internalformat GLenum
 * \param width GLsizei
 * \param format GLenum
 * \param type GLenum
 * \param image const void *
 * \return void
 */
STATICINLINE void convolution_filter_1d(
    group::convolution_target_ext target,
    group::internal_format        internalformat,
    i32                           width,
    group::pixel_format           format,
    group::pixel_type             type,
    span_const_void const&        image,
    error_check                   check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(ConvolutionFilter1DEXT)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glConvolutionFilter1DEXT,
        gsl::span<const char>(
            reinterpret_cast<const char*>(image.data()), image.size_bytes()),
        target,
        internalformat,
        width,
        format,
        type);
#endif
    glConvolutionFilter1DEXT(
        static_cast<GLenum>(target),
        static_cast<GLenum>(internalformat),
        width,
        static_cast<GLenum>(format),
        static_cast<GLenum>(type),
        image.size() ? reinterpret_cast<const void*>(image.data()) : nullptr);
    detail::error_check("ConvolutionFilter1DEXT"sv, check_errors);
}

template<class size_2_i32, class span_const_void>
requires(concepts::size_2d<size_2_i32, i32> && concepts::span<span_const_void>)
/*!
 * \brief Part of GL_EXT_convolution
 * \param target GLenum
 * \param internalformat GLenum
 * \param width GLsizei
 * \param height GLsizei
 * \param format GLenum
 * \param type GLenum
 * \param image const void *
 * \return void
 */
STATICINLINE void convolution_filter_2d(
    group::convolution_target_ext target,
    group::internal_format        internalformat,
    size_2_i32 const&             width,
    group::pixel_format           format,
    group::pixel_type             type,
    span_const_void const&        image,
    error_check                   check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(ConvolutionFilter2DEXT)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glConvolutionFilter2DEXT,
        gsl::span<const char>(
            reinterpret_cast<const char*>(image.data()), image.size_bytes()),
        target,
        internalformat,
        width,
        format,
        type);
#endif
    glConvolutionFilter2DEXT(
        static_cast<GLenum>(target),
        static_cast<GLenum>(internalformat),
        width[0],
        width[1],
        static_cast<GLenum>(format),
        static_cast<GLenum>(type),
        image.size() ? reinterpret_cast<const void*>(image.data()) : nullptr);
    detail::error_check("ConvolutionFilter2DEXT"sv, check_errors);
}

/*!
 * \brief Part of GL_EXT_convolution
 * \param target GLenum
 * \param pname GLenum
 * \param params GLfloat
 * \return void
 */
STATICINLINE void convolution_parameter(
    group::convolution_target_ext target,
    group::convolution_parameter  pname,
    f32                           params,
    error_check                   check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(ConvolutionParameterfEXT)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glConvolutionParameterfEXT, target, pname, params);
#endif
    glConvolutionParameterfEXT(
        static_cast<GLenum>(target), static_cast<GLenum>(pname), params);
    detail::error_check("ConvolutionParameterfEXT"sv, check_errors);
}

template<class span_const_f32>
requires(
    concepts::span<span_const_f32> &&
    std::is_same_v<
        std::decay_t<typename span_const_f32::value_type>,
        std::decay_t<f32>>)
/*!
 * \brief Part of GL_EXT_convolution
 * \param target GLenum
 * \param pname GLenum
 * \param params const GLfloat *
 * \return void
 */
STATICINLINE void convolution_parameter(
    group::convolution_target_ext target,
    group::convolution_parameter  pname,
    span_const_f32 const&         params,
    error_check                   check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(ConvolutionParameterfvEXT)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glConvolutionParameterfvEXT,
        gsl::span<const char>(
            reinterpret_cast<const char*>(params.data()), params.size_bytes()),
        target,
        pname);
#endif
    glConvolutionParameterfvEXT(
        static_cast<GLenum>(target),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<const GLfloat*>(params.data())
                      : nullptr);
    detail::error_check("ConvolutionParameterfvEXT"sv, check_errors);
}

/*!
 * \brief Part of GL_EXT_convolution
 * \param target GLenum
 * \param pname GLenum
 * \param params GLint
 * \return void
 */
STATICINLINE void convolution_parameter(
    group::convolution_target_ext target,
    group::convolution_parameter  pname,
    i32                           params,
    error_check                   check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(ConvolutionParameteriEXT)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glConvolutionParameteriEXT, target, pname, params);
#endif
    glConvolutionParameteriEXT(
        static_cast<GLenum>(target), static_cast<GLenum>(pname), params);
    detail::error_check("ConvolutionParameteriEXT"sv, check_errors);
}

template<class span_const_i32>
requires(
    concepts::span<span_const_i32> &&
    std::is_same_v<
        std::decay_t<typename span_const_i32::value_type>,
        std::decay_t<i32>>)
/*!
 * \brief Part of GL_EXT_convolution
 * \param target GLenum
 * \param pname GLenum
 * \param params const GLint *
 * \return void
 */
STATICINLINE void convolution_parameter(
    group::convolution_target_ext target,
    group::convolution_parameter  pname,
    span_const_i32 const&         params,
    error_check                   check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(ConvolutionParameterivEXT)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glConvolutionParameterivEXT,
        gsl::span<const char>(
            reinterpret_cast<const char*>(params.data()), params.size_bytes()),
        target,
        pname);
#endif
    glConvolutionParameterivEXT(
        static_cast<GLenum>(target),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<const GLint*>(params.data())
                      : nullptr);
    detail::error_check("ConvolutionParameterivEXT"sv, check_errors);
}

template<class vec_2_i32>
requires(concepts::vector<vec_2_i32, i32, 2>)
/*!
 * \brief Part of GL_EXT_convolution
 * \param target GLenum
 * \param internalformat GLenum
 * \param x GLint
 * \param y GLint
 * \param width GLsizei
 * \return void
 */
STATICINLINE void copy_convolution_filter_1d(
    group::convolution_target_ext target,
    group::internal_format        internalformat,
    vec_2_i32 const&              x,
    i32                           width,
    error_check                   check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(CopyConvolutionFilter1DEXT)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(
        glCopyConvolutionFilter1DEXT, target, internalformat, x, width);
#endif
    glCopyConvolutionFilter1DEXT(
        static_cast<GLenum>(target),
        static_cast<GLenum>(internalformat),
        x[0],
        x[1],
        width);
    detail::error_check("CopyConvolutionFilter1DEXT"sv, check_errors);
}

template<class size_2_i32, class vec_2_i32>
requires(
    concepts::vector<vec_2_i32, i32, 2> && concepts::size_2d<size_2_i32, i32>)
/*!
 * \brief Part of GL_EXT_convolution
 * \param target GLenum
 * \param internalformat GLenum
 * \param x GLint
 * \param y GLint
 * \param width GLsizei
 * \param height GLsizei
 * \return void
 */
STATICINLINE void copy_convolution_filter_2d(
    group::convolution_target_ext target,
    group::internal_format        internalformat,
    vec_2_i32 const&              x,
    size_2_i32 const&             width,
    error_check                   check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(CopyConvolutionFilter2DEXT)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(
        glCopyConvolutionFilter2DEXT, target, internalformat, x, width);
#endif
    glCopyConvolutionFilter2DEXT(
        static_cast<GLenum>(target),
        static_cast<GLenum>(internalformat),
        x[0],
        x[1],
        width[0],
        width[1]);
    detail::error_check("CopyConvolutionFilter2DEXT"sv, check_errors);
}

template<class span_void>
requires(concepts::span<span_void>)
/*!
 * \brief Part of GL_EXT_convolution
 * \param target GLenum
 * \param format GLenum
 * \param type GLenum
 * \param image void *
 * \return void
 */
STATICINLINE void get_convolution_filter(
    group::convolution_target_ext target,
    group::pixel_format           format,
    group::pixel_type             type,
    span_void                     image,
    error_check                   check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(GetConvolutionFilterEXT)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glGetConvolutionFilterEXT,
        gsl::span<char>(
            reinterpret_cast<char*>(image.data()), image.size_bytes()),
        target,
        format,
        type);
#endif
    glGetConvolutionFilterEXT(
        static_cast<GLenum>(target),
        static_cast<GLenum>(format),
        static_cast<GLenum>(type),
        image.size() ? reinterpret_cast<void*>(image.data()) : nullptr);
    detail::error_check("GetConvolutionFilterEXT"sv, check_errors);
}

template<class span_f32>
requires(
    concepts::span<span_f32> && std::is_same_v<
                                    std::decay_t<typename span_f32::value_type>,
                                    std::decay_t<f32>>)
/*!
 * \brief Part of GL_EXT_convolution
 * \param target GLenum
 * \param pname GLenum
 * \param params GLfloat *
 * \return void
 */
STATICINLINE void get_convolution_parameter(
    group::convolution_target_ext target,
    group::convolution_parameter  pname,
    span_f32                      params,
    error_check                   check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(GetConvolutionParameterfvEXT)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glGetConvolutionParameterfvEXT,
        gsl::span<char>(
            reinterpret_cast<char*>(params.data()), params.size_bytes()),
        target,
        pname);
#endif
    glGetConvolutionParameterfvEXT(
        static_cast<GLenum>(target),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLfloat*>(params.data()) : nullptr);
    detail::error_check("GetConvolutionParameterfvEXT"sv, check_errors);
}

template<class span_i32>
requires(
    concepts::span<span_i32> && std::is_same_v<
                                    std::decay_t<typename span_i32::value_type>,
                                    std::decay_t<i32>>)
/*!
 * \brief Part of GL_EXT_convolution
 * \param target GLenum
 * \param pname GLenum
 * \param params GLint *
 * \return void
 */
STATICINLINE void get_convolution_parameter(
    group::convolution_target_ext target,
    group::convolution_parameter  pname,
    span_i32                      params,
    error_check                   check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(GetConvolutionParameterivEXT)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glGetConvolutionParameterivEXT,
        gsl::span<char>(
            reinterpret_cast<char*>(params.data()), params.size_bytes()),
        target,
        pname);
#endif
    glGetConvolutionParameterivEXT(
        static_cast<GLenum>(target),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetConvolutionParameterivEXT"sv, check_errors);
}

template<class span_void>
requires(concepts::span<span_void>)
/*!
 * \brief Part of GL_EXT_convolution
 * \param target GLenum
 * \param format GLenum
 * \param type GLenum
 * \param row void *
 * \param column void *
 * \param span void *
 * \return void
 */
STATICINLINE void get_separable_filter(
    group::separable_target_ext target,
    group::pixel_format         format,
    group::pixel_type           type,
    span_void                   row,
    span_void                   column,
    span_void                   span,
    error_check                 check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(GetSeparableFilterEXT)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(
        glGetSeparableFilterEXT,
        target,
        format,
        type,
        gsl::span<char>(reinterpret_cast<char*>(row.data()), row.size_bytes()),
        gsl::span<char>(
            reinterpret_cast<char*>(column.data()), column.size_bytes()),
        gsl::span<char>(
            reinterpret_cast<char*>(span.data()), span.size_bytes()));
#endif
    glGetSeparableFilterEXT(
        static_cast<GLenum>(target),
        static_cast<GLenum>(format),
        static_cast<GLenum>(type),
        row.size() ? reinterpret_cast<void*>(row.data()) : nullptr,
        column.size() ? reinterpret_cast<void*>(column.data()) : nullptr,
        span.size() ? reinterpret_cast<void*>(span.data()) : nullptr);
    detail::error_check("GetSeparableFilterEXT"sv, check_errors);
}

template<class size_2_i32, class span_const_void>
requires(concepts::size_2d<size_2_i32, i32> && concepts::span<span_const_void>)
/*!
 * \brief Part of GL_EXT_convolution
 * \param target GLenum
 * \param internalformat GLenum
 * \param width GLsizei
 * \param height GLsizei
 * \param format GLenum
 * \param type GLenum
 * \param row const void *
 * \param column const void *
 * \return void
 */
STATICINLINE void separable_filter_2d(
    group::separable_target_ext target,
    group::internal_format      internalformat,
    size_2_i32 const&           width,
    group::pixel_format         format,
    group::pixel_type           type,
    span_const_void const&      row,
    span_const_void const&      column,
    error_check                 check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(SeparableFilter2DEXT)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(
        glSeparableFilter2DEXT,
        target,
        internalformat,
        width,
        format,
        type,
        gsl::span<const char>(
            reinterpret_cast<const char*>(row.data()), row.size_bytes()),
        gsl::span<const char>(
            reinterpret_cast<const char*>(column.data()), column.size_bytes()));
#endif
    glSeparableFilter2DEXT(
        static_cast<GLenum>(target),
        static_cast<GLenum>(internalformat),
        width[0],
        width[1],
        static_cast<GLenum>(format),
        static_cast<GLenum>(type),
        row.size() ? reinterpret_cast<const void*>(row.data()) : nullptr,
        column.size() ? reinterpret_cast<const void*>(column.data()) : nullptr);
    detail::error_check("SeparableFilter2DEXT"sv, check_errors);
}

} // namespace gl::ext::convolution
#endif // GL_EXT_convolution
namespace gl::ext::convolution {
constexpr auto name = "GL_EXT_convolution";
}
