#pragma once

#ifdef GL_EXT_convolution
#include "../enums/ConvolutionBorderModeEXT.h"
#include "../enums/ConvolutionParameterEXT.h"
#include "../enums/ConvolutionTargetEXT.h"
#include "../enums/EnableCap.h"
#include "../enums/GetConvolutionParameter.h"
#include "../enums/GetPName.h"
#include "../enums/PixelTransferParameter.h"
#include "../enums/SeparableTargetEXT.h"
namespace gl::ext::convolution {
using gl::group::convolution_border_mode_ext;
using gl::group::convolution_parameter_ext;
using gl::group::convolution_target_ext;
using gl::group::enable_cap;
using gl::group::get_convolution_parameter;
using gl::group::get_prop;
using gl::group::pixel_transfer_parameter;
using gl::group::separable_target_ext;
namespace values {
} // namespace values
template<class span_const_void>
requires(semantic::concepts::Span<span_const_void>) STATICINLINE
    void convolution_filter_1d(
        group::convolution_target_ext target,
        group::internal_format        internalformat,
        i32                           width,
        group::pixel_format           format,
        group::pixel_type             type,
        span_const_void const&        image)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ConvolutionFilter1DEXT)
    }
    glConvolutionFilter1DEXT(
        static_cast<GLenum>(target),
        static_cast<GLenum>(internalformat),
        width,
        static_cast<GLenum>(format),
        static_cast<GLenum>(type),
        image.size() ? reinterpret_cast<const void*>(image.data()) : nullptr);
    detail::error_check("ConvolutionFilter1DEXT"sv);
}

template<class size_2_i32, class span_const_void>
requires(semantic::concepts::Size2D<size_2_i32, i32>&&
             semantic::concepts::Span<span_const_void>) STATICINLINE
    void convolution_filter_2d(
        group::convolution_target_ext target,
        group::internal_format        internalformat,
        size_2_i32 const&             width,
        group::pixel_format           format,
        group::pixel_type             type,
        span_const_void const&        image)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ConvolutionFilter2DEXT)
    }
    glConvolutionFilter2DEXT(
        static_cast<GLenum>(target),
        static_cast<GLenum>(internalformat),
        width[0],
        width[1],
        static_cast<GLenum>(format),
        static_cast<GLenum>(type),
        image.size() ? reinterpret_cast<const void*>(image.data()) : nullptr);
    detail::error_check("ConvolutionFilter2DEXT"sv);
}

STATICINLINE void convolution_parameter(
    group::convolution_target_ext    target,
    group::convolution_parameter_ext pname,
    f32                              params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ConvolutionParameterfEXT)
    }
    glConvolutionParameterfEXT(
        static_cast<GLenum>(target), static_cast<GLenum>(pname), params);
    detail::error_check("ConvolutionParameterfEXT"sv);
}

template<class span_const_f32>
requires(semantic::concepts::Span<span_const_f32>&& std::is_same_v<
         std::decay_t<typename span_const_f32::value_type>,
         std::decay_t<f32>>) STATICINLINE
    void convolution_parameter(
        group::convolution_target_ext    target,
        group::convolution_parameter_ext pname,
        span_const_f32 const&            params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ConvolutionParameterfvEXT)
    }
    glConvolutionParameterfvEXT(
        static_cast<GLenum>(target),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<const GLfloat*>(params.data())
                      : nullptr);
    detail::error_check("ConvolutionParameterfvEXT"sv);
}

STATICINLINE void convolution_parameter(
    group::convolution_target_ext    target,
    group::convolution_parameter_ext pname,
    i32                              params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ConvolutionParameteriEXT)
    }
    glConvolutionParameteriEXT(
        static_cast<GLenum>(target), static_cast<GLenum>(pname), params);
    detail::error_check("ConvolutionParameteriEXT"sv);
}

template<class span_const_i32>
requires(semantic::concepts::Span<span_const_i32>&& std::is_same_v<
         std::decay_t<typename span_const_i32::value_type>,
         std::decay_t<i32>>) STATICINLINE
    void convolution_parameter(
        group::convolution_target_ext    target,
        group::convolution_parameter_ext pname,
        span_const_i32 const&            params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ConvolutionParameterivEXT)
    }
    glConvolutionParameterivEXT(
        static_cast<GLenum>(target),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<const GLint*>(params.data())
                      : nullptr);
    detail::error_check("ConvolutionParameterivEXT"sv);
}

