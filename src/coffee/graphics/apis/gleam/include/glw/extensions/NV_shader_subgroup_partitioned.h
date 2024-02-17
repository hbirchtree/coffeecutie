#pragma once

#ifdef GL_NV_shader_subgroup_partitioned
#include "../enums/SubgroupSupportedFeatures.h"

namespace gl::nv::shader_subgroup_partitioned {
using gl::group::subgroup_supported_features;

namespace values {
} // namespace values
} // namespace gl::nv::shader_subgroup_partitioned
#endif // GL_NV_shader_subgroup_partitioned
namespace gl::nv::shader_subgroup_partitioned {
constexpr auto name = "GL_NV_shader_subgroup_partitioned";
}
