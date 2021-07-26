#pragma once

#ifdef GL_ARB_imaging
#include "../enums/BlendEquationModeEXT.h"
#include "../enums/BlendingFactor.h"
#include "../enums/ColorTableParameterPName.h"
#include "../enums/ColorTableParameterPNameSGI.h"
#include "../enums/ColorTableTarget.h"
#include "../enums/ColorTableTargetSGI.h"
#include "../enums/ConvolutionBorderModeEXT.h"
#include "../enums/ConvolutionParameterEXT.h"
#include "../enums/ConvolutionTarget.h"
#include "../enums/ConvolutionTargetEXT.h"
#include "../enums/EnableCap.h"
#include "../enums/ErrorCode.h"
#include "../enums/GetColorTableParameterPName.h"
#include "../enums/GetColorTableParameterPNameSGI.h"
#include "../enums/GetConvolutionParameter.h"
#include "../enums/GetHistogramParameterPNameEXT.h"
#include "../enums/GetMinmaxParameterPNameEXT.h"
#include "../enums/GetPName.h"
#include "../enums/HistogramTarget.h"
#include "../enums/HistogramTargetEXT.h"
#include "../enums/MinmaxTarget.h"
#include "../enums/MinmaxTargetEXT.h"
#include "../enums/PixelTransferParameter.h"
#include "../enums/SeparableTarget.h"
#include "../enums/SeparableTargetEXT.h"
namespace gl::arb::imaging {
using gl::group::blend_equation_mode_ext;
using gl::group::blending_factor;
using gl::group::color_table_parameter_prop;
using gl::group::color_table_parameter_prop_sgi;
using gl::group::color_table_target;
using gl::group::color_table_target_sgi;
using gl::group::convolution_border_mode_ext;
using gl::group::convolution_parameter_ext;
using gl::group::convolution_target;
using gl::group::convolution_target_ext;
using gl::group::enable_cap;
using gl::group::error_code;
using gl::group::get_color_table_parameter_prop;
using gl::group::get_color_table_parameter_prop_sgi;
using gl::group::get_convolution_parameter;
using gl::group::get_histogram_parameter_prop_ext;
using gl::group::get_minmax_parameter_prop_ext;
using gl::group::get_prop;
using gl::group::histogram_target;
using gl::group::histogram_target_ext;
using gl::group::minmax_target;
using gl::group::minmax_target_ext;
using gl::group::pixel_transfer_parameter;
using gl::group::separable_target;
using gl::group::separable_target_ext;
namespace values {
constexpr libc_types::u32 blend_equation = 0x8009;
#if defined(GL_VERSION_1_0)
constexpr libc_types::u32 color_matrix = 0x80B1;
#endif
#if defined(GL_VERSION_1_0)
constexpr libc_types::u32 color_matrix_stack_depth = 0x80B2;
#endif
#if defined(GL_VERSION_1_0)
constexpr libc_types::u32 max_color_matrix_stack_depth = 0x80B3;
#endif
#if defined(GL_VERSION_1_0)
constexpr libc_types::u32 constant_border = 0x8151;
#endif
#if defined(GL_VERSION_1_0)
constexpr libc_types::u32 replicate_border = 0x8153;
#endif
} // namespace values
template<class vec_4_f32>
requires(semantic::concepts::Vector<vec_4_f32, f32, 4>) STATICINLINE
    void blend_color(vec_4_f32 const& red)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BlendColor)
    }
    glBlendColor(red.x(), red.y(), red.z(), red.w());
    detail::error_check("BlendColor"sv);
}

STATICINLINE void blend_equation(group::blend_equation_mode_ext mode)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BlendEquation)
    }
    glBlendEquation(static_cast<GLenum>(mode));
    detail::error_check("BlendEquation"sv);
}

#if defined(GL_VERSION_1_0)
template<class span_const_void>
requires(semantic::concepts::Span<span_const_void>) STATICINLINE
    void color_sub_table(
        group::color_table_target target,
        i32                       start,
        i32                       count,
        group::pixel_format       format,
        group::pixel_type         type,
        span_const_void const&    data)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ColorSubTable)
    }
    glColorSubTable(
        static_cast<GLenum>(target),
        start,
        count,
        static_cast<GLenum>(format),
        static_cast<GLenum>(type),
        data.size() ? reinterpret_cast<const void*>(data.data()) : nullptr);
    detail::error_check("ColorSubTable"sv);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_const_void>
