#pragma once

#ifdef GL_EXT_texture_compression_latc
#include "../enums/InternalFormat.h"
#include "../enums/SizedInternalFormat.h"

namespace gl::ext::texture_compression_latc {
using gl::group::internal_format;
using gl::group::sized_internal_format;

namespace values {
} // namespace values
} // namespace gl::ext::texture_compression_latc
#endif // GL_EXT_texture_compression_latc
namespace gl::ext::texture_compression_latc {
constexpr auto name = "GL_EXT_texture_compression_latc";
}
