#pragma once

#include "common.h"

namespace gl::group {

// FoveationConfigBitQCOM
enum class foveation_config_bit_qcom : u32
{
    foveation_enable_bit_qcom = 0x00000001, // GL_FOVEATION_ENABLE_BIT_QCOM
    foveation_scaled_bin_method_bit_qcom =
        0x00000002, // GL_FOVEATION_SCALED_BIN_METHOD_BIT_QCOM
    foveation_subsampled_layout_method_bit_qcom =
        0x00000004, // GL_FOVEATION_SUBSAMPLED_LAYOUT_METHOD_BIT_QCOM
}; // enum class foveation_config_bit_qcom
C_FLAGS(foveation_config_bit_qcom, u32);

} // namespace gl::group