requires(semantic::concepts::Span<span_const_void>) STATICINLINE
    void color_table(
        group::color_table_target target,
        group::internal_format    internalformat,
        i32                       width,
        group::pixel_format       format,
        group::pixel_type         type,
        span_const_void const&    table)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ColorTable)
    }
    glColorTable(
        static_cast<GLenum>(target),
        static_cast<GLenum>(internalformat),
        width,
        static_cast<GLenum>(format),
        static_cast<GLenum>(type),
        table.size() ? reinterpret_cast<const void*>(table.data()) : nullptr);
    detail::error_check("ColorTable"sv);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_const_f32>
requires(semantic::concepts::Span<span_const_f32>&& std::is_same_v<
         std::decay_t<typename span_const_f32::value_type>,
         std::decay_t<f32>>) STATICINLINE
    void color_table_parameter(
        group::color_table_target             target,
        group::color_table_parameter_prop_sgi pname,
        span_const_f32 const&                 params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ColorTableParameterfv)
    }
    glColorTableParameterfv(
        static_cast<GLenum>(target),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<const GLfloat*>(params.data())
                      : nullptr);
    detail::error_check("ColorTableParameterfv"sv);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_const_i32>
requires(semantic::concepts::Span<span_const_i32>&& std::is_same_v<
         std::decay_t<typename span_const_i32::value_type>,
         std::decay_t<i32>>) STATICINLINE
    void color_table_parameter(
        group::color_table_target             target,
        group::color_table_parameter_prop_sgi pname,
        span_const_i32 const&                 params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ColorTableParameteriv)
    }
    glColorTableParameteriv(
        static_cast<GLenum>(target),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<const GLint*>(params.data())
                      : nullptr);
    detail::error_check("ColorTableParameteriv"sv);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_const_void>
requires(semantic::concepts::Span<span_const_void>) STATICINLINE
    void convolution_filter_1d(
        group::convolution_target target,
        group::internal_format    internalformat,
        i32                       width,
        group::pixel_format       format,
        group::pixel_type         type,
        span_const_void const&    image)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ConvolutionFilter1D)
    }
    glConvolutionFilter1D(
        static_cast<GLenum>(target),
        static_cast<GLenum>(internalformat),
        width,
        static_cast<GLenum>(format),
        static_cast<GLenum>(type),
        image.size() ? reinterpret_cast<const void*>(image.data()) : nullptr);
    detail::error_check("ConvolutionFilter1D"sv);
}

#endif
#if defined(GL_VERSION_1_0)
template<class size_2_i32, class span_const_void>
requires(semantic::concepts::Size2D<size_2_i32, i32>&&
             semantic::concepts::Span<span_const_void>) STATICINLINE
    void convolution_filter_2d(
        group::convolution_target target,
        group::internal_format    internalformat,
        size_2_i32 const&         width,
        group::pixel_format       format,
        group::pixel_type         type,
        span_const_void const&    image)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ConvolutionFilter2D)
    }
    glConvolutionFilter2D(
        static_cast<GLenum>(target),
        static_cast<GLenum>(internalformat),
        width[0],
        width[1],
        static_cast<GLenum>(format),
        static_cast<GLenum>(type),
        image.size() ? reinterpret_cast<const void*>(image.data()) : nullptr);
    detail::error_check("ConvolutionFilter2D"sv);
}

#endif
#if defined(GL_VERSION_1_0)
STATICINLINE void convolution_parameter(
    group::convolution_target        target,
    group::convolution_parameter_ext pname,
    f32                              params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ConvolutionParameterf)
    }
    glConvolutionParameterf(
        static_cast<GLenum>(target), static_cast<GLenum>(pname), params);
    detail::error_check("ConvolutionParameterf"sv);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_const_f32>
requires(semantic::concepts::Span<span_const_f32>&& std::is_same_v<
         std::decay_t<typename span_const_f32::value_type>,
         std::decay_t<f32>>) STATICINLINE
    void convolution_parameter(
        group::convolution_target        target,
        group::convolution_parameter_ext pname,
        span_const_f32 const&            params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ConvolutionParameterfv)
    }
    glConvolutionParameterfv(
        static_cast<GLenum>(target),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<const GLfloat*>(params.data())
                      : nullptr);
    detail::error_check("ConvolutionParameterfv"sv);
}

