#pragma once

#ifdef GL_ARB_imaging
#include "../enums/BlendEquationModeEXT.h"
#include "../enums/BlendingFactor.h"
#include "../enums/ColorTableParameterPName.h"
#include "../enums/ColorTableTarget.h"
#include "../enums/ColorTableTargetSGI.h"
#include "../enums/ConvolutionBorderModeEXT.h"
#include "../enums/ConvolutionParameter.h"
#include "../enums/ConvolutionTarget.h"
#include "../enums/ConvolutionTargetEXT.h"
#include "../enums/EnableCap.h"
#include "../enums/ErrorCode.h"
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
using gl::group::color_table_target;
using gl::group::color_table_target_sgi;
using gl::group::convolution_border_mode_ext;
using gl::group::convolution_parameter;
using gl::group::convolution_target;
using gl::group::convolution_target_ext;
using gl::group::enable_cap;
using gl::group::error_code;
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
#if defined(GL_VERSION_1_0)
constexpr u32 color_matrix = 0x80B1;
#endif
#if defined(GL_VERSION_1_0)
constexpr u32 color_matrix_stack_depth = 0x80B2;
#endif
#if defined(GL_VERSION_1_0)
constexpr u32 max_color_matrix_stack_depth = 0x80B3;
#endif
#if defined(GL_VERSION_1_0)
constexpr u32 constant_border = 0x8151;
#endif
#if defined(GL_VERSION_1_0)
constexpr u32 replicate_border = 0x8153;
#endif
} // namespace values

template<class vec_4_f32>
requires(concepts::vector<vec_4_f32, f32, 4>)
/*!
 * \brief Part of GL_ARB_imaging
 * \param red GLfloat
 * \param green GLfloat
 * \param blue GLfloat
 * \param alpha GLfloat
 * \return void
 */
STATICINLINE void blend_color(vec_4_f32 const& red)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BlendColor)
    }
    glBlendColor(red[0], red[1], red[2], red[3]);
    detail::error_check("BlendColor"sv);
}

/*!
 * \brief Part of GL_ARB_imaging
 * \param mode GLenum
 * \return void
 */
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

} // namespace gl::arb::imaging
#endif // GL_ARB_imaging
namespace gl::arb::imaging {
constexpr auto name = "GL_ARB_imaging";
}
