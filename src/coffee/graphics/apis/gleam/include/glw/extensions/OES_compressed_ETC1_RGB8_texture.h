#pragma once

#ifdef GL_OES_compressed_ETC1_RGB8_texture
#include "../enums/InternalFormat.h"
#include "../enums/SizedInternalFormat.h"

namespace gl::oes::compressed_etc1_rgb8_texture {
using gl::group::internal_format;
using gl::group::sized_internal_format;

namespace values {
} // namespace values
} // namespace gl::oes::compressed_etc1_rgb8_texture
#endif // GL_OES_compressed_ETC1_RGB8_texture
namespace gl::oes::compressed_etc1_rgb8_texture {
constexpr auto name = "GL_OES_compressed_ETC1_RGB8_texture";
}
