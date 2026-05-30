#pragma once

#ifdef GL_EXT_texture_sRGB
#include "../enums/InternalFormat.h"
#include "../enums/SizedInternalFormat.h"

namespace gl::ext::texture_srgb {
using gl::group::internal_format;
using gl::group::sized_internal_format;

namespace values {
#if defined(GL_VERSION_1_0)
constexpr u32 sluminance_alpha = 0x8C44;
#endif
#if defined(GL_VERSION_1_0)
constexpr u32 sluminance8_alpha8 = 0x8C45;
#endif
#if defined(GL_VERSION_1_0)
constexpr u32 sluminance = 0x8C46;
#endif
#if defined(GL_VERSION_1_0)
constexpr u32 sluminance8 = 0x8C47;
#endif
constexpr u32 compressed_srgb       = 0x8C48;
constexpr u32 compressed_srgb_alpha = 0x8C49;
#if defined(GL_VERSION_1_0)
constexpr u32 compressed_sluminance = 0x8C4A;
#endif
#if defined(GL_VERSION_1_0)
constexpr u32 compressed_sluminance_alpha = 0x8C4B;
#endif
} // namespace values
} // namespace gl::ext::texture_srgb
#endif // GL_EXT_texture_sRGB
namespace gl::ext::texture_srgb {
constexpr auto name = "GL_EXT_texture_sRGB";
}
