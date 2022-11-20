#pragma once

#ifdef GL_EXT_texture_shared_exponent
#include "../enums/InternalFormat.h"
#include "../enums/PixelType.h"
#include "../enums/SizedInternalFormat.h"
namespace gl::ext::texture_shared_exponent {
using gl::group::internal_format;
using gl::group::pixel_type;
using gl::group::sized_internal_format;
namespace values {
constexpr libc_types::u32 texture_shared_size = 0x8C3F;
} // namespace values
} // namespace gl::ext::texture_shared_exponent
#endif // GL_EXT_texture_shared_exponent
namespace gl::ext::texture_shared_exponent {
constexpr auto name = "GL_EXT_texture_shared_exponent";
}
