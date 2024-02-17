#pragma once

#ifdef GL_EXT_texture_compression_s3tc
#include "../enums/InternalFormat.h"
#include "../enums/SizedInternalFormat.h"

namespace gl::ext::texture_compression_s3tc {
using gl::group::internal_format;
using gl::group::sized_internal_format;

namespace values {
} // namespace values
} // namespace gl::ext::texture_compression_s3tc
#endif // GL_EXT_texture_compression_s3tc
namespace gl::ext::texture_compression_s3tc {
constexpr auto name = "GL_EXT_texture_compression_s3tc";
}
