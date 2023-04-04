#pragma once

#include "common.h"

namespace gl::group {

// ShadingRateQCOM
enum class shading_rate_qcom : u32
{
#ifdef GL_SHADING_RATE_1X1_PIXELS_QCOM
    shading_rate_1x1_pixels_qcom = GL_SHADING_RATE_1X1_PIXELS_QCOM,
#endif
#ifdef GL_SHADING_RATE_1X2_PIXELS_QCOM
    shading_rate_1x2_pixels_qcom = GL_SHADING_RATE_1X2_PIXELS_QCOM,
#endif
#ifdef GL_SHADING_RATE_1X4_PIXELS_QCOM
    shading_rate_1x4_pixels_qcom = GL_SHADING_RATE_1X4_PIXELS_QCOM,
#endif
#ifdef GL_SHADING_RATE_2X1_PIXELS_QCOM
    shading_rate_2x1_pixels_qcom = GL_SHADING_RATE_2X1_PIXELS_QCOM,
#endif
#ifdef GL_SHADING_RATE_2X2_PIXELS_QCOM
    shading_rate_2x2_pixels_qcom = GL_SHADING_RATE_2X2_PIXELS_QCOM,
#endif
#ifdef GL_SHADING_RATE_2X4_PIXELS_QCOM
    shading_rate_2x4_pixels_qcom = GL_SHADING_RATE_2X4_PIXELS_QCOM,
#endif
#ifdef GL_SHADING_RATE_4X1_PIXELS_QCOM
    shading_rate_4x1_pixels_qcom = GL_SHADING_RATE_4X1_PIXELS_QCOM,
#endif
#ifdef GL_SHADING_RATE_4X2_PIXELS_QCOM
    shading_rate_4x2_pixels_qcom = GL_SHADING_RATE_4X2_PIXELS_QCOM,
#endif
#ifdef GL_SHADING_RATE_4X4_PIXELS_QCOM
    shading_rate_4x4_pixels_qcom = GL_SHADING_RATE_4X4_PIXELS_QCOM,
#endif
}; // enum class shading_rate_qcom

} // namespace gl::group
