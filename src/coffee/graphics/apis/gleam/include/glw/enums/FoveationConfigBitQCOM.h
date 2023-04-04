#pragma once

#include "common.h"

namespace gl::group {

// FoveationConfigBitQCOM
enum class foveation_config_bit_qcom : u32
{
#ifdef GL_FOVEATION_ENABLE_BIT_QCOM
    foveation_enable_bit_qcom = GL_FOVEATION_ENABLE_BIT_QCOM,
#endif
#ifdef GL_FOVEATION_SCALED_BIN_METHOD_BIT_QCOM
    foveation_scaled_bin_method_bit_qcom =
        GL_FOVEATION_SCALED_BIN_METHOD_BIT_QCOM,
#endif
#ifdef GL_FOVEATION_SUBSAMPLED_LAYOUT_METHOD_BIT_QCOM
    foveation_subsampled_layout_method_bit_qcom =
        GL_FOVEATION_SUBSAMPLED_LAYOUT_METHOD_BIT_QCOM,
#endif
}; // enum class foveation_config_bit_qcom
C_FLAGS(foveation_config_bit_qcom, ::libc_types::u32);

} // namespace gl::group
