#pragma once

#ifdef GL_IMG_texture_compression_pvrtc2
#include "../enums/InternalFormat.h"
#include "../enums/SizedInternalFormat.h"

namespace gl::img::texture_compression_pvrtc2 {
using gl::group::internal_format;
using gl::group::sized_internal_format;

namespace values {
} // namespace values
} // namespace gl::img::texture_compression_pvrtc2
#endif // GL_IMG_texture_compression_pvrtc2
namespace gl::img::texture_compression_pvrtc2 {
constexpr auto name = "GL_IMG_texture_compression_pvrtc2";
}
