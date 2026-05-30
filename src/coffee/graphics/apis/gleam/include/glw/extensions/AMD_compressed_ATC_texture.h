#pragma once

#ifdef GL_AMD_compressed_ATC_texture
#include "../enums/InternalFormat.h"
#include "../enums/SizedInternalFormat.h"

namespace gl::amd::compressed_atc_texture {
using gl::group::internal_format;
using gl::group::sized_internal_format;

namespace values {
} // namespace values
} // namespace gl::amd::compressed_atc_texture
#endif // GL_AMD_compressed_ATC_texture
namespace gl::amd::compressed_atc_texture {
constexpr auto name = "GL_AMD_compressed_ATC_texture";
}