#endif
#if defined(GL_VERSION_1_0)
STATICINLINE void convolution_parameter(
    group::convolution_target        target,
    group::convolution_parameter_ext pname,
    i32                              params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ConvolutionParameteri)
    }
    glConvolutionParameteri(
        static_cast<GLenum>(target), static_cast<GLenum>(pname), params);
    detail::error_check("ConvolutionParameteri"sv);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_const_i32>
requires(semantic::concepts::Span<span_const_i32>&& std::is_same_v<
         std::decay_t<typename span_const_i32::value_type>,
         std::decay_t<i32>>) STATICINLINE
    void convolution_parameter(
        group::convolution_target        target,
        group::convolution_parameter_ext pname,
        span_const_i32 const&            params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ConvolutionParameteriv)
    }
    glConvolutionParameteriv(
        static_cast<GLenum>(target),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<const GLint*>(params.data())
                      : nullptr);
    detail::error_check("ConvolutionParameteriv"sv);
}

#endif
#if defined(GL_VERSION_1_0)
template<class vec_2_i32>
requires(semantic::concepts::Vector<vec_2_i32, i32, 2>) STATICINLINE
    void copy_color_sub_table(
        group::color_table_target target,
        i32                       start,
        vec_2_i32 const&          x,
        i32                       width)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CopyColorSubTable)
    }
    glCopyColorSubTable(
        static_cast<GLenum>(target), start, x.x(), x.y(), width);
    detail::error_check("CopyColorSubTable"sv);
}

#endif
#if defined(GL_VERSION_1_0)
template<class vec_2_i32>
requires(semantic::concepts::Vector<vec_2_i32, i32, 2>) STATICINLINE
    void copy_color_table(
        group::color_table_target target,
        group::internal_format    internalformat,
        vec_2_i32 const&          x,
        i32                       width)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CopyColorTable)
    }
    glCopyColorTable(
        static_cast<GLenum>(target),
        static_cast<GLenum>(internalformat),
        x.x(),
        x.y(),
        width);
    detail::error_check("CopyColorTable"sv);
}

#endif
#if defined(GL_VERSION_1_0)
template<class vec_2_i32>
requires(semantic::concepts::Vector<vec_2_i32, i32, 2>) STATICINLINE
    void copy_convolution_filter_1d(
        group::convolution_target target,
        group::internal_format    internalformat,
        vec_2_i32 const&          x,
        i32                       width)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CopyConvolutionFilter1D)
    }
    glCopyConvolutionFilter1D(
        static_cast<GLenum>(target),
        static_cast<GLenum>(internalformat),
        x.x(),
        x.y(),
        width);
    detail::error_check("CopyConvolutionFilter1D"sv);
}

#endif
#if defined(GL_VERSION_1_0)
template<class size_2_i32, class vec_2_i32>
requires(semantic::concepts::Vector<vec_2_i32, i32, 2>&&
             semantic::concepts::Size2D<size_2_i32, i32>) STATICINLINE
    void copy_convolution_filter_2d(
        group::convolution_target target,
        group::internal_format    internalformat,
        vec_2_i32 const&          x,
        size_2_i32 const&         width)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CopyConvolutionFilter2D)
    }
    glCopyConvolutionFilter2D(
        static_cast<GLenum>(target),
        static_cast<GLenum>(internalformat),
        x.x(),
        x.y(),
        width[0],
        width[1]);
    detail::error_check("CopyConvolutionFilter2D"sv);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_void>
requires(semantic::concepts::Span<span_void>) STATICINLINE void get_color_table(
    group::color_table_target target,
    group::pixel_format       format,
    group::pixel_type         type,
    span_void                 table)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetColorTable)
    }
    glGetColorTable(
        static_cast<GLenum>(target),
        static_cast<GLenum>(format),
        static_cast<GLenum>(type),
        table.size() ? reinterpret_cast<void*>(table.data()) : nullptr);
    detail::error_check("GetColorTable"sv);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_f32>
