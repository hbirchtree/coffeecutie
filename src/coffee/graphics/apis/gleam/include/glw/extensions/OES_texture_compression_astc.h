#pragma once

#ifdef GL_OES_texture_compression_astc
#include "../enums/InternalFormat.h"
#include "../enums/SizedInternalFormat.h"
namespace gl::oes::texture_compression_astc {
using gl::group::internal_format;
using gl::group::sized_internal_format;
namespace values {
} // namespace values
} // namespace gl::oes::texture_compression_astc
#endif // GL_OES_texture_compression_astc
namespace gl::oes::texture_compression_astc {
constexpr auto name = "GL_OES_texture_compression_astc";
}
