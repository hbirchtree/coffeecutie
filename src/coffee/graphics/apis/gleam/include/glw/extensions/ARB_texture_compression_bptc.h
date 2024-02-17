#pragma once

#ifdef GL_ARB_texture_compression_bptc
#include "../enums/InternalFormat.h"
#include "../enums/SizedInternalFormat.h"

namespace gl::arb::texture_compression_bptc {
using gl::group::internal_format;
using gl::group::sized_internal_format;

namespace values {
} // namespace values
} // namespace gl::arb::texture_compression_bptc
#endif // GL_ARB_texture_compression_bptc
namespace gl::arb::texture_compression_bptc {
constexpr auto name = "GL_ARB_texture_compression_bptc";
}
