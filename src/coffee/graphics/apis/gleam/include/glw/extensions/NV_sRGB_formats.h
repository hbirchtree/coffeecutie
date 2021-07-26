#pragma once

#ifdef GL_NV_sRGB_formats
#include "../enums/InternalFormat.h"
#include "../enums/SizedInternalFormat.h"
namespace gl::nv::srgb_formats {
using gl::group::internal_format;
using gl::group::sized_internal_format;
namespace values {
constexpr libc_types::u32 etc1_srgb8         = 0x88EE;
constexpr libc_types::u32 sluminance_alpha   = 0x8C44;
constexpr libc_types::u32 sluminance8_alpha8 = 0x8C45;
constexpr libc_types::u32 sluminance         = 0x8C46;
constexpr libc_types::u32 sluminance8        = 0x8C47;
} // namespace values
} // namespace gl::nv::srgb_formats
#endif // GL_NV_sRGB_formats
namespace gl::nv::srgb_formats {
constexpr auto name = "GL_NV_sRGB_formats";
}