requires(semantic::concepts::Span<span_f32>&& std::is_same_v<
         std::decay_t<typename span_f32::value_type>,
         std::decay_t<f32>>) STATICINLINE
    void get_color_table_parameter(
        group::color_table_target                 target,
        group::get_color_table_parameter_prop_sgi pname,
        span_f32                                  params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetColorTableParameterfv)
    }
    glGetColorTableParameterfv(
        static_cast<GLenum>(target),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLfloat*>(params.data()) : nullptr);
    detail::error_check("GetColorTableParameterfv"sv);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_i32>
requires(semantic::concepts::Span<span_i32>&& std::is_same_v<
         std::decay_t<typename span_i32::value_type>,
         std::decay_t<i32>>) STATICINLINE
    void get_color_table_parameter(
        group::color_table_target                 target,
        group::get_color_table_parameter_prop_sgi pname,
        span_i32                                  params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetColorTableParameteriv)
    }
    glGetColorTableParameteriv(
        static_cast<GLenum>(target),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetColorTableParameteriv"sv);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_void>
requires(semantic::concepts::Span<span_void>) STATICINLINE
    void get_convolution_filter(
        group::convolution_target target,
        group::pixel_format       format,
        group::pixel_type         type,
        span_void                 image)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetConvolutionFilter)
    }
    glGetConvolutionFilter(
        static_cast<GLenum>(target),
        static_cast<GLenum>(format),
        static_cast<GLenum>(type),
        image.size() ? reinterpret_cast<void*>(image.data()) : nullptr);
    detail::error_check("GetConvolutionFilter"sv);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_f32>
requires(semantic::concepts::Span<span_f32>&& std::is_same_v<
         std::decay_t<typename span_f32::value_type>,
         std::decay_t<f32>>) STATICINLINE
    void get_convolution_parameter(
        group::convolution_target        target,
        group::convolution_parameter_ext pname,
        span_f32                         params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetConvolutionParameterfv)
    }
    glGetConvolutionParameterfv(
        static_cast<GLenum>(target),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLfloat*>(params.data()) : nullptr);
    detail::error_check("GetConvolutionParameterfv"sv);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_i32>
requires(semantic::concepts::Span<span_i32>&& std::is_same_v<
         std::decay_t<typename span_i32::value_type>,
         std::decay_t<i32>>) STATICINLINE
    void get_convolution_parameter(
        group::convolution_target        target,
        group::convolution_parameter_ext pname,
        span_i32                         params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetConvolutionParameteriv)
    }
    glGetConvolutionParameteriv(
        static_cast<GLenum>(target),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetConvolutionParameteriv"sv);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_void>
requires(semantic::concepts::Span<span_void>) STATICINLINE void get_histogram(
    group::histogram_target_ext target,
    bool                        reset,
    group::pixel_format         format,
    group::pixel_type           type,
    span_void                   values)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetHistogram)
    }
    glGetHistogram(
        static_cast<GLenum>(target),
        reset,
        static_cast<GLenum>(format),
        static_cast<GLenum>(type),
        values.size() ? reinterpret_cast<void*>(values.data()) : nullptr);
    detail::error_check("GetHistogram"sv);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_f32>
requires(semantic::concepts::Span<span_f32>&& std::is_same_v<
         std::decay_t<typename span_f32::value_type>,
         std::decay_t<f32>>) STATICINLINE
    void get_histogram_parameter(
        group::histogram_target_ext             target,
        group::get_histogram_parameter_prop_ext pname,
        span_f32                                params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetHistogramParameterfv)
    }
    glGetHistogramParameterfv(
        static_cast<GLenum>(target),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLfloat*>(params.data()) : nullptr);
    detail::error_check("GetHistogramParameterfv"sv);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_i32>
