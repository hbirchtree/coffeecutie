#pragma once

#ifdef GL_EXT_texture_snorm
#include "../enums/InternalFormat.h"
#include "../enums/SizedInternalFormat.h"
namespace gl::ext::texture_snorm {
using gl::group::internal_format;
using gl::group::sized_internal_format;
namespace values {
constexpr libc_types::u32 red_snorm                 = 0x8F90;
constexpr libc_types::u32 rg_snorm                  = 0x8F91;
constexpr libc_types::u32 rgb_snorm                 = 0x8F92;
constexpr libc_types::u32 rgba_snorm                = 0x8F93;
constexpr libc_types::u32 signed_normalized         = 0x8F9C;
constexpr libc_types::u32 alpha_snorm               = 0x9010;
constexpr libc_types::u32 luminance_snorm           = 0x9011;
constexpr libc_types::u32 luminance_alpha_snorm     = 0x9012;
constexpr libc_types::u32 intensity_snorm           = 0x9013;
constexpr libc_types::u32 alpha8_snorm              = 0x9014;
constexpr libc_types::u32 luminance8_snorm          = 0x9015;
constexpr libc_types::u32 luminance8_alpha8_snorm   = 0x9016;
constexpr libc_types::u32 intensity8_snorm          = 0x9017;
constexpr libc_types::u32 alpha16_snorm             = 0x9018;
constexpr libc_types::u32 luminance16_snorm         = 0x9019;
constexpr libc_types::u32 luminance16_alpha16_snorm = 0x901A;
constexpr libc_types::u32 intensity16_snorm         = 0x901B;
} // namespace values
} // namespace gl::ext::texture_snorm
#endif // GL_EXT_texture_snorm
namespace gl::ext::texture_snorm {
constexpr auto name = "GL_EXT_texture_snorm";
}