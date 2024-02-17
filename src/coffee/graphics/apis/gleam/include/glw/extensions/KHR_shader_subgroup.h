#pragma once

#ifdef GL_KHR_shader_subgroup
#include "../enums/SubgroupSupportedFeatures.h"

namespace gl::khr::shader_subgroup {
using gl::group::subgroup_supported_features;

namespace values {
constexpr u32 subgroup_size               = 0x9532;
constexpr u32 subgroup_supported_stages   = 0x9533;
constexpr u32 subgroup_supported_features = 0x9534;
constexpr u32 subgroup_quad_all_stages    = 0x9535;
} // namespace values
} // namespace gl::khr::shader_subgroup
#endif // GL_KHR_shader_subgroup
namespace gl::khr::shader_subgroup {
constexpr auto name = "GL_KHR_shader_subgroup";
}
