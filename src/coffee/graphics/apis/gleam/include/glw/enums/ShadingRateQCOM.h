#pragma once

#include "common.h"

namespace gl::group {

// ShadingRateQCOM
enum class shading_rate_qcom : u32
{
    shading_rate_1x1_pixels_qcom = 0x96A6, // GL_SHADING_RATE_1X1_PIXELS_QCOM
    shading_rate_1x2_pixels_qcom = 0x96A7, // GL_SHADING_RATE_1X2_PIXELS_QCOM
    shading_rate_1x4_pixels_qcom = 0x96AA, // GL_SHADING_RATE_1X4_PIXELS_QCOM
    shading_rate_2x1_pixels_qcom = 0x96A8, // GL_SHADING_RATE_2X1_PIXELS_QCOM
    shading_rate_2x2_pixels_qcom = 0x96A9, // GL_SHADING_RATE_2X2_PIXELS_QCOM
    shading_rate_2x4_pixels_qcom = 0x96AD, // GL_SHADING_RATE_2X4_PIXELS_QCOM
    shading_rate_4x1_pixels_qcom = 0x96AB, // GL_SHADING_RATE_4X1_PIXELS_QCOM
    shading_rate_4x2_pixels_qcom = 0x96AC, // GL_SHADING_RATE_4X2_PIXELS_QCOM
    shading_rate_4x4_pixels_qcom = 0x96AE, // GL_SHADING_RATE_4X4_PIXELS_QCOM
}; // enum class shading_rate_qcom

} // namespace gl::group
