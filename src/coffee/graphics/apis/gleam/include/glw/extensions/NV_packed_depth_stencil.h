#pragma once

#ifdef GL_NV_packed_depth_stencil
#include "../enums/InternalFormat.h"
#include "../enums/PixelType.h"

namespace gl::nv::packed_depth_stencil {
using gl::group::internal_format;
using gl::group::pixel_type;

namespace values {
} // namespace values
} // namespace gl::nv::packed_depth_stencil
#endif // GL_NV_packed_depth_stencil
namespace gl::nv::packed_depth_stencil {
constexpr auto name = "GL_NV_packed_depth_stencil";
}
