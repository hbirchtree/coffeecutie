#pragma once

#ifdef GL_OES_texture_stencil8
#include "../enums/InternalFormat.h"
#include "../enums/SizedInternalFormat.h"
namespace gl::oes::texture_stencil8 {
using gl::group::internal_format;
using gl::group::sized_internal_format;
namespace values {
} // namespace values
} // namespace gl::oes::texture_stencil8
#endif // GL_OES_texture_stencil8
namespace gl::oes::texture_stencil8 {
constexpr auto name = "GL_OES_texture_stencil8";
}
