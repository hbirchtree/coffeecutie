#pragma once

#ifdef GL_QCOM_motion_estimation
#include "../enums/FoveationConfigBitQCOM.h"
#include "../enums/GetPName.h"
namespace gl::qcom::motion_estimation {
using gl::group::foveation_config_bit_qcom;
using gl::group::get_prop;
namespace values {
} // namespace values
/*!
 * \brief Part of GL_QCOM_motion_estimation
 * \param ref GLuint
 * \param target GLuint
 * \param output GLuint
 * \return void
 */
STATICINLINE void tex_estimate_motion(u32 ref, u32 target, u32 output)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TexEstimateMotionQCOM)
    }
    glTexEstimateMotionQCOM(ref, target, output);
    detail::error_check("TexEstimateMotionQCOM"sv);
}

/*!
 * \brief Part of GL_QCOM_motion_estimation
 * \param ref GLuint
 * \param target GLuint
 * \param output GLuint
 * \param mask GLuint
 * \return void
 */
STATICINLINE void tex_estimate_motion_regions(
    u32 ref, u32 target, u32 output, u32 mask)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TexEstimateMotionRegionsQCOM)
    }
    glTexEstimateMotionRegionsQCOM(ref, target, output, mask);
    detail::error_check("TexEstimateMotionRegionsQCOM"sv);
}

} // namespace gl::qcom::motion_estimation
#endif // GL_QCOM_motion_estimation
namespace gl::qcom::motion_estimation {
constexpr auto name = "GL_QCOM_motion_estimation";
}
