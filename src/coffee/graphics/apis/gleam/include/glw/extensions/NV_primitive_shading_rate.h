#pragma once

#ifdef GL_NV_primitive_shading_rate
#include "../enums/EnableCap.h"
#include "../enums/GetPName.h"

namespace gl::nv::primitive_shading_rate {
using gl::group::enable_cap;
using gl::group::get_prop;

namespace values {
} // namespace values
} // namespace gl::nv::primitive_shading_rate
#endif // GL_NV_primitive_shading_rate
namespace gl::nv::primitive_shading_rate {
constexpr auto name = "GL_NV_primitive_shading_rate";
}