requires(semantic::concepts::Span<span_i32>&& std::is_same_v<
         std::decay_t<typename span_i32::value_type>,
         std::decay_t<i32>>) STATICINLINE
    void get_histogram_parameter(
        group::histogram_target_ext             target,
        group::get_histogram_parameter_prop_ext pname,
        span_i32                                params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetHistogramParameteriv)
    }
    glGetHistogramParameteriv(
        static_cast<GLenum>(target),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetHistogramParameteriv"sv);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_void>
requires(semantic::concepts::Span<span_void>) STATICINLINE void get_minmax(
    group::minmax_target_ext target,
    bool                     reset,
    group::pixel_format      format,
    group::pixel_type        type,
    span_void                values)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetMinmax)
    }
    glGetMinmax(
        static_cast<GLenum>(target),
        reset,
        static_cast<GLenum>(format),
        static_cast<GLenum>(type),
        values.size() ? reinterpret_cast<void*>(values.data()) : nullptr);
    detail::error_check("GetMinmax"sv);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_f32>
requires(semantic::concepts::Span<span_f32>&& std::is_same_v<
         std::decay_t<typename span_f32::value_type>,
         std::decay_t<f32>>) STATICINLINE
    void get_minmax_parameter(
        group::minmax_target_ext             target,
        group::get_minmax_parameter_prop_ext pname,
        span_f32                             params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetMinmaxParameterfv)
    }
    glGetMinmaxParameterfv(
        static_cast<GLenum>(target),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLfloat*>(params.data()) : nullptr);
    detail::error_check("GetMinmaxParameterfv"sv);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_i32>
requires(semantic::concepts::Span<span_i32>&& std::is_same_v<
         std::decay_t<typename span_i32::value_type>,
         std::decay_t<i32>>) STATICINLINE
    void get_minmax_parameter(
        group::minmax_target_ext             target,
        group::get_minmax_parameter_prop_ext pname,
        span_i32                             params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetMinmaxParameteriv)
    }
    glGetMinmaxParameteriv(
        static_cast<GLenum>(target),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetMinmaxParameteriv"sv);
}

#endif
#if defined(GL_VERSION_1_0)
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
        GLW_FPTR_CHECK(GetSeparableFilter)
    }
    glGetSeparableFilter(
        static_cast<GLenum>(target),
        static_cast<GLenum>(format),
        static_cast<GLenum>(type),
        row.size() ? reinterpret_cast<void*>(row.data()) : nullptr,
        column.size() ? reinterpret_cast<void*>(column.data()) : nullptr,
        span.size() ? reinterpret_cast<void*>(span.data()) : nullptr);
    detail::error_check("GetSeparableFilter"sv);
}

#endif
#if defined(GL_VERSION_1_0)
STATICINLINE void histogram(
    group::histogram_target_ext target,
    i32                         width,
    group::internal_format      internalformat,
    bool                        sink)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Histogram)
    }
    glHistogram(
        static_cast<GLenum>(target),
        width,
        static_cast<GLenum>(internalformat),
        sink);
    detail::error_check("Histogram"sv);
}

#endif
#if defined(GL_VERSION_1_0)
STATICINLINE void minmax(
    group::minmax_target_ext target,
    group::internal_format   internalformat,
    bool                     sink)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Minmax)
    }
    glMinmax(
        static_cast<GLenum>(target), static_cast<GLenum>(internalformat), sink);
    detail::error_check("Minmax"sv);
}

#endif
#if defined(GL_VERSION_1_0)
STATICINLINE void reset_histogram(group::histogram_target_ext target)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ResetHistogram)
    }
    glResetHistogram(static_cast<GLenum>(target));
    detail::error_check("ResetHistogram"sv);
}

#endif
#if defined(GL_VERSION_1_0)
STATICINLINE void reset_minmax(group::minmax_target_ext target)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ResetMinmax)
    }
    glResetMinmax(static_cast<GLenum>(target));
    detail::error_check("ResetMinmax"sv);
}

#endif
#if defined(GL_VERSION_1_0)
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
        GLW_FPTR_CHECK(SeparableFilter2D)
    }
    glSeparableFilter2D(
        static_cast<GLenum>(target),
        static_cast<GLenum>(internalformat),
        width[0],
        width[1],
        static_cast<GLenum>(format),
        static_cast<GLenum>(type),
        row.size() ? reinterpret_cast<const void*>(row.data()) : nullptr,
        column.size() ? reinterpret_cast<const void*>(column.data()) : nullptr);
    detail::error_check("SeparableFilter2D"sv);
}

#endif
} // namespace gl::arb::imaging
#endif // GL_ARB_imaging
namespace gl::arb::imaging {
constexpr auto name = "GL_ARB_imaging";
}
