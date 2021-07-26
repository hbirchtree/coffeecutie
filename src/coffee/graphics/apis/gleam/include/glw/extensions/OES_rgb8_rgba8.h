#pragma once

#ifdef GL_OES_rgb8_rgba8
#include "../enums/InternalFormat.h"
#include "../enums/SizedInternalFormat.h"
namespace gl::oes::rgb8_rgba8 {
using gl::group::internal_format;
using gl::group::sized_internal_format;
namespace values {
} // namespace values
} // namespace gl::oes::rgb8_rgba8
#endif // GL_OES_rgb8_rgba8
namespace gl::oes::rgb8_rgba8 {
constexpr auto name = "GL_OES_rgb8_rgba8";
}
