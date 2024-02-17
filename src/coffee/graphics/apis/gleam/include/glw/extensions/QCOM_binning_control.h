#pragma once

#ifdef GL_QCOM_binning_control
#include "../enums/HintTarget.h"

namespace gl::qcom::binning_control {
using gl::group::hint_target;

namespace values {
constexpr u32 cpu_optimized                = 0x8FB1;
constexpr u32 gpu_optimized                = 0x8FB2;
constexpr u32 render_direct_to_framebuffer = 0x8FB3;
} // namespace values
} // namespace gl::qcom::binning_control
#endif // GL_QCOM_binning_control
namespace gl::qcom::binning_control {
constexpr auto name = "GL_QCOM_binning_control";
}
