#pragma once

#ifdef GL_EXT_packed_float
#include "../enums/InternalFormat.h"
#include "../enums/SizedInternalFormat.h"
namespace gl::ext::packed_float {
using gl::group::internal_format;
using gl::group::sized_internal_format;
namespace values {
constexpr libc_types::u32 unsigned_int_10f_11f_11f_rev = 0x8C3B;
constexpr libc_types::u32 rgba_signed_components       = 0x8C3C;
} // namespace values
} // namespace gl::ext::packed_float
#endif // GL_EXT_packed_float
namespace gl::ext::packed_float {
constexpr auto name = "GL_EXT_packed_float";
}
