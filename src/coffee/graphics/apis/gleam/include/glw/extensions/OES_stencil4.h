#pragma once

#ifdef GL_OES_stencil4
#include "../enums/InternalFormat.h"
#include "../enums/SizedInternalFormat.h"
namespace gl::oes::stencil4 {
using gl::group::internal_format;
using gl::group::sized_internal_format;
namespace values {
} // namespace values
} // namespace gl::oes::stencil4
#endif // GL_OES_stencil4
namespace gl::oes::stencil4 {
constexpr auto name = "GL_OES_stencil4";
}
