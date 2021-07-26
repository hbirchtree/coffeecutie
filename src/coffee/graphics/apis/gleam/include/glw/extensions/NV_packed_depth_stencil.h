#pragma once

#ifdef GL_NV_packed_depth_stencil
#include "../enums/InternalFormat.h"
namespace gl::nv::packed_depth_stencil {
using gl::group::internal_format;
namespace values {
constexpr libc_types::u32 unsigned_int_24_8 = 0x84FA;
} // namespace values
} // namespace gl::nv::packed_depth_stencil
#endif // GL_NV_packed_depth_stencil
namespace gl::nv::packed_depth_stencil {
constexpr auto name = "GL_NV_packed_depth_stencil";
}