template<class vec_2_i32>
requires(semantic::concepts::Vector<vec_2_i32, i32, 2>) STATICINLINE
    void copy_convolution_filter_1d(
        group::convolution_target_ext target,
        group::internal_format        internalformat,
        vec_2_i32 const&              x,
        i32                           width)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CopyConvolutionFilter1DEXT)
    }
    glCopyConvolutionFilter1DEXT(
        static_cast<GLenum>(target),
        static_cast<GLenum>(internalformat),
        x.x(),
        x.y(),
        width);
    detail::error_check("CopyConvolutionFilter1DEXT"sv);
}

template<class size_2_i32, class vec_2_i32>
requires(semantic::concepts::Vector<vec_2_i32, i32, 2>&&
             semantic::concepts::Size2D<size_2_i32, i32>) STATICINLINE
    void copy_convolution_filter_2d(
        group::convolution_target_ext target,
        group::internal_format        internalformat,
        vec_2_i32 const&              x,
        size_2_i32 const&             width)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CopyConvolutionFilter2DEXT)
    }
    glCopyConvolutionFilter2DEXT(
        static_cast<GLenum>(target),
        static_cast<GLenum>(internalformat),
        x.x(),
        x.y(),
        width[0],
        width[1]);
    detail::error_check("CopyConvolutionFilter2DEXT"sv);
}

template<class span_void>
requires(semantic::concepts::Span<span_void>) STATICINLINE
    void get_convolution_filter(
        group::convolution_target_ext target,
        group::pixel_format           format,
        group::pixel_type             type,
        span_void                     image)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetConvolutionFilterEXT)
    }
    glGetConvolutionFilterEXT(
        static_cast<GLenum>(target),
        static_cast<GLenum>(format),
        static_cast<GLenum>(type),
        image.size() ? reinterpret_cast<void*>(image.data()) : nullptr);
    detail::error_check("GetConvolutionFilterEXT"sv);
}

template<class span_f32>
requires(semantic::concepts::Span<span_f32>&& std::is_same_v<
         std::decay_t<typename span_f32::value_type>,
         std::decay_t<f32>>) STATICINLINE
    void get_convolution_parameter(
        group::convolution_target_ext    target,
        group::convolution_parameter_ext pname,
        span_f32                         params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetConvolutionParameterfvEXT)
    }
    glGetConvolutionParameterfvEXT(
        static_cast<GLenum>(target),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLfloat*>(params.data()) : nullptr);
    detail::error_check("GetConvolutionParameterfvEXT"sv);
}

template<class span_i32>
requires(semantic::concepts::Span<span_i32>&& std::is_same_v<
         std::decay_t<typename span_i32::value_type>,
         std::decay_t<i32>>) STATICINLINE
    void get_convolution_parameter(
        group::convolution_target_ext    target,
        group::convolution_parameter_ext pname,
        span_i32                         params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetConvolutionParameterivEXT)
    }
    glGetConvolutionParameterivEXT(
        static_cast<GLenum>(target),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetConvolutionParameterivEXT"sv);
}

template<class span_void>
requires(semantic::concepts::Span<span_void>) STATICINLINE
    void get_separable_filter(
        group::separable_target_ext target,
        group::pixel_format         format,
        group::pixel_type           type,
        span_void                   row,
        span_void                   column,
        span_void                   span)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetSeparableFilterEXT)
    }
    glGetSeparableFilterEXT(
        static_cast<GLenum>(target),
        static_cast<GLenum>(format),
        static_cast<GLenum>(type),
        row.size() ? reinterpret_cast<void*>(row.data()) : nullptr,
        column.size() ? reinterpret_cast<void*>(column.data()) : nullptr,
        span.size() ? reinterpret_cast<void*>(span.data()) : nullptr);
    detail::error_check("GetSeparableFilterEXT"sv);
}

template<class size_2_i32, class span_const_void>
requires(semantic::concepts::Size2D<size_2_i32, i32>&&
             semantic::concepts::Span<span_const_void>) STATICINLINE
    void separable_filter_2d(
        group::separable_target_ext target,
        group::internal_format      internalformat,
        size_2_i32 const&           width,
        group::pixel_format         format,
        group::pixel_type           type,
        span_const_void const&      row,
        span_const_void const&      column)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(SeparableFilter2DEXT)
    }
    glSeparableFilter2DEXT(
        static_cast<GLenum>(target),
        static_cast<GLenum>(internalformat),
        width[0],
        width[1],
        static_cast<GLenum>(format),
        static_cast<GLenum>(type),
        row.size() ? reinterpret_cast<const void*>(row.data()) : nullptr,
        column.size() ? reinterpret_cast<const void*>(column.data()) : nullptr);
    detail::error_check("SeparableFilter2DEXT"sv);
}

} // namespace gl::ext::convolution
#endif // GL_EXT_convolution
namespace gl::ext::convolution {
constexpr auto name = "GL_EXT_convolution";
}
