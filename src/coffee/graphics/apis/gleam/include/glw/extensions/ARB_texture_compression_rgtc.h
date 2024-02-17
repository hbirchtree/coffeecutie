#pragma once

#ifdef GL_ARB_texture_compression_rgtc
#include "../enums/InternalFormat.h"
#include "../enums/SizedInternalFormat.h"

namespace gl::arb::texture_compression_rgtc {
using gl::group::internal_format;
using gl::group::sized_internal_format;

namespace values {
} // namespace values
} // namespace gl::arb::texture_compression_rgtc
#endif // GL_ARB_texture_compression_rgtc
namespace gl::arb::texture_compression_rgtc {
constexpr auto name = "GL_ARB_texture_compression_rgtc";
}
