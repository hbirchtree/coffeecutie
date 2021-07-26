#pragma once

#ifdef GL_EXT_texture_compression_rgtc
#include "../enums/InternalFormat.h"
#include "../enums/SizedInternalFormat.h"
namespace gl::ext::texture_compression_rgtc {
using gl::group::internal_format;
using gl::group::sized_internal_format;
namespace values {
} // namespace values
} // namespace gl::ext::texture_compression_rgtc
#endif // GL_EXT_texture_compression_rgtc
namespace gl::ext::texture_compression_rgtc {
constexpr auto name = "GL_EXT_texture_compression_rgtc";
}
