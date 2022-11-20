#pragma once

#ifdef GL_EXT_packed_depth_stencil
#include "../enums/InternalFormat.h"
#include "../enums/PixelType.h"
#include "../enums/SizedInternalFormat.h"
namespace gl::ext::packed_depth_stencil {
using gl::group::internal_format;
using gl::group::pixel_type;
using gl::group::sized_internal_format;
namespace values {
constexpr libc_types::u32 texture_stencil_size = 0x88F1;
} // namespace values
} // namespace gl::ext::packed_depth_stencil
#endif // GL_EXT_packed_depth_stencil
namespace gl::ext::packed_depth_stencil {
constexpr auto name = "GL_EXT_packed_depth_stencil";
}
