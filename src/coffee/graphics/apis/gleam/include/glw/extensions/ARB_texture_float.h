#pragma once

#ifdef GL_ARB_texture_float
#include "../enums/InternalFormat.h"
#include "../enums/SizedInternalFormat.h"

namespace gl::arb::texture_float {
using gl::group::internal_format;
using gl::group::sized_internal_format;

namespace values {
constexpr u32 alpha32f               = 0x8816;
constexpr u32 intensity32f           = 0x8817;
constexpr u32 luminance32f           = 0x8818;
constexpr u32 luminance_alpha32f     = 0x8819;
constexpr u32 alpha16f               = 0x881C;
constexpr u32 intensity16f           = 0x881D;
constexpr u32 luminance16f           = 0x881E;
constexpr u32 luminance_alpha16f     = 0x881F;
constexpr u32 texture_red_type       = 0x8C10;
constexpr u32 texture_green_type     = 0x8C11;
constexpr u32 texture_blue_type      = 0x8C12;
constexpr u32 texture_alpha_type     = 0x8C13;
constexpr u32 texture_luminance_type = 0x8C14;
constexpr u32 texture_intensity_type = 0x8C15;
constexpr u32 texture_depth_type     = 0x8C16;
constexpr u32 unsigned_normalized    = 0x8C17;
} // namespace values
} // namespace gl::arb::texture_float
#endif // GL_ARB_texture_float
namespace gl::arb::texture_float {
constexpr auto name = "GL_ARB_texture_float";
}
