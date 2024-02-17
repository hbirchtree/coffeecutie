#pragma once

#ifdef GL_IMG_texture_compression_pvrtc
#include "../enums/InternalFormat.h"
#include "../enums/SizedInternalFormat.h"

namespace gl::img::texture_compression_pvrtc {
using gl::group::internal_format;
using gl::group::sized_internal_format;

namespace values {
} // namespace values
} // namespace gl::img::texture_compression_pvrtc
#endif // GL_IMG_texture_compression_pvrtc
namespace gl::img::texture_compression_pvrtc {
constexpr auto name = "GL_IMG_texture_compression_pvrtc";
}
