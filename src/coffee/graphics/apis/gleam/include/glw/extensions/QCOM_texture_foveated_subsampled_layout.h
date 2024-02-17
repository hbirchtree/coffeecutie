#pragma once

#ifdef GL_QCOM_texture_foveated_subsampled_layout
#include "../enums/FoveationConfigBitQCOM.h"

namespace gl::qcom::texture_foveated_subsampled_layout {
using gl::group::foveation_config_bit_qcom;

namespace values {
constexpr u32 max_shader_subsampled_image_units = 0x8FA1;
} // namespace values
} // namespace gl::qcom::texture_foveated_subsampled_layout
#endif // GL_QCOM_texture_foveated_subsampled_layout
namespace gl::qcom::texture_foveated_subsampled_layout {
constexpr auto name = "GL_QCOM_texture_foveated_subsampled_layout";
}
