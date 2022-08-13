#pragma once

#ifdef GL_ARB_depth_buffer_float
#include "../enums/InternalFormat.h"
#include "../enums/SizedInternalFormat.h"
namespace gl::arb::depth_buffer_float {
using gl::group::internal_format;
using gl::group::sized_internal_format;
namespace values {
constexpr libc_types::u32 float_32_unsigned_int_24_8_rev = 0x8DAD;
} // namespace values
} // namespace gl::arb::depth_buffer_float
#endif // GL_ARB_depth_buffer_float
namespace gl::arb::depth_buffer_float {
constexpr auto name = "GL_ARB_depth_buffer_float";
}