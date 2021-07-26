#pragma once

#ifdef GL_EXT_texture_compression_bptc
#include "../enums/InternalFormat.h"
#include "../enums/SizedInternalFormat.h"
namespace gl::ext::texture_compression_bptc {
using gl::group::internal_format;
using gl::group::sized_internal_format;
namespace values {
} // namespace values
} // namespace gl::ext::texture_compression_bptc
#endif // GL_EXT_texture_compression_bptc
namespace gl::ext::texture_compression_bptc {
constexpr auto name = "GL_EXT_texture_compression_bptc";
}
