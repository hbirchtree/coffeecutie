#pragma once

#ifdef GL_KHR_texture_compression_astc_hdr
#include "../enums/InternalFormat.h"
#include "../enums/SizedInternalFormat.h"
namespace gl::khr::texture_compression_astc_hdr {
using gl::group::internal_format;
using gl::group::sized_internal_format;
namespace values {
} // namespace values
} // namespace gl::khr::texture_compression_astc_hdr
#endif // GL_KHR_texture_compression_astc_hdr
namespace gl::khr::texture_compression_astc_hdr {
constexpr auto name = "GL_KHR_texture_compression_astc_hdr";
}
