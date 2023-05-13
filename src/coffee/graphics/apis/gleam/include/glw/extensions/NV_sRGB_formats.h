#pragma once

#ifdef GL_NV_sRGB_formats
#include "../enums/InternalFormat.h"
#include "../enums/SizedInternalFormat.h"
namespace gl::nv::srgb_formats {
using gl::group::internal_format;
using gl::group::sized_internal_format;
namespace values {
constexpr u32 etc1_srgb8         = 0x88EE;
constexpr u32 sluminance_alpha   = 0x8C44;
constexpr u32 sluminance8_alpha8 = 0x8C45;
constexpr u32 sluminance         = 0x8C46;
constexpr u32 sluminance8        = 0x8C47;
} // namespace values
} // namespace gl::nv::srgb_formats
#endif // GL_NV_sRGB_formats
namespace gl::nv::srgb_formats {
constexpr auto name = "GL_NV_sRGB_formats";
}
