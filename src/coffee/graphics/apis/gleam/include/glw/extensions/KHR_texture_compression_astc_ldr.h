#pragma once

#ifdef GL_KHR_texture_compression_astc_ldr
#include "../enums/InternalFormat.h"
#include "../enums/SizedInternalFormat.h"
namespace gl::khr::texture_compression_astc_ldr {
using gl::group::internal_format;
using gl::group::sized_internal_format;
namespace values {
} // namespace values
} // namespace gl::khr::texture_compression_astc_ldr
#endif // GL_KHR_texture_compression_astc_ldr
namespace gl::khr::texture_compression_astc_ldr {
constexpr auto name = "GL_KHR_texture_compression_astc_ldr";
}
