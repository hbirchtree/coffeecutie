#pragma once

#ifdef GL_ARB_texture_buffer_object_rgb32
#include "../enums/InternalFormat.h"
#include "../enums/SizedInternalFormat.h"
namespace gl::arb::texture_buffer_object_rgb32 {
using gl::group::internal_format;
using gl::group::sized_internal_format;
namespace values {
} // namespace values
} // namespace gl::arb::texture_buffer_object_rgb32
#endif // GL_ARB_texture_buffer_object_rgb32
namespace gl::arb::texture_buffer_object_rgb32 {
constexpr auto name = "GL_ARB_texture_buffer_object_rgb32";
}
