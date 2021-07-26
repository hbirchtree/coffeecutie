#pragma once

#ifdef GL_OES_stencil1
#include "../enums/InternalFormat.h"
#include "../enums/SizedInternalFormat.h"
namespace gl::oes::stencil1 {
using gl::group::internal_format;
using gl::group::sized_internal_format;
namespace values {
} // namespace values
} // namespace gl::oes::stencil1
#endif // GL_OES_stencil1
namespace gl::oes::stencil1 {
constexpr auto name = "GL_OES_stencil1";
}
