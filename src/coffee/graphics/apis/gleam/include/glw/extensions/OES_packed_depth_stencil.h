#pragma once

#ifdef GL_OES_packed_depth_stencil
#include "../enums/InternalFormat.h"
#include "../enums/SizedInternalFormat.h"
namespace gl::oes::packed_depth_stencil {
using gl::group::internal_format;
using gl::group::sized_internal_format;
namespace values {
constexpr libc_types::u32 unsigned_int_24_8 = 0x84FA;
} // namespace values
} // namespace gl::oes::packed_depth_stencil
#endif // GL_OES_packed_depth_stencil
namespace gl::oes::packed_depth_stencil {
constexpr auto name = "GL_OES_packed_depth_stencil";
}
