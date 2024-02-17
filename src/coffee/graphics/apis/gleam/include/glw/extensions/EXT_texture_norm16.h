#pragma once

#ifdef GL_EXT_texture_norm16
#include "../enums/InternalFormat.h"
#include "../enums/SizedInternalFormat.h"

namespace gl::ext::texture_norm16 {
using gl::group::internal_format;
using gl::group::sized_internal_format;

namespace values {
} // namespace values
} // namespace gl::ext::texture_norm16
#endif // GL_EXT_texture_norm16
namespace gl::ext::texture_norm16 {
constexpr auto name = "GL_EXT_texture_norm16";
}
