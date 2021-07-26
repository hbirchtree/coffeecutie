#pragma once

#ifdef GL_EXT_texture_compression_s3tc_srgb
#include "../enums/InternalFormat.h"
#include "../enums/SizedInternalFormat.h"
namespace gl::ext::texture_compression_s3tc_srgb {
using gl::group::internal_format;
using gl::group::sized_internal_format;
namespace values {
} // namespace values
} // namespace gl::ext::texture_compression_s3tc_srgb
#endif // GL_EXT_texture_compression_s3tc_srgb
namespace gl::ext::texture_compression_s3tc_srgb {
constexpr auto name = "GL_EXT_texture_compression_s3tc_srgb";
}
