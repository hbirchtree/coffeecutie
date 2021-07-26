#pragma once

#ifdef GL_EXT_texture_compression_dxt1
#include "../enums/InternalFormat.h"
#include "../enums/SizedInternalFormat.h"
namespace gl::ext::texture_compression_dxt1 {
using gl::group::internal_format;
using gl::group::sized_internal_format;
namespace values {
} // namespace values
} // namespace gl::ext::texture_compression_dxt1
#endif // GL_EXT_texture_compression_dxt1
namespace gl::ext::texture_compression_dxt1 {
constexpr auto name = "GL_EXT_texture_compression_dxt1";
}
